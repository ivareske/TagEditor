#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H
 
#include <QtGui>
#include <QtNetwork>
#include "TextViewer.h"
#include "Album.h"
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include "TableWidgetItem.h"
#include "MusicDatabase.h"
#include "MusicBrainz.h"
#include "Discogs.h"
#include "ui_searchDialog.h"


class SearchDialog : public QDialog, private Ui::SearchDialog{
    Q_OBJECT
 
public:

    SearchDialog(QWidget *parent = 0, QString apiKey="", QList<QFileInfo> *infosInput = 0 );    
    enum matchByFlag{ MATCHBYTRACK, MATCHBYTITLE, DONTMATCH, MATCHBYTRACKTITLE, MATCHBYTITLETRACK, MATCHBYFILENAME };
    enum columnFlag{ FILENAME, CURRENTTITLE, CURRENTTRACK, TITLE, TRACK, COMMENT  };
    enum itemCol{ ORIGIN, RESULT  };
		
	
private slots:
        void updateSortingEnabled( bool enabled );
	void search();
	void save();
	void showResult( QListWidgetItem* item );
	//void findMoreImages(QNetworkReply* reply);
	void setCover( QUrl url );
	void setCoverFromReply(QNetworkReply* reply);
	void showCover( int currentCoverInd );
	void showPrevCover();
	void showNextCover();
	void showAlbumInfo( Album a );		
	void saveCover( QUrl url );
	void saveCoverFromReply(QNetworkReply* reply);
        void moveRow( int from, int to, itemCol t );
	void moveRowUp();
	void moveRowTo();
	void moveRowDown();
	QString createCoverName( int ind, QString ext );	
	void saveCoverNow();
	void saveAllCoversNow();
	void resizeColumns(bool state);
	void resizeRows(bool state);
	void resizeHeader( QHeaderView *header, bool state );
	void updateCell( int row, int col );
	void matchBy(bool state);
	void unCheckMatchActions();
	void albumInfoContextMenu(const QPoint &p);
	int matchResult( QVariant toMatch, int matchCol );
	void deleteCells();
	void setResultItem( QString text, QString key );
	void insertBlankItem();
	void removeRow();
	void copyCells();
	void paste();
	void setNonEditable( int startrow, int stoprow );
	void checkRows();
	void databaseChanged( int ind );
	void showAlbumAndCover( Album album );
	void getResults( QHash<QString,Album> results );
	void gotAlbums( QHash<QString,Album> albums );
	void gotAlbum( Album a );


	
private:
	void closeEvent( QCloseEvent *event );
        void readSettings();
	int currentCoverInd;	
	QNetworkAccessManager downloadImageManager;
	QNetworkAccessManager saveCoverManager;
	QByteArray coverData;
	QSettings *settings;
	//QList<QFileInfo> *infos;
	bool matchByTrackChecked;
	bool matchByTitleChecked;
	bool matchByTitleTrackChecked;
	bool matchByTrackTitleChecked;
	bool matchByFileNameChecked;
	bool dontMatchChecked;
	bool autoResizeColumns;
	bool autoResizeRows;
	int nFiles;
	QList<int> originCols;
	QList<int> resultCols;
	QHash<QString,Album> Albums;
	QHash<QString,Album> Results;
	MusicDatabase *Database;
	QString api_key;
	QDir path;


};
 
 
#endif






