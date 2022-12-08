#include "showmanager.h"

ShowManager::ShowManager(QObject* parent)
    : QObject { parent }
    , m_login(nullptr)
    , m_car_video_player(nullptr)
{
    m_login = new Login();
    m_car_video_player = new CarVideoPlayer();

    connect(m_login, &Login::sig_login_success, this, &ShowManager::slot_login_success);
}

ShowManager::~ShowManager()
{
    m_login->deleteLater();
    m_car_video_player->deleteLater();
}

void ShowManager::slot_login_success(QString* user, QJsonValue* data)
{
    m_login->hide();
    m_car_video_player->show();
}

void ShowManager::Start()
{
    m_login->show();
}
