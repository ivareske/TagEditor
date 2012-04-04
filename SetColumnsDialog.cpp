#include "SetColumnsDialog.h"


SetColumnsDialog::SetColumnsDialog(const QList<Global::TagField> &columns,QWidget *parent) : QDialog(parent){

    setupUi(this);

    connect( OkButton, SIGNAL( clicked() ), this, SLOT( save() ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );


    QStringList columnStrs;
    columnStrs.append("Dont show");
    for(int i=0;i<Global::NTagFields;i++){
        columnStrs.append(Global::columnText(static_cast<Global::TagField>(i)));
    }

    for(int i=0;i<Global::NTagFields;i++){

        QLayout *l = Frame->layout();
        if(l==0){
            l = new QVBoxLayout;
            Frame->setLayout(l);
        }

        QComboBox *b = new QComboBox;        
        b->addItems(columnStrs);
        comboboxes.append(b);
        l->addWidget(b);
        b->setCurrentIndex(0);
        if(i<columns.size()){
            int ind = columnStrs.indexOf(Global::columnText(columns[i]));
            if(ind!=-1){
                b->setCurrentIndex(ind);
            }
        }
    }


}


void SetColumnsDialog::save(){

    QList<Global::TagField> cols = this->columns();
    if(cols.size()==0){
        QMessageBox::critical(this,"","You must specify at least one column...");
        return;
    }
    accept();

}

QList<Global::TagField> SetColumnsDialog::columns() const{

    QList<Global::TagField> columns;
    for(int i=0;i<comboboxes.size();i++){
        for(int j=0;j<Global::NTagFields;j++){
            if( Global::columnText(static_cast<Global::TagField>(j))==comboboxes[i]->currentText() ){
                columns.append(static_cast<Global::TagField>(j));
                break;
            }
        }
    }
    return columns;
}
