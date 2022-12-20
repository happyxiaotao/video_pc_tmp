#include "mainform.h"
#include "ui_mainform.h"

MainForm::MainForm(const QString& user)
    : QWidget()
    , ui(new Ui::MainForm)
    , m_user(user)
{
    ui->setupUi(this);

    ui->widget_main_header->SetUserName(m_user);

    connect(ui->widget_main_header, &ClientHeader::sig_quit_account, this, &MainForm::slot_quit_account);
}

MainForm::~MainForm()
{
    disconnect(ui->widget_main_header, &ClientHeader::sig_quit_account, this, &MainForm::slot_quit_account);
    delete ui;
}

void MainForm::slot_quit_account()
{
    emit sig_quit_account();
}
