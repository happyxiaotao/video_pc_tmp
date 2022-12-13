#ifndef REALVIDEOBODY_H
#define REALVIDEOBODY_H

#include "../../car_video_view/carvideoplayer.h"

namespace Ui {
class RealVideoBody;
}
class QGridLayout;
class VideoViewManager;
class RealVideoBody : public QWidget {
    Q_OBJECT

public:
    explicit RealVideoBody(QWidget* parent = nullptr);
    ~RealVideoBody();

public slots:
    void slot_open_video(QString* _device_id);
    void slot_close_video(QString* _device_id);

private:
    void SetCarVideoPlayerSignals(CarVideoPlayer* player);
    void RleaseCarVideoPlayerSignals(CarVideoPlayer* player);

public:
    static QString s_resource_dir;

private:
    Ui::RealVideoBody* ui;

    VideoViewManager* m_view_manager;
    QGridLayout* m_body_grid_layout;
};

#endif // REALVIDEOBODY_H
