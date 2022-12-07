#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

#include <QImage>
#include <QWidget>

#include "jt1078client.h"

namespace Ui {
class VideoView;
}

class VideoView : public QWidget {
    Q_OBJECT

public:
    explicit VideoView(QWidget* parent = nullptr);
    ~VideoView();

private slots:
    void slot_update_video(QImage* img);

private:
    Ui::VideoView* ui;

    Jt1078Client* m_jt1078_client;
};

#endif // VIDEOVIEW_H
