#ifndef SONG_H
#define SONG_H


#include <QtGui>

class Song{

public:
    Song();
    QString track() const;
    QString title() const;
    QString artist() const;
    QString id() const;
    int length() const;
    void setTrack( QString track );
    void setTitle( QString title );
    void setArtist( QString artist );
    void setId( QString id );
    void setLength( int length );

private:
    QString track_; //track as string since track can be A-1, A-2 etc for lp in discogs
    QString title_;
    QString artist_;
    QString id_;
    int length_;


};


#endif // SONG_H
