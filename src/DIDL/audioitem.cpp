#include "audioitem.h"

AudioItem::AudioItem(QString id, QString parentId, QString title, bool restricted, QString creator,
					 QString writeStatus, QString refId, QStringList genres,
					 QString description, QString longDescription, QStringList publishers,
					 QString language, QStringList relations, QStringList rights)
{
	(void)id;
	(void)parentId;
	(void)title;
	(void)restricted;
	(void)creator;
	(void)writeStatus;
	(void)refId;
	(void)genres;
	(void)description;
	(void)longDescription;
	(void)publishers;
	(void)language;
	(void)relations;
	(void)rights;
}

QDomElement AudioItem::toDidlElement(QDomDocument)
{
	return QDomElement();
}
