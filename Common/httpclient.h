#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class HttpClient : public QObject {
    Q_OBJECT
public:
    explicit HttpClient(QObject* parent = nullptr);
    ~HttpClient();

signals:
    void sig_handler_msg(QByteArray* data);
private slots:
    void slot_finished(QNetworkReply* reply);

public:
    void Get(QNetworkRequest& request);
    void Post(QNetworkRequest& request, const QByteArray& data);
    // 针对data是json格式特殊处理
    void PostJson(QNetworkRequest& request, const QByteArray& data);

public:
    static void SetToken(const QString& token);
    static QString GetToken();

private:
    static QMutex s_mutex;
    static QString s_token;

private:
    QNetworkReply* m_reply;
    QNetworkAccessManager* m_manager;
};

#endif // HTTPCLIENT_H
