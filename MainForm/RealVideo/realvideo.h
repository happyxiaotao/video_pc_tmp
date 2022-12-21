#ifndef REALVIDEO_H
#define REALVIDEO_H

#include <QWidget>

namespace Ui {
class RealVideo;
}

class RealVideo : public QWidget {
    Q_OBJECT

public:
    explicit RealVideo(QWidget* parent = nullptr);
    ~RealVideo();

private:
    Ui::RealVideo* ui;
};

#endif // REALVIDEO_H
