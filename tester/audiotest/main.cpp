#include "audiotest.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    audiotest w;
    w.show();

    return a.exec();
}
