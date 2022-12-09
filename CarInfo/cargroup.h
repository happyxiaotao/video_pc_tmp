#ifndef CARGROUP_H
#define CARGROUP_H

#include "carinfo.h"
#include <QMap>
class CarGroup {
public:
    CarGroup();
    ~CarGroup();

public:
    void SetName(const QString& value) { m_name = value; }
    void SetPid(const QString& value) { m_pid = value; }
    void SetId(const QString& value) { m_id = value; }

    const QString& GetPid() { return m_pid; }
    const QString& GetId() { return m_id; }
    const QString& GetName() { return m_name; }

    void AddCar(const QString& id, CarInfo* car);
    void DelCar(const QString& id);
    size_t CarSize() const { return m_mapCar.size(); }
    bool EmptyCar() const { return CarSize() == 0; }
    const QMap<QString, CarInfo*>& GetCarMap() const { return m_mapCar; }

    void AddGroup(const QString& id, CarGroup* group);
    void DelGroup(const QString& id);
    size_t GroupSize() const { return m_mapGroup.size(); }
    bool EmptyGroup() const { return GroupSize() == 0; }
    const QMap<QString, CarGroup*>& GetGroupMap() const { return m_mapGroup; }

    void SetParentGroup(const CarGroup* group) { m_parent_group = const_cast<CarGroup*>(group); }
    CarGroup* GetParentGroup() { return m_parent_group; }

private:
    QString m_name;
    QString m_pid;
    QString m_id;
    QMap<QString, CarInfo*> m_mapCar;
    QMap<QString, CarGroup*> m_mapGroup; // 可能还包含有其他子分组
    CarGroup* m_parent_group;
};

#endif // CARGROUP_H
