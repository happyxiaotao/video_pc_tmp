#ifndef REALVIDEOBODY_H
#define REALVIDEOBODY_H

#include "../../car_video_view/carvideoplayer.h"
#include <QGridLayout>
#include <QWidget>

namespace Ui {
class RealVideoBody;
}

class RealVideoBody : public QWidget {
    Q_OBJECT

    // 视频播放的视图管理器 （目前默认3*3）
    class VideoViewManager {
    public:
        VideoViewManager(QGridLayout* layout);

        ~VideoViewManager();

    public:
        int Capacity() { return m_capacity; }
        int Size();
        bool IsFull() { return Size() >= Capacity(); }

        CarVideoPlayer* GetDstPlayer(const QString& device_id);
        CarVideoPlayer* GetNoUsedPlayer();

    private:
        void GetLocation(int& row, int& col);
        int GetNoUsedIndex();

    private:
        QGridLayout* m_grid_layout; // 指向外界的grid layout

        int m_row;
        int m_col;
        int m_capacity; // 视频组件展示容量
        QMap<int, bool> m_mapUsed; //使用情况，某个下标是否使用了
        QMap<int, CarVideoPlayer*> m_mapPlayer; // int表示下标，作为容器，保存所有的Player 注意，发现不能删除CarVideoPlayer，否则会出现程序崩溃问题
    };

public:
    explicit RealVideoBody(QWidget* parent = nullptr);
    ~RealVideoBody();

public slots:
    void slot_open_video(QString* _device_id);
    void slot_close_video(QString* _device_id);

private:
    void SetCarVideoPlayerSignals(CarVideoPlayer* player);
    void RleaseCarVideoPlayerSignals(CarVideoPlayer* player);

private:
    Ui::RealVideoBody* ui;

    VideoViewManager* m_view_manager;
};

#endif // REALVIDEOBODY_H
