
#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QtGui>
//#include "TagItem.h"
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include "Album.h"
#include <QtXml>


namespace Global{


    //enum ColumnType{ FileNameColumn, ArtistColumn, AlbumColumn, TitleColumn, YearColumn, TrackColumn, GenreColumn, LengthColumn, BitRateColumn, SampleRateColumn, ChannelsColumn, CommentColumn, NColumns }; //,
    enum TagField{ FileName, Artist, Title, AlbumField, Comment, Genre, Year, Track, Length, BitRate, SampleRate, Channels, NTagFields };

    QString columnText( Global::TagField t );
    bool compareAlbum(  const Album &a1, const Album &a2  );
    int naturalCompare( const QString &_a, const QString &_b, Qt::CaseSensitivity caseSensitivity );
    //const uint undefined = 99999999;
}

#endif
