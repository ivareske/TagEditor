#ifndef MUSICDATABASE_H
#define MUSICDATABASE_H

#include <QtGui>
#include <QtNetwork>
#include "Album.h"


class MusicDatabase : public QObject{
    Q_OBJECT    


public:
    enum DataBaseType{ DISCOGS, MUSICBRAINZ };

    MusicDatabase();
    virtual ~MusicDatabase();
    //QHash<QString,SearchResult> Results;
    void search( QString query, bool downloadNow );
    virtual void downloadAlbum( Album result );
    virtual MusicDatabase::DataBaseType type() const;
    void downloadAllAlbums( QHash<QString,Album> results );

protected slots:
    void handleCover( QNetworkReply* reply );
    virtual void handleResults( QNetworkReply* reply );
    virtual void handleRelease( QNetworkReply* reply );
    void setAlbums( QHash<QString,Album> albums );
    void setDownloadImmediately( bool downloadImmediately );
signals:
    void resultsDownloaded( QHash<QString,Album> Results );
    void albumDownloaded( Album album );
    void albumsDownloaded( QHash<QString,Album> Albums );

protected:
    QHash<QString,Album> albums() const;
    bool downloadImmediately() const;
    MusicDatabase::DataBaseType type_;
    bool downloadImmediately_;
    QHash<QString,Album> albums_;
    int nDownloaded_;
    QNetworkAccessManager *searchManager;
    QNetworkAccessManager *releaseManager;
    QNetworkAccessManager *downloadAllManager;
    //QNetworkAccessManager *coverManager;


};



#endif
