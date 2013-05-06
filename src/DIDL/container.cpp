#include "container.h"

#include <QDebug>

Container::Container(QString id, QString parentId, QString title,
					 bool restricted, QString creator, QString writeStatus,
					 bool searchable, QList<SearchClass *> searchClasses,
					 QList<CreateClass *> createClasses)
	: DidlObject(id, parentId, title, restricted,
				 creator, writeStatus)
{
	this->elementName = "container";
	this->upnpClass = DidlObject::getUpnpClass() + ".container";
	this->createClasses = createClasses;
	this->searchClasses = searchClasses;
	this->searchable = searchable;
}

Container::~Container()
{
	qDeleteAll(this->children);
}

void Container::addChild(DidlObject *c)
{
	if (!this->children.contains(c)) {
		this->children.append(c);
		c->setParentId(this->id);
	}
}

int Container::getChildCount()
{
	return this->children.size();
}

QDomElement Container::toDidlElement(QDomDocument& doc)
{
	QDomElement root = DidlObject::toDidlElement(doc);
	root.setAttribute("childCount", QString::number(getChildCount()));

	foreach (SearchClass *s, this->searchClasses) {
		root.appendChild(s->getElement());
	}

	foreach(CreateClass *c, this->createClasses) {
		root.appendChild(c->getElement());
	}

	root.setAttribute("searchable", this->searchable ? "true" : "false");
	return root;
}
