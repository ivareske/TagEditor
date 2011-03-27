#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QtGui>
#include "textViewer.h"
#include "TagItem.h"
#include "ui_replaceDialog.h"


class replaceDialog : public QDialog, private Ui::replaceDialog
{
    Q_OBJECT

public:

    replaceDialog( const QList<TagItem*> &items, QWidget *parent = 0 );

public slots:
    void replace();
    void finito( int result );


private:
    QList<TagItem*> items_;
    QSettings *settings;
protected:


};


#endif






