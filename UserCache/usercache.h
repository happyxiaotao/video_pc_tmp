#ifndef USERCACHE_H
#define USERCACHE_H

#include <QJsonDocument>
#include <QObject>

class UserCache {
public:
    UserCache();

public:
    void LoadFile(const QString& filename);
    // 更新上次的用户账号
    void UpdateLastAccount(const QString& account);
    QString GetLastAccount();

private:
    void AutoSave();

private:
    QString m_filename;
    QString m_last_account; // 上一次登录的账号

    QJsonDocument m_doc;
};

#endif // USERCACHE_H
