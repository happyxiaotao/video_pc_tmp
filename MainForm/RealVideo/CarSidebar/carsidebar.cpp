#include "carsidebar.h"
#include "../../Common/configini.h"
#include "ui_carsidebar.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QNetworkCookie>

QString CarSidebar::s_text_get_car_real_video_tree_failed = CONFIG->Get("text", "get_car_real_video_tree_failed", "");

CarSidebar::CarSidebar(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CarSidebar)
    , m_http_client(nullptr)
{
    ui->setupUi(this);
    m_http_client = new HttpClient(this);

    connect(m_http_client, &HttpClient::sig_handler_msg, this, &CarSidebar::slot_http_finished);
}

CarSidebar::~CarSidebar()
{
    disconnect(m_http_client);
    delete m_http_client;
    delete ui;
}

void CarSidebar::Start()
{
    // 创建http请求
    QNetworkRequest request;
    QUrl url("https://tg808.gcpgps.com/v1/impower/carinfo/GetNewVideoChannelTree");
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");

    QString token = m_http_client->GetToken();
    QString cookie = "token=" + token;
    qDebug() << "cookie:" << cookie << "\n";
    request.setRawHeader("cookie", QByteArray(cookie.toStdString().c_str()));
    request.setRawHeader("token", QByteArray(token.toStdString().c_str()));
    m_http_client->Get(request);
}

void CarSidebar::SetCarTree(const QJsonValue& c, const QJsonValue& m)
{
    QJsonArray c_json_array = c.toArray();
    QJsonArray m_json_array = m.toArray();
    QJsonObject obj;

    QString id;

    // 先生成车队map
    QMap<QString, CarGroup*> mapGroup;
    for (auto iter = m_json_array.begin(); iter != m_json_array.end(); ++iter) {
        obj = iter->toObject();
        id = obj["id"].toString();

        //去掉车辆管理中心分组。此组是所有分组的根节点
        if (id == "4") {
            continue;
        }

        CarGroup* group = new CarGroup();
        group->SetId(id);
        group->SetName(obj["name"].toString());
        group->SetPid(obj["pid"].toString());
        mapGroup[id] = group;
    }

    // 过滤不存在车队的汽车，同时将汽车放入对应的车队中
    QString motorcadeId;
    for (auto iter = c_json_array.begin(); iter != c_json_array.end(); ++iter) {
        obj = iter->toObject();

        motorcadeId = obj["motorcadeId"].toString();
        qDebug() << __FUNCTION__ << ",motorcadeId:" << motorcadeId << "\n";
        auto group_iter = mapGroup.find(motorcadeId);
        if (group_iter == mapGroup.end())
            continue;

        auto& group = group_iter.value();
        CarInfo* car = new CarInfo();
        id = obj["id"].toString();
        car->SetAreaName(obj["areaName"].toString());
        car->SetCarNo(obj["carNo"].toString());
        car->SetCarColor(obj["carNoColor"].toString());
        car->SetChannelSum(obj["channelSum"].toString());
        car->SetIccid(obj["iccid"].toString());
        car->SetId(id);
        car->SetStatACC(obj["statAcc"].toString());
        car->SetStatus(obj["status"].toString());
        car->SetMotorcadeId(motorcadeId);
        car->AutoSetChannleList(); //自适应设置通道列表
        group->AddCar(id, car);
    }

    SetCarTree(mapGroup);

    for (auto iter = mapGroup.begin(); iter != mapGroup.end(); ++iter) {
        delete iter.value();
    }
    mapGroup.clear();
}

// 现在是所有的车队，将车队都加载到树状图中，同时处理一个分组包含多个子分组的情况
void CarSidebar::SetCarTree(QMap<QString, CarGroup*>& mapGroup)
{
    ui->treeWidget_car->reset();

    //  QList<QTreeWidgetItem*> list;

    //  for (auto& iter : mapGroup) {
    //      QTreeWidgetItem* item = new QTreeWidgetItem();
    //      item->setText(0, iter->GetName());
    //      list.append(item);
    //  }
    //  qDebug() << "list count=" << list.count() << "\n";
    //  ui->treeWidget_car->addTopLevelItems(list);
    //  // qDeleteAll(list); //不能调用qDeleteAll将list里面的指针数据释放，否则UI会不展示了。
    //  list.clear();

    QList<QTreeWidgetItem*> list;

    // 遍历一次，将存在父Group的，放入到父Group中。
    QString pid;
    for (auto iter = mapGroup.begin(); iter != mapGroup.end();) {
        auto& group = iter.value();
        pid = group->GetPid();
        auto parent_iter = mapGroup.find(pid);
        if (parent_iter == mapGroup.end()) {
            ++iter;
        } else {
            auto& group_id = group->GetId();
            auto& parent_group = parent_iter.value();
            parent_group->AddGroup(group_id, group);
            iter = mapGroup.erase(iter);
        }
    }

    // 此时还mapGroup中保存的，是顶点节点
    for (auto& iter : mapGroup) {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setText(0, iter->GetName());
        list.append(item);

        // 添加子节点
        SetCarTree(*iter, item);
    }
    qDebug() << "list count=" << list.count() << "\n";
    ui->treeWidget_car->addTopLevelItems(list);
    // 排序
    ui->treeWidget_car->sortItems(0, Qt::SortOrder::AscendingOrder);
    //  // qDeleteAll(list); //不能调用qDeleteAll将list里面的指针数据释放，否则UI会不展示了。
    list.clear();
}

void CarSidebar::SetCarTree(const CarGroup& parent, QTreeWidgetItem* root)
{
#define ADD_LIST_WIDGET_ITEM(list, item, name) \
    do {                                       \
        item->setText(0, name);                \
        list.append(item);                     \
    } while (0)

    QList<QTreeWidgetItem*> list;

    if (parent.EmptyGroup()) {
        // 没有分组了，添加汽车
        const auto& mapCar = parent.GetCarMap();
        for (auto& iter : mapCar) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            ADD_LIST_WIDGET_ITEM(list, item, iter->GetCarNo());
            const QList<CarChannel*>& listChannel = iter->GetChannelList();
            SetCarTree(listChannel, item);
        }

    } else {
        const auto& mapGroup = parent.GetGroupMap();
        for (auto& iter : mapGroup) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            ADD_LIST_WIDGET_ITEM(list, item, iter->GetName());
            SetCarTree(*iter, item);
        }
    }
    root->addChildren(list);
    list.clear();
}

void CarSidebar::SetCarTree(const QList<CarChannel*>& listChannel, QTreeWidgetItem* root)
{
    QList<QTreeWidgetItem*> list;
    for (auto& iter : listChannel) {
        QTreeWidgetItem* item = new QTreeWidgetItem();
        ADD_LIST_WIDGET_ITEM(list, item, iter->GetName());
    }
    root->addChildren(list);
    list.clear();
}

void CarSidebar::slot_http_finished(QByteArray* array)
{
    qDebug() << __FUNCTION__ << ",get data len:" << array->length() << "\n";
    // 解析json数据，进行处理
    QJsonParseError err_rpt;
    QJsonDocument doc = QJsonDocument::fromJson(*array, &err_rpt); // 字符串格式化为JSON
    if (err_rpt.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, SYS_TITLE, s_text_get_car_real_video_tree_failed);
        return;
    }
    auto code = doc["code"];
    auto data = doc["data"];
    if (code.isNull() || !code.isDouble() || code.toInt() != 200 || data.isNull()) {
        QMessageBox::warning(this, SYS_TITLE, s_text_get_car_real_video_tree_failed);
        return;
    }
    const QJsonValue& c = data["c"];
    const QJsonValue& m = data["m"];
    SetCarTree(c, m);
}

void CarSidebar::on_treeWidget_car_collapsed(const QModelIndex& index)
{
    ui->treeWidget_car->resizeColumnToContents(index.column());
}

void CarSidebar::on_treeWidget_car_expanded(const QModelIndex& index)
{
    ui->treeWidget_car->resizeColumnToContents(index.column());
}

void CarSidebar::on_treeWidget_car_pressed(const QModelIndex& index)
{
    ui->treeWidget_car->resizeColumnToContents(index.column());
}

void CarSidebar::on_treeWidget_car_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    // 过滤非device_id的item
    const auto& name = item->text(column);
    if (name.length() != 14) {
        return;
    }
    for (int i = 0; i < name.length(); i++) {
        if (!name[i].isDigit())
            return;
    }

    qDebug() << __FUNCTION__ << ",column:" << column << ", item name:" << item->text(column) << "\n";

    QString* device_id = new QString(name);
    if (m_mapOpeningVideo.find(name) != m_mapOpeningVideo.end()) {
        emit sig_close_video(device_id);
    } else {
        // 发送信号，获取对应通道的视频
        emit sig_open_video(device_id);
    }
}
