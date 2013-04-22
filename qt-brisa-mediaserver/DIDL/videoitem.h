#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include "item.h"
#include "genre.h"

#include <QStringList>

class VideoItem : public Item
{
public:
    VideoItem(QString id = "", QString parentId = "",QString title = "", bool restricted = false,
              QString creator = "", QString writeStatus = WRITE_STATUS_NOT_WRITABLE,
              QString refId = "", QStringList genres = QStringList(),
              QString longDescription = "", QStringList producers = QStringList(),
              QString rating = "", QStringList actors = QStringList(),
              QStringList directors = QStringList(), QString description = "",
              QStringList publishers = QStringList(), QString language = "",
              QStringList relations = QStringList());

    QDomElement toDidlElement();
    QString toString();

protected:
    QString longDescription;
    QString description;
    QString language;
    QString rating;
    QStringList genres;
    QStringList producers;
    QStringList actors;
    QStringList directors;
    QStringList publishers;
    QStringList relations;
};

#endif // VIDEOITEM_H
