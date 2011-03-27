#ifndef SEARCHFORTAGSDIALOG_H
#define SEARCHFORTAGSDIALOG_H

#include <QtGui>
#include "textViewer.h"
#include "TagItem.h"
#include "ui_searchForTagsDialog.h"


class searchForTagsDialog : public QDialog, private Ui::searchForTagsDialog
{
    Q_OBJECT

public:

    searchForTagsDialog( QList<QFileInfo> files, QWidget *parent = 0 );

public slots:
    void search();
    void finito( int result );
    QList<QFileInfo> files();
    void compare( bool *include, QString tagtxt, QString txt, Qt::CaseSensitivity cs );

private:
    QList<QFileInfo> fileInfos;
protected:


};


#endif






