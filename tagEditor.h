#ifndef TAGEDITOR_H
#define TAGEDITOR_H

#include <QtGui>
#include "TagItem.h"
#include "SearchForTagsDialog.h"
#include "globalFunctions.h"
#include "SettingsDialog.h"
#include "ReplaceDialog.h"
#include "RenameDialog.h"
#include "TextViewer.h"
#include "SearchDialog.h"
#include <tbytevector.h>
#include <mpegfile.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <apetag.h>
#include <tag.h>
#include <taglib.h>
#include <fileref.h>
#include <QtPlugin>

#include "ListWidget.h"
#include "ui_TagEditor.h"
#include "TreeWidget.h"


class TagEditor : public QMainWindow, private Ui::TagEditor{
    Q_OBJECT

public:

    TagEditor(QWidget *parent = 0);    

private slots:

    QList<QFileInfo> getDirContent( QString& aPath );
    void addFiles();
    void clearTextFields();    

    void resizeColumn();
    void removeFiles();
    void removeAllFiles();
    void rewriteTag();
    void renameFiles();
    void showTagInfo();
    void openStyleSheet();
    void setGUIStyle( const QString &s );
    void uncheckStyleActions();
    void checkStyleAction( const QString &actionText, bool state );
    bool loadStyleSheet( const QString &file );
    void saveTag();
    void serialize();
    void showSettings();
    void chooseDir();    
    void createActions();
    void searchOnline();
    void readSettings();
    void saveSettings();
    void clearTags();
    //void removeFrames();
    void replaceTags();
    void searchAndAddFiles();

    //protected:
private:
    //void closeEvent( QCloseEvent *event );

    QSettings *guiSettings;
    QString startupFolder;
    QStringList extensions;
    //QString api_key;
    bool subfolders;

    QString lastStyleSheetFolder;
    QString style;
    //QVector<tag> tags;
    bool showSaveTagWarning;
    QSize textViewerSize;

    TreeWidget *TreeWidget_;

};


#endif






