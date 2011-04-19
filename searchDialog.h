#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QtGui>
#include <QtNetwork>
#include "TextViewer.h"
#include "Album.h"
//#include <tag.h>
//#include <taglib.h>
//#include <fileref.h>
#include "TagItem.h"
#include "TableWidgetItem.h"
#include "MusicDatabase.h"
#include "MusicBrainz.h"
#include "Discogs.h"
#include "ui_searchDialog.h"


class SearchDialog : public QDialog, private Ui::SearchDialog{
    Q_OBJECT

public:

    SearchDialog( const QList<TagItem*> &items, QWidget *parent = 0 );


private:
    enum MatchByFlag{ MATCH_BY_TRACK, MATCH_BY_TITLE, DONT_MATCH, MATCH_BY_TRACK_TITLE, MATCH_BY_TITLE_TRACK, MATCH_BY_FILE_NAME };
    enum ColumnFlag{ FILE_NAME, CURRENT_TITLE, CURRENT_TRACK, TITLE, TRACK, COMMENT  };
    enum ColumnType{ ORIGIN, RESULT };
    enum DataBaseType{ DISCOGS, MUSICBRAINZ };

    void closeEvent( QCloseEvent *event );
    void readSettings();


    int currentCoverInd;
    QNetworkAccessManager downloadImageManager;
    QNetworkAccessManager saveCoverManager;
    QByteArray coverData;
    QSettings *settings;

    bool matchByTrackChecked;
    bool matchByTitleChecked;
    bool matchByTitleTrackChecked;
    bool matchByTrackTitleChecked;
    bool matchByFileNameChecked;
    bool dontMatchChecked;
    bool autoResizeColumns;
    bool autoResizeRows;

    //int nFiles;
    QList<int> originCols_;
    QList<int> resultCols_;
    QHash<QString,Album> albums_;
    QHash<QString,Album> results_;
    MusicDatabase *database_;
    QString api_key_;
    QDir path_;
    QList<TagItem*> items_;

private slots:
    void setItems();
    void updateSortingEnabled( bool enabled );
    void search();
    void save();
    void showResult( QListWidgetItem* item );
    //void findMoreImages(QNetworkReply* reply);
    void setCover( const QUrl &url );
    void setCoverFromReply(QNetworkReply* reply);
    void showCover( int currentCoverInd );
    void showPrevCover();
    void showNextCover();
    void showAlbumInfo( const Album &a );
    void saveCover( const QUrl &url );
    void saveCoverFromReply(QNetworkReply* reply);
    void moveRow( int from, int to, ColumnType t );
    void moveRowUp();
    void moveRowTo();
    void moveRowDown();
    QString createCoverName( int ind, const QString &ext );
    void saveCoverNow();
    void saveAllCoversNow();
    void resizeColumns(bool state);
    void resizeRows(bool state);
    void resizeHeader( QHeaderView *header, bool state );
    void updateCell( int row, int col );
    void matchBy(bool state);
    void unCheckMatchActions();
    void albumInfoContextMenu(const QPoint &p);
    int matchResult( const QVariant &toMatch, int matchCol );
    void deleteCells();
    void setResultItem( const QString &text, const QString &key );
    void insertBlankItem();
    void removeRow();
    void copyCells();
    void paste();
    void setNonEditable( int startrow, int stoprow );
    void checkRows();
    void databaseChanged( int ind );
    void showAlbumAndCover( const Album &album );
    void getResults( const QHash<QString,Album> &results );
    void gotAlbums( const QHash<QString,Album> &albums );
    void gotAlbum( const Album &a );


};


#endif






