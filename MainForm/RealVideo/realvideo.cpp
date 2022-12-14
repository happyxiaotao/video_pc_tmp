#include "realvideo.h"
#include "ui_realvideo.h"

RealVideo::RealVideo(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::RealVideo)
{
    ui->setupUi(this);

    connect(ui->widget_real_video_sidebar, &CarSidebar::sig_open_video, ui->widget_real_video_body, &RealVideoBody::slot_open_video);
    connect(ui->widget_real_video_sidebar, &CarSidebar::sig_close_video, ui->widget_real_video_body, &RealVideoBody::slot_close_video);
    connect(ui->widget_real_video_body, &RealVideoBody::sig_close_all, ui->widget_real_video_sidebar, &CarSidebar::slot_close_all);
    connect(ui->widget_real_video_body, &RealVideoBody::sig_close_video, ui->widget_real_video_sidebar, &CarSidebar::slot_close_video);

    connect(ui->widget_real_video_sidebar, &CarSidebar::sig_update_car_position, ui->widget_real_video_map, &ShowMap::slot_update_car_position);

    ui->widget_real_video_sidebar->UpdateCarTree(); //更新车辆树
}

RealVideo::~RealVideo()
{
    disconnect(ui->widget_real_video_sidebar, &CarSidebar::sig_open_video, ui->widget_real_video_body, &RealVideoBody::slot_open_video);
    disconnect(ui->widget_real_video_sidebar, &CarSidebar::sig_close_video, ui->widget_real_video_body, &RealVideoBody::slot_close_video);
    disconnect(ui->widget_real_video_body, &RealVideoBody::sig_close_all, ui->widget_real_video_sidebar, &CarSidebar::slot_close_all);
    disconnect(ui->widget_real_video_body, &RealVideoBody::sig_close_video, ui->widget_real_video_sidebar, &CarSidebar::slot_close_video);

    disconnect(ui->widget_real_video_sidebar, &CarSidebar::sig_update_car_position, ui->widget_real_video_map, &ShowMap::slot_update_car_position);

    delete ui;
}
