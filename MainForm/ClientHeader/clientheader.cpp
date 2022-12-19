#include "clientheader.h"
#include "../../Common/ini_config.h"
#include "ui_clientheader.h"
#include <QDebug>
#include <QDir>
#include <QImage>
#include <QPainter>
#include <QPixmap>

QString ClientHeader::s_resource_dir = QDir::currentPath() + "/resource";

ClientHeader::ClientHeader(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ClientHeader)
{
    ui->setupUi(this);

    // 选中之后的深蓝： QColor(51, 137, 255)
    // 未选中的蓝色：  QColor(32, 91, 229)
    m_color_blue_1 = QColor(51, 137, 255);
    m_color_blue_2 = QColor(32, 91, 229);

    // 设置logo
    QImage img(s_resource_dir + "/client_header/logo.png");
    QImage new_img = img.scaled(ui->label_client_header_logo->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // 第三个参数表示平滑处理，图片缩小时，不失真
    ui->label_client_header_logo->setPixmap(QPixmap::fromImage(new_img));

    // 设置 当前界面
    QIcon icon(s_resource_dir + "/client_header/real_video.png");
    ui->pushButton->setIcon(icon);
    int button_height = ui->pushButton->height();
    int icon_width = button_height * 2 / 3;
    ui->pushButton->setIconSize(QSize(icon_width, icon_width));
    ui->pushButton->setStyleSheet("background-color:rgb(32,91,229);color:rgb(255,255,255);");

    QString user_icon_path = s_resource_dir + "/client_header/user.jpg";
    QImage user_img(user_icon_path);
    QImage user_new_img = user_img.scaled(ui->label_user->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->label_user->setPixmap(QPixmap::fromImage(user_new_img));
}

ClientHeader::~ClientHeader()
{
    delete ui;
}

void ClientHeader::SetUserName(const QString& name)
{
    m_user = name;
    QString welcome = QString::fromStdString(g_ini->Get("text", "text_welcome_prefix", ""));
    welcome += m_user;
    ui->label_text_welcome->setText(welcome);
    ui->label_text_welcome->setStyleSheet("font:12px;color:white;");
}

void ClientHeader::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(m_color_blue_1);
    p.drawRect(rect());
}

void ClientHeader::enterEvent(QEvent* event)
{
    qDebug() << __FUNCTION__ << "\n";

    // 判断鼠标是否在头像上
    if (ui->label_user->geometry().contains(QCursor::pos())) {
        qDebug() << __FUNCTION__ << ", ok\n";
    } else {
        qDebug() << __FUNCTION__ << ", failed\n";
    }
}

void ClientHeader::leaveEvent(QEvent* event)
{
    qDebug() << __FUNCTION__ << "\n";
}
