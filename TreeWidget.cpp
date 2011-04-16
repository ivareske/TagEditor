#include "TreeWidget.h"

TreeWidget::TreeWidget(QWidget *parent) : QTreeWidget(parent){


    for(int i=0;i<Global::NTagFields;i++){
        if(static_cast<Global::TagField>(i)!=Global::Comment){
            //dont include comment field as default
            columns_.append(static_cast<Global::TagField>(i));
        }
    }
    defaultColumns_ = columns_;

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(treeWidgetContextMenu(const QPoint &)));
    showTagInfo_ = false;
    showFullFileName_ = false;


    setColumns();

    setSortingEnabled(true);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void TreeWidget::clearTags(){

    for(int i=0;i<topLevelItemCount();i++){
        TagItem *item = tagItem(i);
        item->clearTags();
        item->setColumnData(columns_,showFullFileName_,showTagInfo_);
    }
}

void TreeWidget::setColumnsList( const QList<Global::TagField> &columns ){
    qDebug()<<"TreeWidget::setColumnsList columns: "<<columns;
    columns_ = columns;
    if(columns.isEmpty()){
        columns_ = defaultColumns_;
    }
    setColumns();
}

QList<Global::TagField> TreeWidget::columns() const{
    return columns_;
}

void TreeWidget::setColumnData( TagItem* item ){
    item->setColumnData( columns_, showFullFileName_, showTagInfo_ );
}

void TreeWidget::setColumns(){

    QStringList headers;
    for(int i=0;i<columns_.size();i++){
        headers.append(Global::columnText(columns_[i]));
    }    
    setHeaderLabels( headers );
    setColumnCount(headers.size());
    qDebug()<<headers<<columnCount();
    for(int i=0;i<this->topLevelItemCount();i++){
        this->tagItem(i)->setColumnData( columns_, showFullFileName_, showTagInfo_ );
    }
}

TagItem* TreeWidget::tagItem( int index ){
    if(index>=this->topLevelItemCount()){
        return 0;
    }
    return static_cast<TagItem*>(this->topLevelItem(index));
}


void TreeWidget::addItem( QTreeWidgetItem *item ){

    TagItem *item_ = static_cast<TagItem*>(item);
    /*
    QString str = item_->fileInfo().fileName();
    if( showFullFileName_ ){
        str = item_->fileInfo().filePath();
    }
    item_->setText(Global::FileName,str);
    */
    if( showTagInfo_ ){
        item_->readTags();        
    }
    item_->setColumnData(columns_,showFullFileName_ ,showTagInfo_);
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
        item_->setColumnData(columns_,showFullFileName_ ,showTagInfo_);
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

    //resize columns to content
    QAction* resizeColumnsAction = new QAction(tr("Resize columns to contents"), this);
    connect(resizeColumnsAction, SIGNAL(triggered()), this, SLOT(resizeColumns()));

    //specify columns
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

    SetColumnsDialog d( columns_, this );
    if(d.exec()==QDialog::Accepted){
        columns_ = d.columns();
        setColumns();
    }
}

void TreeWidget::resizeColumns(){
    for(int i=0;i<this->columnCount();i++){
        resizeColumnToContents( i );
    }
}



void TreeWidget::enableSorting(bool enable){
    setSortingEnabled(enable);
}
