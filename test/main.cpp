#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Register classes for queued signals-slots mechanism
    qRegisterMetaType<LkG5000*>("LkG5000*");
    
    Dialog w;
    w.show();
    
    return a.exec();
}
