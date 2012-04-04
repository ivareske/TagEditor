#include "TableWidgetItem.h"

TableWidgetItem::TableWidgetItem( int type ) : QTableWidgetItem( type ){

}

bool TableWidgetItem::operator< ( const QTableWidgetItem &other ) const{
	
        int res = Global::naturalCompare( this->text().toLower(), other.text().toLower(), Qt::CaseInsensitive );
	return res<0;

}

