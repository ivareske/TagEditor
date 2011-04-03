#include "TreeWidget.h"

TreeWidget::TreeWidget(QWidget *parent) : QTreeWidget(parent){

    qDebug()<<1;
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(treeWidgetContextMenu(const QPoint &)));
    showTagInfo_ = false;
    showFullFileName_ = false;
qDebug()<<2;
    QSettings settings(Global::settingsFile,QSettings::IniFormat,this);
   // columns_ = settings.value("columns",QVariant::fromValue< QList<int> >(columns_)).value< QList<int> >();
    if( columns_.isEmpty() ){
        for(int i=0;i<Global::NColumns;i++){
            columns_.append(i);
        }
    }
    qDebug()<<"setColumns1";
    setColumns();
    qDebug()<<"setColumns2";

    setSortingEnabled(true);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void TreeWidget::setColumns(){

    QStringList headers;
    for(int i=0;i<columns_.size();i++){
        if(columns_[i]!=-1){
            QString str = Global::columnText(static_cast<Global::ColumnType>(columns_[i]));
            headers.append(str);
        }
    }
    setHeaderLabels( headers );
    setColumnCount(headers.size());

    /*
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
    */

}

void TreeWidget::addTopLevelItems( const QList<QTreeWidgetItem*> &items ){

    QString txt = "Adding files";
    if( showTagInfo() ){
        QString txt = "Adding files and reading tags";
    }
    QProgressDialog progress(txt, "Abort", 0, items.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    for(int i=0;i<items.size();i++){
        progress.setValue(i);
        if(progress.wasCanceled()){
            return;
        }
        TagItem *item_ = static_cast<TagItem*>(items[i]);
        if( !item_->tagIsRead() && showTagInfo() ){
            item_->readTags();
        }
        QString str = item_->fileInfo().fileName();
        if( showFullFileName_ ){
            str = item_->fileInfo().filePath();
        }
        item_->setText(Global::FileNameColumn,str);
    }
    progress.setValue(items.size());

    QTreeWidget::addTopLevelItems(items);

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
        item_->setColumnData(columns_);
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



void TreeWidget::updateShowTagInfo( bool enable ){
    //må bytte til qtreeview om man skal ha flere kolonner


    showTagInfo_ = enable;
    if(!enable){
        return;
    }

    bool sort = isSortingEnabled();
    setSortingEnabled(false);
    int n = topLevelItemCount();
    QVector<TagItem*> items_; items_.resize(n);
    int k=0;
    QProgressDialog progress("Reading tags...", "Abort", 0, n, this);
    progress.setWindowModality(Qt::WindowModal);
    for(int i=n-1;i>=0;i--){
        progress.setValue(i-n+1);
        if(progress.wasCanceled()){
            showTagInfo_ = false;
            break;
        }
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
    progress.setValue(n);


}

void TreeWidget::treeWidgetContextMenu(const QPoint &p){
    QMenu *c = new QMenu(this);

    qDebug()<<"TreeWidgetContextMenu";

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

    //show full file names
    QAction* editColumnsAction = new QAction(tr("Edit columns..."), this);
    connect(editColumnsAction, SIGNAL(triggered()), this, SLOT(editColumns()));

    c->addAction(showFullNamesAction);
    c->addAction(showTagInfoAction);
    c->addAction(enableSortAction);
    c->addAction(resizeColumnsAction);
    c->addAction(editColumnsAction);

    QPoint globalPos = mapToGlobal(p);
    c->exec(globalPos);
}

void TreeWidget::editColumns(){


    /*SetColumnsDialog d(columns_,this);
    if(d.exec()==QDialog::Accepted){
        columns_ = d.columns();
        QSettings settings(Global::settingsFile,QSettings::IniFormat,this);
        settings.setValue("columns", QVariant::fromValue< QList<int> >(columns_) );
        setColumns();
    }
    */

}

void TreeWidget::resizeColumns(){
    for(int i=0;i<this->columnCount();i++){
        resizeColumnToContents( i );
    }
}



void TreeWidget::enableSorting(bool enable){
    setSortingEnabled(enable);
}
