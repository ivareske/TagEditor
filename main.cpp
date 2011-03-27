#include <QApplication>
#include "tagEditor.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    
    tagEditor tag;
    tag.show();
    return app.exec();
}
