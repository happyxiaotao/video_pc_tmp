#ifndef REALVIDEOBODY_H
#define REALVIDEOBODY_H

#include "../../car_video_view/carvideoplayer.h"

namespace Ui {
class RealVideoBody;
}
class VideoViewManager;
class RealVideoBody : public QWidget {
    Q_OBJECT

public:
    explicit RealVideoBody(QWidget* parent = nullptr);
    ~RealVideoBody();

signals:
    void sig_close_all();
    void sig_close_video(QString* device_id);
public slots:
    void slot_open_video(QString* _device_id, QString* _channel_alias);
    void slot_close_video(QString* _device_id);
    void slot_close_video_from_view_manager(QString* _device_id);

private:
    void SetCarVideoPlayerSignals(CarVideoPlayer* player);
    void RleaseCarVideoPlayerSignals(CarVideoPlayer* player);

public:
    static QString s_resource_dir;

private slots:
    void on_pushButton_all_close_clicked();

    void on_pushButton_real_video_body_full_screen_clicked();

    void on_comboBox_real_video_wincnt_currentIndexChanged(int index);

private:
    Ui::RealVideoBody* ui;

    VideoViewManager* m_view_manager;
};

#endif // REALVIDEOBODY_H
