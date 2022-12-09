#ifndef CARSIDEBAR_H
#define CARSIDEBAR_H

#include "../../CarInfo/cargroup.h"
#include "../../CarInfo/carinfo.h"
#include "../../Common/httpclient.h"
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
    void SetCarTree(const CarGroup& parent, QTreeWidgetItem* root);
    void SetCarTree(const QList<CarChannel*>& listChannel, QTreeWidgetItem* root);
signals:
    void sig_open_video(QString* device_id);
    void sig_close_video(QString* device_id);
private slots:
    void slot_http_finished(QByteArray* array);
private slots:

    void on_treeWidget_car_collapsed(const QModelIndex& index);

    void on_treeWidget_car_expanded(const QModelIndex& index);

    void on_treeWidget_car_pressed(const QModelIndex& index);

    void on_treeWidget_car_itemDoubleClicked(QTreeWidgetItem* item, int column);

public:
    static QString s_text_get_car_real_video_tree_failed;

private:
    Ui::CarSidebar* ui;
    HttpClient* m_http_client;
    QSet<QString> m_setOpeningVideo;
};

#endif // CARSIDEBAR_H
