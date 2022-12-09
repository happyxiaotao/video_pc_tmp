#ifndef CARVIDEOPLAYER_H
#define CARVIDEOPLAYER_H

#include "carvideoclient.h"
#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QWidget>

namespace Ui {
class CarVideoPlayer;
}

class CarVideoPlayer : public QWidget {
    Q_OBJECT

public:
    explicit CarVideoPlayer(QWidget* parent = nullptr);
    ~CarVideoPlayer();

public:
    // 点击按钮，连接服务器
    void open_video(const QString& device_id);
    void close_video();
    bool IsConnected() { return m_bConnected; }
    QString GetDeviceId() { return m_device_id; }
signals:
    void sig_connect(QHostAddress* address, uint16_t* port, QString* pDeviceId);
    void sig_disconnect();
public slots:
    // 视频连接断开
    void slot_car_video_client_disconnected();
    // 视频连接成功
    void slot_car_video_client_connected();
    // 视频图片更新
    void slot_car_video_client_update_image(QImage* img);

private:
    Ui::CarVideoPlayer* ui;
    CarVideoClient* m_car_video_client; //汽车客户端，会在线程中运行，处理数据收发与数据编解码
    QThread m_car_video_thread;
    bool m_bConnected;
    QString m_device_id;
};

#endif // CARVIDEOPLAYER_H
