#ifndef CONTAINER_H
#define CONTAINER_H

#include "didlobject.h"
#include "item.h"
#include "searchclass.h"
#include "createclass.h"

class Container : public DidlObject
{
public:
	Container(QString id = "", QString parentId = "", QString title = "",
			  bool restricted = false, QString creator = "",
			  QString writeStatus = WRITE_STATUS_NOT_WRITABLE, bool searchable = true,
			  QList<SearchClass*> searchClasses = QList<SearchClass*>(),
			  QList<CreateClass*> createClasses = QList<CreateClass*>());
	~Container();

	virtual QDomElement toDidlElement(QDomDocument&);
	void addChild(DidlObject *c);
	inline const QList<DidlObject*> getChildren() {
		return this->children;
	}
	int getChildCount();
	void setChildCount(int cnt);

private:
	// Attributes
	bool searchable;
	QList<SearchClass*> searchClasses;
	QList<CreateClass*> createClasses;
	QList<DidlObject*> children;
	int childrenCnt;

};

#endif // CONTAINER_H
