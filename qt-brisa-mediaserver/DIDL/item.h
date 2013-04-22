#ifndef ITEM_H
#define ITEM_H

#include "didlobject.h"

class Item : public DidlObject
{
public:
    Item(QString id = "", QString parentId = "", QString title = "",
         bool restricted = false, QString creator = "",
         QString writeStatus = WRITE_STATUS_NOT_WRITABLE, QString refId = "");

    QDomElement toDidlElement();

protected:
    QString refId;
};

#endif // ITEM_H
