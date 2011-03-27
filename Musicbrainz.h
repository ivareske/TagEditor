#ifndef MUSICBRAINZ_H
#define MUSICBRAINZ_H
 
#include <QtGui>
#include "MusicDatabase.h"
#include "ReleaseEvent.h"
#include "Artist.h"


class MusicBrainz : public MusicDatabase{
 Q_OBJECT
	public:
                MusicBrainz();
                ~MusicBrainz();
		
	public slots:
		void handleResults( QNetworkReply* reply );
		void handleRelease( QNetworkReply* reply );

	private:
		
};



#endif
