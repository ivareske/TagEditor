#ifndef TABLEWIDGETITEM_H
#define TABLEWIDGETITEM_H
 
#include <QtGui>
#include "globalFunctions.h"


class TableWidgetItem : public QTableWidgetItem{


	public:
		TableWidgetItem( int type = 0); 	
		bool operator< ( const QTableWidgetItem & other ) const;
		
	private:		

};

#endif