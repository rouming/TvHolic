#include "genre.h"

Genre::Genre(QString id, QString parentId, QString title, bool restricted,
			 QString creator, QString writeStatus, bool searchable,
			 QList<SearchClass *>searchClasses, QList<CreateClass *>createClasses,
			 QString longDescription, QString description)
{
	(void)id;
	(void)parentId;
	(void)title;
	(void)restricted;
	(void)creator;
	(void)writeStatus;
	(void)searchable;
	(void)searchClasses;
	(void)createClasses;
	(void)longDescription;
	(void)description;
}

QDomElement Genre::toDidlElement(QDomDocument&)
{
	return QDomElement();
}
