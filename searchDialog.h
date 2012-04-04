#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QtGui>
#include <QtNetwork>
#include "TextViewer.h"
#include "Album.h"
#include "TableWidget.h"
#include "TagItem.h"
#include "MusicDatabase.h"
#include "MusicBrainz.h"
#include "Discogs.h"
#include "ui_searchDialog.h"


class SearchDialog : public QDialog, private Ui::SearchDialog{
    Q_OBJECT

public:

    SearchDialog( const QList<TagItem*> &items, QWidget *parent = 0 );


private:

    void closeEvent( QCloseEvent *event );
    void readSettings();


    int currentCoverInd;
    QNetworkAccessManager downloadImageManager;
    QNetworkAccessManager saveCoverManager;
    QByteArray coverData;
    QSettings *settings;


    QHash<QString,Album> albums_;
    QHash<QString,Album> results_;
    MusicDatabase *database_;
    QString api_key_;
    QList<TagItem*> items_;
    TableWidget *albumInfo;

private slots:

    //fix cover downloading. keep downloaded covers
    /*
    void getCoverFromReply(QNetworkReply* reply);
    void downloadCover( const Album &album, const QUrl &url );
    */

    void setItems();
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
    QString createCoverName( int ind, const QString &ext );
    void saveCoverNow();
    void saveAllCoversNow();
    void setResultItem( const QString &text, const QString &key );
    void databaseChanged( int ind );
    void showAlbumAndCover( const Album &album );
    void getResults( const QHash<QString,Album> &results );
    void gotAlbums( const QHash<QString,Album> &albums );
    void gotAlbum( const Album &a );


};


#endif






