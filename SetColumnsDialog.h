#ifndef SETCOLUMNSDIALOG_H
#define SETCOLUMNSDIALOG_H

#include <QtGui>
#include "ui_SetColumnsDialog.h"
#include "globalFunctions.h"



class SetColumnsDialog : public QDialog, private Ui::SetColumnsDialog{
    Q_OBJECT
public:
    explicit SetColumnsDialog(const QList<int> &columnIndexes,QWidget *parent = 0);
    QList<int> columns() const;
signals:

public slots:

private:
    QList<QComboBox*> comboboxes;
};

#endif // SETCOLUMNSDIALOG_H
