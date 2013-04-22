#include "genre.h"

Genre::Genre(QString id, QString parentId, QString title, bool restricted,
             QString creator, QString writeStatus, bool searchable,
             QList<SearchClass *>searchClasses, QList<CreateClass *>createClasses,
             QString longDescription, QString description)
{
}

QDomElement Genre::toDidlElement()
{

}
