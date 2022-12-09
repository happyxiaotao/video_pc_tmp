#include "showmanager.h"

ShowManager::ShowManager(QObject* parent)
    : QObject { parent }
    , m_login(nullptr)
    , m_main_form(nullptr)
{
    m_login = new Login();
    connect(m_login, &Login::sig_login_success, this, &ShowManager::slot_login_success);
}

ShowManager::~ShowManager()
{
    qDebug("%s\n", __FUNCTION__);
    delete m_main_form;
    delete m_login;
}

void ShowManager::slot_login_success(QString* user, QJsonValue* data)
{
    m_login->hide();
    m_main_form = new MainForm();
    m_main_form->show();
}

void ShowManager::Start()
{
    m_login->show();
}
