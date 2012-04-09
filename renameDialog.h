#ifndef RENAMEIALOG_H
#define RENAMEIALOG_H


#include <QtGui>
#include "TextViewer.h"
#include "TagItem.h"
#include "ui_renameDialog.h"


class RenameDialog : public QDialog, private Ui::RenameDialog
{
    Q_OBJECT

public:

    RenameDialog( QList<TagItem*> tagitems, QWidget *parent = 0 );

public slots:
    void rename();
    void finito( int result );
    void setDefaultFormat();
    void updateReplaceFormat( QTableWidgetItem* item );

private:
    QList<TagItem*> tagItems;
    QSettings *settings;
    QStringList defaultReplaceFormat;
    QStringList notAllowedCharacters;
    QStringList replaceFormat;
protected:


};


#endif






