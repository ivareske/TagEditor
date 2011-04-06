#include "SetColumnsDialog.h"

SetColumnsDialog::SetColumnsDialog(const QList<int> &columnIndexes,QWidget *parent) : QDialog(parent){

    setupUi(this);

    connect( OkButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );


    QStringList columns;
    columns.append("Dont show");
    for(int i=0;i<Global::NColumns;i++){
        columns.append(Global::columnText(static_cast<Global::ColumnType>(i)));
    }
    for(int i=0;i<Global::NColumns;i++){
        QLayout *l = Frame->layout();
        if(l==0){
            l = new QVBoxLayout;
            Frame->setLayout(l);
        }
        QComboBox *b = new QComboBox;
        b->addItems(columns);
        comboboxes.append(b);
        l->addWidget(b);
        b->setCurrentIndex(0);
        if( i<columnIndexes.size() ){
            if( columnIndexes[i]<columns.size() && columnIndexes[i]>=0 ){
                b->setCurrentIndex(columnIndexes[i]);
            }
        }
    }


}

QList<int> SetColumnsDialog::columns() const{

    QList<int> columns;
    for(int i=0;i<comboboxes.size();i++){
        int columntype = comboboxes[i]->currentIndex()-1;
        columns.append(columntype);
    }
    return columns;
}
