#ifndef CARINFO_H
#define CARINFO_H
#include <QList>
#include <QSet>

class CarChannel {
public:
    CarChannel(const QString& name)
        : m_name(name)
    {
    }
    ~CarChannel() { }

public:
    const QString& GetName() const { return m_name; }

private:
    QString m_name;
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

    const QString& GetCarNo() { return m_car_no; }
    const QString& GetId() { return m_id; }
    const QString& GetIccid() { return m_iccid; }
    const QString& GetMotorcadeId() { return m_motorcade_id; }

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

    QList<CarChannel*> m_listChannel; // 自定义保存的的通道号
};

#endif // CARINFO_H
