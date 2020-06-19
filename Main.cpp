#include <QApplication>
#include "ImuProcesserWidget.h"
int main(int argc, char *argv[])
{    
    QApplication imuprocess(argc, argv);
    QApplication::setStyle("fusion"); //"windows", "windowsvista", "fusion", or "macintosh"
    SetConsoleTitleA("IMU Program");
    ImuProcesserWidget windows(nullptr,false);
    windows.show();
    return imuprocess.exec();
}
