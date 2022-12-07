#include "videoview.h"
#include "ui_videoview.h"

#include <QPixmap>
#include <QThread>

VideoView::VideoView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoView)
{
    ui->setupUi(this);

    m_jt1078_client = new Jt1078Client();
    QObject::connect(m_jt1078_client, &Jt1078Client::sig_update_video, this, &VideoView::slot_update_video);

    uint16_t port = 9521;
    // QHostAddress host("127.0.0.1");
    //    const std::string strIccid = "01035226431402"; //自测的包，10352264314-2.bin
    //    const std::string strIccid = "01380013899902"; // 自测的包，tcpdump.bin
    //  const std::string strIccid = "01234500055502"; // 自测的包，tbox2.bin
    //     const std::string strIccid = "04091037314203"; // 自测的包，对应车牌RPM767

    // const std::string strIccid = "06475361546202"; //线上的车。车牌P2D709通道2
    // const std::string strIccid = "06480992160102"; //线上的车，车牌PV2053
    // const std::string strIccid = "04095886586802"; //车牌RRD577   audio G726
    // const std::string strIccid = "04062002630103"; //车牌U33131   G726
    // const std::string strIccid = "04104980815802"; //车牌 RQH113 G726
    // const std::string strIccid = "06475481243802"; // 车牌RVS129  eG711A
    // const std::string strIccid = "04091037314203"; //车牌RPM767       adpcm
    // const std::string strIccid = "04052446839402"; // 车牌RDA569
    //  const std::string strIccid = "01395221031202"; // 测试车辆123123
    //  QHostAddress host("61.136.148.230"); //宜昌服务器
    //  m_jt1078_client->ConnectToHost(host, port, strIccid);
}

VideoView::~VideoView()
{
    QObject::disconnect(m_jt1078_client, &Jt1078Client::sig_update_video, this, &VideoView::slot_update_video);
    delete ui;
}

void VideoView::slot_update_video(QImage* img)
{
    ui->label_display->setPixmap(QPixmap::fromImage(*img));
    delete img;
}
