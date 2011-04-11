#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QtGui>
#include "TagItem.h"
#include "SetColumnsDialog.h"
#include "globalFunctions.h"


class TreeWidget : public QTreeWidget{
    Q_OBJECT
public:
    explicit TreeWidget(QWidget *parent = 0);
    enum dataType{ FULLFILE=32, LOGICAL=33 };
    //enum ColumnType{ FileName, Artist, Album, Title, Year, Track, Comment, Genre, Length, BitRate, SampleRate, Channels };
    bool showFullFileName() const;
    void setShowFullFileName( bool show );
    void addItem( QTreeWidgetItem *item );
    void updateItems( QList<TagItem*> items );
    bool showTagInfo() const;
    void setShowTagInfo( bool show );
    TagItem* tagItem( int index );
    void setColumnData( TagItem* item );
    QList<Global::TagField> columns() const;
    void setColumnsList( const QList<Global::TagField> &columns );

signals:

public slots:

private slots:
    void resizeColumns();
    void enableSorting(bool enable);
    void treeWidgetContextMenu(const QPoint &p);
    void updateFileNames( bool enable );    
    void updateShowTagInfo( bool enable );
    void setColumns();
    void editColumns();
private:
    bool showFullFileName_;
    bool showTagInfo_;
    QList<Global::TagField> columns_;
    QList<Global::TagField> defaultColumns_;

};

#endif // TREEWIDGET_H
