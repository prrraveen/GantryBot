#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.setFixedSize(440,300);
    w.setWindowTitle("Arduino Control");
    w.show();

    return a.exec();
}
