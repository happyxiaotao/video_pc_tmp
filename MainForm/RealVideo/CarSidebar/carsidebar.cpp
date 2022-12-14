#include "carsidebar.h"
#include "../Common/ini_config.h"
#include "ui_carsidebar.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QNetworkCookie>
#include <QStringListModel>

QString CarSidebar::s_resource_dir = QDir::currentPath() + "/resource";
QString CarSidebar::s_text_title;
QString CarSidebar::s_text_get_car_real_video_tree_failed;
QString CarSidebar::s_text_post_car_real_video_position_failed;
QString CarSidebar::s_text_acc_open;
QString CarSidebar::s_text_acc_close;
QString CarSidebar::s_text_open;
QString CarSidebar::s_text_close;
QString CarSidebar::s_icon_path_car_status_0;
QString CarSidebar::s_icon_path_car_status_1;
QString CarSidebar::s_icon_path_car_status_2;
QString CarSidebar::s_icon_path_car_status_3;
QString CarSidebar::s_icon_path_car_status_4;
QString CarSidebar::s_icon_path_car_status_5;
QString CarSidebar::s_icon_path_car_status_6;
QString CarSidebar::s_icon_path_car_status_7;
QString CarSidebar::s_icon_path_car_status_8;
QString CarSidebar::s_icon_path_car_status_9;
QString CarSidebar::s_icon_path_car_status_10;
QString CarSidebar::s_icon_path_car_status_11;
QString CarSidebar::s_icon_path_channel_type_1;
QString CarSidebar::s_icon_path_channel_type_2;
QString CarSidebar::s_icon_path_channel_type_3;
QString CarSidebar::s_icon_path_channel_type_4;

CarSidebar::CarSidebar(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CarSidebar)
    , m_http_client_get_real_video_tree(nullptr)
    , m_http_client_post_real_video_position(nullptr)
    , m_search_completer(nullptr)
{
    ui->setupUi(this);

    if (s_text_get_car_real_video_tree_failed.isEmpty()) {
        s_text_title = QString::fromStdString(g_ini->Get("text", "system_title", ""));
        s_text_get_car_real_video_tree_failed = QString::fromStdString(g_ini->Get("text", "get_car_real_video_tree_failed", ""));
        s_text_post_car_real_video_position_failed = QString::fromStdString(g_ini->Get("text", "post_car_real_video_position_failed", ""));
        s_text_acc_open = QString::fromStdString(g_ini->Get("text", "acc_open", ""));
        s_text_acc_close = QString::fromStdString(g_ini->Get("text", "acc_close", ""));
        s_text_open = QString::fromStdString(g_ini->Get("text", "open", ""));
        s_text_close = QString::fromStdString(g_ini->Get("text", "close", ""));
        s_icon_path_car_status_0 = s_resource_dir + "/car/0.png";
        s_icon_path_car_status_1 = s_resource_dir + "/car/1.png";
        s_icon_path_car_status_2 = s_resource_dir + "/car/2.png";
        s_icon_path_car_status_3 = s_resource_dir + "/car/3.png";
        s_icon_path_car_status_4 = s_resource_dir + "/car/4.png";
        s_icon_path_car_status_5 = s_resource_dir + "/car/5.png";
        s_icon_path_car_status_6 = s_resource_dir + "/car/6.png";
        s_icon_path_car_status_7 = s_resource_dir + "/car/7.png";
        s_icon_path_car_status_8 = s_resource_dir + "/car/8.png";
        s_icon_path_car_status_9 = s_resource_dir + "/car/9.png";
        s_icon_path_car_status_10 = s_resource_dir + "/car/10.png";
        s_icon_path_car_status_11 = s_resource_dir + "/car/11.png";
        s_icon_path_channel_type_1 = s_resource_dir + "/video/video1.png";
        s_icon_path_channel_type_2 = s_resource_dir + "/video/video2.png";
        s_icon_path_channel_type_3 = s_resource_dir + "/video/video3.png";
        s_icon_path_channel_type_4 = s_resource_dir + "/video/video4.png";
    }

    m_http_client_get_real_video_tree = new HttpClient(this);
    connect(m_http_client_get_real_video_tree, &HttpClient::sig_handler_msg, this, &CarSidebar::slot_http_finished_get_real_video_tree);

    m_http_client_post_real_video_position = new HttpClient(this);
    connect(m_http_client_post_real_video_position, &HttpClient::sig_handler_msg, this, &CarSidebar::slot_http_finished_post_real_video_position);

    m_search_completer = new QCompleter(ui->lineEdit_search_input); // ??????????????????ui->lineEdit?????????????????????delete??????
    m_search_completer->setCaseSensitivity(Qt::CaseInsensitive); //???????????????
    m_search_completer->setFilterMode(Qt::MatchFlags::enum_type::MatchContains);
    ui->lineEdit_search_input->setCompleter(m_search_completer); //??????completer

    // ????????????????????????????????????????????????????????????
    QString style = QString("QTreeView::branch:has-children:!has-siblings:closed,\
    QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: url(%1/tree/plus.png);}\
    QTreeView::branch:open:has-children:!has-siblings,\
    QTreeView::branch:open:has-children:has-siblings{border-image: none; image: url(%1/tree/minus.png);}")
                        .arg(s_resource_dir);
    ui->treeWidget_car->setStyleSheet(style);

    // ???????????????
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &CarSidebar::slot_on_timer);
}

CarSidebar::~CarSidebar()
{
    disconnect(m_http_client_get_real_video_tree, &HttpClient::sig_handler_msg, this, &CarSidebar::slot_http_finished_get_real_video_tree);
    delete m_http_client_get_real_video_tree;
    connect(m_http_client_post_real_video_position, &HttpClient::sig_handler_msg, this, &CarSidebar::slot_http_finished_post_real_video_position);
    delete m_http_client_post_real_video_position;

    ClearRealVideoTree();

    disconnect(m_timer, &QTimer::timeout, this, &CarSidebar::slot_on_timer);
    m_timer->deleteLater();

    delete ui;
}

void CarSidebar::UpdateCarTree()
{
    // ????????????????????????
    ClearRealVideoTree();
    GetRealVideoTree();
}

static QString Json_Value_To_String(const QJsonValue& value)
{
    if (value.isDouble()) {
        return QString("%1").arg(value.toDouble());
    } else if (value.isString()) {
        return value.toString();
    } else if (value.isBool()) {
        return QString("%1").arg(value.toBool());
    } else if (value.isNull()) {
        return QString();
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

    // ???????????????map
    QMap<QString, CarGroup*> mapGroup;
    for (auto iter = m_json_array.begin(); iter != m_json_array.end(); ++iter) {
        obj = iter->toObject();
        id = Json_Value_To_String((obj["id"]));

        //??????????????????????????????????????????????????????????????????
        if (id == "4") {
            continue;
        }

        CarGroup* group = new CarGroup();
        group->SetId(id);

        group->SetName(Json_Value_To_String(obj["name"]));
        group->SetPid(Json_Value_To_String(obj["pid"]));

        mapGroup[id] = group;
    }

    // ????????????????????????????????????????????????????????????????????????
    QString motorcadeId;
    for (auto iter = c_json_array.begin(); iter != c_json_array.end(); ++iter) {
        obj = iter->toObject();

        motorcadeId = Json_Value_To_String(obj["motorcadeId"]);
        // qDebug() << __FUNCTION__ << ",motorcadeId:" << motorcadeId << "\n";

        auto group_iter = mapGroup.find(motorcadeId);
        if (group_iter == mapGroup.end())
            continue;

        id = Json_Value_To_String(obj["id"]);
        if (id == "9663") {
            qDebug() << "\n";
        }

        //?????????????????????????????????equipmentType=1???????????????=2???????????? =3AI??????
        auto equipmentType = Json_Value_To_String(obj["equipmentType"]);
        if (equipmentType.isEmpty() || equipmentType == "1") {
            continue;
        }

        // ?????????????????????0???????????????
        auto channelSum = Json_Value_To_String(obj["channelSum"]);
        if (channelSum.isEmpty() || channelSum == "0") {
            continue;
        }

        auto& group = group_iter.value();
        CarInfo* car = new CarInfo();

        car->SetAreaName(Json_Value_To_String(obj["areaName"]));
        car->SetCarNo(Json_Value_To_String(obj["carNo"]));
        car->SetCarColor(Json_Value_To_String(obj["carNoColor"]));
        car->SetChannelSum(channelSum);
        car->SetIccid(Json_Value_To_String(obj["iccid"]));
        car->SetId(id);
        car->SetStatACC(Json_Value_To_String(obj["statAcc"]));
        car->SetStatus(Json_Value_To_String(obj["status"]));
        car->SetMotorcadeId(motorcadeId);
        car->SetChannelType(Json_Value_To_String(obj["channelType"]));
        car->AutoSetChannleList(); //???????????????????????????
        group->AddCar(id, car);
    }

    SetCarTree(mapGroup);

    m_mapGroup = std::move(mapGroup);

    UpdateCarListForSearch(m_mapGroup);
}
#define ADD_LIST_WIDGET_ITEM(list, item, name) \
    do {                                       \
        item->setText(0, name);                \
        list.append(item);                     \
    } while (0)

// ?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
void CarSidebar::SetCarTree(QMap<QString, CarGroup*>& mapGroup)
{
    ui->treeWidget_car->reset();

    QList<QTreeWidgetItem*> list; // QTreeWidgetItem*??????????????????TreeCarInfoItem

    // ???????????????????????????Group??????????????????Group??????
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

    // ?????????mapGroup??????????????????????????????
    for (auto& iter : mapGroup) {
        TreeCarInfoItem* item = new TreeCarInfoItem(ui->treeWidget_car);
        item->SetGroup(iter);
        // ???????????????
        SetCarTree(*iter, item);

        int nTotalCount;
        int nOnLineCount;
        int nAccOpenCount;
        int nAccCloseCount;
        item->GetGroupOnLineCount(nTotalCount, nOnLineCount, nAccOpenCount, nAccCloseCount);

        QString strInfo = QString("%1[%2/%3 ACC:%4%5 %6%7 ]").arg(iter->GetName()).arg(nOnLineCount).arg(nTotalCount).arg(s_text_open).arg(nAccOpenCount).arg(s_text_close).arg(nAccCloseCount);

        ADD_LIST_WIDGET_ITEM(list, item, strInfo);
    }
    qDebug() << "list count=" << list.count() << "\n";
    ui->treeWidget_car->addTopLevelItems(list);
    // ??????
    ui->treeWidget_car->sortItems(0, Qt::SortOrder::AscendingOrder); // ????????????????????????????????????????????????????????????????????????
    //  // qDeleteAll(list); //????????????qDeleteAll???list????????????????????????????????????UI??????????????????
    list.clear();
}

void CarSidebar::SetCarTree(const CarGroup& parent, TreeCarInfoItem* root)
{
    // ?????????????????????????????????????????????????????????????????????????????????-????????????????????????????????????????????????????????????

    QList<QTreeWidgetItem*> list;

    if (parent.EmptyGroup()) {

        int nOffLineCount = 0;
        int nTotalCount = parent.GetCarMap().size();
        int nAccOpenCount = 0;
        int nAccCloseCount = 0;

        // ??????????????????????????????
        const auto& mapCar = parent.GetCarMap();
        for (auto& iter : mapCar) {
            TreeCarInfoItem* item = new TreeCarInfoItem();

            // ???????????????????????????????????????ACC?????????????????????????????????

            QString car_no = iter->GetCarNo();
            QString strShow = car_no;
            strShow.append(" ");
            const QString& acc = iter->GetAcc();
            if (acc == "0") {
                strShow.append(s_text_acc_close);
                nAccCloseCount++;
            } else {
                strShow.append(s_text_acc_open);
                nAccOpenCount++;
            }

            // ??????-???????????????-???????????????-??????
            const QString& status = iter->GetStatus();
            if (status == "0" || status == "1" || status == "11") {
                item->setForeground(0, Qt::gray);
                nOffLineCount++;
            } else if (status == "2" || status == "5" || status == "8" || status == "9") {
                item->setForeground(0, Qt::red);
                if (status == "2") {
                    nOffLineCount++;
                }
            } else {
                item->setForeground(0, Qt::blue);
            }

            // ???????????????????????????
            int nStatus = status.toInt();
            QString icon_path = GetIconPathByCarStatus(nStatus);
            item->setIcon(0, QIcon(icon_path));

            const QList<CarChannel*>& listChannel = iter->GetChannelList();
            item->SetCarInfo(iter);
            QString car_id = iter->GetId();
            item->SetCarId(car_id);
            SetCarTree(listChannel, item, car_id, car_no);
            ADD_LIST_WIDGET_ITEM(list, item, strShow);
        }

        // ?????????????????????ACC??????ACC?????????
        root->SetGroupOnLineCount(nTotalCount, nTotalCount - nOffLineCount, nAccOpenCount, nAccCloseCount);

    } else {
        int group_nTotalCount = 0;
        int group_nOnLineCount = 0;
        int group_nAccOpenCount = 0;
        int group_nAccCloseCount = 0;

        const auto& mapGroup = parent.GetGroupMap();
        for (auto& iter : mapGroup) {
            TreeCarInfoItem* item = new TreeCarInfoItem();
            item->SetGroup(iter);
            SetCarTree(*iter, item);

            int nTotalCount;
            int nOnLineCount;
            int nAccOpenCount;
            int nAccCloseCount;
            item->GetGroupOnLineCount(nTotalCount, nOnLineCount, nAccOpenCount, nAccCloseCount);

            QString strInfo = QString("%1[%2/%3 ACC:%4%5 %6%7 ]").arg(iter->GetName()).arg(nOnLineCount).arg(nTotalCount).arg(s_text_open).arg(nAccOpenCount).arg(s_text_close).arg(nAccCloseCount);

            ADD_LIST_WIDGET_ITEM(list, item, strInfo);

            group_nTotalCount += nTotalCount;
            group_nOnLineCount += nOnLineCount;
            group_nAccOpenCount += nAccOpenCount;
            group_nAccCloseCount += nAccCloseCount;
        }

        // ?????????????????????ACC??????ACC?????????
        root->SetGroupOnLineCount(group_nTotalCount, group_nOnLineCount, group_nAccOpenCount, group_nAccCloseCount);
    }
    root->addChildren(list);
    list.clear();
}

void CarSidebar::SetCarTree(const QList<CarChannel*>& listChannel, TreeCarInfoItem* root, const QString& car_id, const QString& car_no)
{
    QList<QTreeWidgetItem*> list;
    for (auto& iter : listChannel) {
        TreeCarInfoItem* item = new TreeCarInfoItem();
        item->SetCarChannel(iter);
        item->SetCarId(car_id);
        item->SetCarChannelCarNo(car_no);

        ADD_LIST_WIDGET_ITEM(list, item, iter->GetAliasName());

        // ??????????????????
        const auto& type = iter->GetChannelType();
        QString icon_path = GetIconPathByChannelType(type);
        item->setIcon(0, QIcon(icon_path));
    }
    root->addChildren(list);
    root->sortChildren(0, Qt::SortOrder::AscendingOrder);
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
    QStringListModel* model = new QStringListModel(list, m_search_completer); //??????????????????m_search_completer?????????setModel????????????delete??????model
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
    // ??????????????????????????????????????????
    if (ui->treeWidget_car->currentItem()) {
        QString cur_car_no = ui->treeWidget_car->currentItem()->text(0);
        qDebug() << __FUNCTION__ << ",car_no:" << car_no << ",cur_car_no:" << cur_car_no << "\n";
        if (cur_car_no == car_no) {
            return;
        }
        ui->treeWidget_car->currentItem()->setSelected(false);
    }

    // ???????????????????????????????????????
    QTreeWidgetItemIterator iter(ui->treeWidget_car);
    while (*iter) {

        TreeCarInfoItem* car_item = dynamic_cast<TreeCarInfoItem*>(*iter);
        if (car_item->IsCarInfo() && car_item->GetCarNo() == car_no) {
            ui->treeWidget_car->setCurrentItem(*iter);
            break;
        }

        ++iter;
    }
    if (ui->treeWidget_car->currentItem()) {
        ui->treeWidget_car->scrollToItem(ui->treeWidget_car->currentItem());
    }
}

void CarSidebar::OpenOrCloseVideo(const QString& device_id, const QString& carId, const QString& car_no, const QString& channel_alias)
{
    QString* pDeviceId = new QString(device_id);
    QString channel_show_alias = car_no + "-" + channel_alias;
    QString* pChannelAlias = new QString();
    pChannelAlias->swap(channel_show_alias);

    if (m_setOpeningVideo.find(device_id) != m_setOpeningVideo.end()) {
        m_setOpeningVideo.remove(device_id);
        emit sig_close_video(pDeviceId);

        if (device_id == m_last_car_device_id) {
            ClearLastCarInfo();
        }

    } else {
        // ??????????????????????????????????????????
        m_setOpeningVideo.insert(device_id);

        // ????????????
        emit sig_open_video(pDeviceId, pChannelAlias);

        // ???????????????????????????????????????
        PostRealVideoPosition(carId);

        SetLastCarInfo(device_id, carId);
    }
}

void CarSidebar::ClearRealVideoTree()
{
    for (auto iter = m_mapGroup.begin(); iter != m_mapGroup.end(); ++iter) {
        delete iter.value();
    }
    m_mapGroup.clear();

    ClearCarListForSearch();

    ui->treeWidget_car->clear();
}

void CarSidebar::GetRealVideoTree()
{
    // ??????http??????
    QNetworkRequest request;

    std::string https = g_ini->Get("api", "https", "");
    std::string get_real_video_tree = g_ini->Get("api", "get_real_video_tree", "");

    QUrl url(QString::fromStdString(https + get_real_video_tree));
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");

    QString token = m_http_client_get_real_video_tree->GetToken();
    QString cookie = "token=" + token;
    request.setRawHeader("cookie", QByteArray(cookie.toStdString().c_str()));
    request.setRawHeader("token", QByteArray(token.toStdString().c_str()));
    m_http_client_get_real_video_tree->Get(request);
}

void CarSidebar::PostRealVideoPosition(const QString& device_id)
{
    if (device_id.isEmpty()) {
        return;
    }

    StopTimer();

    QNetworkRequest request;
    std::string https = g_ini->Get("api", "https", "");
    std::string post_real_video_position = g_ini->Get("api", "post_real_video_position", "");
    QUrl url(QString::fromStdString(https + post_real_video_position));
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    QString token = m_http_client_post_real_video_position->GetToken();
    QString cookie = "token=" + token;
    request.setRawHeader("cookie", QByteArray(cookie.toStdString().c_str()));
    request.setRawHeader("token", QByteArray(token.toStdString().c_str()));

    QJsonDocument doc;
    QJsonValue jsonCarId(device_id);
    QJsonArray array;
    array.append(jsonCarId);
    QJsonObject obj;
    obj["carIds"] = array;
    doc.setObject(obj);
    auto data = doc.toJson();
    qDebug() << __FUNCTION__ << "json data:" << data << "\n";
    m_http_client_post_real_video_position->PostJson(request, data);
}

void CarSidebar::SetLastCarInfo(const QString& device_id, const QString& car_id)
{
    m_last_car_device_id = device_id;
    m_last_car_id = car_id;
}

void CarSidebar::ClearLastCarInfo()
{
    m_last_car_device_id.clear();
    m_last_car_id.clear();
}

void CarSidebar::StartTimer(int ms)
{
    m_timer->start(ms);
}

void CarSidebar::StopTimer()
{
    m_timer->stop();
}

QString CarSidebar::GetIconPathByCarStatus(int nCarStatus)
{
    switch (nCarStatus) {
    case 0:
        return s_icon_path_car_status_0;
    case 1:
        return s_icon_path_car_status_1;
    case 2:
        return s_icon_path_car_status_2;
    case 3:
        return s_icon_path_car_status_3;
    case 4:
        return s_icon_path_car_status_4;
    case 5:
        return s_icon_path_car_status_5;
    case 6:
        return s_icon_path_car_status_6;
    case 7:
        return s_icon_path_car_status_7;
    case 8:
        return s_icon_path_car_status_8;
    case 9:
        return s_icon_path_car_status_9;
    case 10:
        return s_icon_path_car_status_10;
    case 11:
        return s_icon_path_car_status_11;
    default:
        return s_icon_path_car_status_10; //????????????????????????
    }
}

QString CarSidebar::GetIconPathByChannelType(const QJsonArray& jsonArray)
{
    const QJsonValue value0("0");
    const QJsonValue value1("1");
    const QJsonValue value2("2");
    const QJsonValue value3("3");

    if (jsonArray.size() == 0) {
        return s_icon_path_channel_type_1;
    } else if (jsonArray.contains(value1) || jsonArray.contains(value2)) {
        return s_icon_path_channel_type_4;
    } else if (jsonArray.contains(value3)) {
        return s_icon_path_channel_type_3;
    } else if (jsonArray.contains(value0)) {
        return s_icon_path_channel_type_2;
    }
    return "";
}

void CarSidebar::slot_http_finished_get_real_video_tree(QByteArray* array)
{
    qDebug() << __FUNCTION__ << ",get data len:" << array->length() << "\n";
    // qDebug() << __FUNCTION__ << ",data:" << QString(*array) << "\n";
    // ??????json?????????????????????
    QJsonParseError err_rpt;
    QJsonDocument doc = QJsonDocument::fromJson(*array, &err_rpt); // ?????????????????????JSON
    if (err_rpt.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, s_text_title, s_text_get_car_real_video_tree_failed);
        return;
    }
    auto code = doc["code"];
    auto data = doc["data"];
    if (code.isNull() || !code.isDouble() || code.toInt() != 200 || data.isNull()) {
        QMessageBox::warning(this, s_text_title, s_text_get_car_real_video_tree_failed);
        return;
    }
    const QJsonValue& c = data["c"];
    const QJsonValue& m = data["m"];
    SetCarTree(c, m);
}

void CarSidebar::slot_http_finished_post_real_video_position(QByteArray* array)
{
    qDebug() << __FUNCTION__ << ",get data len:" << array->length() << "\n";
    // qDebug() << __FUNCTION__ << ",data:" << QString(*array) << "\n";
    QJsonParseError err_rpt;
    QJsonDocument doc = QJsonDocument::fromJson(*array, &err_rpt); // ?????????????????????JSON
    if (err_rpt.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, s_text_title, s_text_post_car_real_video_position_failed);
        return;
    }
    auto code = doc["code"];
    auto data = doc["data"];
    if (code.isNull() || !code.isDouble() || code.toInt() != 200 || data.isNull()) {
        QMessageBox::warning(this, s_text_title, s_text_post_car_real_video_position_failed);
        return;
    }
    auto jsonArray = data.toArray();
    auto first = jsonArray.first();
    auto obj = first.toObject();

    auto glat = obj["glat"];
    auto glng = obj["glng"];
    qDebug() << __FUNCTION__ << "glat:" << glat << ",glng:" << glng << "\n";
    if (glat.isNull() || glng.isNull()) {
        QMessageBox::warning(this, s_text_title, s_text_post_car_real_video_position_failed);
        return;
    }
    // ????????????double???string??????????????????????????????string?????????double
    auto str_glat = Json_Value_To_String(glat);
    auto str_glng = Json_Value_To_String(glng);
    auto speed = Json_Value_To_String(obj["speed"]);
    auto car_no = Json_Value_To_String(obj["carNo"]);
    auto direction = Json_Value_To_String(obj["direction"]);
    auto carStatus = Json_Value_To_String(obj["carStatus"]);

    // ?????????????????????????????????    lat lng image text

    //??????????????????????????????????????????
    int nCarStatus = carStatus.toInt();
    QString image_path = GetIconPathByCarStatus(nCarStatus);
    // QString text = "<div class=\\\"mydiv\\\">"
    //                "<style>"
    //                "p.nowrap {white-space: nowrap;}"
    //                "div.mydiv {width:105px;height:25px; background-color:white; border-left: 2px solid blue; border-top:2px solid blue; border-bottom:2px solid blue; border-right : 2px solid blue}"
    //                "</style>"
    //                "<p class=\\\"nowrap\\\" align=\\\"center\\\"><font color=\\\"blue\\\">"
    //     + car_no + " " + speed + "km/h</font></p></div>";

    QString text = car_no + " " + speed + "km/h";

    emit sig_update_car_position(new QString(str_glat), new QString(str_glng), new QString(image_path), new QString(direction), new QString(text));
    // ???????????????????????????10s
    StartTimer(1000 * 10);
}

void CarSidebar::slot_on_timer()
{
    qDebug() << __FUNCTION__ << "\n";
}

void CarSidebar::slot_close_video(QString* device_id)
{
    m_setOpeningVideo.remove(*device_id);

    if (*device_id == m_last_car_device_id) {
        ClearLastCarInfo();
    }
    delete device_id;
}

void CarSidebar::slot_close_all()
{
    m_setOpeningVideo.clear();
    ClearLastCarInfo();
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

    // ?????????QTreeWidgetItem????????????TreeCarInfoItem?????????
    TreeCarInfoItem* tree_item = dynamic_cast<TreeCarInfoItem*>(item);
    if (!tree_item->IsCarChannel()) {
        return;
    }

    // ?????????device_id???item
    QString strDeviceId = tree_item->GetChannelDeviceId();
    if (strDeviceId.length() != 14) {
        return;
    }
    for (int i = 0; i < strDeviceId.length(); i++) {
        if (!strDeviceId[i].isDigit())
            return;
    }
    QString car_id = tree_item->GetCarId();
    QString channel_alias = tree_item->GetChannelAlias();
    QString car_no = tree_item->GetCarChannelCarNo();

    qDebug() << __FUNCTION__ << ",column:" << column << ", item name:" << strDeviceId << "\n";
    OpenOrCloseVideo(strDeviceId, car_id, car_no, channel_alias);
}

void CarSidebar::on_pushButton_search_clicked()
{
    // ??????
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

// ???????????????????????????????????????????????????????????????
void CarSidebar::on_lineEdit_search_input_textChanged(const QString& arg1)
{
    const QString car_no = ui->lineEdit_search_input->text();
    FindAndJumpFromTree(car_no);
}

// ???????????????
void CarSidebar::on_pushButton_clicked()
{
    UpdateCarTree();
}
