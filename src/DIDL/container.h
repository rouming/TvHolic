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

	QDomElement toDidlElement();
	QString toString();
	void addItem(Item *i);
	void addContainer(Container *c);
	inline QList<Container*> getContainers() {
		return this->containers;
	}
	inline QList<Item*> getItems() {
		return this->items;
	}
	int getChildCount();
	void setChildCount(int c);

private:
	// Attributes
	bool searchable;
	QList<SearchClass*> searchClasses;
	QList<CreateClass*> createClasses;
	QList<Container*> containers;
	QList<Item*> items;
	int childCount;

	// Methods
};

#endif // CONTAINER_H
