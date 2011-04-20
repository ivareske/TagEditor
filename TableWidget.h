#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QtGui>
#include "TagItem.h"
#include "TableWidgetItem.h"
#include "Album.h"
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include <algorithm>

class TableWidget : public QTableWidget{    
    Q_OBJECT

public:

    explicit TableWidget(QWidget *parent = 0);
    ~TableWidget();
    enum ColumnFlag{ FILE_NAME, CURRENT_TITLE, CURRENT_TRACK, TITLE, TRACK, COMMENT  };
    enum ColumnType{ ORIGIN, RESULT };
    enum MatchByFlag{ MATCH_BY_TRACK, MATCH_BY_TITLE, DONT_MATCH, MATCH_BY_TRACK_TITLE, MATCH_BY_TITLE_TRACK, MATCH_BY_FILE_NAME };

signals:

public slots:
    void setItems( const QList<TagItem*> &items_ );
    void showAlbumInfo( const Album &a );

private slots:
    void contextMenu(const QPoint &p, bool init=false );
    void verticalContextMenu(const QPoint &p, bool init=false );
    void moveRow( int from, int to, ColumnType t );
    void moveRowUp();
    void moveRowTo();
    void moveRowDown();
    void matchBy(bool state);
    int matchResult( const QVariant &toMatch, int matchCol );
    void deleteCells();
    void insertBlankRow();
    void removeCurrentRow();
    void copyCells();
    void cut();
    void insertCutRows();
    void paste();
    void setNonEditable( int startrow, int stoprow );
    void checkRows();
    void resizeColumns(bool state);
    void resizeRows(bool state);
    void resizeHeader( QHeaderView *header, bool state );
    void updateCell( int row, int col );
    void unCheckMatchActions();
    void enableSorting( bool enable );
private:
    QList<int> originCols_;
    QList<int> resultCols_;
    QAction* resizeColumnAction;
    QAction* resizeRowAction;
    QAction* matchByTrackAction;
    QAction* matchByTitleAction;
    QAction* dontMatchAction;
    QAction* matchByTrackTitleAction;
    QAction* matchByTitleTrackAction;
    QAction* matchByFileNameAction;


    QSettings *settings;
    QList< QList<TableWidgetItem*> > cutRows_;


};

#endif // TABLEWIDGET_H
