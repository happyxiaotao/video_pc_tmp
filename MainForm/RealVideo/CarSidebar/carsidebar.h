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
    void Start();

private:
    void SetCarTree(const QJsonValue& c, const QJsonValue& m);
    void SetCarTree(QMap<QString, CarGroup*>& mapGroup);
    void SetCarTree(const CarGroup& parent, TreeCarInfoItem* root);
    void SetCarTree(const QList<CarChannel*>& listChannel, TreeCarInfoItem* root, const QString& car_id);

    void ClearCarListForSearch();
    void UpdateCarListForSearch(QMap<QString, CarGroup*>& mapGroup);
    void UpdateCarListForSearch(QMap<QString, CarGroup*>& mapGroup, QStringList& list);
    void UpdateCarListForSearch(const CarGroup& group, QStringList& list);

    void FindAndJumpFromTree(const QString& car_no);

    void OpenOrCloseVideo(const QString& device_id, const QString& carId);

    void GetRealVideoTree();
    void PostRealVideoPosition(const QString& device_id);

    void SetLastCarInfo(const QString& device_id, const QString& car_id);
    void ClearLastCarInfo();

    void StartTimer(int ms);
    void StopTimer();

signals:
    void sig_open_video(QString* device_id);
    void sig_close_video(QString* device_id);

    void sig_update_car_position(QString* glat, QString* glng);
private slots:
    // 获取视频树的回调
    void slot_http_finished_get_real_video_tree(QByteArray* array);
    // 获取定位信息的回调
    void slot_http_finished_post_real_video_position(QByteArray* array);

    void slot_on_timer();
public slots:
    void slot_close_video(QString* device_id); // 通过窗口3*3、4*4之类的变化，删除多余的视频
    void slot_close_all();
private slots:

    void on_treeWidget_car_collapsed(const QModelIndex& index);

    void on_treeWidget_car_expanded(const QModelIndex& index);

    void on_treeWidget_car_pressed(const QModelIndex& index);

    void on_treeWidget_car_itemDoubleClicked(QTreeWidgetItem* item, int column);

    void on_pushButton_search_clicked();

    void on_lineEdit_search_input_editingFinished();

    void on_pushButton_test_1_clicked();

    void on_pushButton_test_2_clicked();

    void on_pushButton_test_3_clicked();

    void on_pushButton_test_4_clicked();

    void on_lineEdit_search_input_textChanged(const QString& arg1);

private:
    static QString s_resource_dir;
    static QString s_text_title;
    static QString s_text_get_car_real_video_tree_failed;
    static QString s_text_post_car_real_video_position_failed;
    static QString s_text_acc_open;
    static QString s_text_acc_close;
    static QString s_text_open;
    static QString s_text_close;

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
};

#endif // CARSIDEBAR_H
