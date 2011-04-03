#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QtGui>
#include "TagItem.h"
#include "globalFunctions.h"
//#include "SetColumnsDialog.h"

class TagItem;

class TreeWidget : public QTreeWidget{
    Q_OBJECT
public:
    explicit TreeWidget(QWidget *parent = 0);
    enum dataType{ FULLFILE=32, LOGICAL=33 };
    //enum ColumnType{ FileName, Artist, Album, Title, Year, Track, Comment, Genre, Length, BitRate, SampleRate, Channels };
    bool showFullFileName() const;
    void setShowFullFileName( bool show );
    void addItem( QTreeWidgetItem *item );
    void addTopLevelItems( const QList<QTreeWidgetItem*> &items );
    void updateItems( QList<TagItem*> items );
    bool showTagInfo() const;
    void setShowTagInfo( bool show );
signals:

public slots:

private slots:
    void resizeColumns();
    void enableSorting(bool enable);
    void treeWidgetContextMenu(const QPoint &p);
    void updateFileNames( bool enable );    
    void updateShowTagInfo( bool enable );
    void editColumns();
    void setColumns();
private:
    bool showFullFileName_;
    bool showTagInfo_;
    QList<int> columns_;

};

#endif // TREEWIDGET_H
