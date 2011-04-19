#find lyrics?
#build qt static: configure -static -no-exceptions; make sub-src
#http://www.techsupportalert.com/best-free-mp3-tag-editor.htm
#http://www.discogs.com/help/api
#some help documentation, link to donation?
#encrypt api_key in settings
#bytt ut & test keyboard shortcuts
#ensure only one tag type in file? not both id3v2 & id3v1 for instance
#search for specific tags, replace in tags
#id3v2 & ape frames/items
#redistribute original files in SearchDialog?
#amazon cover art. access key: AKIAJS6T6TP3SZVTPNKQ
#http://docs.amazonwebservices.com/AlexaWebInfoService/1-0/RequestAuthentication.html
#http://weblog.plexobject.com/?p=1555
#xml lyrics: http://lyrics.wikia.com/LyricWiki:REST. store in SYLT frame in id3v2. fpr mulignes ikke hele teksten pga. rettigheter
#xml sider: musicbrainz: http://musicbrainz.org/doc/XML_Web_Service#The_URL_Schema
#get result: http://musicbrainz.org/ws/1/release/02232360-337e-4a3f-ad20-6cdd4c34288c?type=xml&inc=tracks+artist
#search: http://musicbrainz.org/ws/1/release/?type=xml&artist=bad+religion&title=generator
#search for individual tracks possible in musicbrainz
#last.fm
#http://blog.programmableweb.com/2008/02/21/25-music-apis/
#for musicbrainz (or all), add info when loading album. in musicbrainz one gets some information when searching, the rest when downloading the album: FÅ QNETWORKREQUEST ATTRIBUTE TIL Å FUNKE: ble introdusert i 4.7
#hide/show additional album info fields
#option on how to show files in list? artist - album - year etc.
#stream operators, cleanup/move files
#legg til taglib kompilerings instruksjoner + debuglogger
#prøve å kompilere taglib med qt? pro file ligge ved
#rename klasser til å starte med stor bokstav

TEMPLATE = app
TARGET = TagEditor

CONFIG += qt release warn_on
CONFIG += console silent
QT += network webkit
QT += xml
#QTPLUGIN += qjpeg


INCLUDEPATH  += $$quote(C:/Program Files/taglib/include/taglib/)
LIBS += $$quote(C:/Program Files/taglib/bin/libtag.dll)


HEADERS  = TagEditor.h SettingsDialog.h SettingsClass.h SearchDialog.h Album.h TextViewer.h globalFunctions.h TagItem.h TableWidgetItem.h MusicDatabase.h Discogs.h MusicBrainz.h ReplaceDialog.h RenameDialog.h SearchForTagsDialog.h \
    ReleaseEvent.h \
    Song.h \
    Artist.h \
    TreeWidget.h SetColumnsDialog.h \
    TableWidget.h

FORMS     = TagEditor.ui SettingsDialog.ui SearchDialog.ui TextViewer.ui ReplaceDialog.ui RenameDialog.ui SearchForTagsDialog.ui SetColumnsDialog.ui

SOURCES = main.cpp TagEditor.cpp SettingsDialog.cpp SettingsClass.cpp SearchDialog.cpp Album.cpp TextViewer.cpp globalFunctions.cpp TagItem.cpp TableWidgetItem.cpp MusicDatabase.cpp Discogs.cpp MusicBrainz.cpp ReplaceDialog.cpp RenameDialog.cpp SearchForTagsDialog.cpp \
    ReleaseEvent.cpp \
    Song.cpp \
    Artist.cpp \
    TreeWidget.cpp SetColumnsDialog.cpp \
    TableWidget.cpp

RC_FILE = TagEditor.rc
RESOURCES += TagEditor.qrc


#fant ikke:
#afi a history
#
#
#kommet til: bad religion 80-85, fix opp
