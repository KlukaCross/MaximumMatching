#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(300, 200);
    w.show();
    return QApplication::exec();
}
