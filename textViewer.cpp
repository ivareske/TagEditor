#include "TextViewer.h"

 
TextViewer::TextViewer(QWidget *parent, QString *text) : QDialog(parent){
    setupUi(this); // this sets up GUI 	       	
	if(!text->isNull()){
		textEdit->setText( *text );
	}
        connect(SelectFontButton,SIGNAL(clicked()), this,SLOT(selectFont()) );
}


void TextViewer::setText( QString text ){
	textEdit->setText( text );
}

void TextViewer::selectFont(){
    bool ok;
    QFont font = QFontDialog::getFont(&ok, textEdit->font(), this);
    if(!ok){
        return;
    }
    textEdit->setFont(font);
}
