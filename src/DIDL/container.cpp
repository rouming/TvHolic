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

void Container::addContainer(Container *c)
{
	if (!this->containers.contains(c)) {
		this->containers.append(c);
		c->setParentId(this->id);
	}
}

void Container::addItem(Item *i)
{
	if (!this->items.contains(i)) {
		this->items.append(i);
		i->setParentId(this->id);
	}
}

int Container::getChildCount()
{
	if (!this->containers.empty() || !this->items.empty())
		return this->containers.size() + this->items.size();
	return childCount;
}

QDomElement Container::toDidlElement()
{
	QDomElement root = DidlObject::toDidlElement();
	root.attribute("childCount", QString::number(this->childCount));
	foreach (SearchClass *s, this->searchClasses) {
		root.appendChild(s->getElement());
	}

	foreach(CreateClass *c, this->createClasses) {
		root.appendChild(c->getElement());
	}

	root.attribute("searchable", this->searchable ? "true" : "false");
	return root;
}

void Container::setChildCount(int c)
{
	this->childCount = c;
}

QString Container::toString()
{
	QDomDocument doc("object");
	doc.appendChild(this->toDidlElement());
	return doc.toString();
}
