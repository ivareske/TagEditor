#ifndef SEARCHFORTAGSDIALOG_H
#define SEARCHFORTAGSDIALOG_H

#include <QtGui>
#include "TextViewer.h"
#include "TagItem.h"
#include "ui_SearchForTagsDialog.h"

class SearchForTagsDialog : public QDialog, private Ui::SearchForTagsDialog{
    Q_OBJECT

public:

    SearchForTagsDialog( QList<QFileInfo> files, QWidget *parent = 0 );

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






