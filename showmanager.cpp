#include "showmanager.h"

ShowManager::ShowManager(QObject* parent)
    : QObject { parent }
    , m_login(nullptr)
    , m_car_video_player(nullptr)
{
    m_login = new Login();
    m_car_video_player = new CarVideoPlayer();

    m_show_map = new ShowMap();

    connect(m_login, &Login::sig_login_success, this, &ShowManager::slot_login_success);
}

ShowManager::~ShowManager()
{
    qDebug("%s\n", __FUNCTION__);
    m_login->deleteLater();
    m_car_video_player->deleteLater();

    delete m_show_map;
}

void ShowManager::slot_login_success(QString* user, QJsonValue* data)
{
    m_login->hide();
    m_car_video_player->show();
}

void ShowManager::Start()
{
    // m_login->show();

    m_show_map->show();
}
