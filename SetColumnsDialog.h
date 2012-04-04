#ifndef SETCOLUMNSDIALOG_H
#define SETCOLUMNSDIALOG_H

#include <QtGui>
#include "ui_SetColumnsDialog.h"
#include "globalFunctions.h"



class SetColumnsDialog : public QDialog, private Ui::SetColumnsDialog{
    Q_OBJECT
public:
    explicit SetColumnsDialog(const QList<Global::TagField> &columnIndexes,QWidget *parent = 0);
    QList<Global::TagField> columns() const;

signals:

public slots:

protected:

private slots:
    void save();

private:
    QList<QComboBox*> comboboxes;
};

#endif // SETCOLUMNSDIALOG_H
