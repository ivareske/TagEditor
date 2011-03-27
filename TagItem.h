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
    enum TagField{ Artist, Title, Album, Comment, Genre, Year, Track, Length, BitRate, SampleRate, Channels };
    TagItem(const QString &fullfile="", int type = QTreeWidgetItem::Type, QTreeWidget *parent = 0);
    TagItem( const TagItem &other );
    void clearTags();
    void readTags();
    void changeName( const QString &newFullFileName );
    QVariant getTag( TagItem::TagField field );

    QString artist() const;
    QString title() const;
    QString album() const;
    QString comment() const;
    QString genre() const;
    int year() const;
    int track() const;
    int length() const;
    int bitRate() const;
    int sampleRate() const;
    int channels() const;

    QFileInfo fileInfo() const;
    bool tagIsRead() const;
    bool tagOk() const;
    bool audioPropertiesOk() const;
    void setTag( TagItem::TagField field, const QVariant &tag );
    void setColumnData();
public slots:


    //protected:
private:    

    QFileInfo fileInfo_;
    QString artist_;
    QString title_;
    QString album_;
    QString comment_;
    QString genre_;
    int year_;
    int track_;
    bool tagIsRead_;
    bool tagOk_;
    bool audioPropertiesOk_;

    int length_;
    int bitRate_;
    int sampleRate_;
    int channels_;

};


#endif






