#ifndef CARVIDEOPLAYER_H
#define CARVIDEOPLAYER_H

#include "../../Common/configini.h"
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
    void ClearDeviceId() { m_device_id.clear(); }
    void SetDeviceId(const QString& device_id) { m_device_id = device_id; }
    QString GetDeviceId() { return m_device_id; }

    void ShowDefaultBackGround(const QSize& size = QSize());
    void ShowCircularBackGround();
signals:
    void sig_connect(QHostAddress* address, uint16_t* port, QString* pDeviceId);
    void sig_disconnect();
    void sig_release_client();
public slots:
    // 视频连接断开
    void slot_car_video_client_disconnected();
    // 视频连接成功
    void slot_car_video_client_connected();
    // 视频图片更新
    void slot_car_video_client_update_image(QImage* img);

private:
    // 重写resizeEvent函数，自适应调整背景图片的大小。
    virtual void resizeEvent(QResizeEvent* event) override;

public:
    static QString s_resource_dir;
    static QImage s_default_background_img;

private:
    Ui::CarVideoPlayer* ui;
    CarVideoClient* m_car_video_client; //汽车客户端，会在线程中运行，处理数据收发与数据编解码
    QThread* m_car_video_thread;

    bool m_bConnected;
    QString m_device_id;
};

#endif // CARVIDEOPLAYER_H
