#include "Artist.h"

Artist::Artist(){

}

QString Artist::role() const {
    return role_;
}

QString Artist::name() const {
    return name_;
}

void Artist::setRole( QString role ) {
    role_ = role;
}

void Artist::setName( QString name ) {
    name_ = name;
}

