#ifndef REALVIDEOBODYVIEWS_H
#define REALVIDEOBODYVIEWS_H

#include <QWidget>
class RealVideoBodyViews : public QWidget {
    Q_OBJECT
public:
    explicit RealVideoBodyViews(QWidget* parent = nullptr);

public:
    void FullOrNormalScreen();
signals:
private:
    // 监听鼠标点击事件，对应播放框，添加蓝色边框，作为选中颜色
    virtual void mousePressEvent(QMouseEvent* event) override;

    // 监听按键事件，目前监听ESC键
    virtual void keyPressEvent(QKeyEvent* event) override;

private:
    bool m_bIsFullScreen;
};

#endif // REALVIDEOBODYVIEWS_H
