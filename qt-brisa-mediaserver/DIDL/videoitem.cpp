#include "videoitem.h"

VideoItem::VideoItem(QString id, QString parentId, QString title, bool restricted,
                     QString creator, QString writeStatus, QString refId,
                     QStringList genres, QString longDescription,
                     QStringList producers, QString rating, QStringList actors,
                     QStringList directors, QString description,
                     QStringList publishers, QString language, QStringList relations)
                         : Item(id, parentId, title, restricted, creator,
                                writeStatus, refId)
{
    this->genres = genres;
    this->longDescription = longDescription;
    this->producers = producers;
    this->rating = rating;
    this->actors = actors;
    this->directors = directors;
    this->description = description;
    this->publishers = publishers;
    this->language = language;
    this->relations = relations;
}

QDomElement VideoItem::toDidlElement()
{
    QDomElement root = Item::toDidlElement();
    QDomDocument doc;
    if (!this->longDescription.isEmpty()) {
        QDomElement elt = doc.createElement("upnp:longDescription");
        elt.appendChild(doc.createTextNode(this->longDescription));
        root.appendChild(elt);
    }
    if (!this->rating.isEmpty()) {
        QDomElement elt = doc.createElement("upnp:rating");
        elt.appendChild(doc.createTextNode(this->rating));
        root.appendChild(elt);
    }
    foreach (QString g, this->genres) {
        QDomElement elt = doc.createElement("upnp:genre");
        elt.appendChild(doc.createTextNode(g));
        root.appendChild(elt);
    }
    foreach (QString p, this->producers) {
        QDomElement elt = doc.createElement("upnp:producer");
        elt.appendChild(doc.createTextNode(p));
        root.appendChild(elt);
    }
    foreach (QString a, this->actors) {
        QDomElement elt = doc.createElement("upnp:actor");
        elt.appendChild(doc.createTextNode(a));
        root.appendChild(elt);
    }
    foreach (QString d, this->directors) {
        QDomElement elt = doc.createElement("upnp:director");
        elt.appendChild(doc.createTextNode(d));
        root.appendChild(elt);
    }
    foreach (QString p, this->publishers) {
        QDomElement elt = doc.createElement("upnp:publisher");
        elt.appendChild(doc.createTextNode(p));
        root.appendChild(elt);
    }
    foreach (QString g, this->publishers) {
        QDomElement elt = doc.createElement("upnp:publisher");
        elt.appendChild(doc.createTextNode(g));
        root.appendChild(elt);
    }
    if (!this->description.isEmpty()) {
        QDomElement elt = doc.createElement("upnp:description");
        elt.appendChild(doc.createTextNode(this->description));
        root.appendChild(elt);
    }
    if (!this->language.isEmpty()) {
        QDomElement elt = doc.createElement("upnp:language");
        elt.appendChild(doc.createTextNode(this->language));
        root.appendChild(elt);
    }

    return root;
}

QString VideoItem::toString()
{
    QDomDocument doc;
    doc.appendChild(this->toDidlElement());
    return doc.toString();
}
