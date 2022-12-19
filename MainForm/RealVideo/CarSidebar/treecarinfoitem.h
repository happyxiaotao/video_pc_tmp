#ifndef TREECARINFOITEM_H
#define TREECARINFOITEM_H

#include "../CarInfo/cargroup.h"
#include <QTreeWidgetItem>

class TreeCarInfoItem : public QTreeWidgetItem {
public:
    TreeCarInfoItem();
    explicit TreeCarInfoItem(QTreeWidget* treeview);
    ~TreeCarInfoItem();

public:
    bool IsGroup() const { return m_car_group != nullptr; }
    bool IsCarInfo() const { return m_car_info != nullptr; }
    bool IsCarChannel() const { return m_car_channel != nullptr; }

    // 对车队的相关操作
    void SetGroup(CarGroup* carGroup) { m_car_group = carGroup; }
    // 获取车辆总数和在线总数、ACC开和关个数
    void SetGroupOnLineCount(int nTotalCount, int nOnLineCount, int nAccOpenCount, int nAccCloseCount);
    void GetGroupOnLineCount(int& nTotalCount, int& nOnLineCount, int& nAccOpenCount, int& nAccCloseCount);

    // 对车辆的相关操作
    // 获取车辆状态
    void SetCarInfo(CarInfo* carInfo) { m_car_info = carInfo; }
    QString GetCarInfoCarStatus() { return m_car_info->GetStatus(); }
    QString GetCarNo() { return m_car_info->GetCarNo(); } // 车牌号

    // 对车辆通道的相关操作
    void SetCarChannel(CarChannel* channel) { m_car_channel = channel; }
    // 获取通道名称
    QString GetChannelDeviceId() { return m_car_channel->GetDeviceId(); }
    void SetCarId(const QString& car_id) { m_car_id = car_id; } // 车的标号id
    void SetCarChannelCarNo(const QString& car_no) { m_car_channel_car_no = car_no; }
    QString GetCarId() { return m_car_id; }
    QString GetChannelAlias() { return m_car_channel->GetAliasName(); }
    QString GetCarChannelCarNo() { return m_car_channel_car_no; }

    // 重写比较函数，避免有昵称的通道，排序异常（通道应该根据device_id进行排序）
    bool operator<(const QTreeWidgetItem& other) const;

private:
    CarGroup* m_car_group; // 如果表示的是车队，则为非空
    int m_nTotalCount;
    int m_nOnLineCount;
    int m_nAccOpenCount;
    int m_nAccCloseCount;

    CarInfo* m_car_info; // 如果表示的是车辆，则为非空
    CarChannel* m_car_channel; //如果表示的是车辆通道，则为非空
    QString m_car_id; //特别的，由于需要获取car_id。所以需要保存。（在IsCarInfo和IsCarChannel时有效）
    QString m_car_channel_car_no; //通道对应的车牌号（当类型诶通道时使用）
};

#endif // TREECARINFOITEM_H
