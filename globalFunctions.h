
#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QtGui>

#include <QtXml>

namespace Global{


    enum ColumnType{ FileNameColumn, ArtistColumn, AlbumColumn, TitleColumn, YearColumn, TrackColumn, GenreColumn, LengthColumn, BitRateColumn, SampleRateColumn, ChannelsColumn, CommentColumn, NColumns }; //,

    const QString settingsFile = Global::settingsFile;
    QString columnText( Global::ColumnType t );

    enum TagField{ FileName, Artist, Title, AlbumField, Comment, Genre, Year, Track, Length, BitRate, SampleRate, Channels, NTagFields };

    QString columnText( Global::TagField t );    

    int naturalCompare( const QString &_a, const QString &_b, Qt::CaseSensitivity caseSensitivity );

}

#endif
