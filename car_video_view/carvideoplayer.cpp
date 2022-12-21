#include "carvideoplayer.h"
#include "../Common/ini_config.h"
#include "ui_carvideoplayer.h"
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMessageBox>

int CarVideoPlayer::s_inst_id = 0;
QImage CarVideoPlayer::s_default_background_img;
QImage CarVideoPlayer::s_default_no_data_img;
QString CarVideoPlayer::s_resource_dir = QDir::currentPath() + "/resource";
QString CarVideoPlayer::s_save_img_dir = QDir::currentPath() + "/save_img";
QString CarVideoPlayer::s_text_system_title;
QString CarVideoPlayer::s_text_save_img_succ;
QString CarVideoPlayer::s_pc_server_ip;
uint16_t CarVideoPlayer::s_pc_server_port = -1;

CarVideoPlayer::CarVideoPlayer(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CarVideoPlayer)
    , m_inst_id(++s_inst_id)
    , m_car_video_client(nullptr)
    , m_car_video_thread(nullptr)
    , m_bSendRequested(false)
    , m_bConnected(false)
    , m_cur_view_type(WinViewType::kFullScreen)
    , m_img_width(0)
    , m_img_height(0)
    , m_bPlay(true)
    , m_bIsFullScreen(false)
{
    ui->setupUi(this);

    ui->label_channel_alias->hide();

    // 设置QLabel的属性sizePolicy为Ignored，来保证图片即可以放大也可以缩小
    //  QSizePolicy policy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    // ui->label->setSizePolicy(policy);
    // 父组件，real_video_body已经设置了QSizePolicy::Ignored，不再需要这里进行设置了。

    m_car_video_thread = new QThread();

    connect(m_car_video_thread, &QThread::finished, m_car_video_thread, &QThread::deleteLater); // 绑定deleteLater，线程退出时，自动释放资源

    // m_car_video_client = new CarVideoClient(this); // 必须指定parent为this，否则自己去释放，总是会报错，即使指定deleteLater()也不会主动调用进行释放。
    //但是会报警告。cannot move objects with a parent
    m_car_video_client = new CarVideoClient();
    connect(this, &CarVideoPlayer::sig_connect, m_car_video_client, &CarVideoClient::slot_connect);
    connect(this, &CarVideoPlayer::sig_disconnect, m_car_video_client, &CarVideoClient::slot_disconnect);
    connect(this, &CarVideoPlayer::sig_release_client, m_car_video_client, &CarVideoClient::slot_release);
    connect(m_car_video_client, &CarVideoClient::sig_connected, this, &CarVideoPlayer::slot_car_video_client_connected);
    connect(m_car_video_client, &CarVideoClient::sig_disconnected, this, &CarVideoPlayer::slot_car_video_client_disconnected);
    connect(m_car_video_client, &CarVideoClient::sig_update_image, this, &CarVideoPlayer::slot_car_video_client_update_image);
    connect(this, &CarVideoPlayer::sig_play_audio, m_car_video_client, &CarVideoClient::slot_start_audio);
    connect(this, &CarVideoPlayer::sig_stop_audio, m_car_video_client, &CarVideoClient::slot_stop_audio);
    m_car_video_client->moveToThread(m_car_video_thread);
    m_car_video_thread->start(QThread::Priority::LowPriority);

    // 加载默认背景图片
    qDebug() << __FUNCTION__ << ",inst_id:" << m_inst_id << "\n";
    if (s_default_background_img.isNull()) {
        QString video_bg_path = s_resource_dir + QString("/real_video/video-bg.jpg");
        s_default_background_img.load(video_bg_path);

        QString video_no_data_path = s_resource_dir + QString("/real_video/video-no_data.png");
        s_default_no_data_img.load(video_no_data_path);

        s_text_system_title = QString::fromStdString(g_ini->Get("text", "system_title", ""));
        s_text_save_img_succ = QString::fromStdString(g_ini->Get("text", "text_save_img_succ", ""));

        s_pc_server_ip = QString::fromStdString(g_ini->Get("pc_server", "ip", ""));
        s_pc_server_port = g_ini->GetInteger("pc_server", "port", -1);

        QFileInfo info(s_save_img_dir);
        QDir dir(s_save_img_dir);
        if (!dir.exists()) {
            dir.mkdir(dir.path());
        }
    }

    m_img_width = ui->label->size().width();
    m_img_height = ui->label->size().height();

    ui->pushButton_close->setIcon(QIcon(s_resource_dir + QString("/video_player/close.png")));
    ui->pushButton_close->setIconSize(ui->pushButton_close->size() * 2 / 3);

    ui->pushButton_camera->setIcon(QIcon(s_resource_dir + QString("/video_player/camera.png")));
    ui->pushButton_camera->setIconSize(ui->pushButton_camera->size() * 2 / 3);

    ui->pushButton_play_stop->setIcon(QIcon(s_resource_dir + QString("/video_player/play.png")));
    ui->pushButton_play_stop->setIconSize(ui->pushButton_play_stop->size() * 2 / 3);

    ui->pushButton_fullscreen->setIcon(QIcon(s_resource_dir + QString("/video_player/fullscreen.png")));
    ui->pushButton_fullscreen->setIconSize(ui->pushButton_fullscreen->size() * 2 / 3);

    ui->widget_header->hide();
    ui->widget_footer->hide();
}

CarVideoPlayer::~CarVideoPlayer()
{
    disconnect(this, &CarVideoPlayer::sig_play_audio, m_car_video_client, &CarVideoClient::slot_start_audio);
    disconnect(this, &CarVideoPlayer::sig_stop_audio, m_car_video_client, &CarVideoClient::slot_stop_audio);
    emit sig_release_client();

    delete ui;
}

void CarVideoPlayer::slot_car_video_client_disconnected()
{
    qDebug() << __FUNCTION__ << "\n";
    m_bConnected = false;
}

void CarVideoPlayer::slot_car_video_client_connected()
{
    qDebug() << __FUNCTION__ << "\n";
    m_bConnected = true;
}

void CarVideoPlayer::slot_car_video_client_update_image(QImage* img)
{
    // qDebug() << __FUNCTION__ << "thread_id:" << QThread::currentThreadId() << ",width=" << img->width() << "\n";
    // qDebug() << __FUNCTION__ << "img.size():" << img->size() << ",ui->label->size():" << ui->label->size() << "\n";

    //     if (img->size() == ui->label->size()) {
    //         ui->label->setPixmap(QPixmap::fromImage(*img));
    //     } else {
    //         QImage new_img = img->scaled(ui->label->size());
    //         ui->label->setPixmap(QPixmap::fromImage(new_img));
    //     }
    //

    if (m_bPlay) {
        QImage new_img = img->scaled(m_img_width, m_img_height);
        ui->label->setPixmap(QPixmap::fromImage(new_img));
    }

    delete img;
}

void CarVideoPlayer::AutoSetWidthHeight()
{
    // kFullScreen, //满屏播放
    // k2To1, // 2:1
    // k16To9, // 16:9
    // k4To3,
    // k1To1,
    // k3To4,
    // k1To2,
    // k9To16,
    auto size = ui->label->size();
    switch (m_cur_view_type) {
    case WinViewType::kFullScreen:
        m_img_width = size.width();
        m_img_height = size.height();
        break;
    case WinViewType::k2To1:
        m_img_width = size.width();
        m_img_height = std::min<int>(size.width() / 2, size.height());
        break;
    case WinViewType::k16To9:
        m_img_width = size.width();
        m_img_height = std::min<int>(size.width() / 16 * 9, size.height());
        break;
    case WinViewType::k4To3:
        m_img_width = size.width();
        m_img_height = std::min<int>(size.width() / 4 * 3, size.height());
        break;
    case WinViewType::k1To1:
        m_img_width = m_img_height = std::min<int>(size.width(), size.height());
        break;
    case WinViewType::k3To4:
        m_img_width = std::min<int>(size.height() / 4 * 3, size.width());
        m_img_height = size.height();
        break;
    case WinViewType::k1To2:
        m_img_width = std::min<int>(size.height() / 2, size.width());
        m_img_height = size.height();
        break;
    case WinViewType::k9To16:
        m_img_width = std::min<int>(size.height() / 16 * 9, size.width());
        m_img_height = size.height();
        break;
    default:
        break;
    }
}

void CarVideoPlayer::FullOrNormalScreen()
{
    if (m_bIsFullScreen) {
        this->setWindowFlags(Qt::SubWindow);
        this->showNormal();
        m_bIsFullScreen = false;
    } else {
        this->setWindowFlags(Qt::Window);
        this->showFullScreen();
        m_bIsFullScreen = true;
    }
}

void CarVideoPlayer::resizeEvent(QResizeEvent* event)
{
    qDebug() << __FUNCTION__ << "inst_id:" << m_inst_id << "\n";
    if (!m_bSendRequested) {
        ShowDefaultBackGround(); // 还未发送连接请求
    } else if (m_bSendRequested && !m_bConnected) {
        ShowCircularBackGround(); // 发送连接请求了，但是还未连成功（即没有图片回调）
    } else {
        // 已经连接成功，则不处理，等待更新图片
        // 当时暂停情况下，需要处理进行缩放减小操作
        if (!m_bPlay) {
            ShowImgFromLabel();
        }
    }

    AutoSetWidthHeight();
}

void CarVideoPlayer::keyPressEvent(QKeyEvent* event)
{
    if (m_bIsFullScreen && (event->key() == Qt::Key_Escape)) {
        FullOrNormalScreen();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void CarVideoPlayer::mouseDoubleClickEvent(QMouseEvent* event)
{
    FullOrNormalScreen();
}

void CarVideoPlayer::open_video(const QString& device_id, const QString& channel_alias)
{
    if (s_pc_server_ip.isEmpty() || s_pc_server_port == (uint16_t)-1) {
        return;
    }

    QHostAddress* host = new QHostAddress(s_pc_server_ip);
    uint16_t* port = new uint16_t(s_pc_server_port);
    QString* id = new QString(device_id);
    emit sig_connect(host, port, id);
    m_device_id = device_id;

    m_bSendRequested = true;

    // 设置通道名称
    if (channel_alias.isEmpty()) {
        ui->label_channel_alias->hide();
    } else {
        ui->label_channel_alias->setText(channel_alias);
        ui->label_channel_alias->setStyleSheet("color:white;");
        ui->label_channel_alias->show();
    }

    // 展示header和footer
    ui->widget_header->show();
    ui->widget_footer->show();
}

void CarVideoPlayer::close_video()
{
    emit sig_disconnect();
    m_device_id.clear();
    ui->label->clear();
}

void CarVideoPlayer::ShowDefaultBackGround()
{
    if (s_default_background_img.size() != ui->label->size()) {
        QImage new_img = s_default_background_img.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->label->setPixmap(QPixmap::fromImage(new_img));
        // s_default_background_img = new_img; // 取消替换，避免出现图片模糊的情况。从低分辨率到高分辨率会出现图片模糊的情况。
    } else {
        ui->label->setPixmap(QPixmap::fromImage(s_default_background_img));
    }
}

void CarVideoPlayer::ShowCircularBackGround()
{
    //  设置背景颜色为黑色
    ui->label->setStyleSheet("QLabel{background-color:rgb(0,0,0);}"); //设置样式表
    // 暂时还未获取数据时的图片
    QImage new_img = s_default_no_data_img.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 图片只展示一般内容
    ui->label->setPixmap(QPixmap::fromImage(new_img));
}

void CarVideoPlayer::ShowImgFromLabel()
{
    QImage img = ui->label->pixmap(Qt::ReturnByValue).toImage();
    ui->label->setPixmap(QPixmap::fromImage(img.scaled(ui->label->size(), Qt::IgnoreAspectRatio)));
}

void CarVideoPlayer::ClearLabel()
{
    ui->label->clear();
}

void CarVideoPlayer::UpdateWidthHeightProp(WinViewType type)
{
    if (m_cur_view_type == type) {
        return;
    }

    m_cur_view_type = type;
    AutoSetWidthHeight();
}

void CarVideoPlayer::on_pushButton_close_clicked()
{
    if (m_device_id.isEmpty()) {
        return;
    }
    // 通知外界，关闭此视频。
    emit sig_close_video_by_pushbutton(new QString(m_device_id));
}

void CarVideoPlayer::on_pushButton_play_stop_clicked()
{
    /// qDebug() << "close size:" << ui->pushButton_close->size() << ",camera size:" << ui->pushButton_camera->size() << ",play size:" << ui->pushButton_play_stop->size() << "\n";

    int width, height;
    width = height = std::max<int>(ui->pushButton_play_stop->width(), ui->pushButton_play_stop->height());
    QSize pushbutton_size(width, height);
    QSize icon_size = pushbutton_size * 2 / 3;

    if (m_bPlay) {
        ui->pushButton_play_stop->setIcon(QIcon(s_resource_dir + QString("/video_player/stop.png")));
        ui->pushButton_play_stop->setIconSize(icon_size);
        ui->pushButton_play_stop->resize(pushbutton_size);

        // 音频停止播放，通过信号发送
        emit sig_stop_audio();
        // 视频停止播放，通过本地变量标识
    } else {
        ui->pushButton_play_stop->setIcon(QIcon(s_resource_dir + QString("/video_player/play.png")));
        ui->pushButton_play_stop->setIconSize(icon_size);
        ui->pushButton_play_stop->resize(pushbutton_size);
        emit sig_play_audio();
    }

    m_bPlay = !m_bPlay;
}

void CarVideoPlayer::on_pushButton_camera_clicked()
{
    // 拍照，将照片保存在默认目录下。
    // 图片名称
    // 图片名称，以通道名+时间戳命名

    QDateTime date = QDateTime::currentDateTime();
    QString format = date.toString("yyyyMMdd_HHmmss_zzz");
    QString filename = s_save_img_dir + "/" + format + ".png";

    qDebug() << __FUNCTION__ << ",filename:" << filename << "\n";
    ui->label->pixmap(Qt::ReturnByValue).toImage().save(filename);

    QString strInfo = s_text_save_img_succ;
    strInfo += "\n";
    strInfo += filename;

    QMessageBox::information(nullptr, s_text_system_title, strInfo);
}

void CarVideoPlayer::on_pushButton_fullscreen_clicked()
{
    FullOrNormalScreen();
}
