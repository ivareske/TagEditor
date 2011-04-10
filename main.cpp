#include <QApplication>
#include "TagEditor.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    
    TagEditor tag;
    tag.show();
    return app.exec();
}
