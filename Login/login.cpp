#include "login.h"
#include "../Common/configini.h"
#include "ui_login.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QSslConfiguration>

QString Login::g_text_empty_username_or_password = CONFIG->Get("text", "empty_username_or_password");
QString Login::g_text_login_failed_exception_1 = CONFIG->Get("text", "login_failed_exception_1");
QString Login::g_text_login_failed_exception_2 = CONFIG->Get("text", "login_failed_exception_2");
QString Login::g_text_login_failed_exception_3 = CONFIG->Get("text", "login_failed_exception_3");

Login::Login(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    connect(&m_http_client, &HttpClient::sig_handler_msg, this, &Login::slot_http_finished);

    m_login_url = CONFIG->Get("api", "user/login");
}

Login::~Login()
{
    disconnect(&m_http_client);
    delete ui;
}

void Login::async_login(const QString& user, const QString& password)
{
    // 创建http请求
    QNetworkRequest request;
    QUrl url(m_login_url);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");

    QJsonObject obj;
    obj["login_name"] = user;
    obj["password"] = password;
    obj["login_type"] = 1;

    QJsonDocument doc;
    doc.setObject(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    qDebug() << "url:" << url;
    qDebug() << "post data:" << QString(data);

    m_http_client.PostJson(request, data);

    // long sslLibraryBuildVersionNumber()		// 返回编译时SSL(静态库)版本号
    // QString sslLibraryBuildVersionString()	// 返回编译时SSL(静态库)版本字符串
    // long sslLibraryVersionNumber()			// 返回运行时SSL库版本号
    // QString sslLibraryVersionString()		// 返回运行时SSL库版本字符串
    // bool supportsSsl()						// 返回是否支持SSL true支持/false不支持
    // qDebug() << QSslSocket::sslLibraryBuildVersionNumber() << ", "
    //          << QSslSocket::sslLibraryBuildVersionString() << ", "
    //          << QSslSocket::sslLibraryVersionNumber() << ", "
    //          << QSslSocket::sslLibraryVersionString() << ", "
    //          << QSslSocket::supportsSsl();
}

void Login::on_pushButton_login_clicked()
{
    // 获取用户账号
    QString user = ui->LineEdit_user->text();
    // 获取用户密码
    QString password = ui->LineEdit_password->text();

    const QString default_user = "admin";
    const QString default_password = "herx123###";

    user = default_user;
    password = default_password;

    if (user.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, SYS_TITLE, g_text_empty_username_or_password);
        return;
    }

    m_user = user;
    m_password = password;

    async_login(m_user, m_password);
}

void Login::on_pushButton_quit_clicked()
{
    this->close();
}

void Login::slot_http_finished(QByteArray* array)
{
    // qDebug() << "Login::slot_http_finished, array:" << QString(*array);
    // 登录成功应答示例
    // {"code":1,"msg":"登录成功","time":"1665732856","data":{"userinfo":{"id":1,"username":"admin","nickname":"admin","mobile":"13888888888","score":0,"status":"normal","area_id":1,"desc":null,"menu_auth":null,"user_type":3,"token":"868ea8e1-8022-464d-8b00-bffc0ab82070","user_id":1,"createtime":1665732856,"expiretime":1665819256,"expires_in":86400,"area":"襄州区"}}}
    // 失败时code=0,msg是错误信息
    QJsonParseError err_rpt;
    QJsonDocument doc = QJsonDocument::fromJson(*array, &err_rpt); // 字符串格式化为JSON
    if (err_rpt.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, SYS_TITLE, g_text_login_failed_exception_1);
        return;
    }
    auto code = doc["code"];
    auto msg = doc["msg"];

    if (code.isNull() || !code.isDouble()) {
        QMessageBox::warning(this, SYS_TITLE, g_text_login_failed_exception_2);
        return;
    }
    if (code.toInt() != 200) {
        QMessageBox::warning(this, SYS_TITLE, msg.toString());
        return;
    }
    auto data = doc["data"];
    if (data.isNull()) {
        QMessageBox::warning(this, SYS_TITLE, g_text_login_failed_exception_3);
        return;
    }
    // QMessageBox::information(this, SYS_TITLE, msg.toString());
    QJsonValue* json_data = new QJsonValue(data);
    QString* user = new QString(m_user);

    // 更新token
    QString token = data["token"].toString();
    m_http_client.SetToken(token);
    qDebug() << "set token:" << token << "\n";

    emit sig_login_success(user, json_data); // json_data对应的是json中data字段保存的值
    close();
}
