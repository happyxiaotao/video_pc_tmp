#include "carinfo.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
static void ClearCarChannels(QList<CarChannel*>& list)
{
    for (auto& iter : list) {
        delete iter;
    }
    list.clear();
}
CarInfo::CarInfo()
{
}

CarInfo::~CarInfo()
{
    ClearCarChannels(m_listChannel);
}

void CarInfo::SetIccid(const QString& value)
{
    int need_add_zero_count = 12 - value.size();
    if (need_add_zero_count > 0) {
        QString zero(need_add_zero_count, '0');
        m_iccid = zero + value;
    } else {
        m_iccid = value;
    }
}

void CarInfo::AutoSetChannleList()
{
    ClearCarChannels(m_listChannel);
    if (m_iccid.isEmpty() || m_channel_sum.isEmpty()) {
        return;
    }
    int sum = m_channel_sum.toInt();
    // 如果有返回channelType字段，则设置为对应的字段
    QJsonDocument doc = QJsonDocument::fromJson(m_channelType.toUtf8());
    char tmp[3];
    for (int i = 1; i <= sum; i++) {

        snprintf(tmp, 3, "%02d", i);
        QString strDeviceId = m_iccid + tmp;

        auto strNumber = QString::number(i);
        QString alias;
        if (!m_channelType.isEmpty()) {
            auto obj = doc[strNumber].toObject();
            alias = obj["alias"].toString();
        }
        if (alias.isEmpty()) {
            alias = QString::fromUtf8("通道") + strNumber;
        }
        // alias为空，则使用name代替
        auto channel = new CarChannel(strDeviceId, alias);
        m_listChannel.append(channel);
        //   qDebug() << "iccid:" << m_iccid << ",i:" << i << ",name:" << name << "\n ";
    }
}
