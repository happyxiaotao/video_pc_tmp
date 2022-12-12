#include "realvideobody.h"
#include "ui_realvideobody.h"
#include "videoviewmanager.h"
#include <QDebug>
RealVideoBody::RealVideoBody(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::RealVideoBody)
    , m_view_manager(nullptr)

{
    ui->setupUi(this);
    m_view_manager = new VideoViewManager(ui->gridLayout_videos);
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
    player->ClearDeviceId();
    m_view_manager->ReleasePlayer(player);
}
