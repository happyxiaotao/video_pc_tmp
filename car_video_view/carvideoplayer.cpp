#include "carvideoplayer.h"
#include "ui_carvideoplayer.h"
#include <QFileInfo>
#include <QMessageBox>

QString CarVideoPlayer::s_resource_dir = CONFIG->Get("resource", "dir", "");
QImage CarVideoPlayer::s_default_background_img;

CarVideoPlayer::CarVideoPlayer(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CarVideoPlayer)
    , m_car_video_client(nullptr)
    , m_car_video_thread(nullptr)
    , m_bConnected(false)
{
    ui->setupUi(this);

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
    qDebug() << __FUNCTION__ << "\n";
    if (s_default_background_img.isNull()) {
        QString video_bg_path = s_resource_dir + QString("/real_video/video-bg.jpg");
        s_default_background_img.load(video_bg_path);
    }
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
    // qDebug() << __FUNCTION__ << ",ui->label->size():" << ui->label->size() << "\n";
    if (img->size() == ui->label->size()) {
        ui->label->setPixmap(QPixmap::fromImage(*img));
    } else {
        QImage new_img = img->scaled(ui->label->size());
        ui->label->setPixmap(QPixmap::fromImage(new_img));
    }

    delete img;
}

void CarVideoPlayer::resizeEvent(QResizeEvent* event)
{
    if (s_default_background_img.size() != ui->label->size()) {
        QImage new_img = s_default_background_img.scaled(ui->label->size());
        ui->label->setPixmap(QPixmap::fromImage(new_img));
        // s_default_background_img = new_img; // 取消替换，避免出现图片模糊的情况
    } else {
        ui->label->setPixmap(QPixmap::fromImage(s_default_background_img));
    }

    // 加载css
    ShowCircularBackGround();
}

void CarVideoPlayer::open_video(const QString& device_id)
{
    // QHostAddress* host = new QHostAddress("61.136.148.230");
    QHostAddress* host = new QHostAddress("127.0.0.1");
    uint16_t* port = new uint16_t(9521);
    QString* id = new QString(device_id);
    emit sig_connect(host, port, id);
    m_device_id = device_id;
}

void CarVideoPlayer::close_video()
{
    emit sig_disconnect();
    m_device_id.clear();
    ui->label->clear();
}

void CarVideoPlayer::ShowDefaultBackGround(const QSize& size)
{
    resizeEvent(nullptr);
}

void CarVideoPlayer::ShowCircularBackGround()
{
}
