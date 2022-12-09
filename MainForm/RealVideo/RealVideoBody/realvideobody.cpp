#include "realvideobody.h"
#include "ui_realvideobody.h"
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

    CarVideoPlayer* player = m_view_manager->GetDstPlayer(device_id);
    if (player == nullptr) {
        player = m_view_manager->GetNoUsedPlayer();
    } else {
        if (player->IsConnected()) {
            return;
        }
    }
    if (player == nullptr) {
        return;
    }

    player->open_video(device_id);
}

void RealVideoBody::slot_close_video(QString* _device_id)
{
    qDebug() << __FUNCTION__ << ",device_Id:" << *_device_id << "\n";
    QString device_id;
    device_id.swap(*_device_id);
    delete _device_id;

    // 关闭对应连接
    CarVideoPlayer* player = m_view_manager->GetDstPlayer(device_id);
    if (player == nullptr) {
        qDebug() << "not found player,device_id:" << device_id << "\n";
        return;
    }
    player->close_video();
}

RealVideoBody::VideoViewManager::VideoViewManager(QGridLayout* layout)
    : m_grid_layout(layout)
    , m_row(3)
    , m_col(3)
    , m_capacity(9)
{
    for (int i = 0; i < m_capacity; i++) {
        m_mapUsed.insert(i, false);

        CarVideoPlayer* player = new CarVideoPlayer();
        int row = i / m_col;
        int col = i % m_row;
        m_grid_layout->addWidget(player, row, col);
        m_mapPlayer.insert(i, player);
    }
}

RealVideoBody::VideoViewManager::~VideoViewManager()
{
    for (auto& iter : m_mapPlayer) {
        m_grid_layout->removeWidget(iter);
    }
    qDebug() << __FUNCTION__ << ",grid_layout.widget count:" << m_grid_layout->count() << "\n";

    for (auto& player : m_mapPlayer) {
        delete player;
    }
    m_mapPlayer.clear();
}

int RealVideoBody::VideoViewManager::Size()
{
    int size = 0;
    for (auto& iter : m_mapUsed) {
        if (iter) {
            size++;
        }
    }
    return size;
}

CarVideoPlayer* RealVideoBody::VideoViewManager::GetDstPlayer(const QString& device_id)
{
    for (int i = 0; i < m_capacity; i++) {
        if (m_mapUsed[i] && device_id == m_mapPlayer[i]->GetDeviceId()) {
            return m_mapPlayer[i];
        }
    }
    return nullptr;
}

CarVideoPlayer* RealVideoBody::VideoViewManager::GetNoUsedPlayer()
{
    int index = GetNoUsedIndex();
    if (index == -1) {
        return nullptr;
    }
    return m_mapPlayer[index];
}

void RealVideoBody::VideoViewManager::GetLocation(int& row, int& col)
{
    row = col = -1;

    int location = -1;
    for (int i = 0; i < m_capacity; i++) {
        if (!m_mapUsed[i]) {
            location = i;
        }
    }
    if (location >= 0) {
        row = location / m_col;
        col = location % m_row;
    }
}

int RealVideoBody::VideoViewManager::GetNoUsedIndex()
{
    int row, col;
    GetLocation(row, col);
    if (row == -1 || col == -1) {
        return -1;
    }
    return row * m_col + col * m_row;
}
