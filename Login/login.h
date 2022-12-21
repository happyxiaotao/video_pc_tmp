#ifndef LOGIN_H
#define LOGIN_H

#include "../Common/httpclient.h"
#include <QMainWindow>

namespace Ui {
class Login;
}

class Login : public QMainWindow {
    Q_OBJECT

public:
    explicit Login(QWidget* parent = nullptr);
    ~Login();

signals:
    // 应答的json中data字段数据
    void sig_login_success(QString* user, QJsonValue* data);

private:
    void async_login(const QString& user, const QString& password);

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_quit_clicked();

    void slot_http_finished(QByteArray* array);

private:
    Ui::Login* ui;

    QString m_text_title;
    QString m_text_empty_username_or_password;
    QString m_text_login_failed_exception_1;
    QString m_text_login_failed_exception_2;
    QString m_text_login_failed_exception_3;

    QString m_user;
    QString m_password;
    QString m_login_url;
    HttpClient m_http_client;
};

#endif // LOGIN_H
