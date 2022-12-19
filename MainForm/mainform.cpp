#include "mainform.h"
#include "ui_mainform.h"

MainForm::MainForm(const QString& user)
    : QWidget()
    , ui(new Ui::MainForm)
    , m_user(user)
{
    ui->setupUi(this);

    ui->widget_main_header->SetUserName(m_user);
}

MainForm::~MainForm()
{
    delete ui;
}
