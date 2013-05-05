#ifndef GENRE_H
#define GENRE_H

#include "container.h"

class Genre : public Container
{
public:
	Genre(QString id = "", QString parentId = "", QString title = "", bool restricted = false,
		  QString creator = "", QString writeStatus = WRITE_STATUS_NOT_WRITABLE,
		  bool searchable = true, QList<SearchClass*> searchClasses = QList<SearchClass*>(),
		  QList<CreateClass*> createClasses = QList<CreateClass*>(),
		  QString longDescription = "", QString description = "");

	QDomElement toDidlElement(QDomDocument);

protected:
	QString longDescription;
	QString description;

};

#endif // GENRE_H
