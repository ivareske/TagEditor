#include "Song.h"

Song::Song(){

}

QString Song::track() const {
    return track_;
}

QString Song::title() const {
    return title_;
}

QString Song::artist() const {
    return artist_;
}

QString Song::id() const {
    return id_;
}

int Song::length() const {
    return length_;
}

void Song::setTrack( QString track ) {
    track_ = track;
}

void Song::setTitle( QString title ) {
    title_ = title;
}

void Song::setArtist( QString artist ) {
    artist_ = artist;
}

void Song::setId( QString id ) {
    id_ = id;
}

void Song::setLength( int length ) {
    length_ = length;
}

