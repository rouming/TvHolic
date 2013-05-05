#include "didlobject.h"

#include <QDebug>

DidlObject::DidlObject(QString id, QString parentId,
					   QString title, bool restricted, QString creator,
					   QString writeStatus)
{
	(void)creator;
	this->elementName = "object";
	this->upnpClass = "object";
	this->id = id;
	this->parentId = parentId;
	this->title = title;
	this->restricted = restricted;
	this->writeStatus = writeStatus;
}

DidlObject::~DidlObject()
{
	qDeleteAll(this->resources);
	this->resources.clear();
}

void DidlObject::addResource(Resource *resource)
{
	if (!resources.contains(resource))
		this->resources.append(resource);
}

QDomElement DidlObject::toDidlElement(QDomDocument& doc)
{
	QDomElement root = doc.createElement(this->elementName);
	root.setAttribute("id", this->id);
	root.setAttribute("parentID", this->parentId);
	QDomElement title = doc.createElement("dc:title");
	title.appendChild(doc.createTextNode(this->title));
	root.appendChild(title);
	QDomElement classElement = doc.createElement("upnp:class");
	classElement.appendChild(doc.createTextNode(this->upnpClass));
	root.appendChild(classElement);

	if (this->restricted)
		root.setAttribute("restricted", "true");
	else
		root.setAttribute("restricted", "false");

	if (!this->creator.isEmpty()) {
		QDomElement creatorElement = doc.createElement("dc:creator");
		creatorElement.appendChild(doc.createTextNode(this->creator));
		root.appendChild(creatorElement);
	}
	if (!this->writeStatus.isEmpty()) {
		QDomElement creatorElement = doc.createElement("upnp:writeStatus");
		creatorElement.appendChild(doc.createTextNode(this->writeStatus));
		root.appendChild(creatorElement);
	}

	foreach (Resource *r, this->resources) {
		root.appendChild(r->toDidlElement(doc));
	}

	return root;
}

QString DidlObject::toString(QDomDocument& doc)
{
	QDomElement root = this->toDidlElement(doc);
	doc.appendChild(root);
	return doc.toString();
}
