#include "SearchDialog.h"


SearchDialog::SearchDialog( const QList<TagItem*> &items, QWidget *parent ) : QDialog(parent){

    setupUi(this); // this sets up GUI
    items_ = items;
    for(int i=0;i<items_.size();i++){
        if(!items_[i]->tagIsRead()){
            items_[i]->readTags();
        }
    }

    if(items_.size()==0){
        QMessageBox::critical(this,"","Bug, no items specified");
        return;
    }

    albumInfo = new TableWidget(this);
    QLayout *l = albumInfoFrame->layout();
    if(!l){
        l = new QGridLayout;
        albumInfoFrame->setLayout(l);
    }
    l->addWidget(albumInfo);


    setItems();

    settings = new QSettings("TagEditor.ini",QSettings::IniFormat,this);

    api_key_ = settings->value("SearchDialog/api_key","").toString();
    currentCoverInd=0;  


    // signals/slots mechanism in action		
    connect( prevCoverButton, SIGNAL( clicked() ), this, SLOT( showPrevCover() ) );
    connect( nextCoverButton, SIGNAL( clicked() ), this, SLOT( showNextCover() ) );
    connect( searchButton, SIGNAL( clicked() ), this, SLOT( search() ) );
    connect( saveButton, SIGNAL( clicked() ), this, SLOT( save() ) );
    connect(&downloadImageManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(setCoverFromReply(QNetworkReply*)));
    connect(&saveCoverManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(saveCoverFromReply(QNetworkReply*)));
    connect( searchResults, SIGNAL( itemClicked( QListWidgetItem * )  ), this, SLOT( showResult(QListWidgetItem *) ) );
    connect( databaseComboBox, SIGNAL( currentIndexChanged( int )  ), this, SLOT( databaseChanged(int) ) );
    databaseComboBox->addItem("Discogs");
    databaseComboBox->addItem("MusicBrainz");

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

void SearchDialog::setResultItem( const QString &text, const QString &key ){

    //bool enabled = isSortingEnabled();
    //setSortingEnabled(false);
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(text);
    newItem->setData(Qt::UserRole,key);
    searchResults->addItem( newItem );
    //setSortingEnabled(enabled);
}

void SearchDialog::setItems(){

    //use tag data from first item to create search string
    searchStr->setText( items_[0]->artist() );
    searchTitle->setText( items_[0]->album());
    artistLineEdit->setText( items_[0]->artist() );
    albumLineEdit->setText(items_[0]->album());
    yearLineEdit->setText(QString::number(items_[0]->year()));
    genreLineEdit->setText(items_[0]->genre());
    artistLabel->setText("Artist (current: "+items_[0]->artist()+" ): ");
    albumLabel->setText("Album (current: "+items_[0]->album()+" ): ");
    genreLabel->setText("Genre (current: "+items_[0]->genre()+" ): ");
    yearLabel->setText("Year (current: "+QString::number(items_[0]->year())+" ): ");

    albumInfo->setItems(items_);
}


void SearchDialog::readSettings(){

    this->resize(settings->value("SearchDialog/size", QSize(700, 600)).toSize());
    this->move( settings->value("SearchDialog/pos", QPoint(200, 200)).toPoint() );
    coverFormat->setText( settings->value("SearchDialog/coverFormat","%artist% - %album%").toString() );
    coverFormatCheckBox->setChecked( settings->value("SearchDialog/coverFormatCheckBox",true).toBool() );
    saveCoverCheckBox->setChecked( settings->value("SearchDialog/saveCover",false).toBool() );
    saveAllCoversCheckBox->setChecked( settings->value("SearchDialog/saveAllCovers",true).toBool() );
    artistLabel->setChecked( settings->value("SearchDialog/artistLabel",true).toBool() );
    albumLabel->setChecked( settings->value("SearchDialog/albumLabel",true).toBool() );
    genreLabel->setChecked( settings->value("SearchDialog/genreLabel",true).toBool() );
    yearLabel->setChecked( settings->value("SearchDialog/yearLabel",true).toBool() );
    rolesLabel->setChecked( settings->value("SearchDialog/rolesLabel",false).toBool() );
    formatLabel->setChecked( settings->value("SearchDialog/formatLabel",false).toBool() );
    countryLabel->setChecked( settings->value("SearchDialog/countryLabel",false).toBool() );
    labelLabel->setChecked( settings->value("SearchDialog/labelLabel",false).toBool() );
    notesLabel->setChecked( settings->value("SearchDialog/notesLabel",false).toBool() );
    clearPreviousComentsCheckBox->setChecked( settings->value("SearchDialog/clearPreviousComents",false).toBool() );
    downloadImmediately->setChecked( settings->value("SearchDialog/downloadImmediately",true).toBool() );

    databaseComboBox->setCurrentIndex( settings->value("SearchDialog/databaseComboBox",0).toInt() );
    databaseChanged( databaseComboBox->currentIndex() );
}


void SearchDialog::search(){
    //on search button press


    if( searchStr->text().isEmpty() && searchTitle->text().isEmpty() ){
        QMessageBox::critical(this, "Error",
                              "Empty search not allowed...", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QString url;
    //if( databaseComboBox->itemData(databaseComboBox->currentIndex()).toInt() == SearchDialog::DISCOGS ){
    if( database_->type()==MusicDatabase::DISCOGS ){
        url = "http://www.discogs.com/search?type=all&q="+searchStr->text()+"+"+searchTitle->text()+"&f=xml&api_key="+api_key_;
    }else{
        url = "http://musicbrainz.org/ws/1/release/?type=xml&artist="+searchStr->text()+"&title="+searchTitle->text();
    }
    database_->search( url, downloadImmediately->isChecked() );
    
}

void SearchDialog::getResults( const QHash<QString,Album> &results ){
    //called after database_->search is finished

    qDebug()<<"SearchDialog::getResults";
    results_ = results;
    searchResults->clear();
    albums_.clear();
    if( downloadImmediately->isChecked() ){
        database_->downloadAllAlbums( results_ ); //downloads and appends to albums_
        //gotAlbums is then called, albums are sorted and result items created
    }else{
        QList<Album> tmpresults = results_.values();
        std::sort( tmpresults.begin(), tmpresults.end(), compareAlbum );
        for(int i=0;i<tmpresults.size();i++){
            //if( databaseComboBox->itemData(databaseComboBox->currentIndex()).toInt() == SearchDialog::DISCOGS ){
            if( database_->type()==MusicDatabase::DISCOGS ){
                setResultItem( tmpresults[i].title(), tmpresults[i].key() );
            }else{ // if( databaseComboBox->itemData(databaseComboBox->currentIndex()).toInt() == SearchDialog::MUSICBRAINZ ){
                setResultItem( tmpresults[i].displayArtist()+" - "+tmpresults[i].title() +"("+tmpresults[i].format()+","+QString::number(tmpresults[i].year())+")", tmpresults[i].key() );
            }
        }
    }

    info->setText("Found "+QString::number(results_.size())+" results");


}



void SearchDialog::showResult( QListWidgetItem* item ){
    //called when row changed in listwidget searchResults

    qDebug()<<"SearchDialog::showResult";
    if(!item){
        return;
    }
    if( albums_.contains(item->data(Qt::UserRole).toString()) ){
        qDebug()<<"album is already downloaded";
        showAlbumAndCover( albums_[item->data(Qt::UserRole).toString()] );
    }else{
        qDebug()<<"downloading album";
        database_->downloadAlbum( results_[item->data(Qt::UserRole).toString()] );
    }
}


void SearchDialog::gotAlbums( const QHash<QString,Album> &albums ){

    albums_ = albums;
    QList<Album> tmpalbums = albums_.values();
    std::sort( tmpalbums.begin(), tmpalbums.end(), compareAlbum );
    searchResults->clear();
    for(int i=0;i<tmpalbums.size();i++){
        setResultItem( tmpalbums[i].displayArtist()+" - "+tmpalbums[i].title()+" ("+tmpalbums[i].format()+","+QString::number(tmpalbums[i].year())+")", tmpalbums[i].key() );
    }
    info->setText("Found "+QString::number(tmpalbums.size())+" results");

}

void SearchDialog::showAlbumAndCover( const Album &album ){

    if(album.images().size()==0){
        //if( databaseComboBox->itemData(databaseComboBox->currentIndex()).toInt()==SearchDialog::MUSICBRAINZ ){
        if( database_->type()==MusicDatabase::MUSICBRAINZ ){
            cover->setText("Covers are not available from MusicBrainz");
        }else{
            cover->setText("No cover found...");
        }
        coverLabel->setText("");
    }else{
        showCover( 0 );
    }
    showAlbumInfo( album );

}

void SearchDialog::showAlbumInfo( const Album &a ){

    albumInfo->showAlbumInfo(a);

    artistLineEdit->setText(a.artists().join(", "));
    albumLineEdit->setText(a.title());
    genreLineEdit->setText(a.genre());
    qDebug()<<"a.year()"<<a.year();
    yearLineEdit->setText(QString::number(a.year()));
    formatLineEdit->setText( a.format() );
    countryLineEdit->setText( a.country() );
    labelLineEdit->setText( a.label() );
    //if( databaseComboBox->itemData(databaseComboBox->currentIndex()).toInt() == SearchDialog::DISCOGS ){
    if( database_->type()==MusicDatabase::DISCOGS ){
        rolesTextEdit->setText( a.roles().join(". ") );
        notesTextEdit->setText( a.notes() );
    }

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

    int aind = searchResults->currentRow();
    if(aind==-1){
        return;
    }
    Album album = albums_[searchResults->currentItem()->data(Qt::UserRole).toString()];

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
    int ind = searchResults->currentRow();
    if(ind==-1){
        return;
    }
    Album album = albums_[searchResults->currentItem()->data(Qt::UserRole).toString()];
    if(album.images().size()==0){
        return;
    }
    QList<QUrl> images = album.images();
    saveCover( images[currentCoverInd] );
}


void SearchDialog::saveAllCoversNow(){
    int ind = searchResults->currentRow();
    if(ind==-1){
        return;
    }
    Album album = albums_[searchResults->currentItem()->data(Qt::UserRole).toString()];
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


void SearchDialog::setCover( const QUrl &url ){

    //qDebug()<<"showing cover"<<url;
    downloadImageManager.get(QNetworkRequest(url));

}

void SearchDialog::saveCover( const QUrl &url ){
    saveCoverManager.get(QNetworkRequest(url));
}

/*
void SearchDialog::downloadCover( const Album &album, const QUrl &url ){

    QNetworkRequest coverRequest(url);
    coverRequest.setAttribute(QNetworkRequest::User,album.key());
    QNetworkAccessManager *coverManager = new QNetworkAccessManager;
    connect(coverManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getCoverFromReply(QNetworkReply*)));
    coverManager->get(coverRequest);

}

void SearchDialog::getCoverFromReply(QNetworkReply* reply){

    QByteArray data = reply->readAll();
    QString key = reply->request().attribute(QNetworkRequest::User).toString();
    Album album = albums_[key];
    QImage p;
    p.loadFromData(data);
    p.convertToFormat(QImage::Format_RGB32);
    QList<QPixMap> covers = album.covers();
    if( !covers.contains(p) ){
        covers.append(p);
        album.setCovers(covers);
    }

    albums_[key] = album;

    reply->deleteLater();

}
*/

void SearchDialog::saveCoverFromReply(QNetworkReply* reply){
    //image to be saved must exist in albumLineEdit.images

    QByteArray data = reply->readAll();
    QImage p;
    p.loadFromData(data);
    p.convertToFormat(QImage::Format_RGB32);
    reply->deleteLater();


    int aind = searchResults->currentRow();
    if(aind==-1){
        return;
    }    
    Album album = albums_[searchResults->currentItem()->data(Qt::UserRole).toString()];

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
    if(coverFormatCheckBox->isChecked()){
        int suffInd = name.lastIndexOf(".");
        QString ext = name.right(name.size()-suffInd);
        name = createCoverName( cInd+1, ext );
    }
    //qDebug()<<name;
    QString path = items_[0]->fileInfo().absoluteDir().absolutePath();
    name = path+"/"+name;
    int quality = settings->value("SearchDialog/coverQuality",-1).toInt();
    bool ok = p.save( name, 0, quality );
    if(!ok){
        QMessageBox::critical(this, "Could not save cover",
                              "Could not save cover "+url+" as "+name, QMessageBox::Ok, QMessageBox::Ok);
    }else{
        qDebug()<<"saved"<<name;
    }
}

QString SearchDialog::createCoverName( int ind, const QString &ext ){

    QString name = coverFormat->text();
    name.replace( "%artist%", artistLineEdit->text() );
    name.replace( "%album%", albumLineEdit->text() );
    name.replace( "%year%", yearLineEdit->text() );
    name.replace( "%Genre%", genreLineEdit->text() );
    name = name + QString::number(ind) + ext;
    return name;
}




void SearchDialog::closeEvent( QCloseEvent *event ){

    settings->beginGroup("SearchDialog");
    settings->setValue( "saveCover", saveCoverCheckBox->isChecked() );
    settings->setValue( "saveAllCovers", saveAllCoversCheckBox->isChecked() );
    settings->setValue( "pos", this->pos() );
    settings->setValue( "size", this->size() );
    settings->setValue( "coverFormat", coverFormat->text() );
    settings->setValue( "coverFormatCheckBox", coverFormatCheckBox->isChecked() );
    settings->setValue( "artistLabel", artistLabel->isChecked() );
    settings->setValue( "albumLabel", albumLabel->isChecked() );
    settings->setValue( "genreLabel", genreLabel->isChecked() );
    settings->setValue( "yearLabel", yearLabel->isChecked() );
    settings->setValue( "rolesLabel", rolesLabel->isChecked() );
    settings->setValue( "formatLabel", formatLabel->isChecked() );
    settings->setValue( "countryLabel", countryLabel->isChecked() );
    settings->setValue( "labelLabel", labelLabel->isChecked() );
    settings->setValue( "notesLabel", notesLabel->isChecked() );
    settings->setValue( "downloadImmediately", downloadImmediately->isChecked() );
    settings->setValue( "clearPreviousComents", clearPreviousComentsCheckBox->isChecked() );
    settings->setValue( "databaseComboBox", databaseComboBox->currentIndex() );
    settings->setValue( "sortColumn", albumInfo->horizontalHeader()->sortIndicatorSection() );
    settings->setValue( "sortOrder", albumInfo->horizontalHeader()->sortIndicatorOrder() );
    settings->setValue( "sortingEnabled", albumInfo->isSortingEnabled() );
    settings->endGroup();
    settings->sync();

}



void SearchDialog::save(){


    QString log;
    int aind = searchResults->currentRow();
    if(aind==-1){
        QMessageBox::critical(this, "Save", "Please select a serach result first", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    Album album = albums_[searchResults->currentItem()->data(Qt::UserRole).toString()];
    //save tags
    bool trackOk;bool yearOk; QHash<QString,QString> commentsList;
    for(int i=0;i<albumInfo->rowCount();i++){
        qDebug()<<i;
        if( !albumInfo->item(i,TableWidget::FILE_NAME) ){
            continue;
        }
        if( albumInfo->item(i,TableWidget::FILE_NAME)->text().isEmpty() ){
            continue;
        }
        if(albumInfo->item(i,TableWidget::FILE_NAME)->checkState()!=Qt::Checked){
            continue;
        }
        QString fullfile = albumInfo->item(i,TableWidget::FILE_NAME)->data(Qt::UserRole).toString();
        TagItem* tagItem = 0;
        for(int j=0;j<items_.size();j++){
            qDebug()<<items_[j]->fileInfo().filePath().toLower()<<fullfile;
            if(items_[j]->fileInfo().filePath().toLower()==fullfile.toLower()){
                tagItem = items_[j];
                break;
            }
        }
        if(tagItem==0){
            qDebug()<<"BUG!!!!! COULD NOT FIND "+fullfile;
            continue;
        }

        if( !tagItem->tagOk() ){
            log.append("\nCould not read tag for "+fullfile);
            continue;
        }

        qDebug()<<"saving "<<fullfile;
        //title
        if(albumInfo->item(i,TableWidget::TITLE)){
            tagItem->setTitle( albumInfo->item(i,TableWidget::TITLE)->text() );
        }

        //track
        if(albumInfo->item(i,TableWidget::TRACK)){
            int tmp = albumInfo->item(i,TableWidget::TRACK)->text().toInt(&trackOk,10);
            if(trackOk){
                tagItem->setTrack( tmp );
            }
        }
        //add extra info to comments if checked
        QString comments = "";
        if( albumInfo->item(i,TableWidget::COMMENT) && !clearPreviousComentsCheckBox->isChecked() ){
            comments = albumInfo->item(i,TableWidget::COMMENT)->text();
        }
        QString tmp="Format: "+formatLineEdit->text();
        if(formatLabel->isVisible() && formatLabel->isChecked() && !comments.contains(tmp) && !formatLineEdit->text().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        tmp="Country: "+countryLineEdit->text();
        if(countryLabel->isVisible() && countryLabel->isChecked() && !comments.contains(tmp) && !countryLineEdit->text().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        tmp="Label: "+labelLineEdit->text();
        if(labelLabel->isVisible() && labelLabel->isChecked() && !comments.contains(tmp) && !labelLineEdit->text().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        tmp="Roles: "+rolesTextEdit->toPlainText();
        if(rolesLabel->isVisible() && rolesLabel->isChecked() && !comments.contains(tmp) && !rolesTextEdit->toPlainText().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        tmp="Notes: "+notesTextEdit->toPlainText();
        if(notesLabel->isVisible() && notesLabel->isChecked() && !comments.contains(tmp) && !notesTextEdit->toPlainText().isEmpty() ){
            if(!comments.isEmpty()){ comments.append("\n"); }
            comments.append(tmp);
        }
        commentsList.insert(fullfile,comments);
        //comments
        if(albumInfo->item(i,TableWidget::COMMENT)){
            tagItem->setComment( comments );
        }
        //album
        if(albumLabel->isChecked()){
            tagItem->setAlbum( albumLineEdit->text() );
        }
        //artist
        if(artistLabel->isChecked()){
            tagItem->setArtist( artistLineEdit->text() );
        }
        //genre
        if(genreLabel->isVisible() && genreLabel->isChecked()){
            tagItem->setGenre( genreLineEdit->text() );
        }
        //year
        if(yearLabel->isChecked()){
            int tmp = yearLineEdit->text().toInt(&yearOk,10);
            if(yearOk){                
                tagItem->setYear( tmp );
            }            
        }
        bool saveOk = tagItem->saveTag();
        if(!saveOk){
            log.append("\nCould not save tag for "+fullfile);
        }
    }

    //at last, update saved data in table and labels
    setItems();

    //save covers at last, in case tags are used to name covers
    if( saveCoverCheckBox->isChecked() && !saveAllCoversCheckBox->isChecked() ){
        if( currentCoverInd>=0 && currentCoverInd<album.images().size() ){
            QList<QUrl> images = album.images();
            saveCover( images[currentCoverInd] );
        }
    }else if( saveAllCoversCheckBox->isChecked() ){
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
    //albumInfo->setSortingEnabled(enabled);
}



void SearchDialog::databaseChanged( int ind ){

    delete database_;

    if(databaseComboBox->currentText()=="Discogs"){
        database_ = new Discogs( api_key_ );
        rolesTextEdit->show();
        notesTextEdit->show();
        genreLineEdit->show();
        rolesLabel->show();
        notesLabel->show();
        genreLabel->show();
    }else if(databaseComboBox->currentText()=="MusicBrainz" ){
        database_ = new MusicBrainz();
        rolesTextEdit->hide();
        notesTextEdit->hide();
        genreLineEdit->hide();
        rolesLabel->hide();
        notesLabel->hide();
        genreLabel->hide();
    }else{
        QMessageBox::critical(this,"","Unknown database, possible bug: "+databaseComboBox->currentText());
        return;
    }

    connect( database_, SIGNAL( resultsDownloaded( QHash<QString,Album> ) ), this, SLOT( getResults( QHash<QString,Album> ) ) );
    connect( database_, SIGNAL( albumDownloaded( Album ) ), this, SLOT( gotAlbum( Album ) ) );
    connect( database_, SIGNAL( albumsDownloaded( QHash<QString,Album> ) ), this, SLOT( gotAlbums( QHash<QString,Album> ) ) );

}

void SearchDialog::gotAlbum( const Album &a ){

    albums_.insert(a.key(),a);
    showAlbumAndCover( a );

}






