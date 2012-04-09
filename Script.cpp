#include "Script.h"




QScriptValue toQFileInfo(QScriptEngine *e, const QFileInfo &info) {
    return e->newVariant(info.absoluteFilePath());
}

void fromQFileInfo(const QScriptValue &v, QFileInfo &info) {
    info = QFileInfo(v.toString());
}

//http://www.qtcentre.org/threads/33355-Use-QObject-with-QtScript
QScriptValue constructTag(QScriptContext *context, QScriptEngine *engine){
    if (!context->isCalledAsConstructor()){
        return context->throwError(QScriptContext::SyntaxError, "please use the 'new' operator (var tag = new Tag(fileName);)");
    }
    if(context->argumentCount()!=1 ){
        return context->throwError("1 argument required (Full filename/string): var tag = new Tag(fileName);");
    }
    QString file = context->argument(0).toString();
    // store the shared pointer in the script object that we are constructing
    //return engine->newVariant(context->thisObject(), qVariantFromValue(new Tag(file)));
    TagItem *tag = new TagItem(file);
    return engine->newQObject(tag);
}


QScriptValue scriptGetDirContent(QScriptContext *context, QScriptEngine *engine){

    QList<QFileInfo> info;
    int nargin=context->argumentCount();
    if( nargin>4 || nargin<2 ){
        return context->throwError("2-4 arguments required. QStringList getDirContent( const QString &path, const QStringList &extensions, bool includeSubFolders=true, bool hiddenFiles=true ) ");
    }
    QString path = context->argument(0).toString();
    QScriptValue array = context->argument(1);
    QStringList extensions;
    qScriptValueToSequence( array, extensions );
    bool includeSubFolders = true;
    bool hiddenFiles = true;
    if(nargin>2){
        includeSubFolders = context->argument(2).toBool();
    }
    if(nargin>3){
        hiddenFiles = context->argument(3).toBool();
    }
    info = Global::getDirContent( path, extensions, includeSubFolders, hiddenFiles );

    return engine->toScriptValue(info);
}
