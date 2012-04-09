#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
 
#include <QtGui>
#include "globalFunctions.h"
#include "ui_settingsDialog.h"
 
 
class SettingsDialog : public QDialog, private Ui::SettingsDialog
{
    Q_OBJECT
 
public:

    SettingsDialog(QWidget *parent = 0);
	
	
public slots:
	void finito( int result );
	
protected:
	QSettings *settings;
	
};
 
 
#endif






