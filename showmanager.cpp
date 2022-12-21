#include "showmanager.h"

ShowManager::ShowManager(QWidget* parent)
    : QWidget { parent }
    , m_login(nullptr)
    , m_main_form(nullptr)
{
    m_login = new Login();
    connect(m_login, &Login::sig_login_success, this, &ShowManager::slot_login_success);
}

ShowManager::~ShowManager()
{
    qDebug("%s\n", __FUNCTION__);
    if (m_login) {
        m_login->deleteLater();
    }
    if (m_main_form) {
        disconnect(m_main_form, &MainForm::sig_quit_account, this, &ShowManager::slot_quit_account);
        m_main_form->deleteLater();
    }
}

void ShowManager::slot_login_success(QString* user, QJsonValue* data)
{
    m_login->hide();
    m_main_form = new MainForm(*user);

    connect(m_main_form, &MainForm::sig_quit_account, this, &ShowManager::slot_quit_account);

    m_main_form->show();

    delete user;
    delete data;
}

void ShowManager::slot_quit_account()
{
    if (m_main_form) {
        disconnect(m_main_form, &MainForm::sig_quit_account, this, &ShowManager::slot_quit_account);
        m_main_form->deleteLater();
    }
    m_login->show();
}

void ShowManager::Start()
{
    m_login->show();
}
