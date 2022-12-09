#ifndef CONFIGINI_H
#define CONFIGINI_H

#include "Singleton.hpp"

#include <QMap>
#include <QString>

class ConfigIni : public Singleton<ConfigIni> {

public:
    using Section = QMap<QString, QString>;
    using Ini = QMap<QString, Section>;

    ConfigIni();

public:
    QString Get(const QString& section, const QString& key, const QString& default_value = "");
    long long GetInteger(const QString& section, const QString& key, long long default_value = 0);
    double GetDouble(const QString& section, const QString& key, double default_value = 0.0);
    bool GetBoolean(const QString& section, const QString& key, bool default_value = false);

private:
    bool ExistsSection(const QString& section);
    const Section& GetSection(const QString& section);
    static QString GetValue(const Section& section, const QString& key, const QString& default_value);
    static int GetValue(const Section& section, const QString& key, int default_value);
    static long long GetValue(const Section& section, const QString& key, long long default_value);
    static double GetValue(const Section& section, const QString& key, double default_value);
    static bool GetValue(const Section& section, const QString& key, bool default_value);

private:
    void Init();

private:
    Ini m_ini;
};

// 模板特化
template <>
QMutex Singleton<ConfigIni>::s_mutex;
template <>
ConfigIni* Singleton<ConfigIni>::s_instance = nullptr;

#define CONFIG ConfigIni::instance()

#define SYS_TITLE CONFIG->Get("text", "system_title", "系统提示")

#endif // CONFIGINI_H
