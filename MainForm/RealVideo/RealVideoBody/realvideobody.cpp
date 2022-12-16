#include "realvideobody.h"
#include "ui_realvideobody.h"
#include "videoviewmanager.h"
#include <QDebug>
#include <QDir>
#include <QGridLayout>
QString RealVideoBody::s_resource_dir = QDir::currentPath() + "/resource";
RealVideoBody::RealVideoBody(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::RealVideoBody)
    , m_view_manager(nullptr)

{
    ui->setupUi(this);

    m_view_manager = new VideoViewManager(this, ui->widget_real_video_body);

    QIcon icon;
    icon.addFile(s_resource_dir + QString("/real_video/body_full_screen.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->pushButton_real_video_body_full_screen->setIcon(icon);

    // 默认1个窗口
    WinCountType type = WinCountType::One_One;
    m_view_manager->ChangeWinCount(type);
}

RealVideoBody::~RealVideoBody()
{
    delete ui;
    delete m_view_manager;
}

void RealVideoBody::slot_open_video(QString* _device_id)
{
    qDebug() << __FUNCTION__ << ",device_Id:" << *_device_id << "\n";
    QString device_id;
    device_id.swap(*_device_id);
    delete _device_id;

    CarVideoPlayer* player = m_view_manager->GetPlayer(device_id);
    if (player == nullptr) {
        qDebug() << __FUNCTION__ << ",get player nullptr, device_id:" << device_id << "\n";
        return;
    }

    player->SetDeviceId(device_id);
    player->open_video(device_id);
}

void RealVideoBody::slot_close_video(QString* _device_id)
{
    qDebug() << __FUNCTION__ << ",device_Id:" << *_device_id << "\n";
    QString device_id;
    device_id.swap(*_device_id);
    delete _device_id;

    // 关闭对应连接
    CarVideoPlayer* player = m_view_manager->GetPlayer(device_id);
    if (player == nullptr) {
        qDebug() << "not found player,device_id:" << device_id << "\n";
        return;
    }
    player->close_video();
    m_view_manager->ReleasePlayer(player);
}

void RealVideoBody::on_pushButton_all_close_clicked()
{
    // 关闭所有连接
    m_view_manager->CloseAll();
    emit sig_close_all();
}

void RealVideoBody::on_comboBox_3_currentIndexChanged(int index)
{
    // 0 -- 1*1
    // 1 -- 2*2
    // 2 -- 3*3
    // 4 -- 4*4

    qDebug() << __FUNCTION__ << ",index=" << index << "\n";
    WinCountType type = WinCountType::One_One;
    switch (index) {
    case 1:
        type = WinCountType::Two_Two;
        break;
    case 2:
        type = WinCountType::Three_Three;
        break;
    case 3:
        type = WinCountType::Four_Four;
        break;
    default:
        break;
    }
    m_view_manager->ChangeWinCount(type);
}
