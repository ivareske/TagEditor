#include "SettingsClass.h"

 
SettingsClass::SettingsClass(){
    
	startupFolder="C:\\";
	extensions << "*.mp3"<<"*.wma"<<"*.wav"<<"*.ogg"<<"*.aac"<<"*.ac3"<<"*.ape";
	subfolders = true;	
	artistChecked = true;
	titleChecked = true;
	albumChecked = true;
	yearChecked = true;
	trackChecked = true;
	genreChecked = true;
	commentChecked = true;
	saveCover=true;
	saveAllCovers=true;
	api_key = "";
	renameFormat="%artist% - %title%";
	lastStyleSheetFolder="";
}

void SettingsClass::readSettings( QString settingsFile ){

	
	QFile file( settingsFile );
	QString line;
	if (!file.open(QIODevice::ReadOnly)){		
		return;				
	}
		
    QTextStream t( &file );
	bool ok;
	
	line = t.readLine();
    while ( !line.isNull() ) {    
		
		qDebug()<<line;	
		if(line=="startupFolder"){
			line = t.readLine();
			startupFolder = line;
			qDebug()<<"startupFolder: "<<startupFolder;
		}else if(line=="style"){
			line = t.readLine();
			style = line;
			qDebug()<<"style: "<<style;
		}else if(line=="lastStyleSheetFolder"){
			line = t.readLine();
			lastStyleSheetFolder = line;
			qDebug()<<"lastStyleSheetFolder: "<<lastStyleSheetFolder;
		}else if(line=="renameFormat"){
			line = t.readLine();
			renameFormat = line;
			qDebug()<<"renameFormat: "<<renameFormat;
		}else if(line=="api_key"){
			line = t.readLine();
			api_key = line;
			qDebug()<<"api_key: "<<api_key;
		}else if(line=="saveCover"){
			line = t.readLine();
			saveCover = (bool)line.toInt(&ok,10);
			qDebug()<<"saveCover: "<<saveCover;
		}else if(line=="saveAllCovers"){
			line = t.readLine();
			saveAllCovers = (bool)line.toInt(&ok,10);
			qDebug()<<"saveAllCovers: "<<saveAllCovers;
		}else if(line=="subfolders"){
			line = t.readLine();
			subfolders = (bool)line.toInt(&ok,10);
			qDebug()<<"subfolders: "<<subfolders;
		}else if(line=="artistChecked"){
			line = t.readLine();
			artistChecked = (bool)line.toInt(&ok,10);
			qDebug()<<"artistChecked: "<<artistChecked;
		}else if(line=="titleChecked"){
			line = t.readLine();
			titleChecked = (bool)line.toInt(&ok,10);
			qDebug()<<"titleChecked: "<<titleChecked;
		}else if(line=="albumChecked"){
			line = t.readLine();
			albumChecked = (bool)line.toInt(&ok,10);
			qDebug()<<"albumChecked: "<<albumChecked;
		}else if(line=="yearChecked"){
			line = t.readLine();
			yearChecked = (bool)line.toInt(&ok,10);
			qDebug()<<"yearChecked: "<<yearChecked;
		}else if(line=="trackChecked"){
			line = t.readLine();
			trackChecked = (bool)line.toInt(&ok,10);
			qDebug()<<"trackChecked: "<<trackChecked;
		}else if(line=="genreChecked"){
			line = t.readLine();
			genreChecked = (bool)line.toInt(&ok,10);
			qDebug()<<"genreChecked: "<<genreChecked;
		}else if(line=="commentChecked"){
			line = t.readLine();
			commentChecked = (bool)line.toInt(&ok,10);
			qDebug()<<"commentChecked: "<<commentChecked;
		}else if(line=="extensions"){
			qDebug()<<extensions;						 
			line = t.readLine();								
			extensions = line.split(";");
		}else{
			line = t.readLine();
		}
		
    }
	qDebug()<<"finished reading settings";
    file.close();
	
	return;
}

void SettingsClass::saveSettings( QString settingsFile ){

	QFile f( settingsFile );

	if( !f.open( QIODevice::WriteOnly ) ){
		QMessageBox::warning(0, "",
		"Could not open "+settingsFile+" for writing",
		QMessageBox::Ok, QMessageBox::Ok);
	    return;
	}
	QTextStream ts( &f );

	ts << "style" << endl;
	ts << style << endl;
	ts << "lastStyleSheetFolder" << endl;
	ts << lastStyleSheetFolder << endl;
	ts << "renameFormat" << endl;
	ts << renameFormat << endl;
	ts << "saveCover" << endl;
	ts << saveCover << endl;
	ts << "saveAllCovers" << endl;
	ts << saveAllCovers << endl;
	ts << "startupFolder" << endl;
	ts << startupFolder << endl;	
	ts << "api_key" << endl;
	ts << api_key << endl;	
	ts << "subfolders" << endl;
	ts << subfolders << endl;
	ts << "artistChecked" << endl;
	ts << artistChecked << endl;
	ts << "titleChecked" << endl;
	ts << titleChecked << endl;
	ts << "albumChecked" << endl;
	ts << albumChecked << endl;
	ts << "yearChecked" << endl;
	ts << yearChecked << endl;
	ts << "trackChecked" << endl;
	ts << trackChecked << endl;
	ts << "genreChecked" << endl;
	ts << genreChecked << endl;
	ts << "commentChecked" << endl;
	ts << commentChecked << endl;
	ts << "extensions" << endl;
	for(int i=0;i<extensions.size()-1;i++){
		ts << extensions[i]<<";";
	}
	ts << extensions.last()<<endl;
	
	
	f.close();
}

