#include "RenameDialog.h"

RenameDialog::RenameDialog( QWidget *parent, QList<TagItem*> *tagitems, QString *renameformat ) : QDialog(parent){
    setupUi(this); // this sets up GUI

    settings = new QSettings("TagEditor.ini",QSettings::IniFormat,0);
    connect( RenameButton, SIGNAL( clicked()  ), this, SLOT(rename() ) );
    tagItems = tagitems;
    qDebug()<<1;
    defaultReplaceFormat<<"\\"<<"_"<<"/"<<"_"<<"*"<<"_"<<":"<<"_"<<"?"<<"_"<<"<"<<"_"<<">"<<"_"<<"|"<<"_"<<"\""<<"_";
    replaceFormat = settings->value("RenameDialog/replaceFormat",defaultReplaceFormat).toStringList();
    notAllowedCharacters<<"\\"<<"/"<<"*"<<":"<<"?"<<"<"<<">"<<"|"<<"\"";
    Format->setText(*renameformat);
    renameFormat = renameformat;
    int k=0;
    qDebug()<<replaceFormat.size();
    ReplaceTable->setRowCount(2);
    ReplaceTable->setColumnCount(9);
    for(int i=0;i<replaceFormat.size();i=i+2){
        QTableWidgetItem *item1 = new QTableWidgetItem;
        item1->setText(replaceFormat[i]);
        item1->setFlags( item1->flags() ^ Qt::ItemIsEditable );
        QTableWidgetItem *item2 = new QTableWidgetItem;
        item2->setText(replaceFormat[i+1]);
        ReplaceTable->setItem(0,k,item1);
        ReplaceTable->setItem(1,k,item2);
        qDebug()<<replaceFormat[i]<<replaceFormat[i+1]<<k;
        k++;
    }

    QStringList vHeaders;
    vHeaders << "To be replaced" << "Replacement";
    ReplaceTable->setVerticalHeaderLabels(vHeaders);


    connect( this, SIGNAL( finished( int ) ), this, SLOT( finito( int ) ) );
    connect( Format, SIGNAL( textChanged( const QString & ) ), this, SLOT( updateFormat(  const QString & ) ) );
    connect( ReplaceTable, SIGNAL( itemChanged( QTableWidgetItem* ) ), this, SLOT( updateReplaceFormat(  QTableWidgetItem* ) ) );

    QAction* setDefaultFormatAction = new QAction(tr("Inset default replace format"), this);
    connect(setDefaultFormatAction, SIGNAL(triggered()), this, SLOT(setDefaultFormat()));

    ReplaceTable->setContextMenuPolicy(Qt::ActionsContextMenu);
    ReplaceTable->addAction(setDefaultFormatAction);

}

void RenameDialog::updateReplaceFormat( QTableWidgetItem* item ){

    int ind = (item->column()*ReplaceTable->rowCount()) + item->row();
    qDebug()<<ind;
    replaceFormat[ind] = item->text();
}

void RenameDialog::updateFormat(  const QString &format ){
    *renameFormat = format;
}

void RenameDialog::finito( int result ){

    settings->setValue( "RenameDialog/replaceFormat", replaceFormat );
    settings->sync();
}

void RenameDialog::setDefaultFormat(){
    int k=0;
    for(int i=0;i<replaceFormat.size();i=i+2){
        QTableWidgetItem *item1 = new QTableWidgetItem;
        item1->setText(replaceFormat[i]);
        item1->setFlags( item1->flags() ^ Qt::ItemIsEditable );
        QTableWidgetItem *item2 = new QTableWidgetItem;
        item2->setText(replaceFormat[i+1]);
        ReplaceTable->setItem(0,k,item1);
        ReplaceTable->setItem(1,k,item2);
        k++;
    }
}

void RenameDialog::rename(){

    QString tmpformat;
    if( replaceFormat.size()%2 != 0 ){
        QMessageBox msgBox;
        msgBox.setText("Replace format must be specified in comma-separated pairs...");
        msgBox.exec();
        return;
    }

    QString log;
    QProgressDialog p("Renaming files...", "Cancel", 0, tagItems->size(), 0);
    for(int i=0;i<tagItems->size();i++){

        p.setValue(i);
        if( p.wasCanceled() ){
            break;
        }

        TagItem *item = tagItems->at(i);
        QFileInfo fi = item->fileInfo();
        tmpformat = Format->text();

        if( !item->tagIsRead() ){
            item->readTags();
        }

        if( item->tagOk() ){
            tmpformat.replace( "%artist%", item->artist() );
            tmpformat.replace( "%album%", item->album() );
            tmpformat.replace( "%title%", item->title() );
            tmpformat.replace( "%genre%", item->genre() );
            tmpformat.replace( "%track%", QString::number( item->track() ) );
            tmpformat.replace( "%year%", QString::number( item->year() ) );
            tmpformat.replace( "%comment%", item->comment() );
            tmpformat.replace( "%bitrate%", QString::number( item->bitRate() ) );
            tmpformat.replace( "%samplerate%", QString::number( item->sampleRate() ) );
            tmpformat.replace( "%length%", QString::number( item->length() ) );
            tmpformat.replace( "%channels%", QString::number( item->channels() ) );

            for(int i=0;i<replaceFormat.size();i=i+2){
                tmpformat.replace( replaceFormat[i], replaceFormat[i+1] );
            }

            bool formatok=true;
            for(int i=0;i<notAllowedCharacters.size();i++){
                if( tmpformat.contains(notAllowedCharacters[i]) ){
                    log.append("\nCharacter "+notAllowedCharacters[i]+" is not allowed in filename and must be replaced.");
                    formatok = false;
                }
            }
            if(!formatok){
                continue;
            }

            QString ext = fi.suffix();
            QString newname = fi.absolutePath()+"/"+tmpformat+"."+ext;
            //QString newnameShort = tmpformat+"."+ext;
            bool ok = QFile::rename( fi.absoluteFilePath(), newname );

            if(!ok){
                if(fi.absoluteFilePath()==newname){
                    //log.append("\nCould not rename "+fi.absoluteFilePath()+" to "+newname);
                }else{
                    log.append("\nCould not rename "+fi.absoluteFilePath()+" to "+newname);
                }
            }else{
                //update listWidgetItem with new name
                item->changeName( newname );
            }
        }else{
            log.append("\nCould not read tag for "+fi.absoluteFilePath());
            //delete f;
        }
    }
    p.setValue(tagItems->size());
    if(!log.isEmpty()){
        TextViewer t(this, &log);
        t.exec();
    }

    accept();
}
