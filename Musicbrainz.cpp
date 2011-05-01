#include "MusicBrainz.h"


MusicBrainz::MusicBrainz(){

    type_ = MusicDatabase::MUSICBRAINZ;

}


void MusicBrainz::handleResults( QNetworkReply* reply ){

    QString err="";

    QStringList artists;

    QByteArray response(reply->readAll());
    QXmlStreamReader xml(response);
    while (!xml.atEnd()) {
        xml.readNext();
        //qDebug()<<xml.name();

        if (xml.tokenType() == QXmlStreamReader::StartElement){

            if (xml.name() == "release") {
                //SearchResult res;
                Album res;
                QList<ReleaseEvent> events;
                res.setType( xml.attributes().value("type").toString() );
                res.setRelease( xml.attributes().value("id").toString() );
                res.setUrl( QUrl( "http://musicbrainz.org/ws/1/release/"+res.release()+"?type=xml&inc=tracks+artist" ) );
                //qDebug()<<res.type;
                //qDebug()<<res.release;
                while (xml.readNextStartElement()) {
                    //qDebug()<<"xml.name: "<<xml.name();
                    if (xml.name() == "title"){
                        res.setTitle( xml.readElementText() );
                        //qDebug()<<res.title;
                    }else if (xml.name() == "artist"){
                        while (xml.readNextStartElement()) {
                            if (xml.name() == "name"){
                                artists.append( xml.readElementText() );
                                //qDebug()<<res.artists.last();
                            }else{
                                xml.skipCurrentElement();
                            }
                            //qDebug()<<xml.name();
                        }
                    }else if (xml.name() == "release-event-list"){
                        while (xml.readNextStartElement()) {
                            if(xml.name() == "event"){
                                ReleaseEvent event;
                                //qDebug()<<"new event";
                                event.setFormat( xml.attributes().value("format").toString() );
                                event.setCountry( xml.attributes().value("country").toString() );
                                QString datestr = xml.attributes().value("date").toString();
                                //qDebug()<<"datestr "<<datestr;
                                event.setDate( QDate::fromString( datestr, "yyyy-MM-dd" ) );
                                //qDebug()<<event.format<<event.date.year()<<event.country;
                                while (xml.readNextStartElement()) {
                                    //qDebug()<<xml.name();
                                    if (xml.name() == "label"){
                                        QStringList labels;
                                        while (xml.readNextStartElement()) {
                                            if (xml.name() == "name"){
                                                labels.append( xml.readElementText() );
                                                //qDebug()<<"label: "<<event.labels.last();
                                            }else{
                                                //qDebug()<<"skipping "<<xml.name();
                                                xml.skipCurrentElement(); //break;
                                            }
                                        }
                                        event.setLabels( labels );
                                    }else{
                                        //qDebug()<<"skipping "<<xml.name();
                                        xml.skipCurrentElement(); //break;
                                    }
                                }
                                //qDebug()<<"appending event";
                                events.append(event);
                            }else{
                                //qDebug()<<"skipping "<<xml.name();
                                xml.skipCurrentElement(); //break;
                            }
                        }
                        //qDebug()<<1<<xml.name();
                    }else if (xml.name() == "disc-list"){
                        res.setNDiscs( xml.attributes().value("count").toString().toInt() );
                        //qDebug()<<"discs "<<res.ndiscs;
                    }else if (xml.name() == "track-list"){
                        res.setNTracks( xml.attributes().value("count").toString().toInt() );
                        //qDebug()<<"tracks "<<res.ntracks;
                    }else{
                        xml.skipCurrentElement();
                    }
                    //qDebug()<<2;
                }
                //qDebug()<<"events.size() "<<events.size();
                for(int i=0;i<events.size();i++){
                    Album tmp = res;
                    tmp.setFormat( events[i].format() );                    
                    tmp.setYear( events[i].date().year() );                    
                    tmp.setCountry( events[i].country() );
                    tmp.setLabel( events[i].labels().join(", ") );
                    qDebug()<<"musicbrainz "<<tmp.country()<<tmp.label()<<tmp.format();
                    tmp.setArtists( artists );
                    //qDebug()<<"appending result!";
                    //tmp.disp();
                    albums_.insert(tmp.key(),tmp);
                }
            }
        }
    }

    if( reply->error() || !err.isEmpty() ){
        if( reply->error() ){
            err = reply->errorString()+":\n"+err;
        }
        QMessageBox::critical(0, "Error",
                              err, QMessageBox::Ok, QMessageBox::Ok);
    }


    reply->deleteLater();

    if( !downloadImmediately_ ){
        emit resultsDownloaded( albums_ );
    }else{
        if( albums_.size()==0 ){
            emit albumsDownloaded( albums_ );
        }else{
            downloadAllAlbums( albums_ );
        }
    }


}



void MusicBrainz::handleRelease( QNetworkReply* reply ){

    Album album;
    QNetworkRequest request = reply->request();
    QString key = request.attribute(QNetworkRequest::User).toString();
    //qDebug()<<"keeeeeeeeeeeeeeeeey"<<key;

    if( albums_.contains(key) ){
        album = albums_[key];
        //album.artists.clear();
        //album.songs.clear();
    }
qDebug()<<"MusicBrainz::handleRelease "<<album.country()<<album.label()<<album.format();

    QString err;
    QByteArray response(reply->readAll());
    QXmlStreamReader xml(response);

    //QList<Artist> Artists;
    QStringList artists;
    QList<Song> songs;

    while (!xml.atEnd()) {
        xml.readNext();
        //qDebug()<<xml.name();

        if (xml.tokenType() == QXmlStreamReader::StartElement){

            if (xml.name() == "release") {
                album.setRelease( xml.attributes().value("id").toString() );
                album.setType( xml.attributes().value("type").toString() );
            }
            if (xml.name() == "title") {
                album.setTitle( xml.readElementText() );
            }
            if (xml.name() == "artist") {
                while (xml.readNextStartElement()) {
                    if( xml.name() == "name" ){
                        QString artist = xml.readElementText();
                        if(!artists.contains(artist)){
                            artists.append( artist );
                        }
                    }else{
                        break;
                    }
                }
            }
            if (xml.name() == "track-list") {
                //qDebug()<<"track-list";
                int track=1;
                while (xml.readNextStartElement()) {
                    if( xml.name() == "track" ){
                        Song s;
                        while (xml.readNextStartElement()) {
                            if( xml.name() == "title" ){
                                s.setTitle( xml.readElementText() );
                                //qDebug()<<s.title;
                            }else if( xml.name() == "duration" ){
                                s.setLength( floor( ((double)xml.readElementText().toInt())/1000.0 ) );
                                //qDebug()<<s.length;
                            }else if( xml.name() == "artist" ){
                                while (xml.readNextStartElement()) {
                                    if( xml.name() == "name" ){
                                        s.setArtist( xml.readElementText() );
                                        //qDebug()<<s.artist;
                                    }else{
                                        xml.skipCurrentElement();
                                    }
                                }
                            }else{
                                xml.skipCurrentElement();
                            }
                        }
                        s.setTrack( QString::number(track) ); track++;
                        songs.append(s);
                    }else{
                        xml.skipCurrentElement();
                    }
                }
            }
        }
    }

    album.setSongs(songs);
    album.setArtists(artists);

    if( reply->error() || !err.isEmpty() ){
        if( reply->error() ){
            err = reply->errorString()+":\n"+err;
        }
        QMessageBox::critical(0, "Error",
                              err, QMessageBox::Ok, QMessageBox::Ok);
    }

    reply->deleteLater();
    albums_.insert(album.key(),album);
    //album.disp();
    nDownloaded_++;

    if( downloadImmediately_ && albums_.size()==nDownloaded_ ){
        emit albumsDownloaded( albums_ );
    }
    if(!downloadImmediately_){
        emit albumDownloaded( album );
    }
}

