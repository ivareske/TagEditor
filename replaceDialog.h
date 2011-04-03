#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QtGui>
#include "TextViewer.h"
#include "TagItem.h"
#include "ui_replaceDialog.h"


class ReplaceDialog : public QDialog, private Ui::ReplaceDialog
{
    Q_OBJECT

public:

    ReplaceDialog( const QList<TagItem*> &items, QWidget *parent = 0 );

public slots:
    void replace();
    void finito( int result );


private:
    QList<TagItem*> items_;
    QSettings *settings;
protected:


};


#endif






