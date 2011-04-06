
#ifndef DISCOGSALBUM_H
#define DISCOGSALBUM_H

#include <QtGui>
#include "Song.h"

class Album{

public:
	Album();
        QString key() const;
        QString displayArtist() const;
        void disp() const;
        QList<QUrl> images() const;
        QStringList artists() const;
        QStringList roles() const;
        QString type() const;
        QString title() const;
        int year() const;
        int nDiscs() const;
        int nTracks() const;
        QUrl url() const;
        QString label() const;
        QString catalog() const;
        QString format() const;
        QString country() const;
        QString genre() const;
        QString style() const;
        QList<Song> songs() const;
        QString notes() const;
        QString release() const;
        QHash<QString,QPixmap> downloadedImages() const;

        void setImages( QList<QUrl> images );
        void setArtists( QStringList artists );
        void setRoles( QStringList roles );
        void setType( QString type );
        void setTitle( QString title );
        void setYear( int );
        void setNDiscs( int nDiscs );
        void setNTracks( int nTracks );
        void setUrl( QUrl url );
        void setLabel( QString label );
        void setCatalog( QString catalog );
        void setFormat( QString format );
        void setCountry( QString country );
        void setGenre( QString genre );
        void setStyle( QString style );
        void setSongs( QList<Song> songs );
        void setNotes( QString notes );
        void setRelease( QString release );
        void setDownloadedImages( QHash<QString,QPixmap> dImages );

private:
        QList<QUrl> images_;
        QHash<QString,QPixmap> downloadedImages_;
        QStringList artists_;
        QStringList roles_;
        QString type_;
        QString title_;
        int year_;
        int nDiscs_;
        int nTracks_;
        QUrl url_;
        QString label_;
        QString catalog_;
        QString format_;
        QString country_;
        QString genre_;
        QString style_;
        QList<Song> songs_;
        QString notes_;
        QString release_;

};



#endif
