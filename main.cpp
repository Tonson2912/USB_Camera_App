#include "IrisCameraDebugApplication.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IrisCameraDebugApplicationWindow w;
    w.show();
    return a.exec();
}
