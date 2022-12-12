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

#include <QStringListModel>

QString CarSidebar::s_text_get_car_real_video_tree_failed = CONFIG->Get("text", "get_car_real_video_tree_failed", "");

CarSidebar::CarSidebar(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CarSidebar)
    , m_http_client(nullptr)
    , m_search_completer(nullptr)
{
    ui->setupUi(this);
    m_http_client = new HttpClient(this);

    connect(m_http_client, &HttpClient::sig_handler_msg, this, &CarSidebar::slot_http_finished);

    m_search_completer = new QCompleter(ui->lineEdit_search_input); // 设置父对象为ui->lineEdit后，不需要显式delete资源
    m_search_completer->setCaseSensitivity(Qt::CaseInsensitive); //忽略大小写
    m_search_completer->setFilterMode(Qt::MatchFlags::enum_type::MatchContains);
    ui->lineEdit_search_input->setCompleter(m_search_completer); //绑定completer
}

CarSidebar::~CarSidebar()
{
    disconnect(m_http_client);
    delete m_http_client;

    for (auto iter = m_mapGroup.begin(); iter != m_mapGroup.end(); ++iter) {
        delete iter.value();
    }
    m_mapGroup.clear();

    ClearCarListForSearch();

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

static QString Json_Value_To_String(const QJsonValue& value)
{
    if (value.isDouble()) {
        return QString("%1").arg(value.toDouble());
    } else if (value.isString()) {
        return value.toString();
    } else if (value.isBool()) {
        return QString("%1").arg(value.toBool());
    } else {
        return "";
    }
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
        id = Json_Value_To_String((obj["id"]));

        //去掉车辆管理中心分组。此组是所有分组的根节点
        if (id == "4") {
            continue;
        }

        CarGroup* group = new CarGroup();
        group->SetId(id);

        group->SetName(Json_Value_To_String(obj["name"]));
        group->SetPid(Json_Value_To_String(obj["pid"]));

        mapGroup[id] = group;
    }

    // 过滤不存在车队的汽车，同时将汽车放入对应的车队中
    QString motorcadeId;
    for (auto iter = c_json_array.begin(); iter != c_json_array.end(); ++iter) {
        obj = iter->toObject();

        motorcadeId = Json_Value_To_String(obj["motorcadeId"]);
        // qDebug() << __FUNCTION__ << ",motorcadeId:" << motorcadeId << "\n";
        auto group_iter = mapGroup.find(motorcadeId);
        if (group_iter == mapGroup.end())
            continue;

        auto& group = group_iter.value();
        CarInfo* car = new CarInfo();

        id = Json_Value_To_String(obj["id"]);
        car->SetAreaName(Json_Value_To_String(obj["areaName"]));
        car->SetCarNo(Json_Value_To_String(obj["carNo"]));
        car->SetCarColor(Json_Value_To_String(obj["carNoColor"]));
        car->SetChannelSum(Json_Value_To_String(obj["channelSum"]));
        car->SetIccid(Json_Value_To_String(obj["iccid"]));
        car->SetId(id);
        car->SetStatACC(Json_Value_To_String(obj["statAcc"]));
        car->SetStatus(Json_Value_To_String(obj["status"]));
        car->SetMotorcadeId(motorcadeId);
        car->AutoSetChannleList(); //自适应设置通道列表
        group->AddCar(id, car);
    }

    SetCarTree(mapGroup);

    m_mapGroup = std::move(mapGroup);

    UpdateCarListForSearch(m_mapGroup);
}

// 现在是所有的车队，将车队都加载到树状图中，同时处理一个分组包含多个子分组的情况
void CarSidebar::SetCarTree(QMap<QString, CarGroup*>& mapGroup)
{
    ui->treeWidget_car->reset();

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
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget_car);
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

void CarSidebar::ClearCarListForSearch()
{
    m_search_completer->setModel(nullptr);
}

void CarSidebar::UpdateCarListForSearch(QMap<QString, CarGroup*>& mapGroup)
{
    ClearCarListForSearch();
    QStringList list;
    UpdateCarListForSearch(mapGroup, list);
    QStringListModel* model = new QStringListModel(list, m_search_completer); //设置父对象为m_search_completer时，在setModel函数中会delete旧的model
    m_search_completer->setModel(model);
}

void CarSidebar::UpdateCarListForSearch(QMap<QString, CarGroup*>& mapGroup, QStringList& list)
{
    for (const auto& group : mapGroup) {
        UpdateCarListForSearch(*group, list);
    }
}

void CarSidebar::UpdateCarListForSearch(const CarGroup& group, QStringList& list)
{
    if (group.EmptyGroup()) {
        const auto& mapCar = group.GetCarMap();
        for (const auto& car : mapCar) {
            const auto& car_no = car->GetCarNo();
            list.append(car_no);
        }
    } else {
        const auto& mapGroup = group.GetGroupMap();
        for (const auto& iter : mapGroup) {
            UpdateCarListForSearch(*iter, list);
        }
    }
}

void CarSidebar::FindAndJumpFromTree(const QString& car_no)
{
    if (car_no.isEmpty()) {
        return;
    }
    // 如果和当前一致，则不需要跳转
    if (ui->treeWidget_car->currentItem()) {
        QString cur_car_no = ui->treeWidget_car->currentItem()->text(0);
        qDebug() << __FUNCTION__ << ",car_no:" << car_no << ",cur_car_no:" << cur_car_no << "\n";
        if (cur_car_no == car_no) {
            return;
        }
        ui->treeWidget_car->currentItem()->setSelected(false);
    }

    // 遍历车辆树，跳转到对应位置
    QTreeWidgetItemIterator iter(ui->treeWidget_car);
    while (*iter) {
        if ((*iter)->text(0) == car_no) {
            ui->treeWidget_car->setCurrentItem(*iter);
            break;
        }
        ++iter;
    }
    if (ui->treeWidget_car->currentItem()) {
        ui->treeWidget_car->scrollToItem(ui->treeWidget_car->currentItem());
    }
}

void CarSidebar::OpenOrCloseVideo(const QString& device_id)
{
    QString* pDeviceId = new QString(device_id);

    if (m_setOpeningVideo.find(device_id) != m_setOpeningVideo.end()) {
        m_setOpeningVideo.remove(device_id);
        emit sig_close_video(pDeviceId);
    } else {
        // 发送信号，获取对应通道的视频
        m_setOpeningVideo.insert(device_id);
        emit sig_open_video(pDeviceId);
    }
}

void CarSidebar::slot_http_finished(QByteArray* array)
{
    qDebug() << __FUNCTION__ << ",get data len:" << array->length() << "\n"; // << ",data:" << QString(*array) << "\n";
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
    qDebug() << __FUNCTION__ << "\n";
    // 过滤非device_id的item
    const auto& name = item->text(column);
    if (name.length() != 14) {
        return;
    }
    for (int i = 0; i < name.length(); i++) {
        if (!name[i].isDigit())
            return;
    }

    qDebug() << __FUNCTION__ << ",column:" << column << ", item name:" << name << "\n";
    OpenOrCloseVideo(name);
}

void CarSidebar::on_pushButton_search_clicked()
{
    // 搜索
    qDebug() << __FUNCTION__ << "\n";
    const QString car_no = ui->lineEdit_search_input->text();
    FindAndJumpFromTree(car_no);
}

void CarSidebar::on_lineEdit_search_input_editingFinished()
{
    qDebug() << __FUNCTION__ << "\n";
    const QString car_no = ui->lineEdit_search_input->text();
    FindAndJumpFromTree(car_no);
}

void CarSidebar::on_pushButton_test_1_clicked()
{
    // 测试车辆
    QString default_name = "01395221031201";

    OpenOrCloseVideo(default_name);
}

void CarSidebar::on_pushButton_test_2_clicked()
{ // 测试车辆
    QString default_name = "01395221031202";

    OpenOrCloseVideo(default_name);
}

void CarSidebar::on_pushButton_test_3_clicked()
{ // 测试车辆
    QString default_name = "01395221031203";

    OpenOrCloseVideo(default_name);
}

void CarSidebar::on_pushButton_test_4_clicked()
{ // 测试车辆
    QString default_name = "01395221031204";

    OpenOrCloseVideo(default_name);
}

void CarSidebar::on_pushButton_test_5_clicked()
{ // 测试车辆
    QString default_name = "01395221031205";

    OpenOrCloseVideo(default_name);
}

void CarSidebar::on_pushButton_test_6_clicked()
{ // 测试车辆
    QString default_name = "01395221031206";

    OpenOrCloseVideo(default_name);
}
