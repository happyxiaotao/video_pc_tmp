#include "showmanager.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    ShowManager show_manager;
    show_manager.Start();

    return a.exec();
}
