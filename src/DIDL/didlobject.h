#ifndef DIDLOBJECT_H
#define DIDLOBJECT_H

#include <QDomDocument>
#include <QDomElement>
#include <QString>
#include <QList>

#include "resource.h"


#define WRITE_STATUS_NOT_WRITABLE "NOT_WRITABLE"
#define WRITE_STATUS_WRITABLE "WRITABLE"
#define WRITE_STATUS_PROTECTED "PROTECTED"
#define WRITE_STATUS_UNKNOWN "UNKNOWN"
#define WRITE_STATUS_MIXED "MIXED"

class DidlObject
{
public:
	DidlObject(QString id = "",  QString parentId = "", QString title = "", bool restricted = false,
			   QString creator = "", QString writeStatus = WRITE_STATUS_NOT_WRITABLE);

	virtual ~DidlObject();
	void addResource(Resource *resource);
	QString toString(QDomDocument&);
	virtual QDomElement toDidlElement(QDomDocument&);
	inline QString getElementName() {
		return elementName;
	}
	inline QString getUpnpClass() {
		return upnpClass;
	}
	inline QString getId() {
		return id;
	}
	inline QString getParentId() {
		return parentId;
	}
	inline void setParentId(QString parentId) {
		this->parentId = parentId;
	}
	inline QString getTitle() {
		return title;
	}
	inline QString getCreator() {
		return creator;
	}
	inline QString getWriteStatus() {
		return writeStatus;
	}
	inline bool isRestricted() {
		return restricted;
	}
	inline QList<Resource*> getResourceList() {
		return resources;
	}

private:
	DidlObject(const DidlObject&);
	DidlObject& operator= (const DidlObject&);

protected:
	QString elementName;
	QString upnpClass;
	QList<Resource*> resources;
	QString id;
	QString parentId;
	QString title;
	QString creator;
	QString writeStatus;
	bool restricted;
};

#endif // DIDLOBJECT_H
