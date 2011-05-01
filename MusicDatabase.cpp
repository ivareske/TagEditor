#include "MusicDatabase.h"


MusicDatabase::MusicDatabase(){
    downloadImmediately_=true;
    nDownloaded_=0;
    searchManager  = new QNetworkAccessManager;
    releaseManager = new QNetworkAccessManager;
    downloadAllManager = new QNetworkAccessManager;
    //coverManager = new QNetworkAccessManager;
    connect(downloadAllManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleRelease(QNetworkReply*)));
    connect(searchManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleResults(QNetworkReply*)));
    connect(releaseManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleRelease(QNetworkReply*)));
    //connect(coverManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleCover(QNetworkReply*)));

}
MusicDatabase::~MusicDatabase(){

    delete searchManager;
    delete releaseManager;
    delete downloadAllManager;
    //delete coverManager;

}

void MusicDatabase::handleCover( QNetworkReply* reply ){



}

MusicDatabase::DataBaseType MusicDatabase::type() const{
    return type_;
}

void MusicDatabase::downloadAllAlbums( QHash<QString,Album> results ){

    //Albums.clear();
    nDownloaded_=0;
    QList<Album> tmpresults = results.values();

    qDebug()<<tmpresults.size()<<" ALBUMS FOUND!!!!!!!!!!";
    /*
    QStringList keys;
    for(int i=0;i<tmpresults.size();i++){
        keys.append(tmpresults[i].key());
    }
    qDebug()<<"NUMBER OF UNIQUE KEYS: "<<keys.toSet().toList().size();
    */
    for(int i=0;i<tmpresults.size();i++){
        qDebug()<<"downloading album "<<tmpresults[i].title();
        downloadAlbum( tmpresults[i] );
    }

}

void MusicDatabase::downloadAlbum( Album result ){

    QNetworkRequest r(result.url()); r.setAttribute( QNetworkRequest::User, result.key() );
    releaseManager->get(r);
    //handleRelease is then called
}

void MusicDatabase::handleResults( QNetworkReply* reply ){
    //must be overloaded!
}

void MusicDatabase::handleRelease( QNetworkReply* reply ){
    //must be overloaded!

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

