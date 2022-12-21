#include "realvideobodyviews.h"
#include <QDebug>
#include <QKeyEvent>

RealVideoBodyViews::RealVideoBodyViews(QWidget* parent)
    : QWidget { parent }
    , m_bIsFullScreen(false)
{
    // 监听按键事件
    this->setFocusPolicy(Qt::StrongFocus); // 支持子控件捕捉按键事件
}

void RealVideoBodyViews::keyPressEvent(QKeyEvent* event)
{
    // this->IsFullScrenn 有时候并不准确，自己通过m_bIsFullScreen进行处理
    if (m_bIsFullScreen && (event->key() == Qt::Key_Escape)) {
        FullOrNormalScreen();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void RealVideoBodyViews::FullOrNormalScreen()
{
    if (m_bIsFullScreen) {
        this->setWindowFlags(Qt::SubWindow);
        this->showNormal();
        m_bIsFullScreen = false;
    } else {
        this->setWindowFlags(Qt::Window);
        this->showFullScreen();
        m_bIsFullScreen = true;
    }
}
