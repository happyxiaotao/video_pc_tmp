#ifndef VIDEOVIEWMANAGER_H
#define VIDEOVIEWMANAGER_H

#include "../../car_video_view/carvideoplayer.h"
enum WinCountType {
    None_None,
    One_One,
    Two_Two,
    Three_Three,
    Four_Four
};
class RealVideoBody;
class QGridLayout;
class VideoViewManager : public QObject {
    Q_OBJECT
public:
    explicit VideoViewManager(RealVideoBody* real_video_body, QWidget* body);
    ~VideoViewManager();
signals:
public:
    inline int Capacity() { return m_vecPlayer.size(); }
    int Size();
    inline bool IsFull() { return Size() >= Capacity(); }

    // 获取player，如果对应device_id不存在则创建。
    CarVideoPlayer* GetPlayer(const QString& device_id);
    void ReleasePlayer(CarVideoPlayer* player);

    void CloseAll();

    void ChangeWinCount(WinCountType type);

private:
    void GetLocation(int& row, int& col);
    int GetNoUsedIndex();

    void ConnectSignal(CarVideoPlayer* player);
    void DisconnectSignal(CarVideoPlayer* player);

    // 用来删除gridlayout布局中的组件
    void ClearGridLayout(int nOldCapacity, int nNewCapacity);

private:
    RealVideoBody* m_real_video_body;
    QWidget* m_body;
    QGridLayout* m_grid_layout; // 指向body的grid layout

    int m_row; // 行数
    int m_col; // 列数
    QVector<bool> m_vecUsed;
    QVector<CarVideoPlayer*> m_vecPlayer;
    int m_nMaxHistoryRow;
    int m_nMaxHistoryCol;
    int m_nMaxHistoryCapacity;

    WinCountType m_cur_win_count_type;
};

#endif // VIDEOVIEWMANAGER_H
