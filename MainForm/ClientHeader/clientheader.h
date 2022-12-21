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

signals:
    void sig_quit_account();

private:
    // 由于clientheader.ui中设置背景红色，不生效，设置到了子组件中。所以需要重写paintEvent来实现
    virtual void paintEvent(QPaintEvent* event) override;

public:
    static QString s_resource_dir;

private slots:

    void on_pushButton_quit_account_clicked();

private:
    Ui::ClientHeader* ui;

    QColor m_color_blue_1; // 标题栏的颜色
    QColor m_color_blue_2; // 按钮选中后的颜色

    QString m_user;
};

#endif // CLIENTHEADER_H
