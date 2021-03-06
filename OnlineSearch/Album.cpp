#include "Album.h"


Album::Album(){


}


/*
Album& Album::operator=(const Album &other){

    covers_ = other.covers();
    images_ = other.images();
    artists_ = other.artists();
    roles_ = other.roles();
    type_ = other.type();
    title_ = other.title();
    year_ = other.year();
    nDiscs_ = other.nDiscs();
    nTracks_ = other.nTracks();
    url_ = other.url();
    label_ = other.label();
    catalog_ = other.catalog();
    format_ = other.format();
    country_ = other.country();
    genre_ = other.genre();
    style_ = other.style();
    songs_ = other.songs();
    notes_ = other.notes();
    release_ = other.release();

    return *this;
}

Album::Album( const Album &other ){

    covers_ = other.covers();
    images_ = other.images();
    artists_ = other.artists();
    roles_ = other.roles();
    type_ = other.type();
    title_ = other.title();
    year_ = other.year();
    nDiscs_ = other.nDiscs();
    nTracks_ = other.nTracks();
    url_ = other.url();
    label_ = other.label();
    catalog_ = other.catalog();
    format_ = other.format();
    country_ = other.country();
    genre_ = other.genre();
    style_ = other.style();
    songs_ = other.songs();
    notes_ = other.notes();
    release_ = other.release();

}
*/
void Album::setCovers( const QList<QPixmap> covers ){
    covers_ = covers;
}

QList<QPixmap> Album::covers() const{
    return covers_;
}
QList<QUrl> Album::images() const{
    return images_;
}
QStringList Album::artists() const{
    return artists_;
}
QStringList Album::roles() const{
    return roles_;
}
QString Album::type() const{
    return type_;
}
QString Album::title() const{
    return title_;
}
int Album::year() const{
    return year_;
}
int Album::nDiscs() const{
    return nDiscs_;
}
int Album::nTracks() const{
    return nTracks_;
}
QUrl Album::url() const{
    return url_;
}
QString Album::label() const{
    return label_;
}
QString Album::catalog() const{
    return catalog_;
}
QString Album::format() const{
    return format_;
}
QString Album::country() const{
    return country_;
}
QString Album::genre() const{
    return genre_;
}
QString Album::style() const{
    return style_;
}
QList<Song> Album::songs() const{
    return songs_;
}
QString Album::notes() const{
    return notes_;
}
QString Album::release() const{
    return release_;
}


void Album::setImages( QList<QUrl> images ){
    images_ = images;
}

void Album::setArtists( QStringList artists ){
    artists_ = artists;
}

void Album::setRoles( QStringList roles ){
    roles_ = roles;
}

void Album::setType( QString type ){
    type_ = type;
}

void Album::setTitle( QString title ){
    title_ = title;
}

void Album::setYear( int year ){
    year_ = year;
}

void Album::setNDiscs( int nDiscs ){
    nDiscs_ = nDiscs;
}

void Album::setNTracks( int nTracks ){
    nTracks_ = nTracks_;
}

void Album::setUrl( QUrl url ){
    url_ = url;
}

void Album::setLabel( QString label ){
    label_ = label;
}

void Album::setCatalog( QString catalog ){
    catalog_ = catalog;
}

void Album::setFormat( QString format ){
    format_ = format;
}

void Album::setCountry( QString country ){
    country_ = country;
}

void Album::setGenre( QString genre ){
    genre_ = genre;
}

void Album::setStyle( QString style ){
    style_ = style;
}

void Album::setSongs( QList<Song> songs ){
    songs_ = songs;
}

void Album::setNotes( QString notes ){
    notes_ = notes;
}

void Album::setRelease( QString release ){
    release_ = release;
}


QString Album::key() const{

    //QString key_ = release_+format_+country_+QString::number(year_);
    QString key_ = release_;//+artists_.join(",")+title_;
    //qDebug()<< key_;
    return key_;

}

QString Album::displayArtist() const{

    QString res="Unknown";
    if(artists_.size()>1){
        //res = "Various";
        artists_.join(", ");
    }else if(artists_.size()==1){
        res = artists_[0];
    }
    //qDebug()<<res;
    return res;

}

void Album::disp() const{
    qDebug()<<"songs "<<songs_.size();
/*
    qDebug()<<"-----------------------------";
    for(int i=0;i<images.size();i++){
        qDebug()<<images[i];
    }
    qDebug()<<artists.join(", ");
    qDebug()<<roles.join(", ");
    qDebug()<<type;
    qDebug()<<title;
    qDebug()<<year;
    qDebug()<<ndiscs;
    qDebug()<<ntracks;
    qDebug()<<url;
    qDebug()<<label;
    qDebug()<<catalog;
    qDebug()<<format;
    qDebug()<<country;
    qDebug()<<genre;
    qDebug()<<style;
    for(int i=0;i<songs.size();i++){
        qDebug()<<songs[i].track;
        qDebug()<<songs[i].title;
        qDebug()<<songs[i].artist;
        qDebug()<<songs[i].id;
        qDebug()<<songs[i].length;
    }
    qDebug()<<notes;
    qDebug()<<release;
    qDebug()<<"-----------------------------";
    */
}

bool compareAlbum( const Album &a1, const Album &a2 ){
    int res = Global::naturalCompare( a1.format(), a2.format(), Qt::CaseInsensitive );
    if(res==0){
        return a1.year() < a2.year();
    }else{
        return res<0;
    }

}
