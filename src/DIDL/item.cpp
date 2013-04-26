#include "item.h"

Item::Item(QString id, QString parentId, QString title, bool restricted,
		   QString creator, QString writeStatus, QString refId)
	: DidlObject(id, parentId, title, restricted, creator, writeStatus)
{
	this->refId = refId;
}

QDomElement Item::toDidlElement()
{
	QDomElement root = DidlObject::toDidlElement();
	root.attribute("refID", this->refId);
	return root;
}
