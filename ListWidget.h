#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QtGui>
#include "TagItem.h"
#include "globalFunctions.h"

class ListWidget : public QListWidget{
    Q_OBJECT
public:
    enum dataType{ FULLFILE=32, LOGICAL=33 };
    explicit ListWidget(QWidget *parent = 0);
    bool showFullFileName() const;
    void setShowFullFileName( bool show );
    void addItem( QListWidgetItem *item );

signals:

public slots:

private slots:
    void listWidgetContextMenu(const QPoint &p);
    void updateFileNames( bool enable );
    void updateShowTagInfo( bool enable );
    void sortListWidget(const QString &string );

private:
    bool showFullFileName_;
    bool showTagInfo_;
};

#endif // LISTWIDGET_H
