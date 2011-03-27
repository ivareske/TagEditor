
#ifndef MUSICBRAINZALBUM_H
#define MUSICBRAINZALBUM_H

#include <QtGui>
#include "structs.h"

class MusicbrainzAlbum{

public:
	MusicbrainzAlbum();

	QList<QUrl> images;
	QString artist;
	QString title;
	int year;
	QString label;
	QString format;
	QString country;
	QString genre;
	QString style;
	QList<song> songs;
	QString release;
};



#endif
