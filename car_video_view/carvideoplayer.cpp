#include "carvideoplayer.h"
#include "ui_carvideoplayer.h"
#include <QMessageBox>

CarVideoPlayer::CarVideoPlayer(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CarVideoPlayer)
    , m_car_video_client(nullptr)
{
    ui->setupUi(this);

    m_car_video_client = new CarVideoClient();
    connect(this, &CarVideoPlayer::sig_connect, m_car_video_client, &CarVideoClient::slot_connect);
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
    m_car_video_client->deleteLater();
    delete ui;
}

void CarVideoPlayer::on_pushButton_clicked()
{
    QString strIp = ui->ipLineEdit->text();
    QString strPort = ui->portLineEdit->text();
    QString strDeviceId = ui->idLineEdit->text();

    //  QString default_ip = ui->ipLineEdit->placeholderText();
    //  QString default_port = ui->portLineEdit->placeholderText();
    //  QString default_device_id = ui->idLineEdit->placeholderText();

    QHostAddress* pIp = new QHostAddress(strIp);
    uint16_t* pPort = new uint16_t(strPort.toUShort());
    QString* pDeviceId = new QString(strDeviceId);

    emit sig_connect(pIp, pPort, pDeviceId);
}

void CarVideoPlayer::slot_car_video_client_disconnected()
{
    qDebug() << __FUNCTION__ << "\n";
}

void CarVideoPlayer::slot_car_video_client_connected()
{
    qDebug() << __FUNCTION__ << "\n";
}

void CarVideoPlayer::slot_car_video_client_update_image(QImage* img)
{
    qDebug() << __FUNCTION__ << "thread_id:" << QThread::currentThreadId() << ",width=" << img->width() << "\n";
    if (img->size() == ui->label->size()) {
        ui->label->setPixmap(QPixmap::fromImage(*img));
    } else {
        QImage new_img = img->scaled(ui->label->size());
        ui->label->setPixmap(QPixmap::fromImage(new_img));
    }

    delete img;
}
