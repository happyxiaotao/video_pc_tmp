#include "MainForm/mainform.h"
#include "showmanager.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    ShowManager show_manager;
    show_manager.Start();

    // MainForm mainform;
    // mainform.show();

    return a.exec();
}
