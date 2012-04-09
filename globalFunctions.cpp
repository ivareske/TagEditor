#include "globalFunctions.h"


namespace Global{

    QSettings* guiSettings(QObject *parent){
        return new QSettings("TagEditor.ini",QSettings::IniFormat,parent);
    }


    QString columnText( Global::TagField t ){

        switch (t){
            case Global::FileName: return "File name";
            case Global::Artist: return "Artist";
            case Global::AlbumField: return "Album";
            case Global::Title: return "Title";
            case Global::Year: return "Year";
            case Global::Track: return "Track";
            case Global::Genre: return "Genre";
            case Global::Length: return "Length [s]";
            case Global::BitRate: return "Bitrate [kbps]";
            case Global::SampleRate: return "Sample rate [Hz]";
            case Global::Channels: return "Channels";
            case Global::Comment: return "Comments";
            default: return "";
        }
    }

    QList<QFileInfo> getDirContent(const QString& aPath, const QStringList &extensions, bool includeSubFolders, bool hiddenFiles, bool *canceled)  {

        // append the filtered files to this list

        //decide to include subfolder or not
        QDirIterator::IteratorFlag subdirFlag;
        if (includeSubFolders) {
            subdirFlag = QDirIterator::Subdirectories;
        }else {
            subdirFlag = QDirIterator::NoIteratorFlags;
        }
        QDir::Filters flags = QDir::Files | QDir::NoDotAndDotDot;
        if (hiddenFiles) {
            flags = flags | QDir::Hidden;
        }

        // set dir iterator
        QDirIterator dirIterator(aPath,extensions,flags,subdirFlag);


        qDebug()<<"Locating files in "<<aPath;
        QList<QFileInfo> fileInfo;
        QProgressDialog p("Locating files...", "Abort", 0, 0, 0);
        p.setWindowModality(Qt::WindowModal);
        int k=0; int tot=0; int lim=100; bool first=true;
        while (dirIterator.hasNext()) {
            if(p.wasCanceled()){
                if(canceled!=0){
                    *canceled=true;
                }
                return fileInfo;
            }
            dirIterator.next();
            fileInfo.append(dirIterator.fileInfo());
            if(k==lim){
                k=0;
                if(first){
                    std::cout<<"Files found: ";
                    first=false;
                }
                std::cout<<tot<<" ";
            }
            tot++;k++;
        }
        p.close();
        qDebug() << "Searched "<<aPath<<", found "<<fileInfo.size()<<" files.";
        return fileInfo;
    }


    int naturalCompare( const QString &_a, const QString &_b, Qt::CaseSensitivity caseSensitivity ){
        // This method chops the input a and b into pieces of
        // digits and non-digits (a1.05 becomes a | 1 | . | 05)
        // and compares these pieces of a and b to each other
        // (first with first, second with second, ...).
        //
        // This is based on the natural sort order code code by Martin Pool
        // http://sourcefrog.net/projects/natsort/
        // Martin Pool agreed to license this under LGPL or GPL.

        // FIXME: Using toLower() to implement case insensitive comparison is
        // sub-optimal, but is needed because we compare strings with
        // localeAwareCompare(), which does not know about case sensitivity.
        // A task has been filled for this in Qt Task Tracker with ID 205990.
        // http://trolltech.com/developer/task-tracker/index_html?method=entry&id=205990
        QString a;
        QString b;
        if (caseSensitivity == Qt::CaseSensitive) {
            a = _a;
            b = _b;
        } else {
            a = _a.toLower();
            b = _b.toLower();
        }

        const QChar* currA = a.unicode(); // iterator over a
        const QChar* currB = b.unicode(); // iterator over b

        if (currA == currB) {
            return 0;
        }

        int lean = 0; // comparison of first ignored difference, to be used if all else is equal

        while (!currA->isNull() && !currB->isNull()) {
            const QChar* begSeqA = currA; // beginning of a new character sequence of a
            const QChar* begSeqB = currB;
            if (currA->unicode() == QChar::ObjectReplacementCharacter) {
                return 1;
            }

            if (currB->unicode() == QChar::ObjectReplacementCharacter) {
                return -1;
            }

            if (currA->unicode() == QChar::ReplacementCharacter) {
                return 1;
            }

            if (currB->unicode() == QChar::ReplacementCharacter) {
                return -1;
            }

            // find sequence of characters ending at the first non-character
            while (!currA->isNull() && !currA->isDigit() && !currA->isPunct() && !currA->isSpace()) {
                ++currA;
            }

            while (!currB->isNull() && !currB->isDigit() && !currB->isPunct() && !currB->isSpace()) {
                ++currB;
            }

            // compare these sequences
            const QStringRef& subA(a.midRef(begSeqA - a.unicode(), currA - begSeqA));
            const QStringRef& subB(b.midRef(begSeqB - b.unicode(), currB - begSeqB));
            const int cmp = QStringRef::localeAwareCompare(subA, subB);
            if (cmp != 0) {
                return cmp < 0 ? -1 : +1;
            }

            if (currA->isNull() || currB->isNull()) {
                break;
            }

            // find sequence of characters ending at the first non-character
            while ((currA->isPunct() || currA->isSpace()) && (currB->isPunct() || currB->isSpace())) {
                if (*currA != *currB) {
                    // accept difference in punctuation for now ('_' or '.' used instead of ' ').
                    if (lean == 0)
                        lean = (*currA < *currB) ? -1 : +1;
                }
                ++currA;
                ++currB;
            }

            // now some digits follow...
            if ((*currA == '0') || (*currB == '0')) {
                // one digit-sequence starts with 0 -> assume we are in a fraction part
                // do left aligned comparison (numbers are considered left aligned)
                while (1) {
                    if (!currA->isDigit() && !currB->isDigit()) {
                        break;
                    } else if (!currA->isDigit()) {
                        return +1;
                    } else if (!currB->isDigit()) {
                        return -1;
                    } else if (*currA < *currB) {
                        return -1;
                    } else if (*currA > *currB) {
                        return + 1;
                    }
                    ++currA;
                    ++currB;
                }
            } else {
                // No digit-sequence starts with 0 -> assume we are looking at some integer
                // do right aligned comparison.
                //
                // The longest run of digits wins. That aside, the greatest
                // value wins, but we can't know that it will until we've scanned
                // both numbers to know that they have the same magnitude.

                bool isFirstRun = true;
                int weight = 0;
                while (1) {
                    if (!currA->isDigit() && !currB->isDigit()) {
                        if (weight != 0) {
                            return weight;
                        }
                        break;
                    } else if (!currA->isDigit()) {
                        if (isFirstRun) {
                            return *currA < *currB ? -1 : +1;
                        } else {
                            return -1;
                        }
                    } else if (!currB->isDigit()) {
                        if (isFirstRun) {
                            return *currA < *currB ? -1 : +1;
                        } else {
                            return +1;
                        }
                    } else if ((*currA < *currB) && (weight == 0)) {
                        weight = -1;
                    } else if ((*currA > *currB) && (weight == 0)) {
                        weight = + 1;
                    }
                    ++currA;
                    ++currB;
                    isFirstRun = false;
                }
            }
            continue;
        }

        if (currA->isNull() && currB->isNull()) {
            return lean;
        }

        return currA->isNull() ? -1 : + 1;
    }


}
