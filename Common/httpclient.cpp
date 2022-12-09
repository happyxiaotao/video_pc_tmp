#include "httpclient.h"

QMutex HttpClient::s_mutex;
QString HttpClient::s_token;

HttpClient::HttpClient(QObject* parent)
    : QObject { parent }
    , m_reply(nullptr)
    , m_manager(nullptr)
{

    m_manager = new QNetworkAccessManager(this);

    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_finished(QNetworkReply*)));
}

HttpClient::~HttpClient()
{
    delete m_manager;
}

void HttpClient::slot_finished(QNetworkReply* reply)
{
    QByteArray* data = new QByteArray(reply->readAll());
    emit sig_handler_msg(data);
    reply->deleteLater();
}

void HttpClient::Get(QNetworkRequest& request)
{
    m_reply = m_manager->get(request);
}

void HttpClient::Post(QNetworkRequest& request, const QByteArray& data)
{
    QSslConfiguration ssl_config;
    ssl_config.setPeerVerifyMode(QSslSocket::VerifyNone);
    ssl_config.setProtocol(QSsl::AnyProtocol);
    request.setSslConfiguration(ssl_config);
    m_reply = m_manager->post(request, data);
}

void HttpClient::PostJson(QNetworkRequest& request, const QByteArray& data)
{
    // 添加下面的请求
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    Post(request, data);
}

void HttpClient::SetToken(const QString& token)
{
    QMutexLocker locker(&s_mutex);
    s_token = token;
}

QString HttpClient::GetToken()
{
    QMutexLocker locker(&s_mutex);
    return s_token;
}
