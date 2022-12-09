#include "configini.h"

#include <QDir>
// 当前工作目录，用来根据目录，找到对应的安装位置

ConfigIni::ConfigIni()
{
    Init();
}

QString ConfigIni::Get(const QString& section, const QString& key, const QString& default_value)
{
    if (!ExistsSection(section)) {
        return default_value;
    }
    return GetValue(GetSection(section), key, default_value);
}

long long ConfigIni::GetInteger(const QString& section, const QString& key, long long default_value)
{
    if (!ExistsSection(section)) {
        return default_value;
    }
    return GetValue(GetSection(section), key, default_value);
}

double ConfigIni::GetDouble(const QString& section, const QString& key, double default_value)
{
    if (!ExistsSection(section)) {
        return default_value;
    }
    return GetValue(GetSection(section), key, default_value);
}

bool ConfigIni::GetBoolean(const QString& section, const QString& key, bool default_value)
{
    if (!ExistsSection(section)) {
        return default_value;
    }
    return GetValue(GetSection(section), key, default_value);
}

bool ConfigIni::ExistsSection(const QString& section)
{
    return m_ini.find(section) != m_ini.end();
}

const ConfigIni::Section& ConfigIni::GetSection(const QString& section)
{
    // 必须先调用ExistsSection判断是否存在
    return m_ini.find(section).value();
}

QString ConfigIni::GetValue(const ConfigIni::Section& section, const QString& key, const QString& default_value)
{
    auto iter = section.find(key);
    if (iter == section.end()) {
        return default_value;
    }
    return section[key];
}

int ConfigIni::GetValue(const ConfigIni::Section& section, const QString& key, int default_value)
{
    auto iter = section.find(key);
    if (iter == section.end()) {
        return default_value;
    }
    return section[key].toInt();
}

long long ConfigIni::GetValue(const ConfigIni::Section& section, const QString& key, long long default_value)
{
    auto iter = section.find(key);
    if (iter == section.end()) {
        return default_value;
    }
    return section[key].toLongLong();
}

double ConfigIni::GetValue(const ConfigIni::Section& section, const QString& key, double default_value)
{
    auto iter = section.find(key);
    if (iter == section.end()) {
        return default_value;
    }
    return section[key].toDouble();
}

bool ConfigIni::GetValue(const ConfigIni::Section& section, const QString& key, bool default_value)
{
    auto iter = section.find(key);
    if (iter == section.end()) {
        return default_value;
    }
    auto value = section[key];
    if (value == "true" || value == "yes" || value == "on" || value == "1")
        return true;
    else if (value == "false" || value == "no" || value == "off" || value == "0")
        return false;

    return false;
}

void ConfigIni::Init()
{
    Section text {
        { "system_title", QString::fromLocal8Bit("系统提示") },
        { "online", QString::fromLocal8Bit("在线") },
        { "offline", QString::fromLocal8Bit("离线") },
        { "connecting", QString::fromLocal8Bit("连接中") },
        { "invalid_uid", QString::fromLocal8Bit("错误的UID！") },
        { "wrong_password", QString::fromLocal8Bit("密码错误！") },
        { "connect_dev_failed", QString::fromLocal8Bit("设备连接失败！") },
        { "open_video_stream_failed", QString::fromLocal8Bit("打开视频流失败！") },
        { "open_audio_stream_failed", QString::fromLocal8Bit("打开音频流失败！") },
        { "add_dev_failed", QString::fromLocal8Bit("添加设备成功！") },
        { "dev_not_online", QString::fromLocal8Bit("设备不在线！") },
        { "device id:", QString::fromLocal8Bit("device id：") },
        { "get_snapshot_failed", QString::fromLocal8Bit("获取快照失败") },
        { "dev_init_failed", QString::fromLocal8Bit("设备初始化失败！") },
        { "dev_not_exists", QString::fromLocal8Bit("设备不存在") },
        { "win_cnt_more_limit", QString::fromLocal8Bit("窗口数量超过上线") },

        { "device_id_is_empty", QString::fromLocal8Bit("设备id为空") },
        { "device_id_is_exists", QString::fromLocal8Bit("设备id已存在") },
        { "device_name_is_empty", QString::fromLocal8Bit("设备名称为空") },
        { "device_uuid_is_empty", QString::fromLocal8Bit("设备编号为空") },
        { "no_device_yet", QString::fromLocal8Bit("暂无设备") },
        { "device_name_length_more_than_20", QString::fromLocal8Bit("设备名称长度超过20个字符") },
        { "device_uuid_invalid", QString::fromLocal8Bit("设备编号不合理") },
        { "only_support_letter_or_number", QString::fromLocal8Bit("只支持字母或数字") },
        { "delete_device", QString::fromLocal8Bit("删除设备") },
        { "device_uuid_is_exists", QString::fromLocal8Bit("设备已存在！") },

        { "no_extra_win", QString::fromLocal8Bit("没有多余的窗口") },

        { "not_support_this_service", QString::fromLocal8Bit("暂不支持此功能") },

        // 中文特殊字符
        { "chinese_comma", QString::fromLocal8Bit("，") }, //中文逗号
        { "chinese_colon", QString::fromLocal8Bit("：") }, //中文冒号
        { "chinese_exclamatory_mark", QString::fromLocal8Bit("！") }, // 中文感叹号

        // 下面是一些按钮的文本
        { "carema", QString::fromLatin1("拍照") },
        { "start_record", QString::fromLocal8Bit("开始录像") },
        { "stop_record", QString::fromLocal8Bit("停止录像") },
        { "start_talk", QString::fromLocal8Bit("开始通话") },
        { "stop_talk", QString::fromLocal8Bit("停止通话") },
        { "mute", QString::fromLocal8Bit("靜音") },
        { "cancel_mute", QString::fromLocal8Bit("取消靜音") },
        { "fullscreen", QString::fromLocal8Bit("全屏播放") },
        { "cancel_fullscreen", QString::fromLocal8Bit("取消全屏播放") },
        { "move_up", QString::fromLocal8Bit("向上") },
        { "move_down", QString::fromLocal8Bit("向下") },
        { "move_left", QString::fromLocal8Bit("向左") },
        { "move_right", QString::fromLocal8Bit("向右") },

        // 账号登录相关
        { "empty_username_or_password", QString::fromLocal8Bit("用户名或密码为空") },
        { "incorrect_username_or_password", QString::fromLocal8Bit("用户名或密码不正确") },
        { "login_failed_exception_1", QString::fromLocal8Bit("登录失败，数据异常：1") },
        { "login_failed_exception_2", QString::fromLocal8Bit("登录失败，数据异常：2") },
        { "login_failed_exception_3", QString::fromLocal8Bit("登录失败，数据异常：3") },

        { "get_car_real_video_tree_failed", QString::fromLocal8Bit("获取实时视频汽车列表失败") },
    };
    m_ini["text"] = text;

    // 显示相关配置
    Section view {
        { "view_cnt", "1" }, //直播流窗口展示个数
    };
    m_ini["view"] = view;

    // 设备相关配置
    Section camera {
        { "default_password", "admin" }
    };
    m_ini["camera"] = camera;

    // http的api接口
    const QString http_host("https://tg808.gcpgps.com");
    Section server {
        { "http_host", http_host },
    };
    m_ini["server"] = server;

    Section api {
        { "user/login", http_host + "/v1/public/system/UserLogin" },
    };
    m_ini["api"] = api;

    QString currentWorkDir = QDir::currentPath();

    // 下面是UserData相关设置
    Section userdata {
        { "default_json_file", currentWorkDir + "/userdata.json" },
        { "default_media_dir", currentWorkDir + "/media" },
        { "default_resource_dir", currentWorkDir + "/resource" },
        { "default_pg_svr_addr", "connect.peergine.com:7781" },
    };
    m_ini["userdata"] = userdata;
}
