#include "Discogs.h"


Discogs::Discogs( QString key ){

    api_key = key;

    type_ = MusicDatabase::DISCOGS;

}


void Discogs::handleResults( QNetworkReply* reply ){
    //a serach result, not the same as a release. Contains less info
    qDebug()<<"Discogs handleResults";
    QString err="";
    //QList<SearchResult> Results;
    //if (!reply->error()) {

    QByteArray response(reply->readAll());
    QXmlStreamReader xml(response);
    while (!xml.atEnd()) {
        xml.readNext();
        //qDebug()<<xml.name();

        if (xml.tokenType() == QXmlStreamReader::StartElement){
            if (xml.name() == "resp") {
                if( xml.attributes().hasAttribute("stat") ){
                    QString stat = xml.attributes().value("stat").toString();
                    if(stat=="fail"){
                        while(xml.readNextStartElement()){
                            if( xml.name() == "error" ){
                                err.append(xml.readElementText()+"\n");
                            }else{
                                xml.skipCurrentElement();
                            }
                        }
                    }
                }else if( xml.attributes().hasAttribute("requests") ){
                    //int nSearches = xml.attributes().value("requests").toString().toInt();
                    //qDebug()<<nSearches;
                    //info->setText(QString::number(nSearches)+" searches performed within the last 24 hours");
                }else{
                    xml.skipCurrentElement();
                }
            }

            if (xml.name() == "result") {
                if( xml.attributes().value("type").toString()=="release" ){
                    //SearchResult res;
                    Album res;
                    //QHash<QString,Album> albums_ = albums();
                    while (xml.readNextStartElement()) {
                        if (xml.name() == "title"){
                            res.setTitle( xml.readElementText() );
                        }else if (xml.name() == "uri"){
                            QString tmp = xml.readElementText();
                            int ind = tmp.lastIndexOf("/")+1;
                            if(ind!=-1){
                                //qDebug()<<tmp;
                                tmp=tmp.right(tmp.size()-ind);
                                //qDebug()<<tmp;
                                res.setUrl( QUrl( "http://www.discogs.com/release/"+tmp+"?f=xml&api_key="+api_key ) );
                                res.setRelease( tmp );
                            }
                        }else if (xml.name() == "summary"){
                            //res.summary = xml.readElementText();
                        }else{
                            xml.skipCurrentElement();
                        }
                    }
                    //qDebug()<<res.title<<" "<<res.url<<" "<<res.summary;
                    //Results.insert(res.release+res.format,res);
                    albums_.insert(res.key(),res);
                    setAlbums( albums_ );
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

    if( !downloadImmediately() ){
        emit resultsDownloaded( albums_ );
    }else{
        if( albums_.size()==0 ){
            emit albumsDownloaded( albums_ );
        }else{
            downloadAllAlbums( albums_ );
        }
    }

}


void Discogs::handleRelease( QNetworkReply* reply ){

    Album album;
    QNetworkRequest request = reply->request();
    QString key = request.attribute(QNetworkRequest::User).toString();

    if( albums_.contains(key) ){
        album = albums_[key];
        //album.setArtists( QStringList() );
        //album.setSongs( QList<Song>() );
    }


    //cover->setText("");
    QString err;
    QByteArray response(reply->readAll());
    QXmlStreamReader xml(response);

    QList<Artist> Artists;
    QList<QUrl> images;
    QStringList artists,roles;
    QList<Song> songs;
    while (!xml.atEnd()) {
        xml.readNext();
        //qDebug()<<xml.name();

        if( xml.name() == "release" && xml.attributes().hasAttribute("id") ){
            QString id = xml.attributes().value("id").toString();
            album.setRelease(id);
        }

        if(xml.tokenType() == QXmlStreamReader::StartElement){
            if( xml.attributes().hasAttribute("stat") ){
                QString stat = xml.attributes().value("stat").toString();
                if(stat=="fail"){
                    while(xml.readNextStartElement()){
                        if( xml.name() == "error" ){
                            err.append(xml.readElementText()+"\n");
                        }else{
                            xml.skipCurrentElement();
                        }
                    }
                }
            }
            if( xml.attributes().hasAttribute("requests") ){
                //&nSearches = xml.attributes().value("requests").toString().toInt();
                //info->setText(QString::number(nSearches)+" searches performed within the last 24 hours");
            }
            if(xml.name() == "images"){
                xml.readNext();
                while(xml.name()=="image"){
                    if( xml.attributes().hasAttribute("uri") ){
                        images.append( QUrl( xml.attributes().value("uri").toString() ) );
                        //qDebug()<<"found image "<<xml.attributes().value("uri").toString();
                    }
                    xml.readNext();
                }
            }
            if(xml.name() == "artists"){
                while(xml.readNextStartElement()){
                    if(xml.name() == "artist"){
                        while(xml.readNextStartElement()){
                            if(xml.name() == "name"){
                                artists.append( xml.readElementText() );
                            }else{
                                xml.skipCurrentElement();
                            }
                        }
                    }else{
                        xml.skipCurrentElement();
                    }
                }
            }
            if(xml.name() == "extraartists"){
                while(xml.readNextStartElement()){
                    if(xml.name() == "artist"){
                        //QString name;QString role;
                        Artist a;
                        while(xml.readNextStartElement()){
                            if(xml.name() == "name"){
                                a.setName( xml.readElementText() );
                            }else if(xml.name() == "role"){
                                a.setRole( xml.readElementText() );
                            }else{
                                xml.skipCurrentElement();
                            }
                            Artists.append(a);
                        }
                        //qDebug()<<role+": "+name;
                        //album.roles.append( role+": "+name );
                    }else{
                        xml.skipCurrentElement();
                    }
                }
            }
            if(xml.name() == "formats"){
                while(xml.readNextStartElement()){
                    if( xml.name()=="format" && xml.attributes().hasAttribute("name") ){
                        QString sep=", ";
                        if(album.format().isEmpty()){
                            sep="";
                        }
                        album.setFormat( album.format() + sep + xml.attributes().value("name").toString() );
                    }else{
                        xml.skipCurrentElement();
                    }
                }
            }
            if(xml.name() == "title"){
                album.setTitle( xml.readElementText() );
            }
            if(xml.name() == "labels"){
                xml.readNext();
                while(xml.name()=="label"){
                    if( xml.attributes().hasAttribute("name") ){
                        QString sep=", ";
                        if(album.label().isEmpty()){
                            sep="";
                        }
                        album.setLabel( album.label()+sep+xml.attributes().value("name").toString() );
                    }
                    xml.readNext();
                }
            }
            if(xml.name() == "genres"){
                while(xml.readNextStartElement()){
                    if(xml.name() == "genre"){
                        QString sep=", ";
                        if(album.genre().isEmpty()){
                            sep="";
                        }
                        album.setGenre( album.genre() + sep + xml.readElementText() );
                    }else{
                        xml.skipCurrentElement();
                    }
                }
            }
            if(xml.name() == "styles"){
                while(xml.readNextStartElement()){
                    if(xml.name() == "style"){
                        QString sep=", ";
                        if(album.style().isEmpty()){
                            sep="";
                        }
                        album.setStyle( album.style() + sep + xml.readElementText() );
                    }else{
                        xml.skipCurrentElement();
                    }
                }
            }
            if(xml.name() == "country"){
                album.setCountry( xml.readElementText() );
            }
            if(xml.name() == "released"){
                album.setYear( xml.readElementText().toInt() );
            }
            if(xml.name() == "notes"){
                album.setNotes( xml.readElementText() );
            }
            if(xml.name() == "tracklist"){
                while(xml.readNextStartElement()){
                    if(xml.name() == "track"){
                        Song s;
                        while(xml.readNextStartElement()){
                            //qDebug()<<xml.name()<<xml.readElementText();
                            if(xml.name() == "position"){
                                s.setTrack( xml.readElementText() );
                            }else if(xml.name() == "title"){
                                s.setTitle( xml.readElementText() );
                            }else if(xml.name() == "duration"){
                                QStringList tmp = xml.readElementText().split(":");
                                int sum=0;
                                for(int j=0;j<tmp.size();j++){
                                    sum = sum + ( tmp[j].toInt()*pow(60,tmp.size()-j-1) );
                                }
                                s.setLength(sum);
                            }else{
                                xml.skipCurrentElement();
                            }
                        }
                        songs.append(s);
                    }else{
                        xml.skipCurrentElement();
                    }
                }
            }
        }
    }

    //fix/compress roles for artists
    //QList<artist> newArtists;
    while( Artists.size()>0 ){
        QString role = Artists[0].role();
        if(role.isEmpty()){
            Artists.removeAt(0);
            continue;
        }
        Artist a;a.setRole(role);a.setName(Artists[0].name());
        //qDebug()<<role;
        for(int j=Artists.size()-1;j>=1;j--){
            if( Artists[j].role()==role ){
                if( !a.name().contains( Artists[j].name() ) ){
                    a.setName(a.name()+"/"+Artists[j].name());
                    //qDebug()<<a.name;
                }
                Artists.removeAt(j);
            }
        }
        Artists.removeAt(0);
        roles.append( a.role()+": "+a.name() );
        //newArtists.append(a);
    }

    //remove html links from notes
    QString notes = album.notes();
    while(1){
        int ind = notes.indexOf("<a href");
        if(ind==-1){
            break;
        }
        int ind2 = notes.indexOf(">",ind);
        notes.remove(ind,ind2-ind+1);
    }
    notes.remove("</a>",Qt::CaseInsensitive);
    album.setNotes(notes);

    if( reply->error() || !err.isEmpty() ){
        if( reply->error() ){
            err = reply->errorString()+":\n"+err;
        }
        QMessageBox::critical(0, "Error",
                              err, QMessageBox::Ok, QMessageBox::Ok);
    }

    album.setArtists(artists);
    album.setRoles(roles);
    album.setImages(images);
    album.setSongs(songs);

    albums_.insert(album.key(),album);
    reply->deleteLater();

    /*
    if(images.size()>0){
        //qDebug()<<"starting image downloading for album "+album.title()<<", images: "<<images;
        QNetworkRequest coverRequest(images[0]);        
        coverRequest.setAttribute(QNetworkRequest::User,album.key());
        QNetworkAccessManager *coverManager = new QNetworkAccessManager;
        connect(coverManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleCover(QNetworkReply*)));
        coverManager->get(coverRequest);
    }else {
        */
        nDownloaded_++;
        qDebug()<<"downloaded album "<<album.title();
        if( downloadImmediately_ && nDownloaded_==albums_.size() ){
            emit albumsDownloaded( albums_ );
        }
        if(!downloadImmediately_){
            emit albumDownloaded( album );
        }
    //}


}

/*
  //too slow downloading all images at once
void Discogs::handleCover(QNetworkReply* reply){

    QByteArray data = reply->readAll();
    QPixmap p;
    p.loadFromData(data);
    QString key = reply->request().attribute(QNetworkRequest::User).toString();
    //qDebug()<<key;
    Album album = albums_[key];
    QList<QPixmap> covers = album.covers();
    covers.append(p);
    album.setCovers(covers);
    albums_[key] = album;
    QList<QUrl> images = album.images();
    int ind = -1;
    for(int i=0;i<images.size();i++){
        if(images[i]==reply->url()){
            ind = i+1;
            break;
        }
    }

    //qDebug()<<"downloaded "<<reply->url().toString()<<" for album "<<album.title()<<", "<<album.covers().size()<<" of "<<album.images().size()<<" finished ";
    //qDebug()<<album.title()<<": "<<album.covers().size()<<" of "<<album.images().size()<<" covers finished "<<reply->url().toString();

    reply->deleteLater();

    if(ind!=-1 && ind<images.size()){
        //download next cover in line
        QNetworkRequest coverRequest(images[ind]);
        coverRequest.setAttribute(QNetworkRequest::User,album.key());
        QNetworkAccessManager *coverManager = new QNetworkAccessManager;
        connect(coverManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleCover(QNetworkReply*)));
        coverManager->get(coverRequest);
    }else{

        if(album.covers().size()==album.images().size()){
            //all covers for this album downloaded
            nDownloaded_++;
            qDebug()<<"downloaded all covers for album "<<album.title();
            //qDebug()<<downloadImmediately_<<nDownloaded_<<albums_.size();
            if( downloadImmediately_ && nDownloaded_==albums_.size() ){
                emit albumsDownloaded( albums_ );
            }
            if(!downloadImmediately_){
                emit albumDownloaded( album );
            }
        }
    }

}
*/
