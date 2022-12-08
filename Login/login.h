#ifndef LOGIN_H
#define LOGIN_H

#include "httpclient.h"
#include <QMainWindow>
#include <QNetworkAccessManager>

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

public:
    static QString g_text_empty_username_or_password;
    static QString g_text_login_failed_exception_1;
    static QString g_text_login_failed_exception_2;
    static QString g_text_login_failed_exception_3;

private:
    Ui::Login* ui;

    QNetworkAccessManager* m_manager;

    QString m_user;
    QString m_password;
    QString m_login_url;
    HttpClient m_http_client;
};

#endif // LOGIN_H
