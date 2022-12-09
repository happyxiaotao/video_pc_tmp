#include "realvideobody.h"
#include "ui_realvideobody.h"
#include <QDebug>

RealVideoBody::RealVideoBody(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::RealVideoBody)
    , m_grid_layout(nullptr)
{
    ui->setupUi(this);

    m_grid_layout = new QGridLayout(this);
}

RealVideoBody::~RealVideoBody()
{
    qDeleteAll(m_players);
    delete m_grid_layout;
    delete ui;
}

void RealVideoBody::slot_open_video(QString* device_id)
{
    qDebug() << __FUNCTION__ << ",device_Id:" << *device_id << "\n";
    QString id;
    id.swap(*device_id);
    delete device_id;

    // 查看是否存在了
    if (m_players.find(id) != m_players.end()) {
        return;
    }

    // 不存在， 创建对应的播放组件
    CarVideoPlayer* player = new CarVideoPlayer();
}

void RealVideoBody::slot_close_video(QString* device_id)
{
    qDebug() << __FUNCTION__ << ",device_Id:" << *device_id << "\n";
    QString id;
    id.swap(*device_id);
    delete device_id;
}
