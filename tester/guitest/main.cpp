#include "guitest.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    guitest w;
    w.show();

    return a.exec();
}
