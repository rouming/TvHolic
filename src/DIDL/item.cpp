#include "item.h"

Item::Item(QString id, QString parentId, QString title, bool restricted,
		   QString creator, QString writeStatus, QString refId)
	: DidlObject(id, parentId, title, restricted, creator, writeStatus)
{
	this->elementName = "item";
	this->upnpClass = DidlObject::getUpnpClass() + ".item";
	this->refId = refId;
}

QDomElement Item::toDidlElement(QDomDocument doc)
{
	QDomElement root = DidlObject::toDidlElement(doc);
	root.setAttribute("refID", this->refId);
	return root;
}
