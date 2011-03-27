#include "TagItem.h"

 
TagItem::TagItem( const QString &fullfile, int type, QTreeWidget *parent ) : QTreeWidgetItem( parent, type)    {

        artist_="";
        title_="";
        album_="";
        comment_="";
        genre_="";
        year_=-1;
        track_=-1;
        fileInfo_ = QFileInfo( fullfile );
        tagIsRead_=false;
        tagOk_=false;
        audioPropertiesOk_=false;
	
        length_=-1;
        bitRate_=-1;
        sampleRate_=-1;
        channels_=-1;

        setToolTip( 0, fullfile );
        setColumnData();



}

void TagItem::setColumnData(){

    setData( Global::ArtistColumn, Qt::DisplayRole, artist_ );
    setData( Global::TitleColumn, Qt::DisplayRole, title_ );
    setData( Global::AlbumColumn, Qt::DisplayRole, album_ );
    //setData( Global::CommentColumn, Qt::DisplayRole, comment_ );
    setData( Global::GenreColumn, Qt::DisplayRole, genre_ );
    if( track_==-1 ){
        setData( Global::TrackColumn, Qt::DisplayRole, "" );
    }else{
        setData( Global::TrackColumn, Qt::DisplayRole, track_ );
    }
    if( year_==-1 ){
        setData( Global::YearColumn, Qt::DisplayRole, "" );
    }else{
        setData( Global::YearColumn, Qt::DisplayRole, year_ );
    }
    if( length_==-1 ){
        setData( Global::LengthColumn, Qt::DisplayRole, "" );
    }else{
        setData( Global::LengthColumn, Qt::DisplayRole, length_ );
    }
    if( bitRate_==-1 ){
        setData( Global::BitRateColumn, Qt::DisplayRole, "" );
    }else{
        setData( Global::BitRateColumn, Qt::DisplayRole, bitRate_ );
    }
    if( sampleRate_==-1 ){
        setData( Global::SampleRateColumn, Qt::DisplayRole, "" );
    }else{
        setData( Global::SampleRateColumn, Qt::DisplayRole, sampleRate_ );
    }
    if( bitRate_==-1 ){
        setData( Global::ChannelsColumn, Qt::DisplayRole, "" );
    }else{
        setData( Global::ChannelsColumn, Qt::DisplayRole, channels_ );
    }

/*
    QString filename = fileInfo_.fileName();
    if( showFullFileName ){
        filename = fileInfo_.absoluteFilePath();
    }
    setData( Global::FileNameColumn, Qt::DisplayRole, filename );
*/
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

int TagItem::year() const{
    return year_;
}

int TagItem::track() const{
    return track_;
}

int TagItem::length() const{
    return length_;
}
int TagItem::bitRate() const{
    return bitRate_;
}
int TagItem::sampleRate() const{
    return sampleRate_;
}
int TagItem::channels() const{
    return channels_;
}


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

    setToolTip( 0, fileInfo_.absoluteFilePath() );
    setColumnData();

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

QVariant TagItem::getTag( TagItem::TagField field ){
	
        if(!tagIsRead_){
		readTags();
	}
	
	QVariant result;
        if(field==Artist){
                result = QVariant(artist_);
        }else if(field==Album){
                result = QVariant(album_);
        }else if(field==Title){
                result = QVariant(title_);
        }else if(field==Genre){
                result = QVariant(genre_);
        }else if(field==Comment){
                result = QVariant(artist_);
        }else if(field==Year){
                result = QVariant(year_);
        }else if(field==Track){
                result = QVariant(track_);
        }else if(field==Length){
                result = QVariant(length_);
        }else if(field==BitRate){
                result = QVariant(bitRate_);
        }else if(field==SampleRate){
                result = QVariant(sampleRate_);
        }else if(field==Channels){
                result = QVariant(channels_);
	}
	return result;
}

void TagItem::changeName( const QString &newFullFileName ){
	
        fileInfo_ = QFileInfo( newFullFileName );
        //setText( 0, fileInfo_.fileName() );
        setColumnData();
	
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
	if( f.audioProperties()!=NULL ){				
                length_ = f.audioProperties()->length();
                bitRate_ = f.audioProperties()->bitrate();
                sampleRate_ = f.audioProperties()->sampleRate();
                channels_ = f.audioProperties()->channels();
                audioPropertiesOk_=true;
	}
        setColumnData();

}

void TagItem::setTag( TagItem::TagField field, const QVariant &tag ){
	
	bool ok;
        if(field==Artist){
		ok = tag.canConvert(QVariant::String);
                artist_ = tag.toString();
        }else if(field==Album){
		ok = tag.canConvert(QVariant::String);
                album_ = tag.toString();
        }else if(field==Title){
		ok = tag.canConvert(QVariant::String);
                title_ = tag.toString();
        }else if(field==Genre){
		ok = tag.canConvert(QVariant::String);
                genre_ = tag.toString();
        }else if(field==Comment){
		ok = tag.canConvert(QVariant::String);
                comment_ = tag.toString();
        }else if(field==Year){
		ok = tag.canConvert(QVariant::UInt);
                year_ = tag.toInt();
        }else if(field==Track){
		ok = tag.canConvert(QVariant::UInt);
                track_ = tag.toInt();
	}
	//return ok;
        setColumnData();
}


void TagItem::clearTags(){
        artist_="";
        title_="";
        album_="";
        comment_="";
        genre_="";
	//year=0;
	//track=0;
        tagIsRead_=false;
        audioPropertiesOk_=false;
        tagOk_=false;
        length_=-1;
        bitRate_=-1;
        sampleRate_=-1;
        channels_=-1;
        setColumnData();
}
