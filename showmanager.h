#ifndef SHOWMANAGER_H
#define SHOWMANAGER_H

#include "Login/login.h"
#include "MainForm/mainform.h"
#include <QJsonValue>
#include <QString>

class ShowManager : public QObject {
    Q_OBJECT
public:
    explicit ShowManager(QObject* parent = nullptr);
    ~ShowManager();
signals:
private slots:
    void slot_login_success(QString* user, QJsonValue* data);

public:
    void Start();

private:
    Login* m_login;

    MainForm* m_main_form;
};

#endif // SHOWMANAGER_H
