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
    // 监听按键事件，目前监听ESC键
    virtual void keyPressEvent(QKeyEvent* event) override;

private:
    bool m_bIsFullScreen;
};

#endif // REALVIDEOBODYVIEWS_H
