#include "cargroup.h"

CarGroup::CarGroup()
{
}

CarGroup::~CarGroup()
{
    for (auto iter = m_mapCar.begin(); iter != m_mapCar.end(); ++iter) {
        delete iter.value();
        iter.value() = nullptr;
    }
    m_mapCar.clear();

    for (auto iter = m_mapGroup.begin(); iter != m_mapGroup.end(); ++iter) {
        delete iter.value();
        iter.value() = nullptr;
    }
    m_mapGroup.clear();
}

void CarGroup::AddCar(const QString& id, CarInfo* car)
{
    DelCar(id);
    m_mapCar[id] = car;
}

void CarGroup::DelCar(const QString& id)
{
    auto iter = m_mapCar.find(id);
    if (iter != m_mapCar.end()) {
        delete iter.value();
        m_mapCar.erase(iter);
    }
}

void CarGroup::AddGroup(const QString& id, CarGroup* group)
{
    DelGroup(id);
    m_mapGroup[id] = group;
    group->SetParentGroup(this);
}

void CarGroup::DelGroup(const QString& id)
{
    auto iter = m_mapGroup.find(id);
    if (iter != m_mapGroup.end()) {
        iter.value()->SetParentGroup(nullptr);
        delete iter.value();
        m_mapGroup.erase(iter);
    }
}
