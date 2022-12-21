#include "treecarinfoitem.h"

TreeCarInfoItem::TreeCarInfoItem()
    : m_car_group(nullptr)
    , m_car_info(nullptr)
    , m_car_channel(nullptr)
{
    m_nTotalCount = 0;
    m_nOnLineCount = 0;
    m_nAccOpenCount = 0;
    m_nAccCloseCount = 0;
}

TreeCarInfoItem::TreeCarInfoItem(QTreeWidget* treeview)
    : QTreeWidgetItem(treeview)
    , m_car_group(nullptr)
    , m_car_info(nullptr)
    , m_car_channel(nullptr)
{
    m_nTotalCount = 0;
    m_nOnLineCount = 0;
    m_nAccOpenCount = 0;
    m_nAccCloseCount = 0;
}

TreeCarInfoItem::~TreeCarInfoItem()
{
}

void TreeCarInfoItem::SetGroupOnLineCount(int nTotalCount, int nOnLineCount, int nAccOpenCount, int nAccCloseCount)
{
    m_nTotalCount = nTotalCount;
    m_nOnLineCount = nOnLineCount;
    m_nAccOpenCount = nAccOpenCount;
    m_nAccCloseCount = nAccCloseCount;
}

void TreeCarInfoItem::GetGroupOnLineCount(int& nTotalCount, int& nOnLineCount, int& nAccOpenCount, int& nAccCloseCount)
{
    nTotalCount = m_nTotalCount;
    nOnLineCount = m_nOnLineCount;
    nAccOpenCount = m_nAccOpenCount;
    nAccCloseCount = m_nAccCloseCount;
}

bool TreeCarInfoItem::operator<(const QTreeWidgetItem& other) const
{
    const TreeCarInfoItem& right = dynamic_cast<const TreeCarInfoItem&>(other);

    // 1、车队比较
    if (IsGroup() && m_car_group) {
        //根据车队名称比较
        return m_car_group->GetName() < right.m_car_group->GetName();
    }

    // 2、车辆比较
    if (IsCarInfo() && m_car_info) {
        return m_car_info->GetCarNo() < right.m_car_info->GetCarNo();
    }

    // 3、通道比较
    if (IsCarChannel() && m_car_channel) {
        return m_car_channel->GetDeviceId() < right.m_car_channel->GetDeviceId();
    }

    return false;
}
