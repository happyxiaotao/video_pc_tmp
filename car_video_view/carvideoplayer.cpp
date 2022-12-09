#include "carvideoplayer.h"
#include "ui_carvideoplayer.h"
#include <QMessageBox>

CarVideoPlayer::CarVideoPlayer(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CarVideoPlayer)
    , m_car_video_client(nullptr)
    , m_bConnected(false)
{
    ui->setupUi(this);

    m_car_video_client = new CarVideoClient();
    connect(this, &CarVideoPlayer::sig_connect, m_car_video_client, &CarVideoClient::slot_connect);
    connect(this, &CarVideoPlayer::sig_disconnect, m_car_video_client, &CarVideoClient::slot_disconnect);
    connect(m_car_video_client, &CarVideoClient::sig_connected, this, &CarVideoPlayer::slot_car_video_client_connected);
    connect(m_car_video_client, &CarVideoClient::sig_disconnected, this, &CarVideoPlayer::slot_car_video_client_disconnected);
    connect(m_car_video_client, &CarVideoClient::sig_update_image, this, &CarVideoPlayer::slot_car_video_client_update_image);
    m_car_video_client->moveToThread(&m_car_video_thread);
    m_car_video_thread.start(QThread::Priority::LowPriority);
}

CarVideoPlayer::~CarVideoPlayer()
{
    disconnect(this);
    disconnect(m_car_video_client);

    m_car_video_thread.quit();
    m_car_video_thread.wait(5);

    // delete m_car_video_client;
    m_car_video_client->deleteLater();

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
    if (img->size() == ui->label->size()) {
        ui->label->setPixmap(QPixmap::fromImage(*img));
    } else {
        QImage new_img = img->scaled(ui->label->size());
        ui->label->setPixmap(QPixmap::fromImage(new_img));
    }

    delete img;
}

void CarVideoPlayer::open_video(const QString& device_id)
{
    QHostAddress* host = new QHostAddress("61.136.148.230");
    uint16_t* port = new uint16_t(9521);
    QString* id = new QString(device_id);
    emit sig_connect(host, port, id);
    m_device_id = device_id;
}

void CarVideoPlayer::close_video()
{
    emit sig_disconnect();
    m_device_id.clear();
}
