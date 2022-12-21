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
    void ShowImgFromLabel(); //放大缩小的情况下，展示之前QLabel的图片

    void ClearLabel();

    // 更新视频图片的宽高比例
    void UpdateWidthHeightProp(WinViewType type);
signals:
    void sig_connect(QHostAddress* address, uint16_t* port, QString* pDeviceId);
    void sig_disconnect();
    void sig_release_client();

    // 通过点击视频解码的左上角进行关闭
    void sig_close_video_by_pushbutton(QString* device_id);

    // 播放/停止 音频
    void sig_play_audio();
    void sig_stop_audio();
public slots:
    // 视频连接断开
    void slot_car_video_client_disconnected();
    // 视频连接成功
    void slot_car_video_client_connected();
    // 视频图片更新
    void slot_car_video_client_update_image(QImage* img);

private:
    void AutoSetWidthHeight();
    void FullOrNormalScreen();

private:
    // 重写resizeEvent函数，自适应调整背景图片的大小。
    virtual void resizeEvent(QResizeEvent* event) override;

    // 监听按键
    virtual void keyPressEvent(QKeyEvent* event) override;

    // 处理鼠标双击事件
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

public:
    static int s_inst_id;
    static QString s_resource_dir;
    static QImage s_default_background_img;
    static QImage s_default_no_data_img;
    static QString s_save_img_dir;

private slots:

    void on_pushButton_close_clicked();

    void on_pushButton_play_stop_clicked();

    void on_pushButton_camera_clicked();

    void on_pushButton_fullscreen_clicked();

private:
    static QString s_text_system_title;
    static QString s_text_save_img_succ;
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

    bool m_bPlay; //是否播放视频，用户点击暂停时，不能播放

    bool m_bIsFullScreen; //是否全屏
};

#endif // CARVIDEOPLAYER_H
