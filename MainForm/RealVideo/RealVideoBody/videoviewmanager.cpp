#include "videoviewmanager.h"
#include "realvideobody.h"
#include <QDebug>
#include <QGridLayout>
VideoViewManager::VideoViewManager(RealVideoBody* real_video_body, QWidget* body)
    : m_real_video_body(real_video_body)
    , m_body(body)
    , m_grid_layout { nullptr }
    , m_row(0)
    , m_col(0)
    , m_nMaxHistoryRow(0)
    , m_nMaxHistoryCol(0)
    , m_nMaxHistoryCapacity(0)
    , m_cur_win_count_type(WinCountType::None_None)
{
    m_grid_layout = new QGridLayout(body);
    m_grid_layout->setSpacing(1);
    body->setLayout(m_grid_layout);
}

VideoViewManager::~VideoViewManager()
{
    for (auto& player : m_vecPlayer) {
        m_grid_layout->removeWidget(player);
        ReleaseCarVideoPlayer(player);
    }
    m_vecPlayer.clear();
    m_grid_layout->deleteLater();
}

int VideoViewManager::Size()
{
    int i = 0;
    std::for_each(m_vecUsed.begin(), m_vecUsed.end(), [&i](bool flag) mutable {if (flag){i++;} });
    return i;
}

CarVideoPlayer* VideoViewManager::GetPlayer(const QString& device_id)
{
    for (int i = 0; i < m_vecPlayer.size(); i++) {
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
        ReleaseCarVideoPlayer(m_vecPlayer[index]);
    }
    auto player = CreateCarVideoPlayer();
    m_vecPlayer[index] = player;

    if (old_player) {
        m_grid_layout->removeWidget(old_player);
    }

    int row = index / m_col;
    int col = index % m_row;
    qDebug() << "index:" << index << ",row:" << row << ",col:" << col << "\n";
    m_grid_layout->addWidget(player, row, col);
    player->ShowDefaultBackGround();

    return player;
}

void VideoViewManager::ReleasePlayer(CarVideoPlayer* player)
{
    if (player == nullptr) {
        return;
    }

    int index = -1;
    for (int i = 0; i < m_vecPlayer.size(); i++) {
        if (m_vecPlayer[i] == player) {
            ReleaseCarVideoPlayer(m_vecPlayer[i]);
            m_vecUsed[i] = false;
            index = i;
            break;
        }
    }
    if (index == -1) {
        qDebug() << __FUNCTION__ << ",release failed, not found\n";
    } else {
        m_grid_layout->removeWidget(player);
        auto player = CreateCarVideoPlayer();
        m_vecPlayer[index] = player;
        m_grid_layout->addWidget(player, index / m_col, index % m_row);
        player->ShowDefaultBackGround();
    }
}

void VideoViewManager::CloseAll()
{
    for (int i = 0; i < m_vecPlayer.size(); i++) {
        auto player = m_vecPlayer[i];

        if (!player->GetDeviceId().isEmpty()) {
            player->close_video();
            m_grid_layout->removeWidget(player);

            ReleaseCarVideoPlayer(player);
            player = CreateCarVideoPlayer();
            m_grid_layout->addWidget(player, i / m_col, i % m_row);
            player->ShowDefaultBackGround();
            m_vecPlayer[i] = player;
        }
    }
    for (int i = 0; i < m_vecPlayer.size(); i++) {
        m_vecUsed[i] = false;
    }
}

void VideoViewManager::ChangeWinCount(WinCountType type)
{
    if (m_cur_win_count_type == type) {
        return;
    }

    switch (type) {
    case WinCountType::One_One:
        m_row = 1;
        m_col = 1;
        break;
    case WinCountType::Two_Two:
        m_row = 2;
        m_col = 2;
        break;
    case WinCountType::Three_Three:
        m_row = 3;
        m_col = 3;
        break;
    case WinCountType::Four_Four:
        m_row = 4;
        m_col = 4;
        break;
    default:
        return;
    }

    // 新容量
    int nOldCapacity = m_vecPlayer.size();
    int nNewCapacity = m_row * m_col;

    // 隐藏所有的图片
    for (auto& player : m_vecPlayer) {
        player->setVisible(false); // 隐藏起来，否则，布局会有问题
        player->ClearLabel(); // 先清空QLabel内容，否则重新布局时，会导致窗口大小分配不平均
    }

    // m_vecPlayer如果不够，则添加
    for (int i = nOldCapacity; i < nNewCapacity; i++) {
        auto player = CreateCarVideoPlayer();
        m_vecPlayer.append(player);
    }

    // 清空旧的布局
    // 注：由于m_grid_layout通过removeItem删除不掉实际的内部布局，导致从大到小时，视频尺寸异常
    // 处理方法待定
    ClearGridLayout(nOldCapacity, nNewCapacity);
    qDebug() << __FUNCTION__ << "grid,row:" << m_grid_layout->rowCount() << ",col:" << m_grid_layout->columnCount() << "\n";

    // 使用新的布局
    int row = 0;
    int col = 0;
    for (int i = 0; i < nNewCapacity; i++) {
        row = i / m_col;
        col = i % m_row;
        m_grid_layout->addWidget(m_vecPlayer[i], row, col);
        m_grid_layout->setRowStretch(row, 1);
        m_grid_layout->setColumnStretch(col, 1);
    }
    // 删除多余视频
    while (m_vecPlayer.size() > nNewCapacity) {
        auto& player = m_vecPlayer.back();

        QString* device_id = new QString(player->GetDeviceId());
        qDebug() << __FUNCTION__ << ",*device_id:" << *device_id << "\n";
        // emit m_real_video_body->sig_close_video(device_id);
        emit sig_close_video(device_id);

        player->close_video();

        ReleaseCarVideoPlayer(player);
        m_vecPlayer.pop_back();
    }
    //设置m_vecUsed
    m_vecUsed.resize(nNewCapacity);
    for (int i = nOldCapacity; i < nNewCapacity; i++) {
        m_vecUsed[i] = false;
    }
    // 新窗口设置背景图片
    for (int i = nOldCapacity; i < nNewCapacity; i++) {
        m_vecPlayer[i]->ShowDefaultBackGround();
    }
    // 重新设置视频尺寸
    for (int i = 0; i < m_row; i++) {
        for (int j = 0; j < m_col; j++) {
            int index = i * m_col + j;
            m_vecPlayer[index]->setVisible(true);
        }
    }
}

void VideoViewManager::GetLocation(int& row, int& col)
{
    row = col = -1;

    int location = -1;
    for (int i = 0; i < m_vecPlayer.size(); i++) {
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
    for (int i = 0; i < m_vecPlayer.size(); i++) {
        if (!m_vecUsed[i]) {
            return i;
        }
    }

    return -1;
}

// 由于gridlayout中的内部布局，无法实际删除，所以需要设置strech为0
void VideoViewManager::ClearGridLayout(int nOldCapacity, int nNewCapacity)
{
    m_nMaxHistoryCapacity = std::max<int>(std::max<int>(nOldCapacity, nNewCapacity), m_nMaxHistoryCapacity);
    m_nMaxHistoryRow = std::max<int>(m_nMaxHistoryRow, m_row);
    m_nMaxHistoryCol = std::max<int>(m_nMaxHistoryCol, m_col);

    QLayoutItem* child;
    while ((child = m_grid_layout->takeAt(0)) != nullptr) {
        child->widget()->setParent(nullptr);
        m_grid_layout->removeWidget(child->widget());
        delete child;
    }

    // 设置布局stretch为0
    int row = 0;
    int col = 0;
    for (int i = 0; i < m_nMaxHistoryCapacity; i++) {
        row = i / m_nMaxHistoryCol;
        col = i % m_nMaxHistoryRow;
        m_grid_layout->setRowStretch(row, 0);
        m_grid_layout->setColumnStretch(col, 0);
    }
}

CarVideoPlayer* VideoViewManager::CreateCarVideoPlayer()
{
    auto player = new CarVideoPlayer();
    BindOrUnBindCarVideoPlayerSignals(player, true);
    return player;
}

void VideoViewManager::ReleaseCarVideoPlayer(CarVideoPlayer*& player)
{
    BindOrUnBindCarVideoPlayerSignals(player, false);
    delete player;
    player = nullptr;
}

void VideoViewManager::BindOrUnBindCarVideoPlayerSignals(CarVideoPlayer* player, bool bBind)
{
    if (bBind) {
        connect(player, &CarVideoPlayer::sig_close_video_by_pushbutton, this, &VideoViewManager::slot_close_video_by_car_video_player);
    } else {
        disconnect(player, &CarVideoPlayer::sig_close_video_by_pushbutton, this, &VideoViewManager::slot_close_video_by_car_video_player);
    }
}

void VideoViewManager::slot_close_video_by_car_video_player(QString* device_id)
{
    auto player = GetPlayer(*device_id);
    ReleasePlayer(player);

    // 通知CarSiderbar关闭此通道的缓存，支持重新点击，重新加载
    emit sig_close_video(device_id);
}
