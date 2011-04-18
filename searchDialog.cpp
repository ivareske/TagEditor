#include "SearchDialog.h"


SearchDialog::SearchDialog( QWidget *parent, QString apiKey, QList<QFileInfo> *infos ) : QDialog(parent){

    setupUi(this); // this sets up GUI

    settings = new QSettings("TagEditor.ini",QSettings::IniFormat,this);

    api_key = apiKey;
    //infos = infosInput;
    currentCoverInd=0;

    originCols <<0<<1<<2<<5;
    resultCols <<3<<4;

    nFiles = infos->size();
    AlbumInfo->setRowCount(infos->size());
    AlbumInfo->setColumnCount(6);
    QStringList headers;
    headers << "File" << "Current title" << "Current track" << "Found title" << "Found track"<<"Current comments";
    AlbumInfo->setHorizontalHeaderLabels(headers);

    QString artist = "";
    QString album = "";
    QString year = "";
    QString genre = "";

    bool enabled = AlbumInfo->isSortingEnabled();
    AlbumInfo->setSortingEnabled(false);
    for(int i=0;i<infos->size();i++){
	
        QString fullfile = infos->at(i).filePath();
        if(i==0){
            path = QDir( infos->at(i).absolutePath() );
        }
        TableWidgetItem *newItem = new TableWidgetItem;
        newItem->setText( infos->at(i).fileName() );
        newItem->setData( Qt::UserRole, QVariant(fullfile) );
        QFont font = newItem->font();font.setBold(true);
        newItem->setFont(font);
        newItem->setFlags( newItem->flags() | Qt::ItemIsUserCheckable );
        newItem->setFlags( newItem->flags() ^ Qt::ItemIsEditable );
        newItem->setCheckState(Qt::Checked);
        AlbumInfo->setItem(i, FILENAME, newItem);

        TagLib::FileRef f( fullfile.toStdString().c_str() );
        QString tagTitle="Could not get tag title...";
        QString tagTrack="Could not get tag track...";
        QString tagComment="Could not get tag comment...";
        if( !f.isNull() && f.tag()  ){
            tagTitle = QString( f.tag()->title().toCString() );
            tagTrack = QString::number( f.tag()->track() );
            tagComment = QString( f.tag()->comment().toCString() );
            if(artist.isEmpty()){
                artist = QString( f.tag()->artist().toCString());
            }
            if(album.isEmpty()){
                album = QString( f.tag()->album().toCString());
            }
            if(year.isEmpty()){
                year = QString::number( f.tag()->year() );
            }
            if(genre.isEmpty()){
                genre = QString( f.tag()->genre().toCString());
            }
            if(i==0){
                SearchStr->setText( artist );
                SearchTitle->setText( album );
            }
        }

        TableWidgetItem *newItem1 = new TableWidgetItem;
        newItem1->setText( tagTitle );
        newItem1->setFont(font);
        newItem1->setFlags( newItem1->flags() ^ Qt::ItemIsEditable );
        AlbumInfo->setItem(i, CURRENTTITLE, newItem1);

        TableWidgetItem *newItem2 = new TableWidgetItem;
        newItem2->setText( tagTrack );
        newItem2->setFont(font);
        newItem2->setFlags( newItem2->flags() ^ Qt::ItemIsEditable );
        AlbumInfo->setItem(i, CURRENTTRACK, newItem2);

        TableWidgetItem *newItem3 = new TableWidgetItem;
        newItem3->setText( tagComment );
        AlbumInfo->setItem(i, COMMENT, newItem3);

        TableWidgetItem *newItem4 = new TableWidgetItem;
        AlbumInfo->setItem(i, TITLE, newItem4);
        TableWidgetItem *newItem5 = new TableWidgetItem;
        AlbumInfo->setItem(i, TRACK, newItem5);
    }
    AlbumInfo->setSortingEnabled(enabled);
    Artist->setText(artist);
    AlbumField->setText(album);
    Year->setText(year);
    Genre->setText(genre);

    ArtistLabel->setText("Artist (current: "+artist+" ): ");
    AlbumLabel->setText("Album (current: "+album+" ): ");
    GenreLabel->setText("Genre (current: "+genre+" ): ");
    YearLabel->setText("Year (current: "+year+" ): ");


    // signals/slots mechanism in action		
    connect( PrevCoverButton, SIGNAL( clicked() ), this, SLOT( showPrevCover() ) );
    connect( NextCoverButton, SIGNAL( clicked() ), this, SLOT( showNextCover() ) );
    connect( SearchButton, SIGNAL( clicked() ), this, SLOT( search() ) );
    connect( SaveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
    connect(&downloadImageManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(setCoverFromReply(QNetworkReply*)));
    connect(&saveCoverManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(saveCoverFromReply(QNetworkReply*)));
    connect( SearchResults, SIGNAL( itemClicked( QListWidgetItem * )  ), this, SLOT( showResult(QListWidgetItem *) ) );
    connect( AlbumInfo, SIGNAL( cellChanged( int, int )  ), this, SLOT( updateCell(int, int) ) );
    connect( DatabaseComboBox, SIGNAL( currentIndexChanged( int )  ), this, SLOT( databaseChanged(int) ) );


    AlbumInfo->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(AlbumInfo, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(albumInfoContextMenu(const QPoint &)));


    //save cover now action
    QAction* saveCoverAction = new QAction(tr("Save current cover now"), this);
    //saveCoverAction->setShortcut(tr("Ctrl+U"));saveCoverAction->setShortcutContext(Qt::WidgetShortcut);
    connect(saveCoverAction, SIGNAL(triggered()), this, SLOT(saveCoverNow()));
    QAction* saveAllCoversAction = new QAction(tr("Save all covers now"), this);
    //saveAllCoversAction->setShortcut(tr("Ctrl+U"));saveAllCoversAction->setShortcutContext(Qt::WidgetShortcut);
    connect(saveAllCoversAction, SIGNAL(triggered()), this, SLOT(saveAllCoversNow()));
    cover->setContextMenuPolicy(Qt::ActionsContextMenu);
    cover->addAction(saveCoverAction);
    cover->addAction(saveAllCoversAction);


    readSettings();
}

void SearchDialog::readSettings(){

    this->resize(settings->value("SearchDialog/size", QSize(700, 600)).toSize());
    this->move( settings->value("SearchDialog/pos", QPoint(200, 200)).toPoint() );
    coverFormat->setText( settings->value("SearchDialog/coverFormat","%artist% - %album%").toString() );
    CoverFormatCheckBox->setChecked( settings->value("SearchDialog/CoverFormatCheckBox",true).toBool() );
    SaveCoverCheckBox->setChecked( settings->value("SearchDialog/saveCover",false).toBool() );
    SaveAllCoversCheckBox->setChecked( settings->value("SearchDialog/saveAllCovers",true).toBool() );
    ArtistLabel->setChecked( settings->value("SearchDialog/ArtistLabel",true).toBool() );
    AlbumLabel->setChecked( settings->value("SearchDialog/AlbumLabel",true).toBool() );
    GenreLabel->setChecked( settings->value("SearchDialog/GenreLabel",true).toBool() );
    YearLabel->setChecked( settings->value("SearchDialog/YearLabel",true).toBool() );
    RolesLabel->setChecked( settings->value("SearchDialog/RolesLabel",false).toBool() );
    FormatLabel->setChecked( settings->value("SearchDialog/FormatLabel",false).toBool() );
    CountryLabel->setChecked( settings->value("SearchDialog/CountryLabel",false).toBool() );
    LabelLabel->setChecked( settings->value("SearchDialog/LabelLabel",false).toBool() );
    NotesLabel->setChecked( settings->value("SearchDialog/NotesLabel",false).toBool() );
    ClearPreviousComentsCheckBox->setChecked( settings->value("SearchDialog/clearPreviousComents",false).toBool() );
    DownloadImmediately->setChecked( settings->value("SearchDialog/DownloadImmediately",true).toBool() );
    autoResizeColumns = settings->value("SearchDialog/autoResizeColumns",true).toBool();
    resizeColumns(autoResizeColumns);
    autoResizeRows = settings->value("SearchDialog/autoResizeRows",false).toBool();
    resizeRows(autoResizeRows);
    AlbumInfo->setSortingEnabled( settings->value("SearchDialog/sortingEnabled",true).toBool() );
    if( AlbumInfo->isSortingEnabled() ){
        int sortColumn = settings->value("SearchDialog/sortColumn",(int)CURRENTTRACK).toInt();
        Qt::SortOrder sortOrder = (Qt::SortOrder)settings->value("discogs/sortOrder", (int)Qt::AscendingOrder ).toInt();
        AlbumInfo->sortByColumn( sortColumn, sortOrder );
        AlbumInfo->horizontalHeader()->setSortIndicator(sortColumn, sortOrder);
        AlbumInfo->horizontalHeader()->setSortIndicatorShown(true);
        AlbumInfo->horizontalHeader()->setClickable(true);
    }
    matchByTrackChecked = settings->value("SearchDialog/matchByTrack",false).toBool();
    matchByTitleChecked = settings->value("SearchDialog/matchByTitle",false).toBool();
    dontMatchChecked = settings->value("SearchDialog/dontMacth",true).toBool();
    matchByTrackTitleChecked = settings->value("SearchDialog/matchByTrackTitle",false).toBool();
    matchByTitleTrackChecked = settings->value("SearchDialog/matchByTitleTrack",false).toBool();
    matchByFileNameChecked = settings->value("SearchDialog/matchByFileName",false).toBool();
    DatabaseComboBox->setCurrentIndex( settings->value("SearchDialog/DatabaseComboBox",0).toInt() );
    databaseChanged( DatabaseComboBox->currentIndex() );
}


void SearchDialog::search(){
    //on search button press


    if( SearchStr->text().isEmpty() && SearchTitle->text().isEmpty() ){
        QMessageBox::critical(this, "Error",
                              "Empty search not allowed...", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QString url;
    if(DatabaseComboBox->currentText()=="Discogs"){
        url = "http://www.discogs.com/search?type=all&q="+SearchStr->text()+"+"+SearchTitle->text()+"&f=xml&api_key="+api_key;
    }else{
        url = "http://musicbrainz.org/ws/1/release/?type=xml&artist="+SearchStr->text()+"&title="+SearchTitle->text();
    }
    Database->search( url, DownloadImmediately->isChecked() );
    
}

void SearchDialog::getResults( QHash<QString,Album> results ){
    //called after Database->search is finished

    Results = results;
    SearchResults->clear();
    Albums.clear();
    if( DownloadImmediately->isChecked() ){
        Database->downloadAllAlbums( Results ); //downloads and appends to Albums
        //gotAlbums is then called, albums are sorted and result items created
    }else{
        QList<Album> tmpresults = Results.values();
        std::sort( tmpresults.begin(), tmpresults.end(), Global::compareAlbum );
        for(int i=0;i<tmpresults.size();i++){
            if( DatabaseComboBox->currentText()=="Discogs" ){
                setResultItem( tmpresults[i].title(), tmpresults[i].key() );
            }else if( DatabaseComboBox->currentText()=="MusicBrainz" ){
                setResultItem( tmpresults[i].displayArtist()+" - "+tmpresults[i].title() +"("+tmpresults[i].format()+","+QString::number(tmpresults[i].year())+")", tmpresults[i].key() );
            }
        }
    }

    info->setText("Found "+QString::number(Results.size())+" results");


}


void SearchDialog::setResultItem( QString text, QString key ){

    bool enabled = AlbumInfo->isSortingEnabled();
    AlbumInfo->setSortingEnabled(false);
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(text);
    newItem->setData(Qt::UserRole,key);
    SearchResults->addItem( newItem );
    AlbumInfo->setSortingEnabled(enabled);
}


void SearchDialog::showResult( QListWidgetItem* item ){
    //called when row changed in listwidget SearchResults

    if(!item){
        return;
    }
    if( Albums.contains(item->data(Qt::UserRole).toString()) ){
        qDebug()<<"album is already downloaded";
        showAlbumAndCover( Albums[item->data(Qt::UserRole).toString()] );
    }else{
        qDebug()<<"downloading album";
        Database->downloadAlbum( Results[item->data(Qt::UserRole).toString()] );
    }
}


void SearchDialog::gotAlbums( QHash<QString,Album> albums ){

    Albums = albums;
    QList<Album> tmpalbums = Albums.values();
    std::sort( tmpalbums.begin(), tmpalbums.end(), Global::compareAlbum );
    SearchResults->clear();
    for(int i=0;i<tmpalbums.size();i++){
        setResultItem( tmpalbums[i].displayArtist()+" - "+tmpalbums[i].title()+" ("+tmpalbums[i].format()+","+QString::number(tmpalbums[i].year())+")", tmpalbums[i].key() );
    }
    info->setText("Found "+QString::number(tmpalbums.size())+" results");

}

void SearchDialog::showAlbumAndCover( Album album ){

    if(album.images().size()==0){
        cover->setText("No cover found...");
        coverLabel->setText("");
    }else{
        showCover( 0 );
    }    
    showAlbumInfo( album );

}

void SearchDialog::showAlbumInfo( Album a ){

    int n = nFiles;
    if(a.songs().size()>nFiles){
        n = a.songs().size();
    }
    qDebug()<<nFiles<<n;
    AlbumInfo->setRowCount(n);
    bool enabled = AlbumInfo->isSortingEnabled();
    AlbumInfo->setSortingEnabled(false);
    setNonEditable( nFiles, n );
    /*int n = AlbumInfo->rowCount();
	//qDebug()<<"updating table:"<<a.songs.size();
	if(n<a.songs.size()){
                AlbumInfo->setRowCount(a.songs.size());
		setNonEditable( n, a.songs.size() );
	}
	*/

    Artist->setText(a.artists().join(", "));
    AlbumField->setText(a.title());
    Genre->setText(a.genre());
    qDebug()<<"a.year()"<<a.year();
    Year->setText(QString::number(a.year()));
    Format->setText( a.format() );
    Country->setText( a.country() );
    Label->setText( a.label() );
    if( DatabaseComboBox->currentText()=="Discogs" ){
        Roles->setText( a.roles().join(". ") );        
        Notes->setText( a.notes() );
    }
    //delete old results
    for(int i=0;i<AlbumInfo->rowCount();i++){
        delete AlbumInfo->takeItem(i,TITLE);
        delete AlbumInfo->takeItem(i,TRACK);
    }

    int k=0;
    QList<Song> leftOverSongs;
    QList<Song> songs = a.songs();
    while( songs.size()>0 ){
        int row;
        //qDebug()<<a.songs[0].title;
        if(matchByTrackChecked){
            row = matchResult( songs[0].track(), CURRENTTRACK );
        }else if(matchByTitleChecked){
            row = matchResult( songs[0].title(), CURRENTTITLE );
        }else if(matchByTitleTrackChecked){
            row = matchResult( songs[0].title(), CURRENTTITLE );
            if(row==-1){
                row = matchResult( songs[0].track(), CURRENTTRACK );
            }
            if(row==-1){
                row = matchResult( songs[0].title(), FILENAME );
            }
        }else if(matchByTrackTitleChecked){
            row = matchResult( songs[0].track(), CURRENTTRACK );
            if(row==-1){
                row = matchResult( songs[0].title(), CURRENTTITLE );
            }
            if(row==-1){
                row = matchResult( songs[0].title(), FILENAME );
            }
        }else if(matchByFileNameChecked){
            row = matchResult( songs[0].title(), FILENAME );
        }else if(dontMatchChecked){;
            row = k;
            k++;
        }else{
            row = k;
            k++;
        }
        if(row==-1){
            leftOverSongs.append(songs[0]);
            songs.removeAt(0);
            continue;
        }

        TableWidgetItem *newItem = new TableWidgetItem;
        newItem->setText(songs[0].title());
        QFont f = newItem->font();f.setPointSize(10);
        newItem->setFont(f);
        AlbumInfo->setItem(row, TITLE, newItem);

        TableWidgetItem *newItem1 = new TableWidgetItem;
        newItem1->setText(songs[0].track());
        newItem1->setFont(f);
        AlbumInfo->setItem(row, TRACK, newItem1);

        songs.removeAt(0);
    }
    for(int i=0;i<leftOverSongs.size();i++){
        int n = AlbumInfo->rowCount();
        int row=n+1;
        for(int j=0;j<n;j++){
            if(!AlbumInfo->item(j,TITLE)){
                row=j;
                break;
            }
        }
        //qDebug()<<leftOverSongs[i].title<<" leftover, put in row "<<row;
        TableWidgetItem *newItem = new TableWidgetItem;
        newItem->setText(leftOverSongs[i].title());
        QFont f = newItem->font();f.setPointSize(10);
        newItem->setFont(f);
        AlbumInfo->setItem(row, TITLE, newItem);

        TableWidgetItem *newItem1 = new TableWidgetItem;
        newItem1->setText(leftOverSongs[i].track());
        newItem1->setFont(f);
        AlbumInfo->setItem(row, TRACK, newItem1);
    }
    AlbumInfo->setSortingEnabled(enabled);
}

void SearchDialog::showPrevCover(){

    currentCoverInd--;
    showCover( currentCoverInd );
}

void SearchDialog::showNextCover(){
    currentCoverInd++;
    showCover( currentCoverInd );
}

void SearchDialog::showCover( int ind ){

    int aind = SearchResults->currentRow();
    if(aind==-1){
        return;
    }
    Album album = Albums[SearchResults->currentItem()->data(Qt::UserRole).toString()];

    QList<QUrl> images = album.images();
    int n = images.size();
    //qDebug()<<"Trying to set cover "<<ind<<" of "<<n;
    if( n==0 ){
        currentCoverInd=0;
        return;
    }
    if( ind<0 ){
        ind=0;
    }
    if( ind>=n ){
        ind=n-1;
    }
    setCover( images[ind] );
    //qDebug()<<"Set cover "<<ind<<" of "<<n;
    currentCoverInd = ind;
    coverLabel->setText("Cover "+QString::number(ind+1)+" of "+QString::number(n));
}

void SearchDialog::saveCoverNow(){
    int ind = SearchResults->currentRow();
    if(ind==-1){
        return;
    }
    Album album = Albums[SearchResults->currentItem()->data(Qt::UserRole).toString()];
    if(album.images().size()==0){
        return;
    }
    QList<QUrl> images = album.images();
    saveCover( images[currentCoverInd] );
}


void SearchDialog::saveAllCoversNow(){
    int ind = SearchResults->currentRow();
    if(ind==-1){
        return;
    }
    Album album = Albums[SearchResults->currentItem()->data(Qt::UserRole).toString()];
    if(album.images().size()==0){
        return;
    }
    QList<QUrl> images = album.images();
    for(int i=0;i<images.size();i++){
        saveCover( images[i] );
    }
}

void SearchDialog::setCoverFromReply(QNetworkReply* reply){

    QByteArray data = reply->readAll();
    QPixmap p;
    p.loadFromData(data);
    //p=p.scaled(QSize(150,150));
    p=p.scaled(cover->size());
    cover->setPixmap(p);
    reply->deleteLater();
}


void SearchDialog::setCover( QUrl url ){

    //qDebug()<<"showing cover"<<url;
    downloadImageManager.get(QNetworkRequest(url));

}

void SearchDialog::saveCover( QUrl url ){
    saveCoverManager.get(QNetworkRequest(url));
}

void SearchDialog::saveCoverFromReply(QNetworkReply* reply){
    //image to be saved must exist in Album.images

    QByteArray data = reply->readAll();
    QImage p;
    p.loadFromData(data);
    p.convertToFormat(QImage::Format_RGB32);
    reply->deleteLater();

    int aind = SearchResults->currentRow();
    if(aind==-1){
        return;
    }
    Album album = Albums[SearchResults->currentItem()->data(Qt::UserRole).toString()];
    QList<QUrl> images = album.images();
    QString url = reply->url().toString();
    int cInd=-1;
    for(int i=0;i<images.size();i++){
        if(url==images[i].toString()){
            cInd=i;
            break;
        }
    }
    if(cInd==-1){
        return;
    }

    QString name = images[cInd].toString();
    //qDebug()<<name;
    int ind = name.lastIndexOf("/");
    if(ind==-1){
        ind = name.lastIndexOf("\\");
    }
    name = name.remove(0,ind+1);
    if(CoverFormatCheckBox->isChecked()){
        int suffInd = name.lastIndexOf(".");
        QString ext = name.right(name.size()-suffInd);
        name = createCoverName( cInd+1, ext );
    }
    //qDebug()<<name;
    name = path.absolutePath()+"/"+name;
    int quality = settings->value("discogs/coverQuality",-1).toInt();
    bool ok = p.save( name, 0, quality );
    if(!ok){
        QMessageBox::critical(this, "Could not save cover",
                              "Could not save cover "+url+" as "+name, QMessageBox::Ok, QMessageBox::Ok);
    }else{
        qDebug()<<"saved"<<name;
    }
}

QString SearchDialog::createCoverName( int ind, QString ext ){

    QString name = coverFormat->text();
    name.replace( "%artist%", Artist->text() );
    name.replace( "%album%", AlbumField->text() );
    name.replace( "%year%", Year->text() );
    name.replace( "%Genre%", Genre->text() );
    name = name + QString::number(ind) + ext;
    return name;
}


void SearchDialog::moveRowTo(){

    bool ok;
    int n = AlbumInfo->rowCount();
    int row = QInputDialog::getInt(this, "",
                                   tr("Move to row:"), 1, 1, n, 1, &ok)-1; // -1,let rows start at 1 instead of 0 for user
    if(!ok){
        return;
    }
    int current = AlbumInfo->currentRow();
    if( row<0 || row>=n ){
        QMessageBox::critical(this, "Move row "+QString::number(current)+" to:",
                              "Row nr out of bounds",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    QAction *action = qobject_cast<QAction *>(sender());
    itemCol t = (itemCol)action->data().toInt();
    moveRow( current, row, t );
}

void SearchDialog::moveRow( int from, int to, itemCol t ){
    //move current row to row

    int n = AlbumInfo->rowCount();
    if( n==0 ){
        return;
    }
    if( from<0 || to<0	|| from>=n || to>=n || from==to ){
        return;
    }
    QList<int> inds;
    if(t==ORIGIN){
        inds = originCols;
        qDebug()<<"moving origin columns";
    }else{
        qDebug()<<"moving result columns";
        inds = resultCols;
    }
    bool enabled = AlbumInfo->isSortingEnabled();
    AlbumInfo->setSortingEnabled(false);
    for(int i=0;i<inds.size();i++){
        int col = inds[i];
        QList<TableWidgetItem*> tmp;
        for(int row=0;row<n;row++){
            tmp << (TableWidgetItem*)AlbumInfo->takeItem( row, col );
        }
        qDebug()<<"moving column "<<col<<" from "<<from<<"to "<<to;
        tmp.insert( to, tmp.takeAt(from) );
        for(int row=0;row<n;row++){
            AlbumInfo->setItem( row, col, tmp.at(row) );
        }
    }
    AlbumInfo->setSortingEnabled(enabled);
}

void SearchDialog::moveRowUp(){
    QAction *action = qobject_cast<QAction *>(sender());
    itemCol t = (itemCol)action->data().toInt();
    int current = AlbumInfo->currentRow();
    moveRow( current, current-1, t );
}

void SearchDialog::moveRowDown(){
    QAction *action = qobject_cast<QAction *>(sender());
    itemCol t = (itemCol)action->data().toInt();
    int current = AlbumInfo->currentRow();
    moveRow( current, current+1, t );
}


void SearchDialog::closeEvent( QCloseEvent *event ){

    settings->beginGroup("SearchDialog");
    settings->setValue( "saveCover", SaveCoverCheckBox->isChecked() );
    settings->setValue( "saveAllCovers", SaveAllCoversCheckBox->isChecked() );
    settings->setValue( "pos", this->pos() );
    settings->setValue( "size", this->size() );
    settings->setValue( "coverFormat", coverFormat->text() );
    settings->setValue( "CoverFormatCheckBox", CoverFormatCheckBox->isChecked() );
    settings->setValue( "ArtistLabel", ArtistLabel->isChecked() );
    settings->setValue( "AlbumLabel", AlbumLabel->isChecked() );
    settings->setValue( "GenreLabel", GenreLabel->isChecked() );
    settings->setValue( "YearLabel", YearLabel->isChecked() );
    settings->setValue( "RolesLabel", RolesLabel->isChecked() );
    settings->setValue( "FormatLabel", FormatLabel->isChecked() );
    settings->setValue( "CountryLabel", CountryLabel->isChecked() );
    settings->setValue( "LabelLabel", LabelLabel->isChecked() );
    settings->setValue( "NotesLabel", NotesLabel->isChecked() );
    settings->setValue( "matchByTrack", matchByTrackChecked );
    settings->setValue( "matchByTitle", matchByTitleChecked );
    settings->setValue( "DownloadImmediately", DownloadImmediately->isChecked() );
    settings->setValue( "dontMacth", dontMatchChecked );
    settings->setValue( "matchByTrackTitle", matchByTrackTitleChecked );
    settings->setValue( "matchByTitleTrack", matchByTitleTrackChecked );
    settings->setValue( "matchByFileName", matchByFileNameChecked );
    settings->setValue( "clearPreviousComents", ClearPreviousComentsCheckBox->isChecked() );
    settings->setValue( "autoResizeColumns", autoResizeColumns );
    settings->setValue( "autoResizeRows", autoResizeRows );
    settings->setValue( "DatabaseComboBox", DatabaseComboBox->currentIndex() );
    settings->setValue( "sortColumn", AlbumInfo->horizontalHeader()->sortIndicatorSection() );
    settings->setValue( "sortOrder", (int)AlbumInfo->horizontalHeader()->sortIndicatorOrder() );
    settings->setValue( "sortingEnabled", AlbumInfo->isSortingEnabled() );
    settings->endGroup();
    settings->sync();

}


int SearchDialog::matchResult( QVariant toMatch, int matchCol ){
    //qDebug()<<"matching";
    int res=-1;
    for(int i=0;i<AlbumInfo->rowCount();i++){
        if( AlbumInfo->item(i,TITLE) || AlbumInfo->item(i,TRACK) || !AlbumInfo->item(i,matchCol) ){
            //if an item already set there, or item to compare to doesn`t exist, continue
            continue;
        }
        if( (columnFlag)matchCol==CURRENTTRACK ){
            //compare track as int if can be converted, else as strings
            bool ok1,ok2;
            int comp1 = AlbumInfo->item(i,matchCol)->text().toInt(&ok1);
            int comp2 = toMatch.toInt(&ok2);
            if( ok1 && ok2){
                if( comp1==comp2 ){
                    //qDebug()<<"int "<<comp1<<"=="<<comp2;
                    res = i;
                    break;
                }
            }else if( AlbumInfo->item(i,matchCol)->text()==toMatch ){
                //qDebug()<<"string "<<AlbumInfo->item(i,matchCol)->text()<<"=="<<toMatch.toString();
                res = i;
                break;
            }
        }else{
            //compare title or filename by contains
            if(AlbumInfo->item(i,matchCol)->text().contains( toMatch.toString() )){
                //qDebug()<<"string "<<AlbumInfo->item(i,matchCol)->text()<<"contains"<<toMatch.toString();
                res = i;
                break;
            }
        }

    }
    //qDebug()<<"matched "<<toMatch.toString()<<" to row "<<res;
    return res;
}



void SearchDialog::setNonEditable( int startrow, int stoprow ){

    bool enabled = AlbumInfo->isSortingEnabled();
    AlbumInfo->setSortingEnabled(false);
    for(int i=0;i<(int)TITLE;i++){
        for(int j=startrow;j<stoprow;j++){
            if( j>=AlbumInfo->rowCount() ){
                continue;
            }
            if( !AlbumInfo->item(j,i) ){
                TableWidgetItem *item = new TableWidgetItem;
                item->setFlags( item->flags() ^ Qt::ItemIsEditable );
                AlbumInfo->setItem( j, i, item );
            }else{
                AlbumInfo->item(j,i)->setFlags( AlbumInfo->item(j,i)->flags() ^ Qt::ItemIsEditable );
            }
        }
    }
    AlbumInfo->setSortingEnabled(enabled);

}

void SearchDialog::save(){


    QString log;
    int aind = SearchResults->currentRow();
    if(aind==-1){
        QMessageBox::critical(this, "Save",
                              "Please select a serach result first", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    Album album = Albums[SearchResults->currentItem()->data(Qt::UserRole).toString()];
    //save tags
    bool trackOk;bool yearOk; QHash<QString,QString> commentsList;
    for(int i=0;i<AlbumInfo->rowCount();i++){
        if( !AlbumInfo->item(i,FILENAME) ){
            continue;
        }
        if( AlbumInfo->item(i,FILENAME)->text().isEmpty() ){
            continue;
        }
        if(AlbumInfo->item(i,FILENAME)->checkState()!=Qt::Checked){
            continue;
        }
        QString fullfile = AlbumInfo->item(i,FILENAME)->data(Qt::UserRole).toString();
        TagLib::FileRef f( fullfile.toStdString().c_str() );
        if( f.isNull() || !f.tag() ){
            log.append("\nCould not read tag for "+fullfile);
            continue;
        }
        qDebug()<<"saving "<<fullfile;
        //title
        if(AlbumInfo->item(i,TITLE)){
            f.tag()->setTitle( AlbumInfo->item(i,TITLE)->text().toStdString().c_str() );
        }

        //track
        if(AlbumInfo->item(i,TRACK)){
            int tmp = AlbumInfo->item(i,TRACK)->text().toInt(&trackOk,10);
            if(trackOk){
                f.tag()->setTrack( tmp );
            }
        }
        //add extra info to comments if checked
        QString comments = "";
        if( AlbumInfo->item(i,COMMENT) && !ClearPreviousComentsCheckBox->isChecked() ){
            comments = AlbumInfo->item(i,COMMENT)->text();
        }
        QString tmp="Format: "+Format->text();
        if(FormatLabel->isVisible() && FormatLabel->isChecked() && !comments.contains(tmp) && !Format->text().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        tmp="Country: "+Country->text();
        if(CountryLabel->isVisible() && CountryLabel->isChecked() && !comments.contains(tmp) && !Country->text().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        tmp="Label: "+Label->text();
        if(LabelLabel->isVisible() && LabelLabel->isChecked() && !comments.contains(tmp) && !Label->text().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        tmp="Roles: "+Roles->toPlainText();
        if(RolesLabel->isVisible() && RolesLabel->isChecked() && !comments.contains(tmp) && !Roles->toPlainText().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        tmp="Notes: "+Notes->toPlainText();
        if(NotesLabel->isVisible() && NotesLabel->isChecked() && !comments.contains(tmp) && !Notes->toPlainText().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        commentsList.insert(fullfile,comments);
        //comments
        if(AlbumInfo->item(i,COMMENT)){
            f.tag()->setComment( comments.toStdString().c_str() );
        }
        //album
        if(AlbumLabel->isChecked()){
            f.tag()->setAlbum( AlbumField->text().toStdString().c_str() );
        }
        //artist
        if(ArtistLabel->isChecked()){
            f.tag()->setArtist( Artist->text().toStdString().c_str() );
        }
        //genre
        if(GenreLabel->isVisible() && GenreLabel->isChecked()){
            f.tag()->setGenre( Genre->text().toStdString().c_str() );
        }
        //year
        if(YearLabel->isChecked()){
            int tmp = Year->text().toInt(&yearOk,10);
            if(yearOk){                
                f.tag()->setYear( tmp );
            }            
        }
        bool saveOk = f.save();
        if(!saveOk){
            log.append("\nCould not save tag for "+fullfile);
        }
    }

    //at last, update saved data in table and labels
    //so far not done by reading tags again, to speed things up
    bool enabled = AlbumInfo->isSortingEnabled();
    AlbumInfo->setSortingEnabled(false);
    for(int i=0;i<AlbumInfo->rowCount();i++){
        if( !AlbumInfo->item(i,FILENAME) ){
            continue;
        }
        if( AlbumInfo->item(i,FILENAME)->text().isEmpty() ){
            continue;
        }
        if(AlbumInfo->item(i,FILENAME)->checkState()!=Qt::Checked){
            continue;
        }
        //title
        TableWidgetItem *newItem1 = new TableWidgetItem;
        QFont font = newItem1->font();font.setBold(true);
        newItem1->setFont(font);
        if(AlbumInfo->item(i,TITLE)){
            newItem1->setText( AlbumInfo->item(i,TITLE)->text() );
        }else{
            newItem1->setText( AlbumInfo->item(i,CURRENTTITLE)->text() );
        }
        //newItem1->setFont(font);
        newItem1->setFlags( newItem1->flags() ^ Qt::ItemIsEditable );
        AlbumInfo->setItem(i, CURRENTTITLE, newItem1);
	
        //track
        if(trackOk){
            TableWidgetItem *newItem2 = new TableWidgetItem;
            newItem2->setFont(font);
            if(AlbumInfo->item(i,TRACK)){
                newItem2->setText( AlbumInfo->item(i,TRACK)->text() );
            }else{
                newItem2->setText( AlbumInfo->item(i,CURRENTTRACK)->text() );
            }
            //newItem2->setFont(font);
            newItem2->setFlags( newItem2->flags() ^ Qt::ItemIsEditable );
            AlbumInfo->setItem(i, CURRENTTRACK, newItem2);
        }
        //comments
        TableWidgetItem *newItem3 = new TableWidgetItem;
        //newItem3->setFont(font);
        newItem3->setText( commentsList[AlbumInfo->item(i,FILENAME)->data(Qt::UserRole).toString()] );
        //newItem3->setFont(font);
        //newItem3->setFlags( newItem3->flags() ^ Qt::ItemIsEditable );
        AlbumInfo->setItem(i, COMMENT, newItem3);

    }
    if(ArtistLabel->isChecked()){
        ArtistLabel->setText("Artist (current: "+Artist->text()+" ): ");
    }
    if(AlbumLabel->isChecked()){
        AlbumLabel->setText("Album (current: "+AlbumField->text()+" ): ");
    }
    if(GenreLabel->isChecked()){
        GenreLabel->setText("Genre (current: "+Genre->text()+" ): ");
    }
    if(YearLabel->isChecked() && yearOk){
        YearLabel->setText("Year (current: "+Year->text()+" ): ");
    }
    //save covers at last, in case tags are used to name covers
    if( SaveCoverCheckBox->isChecked() && !SaveAllCoversCheckBox->isChecked() ){
        if( currentCoverInd>=0 && currentCoverInd<album.images().size() ){
            QList<QUrl> images = album.images();
            saveCover( images[currentCoverInd] );
        }
    }else if( SaveAllCoversCheckBox->isChecked() ){
	//save all covers
        QList<QUrl> images = album.images();
        for(int i=0;i<images.size();i++){
            saveCover( images[i] );
        }
    }
    //show log if any
    if(!log.isEmpty()){
        TextViewer t(this, &log);
        t.exec();
    }
    info->setText("Tags saved");
    AlbumInfo->setSortingEnabled(enabled);
}



void SearchDialog::databaseChanged( int ind ){

    //delete Database;

    if(DatabaseComboBox->currentText()=="Discogs"){
        Database = new Discogs( api_key );
        Roles->show();
        Notes->show();
        Genre->show();
        RolesLabel->show();
        NotesLabel->show();
        GenreLabel->show();
    }else{
        Database = new MusicBrainz();
        Roles->hide();
        Notes->hide();
        Genre->hide();
        RolesLabel->hide();
        NotesLabel->hide();
        GenreLabel->hide();
    }

    connect( Database, SIGNAL( resultsDownloaded( QHash<QString,Album> ) ), this, SLOT( getResults( QHash<QString,Album> ) ) );
    connect( Database, SIGNAL( albumDownloaded( Album ) ), this, SLOT( gotAlbum( Album ) ) );
    connect( Database, SIGNAL( albumsDownloaded( QHash<QString,Album> ) ), this, SLOT( gotAlbums( QHash<QString,Album> ) ) );

}

void SearchDialog::gotAlbum( Album a ){

    Albums.insert(a.key(),a);
    showAlbumAndCover( a );

}

void SearchDialog::checkRows(){

    QAction *action = qobject_cast<QAction*>(sender());
    bool checked = action->data().toBool();
    QList<QTableWidgetSelectionRange> ranges = AlbumInfo->selectedRanges();
    if(ranges.size()==0){
        return;
    }
    QTableWidgetSelectionRange range = ranges[0];
    for(int i=range.topRow(); i<=range.bottomRow(); ++i) {
        if(!AlbumInfo->item(i,0)){
            continue;
        }
        if(checked){
            AlbumInfo->item(i,0)->setCheckState(Qt::Checked);
        }else{
            AlbumInfo->item(i,0)->setCheckState(Qt::Unchecked);
        }
    }

}

void SearchDialog::updateSortingEnabled( bool enabled ){
    AlbumInfo->setSortingEnabled(enabled);
}


void SearchDialog::albumInfoContextMenu(const QPoint &p){

    /*
    QModelIndex index = AlbumInfo->indexAt(p);
    int col = index.column();
    //int row = index.row();
    QString str = "file";
    itemCol t = ORIGIN;
    if(col==(int)TRACK || col==(int)TITLE){
        str = "result";
        t = RESULT;
    }
    */
    QMenu *c = new QMenu(AlbumInfo);


    QAction* selectAction = new QAction("Check selected rows", AlbumInfo);
    selectAction->setData(true);
    connect(selectAction, SIGNAL(triggered()), this, SLOT(checkRows()));
    QAction* deselectAction = new QAction("Uncheck selected rows", AlbumInfo);
    deselectAction->setData(false);
    connect(deselectAction, SIGNAL(triggered()), this, SLOT(checkRows()));

    //move up/down actions for AlbumInfo
    QAction* moveResultUpAction = new QAction("Move result up", AlbumInfo);
    moveResultUpAction->setData((int)RESULT);
    connect(moveResultUpAction, SIGNAL(triggered()), this, SLOT(moveRowUp()));
    QAction* moveResultDownAction = new QAction("Move result down", AlbumInfo);
    moveResultDownAction->setData((int)RESULT);
    connect(moveResultDownAction, SIGNAL(triggered()), this, SLOT(moveRowDown()));
    QAction* moveResultToAction = new QAction("Move result to...", AlbumInfo);
    moveResultToAction->setData((int)RESULT);
    connect(moveResultToAction, SIGNAL(triggered()), this, SLOT(moveRowTo()));

    QAction* moveOriginUpAction = new QAction("Move file up", AlbumInfo);
    moveOriginUpAction->setData((int)ORIGIN);
    connect(moveOriginUpAction, SIGNAL(triggered()), this, SLOT(moveRowUp()));
    QAction* moveOriginDownAction = new QAction("Move file down", AlbumInfo);
    moveOriginDownAction->setData((int)ORIGIN);
    connect(moveOriginDownAction, SIGNAL(triggered()), this, SLOT(moveRowDown()));
    QAction* moveOriginToAction = new QAction("Move file to...", AlbumInfo);
    moveOriginToAction->setData((int)ORIGIN);
    connect(moveOriginToAction, SIGNAL(triggered()), this, SLOT(moveRowTo()));

    QAction* sortEnabledAction = new QAction(tr("Sorting enabled"), AlbumInfo);
    sortEnabledAction->setCheckable(true); sortEnabledAction->setChecked( AlbumInfo->isSortingEnabled() );
    connect(sortEnabledAction, SIGNAL(toggled(bool)), this, SLOT(updateSortingEnabled(bool)));

    QAction* resizeColumnAction = new QAction(tr("Auto resize columns"), AlbumInfo);
    resizeColumnAction->setCheckable(true);
    //resizeColumnAction->setShortcut(tr("Ctrl+C"));
    connect(resizeColumnAction, SIGNAL(toggled(bool)), this, SLOT(resizeColumns(bool)));    
    QAction* resizeRowAction = new QAction(tr("Auto resize rows"), AlbumInfo);
    resizeRowAction->setCheckable(true);
    //resizeRowAction->setShortcut(tr("Ctrl+R"));
    connect(resizeRowAction, SIGNAL(toggled(bool)), this, SLOT(resizeRows(bool)));
    QAction* deleteAction = new QAction(tr("Clear selection"), AlbumInfo);
    //deleteAction->setShortcut(tr("Ctrl+Del"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteCells()));

    QAction* insertBlankFileAction = new QAction("Insert empty file before", AlbumInfo);
    insertBlankFileAction->setData((int)ORIGIN);
    connect(insertBlankFileAction, SIGNAL(triggered()), this, SLOT(insertBlankItem()));
    QAction* insertBlankResultAction = new QAction("Insert empty result before", AlbumInfo);
    insertBlankResultAction->setData((int)RESULT);
    connect(insertBlankResultAction, SIGNAL(triggered()), this, SLOT(insertBlankItem()));

    QAction* removeRowAction = new QAction(tr("Remove entire row"), AlbumInfo);
    connect(removeRowAction, SIGNAL(triggered()), this, SLOT(removeRow()));
    QAction* copyAction = new QAction(tr("Copy selection"), AlbumInfo);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copyCells()));
    QAction* pasteAction = new QAction(tr("Paste"), AlbumInfo);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    QMenu* matchByMenu = new QMenu(tr("Match results by..."), AlbumInfo);
    QAction* matchByTrackAction = new QAction(tr("Match by track"), AlbumInfo); matchByTrackAction->setData( (int)MATCHBYTRACK );
    matchByTrackAction->setCheckable(true); connect(matchByTrackAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    QAction* matchByTitleAction = new QAction(tr("Match by title"), AlbumInfo); matchByTitleAction->setData( (int)MATCHBYTITLE );
    matchByTitleAction->setCheckable(true); connect(matchByTitleAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    QAction* dontMatchAction = new QAction(tr("Do not match"), AlbumInfo); dontMatchAction->setData( (int)DONTMATCH );
    dontMatchAction->setCheckable(true); connect(dontMatchAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    QAction* matchByTrackTitleAction = new QAction(tr("Match by track, then title, then title to filename"), AlbumInfo); matchByTrackTitleAction->setData( (int)MATCHBYTRACKTITLE );
    matchByTrackTitleAction->setCheckable(true); connect(matchByTrackTitleAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    QAction* matchByTitleTrackAction = new QAction(tr("Match by title, then track, then title to filename"), AlbumInfo); matchByTitleTrackAction->setData( (int)MATCHBYTITLETRACK );
    matchByTitleTrackAction->setCheckable(true); connect(matchByTitleTrackAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    QAction* matchByFileNameAction = new QAction(tr("Match title to filename"), AlbumInfo); matchByFileNameAction->setData( (int)MATCHBYFILENAME );
    matchByFileNameAction->setCheckable(true); connect(matchByFileNameAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    matchByMenu->addAction( dontMatchAction );
    matchByMenu->addAction( matchByTitleAction );
    matchByMenu->addAction( matchByTrackAction );
    matchByMenu->addAction( matchByTrackTitleAction );
    matchByMenu->addAction( matchByTitleTrackAction );
    matchByMenu->addAction( matchByFileNameAction );

    c->addMenu(matchByMenu);
    c->addSeparator();
    //if(col==(int)FILENAME){
    c->addAction(deselectAction);
    c->addAction(selectAction);
    c->addSeparator();
    //}
    c->addAction(moveResultUpAction);
    c->addAction(moveResultDownAction);
    c->addAction(moveResultToAction);
    c->addAction(moveOriginUpAction);
    c->addAction(moveOriginDownAction);
    c->addAction(moveOriginToAction);
    c->addSeparator();
    c->addAction(resizeColumnAction);
    c->addAction(resizeRowAction);
    c->addAction(sortEnabledAction);
    c->addSeparator();
    c->addAction(copyAction);
    c->addAction(pasteAction);
    c->addAction(deleteAction);
    c->addSeparator();
    c->addAction(insertBlankFileAction);
    c->addAction(insertBlankResultAction);
    c->addAction(removeRowAction);

    dontMatchAction->setChecked( dontMatchChecked );
    matchByTitleAction->setChecked( matchByTitleChecked );
    matchByTrackAction->setChecked( matchByTrackChecked );
    matchByTrackTitleAction->setChecked( matchByTrackTitleChecked );
    matchByTitleTrackAction->setChecked( matchByTitleTrackChecked );
    matchByFileNameAction->setChecked( matchByFileNameChecked );
    resizeRowAction->setChecked(autoResizeRows);
    resizeColumnAction->setChecked(autoResizeColumns);

    QPoint globalPos = AlbumInfo->mapToGlobal(p);
    //menu->exec( globalPos );
    c->exec(globalPos);
}

void SearchDialog::paste(){

    QList<QTableWidgetSelectionRange> ranges = AlbumInfo->selectedRanges();
    if(ranges.size()==0){
        return;
    }
    QTableWidgetSelectionRange range = ranges[0];
    if(range.leftColumn()<(int)TITLE || range.rightColumn()<(int)TITLE){
        QMessageBox::information(this, tr("Discogs dialog"), tr("Pasting in the three first columns is not allowed") );
        return;
    }
    QString str = QApplication::clipboard()->text();
    qDebug()<<"clipboard: ";
    qDebug()<<str;
    QStringList rows = str.split('\n');
    int numRows = rows.count();
    int numColumns = rows.first().count('\t') + 1;
    if( range.rowCount() * range.columnCount() != 1
        && (range.rowCount() != numRows
            || range.columnCount() != numColumns)) {
        QMessageBox::information(this, tr("Discogs dialog"),
                                 tr("The information cannot be pasted because the copy "
                                    "and paste areas aren't the same size."));
        return;
    }
    bool enabled = AlbumInfo->isSortingEnabled();
    AlbumInfo->setSortingEnabled(false);
    for(int i=0; i<numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for(int j=0; j<numColumns; ++j) {
            int row = range.topRow() +i;
            int column = range.leftColumn() +j;
            if(row < AlbumInfo->rowCount() && column < AlbumInfo->columnCount()){
                if(!AlbumInfo->item(row,column)){
                    TableWidgetItem *item = new TableWidgetItem;
                    AlbumInfo->setItem(row,column,item);
                }
                AlbumInfo->item(row,column)->setText(columns[j]);
            }
        }
    }
    AlbumInfo->setSortingEnabled(enabled);
}

void SearchDialog::copyCells(){

    QList<QTableWidgetSelectionRange> ranges = AlbumInfo->selectedRanges();
    if(ranges.size()==0){
        return;
    }
    QTableWidgetSelectionRange range = ranges[0];
    QString str;
    for(int i=0; i<range.rowCount(); ++i) {
        if(i>0){
            str += "\n";
        }
        for(int j=0; j< range.columnCount(); ++j) {
            if(j>0){
                str += "\t";
            }
            if(!AlbumInfo->item(i,j)){
                str += "";
            }else{
                str += AlbumInfo->item(range.topRow() +i, range.leftColumn()+j)->text();
                qDebug()<<"adding to clipbard: "<<AlbumInfo->item(range.topRow() +i, range.leftColumn()+j)->text();
            }
        }
    }
    QApplication::clipboard()->setText(str);
}

void SearchDialog::removeRow(){

    int ind = AlbumInfo->currentRow();
    if(ind==-1){
        return;
    }
    AlbumInfo->removeRow(ind);
    nFiles=nFiles-1;

    resize(size().height()-1,size().width());
    resize(size().height()+1,size().width());
}

void SearchDialog::insertBlankItem(){

    int ind = AlbumInfo->currentRow();
    if(ind==-1){
        return;
    }
    bool enabled = AlbumInfo->isSortingEnabled();
    AlbumInfo->setSortingEnabled(false);
    int n = AlbumInfo->rowCount();
    //QList<int> inds;inds<<0<<1<<2<<5;
    QAction *action = qobject_cast<QAction *>(sender());
    itemCol t = (itemCol)action->data().toInt();
    QList<int> inds;
    if(t==ORIGIN){
        inds = originCols;
        qDebug()<<"origin columns";
    }else{
        qDebug()<<"result columns";
        inds = resultCols;
    }

    for(int i=0;i<inds.size();i++){
        int col = inds[i];
        QList<TableWidgetItem*> tmp;
        for(int row=ind;row<n;row++){
            tmp << (TableWidgetItem*)AlbumInfo->takeItem( row, col );
        }
        int row = ind+1;
        for(int j=0;j<tmp.size();j++){
            if(row>=AlbumInfo->rowCount()){
                AlbumInfo->setRowCount(row+1);
                setNonEditable( row+1, row+1 );
            }
            AlbumInfo->setItem( row, col, tmp.at(j) );
            qDebug()<<i<<j;
            row++;
        }
        //blank item
        TableWidgetItem *newItem = new TableWidgetItem;
        newItem->setFlags( newItem->flags() ^ Qt::ItemIsEditable );
        AlbumInfo->setItem( ind, col, newItem );
    }
    AlbumInfo->setSortingEnabled(enabled);
    nFiles=nFiles+1;

    for(int i=0;i<AlbumInfo->rowCount();i++){
        AlbumInfo->item(i,CURRENTTITLE)->setFlags( AlbumInfo->item(i,CURRENTTITLE)->flags() ^ Qt::ItemIsEditable );
        AlbumInfo->item(i,CURRENTTITLE)->setFlags( AlbumInfo->item(i,CURRENTTITLE)->flags() ^ Qt::ItemIsEditable );
    }

}

void SearchDialog::deleteCells(){

    bool enabled = AlbumInfo->isSortingEnabled();
    AlbumInfo->setSortingEnabled(false);
    QModelIndexList indexes = AlbumInfo->selectionModel()->selectedIndexes();
    for(int i=0;i<indexes.size();i++){
        int row = indexes[i].row();
        int column = indexes[i].column();
        if( AlbumInfo->item(row,column) && !originCols.contains(column) ){
            delete AlbumInfo->takeItem(row,column);
            TableWidgetItem *newItem = new TableWidgetItem;
            newItem->setFlags( newItem->flags() ^ Qt::ItemIsEditable );
            AlbumInfo->setItem( row, column, newItem );
        }
    }
    AlbumInfo->setSortingEnabled(enabled);
    resize(size().height()+1,size().width());
}

void SearchDialog::unCheckMatchActions(){

    matchByTrackChecked = false;
    matchByTitleChecked = false;
    dontMatchChecked = false;
    matchByTitleTrackChecked = false;
    matchByTrackTitleChecked = false;
    matchByFileNameChecked = false;
}

void SearchDialog::matchBy(bool state){

    QAction *action = qobject_cast<QAction*>(sender());
    if(state==false){
        action->setChecked(true);
        return;
    }
    matchByFlag f = (matchByFlag)action->data().toInt();
    unCheckMatchActions();
    action->setChecked(true);
    if(f==MATCHBYTRACK){
        matchByTrackChecked = true;
    }else if(f==MATCHBYTITLE){
        matchByTitleChecked = true;
    }else if(f==DONTMATCH){
        dontMatchChecked = true;
    }else if(f==MATCHBYTITLETRACK){
        matchByTitleTrackChecked = true;
    }else if(f==MATCHBYTRACKTITLE){
        matchByTrackTitleChecked = true;
    }else if(f==MATCHBYFILENAME){
        matchByFileNameChecked = true;
    }

}

void SearchDialog::updateCell( int row, int col ){

    //only current track & title, and track and title
    if(col<1 || col>4 ){
        return;
    }
    int currentCol=col-2;
    if(col<3){
        currentCol=col;
        col = currentCol+2;
    }
    TableWidgetItem *item  = (TableWidgetItem*)AlbumInfo->item(row,currentCol);
    TableWidgetItem *item2 = (TableWidgetItem*)AlbumInfo->item(row,col);
    if( !item2 || !item || item2->text().isEmpty() ){
        return;
    }

    QColor q;
    if( item->text().toLower()==item2->text().toLower() ){
        q = QColor( Qt::white );
    }else{
        q = QColor( Qt::gray );
    }
    item->setData(Qt::BackgroundRole, q);

}

void SearchDialog::resizeHeader( QHeaderView *header, bool state ){
    QHeaderView::ResizeMode mode;
    if(state){
        mode = QHeaderView::ResizeToContents;
    }else{
        mode = QHeaderView::Interactive;
    }
    header->setResizeMode( mode );
}
void SearchDialog::resizeColumns(bool state){
    resizeHeader( AlbumInfo->horizontalHeader(), state );
    autoResizeColumns = state;
    //settings->setValue( "discogs/autoResizeColumns", state );
    //settings->sync();
}
void SearchDialog::resizeRows(bool state){
    resizeHeader( AlbumInfo->verticalHeader(), state );
    autoResizeRows = state;
    //settings->setValue( "discogs/autoResizeRows", state );
    //settings->sync();
}

