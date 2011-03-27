#include "ListWidget.h"

ListWidget::ListWidget(QWidget *parent) : QListWidget(parent){

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(listWidgetContextMenu(const QPoint &)));
    showTagInfo_ = false;
    showFullFileName_ = false;
}


void ListWidget::addItem( QListWidgetItem *item ){
    TagItem *item_ = (TagItem*)item;
    QString str = item_->fileInfo().fileName();
    if( showFullFileName_ ){
        str = item_->fileInfo().filePath();
    }
    item_->setText(str);
    QListWidget::addItem(item_);
}


void ListWidget::updateFileNames( bool enable ){
    showFullFileName_ = enable;
    for(int i=0;i<count();i++){
        TagItem *item_ = (TagItem*)item(i);
        QString str = item_->fileInfo().absoluteFilePath();
        if(!enable){
            str = item_->fileInfo().fileName();
        }
        item_->setText( str );
    }
}

bool ListWidget::showFullFileName() const{
    return showFullFileName_;
}
void ListWidget::setShowFullFileName( bool show ){
    showFullFileName_ = show;
}

void ListWidget::sortListWidget(const QString &string ){

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
}

void ListWidget::updateShowTagInfo( bool enable ){
    må bytte til qtreeview om man skal ha flere kolonner
    /*
    showTagInfo_ = enable;
    if(enable){
    for(int i=0;i<count();i++){
        TagItem *item_ = (TagItem*)item(i);
        QString album = item_->getTag(TagItem::Album).toString();
        QString title = item_->getTag(TagItem::Title).toString();
        QString artist = item_->getTag(TagItem::Artist).toString();
        QString genre = item_->getTag(TagItem::Genre).toString();
        QString comment = item_->getTag(TagItem::Comment).toString();
        QString year = QString::number(item_->getTag(TagItem::Year).toInt());
        QString track = QString::number(item_->getTag(TagItem::Track).toInt());

    }
}else{
    for(int i=0;i<count();i++){
        for(int i=0;i<count();i++){
        }
    }
}
*/

}

void ListWidget::listWidgetContextMenu(const QPoint &p){
    QMenu *c = new QMenu(this);

    qDebug()<<"listWidgetContextMenu";
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

    connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(sortListWidget(const QString &)));

    //show full file names
    QAction* showFullNamesAction = new QAction(tr("Show full file names"), this);
    showFullNamesAction->setCheckable( true ); showFullNamesAction->setChecked( showFullFileName_ );
    connect(showFullNamesAction, SIGNAL(toggled(bool)), this, SLOT(updateFileNames(bool)));

    //show tag info
    QAction* showTagInfoAction = new QAction(tr("Show full file names"), this);
    showTagInfoAction->setCheckable( true ); showTagInfoAction->setChecked( showTagInfo_ );
    showTagInfoAction->setToolTip("Warning, all tags for the files in workspace must then be read, which will be much slower than just showing teh filename.");
    connect(showTagInfoAction, SIGNAL(toggled(bool)), this, SLOT(updateShowTagInfo(bool)));

    c->addAction(sortByNameAction);
    c->addAction(sortByArtistAction);
    c->addAction(sortByAlbumAction);
    c->addAction(sortByTitleAction);
    c->addAction(sortByTrackAction);
    c->addAction(sortByYearAction);
    c->addAction(sortByGenreAction);
    c->addAction(sortByCommentAction);
    c->addSeparator();
    c->addAction(showFullNamesAction);
    c->addAction(showTagInfoAction);

    QPoint globalPos = mapToGlobal(p);
    c->exec(globalPos);
}
