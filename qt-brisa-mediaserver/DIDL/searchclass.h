#ifndef SEARCHCLASS_H
#define SEARCHCLASS_H

#include <QString>
#include <QBool>
#include <QDomElement>

class SearchClass
{
public:
    SearchClass(QString className, bool includeDerived = false,
                QString classFriendlyName = "");
    QDomElement getElement();

private:
    QString className;
    QString classFriendlyName;
    bool includeDerived;
};

#endif // SEARCHCLASS_H
