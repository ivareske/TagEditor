#include "globalFunctions.h"


namespace Global{
/*

    bool compareName( TagItem *s1, TagItem *s2 ){
        int res = naturalCompare( s1->fileInfo().fileName().toLower(), s2->fileInfo().fileName().toLower(), Qt::CaseInsensitive );
        return  res<0 ;
    }
    bool compareArtistTag( TagItem *s1, TagItem *s2 ){
        QString comp1 = s1->getTag(TagItem::Artist).toString().toLower();
        QString comp2 = s2->getTag(TagItem::Artist).toString().toLower();
        int res = naturalCompare( comp1, comp2, Qt::CaseInsensitive );
        return res<0;
    }
    bool compareAlbumTag( TagItem *s1, TagItem *s2 ){
        QString comp1 = s1->getTag(TagItem::Album).toString().toLower();
        QString comp2 = s2->getTag(TagItem::Album).toString().toLower();
        int res = naturalCompare( comp1, comp2, Qt::CaseInsensitive );
        return res<0;
    }
    bool compareTitleTag( TagItem *s1, TagItem *s2 ){
        QString comp1 = s1->getTag(TagItem::Title).toString().toLower();
        QString comp2 = s2->getTag(TagItem::Title).toString().toLower();
        int res = naturalCompare( comp1, comp2, Qt::CaseInsensitive );
        return res<0;
    }
    bool compareTrackTag( TagItem *s1, TagItem *s2 ){
        int comp1 = s1->getTag(TagItem::Track).toInt();
        int comp2 = s2->getTag(TagItem::Track).toInt();
        return comp1 < comp2;
    }
    bool compareYearTag( TagItem *s1, TagItem *s2 ){
        int comp1 = s1->getTag(TagItem::Year).toInt();
        int comp2 = s2->getTag(TagItem::Year).toInt();
        return comp1 < comp2;
    }
    bool compareGenreTag( TagItem *s1, TagItem *s2 ){
        QString comp1 = s1->getTag(TagItem::Genre).toString().toLower();
        QString comp2 = s2->getTag(TagItem::Genre).toString().toLower();
        int res = naturalCompare( comp1, comp2, Qt::CaseInsensitive );
        return res<0;
    }
    bool compareCommentTag( TagItem *s1, TagItem *s2 ){
        QString comp1 = s1->getTag(TagItem::Comment).toString().toLower();
        QString comp2 = s2->getTag(TagItem::Comment).toString().toLower();
        int res = naturalCompare( comp1, comp2, Qt::CaseInsensitive );
        return res<0;
    }
*/
    /*
bool compareResult( SearchResult s1, SearchResult s2 ){
        int res = naturalCompare( s1.format, s2.format, Qt::CaseInsensitive );
        if(res==0){
                return s1.year < s2.year;
        }else{
                return res<0;
        }
}
*/

    bool compareAlbum( const Album &a1, const Album &a2 ){
        int res = naturalCompare( a1.format(), a2.format(), Qt::CaseInsensitive );
        if(res==0){
            return a1.year() < a2.year();
        }else{
            return res<0;
        }

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
