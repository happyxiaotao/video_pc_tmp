#ifndef CARVIDEOPLAYER_H
#define CARVIDEOPLAYER_H

#include "carvideoclient.h"
#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QWidget>

enum class WinViewType : int {
    kNone,
    kFullScreen, //满屏播放
    k2To1, // 2:1
    k16To9, // 16:9
    k4To3,
    k1To1,
    k3To4,
    k1To2,
    k9To16,
};

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
    void open_video(const QString& device_id, const QString& channel_alias);
    void close_video();
    bool IsConnected() { return m_bConnected; }
    void SetDeviceId(const QString& device_id) { m_device_id = device_id; }
    QString GetDeviceId() { return m_device_id; }

    void ShowDefaultBackGround();
    void ShowCircularBackGround();

    void ClearLabel();
signals:
    void sig_connect(QHostAddress* address, uint16_t* port, QString* pDeviceId);
    void sig_disconnect();
    void sig_release_client();

    // 通过点击视频解码的左上角进行关闭
    void sig_close_video_by_pushbutton(QString* device_id);
public slots:
    // 视频连接断开
    void slot_car_video_client_disconnected();
    // 视频连接成功
    void slot_car_video_client_connected();
    // 视频图片更新
    void slot_car_video_client_update_image(QImage* img);

    // 更新图片的宽高比例
    void slot_update_width_height_prop(int prop);

private:
    void AutoSetWidthHeight();

private:
    // 重写resizeEvent函数，自适应调整背景图片的大小。
    virtual void resizeEvent(QResizeEvent* event) override;

public:
    static int s_inst_id;
    static QString s_resource_dir;
    static QImage s_default_background_img;
    static QImage s_default_no_data_img;

private slots:
    void on_pushButton_clicked();

private:
    static QString s_pc_server_ip;
    static uint16_t s_pc_server_port;

private:
    Ui::CarVideoPlayer* ui;
    int m_inst_id;
    CarVideoClient* m_car_video_client; //汽车客户端，会在线程中运行，处理数据收发与数据编解码
    QThread* m_car_video_thread;

    bool m_bSendRequested; //是否发送了请求
    bool m_bConnected;
    QString m_device_id;

    //图像展示的分辨率
    WinViewType m_cur_view_type;
    int m_img_width;
    int m_img_height;
};

#endif // CARVIDEOPLAYER_H
