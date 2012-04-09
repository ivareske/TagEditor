#include "TagItem.h"


TagItem::TagItem( const QString &fullfile, QTreeWidget *parent, int type ) : QObject(parent),QTreeWidgetItem( parent, type) {

    artist_="";
    title_="";
    album_="";
    comment_="";
    genre_="";
    year_=0;
    track_=0;
    tagOk_=false;
    audioPropertiesOk_=false;
    length_=0;
    bitRate_=0;
    sampleRate_=0;
    channels_=0;
    unSavedChanges_=false;

    fileInfo_ = QFileInfo( fullfile );
    tagIsRead_=false;    

    setToolTip( 0, fullfile );
    //setColumnData();

}

bool TagItem::operator<(const QTreeWidgetItem &other) const{
    int column = treeWidget()->sortColumn();
    const QVariant v1 = data(column, Qt::DisplayRole);
    const QVariant v2 = other.data(column, Qt::DisplayRole);
    return Global::naturalCompare(v1.toString(),v2.toString(),Qt::CaseInsensitive) < 0;
}
/*
TagItem::TagItem( const TagItem &other ){

    artist_ = other.artist();
    title_ = other.title();
    album_ = other.album();
    comment_ = other.comment();
    genre_ = other.genre();
    year_ = other.year();
    track_ = other.track();

    length_ = other.length();
    bitRate_ = other.bitRate();
    sampleRate_ = other.sampleRate();
    channels_ = other.channels();

    tagIsRead_ = other.tagIsRead();
    fileInfo_ = other.fileInfo();
    tagOk_ = other.tagOk();
    audioPropertiesOk_ = other.audioPropertiesOk();
    unSavedChanges_ = other.unSavedChanges();

    setToolTip( 0, fileInfo_.absoluteFilePath() );
    //setColumnData();

}
*/
void TagItem::setColumnData( const QList<Global::TagField> &columns, bool showFullFileName, bool readTags_ ){

    if(readTags_){
        readTags();
    }

    int k=0;
    for(int i=0;i<columns.size();i++){

        QString str = getTag( columns[i] ).toString();
        if(columns[i]==Global::FileName && showFullFileName){
            str = fileInfo_.absoluteFilePath();
        }
        setData( k, Qt::DisplayRole, str );
        k++;
    }

}

QString TagItem::artist() const{
    return artist_;
}

QString TagItem::title() const{
    return title_;
}

QString TagItem::album() const{
    return album_;
}

QString TagItem::comment() const{
    return comment_;
}

QString TagItem::genre() const{
    return genre_;
}

uint TagItem::year() const{
    return year_;
}

uint TagItem::track() const{
    return track_;
}

uint TagItem::length() const{
    return length_;
}
uint TagItem::bitRate() const{
    return bitRate_;
}
uint TagItem::sampleRate() const{
    return sampleRate_;
}
uint TagItem::channels() const{
    return channels_;
}

void TagItem::setArtist( const QString &artist ){
    unSavedChanges_ = artist!=artist_;
    artist_ = artist;
}

void TagItem::setAlbum( const QString &album ){
    unSavedChanges_ = album!=album_;
    album_ = album;
}

void TagItem::setTitle( const QString &title ){
    unSavedChanges_ = title!=title_;
    title_ = title;
}

void TagItem::setGenre( const QString &genre ){
    unSavedChanges_ = genre!=genre_;
    genre_ = genre;
}

void TagItem::setComment( const QString &comment ){
    unSavedChanges_ = comment!=comment_;
    comment_ = comment;
}

void TagItem::setTrack( uint track ){
    unSavedChanges_ = track!=track_;
    track_ = track;
}

void TagItem::setYear( uint year ){
    unSavedChanges_ = year!=year_;
    year_ = year;
}

bool TagItem::unSavedChanges() const{
    return unSavedChanges_;
}

bool TagItem::tagOk() const{
    return tagOk_;
}

bool TagItem::tagIsRead() const{
    return tagIsRead_;
}

bool TagItem::audioPropertiesOk() const{
    return audioPropertiesOk_;
}

QFileInfo TagItem::fileInfo() const{
    return fileInfo_;
}

QVariant TagItem::getTag( Global::TagField field, bool read ){


    if(!tagIsRead_ && read){
        readTags();
    }


    QVariant result;
    if(field==Global::Artist){
        result = QVariant(artist_);
    }else if(field==Global::AlbumField){
        result = QVariant(album_);
    }else if(field==Global::Title){
        result = QVariant(title_);
    }else if(field==Global::Genre){
        result = QVariant(genre_);
    }else if(field==Global::Comment){
        result = QVariant(artist_);
    }else if(field==Global::Year){
        result = QVariant(year_);
    }else if(field==Global::Track){
        result = QVariant(track_);
    }else if(field==Global::Length){
        result = QVariant(length_);
    }else if(field==Global::BitRate){
        result = QVariant(bitRate_);
    }else if(field==Global::SampleRate){
        result = QVariant(sampleRate_);
    }else if(field==Global::Channels){
        result = QVariant(channels_);
    }else if(field==Global::FileName){
        result = QVariant(fileInfo_.fileName());
    }
    return result;
}

void TagItem::changeName( const QString &newFullFileName ){

    fileInfo_ = QFileInfo( newFullFileName );
    setToolTip( 0, fileInfo_.absoluteFilePath() );

}

void TagItem::readTags(){

    tagIsRead_=true;

    TagLib::FileRef f( fileInfo_.absoluteFilePath().toStdString().c_str() );
    if( !f.isNull() || f.tag()  ){
        artist_ = f.tag()->artist().toCString();
        album_ = f.tag()->album().toCString();
        title_ = f.tag()->title().toCString();
        genre_ = f.tag()->genre().toCString();
        comment_ = f.tag()->comment().toCString();
        year_ = f.tag()->year();
        track_ = f.tag()->track();
        tagOk_=true;
    }
    if( f.audioProperties()!=0 ){
        length_ = f.audioProperties()->length();
        bitRate_ = f.audioProperties()->bitrate();
        sampleRate_ = f.audioProperties()->sampleRate();
        channels_ = f.audioProperties()->channels();
        audioPropertiesOk_=true;
    }

    unSavedChanges_ = false;
}


bool TagItem::saveTag(){

    TagLib::FileRef f( fileInfo_.absoluteFilePath().toStdString().c_str() );
    bool ok = false;
    if( !f.isNull() || f.tag()  ){
        f.tag()->setTrack(track_);
        f.tag()->setYear(year_);
        f.tag()->setAlbum(album_.toStdString().c_str());
        f.tag()->setArtist(artist_.toStdString().c_str());
        f.tag()->setTitle(title_.toStdString().c_str());
        f.tag()->setGenre(genre_.toStdString().c_str());
        f.tag()->setComment(comment_.toStdString().c_str());
        ok = f.save();
        unSavedChanges_ = !ok;
    }
    return ok;
}

void TagItem::clearTags(){
    artist_="";
    title_="";
    album_="";
    comment_="";
    genre_="";
    year_=0;
    track_=0;
    tagIsRead_=false;
    audioPropertiesOk_=false;
    tagOk_=false;
    length_=-1;
    bitRate_=-1;
    sampleRate_=-1;
    channels_=-1;
    unSavedChanges_=false;

}
