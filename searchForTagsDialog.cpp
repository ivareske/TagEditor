#include "SearchForTagsDialog.h"

SearchForTagsDialog::SearchForTagsDialog( QList<QFileInfo> files, QWidget *parent ) : QDialog(parent){
    setupUi(this); // this sets up GUI


    connect( SearchButton, SIGNAL( clicked()  ), this, SLOT(search() ) );
    fileInfos = files;

    QSettings settings("TagEditor.ini",QSettings::IniFormat,0);
    CaseSensitive->setChecked( settings.value("SearchForTagsDialog/CaseSensitive",false).toBool() );
    artistCheckbox->setChecked( settings.value("SearchForTagsDialog/artistChecked",true).toBool());
    titleCheckbox->setChecked( settings.value("SearchForTagsDialog/titleChecked",true).toBool());
    albumCheckbox->setChecked( settings.value("SearchForTagsDialog/albumChecked",true).toBool());
    yearCheckbox->setChecked( settings.value("SearchForTagsDialog/yearChecked",true).toBool());
    trackCheckbox->setChecked( settings.value("SearchForTagsDialog/trackChecked",true).toBool());
    genreCheckbox->setChecked( settings.value("SearchForTagsDialog/genreChecked",true).toBool());
    commentCheckbox->setChecked( settings.value("SearchForTagsDialog/commentChecked",true).toBool() );

    connect( this, SIGNAL( finished( int ) ), this, SLOT( finito( int ) ) );
    connect( addToWorkSpaceButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    Info->setText(QString::number(fileInfos.size())+" files selected");
}


QList<QFileInfo> SearchForTagsDialog::files(){

    QList<QFileInfo> files;
    for(int i=0;i<foundList->count();i++){        
        files.append(QFileInfo(foundList->item(i)->text()));
    }
    return files;
}


void SearchForTagsDialog::finito( int result ){
    QSettings settings("TagEditor.ini",QSettings::IniFormat,0);
    settings.setValue( "SearchForTagsDialog/CaseSensitive", CaseSensitive->isChecked() );
    settings.setValue("SearchForTagsDialog/artistChecked", artistCheckbox->isChecked() );
    settings.setValue("SearchForTagsDialog/titleChecked", titleCheckbox->isChecked() );
    settings.setValue("SearchForTagsDialog/albumChecked", albumCheckbox->isChecked() );
    settings.setValue("SearchForTagsDialog/yearChecked", yearCheckbox->isChecked() );
    settings.setValue("SearchForTagsDialog/trackChecked", trackCheckbox->isChecked() );
    settings.setValue("SearchForTagsDialog/genreChecked", genreCheckbox->isChecked() );
    settings.setValue("SearchForTagsDialog/commentChecked", commentCheckbox->isChecked() );
    settings.sync();
}

void SearchForTagsDialog::compare( bool *include, QString tagtxt, QString txt, Qt::CaseSensitivity cs ){
    if( txt.isEmpty() ){
        *include = *include || tagtxt.isEmpty();
    }else{
        *include = *include || tagtxt.contains( txt, cs );
    }
}

void SearchForTagsDialog::search(){

    foundList->clear();

    QProgressDialog p("Searching for tags...", "Cancel", 0, fileInfos.size(), 0);
    p.setWindowModality(Qt::WindowModal);
    QString log; QList<QFileInfo> found;
    TagLib::FileRef f;
    Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    if( CaseSensitive->isChecked() ){
        cs = Qt::CaseSensitive;
    }
    for(int i=0;i<fileInfos.size();i++){

        p.setValue(i);
        if( p.wasCanceled() ){
            break;
        }

        bool include=false;
        f = TagLib::FileRef( fileInfos.at(i).absoluteFilePath().toStdString().c_str() );
        if( f.tag() ){
            QString tagtxt;
            if( artistCheckbox->isChecked() ){
                tagtxt = QString(f.tag()->artist().toCString());
                QString txt = Artist->text();
                compare( &include, tagtxt, txt, cs );
            }
            if( albumCheckbox->isChecked() ){
                tagtxt = QString(f.tag()->album().toCString());
                QString txt = Album->text();
                compare( &include, tagtxt, txt, cs );
            }
            if( titleCheckbox->isChecked() ){
                tagtxt = QString(f.tag()->title().toCString());
                QString txt = Title->text();
                compare( &include, tagtxt, txt, cs );
            }
            if( genreCheckbox->isChecked() ){
                tagtxt = QString(f.tag()->genre().toCString());
                QString txt = Genre->text();
                compare( &include, tagtxt, txt, cs );
            }
            if( commentCheckbox->isChecked() ){
                tagtxt = QString(f.tag()->comment().toCString());
                QString txt = Comment->text();
                compare( &include, tagtxt, txt, cs );
            }
            if( trackCheckbox->isChecked() ){
                tagtxt = QString::number(f.tag()->track());
                QString txt = Track->text();
                compare( &include, tagtxt, txt, cs );
            }
            if( yearCheckbox->isChecked() ){
                tagtxt = QString::number(f.tag()->year());
                QString txt = Year->text();
                compare( &include, tagtxt, txt, cs );
            }
        }else{
            log.append("\n Could not get tag for "+fileInfos.at(i).absoluteFilePath());
        }
        if(include){
            qDebug()<<fileInfos.at(i).absoluteFilePath();
            foundList->addItem(fileInfos.at(i).absoluteFilePath());
        }
    }
    p.setValue(fileInfos.size());
    Info->setText(QString::number(foundList->count())+" of "+QString::number(fileInfos.size())+" files matches the search criteria");
    if(!log.isEmpty()){
        TextViewer t(this, &log);
        t.exec();
    }
}
