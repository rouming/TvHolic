#ifndef AUDIOITEM_H
#define AUDIOITEM_H

#include <QStringList>
#include "item.h"
#include "genre.h"


class AudioItem : public Item
{
public:
    AudioItem(QString id = "", QString parentId = "", QString title = "", bool restricted = false,
              QString creator = "", QString writeStatus = WRITE_STATUS_NOT_WRITABLE,
              QString refId = "", QStringList genres = QStringList(),
              QString description = "", QString longDescription = "", QStringList publishers = QStringList(),
              QString language = "", QStringList relations = QStringList(), QStringList rights = QStringList());
    
    QDomElement toDidlElement();
    
protected:
    QList<Genre*> genres;
    QString description;
    QString longDescription;
    QStringList publishers;
    QString language;
    QStringList relations;
    QStringList rights;
};

#endif // AUDIOITEM_H
