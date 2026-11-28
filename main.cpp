#include "mainwindow.h"

#include <QApplication>
#include <locale>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::locale::global(std::locale("C.UTF-8"));
    MainWindow w;
    w.show();
    return QApplication::exec();
}
