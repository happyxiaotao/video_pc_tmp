#include "usercache.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>

UserCache::UserCache()
{
}

void UserCache::LoadFile(const QString& filename)
{
    m_filename = filename;

    QFile file(filename);
    if (!file.exists()) {
        return;
    }

    if (!file.open(QFile::ReadOnly)) {
        return;
    }

    QByteArray data = file.readAll();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        return;
    }

    m_doc = doc;

    QJsonObject obj = m_doc.object();
    m_last_account = obj["last_account"].toString();
}

void UserCache::UpdateLastAccount(const QString& account)
{
    QJsonObject obj = m_doc.object();
    obj["last_account"] = account;
    m_doc.setObject(obj);

    AutoSave();
}

QString UserCache::GetLastAccount()
{
    return m_last_account;
}

void UserCache::AutoSave()
{
    QFileInfo fileinfo(m_filename);

    // 目录不存在则创建
    QDir dir = fileinfo.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            return;
        }
    }

    // 文件不存在则创建
    QFile file(m_filename);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return;
    }

    QByteArray data = m_doc.toJson();
    file.write(data);
    file.close();
}
