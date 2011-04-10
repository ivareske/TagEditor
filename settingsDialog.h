#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
 
#include <QtGui>
#include "SettingsClass.h"
#include "ui_settingsDialog.h"
 
 
class settingsDialog : public QDialog, private Ui::settingsDialog
{
    Q_OBJECT
 
public:

    settingsDialog(QWidget *parent = 0);
	
	
public slots:
	void finito( int result );
	
protected:
	QSettings *settings;
	
};
 
 
#endif






