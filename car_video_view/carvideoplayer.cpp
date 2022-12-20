#include "carvideoplayer.h"
#include "../Common/ini_config.h"
#include "ui_carvideoplayer.h"
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

int CarVideoPlayer::s_inst_id = 0;
QImage CarVideoPlayer::s_default_background_img;
QImage CarVideoPlayer::s_default_no_data_img;
QString CarVideoPlayer::s_resource_dir = QDir::currentPath() + "/resource";
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
    m_car_video_client->moveToThread(m_car_video_thread);
    m_car_video_thread->start(QThread::Priority::LowPriority);

    // 加载默认背景图片
    qDebug() << __FUNCTION__ << ",inst_id:" << m_inst_id << "\n";
    if (s_default_background_img.isNull()) {
        QString video_bg_path = s_resource_dir + QString("/real_video/video-bg.jpg");
        s_default_background_img.load(video_bg_path);

        QString video_no_data_path = s_resource_dir + QString("/real_video/video-no_data.png");
        s_default_no_data_img.load(video_no_data_path);

        s_pc_server_ip = QString::fromStdString(g_ini->Get("pc_server", "ip", ""));
        s_pc_server_port = g_ini->GetInteger("pc_server", "port", -1);
    }

    m_img_width = ui->label->size().width();
    m_img_height = ui->label->size().height();
}

CarVideoPlayer::~CarVideoPlayer()
{
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

    if (img->size() == ui->label->size()) {
        ui->label->setPixmap(QPixmap::fromImage(*img));
    } else {
        QImage new_img = img->scaled(ui->label->size());
        ui->label->setPixmap(QPixmap::fromImage(new_img));
    }

    delete img;
}

void CarVideoPlayer::slot_update_width_height_prop(int prop)
{
    WinViewType type = static_cast<WinViewType>(prop);
    if (m_cur_view_type == type) {
        return;
    }

    m_cur_view_type = type;
    AutoSetWidthHeight();
}

void CarVideoPlayer::AutoSetWidthHeight()
{
    auto size = ui->label->size();
    switch (m_cur_view_type) {
    case WinViewType::kFullScreen:
        m_img_width = size.width();
        m_img_height = size.height();
        break;
    default:
        break;
    }
}

void CarVideoPlayer::resizeEvent(QResizeEvent* event)
{
    // qDebug() << __FUNCTION__ << "inst_id:" << m_inst_id << "\n";
    if (!m_bSendRequested) {
        ShowDefaultBackGround();
    } else if (m_bSendRequested && !m_bConnected) {
        ShowCircularBackGround();
    }
    // 已经连接成功，则不处理，等待更新图片
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
        QImage new_img = s_default_background_img.scaled(ui->label->size());
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
    QImage new_img = s_default_no_data_img.scaled(ui->label->size()); // 图片只展示一般内容
    ui->label->setPixmap(QPixmap::fromImage(new_img));
}

void CarVideoPlayer::ClearLabel()
{
    ui->label->clear();
}

void CarVideoPlayer::on_pushButton_clicked()
{
    if (m_device_id.isEmpty()) {
        return;
    }
    // 通知外界，关闭此视频。
    emit sig_close_video_by_pushbutton(new QString(m_device_id));
}
