#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent){
    setupUi(this); // this sets up GUI

    //settings = new QSettings("TagEditor.ini",QSettings::IniFormat,this);
    settings = new QSettings(Global::settingsFile,QSettings::IniFormat,this);
    api_key->setText( settings->value("SearchDialog/api_key","").toString() );

    QString defaultExt = "*.mp3;*.wma;*.wav;*.ogg;*.aac;*.ac3;*.m4a";
    extensions->setText( settings->value( "extensions", defaultExt.split(";") ).toStringList().join(";") );
    subfolders->setChecked( settings->value("subfolders",true).toBool() );
    coverSpinBox->setValue( settings->value("SearchDialog/coverQuality",-1).toInt() );

    // signals/slots
    connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( this, SIGNAL( finished( int ) ), this, SLOT( finito( int ) ) );

}


void SettingsDialog::finito( int result ){

    if( result==QDialog::Accepted ){
        settings->setValue( "subfolders", subfolders->isChecked() );
        settings->setValue( "SearchDialog/api_key", api_key->text() );
        settings->setValue( "extensions", extensions->text().split(";") );
        settings->setValue( "SearchDialog/coverQuality", coverSpinBox->value() );
        settings->sync();
    }

}
