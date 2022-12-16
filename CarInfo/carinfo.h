#ifndef CARINFO_H
#define CARINFO_H
#include <QList>
#include <QSet>

class CarChannel {
public:
    CarChannel(const QString& deviceId, const QString& alias = QString())
        : m_devideId(deviceId)
        , m_alias(alias)
    {
    }
    ~CarChannel() { }

public:
    const QString& GetDeviceId() const
    {
        return m_devideId;
    }
    const QString& GetAliasName() const
    {
        return m_alias;
    }

private:
    QString m_devideId;
    QString m_alias;
};

class CarInfo {
public:
    CarInfo();
    ~CarInfo();

public:
    void SetAreaName(const QString& value) { m_areaName = value; }
    void SetCarNo(const QString& value) { m_car_no = value; }
    void SetCarColor(const QString& value) { m_car_color = value; }
    void SetChannelSum(const QString& value) { m_channel_sum = value; }
    void SetIccid(const QString& value);
    void SetId(const QString& value) { m_id = value; }
    void SetStatACC(const QString& value) { m_stat_acc = value; }
    void SetStatus(const QString& value) { m_status = value; }
    void SetMotorcadeId(const QString& value) { m_motorcade_id = value; }
    void SetChannelType(const QString& value) { m_channelType = value; }

    const QString& GetCarNo() { return m_car_no; }
    const QString& GetId() { return m_id; }
    const QString& GetIccid() { return m_iccid; }
    const QString& GetMotorcadeId() { return m_motorcade_id; }
    const QString& GetStatus() { return m_status; }
    const QString& GetAcc() { return m_stat_acc; }

    // 自动设置通道列表
    void AutoSetChannleList();
    const QList<CarChannel*>& GetChannelList() const { return m_listChannel; }

private:
    QString m_areaName; // 区域
    QString m_car_no; // 车牌
    QString m_car_color; //汽车颜色
    QString m_channel_sum; //通道总数
    // QString m_car_device_id; // 汽车的设备id，不是实际请求的通道id  01421187210600
    QString m_iccid; // iccid  14211872106
    QString m_id; // 车辆编号
    QString m_stat_acc; //是否开启了acc
    QString m_status; //状态
    QString m_motorcade_id; //对应车队id
    QString m_channelType; //通道类型（json格式，各个通道对应的外号）

    QList<CarChannel*> m_listChannel; // 自定义保存的的通道号
};

#endif // CARINFO_H
