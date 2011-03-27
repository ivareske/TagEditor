#ifndef SETTINGSCLASS_H
#define SETTINGSCLASS_H

#include <QtGui>


class settingsClass{

public:
	settingsClass();
		
	QString startupFolder;
	QStringList extensions;
	QString api_key;
	bool subfolders;
	bool artistChecked;
	bool titleChecked;
	bool albumChecked;
	bool yearChecked;
	bool trackChecked;
	bool genreChecked;
	bool commentChecked;
	bool saveCover;
	bool saveAllCovers;
	QString renameFormat;
	QString lastStyleSheetFolder;
	QString style;
	
	void readSettings( QString settingsFile );
	void saveSettings( QString settingsFile );

};


 
#endif

