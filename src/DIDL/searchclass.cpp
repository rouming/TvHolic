#include "searchclass.h"

SearchClass::SearchClass(QString className, bool includeDerived, QString classFriendlyName)
{
	this->className = className;
	this->includeDerived = includeDerived;
	this->classFriendlyName = classFriendlyName;
}

QDomElement SearchClass::getElement()
{
	QDomDocument doc;
	QDomElement elt = doc.createElement("upnp:seachClass");
	elt.attribute("includeDerived", this->includeDerived ? "true" : "false");

	if (!this->classFriendlyName.isEmpty()) {
		elt.attribute("name", this->classFriendlyName);
	}
	QDomText text = doc.createTextNode(this->className);
	elt.appendChild(text);
	return elt;
}
