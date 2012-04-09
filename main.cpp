#include <QApplication>
#include "TagEditor.h"

void usage();


int main(int argc, char *argv[]){    

    QApplication app(argc, argv);
    app.setApplicationVersion(APP_VERSION);
    app.setApplicationName(APP_NAME);

    QStringList arguments = QApplication::arguments();
    //remove application name if that exists as argument
    if(arguments.size()>0){
        QString tmp = arguments[0];
        if(tmp.replace("/","\\")==qApp->applicationFilePath().replace("/","\\")){
            arguments.removeAt(0);
        }
    }


    if(arguments.size()>1){
        usage();
        return 0;
    }
    TagEditor tagEditor;
    if(arguments.size()==1){
        QString script = arguments[0];
        QFile f(script);
        if(f.exists()){
            if(!f.open(QFile::ReadOnly | QFile::Text)){
                qDebug()<<"Could not open file "<<script;
                return 0;
            }
            script = f.readAll();
            f.close();
        }
        return tagEditor.runScript(script,false);
    }
    tagEditor.show();
    return app.exec();
}

void usage(){
    qDebug()<<"Command line usage:";
    qDebug()<<"'"<<qApp->applicationName()<<" scriptFile.js'";
    qDebug()<<"'"<<qApp->applicationName()<<" script' (where script is a string/javascript)";
}

