#ifndef VIDEOVIEWMANAGER_H
#define VIDEOVIEWMANAGER_H

#include "../../car_video_view/carvideoplayer.h"
enum class WinCountType : int {
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
    void sig_close_video(QString* device_id);

    void sig_update_view_width_height_prop(int prop);

public:
    inline int Capacity() { return m_vecPlayer.size(); }
    int Size();
    inline bool IsFull() { return Size() >= Capacity(); }

    // 获取player，如果对应device_id不存在则创建。
    CarVideoPlayer* GetPlayer(const QString& device_id);
    void ReleasePlayer(CarVideoPlayer* player);

    void CloseAll();

    void ChangeWinCount(WinCountType type);
    void ChangeWinView(WinViewType type);

private:
    void GetLocation(int& row, int& col);
    int GetNoUsedIndex();

    void ConnectSignal(CarVideoPlayer* player);
    void DisconnectSignal(CarVideoPlayer* player);

    // 用来删除gridlayout布局中的组件
    void ClearGridLayout(int nOldCapacity, int nNewCapacity);

    // 创建car_video_player
    CarVideoPlayer* CreateCarVideoPlayer();
    void ReleaseCarVideoPlayer(CarVideoPlayer*& player);
    // 绑定或者解绑videoPlayer的信号
    void BindOrUnBindCarVideoPlayerSignals(CarVideoPlayer* player, bool bBind = true);
private slots:
    void slot_close_video_by_car_video_player(QString* device_id);

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
