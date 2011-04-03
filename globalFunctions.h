
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

    enum ColumnType{ FileNameColumn, ArtistColumn, AlbumColumn, TitleColumn, YearColumn, TrackColumn, GenreColumn, LengthColumn, BitRateColumn, SampleRateColumn, ChannelsColumn }; //, CommentColumn

   // bool readFile(QIODevice &device, QSettings::SettingsMap &map);
   // bool writeFile(QIODevice &device, const QSettings::SettingsMap &map);
    /*
bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map);
bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map);
bool sortXmlElements(const QString &s1, const QString &s2);
*/
/*
    bool compareName( TagItem *s1, TagItem *s2 );
    bool compareArtistTag( TagItem *s1, TagItem *s2 );
    bool compareAlbumTag( TagItem *s1, TagItem *s2 );
    bool compareTitleTag( TagItem *s1, TagItem *s2 );
    bool compareTrackTag( TagItem *s1, TagItem *s2 );
    bool compareYearTag( TagItem *s1, TagItem *s2 );
    bool compareGenreTag( TagItem *s1, TagItem *s2 );
    bool compareCommentTag( TagItem *s1, TagItem *s2 );
*/
    //bool compareResult( SearchResult s1, SearchResult s2 );

    bool compareAlbum(  const Album &a1, const Album &a2  );
    int naturalCompare( const QString &_a, const QString &_b, Qt::CaseSensitivity caseSensitivity );

}

#endif
