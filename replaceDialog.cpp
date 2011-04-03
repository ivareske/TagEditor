#include "replaceDialog.h"

replaceDialog::replaceDialog( const QList<TagItem*> &items, QWidget *parent ) : QDialog(parent){
    setupUi(this); // this sets up GUI

    connect( ReplaceButton, SIGNAL( clicked()  ), this, SLOT(replace() ) );
    items_ = items;
    QStringList list; list<<"Artist"<<"Album"<<"Title"<<"Year"<<"Track"<<"Genre"<<"Comment";
    tagComboBox->addItems(list);

    settings = new QSettings("TagEditor.ini",QSettings::IniFormat,0);
    CaseSensitive->setChecked( settings->value("replaceDialog/CaseSensitive",false).toBool() );

    connect( this, SIGNAL( finished( int ) ), this, SLOT( finito( int ) ) );

}

void replaceDialog::finito( int result ){
    settings->setValue( "replaceDialog/CaseSensitive", CaseSensitive->isChecked() );
    settings->sync();
}

void replaceDialog::replace(){

    QProgressDialog p("Replacing tags...", "Cancel", 0, items_.size(), 0);
    p.setWindowModality(Qt::WindowModal);
    Qt::CaseSensitivity case_sens = Qt::CaseInsensitive;
    if(CaseSensitive->isChecked()){
        case_sens = Qt::CaseSensitive;
    }
    QString log; int nReplaced=0;
    TagLib::FileRef f; QString before;
    for(int i=0;i<items_.size();i++){

        p.setValue(i);
        if( p.wasCanceled() ){
            break;
        }

        f = TagLib::FileRef( items_[i]->fileInfo().absoluteFilePath().toStdString().c_str() );
        if( f.tag() ){
            QString field = tagComboBox->currentText().toLower();
            QString tagtxt;
            if( field=="artist" ){
                tagtxt = f.tag()->artist().toCString(); before=tagtxt;
                tagtxt.replace( Before->text(), After->text(), case_sens );
                if(before!=tagtxt){
                    nReplaced++;
                }
                f.tag()->setArtist( tagtxt.toStdString().c_str() );
            }else if(field=="album"){
                tagtxt = f.tag()->album().toCString(); before=tagtxt;
                tagtxt.replace( Before->text(), After->text(), case_sens );
                if(before!=tagtxt){
                    nReplaced++;
                }
                f.tag()->setAlbum( tagtxt.toStdString().c_str() );
            }else if(field=="title"){
                tagtxt = f.tag()->title().toCString(); before=tagtxt;
                tagtxt.replace( Before->text(), After->text(), case_sens );
                if(before!=tagtxt){
                    nReplaced++;
                }
                f.tag()->setTitle( tagtxt.toStdString().c_str() );
            }else if(field=="genre"){
                tagtxt = f.tag()->genre().toCString(); before=tagtxt;
                tagtxt.replace( Before->text(), After->text(), case_sens );
                if(before!=tagtxt){
                    nReplaced++;
                }
                f.tag()->setGenre( tagtxt.toStdString().c_str() );
            }else if(field=="comment"){
                tagtxt = f.tag()->comment().toCString(); before=tagtxt;
                tagtxt.replace( Before->text(), After->text(), case_sens );
                if(before!=tagtxt){
                    nReplaced++;
                }
                f.tag()->setComment( tagtxt.toStdString().c_str() );
            }else if(field=="track"){
                tagtxt = QString::number(f.tag()->track()); before=tagtxt;
                tagtxt.replace( Before->text(), After->text(), case_sens );
                bool ok; int tmp=tagtxt.toInt(&ok);
                if(ok){
                    if(before!=tagtxt){
                        nReplaced++;
                    }
                    f.tag()->setTrack( tmp );
                }else{
                    log.append("\nInvalid value for "+field+": "+tagtxt+". "+field+" must be uint");
                }
            }else if(field=="year"){
                tagtxt = QString::number(f.tag()->year()); before=tagtxt;
                tagtxt.replace( Before->text(), After->text(), case_sens );
                bool ok; int tmp=tagtxt.toInt(&ok);
                if(ok){
                    if(before!=tagtxt){
                        nReplaced++;
                    }
                    f.tag()->setYear( tmp );
                }else{
                    log.append("\nInvalid value for "+field+": "+tagtxt+". "+field+" must be uint");
                }
            }
            f.save();
            items_[i]->clearTags();
            items_[i]->readTags();
        }else{
            log.append("\n Could not get tag for "+items_[i]->fileInfo().absoluteFilePath());
        }

    }
    log.append("\n\nReplaced "+Before->text()+" with "+After->text()+" in "+QString::number(nReplaced)+" tags");
    p.setValue(items_.size());
    textViewer t(0, &log);
    t.exec();

    accept();
}
