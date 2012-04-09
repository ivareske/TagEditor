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
#include <QtGui>
#include <QtScript>
//#include "ListWidget.h"
#include "TreeWidget.h"
#include "ui_tagEditor.h"
#include "Script.h"
#include "metaTypes.h"
#include "codeeditor.h"

class TagEditor : public QMainWindow, private Ui::TagEditor{
    Q_OBJECT

public:

    TagEditor(QWidget *parent = 0);

public slots:
    bool runScript(const QString &script="", bool guiMode=true);

private slots:

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
    void checkStyleAction( QString actionText, bool state );
    bool loadStyleSheet( QString file );
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
    void loadScript();
    void saveScript();

    //protected:
private:
    void closeEvent( QCloseEvent *event );

    QSettings *guiSettings;
    QString startupFolder;
    QStringList extensions;
    //QString api_key;
    bool subfolders;

    QString lastStyleSheetFolder;
    QString style;
    //QVector<tag> tags;
    bool showSaveTagWarning;   
    TreeWidget *TreeWidget_;
    QScriptEngine engine_;
    CodeEditor *scriptEditor_;


};


#endif






