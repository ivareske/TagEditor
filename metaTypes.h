#ifndef METATYPES_H
#define METATYPES_H

#include <QtGui>
#include <QtScript>

/*
    Allows a metatype to be declared for a type containing a single comma.
    For example:
        Q_DECLARE_METATYPE2(QList<QPair<QByteArray,QByteArray> >)
 */
#define Q_DECLARE_METATYPE2(TYPE1, TYPE2)                               \
    QT_BEGIN_NAMESPACE                                                  \
    template <>                                                         \
    struct QMetaTypeId< TYPE1,TYPE2 >                                   \
{                                                                   \
    enum { Defined = 1 };                                           \
    static int qt_metatype_id()                                     \
{                                                           \
    static QBasicAtomicInt metatype_id = Q_BASIC_ATOMIC_INITIALIZER(0); \
    if (!metatype_id)                                       \
    metatype_id = qRegisterMetaType< TYPE1,TYPE2 >( #TYPE1 "," #TYPE2, \
    reinterpret_cast< TYPE1,TYPE2  *>(quintptr(-1))); \
    return metatype_id;                                     \
    }                                                           \
    };                                                                  \
    QT_END_NAMESPACE

Q_DECLARE_METATYPE(QFileInfo)
Q_DECLARE_METATYPE(QList<QFileInfo>)

#endif // METATYPES_H
