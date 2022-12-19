#ifndef CLIENTHEADER_H
#define CLIENTHEADER_H

#include <QWidget>

namespace Ui {
class ClientHeader;
}

class ClientHeader : public QWidget {
    Q_OBJECT

public:
    explicit ClientHeader(QWidget* parent = nullptr);
    ~ClientHeader();

public:
    void SetUserName(const QString& name);

private:
    // 由于clientheader.ui中设置背景红色，不生效，设置到了子组件中。所以需要重写paintEvent来实现
    virtual void paintEvent(QPaintEvent* event) override;

    // 鼠标进入
    virtual void enterEvent(QEvent* event) override;
    // 鼠标离开
    virtual void leaveEvent(QEvent* event) override;

public:
    static QString s_resource_dir;

private slots:

private:
    Ui::ClientHeader* ui;

    QColor m_color_blue_1; // 标题栏的颜色
    QColor m_color_blue_2; // 按钮选中后的颜色

    QString m_user;
};

#endif // CLIENTHEADER_H
