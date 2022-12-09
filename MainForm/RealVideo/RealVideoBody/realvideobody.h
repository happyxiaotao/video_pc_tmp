#ifndef REALVIDEOBODY_H
#define REALVIDEOBODY_H

#include "../../car_video_view/carvideoplayer.h"
#include <QGridLayout>
#include <QWidget>

namespace Ui {
class RealVideoBody;
}

class RealVideoBody : public QWidget {
    Q_OBJECT

public:
    explicit RealVideoBody(QWidget* parent = nullptr);
    ~RealVideoBody();

public slots:
    void slot_open_video(QString* device_id);
    void slot_close_video(QString* device_id);

private:
    Ui::RealVideoBody* ui;

    QGridLayout* m_grid_layout;
    QMap<int, QString> m_indexs;
    QMap<QString, CarVideoPlayer*> m_players;
};

#endif // REALVIDEOBODY_H
