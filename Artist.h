#ifndef ARTIST_H
#define ARTIST_H


#include <QtGui>
//#include "structs.h"

class Artist{

public:
    Artist();
    QString role() const;
    QString name() const;
    void setRole( QString role );
    void setName( QString name );

private:
    QString role_;
    QString name_;

};



#endif // ARTIST_H
