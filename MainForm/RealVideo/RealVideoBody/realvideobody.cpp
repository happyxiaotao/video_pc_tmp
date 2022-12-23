#include "realvideobody.h"
#include "ui_realvideobody.h"
#include "videoviewmanager.h"
#include <QDebug>
#include <QDir>
#include <QGridLayout>

QString RealVideoBody::s_resource_dir = QDir::currentPath() + "/resource";
RealVideoBody::RealVideoBody(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::RealVideoBody)
    , m_view_manager(nullptr)

{
    ui->setupUi(this);

    m_view_manager = new VideoViewManager(this, ui->widget_real_video_body);

    connect(m_view_manager, &VideoViewManager::sig_close_video, this, &RealVideoBody::slot_close_video_from_view_manager);

    QIcon icon;
    icon.addFile(s_resource_dir + QString("/real_video/body_full_screen.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->pushButton_real_video_body_full_screen->setIcon(icon);

    // 默认窗口3*3
    int index = 2;
    ui->comboBox_real_video_wincnt->setCurrentIndex(index);
    // WinCountType type = WinCountType::One_One;
    WinCountType type = WinCountType::Three_Three;
    m_view_manager->ChangeWinCount(type);
    // emit sig_update_max_view_count(9);//这个时候emit由于，在构造函数中，还未执行到绑定信号的操作。所以此时emit是不生效的。
    // 提供了 GetMaxCount()函数来代替

    // 默认满屏
    int view_index = 0;
    ui->comboBox_real_video_prop->setCurrentIndex(view_index);
    WinViewType view_type = WinViewType::kFullScreen;
    m_view_manager->ChangeWinView(view_type);
}

RealVideoBody::~RealVideoBody()
{
    disconnect(m_view_manager, &VideoViewManager::sig_close_video, this, &RealVideoBody::slot_close_video_from_view_manager);

    delete ui;
    delete m_view_manager;
}

int RealVideoBody::GetMaxCount()
{
    // 0 -- 1*1
    // 1 -- 2*2
    // 2 -- 3*3
    // 4 -- 4*4
    int index = ui->comboBox_real_video_wincnt->currentIndex();
    int max_count = 1;
    switch (index) {
    case 1:
        max_count = 4;
        break;
    case 2:
        max_count = 9;
        break;
    case 3:
        max_count = 16;
        break;
    default:
        break;
    }
    return max_count;
}

void RealVideoBody::slot_open_video(QString* _device_id, QString* _channel_alias)
{
    qDebug() << __FUNCTION__ << ",device_Id:" << *_device_id << "\n";
    QString device_id;
    device_id.swap(*_device_id);
    delete _device_id;

    QString channel_alias;
    channel_alias.swap(*_channel_alias);
    delete _channel_alias;

    CarVideoPlayer* player = m_view_manager->GetPlayer(device_id);
    if (player == nullptr) {
        qDebug() << __FUNCTION__ << ",get player nullptr, device_id:" << device_id << "\n";
        return;
    }

    player->SetDeviceId(device_id);
    player->open_video(device_id, channel_alias);

    //如果是单车播放，则需要删掉其他车辆
    m_view_manager->ReleaseOtherCarIfNeed(device_id);
}

void RealVideoBody::slot_close_video(QString* _device_id)
{
    qDebug() << __FUNCTION__ << ",device_Id:" << *_device_id << "\n";
    QString device_id;
    device_id.swap(*_device_id);
    delete _device_id;

    // 关闭对应连接
    CarVideoPlayer* player = m_view_manager->GetPlayer(device_id);
    if (player == nullptr) {
        qDebug() << "not found player,device_id:" << device_id << "\n";
        return;
    }
    player->close_video();
    m_view_manager->ReleasePlayer(player);
}

void RealVideoBody::slot_close_video_from_view_manager(QString* _device_id)
{
    emit sig_close_video(_device_id);
}

void RealVideoBody::on_pushButton_all_close_clicked()
{
    // 关闭所有连接
    m_view_manager->CloseAll();
    emit sig_close_all();
}

// 点击全屏
void RealVideoBody::on_pushButton_real_video_body_full_screen_clicked()
{
    ui->widget_real_video_body->FullOrNormalScreen();
}

void RealVideoBody::on_comboBox_real_video_wincnt_currentIndexChanged(int index)
{
    // 0 -- 1*1
    // 1 -- 2*2
    // 2 -- 3*3
    // 4 -- 4*4
    int max_count = 1;

    qDebug() << __FUNCTION__ << ",index=" << index << "\n";
    WinCountType type = WinCountType::One_One;
    switch (index) {
    case 1:
        type = WinCountType::Two_Two;
        max_count = 4;
        break;
    case 2:
        type = WinCountType::Three_Three;
        max_count = 9;
        break;
    case 3:
        type = WinCountType::Four_Four;
        max_count = 16;
        break;
    default:
        break;
    }
    m_view_manager->ChangeWinCount(type);
    emit sig_update_max_view_count(max_count);
}

void RealVideoBody::on_comboBox_real_video_prop_currentIndexChanged(int index)
{
    // 0 --  满屏播放
    // 1 --  2:1显示
    // 2 --  16:9显示
    // 3 --  4:3显示
    // 4 --  1:1显示
    // 5 --  3:4显示
    // 6 --  1:2显示
    // 7 --  9:16显示

    qDebug() << __FUNCTION__ << ",index=" << index << "\n";
    WinViewType type = WinViewType::kFullScreen;
    switch (index) {
    case 1:
        type = WinViewType::k2To1;
        break;
    case 2:
        type = WinViewType::k16To9;
        break;
    case 3:
        type = WinViewType::k4To3;
        break;
    case 4:
        type = WinViewType::k1To1;
        break;
    case 5:
        type = WinViewType::k3To4;
        break;
    case 6:
        type = WinViewType::k1To2;
        break;
    case 7:
        type = WinViewType::k9To16;
        break;
    default:
        break;
    }
    m_view_manager->ChangeWinView(type);
}

void RealVideoBody::on_comboBox_real_video_car_count_currentIndexChanged(int index)
{
    // 0 -- 多车播放
    // 1 -- 单车播放
    if (index == 0) {
        m_view_manager->ChangeWinCarPlayType(WinCarPlayType::kMultiCar);
    } else if (index == 1) {
        m_view_manager->ChangeWinCarPlayType(WinCarPlayType::kSigCar);
    }
}
