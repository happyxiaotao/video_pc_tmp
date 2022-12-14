#include "Common/Log.h"
#include "Common/ini_config.h"
#include "MainForm/mainform.h"
#include "showmanager.h"
#include <QApplication>
#include <QDir>

INIReader* g_ini;

int main(int argc, char* argv[])
{
    // // 下面的方法，可以使得，在高分辨率下，也有较好的显示效果。
    // #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    //     {
    //         QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //         QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //     }
    // #endif

    QString currentWorkDir
        = QDir::currentPath();
    std::string config_file = currentWorkDir.toStdString() + "/conf/client.ini";
    g_ini = new INIReader(config_file);

    spdlog::default_logger()->set_level(spdlog::level::trace);
    Info("current config file:{}", config_file);

    QApplication a(argc, argv);

    ShowManager show_manager;
    show_manager.Start();

    // MainForm mainform;
    // mainform.show();

    return a.exec();
}
