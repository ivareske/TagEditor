#include "textViewer.h"

 
textViewer::textViewer(QWidget *parent, QString *text) : QDialog(parent){
    setupUi(this); // this sets up GUI 	       	
	if(!text->isNull()){
		textEdit->setText( *text );
	}
        connect(SelectFontButton,SIGNAL(clicked()), this,SLOT(selectFont()) );
}


void textViewer::setText( QString text ){
	textEdit->setText( text );
}

void textViewer::selectFont(){
    bool ok;
    QFont font = QFontDialog::getFont(&ok, textEdit->font(), this);
    if(!ok){
        return;
    }
    textEdit->setFont(font);
}
