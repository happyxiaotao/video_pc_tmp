#include "carinfo.h"
#include <QDebug>
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
    char tmp[3];
    for (int i = 1; i <= sum; i++) {

        snprintf(tmp, 3, "%02d", i);
        QString name = m_iccid + tmp;

        m_listChannel.append(new CarChannel(name));
        //   qDebug() << "iccid:" << m_iccid << ",i:" << i << ",name:" << name << "\n ";
    }
}
