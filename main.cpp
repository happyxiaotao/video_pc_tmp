#include "car_video_view/carvideoplayer.h"
#include "videoview.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    //  VideoView view;
    // view.show();

    CarVideoPlayer player;
    player.show();

    return a.exec();
}
