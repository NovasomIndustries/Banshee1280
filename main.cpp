#include "banshee1280.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Banshee1280 w;
    w.show();

    return a.exec();
}
