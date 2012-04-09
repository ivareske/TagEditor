#ifndef SCRIPT_H
#define SCRIPT_H



#include <QtGui>
#include "TagItem.h"
#include <QtScript>
#include "metaTypes.h"



QScriptValue toQFileInfo(QScriptEngine *e, const QFileInfo &info);
void fromQFileInfo(const QScriptValue &v, QFileInfo &info);
QScriptValue constructTag(QScriptContext *context, QScriptEngine *engine);








#endif // SCRIPT_H
