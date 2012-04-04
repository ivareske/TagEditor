#include "ReleaseEvent.h"

ReleaseEvent::ReleaseEvent(){

}

QString ReleaseEvent::format() const{
    return format_;
}
QDate ReleaseEvent::date() const{
    return date_;
}
QString ReleaseEvent::country() const{
    return country_;
}
QStringList ReleaseEvent::labels() const{
    return labels_;
}
void ReleaseEvent::setFormat( QString format ){
    format_ = format;
}
void ReleaseEvent::setDate( QDate date ){
    date_ = date;
}
void ReleaseEvent::setCountry( QString country ){
    country_ = country;
}
void ReleaseEvent::setLabels( QStringList labels ){
    labels_ = labels;
}
