

#include "TagEditor.h"

TagEditor::TagEditor(QWidget *parent) : QMainWindow(parent){
    setupUi(this); // this sets up GUI


    TreeWidget_ = new TreeWidget(this);
    ListWidgetFrame->layout()->addWidget(TreeWidget_);

    createActions();

    QFileSystemModel *model = new QFileSystemModel;
    TreeWidget_->setModel(model);

    guiSettings = new QSettings(Global::settingsFile,QSettings::IniFormat,this);
    readSettings();
    setSettings();

    setGUIStyle( style );

}


void TagEditor::openStyleSheet(){

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open style sheet file"),
                                                    lastStyleSheetFolder, tr("*.qss"));
    if( fileName.isEmpty() ){
        checkStyleAction( "Custom...", false );
        return;
    }

    QFileInfo f(fileName);
    guiSettings->setValue("lastStyleSheetFolder",f.absolutePath());
    guiSettings->sync();
    bool ok = loadStyleSheet( fileName );
    if(!ok){
        QMessageBox::critical(this, "",
                              "Could not open "+fileName,
                              QMessageBox::Ok, QMessageBox::Ok);
        checkStyleAction( "Custom...", false );
        return;
    }
    uncheckStyleActions();
    checkStyleAction( "Custom...", true );
    style = fileName;
    //lastStyleSheetFolder = f.absolutePath();

}

bool TagEditor::loadStyleSheet( const QString &file ){

    QFile data(file);
    QString s;

    if(!data.open(QFile::ReadOnly)){
        return false;
    }

    s = QLatin1String(data.readAll());
    data.close();
    qApp->setStyleSheet(s);

    return true;
}

void TagEditor::uncheckStyleActions(){

    QList<QAction *> actions = menuStyle->actions();
    for(int i=0;i<actions.size();i++){
        actions[i]->setChecked(false);
    }
}

void TagEditor::checkStyleAction( const QString &actionText, bool state ){

    QList<QAction *> actions = menuStyle->actions();
    for(int i=0;i<actions.size();i++){
        if( actions[i]->text()==actionText ){
            actions[i]->setChecked(state);
        }
    }
}

void TagEditor::setGUIStyle( const QString &s ){

    qDebug()<<"style: "<<s;
    QStringList styles = QStyleFactory::keys();
    qDebug()<<styles;qDebug()<<"contains: "<<styles.contains( s );
    if( styles.contains( s ) ){
        //build in style
        QList<QAction *> actions = menuStyle->actions();
        qDebug()<<actions.size();
        for(int i=0;i<actions.size();i++){
            qDebug()<<actions[i]->text();
            if( actions[i]->text()==s ){
                QApplication::setStyle( QStyleFactory::create ( s ) );
                uncheckStyleActions(); style = s;
                actions[i]->setChecked(true);
                return;
            }
        }
    }

    //if function has not returned yet, assume qss file
    qDebug()<<"Custom style!";
    bool ok = loadStyleSheet( s );
    if(!ok){
        QMessageBox::critical(this, "",
                              "Could not open style "+s,
                              QMessageBox::Ok, QMessageBox::Ok);
        //uncheck currently clicked action
        checkStyleAction( "Custom...", false );
    }else{
        uncheckStyleActions();
        checkStyleAction( "Custom...", true );
        style = s;
    }

}


void TagEditor::renameFiles(){

    QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows();
    if(indexes.size()==0){
        QMessageBox msgBox;
        msgBox.setText("No files selected...");
        msgBox.exec();
        return;
    }

    QList<TagItem*> items; // = new QList<TagItem*>;
    for(int i=0;i<indexes.size();i++){
        TagItem *item = TreeWidget_->tagItem(indexes[i].row());
        items.append(item);
    }


    RenameDialog r(items,this);
    if(r.exec()==QDialog::Accepted){
        TreeWidget_->updateItems(items);
        statusBar()->showMessage("Finished renaming files", 8000);
    }
}


void TagEditor::removeFrames(){
    QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows();

    if(indexes.size()==0){
        QMessageBox msgBox;
        msgBox.setText("No files selected...");
        msgBox.exec();
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Removing frames/items");
    msgBox.setInformativeText("Are you sure you want to remove all frames/items from ID3v2/APE tags for the selected files?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if(ret==QMessageBox::Cancel){
        return;
    }

    QProgressDialog p("Removing frames...", "Cancel", 0, indexes.size(), this);
    p.setWindowModality(Qt::WindowModal);
    QString log;
    for(int i=0;i<indexes.size();i++){
        QString fullfile = ((TagItem*)TreeWidget_->topLevelItem( indexes[i].row() ))->fileInfo().absoluteFilePath();
        TagLib::MPEG::File f( fullfile.toStdString().c_str() );

        if(f.ID3v2Tag()){
            TagLib::ID3v2::FrameList::ConstIterator it = f.ID3v2Tag()->frameList().begin();
            qDebug()<<1.2;
            int k=0;
            while ( f.ID3v2Tag()->frameList().size()>0 ) {
                TagLib::ID3v2::Frame *frame = *(f.ID3v2Tag()->frameList().begin());
                qDebug()<<"removing frame: "<<frame->toString().toCString();
                f.ID3v2Tag()->removeFrame(frame, true );
            }

            if(k>0){
                log.append("Removed "+QString::number(k)+" frames from ID3v2 tag for "+fullfile);
            }
        }
        qDebug()<<2;
        TagLib::APE::Tag *ape = f.APETag();
        if(ape){
            int k=0;
            for(TagLib::APE::ItemListMap::ConstIterator it = ape->itemListMap().begin(); it != ape->itemListMap().end(); ++it){
                ape->removeItem( (*it).second.key() );
            }
            if(k>0){
                log.append("Removed "+QString::number(k)+" frames from APE tag for "+fullfile);
            }
        }
        qDebug()<<3;
        p.setValue(i);
        p.setLabelText("Removing frames/items from ID3v2/APE tags...\nFile "+QString::number(i)+" of "+QString::number(indexes.size()));
        if( p.wasCanceled() ){
            log.append("\n\nCanceled by user at "+fullfile);
            break;
        }
        if(!ape && !f.ID3v2Tag()){
            continue;
        }
        bool ok = f.save();
        qDebug()<<"saved file"<<ok;
        if(!ok){
            log.append("\nCould not save tag for "+fullfile);
        }
    }
    p.setValue(indexes.size());
    statusBar()->showMessage("Finished removing frames/items", 8000);
    if(!log.isEmpty()){
        TextViewer t(this, &log);
        t.resize(textViewerSize);
        t.exec();
        textViewerSize = t.size();
    }
}

void TagEditor::rewriteTag(){

    QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows();

    if(indexes.size()==0){
        QMessageBox msgBox;
        msgBox.setText("No files selected...");
        msgBox.exec();
        return;
    }

    QProgressDialog p("Rewriting tags...", "Cancel", 0, indexes.size(), this);
    p.setWindowModality(Qt::WindowModal);
    QString log; QList<TagItem*> items;
    for(int i=0;i<indexes.size();i++){
        TagItem* item = TreeWidget_->tagItem(indexes[i].row());
        QString fullfile = item->fileInfo().absoluteFilePath();
        item->readTags();
        if( !item->tagOk() ){
            log.append("\nCould not read tag for "+fullfile);
            continue;
        }
        bool ok = item->saveTag();
        if(!ok){
            log.append("\nCould not save tag for "+fullfile);
        }
        p.setValue(i);
        p.setLabelText("Rewriting tags...\nFile "+QString::number(i)+" of "+QString::number(indexes.size()));
        if( p.wasCanceled() ){
            log.append("\n\nCanceled by user at "+fullfile);
            break;
        }
        items.append(item);
    }
    TreeWidget_->updateItems(items);
    p.setValue(indexes.size());
    statusBar()->showMessage("Finished rewriting tags", 8000);
    if(!log.isEmpty()){
        TextViewer t(this, &log);
        t.resize(textViewerSize);
        t.exec();
        textViewerSize = t.size();
    }
}

void TagEditor::saveSettings(){

    //guiSettings->clear();
    guiSettings->beginGroup("MainWindow");
    //guiSettings->setValue("size", this->size());
    //guiSettings->setValue("pos", this->pos());
    guiSettings->endGroup();
    guiSettings->setValue("style", style );
    guiSettings->setValue("startupFolder", startupFolder );	
    //guiSettings->setValue("api_key", api_key );
    guiSettings->setValue("showSaveTagWarning", showSaveTagWarning );
    guiSettings->setValue("subfolders", subfolders );
    guiSettings->setValue("artistChecked", ArtistCheckbox->isChecked() );
    guiSettings->setValue("titleChecked", TitleCheckbox->isChecked() );
    guiSettings->setValue("albumChecked", AlbumCheckbox->isChecked() );
    guiSettings->setValue("yearChecked", YearCheckbox->isChecked() );
    guiSettings->setValue("trackChecked", TrackCheckbox->isChecked() );
    guiSettings->setValue("genreChecked", GenreCheckbox->isChecked() );
    guiSettings->setValue("commentChecked", CommentCheckbox->isChecked() );
    guiSettings->setValue("textViewerSize", textViewerSize );
    guiSettings->setValue("lastStyleSheetFolder", lastStyleSheetFolder );

    guiSettings->setValue("showFullFileName", TreeWidget_->showFullFileName() );
    guiSettings->setValue("sortingEnabled", TreeWidget_->isSortingEnabled() );
    guiSettings->setValue("showTagInfo", TreeWidget_->showTagInfo() );
    guiSettings->setValue("sortColumn", TreeWidget_->sortColumn() );
    guiSettings->setValue("sortOrder", TreeWidget_->header()->sortIndicatorOrder() );
    guiSettings->setValue("windowState", saveState() );    
    guiSettings->setValue("geometry", saveGeometry());
    QList<Global::TagField> cols = TreeWidget_->columns();
    QVariantList colslist;
    for(int i=0;i<cols.size();i++){
        colslist.append(cols[i]);
    }    
    guiSettings->setValue("columns", colslist );    

    guiSettings->sync();
}

void TagEditor::readSettings(){


    //guiSettings->beginGroup("MainWindow");
    //this->resize(guiSettings->value("size", QSize(630, 750)).toSize());
    //this->move(guiSettings->value("pos", QPoint(200, 200)).toPoint());
    //guiSettings->endGroup();

    style = guiSettings->value("style","WindowsXP").toString();
    startupFolder = guiSettings->value("startupFolder","").toString();
    QString defaultExt = "*.mp3;*.wma;*.wav;*.ogg;*.aac;*.ac3;*.m4a";
    extensions = guiSettings->value("extensions",defaultExt.split(";")).toStringList();

    QFileSystemModel *model = static_cast<QFileSystemModel*>(TreeView->model());
    model->setRootPath(startupFolder);
    model->setNameFilters(extensions);
    model->setNameFilterDisables(false);
    TreeView->setRootIndex(model->index(startupFolder));
    treeViewLabel->setText(startupFolder);

    //api_key = guiSettings->value("api_key","").toString();
    showSaveTagWarning = guiSettings->value("showSaveTagWarning",true).toBool();
    subfolders = guiSettings->value("subfolders",true).toBool();
    ArtistCheckbox->setChecked( guiSettings->value("artistChecked",true).toBool() );
    TitleCheckbox->setChecked( guiSettings->value("titleChecked",true).toBool() );
    AlbumCheckbox->setChecked( guiSettings->value("albumChecked",true).toBool() );
    YearCheckbox->setChecked( guiSettings->value("yearChecked",true).toBool() );
    TrackCheckbox->setChecked( guiSettings->value("trackChecked",true).toBool() );
    GenreCheckbox->setChecked( guiSettings->value("genreChecked",true).toBool() );
    CommentCheckbox->setChecked( guiSettings->value("commentChecked",true).toBool() );
    textViewerSize = guiSettings->value("textViewerSize", QSize(750, 350)).toSize();
    lastStyleSheetFolder = guiSettings->value("lastStyleSheetFolder","").toString();
    TreeWidget_->setShowFullFileName( guiSettings->value("showFullFileName",false).toBool() );
    TreeWidget_->setShowTagInfo( guiSettings->value("showTagInfo",false).toBool() );
    TreeWidget_->setSortingEnabled( guiSettings->value("sortingEnabled",true).toBool() );
    if(TreeWidget_->isSortingEnabled()){
        TreeWidget_->header()->setSortIndicator( guiSettings->value("sortColumn",0).toInt(), static_cast<Qt::SortOrder>(guiSettings->value("sortOrder",0).toInt()) );
        TreeWidget_->header()->setSortIndicatorShown(true);
    }    
    QVariantList colstmp = guiSettings->value("columns").toList();
    QList<Global::TagField> cols;
    for(int i=0;i<colstmp.size();i++){
        qDebug()<<"colstmp[i] "<<colstmp[i];
        cols.append(static_cast<Global::TagField>(colstmp[i].toInt()));
    }
    TreeWidget_->setColumnsList(cols);

    restoreState(guiSettings->value("windowState").toByteArray());
    restoreGeometry(guiSettings->value("geometry").toByteArray());

    //splitter->setSizes(guiSettings->value("splittersizes").value< QList<int> >() );

}



void TagEditor::setSettings(){


    artistCheckbox->setChecked( artistChecked );
    titleCheckbox->setChecked( titleChecked );
    albumCheckbox->setChecked( albumChecked );
    yearCheckbox->setChecked( yearChecked );
    trackCheckbox->setChecked( trackChecked );
    genreCheckbox->setChecked( genreChecked );
    commentCheckbox->setChecked( commentChecked );

    QFileSystemModel *model = (QFileSystemModel *)treeView->model();
    model->setRootPath(startupFolder);
    model->setNameFilters(extensions);
    model->setNameFilterDisables(false);
    treeView->setRootIndex(model->index(startupFolder));
    treeViewLabel->setText(startupFolder);

}


void TagEditor::chooseDir(){

    static QString dir;
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    if(!dir.isEmpty()){
        dialog.setDirectory( dir );
    }
    if( dialog.exec() ){
        QStringList d = dialog.selectedFiles();
        dir = d[0];
        QFileSystemModel *m =static_cast<QFileSystemModel*>(TreeView->model());
        TreeView->setRootIndex(m->index(dir));
        treeViewLabel->setText(dir);
        startupFolder = dir;
    }
}

void TagEditor::showSettings(){

    SettingsDialog s;
    if( s.exec()==QDialog::Accepted ){
        //obtain newly saved data from settingsdialog
        extensions = guiSettings->value("extensions","").toStringList();
        //api_key = guiSettings->value("api_key","").toString();
        subfolders = guiSettings->value("subfolders",true).toBool();
    }

}

void TagEditor::serialize(){

    QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows();
    qDebug()<<indexes.size();
    if(indexes.size()==0){
        QMessageBox msgBox;
        msgBox.setText("No files selected...");
        msgBox.exec();
        return;
    }

    bool ok;
    uint track = QInputDialog::getInt(this, "",tr("Starting index:"), 1, 0, 99999, 1, &ok);

    qDebug()<<ok;
    if(!ok){
        statusBar()->showMessage("Canceled by user", 8000);
        return;
    }

    QString log;
    TagLib::FileRef f;
    for(int i=0;i<indexes.size();i++){
        TagItem *item = TreeWidget_->tagItem(indexes[i].row()); // ((TagItem*)TreeWidget_->topLevelItem( indexes[i].row() ));
        QString fullfile = item->fileInfo().absoluteFilePath();
        /*
        f = TagLib::FileRef( fullfile.toStdString().c_str() );
        if( f.tag() ){
            f.tag()->setTrack( track );
            item->setTag( Global::Track, track );
            bool ok = f.save();
            if(!ok){
                log.append("\nCould not save tag for "+fullfile);
            }
        }else{
            log.append("\nCould not read tag for "+fullfile);
        }
        */
        item->setTrack(track);
        bool ok = item->saveTag();
        if(!ok){
            log.append("\nCould not save tag for "+fullfile);
        }
        track++;
    }

    statusBar()->showMessage("Finished serializing tracks", 8000);
    if(!log.isEmpty()){
        TextViewer t(this, &log);
        t.resize(textViewerSize);
        t.exec();
        textViewerSize = t.size();
    }
}

void TagEditor::saveTag(){

    //QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows();
    QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows(0);
    qDebug()<<indexes.size();

    if( indexes.size()>1 && showSaveTagWarning ){
        QMessageBox msgBox; QCheckBox dontPrompt("Do not prompt again", &msgBox);
        //int nrows = (dynamic_cast<QGridLayout*>(msgBox.layout()))->rowCount();
  //int ncols = (dynamic_cast<QGridLayout*>(msgBox.layout()))->columnCount();
  //dynamic_cast<QGridLayout*>(msgBox.layout())->addWidget(&dontPrompt, nrows, 0, 1, ncols );

        dontPrompt.blockSignals(true);
        msgBox.addButton(&dontPrompt, QMessageBox::ActionRole);
        msgBox.setText("More than one file is selected");
        msgBox.setInformativeText("Save shown data for all files?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        showSaveTagWarning = !dontPrompt.isChecked();
        if(ret==QMessageBox::Cancel){
            statusBar()->showMessage("Aborted", 8000);
            return;
        }
    }
    //aply to all selected files
    QString log;
    int n = indexes.size();
    QProgressDialog *p=0;
    if(n>1){
        p = new QProgressDialog("Saving tags...", "Cancel", 0, indexes.size(), this);
        p->setWindowModality(Qt::WindowModal);
    }
    for(int i=0;i<n;i++){

        if(n>1){
            p->setValue(i);
            if( p->wasCanceled() ){
                break;
            }
        }
	
        int row = indexes[i].row();
        //TagItem *item = (TagItem*)TreeWidget_->topLevelItem( row );
        TagItem *item = TreeWidget_->tagItem(row);
        QString fullfile = item->fileInfo().absoluteFilePath();
        qDebug()<<fullfile;
        //TagLib::FileRef f( fullfile.toStdString().c_str() );
        if( !item->tagOk() ){
            log.append("\nCould not read tag for "+fullfile);
            continue;
        }

        if(ArtistCheckbox->isChecked()){
            //f.tag()->setArtist( Artist->text().toStdString().c_str() );
            //item->setTag( Global::Artist, Artist->text() );
            item->setArtist(Artist->text());
        }
        if(TitleCheckbox->isChecked()){
            //f.tag()->setTitle( Title->text().toStdString().c_str() );
            //item->setTag( Global::Title, Title->text() );
            item->setTitle(Title->text());
        }
        if(AlbumCheckbox->isChecked()){
            //f.tag()->setAlbum( Album->text().toStdString().c_str() );
            //item->setTag( Global::AlbumField, Album->text() );
            item->setAlbum(Album->text());
        }
        if(YearCheckbox->isChecked()){
            bool ok;
            uint year = Year->text().toUInt(&ok);
            qDebug()<<"converted to uint: "<<ok<<year;
            if(!ok){
                //statusBar()->showMessage(fullfile+": Year has to be an positive integer value!", 8000);
                QMessageBox::critical(this, "",
                                      fullfile+":\nYear has to be an positive integer value!",
                                      QMessageBox::Ok, QMessageBox::Ok);
                //Year->setText( item->getTag("year").toString() );
                Year->setText( QString::number(item->year()) );
            }else{
                //f.tag()->setYear( year );
                //item->setTag( Global::Year, year );
                item->setYear(year);
            }
        }
        if(TrackCheckbox->isChecked()){
            bool ok;
            uint track = Track->text().toUInt(&ok);
            if(!ok){
                //statusBar()->showMessage(fullfile+": Track has to be an positive integer value!", 8000);
                QMessageBox::critical(this, "",
                                      fullfile+":\nTrack has to be an positive integer value!",
                                      QMessageBox::Ok, QMessageBox::Ok);
                Track->setText( QString::number(item->track()) );
            }else{
                //f.tag()->setTrack( track );
                //item->setTag( Global::Track, track );
                item->setTrack(track);
            }
        }
        if(GenreCheckbox->isChecked()){
            //f.tag()->setGenre( Genre->text().toStdString().c_str() );
            //item->setTag( Global::Genre, Genre->text() );
            item->setGenre(Genre->text());
        }
        if(CommentCheckbox->isChecked()){
            //f.tag()->setComment( Comment->toPlainText().toStdString().c_str() );
            //item->setTag( Global::Comment, Comment->toPlainText() );
            item->setComment(Comment->toPlainText());
        }

        bool ok = item->saveTag();//f.save();
        if(!ok){
            log.append("\nCould not save tag for "+fullfile);
        }
    }
    if(n>1){
        p->setValue(n);
    }
    if(indexes.size()>1){
        statusBar()->showMessage("Fisnihed saving tags", 8000);
    }else{
        statusBar()->showMessage("Tag saved", 8000);
    }
    if(!log.isEmpty()){
        TextViewer t(this, &log);
        t.resize(textViewerSize);
        t.exec();
        textViewerSize = t.size();
    }
}

void TagEditor::clearTextFields(){
    Artist->setText( "" );
    Title->setText( "" );
    Album->setText( "" );
    Year->setText( "" );
    Track->setText( "" );
    Genre->setText( "" );
    Comment->setText( "" );
    //FileLabel->setText( "" );
}

void TagEditor::showTagInfo(){

    //QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows();
    QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows(0);
    //const QModelIndexList qml = TreeWidget_->selectedIndexes();

    if( indexes.size()==0 ){
        return;
    }
    qDebug()<<indexes[0].row()<<TreeWidget_->topLevelItemCount();
    if( indexes.size()==1 && indexes[0].row()>=TreeWidget_->topLevelItemCount() ){
        return;
    }

    if( TreeWidget_->currentItem()==0 ){
        return;
    }

    //qDebug()<<"indexes.size(): "<<indexes.size()<<" currentRow: "<<TreeWidget_->currentRow();
    for(int i=0;i<indexes.size();i++){
        qDebug()<<indexes[i].row();
    }

    if(indexes.size()>1){
        qDebug()<<"multiple rows selected!";
        FileLabel->setText("Pressing the save button when more than one file is selected will store the checked fields for all the selected files! ("+QString::number(indexes.size())+") ");
        return;
    }
    if(indexes.size()==0){
        qDebug()<<"no row selected!";
        FileLabel->setText("");
        clearTextFields();
        return;
    }
    //int row = indexes[0].row();


    //tag
    TagItem *item = static_cast<TagItem*>(TreeWidget_->currentItem());
    FileLabel->setText( item->fileInfo().absoluteFilePath() );

    if( !item->tagIsRead() ){
        item->readTags();
    }
    TreeWidget_->setColumnData(item);

    if( item->tagOk() ){
        Artist->setText( item->artist() );
        Title->setText( item->title() );
        Album->setText( item->album() );
        Year->setText( QString::number( item->year() ) );
        Track->setText( QString::number( item->track() ) );
        Genre->setText( item->genre() );
        Comment->setText( item->comment() );
    }else{
        clearTextFields();
    }
    //audioproperties
    if( item->audioPropertiesOk() ){
        FileLabel->setText( FileLabel->text() + "\n(Bitrate: "+QString::number(item->bitRate())+" kb/s, samplerate: "+
                           QString::number(item->sampleRate())+" Hz, length: "+QString::number(item->length())+
                           " s, chanels: "+QString::number(item->channels())+")" );
    }else{
        FileLabel->setText( FileLabel->text() + "\n(Could not read tag...)");
    }

}

void TagEditor::resizeColumn(){
    
    TreeView->resizeColumnToContents(0);

}

void TagEditor::removeAllFiles(){

    TreeWidget_->clear();
    FileLabel->setText( "" );

    clearTextFields();
}

void TagEditor::removeFiles(){

    QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows();
    if(indexes.size()==TreeWidget_->topLevelItemCount()){
        removeAllFiles();
        return;
    }
    for(int i=indexes.size()-1;i>=0;i--){
        delete TreeWidget_->takeTopLevelItem(indexes[i].row());
    }
    FileLabel->setText( "" );
    clearTextFields();
}

void TagEditor::searchAndAddFiles(){

    QModelIndexList indexes = TreeView->selectionModel()->selectedRows(0);
    QFileSystemModel *m = (QFileSystemModel *)TreeView->model();

    QList<QFileInfo> infos;
    for(int i=0;i<indexes.size();i++){

        QModelIndex index = indexes[i];
        bool isdir = m->isDir(index);

        if(isdir){
            QString folder = m->filePath(index);
            infos += getDirContent( folder );
        }else{
            infos.append( QFileInfo(m->filePath(index)) );
        }
    }

    SearchForTagsDialog r(infos,this);
    if( r.exec()==QDialog::Accepted){

        QList<QFileInfo> files = r.files();
        for(int j=0;j<files.size();j++){
            TagItem *newItem = new TagItem( files[j].filePath() );
            TreeWidget_->addItem( newItem );
        }
    }
}

void TagEditor::addFiles( const QList<QFileInfo> &infos ){

    int n = infos.size();

    QList<QTreeWidgetItem*> items; items.reserve(n);
    for(int j=0;j<n;j++){
        TagItem *newItem = new TagItem( infos[j].filePath() );
        //treeWidget->addItem( newItem );
        items.append(newItem);
    }

    treeWidget->addTopLevelItems(items);

}

void TagEditor::addFiles(){

    QModelIndexList indexes = TreeView->selectionModel()->selectedRows(0);
    QFileSystemModel *m = (QFileSystemModel *)TreeView->model();


    for(int i=0;i<indexes.size();i++){

        QModelIndex index = indexes[i];
        qDebug()<<index.row()<<index.column();

        bool isdir = m->isDir(index);

        if(isdir){
            QString folder = m->filePath(index);

            QList<QFileInfo> info = getDirContent( folder );
            for(int j=0;j<info.size();j++){                
                TagItem *newItem = new TagItem( info[j].filePath() );
                TreeWidget_->addItem( newItem );
            }

        }else{
            QFileInfo f(m->filePath(index));
            TagItem *newItem = new TagItem( f.filePath() );
            TreeWidget_->addItem( newItem );
        }
    }

}


QList<QFileInfo> TagEditor::getDirContent( const QString& aPath ){

    qDebug()<<aPath;
    //decide to include subfolder or not
    QDirIterator::IteratorFlag subdirFlag;
    if( subfolders ){
        subdirFlag = QDirIterator::Subdirectories;
    }else{
        subdirFlag = QDirIterator::NoIteratorFlags;
    }

    // set dir iterator
    QDirIterator dirIterator(aPath,
                             extensions,
                             QDir::Files|QDir::NoSymLinks,
                             subdirFlag);

    QList<QFileInfo> fileInfo;
    while( dirIterator.hasNext() ){
        dirIterator.next();
        fileInfo.append( dirIterator.fileInfo() );
    }

    return fileInfo;

}

void TagEditor::closeEvent( QCloseEvent *event ){

    saveSettings();
}

void TagEditor::replaceTags(){

    QModelIndexList indexes = TreeWidget_->selectionModel()->selectedRows();
    if(indexes.size()==0){
        QMessageBox msgBox;
        msgBox.setText("No files selected...");
        msgBox.exec();
        return;
    }
    //QList<QFileInfo> *fileInfos = new QList<QFileInfo>;
    QList<TagItem*> items;
    for(int i=0;i<indexes.size();i++){
        TagItem *item = ((TagItem*)TreeWidget_->topLevelItem( indexes[i].row() ));
        //fileInfos->append(item->fileInfo());
        items.append(item);
    }
    ReplaceDialog r(items,this);
    if(r.exec()==QDialog::Accepted){
        TreeWidget_->updateItems(items);
    }
    //delete fileInfos;

    /*
    for(int i=0;i<indexes.size();i++){
        TagItem *item = ((TagItem*)TreeWidget_->topLevelItem( indexes[i].row() ));
        item->clearTags();
    }
    */
}


void TagEditor::createActions(){

    QAction* searchOnlineAction = new QAction(tr("Search for selected file/album in online musicdatabases..."), this);
    searchOnlineAction->setShortcut(tr("Ctrl+S"));
    connect(searchOnlineAction, SIGNAL(triggered()), this, SLOT(searchOnline()));
    QAction* searchForFilesAction = new QAction(tr("Search for files to add to workspace..."), this);
    //searchForFilesAction->setShortcut(tr("Ctrl+S"));
    connect(searchForFilesAction, SIGNAL(triggered()), this, SLOT(searchAndAddFiles()));

    TreeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    TreeView->addAction(searchOnlineAction);
    TreeView->addAction(searchForFilesAction);

    connect( TreeView, SIGNAL( expanded( const QModelIndex & )  ), this, SLOT( resizeColumn() ) );
    connect( TreeView, SIGNAL( collapsed( const QModelIndex & )  ), this, SLOT( resizeColumn() ) );
    //connect( TreeWidget_, SIGNAL( currentRowChanged( int )  ), this, SLOT( showTagInfo(int) ) );
    connect( TreeWidget_, SIGNAL( itemSelectionChanged() ), this, SLOT( showTagInfo() ) );
    connect( AddButton, SIGNAL( clicked()  ), this, SLOT(addFiles() ) );
    connect( RemoveButton, SIGNAL( clicked()  ), this, SLOT(removeFiles() ) );
    connect( ClearButton, SIGNAL( clicked()  ), this, SLOT(removeAllFiles() ) );
    connect( SaveButton, SIGNAL( clicked()  ), this, SLOT(saveTag() ) );
    connect( ChooseDirButton, SIGNAL( clicked()  ), this, SLOT(chooseDir() ) );

    connect( actionSettings, SIGNAL( triggered() ), this, SLOT( showSettings() ) );
    connect( actionRewriteTag, SIGNAL( triggered() ), this, SLOT( rewriteTag() ) );
    connect( actionRenameFiles, SIGNAL( triggered() ), this, SLOT( renameFiles() ) );
    connect( actionReplaceTags, SIGNAL( triggered() ), this, SLOT( replaceTags() ) );
    connect( actionSerialize, SIGNAL( triggered() ), this, SLOT( serialize() ) );
    connect( actionClearTags, SIGNAL( triggered() ), this, SLOT( clearTags() ) );
    //connect( actionRemoveFrames, SIGNAL( triggered() ), this, SLOT( removeFrames() ) );
    //styles

    QSignalMapper *styleMapper = new QSignalMapper(this);
    QStringList styles = QStyleFactory::keys();
    for(int i=0;i<styles.size();i++){
        QAction *a = new QAction(styles[i],menuStyle);
        a->setCheckable(true);
        connect(a, SIGNAL(triggered()), styleMapper, SLOT(map()));
        styleMapper->setMapping(a, styles[i]);
        menuStyle->addAction(a);
    }
    menuStyle->addSeparator();
    QAction *actionCustomStyleSheet = new QAction("Custom...",menuStyle);
    actionCustomStyleSheet->setCheckable(true);
    connect(actionCustomStyleSheet, SIGNAL(triggered()), this, SLOT(openStyleSheet()));
    menuStyle->addAction( actionCustomStyleSheet );
    connect(styleMapper, SIGNAL(mapped(const QString &)), this, SLOT(setGUIStyle(const QString &)));

}



void TagEditor::clearTags(){

    TreeWidget_->clearTags();
    statusBar()->showMessage("Tags cleared", 8000);

}



void TagEditor::searchOnline(){

    QModelIndexList indexes = TreeView->selectionModel()->selectedRows(0);

    if(indexes.size()==0){
        QMessageBox msgBox;
        msgBox.setText("No rows selected");
        msgBox.exec();
        return;
    }

    QModelIndex index = indexes[0];
    QFileSystemModel *m = (QFileSystemModel *)TreeView->model();
    bool isdir = m->isDir(index);

    QString query;
    QList<QFileInfo> info;
    QList<TagItem*> items;
    if(isdir){
        //folder/album
        QString folder = m->filePath(index);
        info = getDirContent( folder );
    }else{
        for(int i=0;i<indexes.size();i++){
            info.append( m->fileInfo( indexes[i] ) );
        }
    }
    if(info.size()==0){
        QMessageBox msgBox;
        msgBox.setText("Empty folder...");
        msgBox.exec();
        return;
    }


    for(int i=0;i<info.size();i++){
        items.append(new TagItem(info[i].absoluteFilePath()));
    }

    SearchDialog d( items, this );
    //d.setModal(false);
    d.exec();


    //delete tag data from these files if they exist in TreeWidget_, since the tag data now might get changed
    for(int i=0;i<TreeWidget_->topLevelItemCount();i++){
        if( info.size()==0 ){
            break;
        }
        TagItem *item = (TagItem*)TreeWidget_->topLevelItem(i);
        for(int j=0;j<info.size();j++){
            if( item->data(0,TreeWidget::FULLFILE)==info.at(j).absoluteFilePath() ){
                item->clearTags();
                info.removeAt( j );
                break;
            }
        }
    }

}




