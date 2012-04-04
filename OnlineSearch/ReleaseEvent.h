#ifndef RELEASEEVENT_H
#define RELEASEEVENT_H


#include <QtGui>

class ReleaseEvent{

public:
    ReleaseEvent();
    QString format() const;
    QDate date() const;
    QString country() const;
    QStringList labels() const;
    void setFormat( QString format );
    void setDate( QDate date );
    void setCountry( QString country );
    void setLabels( QStringList labels );


private:
    QString format_;
    QDate date_;
    QString country_;
    QStringList labels_;

};


#endif // RELEASEEVENT_H
