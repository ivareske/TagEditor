#ifndef TAGITEM_H
#define TAGITEM_H

#include <QtGui>
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include "globalFunctions.h"

class TagItem : public QTreeWidgetItem{
    //Q_OBJECT

public:

    TagItem(const QString &fullfile="", QTreeWidget *parent = 0, int type = QTreeWidgetItem::Type);
    TagItem( const TagItem &other );
    bool operator<(const QTreeWidgetItem &other) const;

    void clearTags();
    void readTags();    
    bool saveTag();
    void changeName( const QString &newFullFileName );    

    void setArtist( const QString &artist );
    void setAlbum( const QString &album );
    void setTitle( const QString &title );
    void setGenre( const QString &genre );
    void setComment( const QString &comment );
    void setTrack( uint track );
    void setYear( uint year );

    QString artist() const;
    QString title() const;
    QString album() const;
    QString comment() const;
    QString genre() const;
    uint year() const;
    uint track() const;
    uint length() const;
    uint bitRate() const;
    uint sampleRate() const;
    uint channels() const;

    bool unSavedChanges() const;
    QFileInfo fileInfo() const;
    bool tagIsRead() const;
    bool tagOk() const;
    bool audioPropertiesOk() const;    
    QVariant getTag( Global::TagField field, bool read = false );
    void setColumnData( const QList<Global::TagField> &columns, bool showFullFileName, bool readTags_ );
public slots:


    //protected:
private:    
    //bool setTag( Global::TagField field, const QVariant &tag );

    QFileInfo fileInfo_;
    QString artist_;
    QString title_;
    QString album_;
    QString comment_;
    QString genre_;
    uint year_;
    uint track_;
    uint length_;
    uint bitRate_;
    uint sampleRate_;
    uint channels_;
    bool tagOk_;
    bool audioPropertiesOk_;
    bool unSavedChanges_;

    bool tagIsRead_;    

};


#endif






