#ifndef CARSIDEBAR_H
#define CARSIDEBAR_H

#include "../../CarInfo/cargroup.h"
#include "../../CarInfo/carinfo.h"
#include "../../Common/httpclient.h"
#include "treecarinfoitem.h"
#include <QCompleter>
#include <QStringList>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QWidget>
namespace Ui {
class CarSidebar;
}

class CarSidebar : public QWidget {
    Q_OBJECT

public:
    explicit CarSidebar(QWidget* parent = nullptr);
    ~CarSidebar();

public:
    void UpdateMaxCount(int max_count);

    void UpdateCarTree();

private:
    void SetCarTree(const QJsonValue& c, const QJsonValue& m);
    void SetCarTree(QMap<QString, CarGroup*>& mapGroup);
    void SetCarTree(const CarGroup& parent, TreeCarInfoItem* root);
    void SetCarTree(const QList<CarChannel*>& listChannel, TreeCarInfoItem* root, const QString& car_id, const QString& car_no);

    void ClearCarListForSearch();
    void UpdateCarListForSearch(QMap<QString, CarGroup*>& mapGroup);
    void UpdateCarListForSearch(QMap<QString, CarGroup*>& mapGroup, QStringList& list);
    void UpdateCarListForSearch(const CarGroup& group, QStringList& list);

    void FindAndJumpFromTree(const QString& car_no);

    void OpenOrCloseVideo(const QString& device_id, const QString& carId, const QString& car_no, const QString& channel_alias);

    void ClearRealVideoTree();
    void GetRealVideoTree();
    void PostRealVideoPosition(const QString& device_id);

    void SetLastCarInfo(const QString& device_id, const QString& car_id);
    void ClearLastCarInfo();

    void StartTimer(int ms);
    void StopTimer();

    // 根据车辆状态，返回图标地址
    QString GetIconPathByCarStatus(int nCarStatus);
    QString GetIconPathByChannelType(const QJsonArray& jsonArray);

signals:
    // 对外发送信号，打开或关闭车辆信息
    void sig_open_video(QString* device_id, QString* channel_alias);
    void sig_close_video(QString* device_id);

    // 对外发送信号，更新车辆位置信息
    void sig_update_car_position(QString* glat, QString* glng, QString* image, QString* direction, QString* text);
    // 对外发送信号，删除车辆的位置标记
    void sig_clear_car_position();
private slots:
    // 获取视频树的回调
    void slot_http_finished_get_real_video_tree(QByteArray* array);
    // 获取定位信息的回调
    void slot_http_finished_post_real_video_position(QByteArray* array);

    // 定时任务回调
    void slot_on_timer();
public slots:

    // 更新视频最大个数。超过最大个数后，再次点击，会弹框提示
    void slot_update_max_view_count(int max_count);

    // 通过窗口个数变化，被动删除的多余窗口
    void slot_close_video(QString* device_id); // 通过窗口3*3、4*4之类的变化，删除多余的视频

    // 用户在视频界面，点击"全部关闭"
    void slot_close_all();
private slots:

    void on_treeWidget_car_collapsed(const QModelIndex& index);

    void on_treeWidget_car_expanded(const QModelIndex& index);

    void on_treeWidget_car_pressed(const QModelIndex& index);

    void on_treeWidget_car_itemDoubleClicked(QTreeWidgetItem* item, int column);

    void on_pushButton_search_clicked();

    void on_lineEdit_search_input_editingFinished();

    void on_lineEdit_search_input_textChanged(const QString& arg1);

    void on_pushButton_refresh_clicked();

private:
    static QString s_resource_dir;
    static QString s_text_title;
    static QString s_text_get_car_real_video_tree_failed;
    static QString s_text_post_car_real_video_position_failed;
    static QString s_text_acc_open;
    static QString s_text_acc_close;
    static QString s_text_open;
    static QString s_text_close;
    static QString s_icon_path_car_status_0;
    static QString s_icon_path_car_status_1;
    static QString s_icon_path_car_status_2;
    static QString s_icon_path_car_status_3;
    static QString s_icon_path_car_status_4;
    static QString s_icon_path_car_status_5;
    static QString s_icon_path_car_status_6;
    static QString s_icon_path_car_status_7;
    static QString s_icon_path_car_status_8;
    static QString s_icon_path_car_status_9;
    static QString s_icon_path_car_status_10;
    static QString s_icon_path_car_status_11;
    static QString s_icon_path_channel_type_1;
    static QString s_icon_path_channel_type_2;
    static QString s_icon_path_channel_type_3;
    static QString s_icon_path_channel_type_4;

private:
    Ui::CarSidebar* ui;

    HttpClient* m_http_client_get_real_video_tree;
    HttpClient* m_http_client_post_real_video_position; //获取实时位置
    QSet<QString> m_setOpeningVideo;
    QMap<QString, CarGroup*> m_mapGroup; //保存的车队列表，里面包含子车队和车辆

    QCompleter* m_search_completer; // 搜索智能提供组件

    QTimer* m_timer;
    QString m_last_car_device_id; //最近使用的device_id
    QString m_last_car_id; // 最近使用的car_id用来在定时器中使用

    int m_max_open_count; // 允许打开的最大窗口个数
};

#endif // CARSIDEBAR_H
