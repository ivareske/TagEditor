#ifndef DISCOGS_H
#define DISCOGS_H

#include <QtGui>
#include "MusicDatabase.h"
#include "Artist.h"


class Discogs : public MusicDatabase{
 Q_OBJECT

public:
    Discogs( QString key = "" );
    ~Discogs();
	public slots:
    void handleResults( QNetworkReply* reply );
    void handleRelease( QNetworkReply* reply );



private:
    QString api_key;
};



#endif
