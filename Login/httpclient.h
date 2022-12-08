#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

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

private:
    QNetworkAccessManager* m_manager;
    QNetworkReply* m_reply;
};

#endif // HTTPCLIENT_H
