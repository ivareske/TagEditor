#include "MusicDatabase.h"

 
MusicDatabase::MusicDatabase(){
        downloadImmediately_=true;
        nDownloaded_=0;
	searchManager  = new QNetworkAccessManager;
	releaseManager = new QNetworkAccessManager;
	downloadAllManager = new QNetworkAccessManager;
	connect(downloadAllManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleRelease(QNetworkReply*)));
	connect(searchManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleResults(QNetworkReply*)));
	connect(releaseManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleRelease(QNetworkReply*)));

}
MusicDatabase::~MusicDatabase(){

	delete searchManager;
	delete releaseManager;
	delete downloadAllManager;
	
}

void MusicDatabase::downloadAllAlbums( QHash<QString,Album> results ){
	
	//Albums.clear();
        nDownloaded_=0;
	QList<Album> tmpresults = results.values();
	for(int i=0;i<tmpresults.size();i++){		
                qDebug()<<"downloading album "<<tmpresults[i].title();
		downloadAlbum( tmpresults[i] );
	}	

}

void MusicDatabase::downloadAlbum( Album result ){
	
        QNetworkRequest r(result.url()); r.setAttribute( QNetworkRequest::User, result.key() );
	releaseManager->get(r);
}

void MusicDatabase::handleResults( QNetworkReply* reply ){
	
}

void MusicDatabase::handleRelease( QNetworkReply* reply ){

}


void MusicDatabase::search( QString query, bool downloadNow ){
	qDebug()<<"searching!: "<<query;
        downloadImmediately_ = downloadNow;
	//Results.clear();
        albums_.clear();
	searchManager->get(QNetworkRequest(QString(query)));
	//handleResults is then executed when searchManager is finished
	
}

QHash<QString,Album> MusicDatabase::albums() const{
    return albums_;
}
bool MusicDatabase::downloadImmediately() const{
    return downloadImmediately_;
}

void MusicDatabase::setAlbums( QHash<QString,Album> albums ){
    albums_ = albums;
}
void MusicDatabase::setDownloadImmediately( bool downloadImmediately ){
    downloadImmediately_ = downloadImmediately;
}

