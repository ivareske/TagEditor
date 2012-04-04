#include "TableWidget.h"

TableWidget::TableWidget(QWidget *parent) : QTableWidget(parent){

    originCols_ <<0<<1<<2<<5;
    resultCols_ <<3<<4;

    resizeColumnAction = new QAction(tr("Auto resize columns"), this); resizeColumnAction->setCheckable(true);
    resizeRowAction = new QAction(tr("Auto resize rows"), this); resizeRowAction->setCheckable(true);
    matchByTrackAction = new QAction(tr("Match by track"), this); matchByTrackAction->setData( MATCH_BY_TRACK ); matchByTrackAction->setCheckable(true);
    matchByTitleAction = new QAction(tr("Match by title"), this); matchByTitleAction->setData( MATCH_BY_TITLE ); matchByTitleAction->setCheckable(true);
    dontMatchAction = new QAction(tr("Do not match"), this); dontMatchAction->setData( DONT_MATCH ); dontMatchAction->setCheckable(true);
    matchByTrackTitleAction = new QAction(tr("Match by track, then title, then title to filename"), this); matchByTrackTitleAction->setData( MATCH_BY_TRACK_TITLE ); matchByTrackTitleAction->setCheckable(true);
    matchByTitleTrackAction = new QAction(tr("Match by title, then track, then title to filename"), this); matchByTitleTrackAction->setData( MATCH_BY_TITLE_TRACK ); matchByTitleTrackAction->setCheckable(true);
    matchByFileNameAction = new QAction(tr("Match title to filename"), this); matchByFileNameAction->setData( MATCH_BY_FILE_NAME ); matchByFileNameAction->setCheckable(true);

    //context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu(const QPoint &)));
    contextMenu(QPoint(), true ); //initialize actions

    //verticla header context menu
    this->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this->verticalHeader(), SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(verticalContextMenu(const QPoint &)));
    verticalContextMenu(QPoint(), true ); //initialize actions

    connect( this, SIGNAL( cellChanged( int, int )  ), this, SLOT( updateCell(int, int) ) );

    settings = new QSettings("TagEditor.ini",QSettings::IniFormat,this);

    resizeColumns( settings->value("SearchDialog/autoResizeColumns",true).toBool() );
    resizeColumnAction->setChecked( settings->value("SearchDialog/autoResizeColumns",true).toBool() );
    resizeRows( settings->value("SearchDialog/autoResizeRows",false).toBool() );
    resizeRowAction->setChecked( settings->value("SearchDialog/autoResizeRows",false).toBool() );
    setSortingEnabled( settings->value("SearchDialog/sortingEnabled",true).toBool() );
    if( isSortingEnabled() ){
        int sortColumn = settings->value("SearchDialog/sortColumn",CURRENT_TRACK).toInt();
        Qt::SortOrder sortOrder = (Qt::SortOrder)settings->value("SearchDialog/sortOrder", Qt::AscendingOrder ).toInt();
        sortByColumn( sortColumn, sortOrder );
        horizontalHeader()->setSortIndicator(sortColumn, sortOrder);
        horizontalHeader()->setSortIndicatorShown(true);
        horizontalHeader()->setClickable(true);
    }
    matchByTrackAction->setChecked( settings->value("SearchDialog/matchByTrack",false).toBool() );
    matchByTitleAction->setChecked( settings->value("SearchDialog/matchByTitle",false).toBool() );
    dontMatchAction->setChecked( settings->value("SearchDialog/dontMacth",true).toBool() );
    matchByTrackTitleAction->setChecked( settings->value("SearchDialog/matchByTrackTitle",true).toBool() );
    matchByTitleTrackAction->setChecked( settings->value("SearchDialog/matchByTitleTrack",false).toBool() );
    matchByFileNameAction->setChecked( settings->value("SearchDialog/matchByFileName",false).toBool() );

    this->horizontalHeader()->setStretchLastSection(true);
    this->verticalHeader()->setStretchLastSection(true);

}

void TableWidget::enableSorting( bool enable ){
    setSortingEnabled(enable);
}


int TableWidget::matchResult( const QVariant &toMatch, int matchCol ){
    //qDebug()<<"matching";
    int res=-1;
    for(int i=0;i<rowCount();i++){
        if( item(i,TITLE) || item(i,TRACK) || !item(i,matchCol) ){
            //if an item already set there (already matched), or item to compare to doesn`t exist, continue
            continue;
        }
        if( matchCol==CURRENT_TRACK ){
            //compare track as int if can be converted, else as strings
            bool ok1,ok2;
            int comp1 = item(i,matchCol)->text().toInt(&ok1);
            int comp2 = toMatch.toInt(&ok2);
            if( ok1 && ok2){
                if( comp1==comp2 ){
                    //qDebug()<<"int "<<comp1<<"=="<<comp2;
                    res = i;
                    break;
                }
            }else if( item(i,matchCol)->text()==toMatch ){
                //qDebug()<<"string "<<item(i,matchCol)->text()<<"=="<<toMatch.toString();
                res = i;
                break;
            }
        }else{
            //compare title or filename by contains
            QString match1 = item(i,matchCol)->text();
            QString match2 = toMatch.toString();
            if( match1.contains( match2, Qt::CaseInsensitive ) || match2.contains( match1, Qt::CaseInsensitive ) ){
                //qDebug()<<"string "<<item(i,matchCol)->text()<<"contains"<<toMatch.toString();
                res = i;
                break;
            }
        }

    }
    qDebug()<<"matched "<<toMatch.toString()<<" to row "<<res;
    return res;
}


void TableWidget::removeRows(){

    QModelIndexList indexes = this->selectedIndexes();
    qSort(indexes.begin(), indexes.end());
    for(int i=indexes.size()-1;i>=0;i--){
        removeRow(indexes[i].row());
    }

}

void TableWidget::unCheckMatchActions(){

    matchByTrackAction->setChecked( false );
    matchByTitleAction->setChecked( false );
    dontMatchAction->setChecked( false );
    matchByTitleTrackAction->setChecked( false );
    matchByTrackTitleAction->setChecked( false );
    matchByFileNameAction->setChecked( false );
}

void TableWidget::insertBlankRow(){

    int ind = currentRow()+1;
    if(ind==-1){
        return;
    }
    bool enabled = isSortingEnabled();
    setSortingEnabled(false);
    int n = rowCount();
    //QList<int> inds;inds<<0<<1<<2<<5;
    QAction *action = qobject_cast<QAction *>(sender());
    ColumnType t = static_cast<ColumnType>(action->data().toInt());
    QList<int> inds;
    if(t==ORIGIN){
        inds = originCols_;
        qDebug()<<"origin columns";
    }else if(t==RESULT){
        qDebug()<<"result columns";
        inds = resultCols_;
    }else{
        QMessageBox::critical(this,"","Bug, unknown columntype in TableWidget::insertBlankRow");
        return;
    }

    for(int i=0;i<inds.size();i++){
        int col = inds[i];
        QList<TableWidgetItem*> tmp;
        for(int row=ind;row<n;row++){
            tmp << (TableWidgetItem*)takeItem( row, col );
        }
        int row = ind+1;
        for(int j=0;j<tmp.size();j++){
            if(row>=rowCount()){
                setRowCount(row+1);
                setNonEditable( row+1, row+1 );
            }
            setItem( row, col, tmp.at(j) );
            //qDebug()<<i<<j;
            row++;
        }
        //blank item
        TableWidgetItem *newItem = new TableWidgetItem;
        newItem->setFlags( newItem->flags() &= ~Qt::ItemIsEditable );
        setItem( ind, col, newItem );
    }
    setSortingEnabled(enabled);
    //nFiles=nFiles+1;

    for(int i=0;i<rowCount();i++){
        item(i,CURRENT_TITLE)->setFlags( item(i,CURRENT_TITLE)->flags() &= ~Qt::ItemIsEditable );
        item(i,CURRENT_TITLE)->setFlags( item(i,CURRENT_TITLE)->flags() &= ~Qt::ItemIsEditable );
    }

}

TableWidget::~TableWidget(){

    settings->beginGroup("SearchDialog");
    settings->setValue( "matchByTrack", matchByTrackAction->isChecked() );
    settings->setValue( "matchByTitle", matchByTitleAction->isChecked() );
    settings->setValue( "dontMacth", dontMatchAction->isChecked() );
    settings->setValue( "matchByTrackTitle", matchByTrackTitleAction->isChecked() );
    settings->setValue( "matchByTitleTrack", matchByTitleTrackAction->isChecked() );
    settings->setValue( "matchByFileName", matchByFileNameAction->isChecked() );
    settings->setValue( "autoResizeColumns", resizeColumnAction->isChecked() );
    settings->setValue( "autoResizeRows", resizeRowAction->isChecked() );
    settings->endGroup();
    settings->sync();

    delete settings;
}

void TableWidget::setItems( const QList<TagItem*> &items ){

    items_ = items;

    //nFiles = infos->size();
    this->setRowCount(items_.size());
    this->setColumnCount(6);
    QStringList headers;
    headers << "File" << "Current title" << "Current track" << "Found title" << "Found track"<<"Current comments";
    this->setHorizontalHeaderLabels(headers);

    QString artist = "";
    QString album = "";
    QString year = "";
    QString genre = "";

    bool enabled = this->isSortingEnabled();
    this->setSortingEnabled(false);
    for(int i=0;i<items_.size();i++){

        QString fullfile = items_[i]->fileInfo().filePath();

        TableWidgetItem *newItem = static_cast<TableWidgetItem*>(takeItem(i,FILE_NAME));
        if(!newItem){
            newItem = new TableWidgetItem;
        }
        newItem->setText( items_[i]->fileInfo().fileName() );
        newItem->setData( Qt::UserRole, QVariant(fullfile) );
        QFont font = newItem->font();font.setBold(true);
        newItem->setFont(font);
        newItem->setFlags( newItem->flags() | Qt::ItemIsUserCheckable );
        newItem->setFlags( newItem->flags() &= ~Qt::ItemIsEditable );
        newItem->setCheckState(Qt::Checked);
        this->setItem(i, FILE_NAME, newItem);


        QString tagTitle="Could not get tag title...";
        QString tagTrack="Could not get tag track...";
        QString tagComment="Could not get tag comment...";
        if( items_[i]->tagOk()  ){
            tagTitle = items_[i]->title();
            tagTrack = QString::number( items_[i]->track() );
            tagComment = items_[i]->comment();
            if(artist.isEmpty()){
                artist = items_[i]->artist();
            }
            if(album.isEmpty()){
                album = items_[i]->album();
            }
            if(year.isEmpty()){
                year = QString::number( items_[i]->year() );
            }
            if(genre.isEmpty()){
                genre = items_[i]->genre();
            }

        }

        TableWidgetItem *newItem1 = static_cast<TableWidgetItem*>(takeItem(i,CURRENT_TITLE));
        if(!newItem1){
            newItem1 = new TableWidgetItem;
        }
        newItem1->setText( tagTitle );
        newItem1->setFont(font);
        newItem1->setFlags( newItem1->flags() &= ~Qt::ItemIsEditable );
        this->setItem(i, CURRENT_TITLE, newItem1);

        TableWidgetItem *newItem2 = static_cast<TableWidgetItem*>(takeItem(i,CURRENT_TRACK));
        if(!newItem2){
            newItem2 = new TableWidgetItem;
        }
        newItem2->setText( tagTrack );
        newItem2->setFont(font);
        newItem2->setFlags( newItem2->flags() &= ~Qt::ItemIsEditable );
        this->setItem(i, CURRENT_TRACK, newItem2);

        TableWidgetItem *newItem3 = static_cast<TableWidgetItem*>(takeItem(i,COMMENT));
        if(!newItem3){
            newItem3 = new TableWidgetItem;
        }
        newItem3->setText( tagComment );
        this->setItem(i, COMMENT, newItem3);

        TableWidgetItem *newItem4 = static_cast<TableWidgetItem*>(takeItem(i,TITLE));
        if(!newItem4){
            newItem4 = new TableWidgetItem;
        }
        this->setItem(i, TITLE, newItem4);

        TableWidgetItem *newItem5 = static_cast<TableWidgetItem*>(takeItem(i,TRACK));
        if(!newItem5){
            newItem5 = new TableWidgetItem;
        }
        this->setItem(i, TRACK, newItem5);

    }
    this->setSortingEnabled(enabled);


}


void TableWidget::showAlbumInfo( const Album &a ){

    int n = std::max( a.songs().size(), this->rowCount() );
    setRowCount(n);
    bool enabled = isSortingEnabled();
    setSortingEnabled(false);

    //delete old results
    for(int i=0;i<rowCount();i++){
        delete takeItem(i,TITLE);
        delete takeItem(i,TRACK);
    }

    int k=0;
    QList<Song> leftOverSongs;
    QList<Song> songs = a.songs();
    while( songs.size()>0 ){
        int row;
        //qDebug()<<a.songs[0].title;
        if(matchByTrackAction->isChecked()){
            row = matchResult( songs[0].track(), CURRENT_TRACK );
        }else if(matchByTitleAction->isChecked()){
            row = matchResult( songs[0].title(), CURRENT_TITLE );
        }else if(matchByTitleTrackAction->isChecked()){
            row = matchResult( songs[0].title(), CURRENT_TITLE );
            if(row==-1){
                row = matchResult( songs[0].track(), CURRENT_TRACK );
            }
            if(row==-1){
                row = matchResult( songs[0].title(), FILE_NAME );
            }
        }else if(matchByTrackTitleAction->isChecked()){
            row = matchResult( songs[0].track(), CURRENT_TRACK );
            if(row==-1){
                row = matchResult( songs[0].title(), CURRENT_TITLE );
            }
            if(row==-1){
                row = matchResult( songs[0].title(), FILE_NAME );
            }
        }else if(matchByFileNameAction->isChecked()){
            row = matchResult( songs[0].title(), FILE_NAME );
        }else if(dontMatchAction->isChecked()){;
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
        setItem(row, TITLE, newItem);

        TableWidgetItem *newItem1 = new TableWidgetItem;
        newItem1->setText(songs[0].track());
        newItem1->setFont(f);
        setItem(row, TRACK, newItem1);

        songs.removeAt(0);
    }
    for(int i=0;i<leftOverSongs.size();i++){
        int n = rowCount();
        int row=n+1;
        for(int j=0;j<n;j++){
            if(!item(j,TITLE)){
                row=j;
                break;
            }
        }
        qDebug()<<leftOverSongs[i].title()<<" leftover, put in row "<<row;
        TableWidgetItem *newItem = new TableWidgetItem;
        newItem->setText(leftOverSongs[i].title());
        QFont f = newItem->font();f.setPointSize(10);
        newItem->setFont(f);
        setItem(row, TITLE, newItem);

        TableWidgetItem *newItem1 = new TableWidgetItem;
        newItem1->setText(leftOverSongs[i].track());
        newItem1->setFont(f);
        setItem(row, TRACK, newItem1);
    }
    setSortingEnabled(enabled);
    setNonEditable( 0, n );
}



void TableWidget::contextMenu(const QPoint &p, bool init ){

    /*
    QModelIndex index = this->indexAt(p);
    int col = index.column();
    //int row = index.row();
    QString str = "file";
    ColumnType t = ORIGIN;
    if(col==TRACK || col==TITLE){
        str = "result";
        t = RESULT;
    }
    */


    QMenu *c = new QMenu(this);


    QAction* selectAction = new QAction("Check selected rows", this);
    selectAction->setData(true);
    connect(selectAction, SIGNAL(triggered()), this, SLOT(checkRows()));
    QAction* deselectAction = new QAction("Uncheck selected rows", this);
    deselectAction->setData(false);
    connect(deselectAction, SIGNAL(triggered()), this, SLOT(checkRows()));

    //move up/down actions for this
    QAction* moveResultUpAction = new QAction("Move result up", this);
    moveResultUpAction->setData(RESULT); moveResultUpAction->setShortcut(tr("Ctrl+U"));
    connect(moveResultUpAction, SIGNAL(triggered()), this, SLOT(moveRowUp()));
    QAction* moveResultDownAction = new QAction("Move result down", this);
    moveResultDownAction->setData(RESULT); moveResultDownAction->setShortcut(tr("Ctrl+D"));
    connect(moveResultDownAction, SIGNAL(triggered()), this, SLOT(moveRowDown()));
    QAction* moveResultToAction = new QAction("Move result to...", this);
    moveResultToAction->setData(RESULT); moveResultToAction->setShortcut(tr("Ctrl+T"));
    connect(moveResultToAction, SIGNAL(triggered()), this, SLOT(moveRowTo()));

    QAction* moveOriginUpAction = new QAction("Move file up", this);
    moveOriginUpAction->setData(ORIGIN); moveOriginUpAction->setShortcut(tr("Ctrl+Alt+U"));
    connect(moveOriginUpAction, SIGNAL(triggered()), this, SLOT(moveRowUp()));
    QAction* moveOriginDownAction = new QAction("Move file down", this);
    moveOriginDownAction->setData(ORIGIN); moveOriginDownAction->setShortcut(tr("Ctrl+Alt+D"));
    connect(moveOriginDownAction, SIGNAL(triggered()), this, SLOT(moveRowDown()));
    QAction* moveOriginToAction = new QAction("Move file to...", this);
    moveOriginToAction->setData(ORIGIN); moveOriginToAction->setShortcut(tr("Ctrl+Alt+T"));
    connect(moveOriginToAction, SIGNAL(triggered()), this, SLOT(moveRowTo()));

    QAction* sortEnabledAction = new QAction(tr("Sorting enabled"), this);
    sortEnabledAction->setCheckable(true); sortEnabledAction->setChecked( this->isSortingEnabled() );
    connect(sortEnabledAction, SIGNAL(toggled(bool)), this, SLOT(enableSorting(bool)));

    resizeColumnAction->setShortcut(tr("Ctrl+R"));
    connect(resizeColumnAction, SIGNAL(toggled(bool)), this, SLOT(resizeColumns(bool)));

    connect(resizeRowAction, SIGNAL(toggled(bool)), this, SLOT(resizeRows(bool)));
    QAction* deleteAction = new QAction(tr("Delete selected cells"), this);
    deleteAction->setShortcut(tr("Ctrl+Del"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteCells()));

    QAction* insertBlankFileAction = new QAction("Insert empty file", this);
    insertBlankFileAction->setData(ORIGIN);
    connect(insertBlankFileAction, SIGNAL(triggered()), this, SLOT(insertBlankRow()));
    QAction* insertBlankResultAction = new QAction("Insert empty result", this);
    insertBlankResultAction->setData(RESULT);
    connect(insertBlankResultAction, SIGNAL(triggered()), this, SLOT(insertBlankRow()));

    QAction* copyAction = new QAction(tr("Copy"), this);
    copyAction->setShortcut(tr("Ctrl+C"));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copyCells()));

    QAction* pasteAction = new QAction(tr("Paste"), this);
    pasteAction->setShortcut(tr("Ctrl+V"));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));

    QAction* resetAction = new QAction(tr("Reset"), this);
    connect(resetAction, SIGNAL(triggered()), this, SLOT(resetTable()));

    QMenu* matchByMenu = new QMenu(tr("Match results by..."), this);
    connect(matchByTrackAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    connect(matchByTitleAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    connect(dontMatchAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    connect(matchByTrackTitleAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    connect(matchByTitleTrackAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));
    connect(matchByFileNameAction, SIGNAL(triggered(bool)), this, SLOT(matchBy(bool)));

    matchByMenu->addAction( dontMatchAction );
    matchByMenu->addAction( matchByTitleAction );
    matchByMenu->addAction( matchByTrackAction );
    matchByMenu->addAction( matchByTrackTitleAction );
    matchByMenu->addAction( matchByTitleTrackAction );
    matchByMenu->addAction( matchByFileNameAction );

    c->addMenu(matchByMenu);
    c->addSeparator();
    //if(col==FILE_NAME){
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
    c->addSeparator();
    c->addAction(resetAction);


    QPoint globalPos = this->mapToGlobal(p);
    //menu->exec( globalPos );
    if(!init){
        c->exec(globalPos);
    }else{
        this->addAction( dontMatchAction );
        this->addAction( matchByTitleAction );
        this->addAction( matchByTrackAction );
        this->addAction( matchByTrackTitleAction );
        this->addAction( matchByTitleTrackAction );
        this->addAction( matchByFileNameAction );

        this->addAction(deselectAction);
        this->addAction(selectAction);

        this->addAction(moveResultUpAction);
        this->addAction(moveResultDownAction);
        this->addAction(moveResultToAction);
        this->addAction(moveOriginUpAction);
        this->addAction(moveOriginDownAction);
        this->addAction(moveOriginToAction);

        this->addAction(resizeColumnAction);
        this->addAction(resizeRowAction);
        this->addAction(sortEnabledAction);

        this->addAction(copyAction);
        this->addAction(pasteAction);
        this->addAction(deleteAction);

        this->addAction(insertBlankFileAction);
        this->addAction(insertBlankResultAction);

    }
}

void TableWidget::resetTable(){
    clear();
    setItems(items_);
}

void TableWidget::verticalContextMenu(const QPoint &p, bool init ){

    QMenu *c = new QMenu(this);

    //cut
    QAction* cutAction = new QAction(tr("Cut selected rows"), this);
    cutAction->setShortcut(tr("Ctrl+C"));
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));

    //insert
    QAction* insertCutRowsAction = new QAction("Insert cut rows", this);
    insertCutRowsAction->setShortcut(tr("Ctrl+I"));
    connect(insertCutRowsAction, SIGNAL(triggered()), this, SLOT(insertCutRows()));

    //delete
    QAction* removeRowAction = new QAction(tr("Delete rows"), this);
    removeRowAction->setShortcut(tr("Ctrl+D"));
    connect(removeRowAction, SIGNAL(triggered()), this, SLOT(removeRows()));

    c->addAction(cutAction);
    c->addAction(insertCutRowsAction);
    c->addAction(removeRowAction);

    QPoint globalPos = this->mapToGlobal(p);
    //menu->exec( globalPos );
    if(!init){
        c->exec(globalPos);
    }else{
        this->verticalHeader()->addAction( cutAction );
        this->verticalHeader()->addAction( insertCutRowsAction );
        this->verticalHeader()->addAction( removeRowAction );
    }
}

void TableWidget::moveRowTo(){

    bool ok;
    int n = rowCount();
    int row = QInputDialog::getInt(this, "",
                                   tr("Move to row:"), 1, 1, n, 1, &ok)-1; // -1,let rows start at 1 instead of 0 for user
    if(!ok){
        return;
    }
    int current = currentRow();
    if( row<0 || row>=n ){
        QMessageBox::critical(this, "Move row "+QString::number(current)+" to:",
                              "Row nr out of bounds",QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    QAction *action = qobject_cast<QAction *>(sender());
    ColumnType t = (ColumnType)action->data().toInt();
    moveRow( current, row, t );
}

void TableWidget::moveRow( int from, int to, ColumnType t ){
    //move current row to row

    int n = rowCount();
    if( n==0 ){
        return;
    }
    if( from<0 || to<0	|| from>=n || to>=n || from==to ){
        return;
    }
    QList<int> inds;
    if(t==ORIGIN){
        inds = originCols_;
        qDebug()<<"moving origin columns";
    }else{
        qDebug()<<"moving result columns";
        inds = resultCols_;
    }
    QTableWidgetItem* currItem = this->currentItem();
    bool enabled = isSortingEnabled();
    setSortingEnabled(false);
    for(int i=0;i<inds.size();i++){
        int col = inds[i];
        QList<TableWidgetItem*> tmp;
        for(int row=0;row<n;row++){
            tmp << (TableWidgetItem*)takeItem( row, col );
        }
        qDebug()<<"moving column "<<col<<" from "<<from<<"to "<<to;
        tmp.insert( to, tmp.takeAt(from) );
        for(int row=0;row<n;row++){
            setItem( row, col, tmp.at(row) );
        }
    }
    setSortingEnabled(enabled);
    if(enabled){
        QMessageBox::warning(this,"Move row","Sorting is enabled, so moving a row might not have any effect");
    }
    this->setCurrentItem(currItem);
}

void TableWidget::moveRowUp(){
    QAction *action = qobject_cast<QAction *>(sender());
    ColumnType t = (ColumnType)action->data().toInt();
    int current = currentRow();
    moveRow( current, current-1, t );
}

void TableWidget::moveRowDown(){
    QAction *action = qobject_cast<QAction *>(sender());
    ColumnType t = (ColumnType)action->data().toInt();
    int current = currentRow();
    moveRow( current, current+1, t );
}

void TableWidget::setNonEditable( int startrow, int stoprow ){

    bool enabled = isSortingEnabled();
    setSortingEnabled(false);
    for(int i=0;i<TITLE;i++){
        for(int j=startrow;j<stoprow;j++){
            if( j>=rowCount() ){
                continue;
            }
            if( !item(j,i) ){
                TableWidgetItem *item = new TableWidgetItem;
                item->setFlags( item->flags() &= ~Qt::ItemIsEditable );
                setItem( j, i, item );
            }else{
                item(j,i)->setFlags( item(j,i)->flags() &= ~Qt::ItemIsEditable );
            }
        }
    }
    setSortingEnabled(enabled);

}

void TableWidget::checkRows(){

    QAction *action = qobject_cast<QAction*>(sender());
    bool checked = action->data().toBool();
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if(ranges.size()==0){
        return;
    }
    QTableWidgetSelectionRange range = ranges[0];
    for(int i=range.topRow(); i<=range.bottomRow(); ++i) {
        if(!item(i,0)){
            continue;
        }
        if(checked){
            item(i,0)->setCheckState(Qt::Checked);
        }else{
            item(i,0)->setCheckState(Qt::Unchecked);
        }
    }

}


void TableWidget::paste(){

    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if(ranges.size()==0){
        return;
    }
    QTableWidgetSelectionRange range = ranges[0];
    if(range.leftColumn()<TITLE || range.rightColumn()<TITLE){
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
    bool enabled = isSortingEnabled();
    setSortingEnabled(false);
    for(int i=0; i<numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for(int j=0; j<numColumns; ++j) {
            int row = range.topRow() +i;
            int column = range.leftColumn() +j;
            if(row < rowCount() && column < columnCount()){
                if(!item(row,column)){
                    TableWidgetItem *item = new TableWidgetItem;
                    setItem(row,column,item);
                }
                item(row,column)->setText(columns[j]);
            }
        }
    }
    setSortingEnabled(enabled);
}

void TableWidget::cut(){
    QModelIndexList indexes = this->selectedIndexes();
    for(int i=0;i<indexes.size();i++){
        QList<TableWidgetItem*> row_;
        for(int j=0;this->columnCount();j++){
            row_.append(static_cast<TableWidgetItem*>(takeItem(indexes[i].row(),j)));
        }
        this->removeRow(indexes[i].row());
        cutRows_.append(row_);
    }
}

void TableWidget::insertCutRows(){

    if(cutRows_.size()==0 || currentRow()==-1){
        return;
    }
    bool sortEnabled = this->isSortingEnabled();
    setSortingEnabled(false);
    for(int i=0;i<cutRows_.size();i++){
        this->insertRow(currentRow());
        QList<TableWidgetItem*> row = cutRows_[i];
        for(int j=0;j<row.size();j++){
            TableWidgetItem* item_ = row[j];
            this->setItem(currentRow(),j,item_);
        }
    }
    cutRows_.clear();
    this->setSortingEnabled(sortEnabled);
}


void TableWidget::copyCells(){

    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
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
            if(!item(i,j)){
                str += "";
            }else{
                str += item(range.topRow() +i, range.leftColumn()+j)->text();
                qDebug()<<"adding to clipbard: "<<item(range.topRow() +i, range.leftColumn()+j)->text();
            }
        }
    }
    QApplication::clipboard()->setText(str);
}


void TableWidget::deleteCells(){

    bool enabled = isSortingEnabled();
    setSortingEnabled(false);
    QModelIndexList indexes = selectionModel()->selectedIndexes();
    for(int i=0;i<indexes.size();i++){
        int row = indexes[i].row();
        int column = indexes[i].column();
        //if( item(row,column) && !originCols_.contains(column) ){
        if( item(row,column)->flags().testFlag(Qt::ItemIsEditable) ){
            item(row,column)->setText("");
            //delete takeItem(row,column);
            //TableWidgetItem *newItem = new TableWidgetItem;
            //newItem->setFlags( newItem->flags() &= ~Qt::ItemIsEditable );
            //setItem( row, column, newItem );
        }
    }
    setSortingEnabled(enabled);    
}

void TableWidget::matchBy(bool state){

    QAction *action = qobject_cast<QAction*>(sender());

    unCheckMatchActions();
    action->setChecked(true);

}

void TableWidget::updateCell( int row, int col ){

    //only current track & title, and track and title
    if(col<1 || col>4 ){
        return;
    }
    int currentCol=col-2;
    if(col<3){
        currentCol=col;
        col = currentCol+2;
    }
    TableWidgetItem *item1  = static_cast<TableWidgetItem*>(item(row,currentCol));
    TableWidgetItem *item2 = static_cast<TableWidgetItem*>(item(row,col));
    if( !item2 || !item1 || item2->text().isEmpty() ){
        return;
    }

    QColor q;
    if( item1->text().toLower()==item2->text().toLower() ){
        q = QColor( Qt::white );
    }else{
        q = QColor( Qt::gray );
    }
    item1->setData(Qt::BackgroundRole, q);

}


void TableWidget::resizeHeader( QHeaderView *header, bool state ){
    QHeaderView::ResizeMode mode;
    if(state){
        mode = QHeaderView::ResizeToContents;
    }else{
        mode = QHeaderView::Interactive;
    }
    header->setResizeMode( mode );
}

void TableWidget::resizeColumns(bool state){
    resizeHeader( horizontalHeader(), state );
}

void TableWidget::resizeRows(bool state){
    resizeHeader( verticalHeader(), state );
}
