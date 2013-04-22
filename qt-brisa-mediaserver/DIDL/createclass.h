#ifndef CREATECLASS_H
#define CREATECLASS_H

#include "searchclass.h"

class CreateClass : public SearchClass
{
public:
    CreateClass(QString className, bool includeDerived = false, QString classFriendlyName = "");
    QDomElement getElement();
};

#endif // CREATECLASS_H
