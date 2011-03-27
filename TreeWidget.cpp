#include "TreeWidget.h"

TreeWidget::TreeWidget(QWidget *parent) : QTreeWidget(parent){

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(treeWidgetContextMenu(const QPoint &)));
    showTagInfo_ = false;
    showFullFileName_ = false;
    QStringList headers;
    headers.insert(Global::ArtistColumn,"Artist");
    headers.insert(Global::AlbumColumn,"Album");
    headers.insert(Global::FileNameColumn,"File name");
    headers.insert(Global::TitleColumn,"Title");
    headers.insert(Global::TrackColumn,"Track");
    headers.insert(Global::YearColumn,"Year");
    headers.insert(Global::GenreColumn,"Genre");
    //headers.insert(Global::CommentColumn,"Comment");
    headers.insert(Global::LengthColumn,"Length (s)");
    headers.insert(Global::BitRateColumn,"Bitrate (kbps)");
    headers.insert(Global::SampleRateColumn,"Sample rate (Hz)");
    headers.insert(Global::ChannelsColumn,"Channels");
    //headers<<"File name"<<"Artist"<<"Album"<<"Title"<<"Track"<<"Year"<<"Genre"<<"Comment"<<"Length (s)"<<"BitRate (kbps)"<<"SampleRate (Hz)"<<"Channels";
    setHeaderLabels( headers );
    setColumnCount(headers.size());
    setSortingEnabled(true);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
}


void TreeWidget::addItem( QTreeWidgetItem *item ){
    TagItem *item_ = static_cast<TagItem*>(item);
    QString str = item_->fileInfo().fileName();
    if( showFullFileName_ ){
        str = item_->fileInfo().filePath();
    }
    item_->setText(Global::FileNameColumn,str);
    if( showTagInfo_ ){
        item_->readTags();        
    }
    QTreeWidget::addTopLevelItem(item_);    

}

void TreeWidget::setShowTagInfo( bool show ){
    showTagInfo_ = show;
}

bool TreeWidget::showTagInfo() const{
    return showTagInfo_;
}

void TreeWidget::updateItems( QList<TagItem*> items ){

    bool sort = isSortingEnabled();
    setSortingEnabled(false);
    for(int i=0;i<items.size();i++){
        TagItem *item_ = items[i];
        QString str = item_->fileInfo().absoluteFilePath();
        if(!showFullFileName_){
            str = item_->fileInfo().fileName();
        }
        item_->setText( 0, str );
        item_->setColumnData();
    }
    setSortingEnabled(sort);

}

void TreeWidget::updateFileNames( bool enable ){
    showFullFileName_ = enable;
    bool sort = isSortingEnabled();
    setSortingEnabled(false);
    for(int i=0;i<topLevelItemCount();i++){
        qDebug()<<"updating item "<<i;
        TagItem *item_ = (TagItem*)topLevelItem(i);
        QString str = item_->fileInfo().absoluteFilePath();
        if(!enable){
            str = item_->fileInfo().fileName();
        }
        item_->setText( 0, str );
    }
    setSortingEnabled(sort);
}

bool TreeWidget::showFullFileName() const{
    return showFullFileName_;
}
void TreeWidget::setShowFullFileName( bool show ){
    showFullFileName_ = show;
}

void TreeWidget::sortTreeWidget(const QString &string ){
/*
    int n = count();
    blockSignals(true);
    QVector<TagItem*> items(n,NULL);
    qDebug()<<count();


    for(int i=n-1;i>=0;i--){
        items[i] = (TagItem*)takeItem(i);
        bool isSelected = items[i]->isSelected();
        if(isSelected){
            items[i]->setData(LOGICAL,QVariant(true));
        }
    }


    if( string=="name" ){
        qStableSort( items.begin(), items.end(), Global::compareName );
    }else if( string=="artist"){
        qStableSort( items.begin(), items.end(), Global::compareArtistTag );
    }else if( string=="album"){
        qStableSort( items.begin(), items.end(), Global::compareAlbumTag );
    }else if( string=="title"){
        qStableSort( items.begin(), items.end(), Global::compareTitleTag );
    }else if( string=="track"){
        qStableSort( items.begin(), items.end(), Global::compareTrackTag );
    }else if( string=="year"){
        qStableSort( items.begin(), items.end(), Global::compareYearTag );
    }else if( string=="genre"){
        qStableSort( items.begin(), items.end(), Global::compareGenreTag );
    }else if( string=="comment"){
        qStableSort( items.begin(), items.end(), Global::compareCommentTag );
    }else{
        qStableSort( items.begin(), items.end(), Global::compareName );
    }


    clear();
    for(int i=0;i<items.size();i++){
        addItem( items[i] );
        if( items[i]->data(LOGICAL).toBool()==true ){
            item(i)->setSelected(true);
        }

    }

    blockSignals(false);
    //statusBar()->showMessage("Finished sorting tags by "+string, 8000);
    */
}

void TreeWidget::updateShowTagInfo( bool enable ){
    //må bytte til qtreeview om man skal ha flere kolonner


    showTagInfo_ = enable;
    if(!enable){
        return;
    }

    bool sort = isSortingEnabled();
    setSortingEnabled(false);
    QVector<TagItem*> items_; items_.resize(topLevelItemCount());
    int k=0;
    for(int i=topLevelItemCount()-1;i>=0;i--){
        TagItem *item_ = static_cast<TagItem*>(takeTopLevelItem(i));
        if(!item_->tagIsRead()){
            item_->readTags();
        }
        //items_.append(item_);
        items_[k] = item_;
        k++;
    }
    for(int i=0;i<items_.size();i++){
        addItem(items_[i]);
    }
    setSortingEnabled(sort);


}

void TreeWidget::treeWidgetContextMenu(const QPoint &p){
    QMenu *c = new QMenu(this);

    qDebug()<<"TreeWidgetContextMenu";

    /*
    QSignalMapper *signalMapper = new QSignalMapper(this);

    QAction* sortByNameAction = new QAction(tr("Sort files by name"), this);
    connect(sortByNameAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    QAction* sortByArtistAction = new QAction(tr("Sort files by artist"), this);
    connect(sortByArtistAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    QAction* sortByAlbumAction = new QAction(tr("Sort files by album"), this);
    connect(sortByAlbumAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    QAction* sortByTitleAction = new QAction(tr("Sort files by title"), this);
    connect(sortByTitleAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    QAction* sortByTrackAction = new QAction(tr("Sort files by track"), this);
    connect(sortByTrackAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    QAction* sortByYearAction = new QAction(tr("Sort files by year"), this);
    connect(sortByYearAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    QAction* sortByGenreAction = new QAction(tr("Sort files by genre"), this);
    connect(sortByGenreAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    QAction* sortByCommentAction = new QAction(tr("Sort files by comment"), this);
    connect(sortByCommentAction, SIGNAL(triggered()), signalMapper, SLOT(map()));

    signalMapper->setMapping(sortByNameAction, QString("name"));
    signalMapper->setMapping(sortByArtistAction, QString("artist"));
    signalMapper->setMapping(sortByAlbumAction, QString("album"));
    signalMapper->setMapping(sortByTitleAction, QString("title"));
    signalMapper->setMapping(sortByTrackAction, QString("track"));
    signalMapper->setMapping(sortByYearAction, QString("year"));
    signalMapper->setMapping(sortByGenreAction, QString("genre"));
    signalMapper->setMapping(sortByCommentAction, QString("comment"));

    connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(sortTreeWidget(const QString &)));

    c->addAction(sortByNameAction);
    c->addAction(sortByArtistAction);
    c->addAction(sortByAlbumAction);
    c->addAction(sortByTitleAction);
    c->addAction(sortByTrackAction);
    c->addAction(sortByYearAction);
    c->addAction(sortByGenreAction);
    c->addAction(sortByCommentAction);
    c->addSeparator();
*/

    //enable sorting
    QAction* enableSortAction = new QAction(tr("Enable sorting"), this);
    enableSortAction->setCheckable( true ); enableSortAction->setChecked( this->isSortingEnabled() );
    connect(enableSortAction, SIGNAL(toggled(bool)), this, SLOT(enableSorting(bool)));

    //show full file names
    QAction* showFullNamesAction = new QAction(tr("Show full file names"), this);
    showFullNamesAction->setCheckable( true ); showFullNamesAction->setChecked( showFullFileName_ );
    connect(showFullNamesAction, SIGNAL(toggled(bool)), this, SLOT(updateFileNames(bool)));

    //show tag info
    QAction* showTagInfoAction = new QAction(tr("Always show tag info"), this);
    showTagInfoAction->setCheckable( true ); showTagInfoAction->setChecked( showTagInfo_ );
    showTagInfoAction->setToolTip("Warning, all tags for the files in workspace must then be read, which will be much slower than just showing teh filename.");
    connect(showTagInfoAction, SIGNAL(toggled(bool)), this, SLOT(updateShowTagInfo(bool)));

    //show full file names
    QAction* resizeColumnsAction = new QAction(tr("Resize columns to contents"), this);
    connect(resizeColumnsAction, SIGNAL(triggered()), this, SLOT(resizeColumns()));

    c->addAction(showFullNamesAction);
    c->addAction(showTagInfoAction);
    c->addAction(enableSortAction);
    c->addAction(resizeColumnsAction);

    QPoint globalPos = mapToGlobal(p);
    c->exec(globalPos);
}

void TreeWidget::resizeColumns(){
    for(int i=0;i<this->columnCount();i++){
        resizeColumnToContents( i );
    }
}



void TreeWidget::enableSorting(bool enable){
    setSortingEnabled(enable);
}
