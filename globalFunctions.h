
#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QtGui>
#include <QtXml>
#include <iostream>


namespace Global{


    enum TagField{ FileName, Artist, Title, AlbumField, Comment, Genre, Year, Track, Length, BitRate, SampleRate, Channels, NTagFields };

    QList<QFileInfo> getDirContent(const QString& aPath, const QStringList &extensions, bool includeSubFolders=true, bool hiddenFiles=true, bool *canceled=false);
    QSettings* guiSettings(QObject *parent=0);
    QString columnText( Global::TagField t );    
    int naturalCompare( const QString &_a, const QString &_b, Qt::CaseSensitivity caseSensitivity );

}

#endif
