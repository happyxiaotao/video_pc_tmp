#ifndef VIDEOVIEWMANAGER_H
#define VIDEOVIEWMANAGER_H

#include "../../car_video_view/carvideoplayer.h"

class QGridLayout;
class VideoViewManager : public QObject {
    Q_OBJECT
public:
    explicit VideoViewManager(QGridLayout* layout);
    ~VideoViewManager();
signals:
public:
    int Capacity() { return m_capacity; }
    int Size();
    bool IsFull() { return Size() >= Capacity(); }

    // 获取player，如果对应device_id不存在则创建。
    CarVideoPlayer* GetPlayer(const QString& device_id);
    void ReleasePlayer(CarVideoPlayer* player);

    void CloseAll();

private:
    void GetLocation(int& row, int& col);
    int GetNoUsedIndex();

    void ConnectSignal(CarVideoPlayer* player);
    void DisconnectSignal(CarVideoPlayer* player);

private:
    QGridLayout* m_grid_layout; // 指向外界的grid layout

    int m_row;
    int m_col;
    int m_capacity; // 视频组件展示容量
    QVector<bool> m_vecUsed;
    QVector<CarVideoPlayer*> m_vecPlayer;
};

#endif // VIDEOVIEWMANAGER_H
