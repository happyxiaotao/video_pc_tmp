#include "videoviewmanager.h"
#include <QDebug>
#include <QGridLayout>
VideoViewManager::VideoViewManager(QGridLayout* layout)
    : m_grid_layout { layout }
    , m_row(3)
    , m_col(3)
    , m_capacity(9)
{
    m_vecUsed.resize(m_capacity);
    for (auto& iter : m_vecUsed) {
        iter = false;
    }

    m_vecPlayer.resize(m_capacity);
    for (auto& iter : m_vecPlayer) {
        iter = new CarVideoPlayer();
    }

    for (int i = 0; i < m_capacity; i++) {
        m_grid_layout->addWidget(m_vecPlayer[i], i / m_col, i % m_row);
    }
}

VideoViewManager::~VideoViewManager()
{
    for (auto& player : m_vecPlayer) {
        m_grid_layout->removeWidget(player);
        delete player;
    }
    m_vecPlayer.clear();
}

int VideoViewManager::Size()
{
    int i = 0;
    std::for_each(m_vecUsed.begin(), m_vecUsed.end(), [&i](bool flag) mutable {if (flag){i++;} });
    return i;
}

CarVideoPlayer* VideoViewManager::GetPlayer(const QString& device_id)
{
    for (int i = 0; i < m_capacity; i++) {
        if (m_vecPlayer[i] != nullptr && m_vecPlayer[i]->GetDeviceId() == device_id) {
            return m_vecPlayer[i];
        }
    }

    qDebug() << __FUNCTION__ << ",SIZE:" << Size() << "\n";

    // 走到这里，说明目前不存则，需要创建
    if (IsFull()) {
        return nullptr;
    }
    // 还有空闲窗口
    int index = GetNoUsedIndex();
    if (index < 0) {
        return nullptr;
    }
    m_vecUsed[index] = true;
    CarVideoPlayer* old_player = nullptr;
    if (m_vecPlayer[index] != nullptr) {
        old_player = m_vecPlayer[index];
        delete m_vecPlayer[index];
        m_vecPlayer[index] = nullptr;
    }
    auto player = new CarVideoPlayer();
    m_vecPlayer[index] = player;

    if (old_player) {
        m_grid_layout->removeWidget(old_player);
    }

    m_grid_layout->addWidget(player, index / m_col, index % m_row);

    return player;
}

void VideoViewManager::ReleasePlayer(CarVideoPlayer* player)
{
    int index = -1;
    for (int i = 0; i < m_capacity; i++) {
        if (m_vecPlayer[i] == player) {
            delete m_vecPlayer[i];
            m_vecPlayer[i] = nullptr;
            m_vecUsed[i] = false;
            index = i;
            break;
        }
    }
    if (index == -1) {
        qDebug() << __FUNCTION__ << ",release failed, not found\n";
    } else {
        m_grid_layout->removeWidget(player);
        auto player = new CarVideoPlayer();
        m_grid_layout->addWidget(player, index / m_col, index % m_row);
    }
}

void VideoViewManager::GetLocation(int& row, int& col)
{
    row = col = -1;

    int location = -1;
    for (int i = 0; i < m_capacity; i++) {
        if (!m_vecUsed[i]) {
            location = i;
            break;
        }
    }
    if (location >= 0) {
        row = location / m_col;
        col = location % m_row;
    }
}

int VideoViewManager::GetNoUsedIndex()
{
    int row, col;
    GetLocation(row, col);
    if (row == -1 || col == -1) {
        return -1;
    }
    return row * m_col + col * m_row;
}
