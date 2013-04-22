#include "createclass.h"

CreateClass::CreateClass(QString className, bool includeDerived,
                         QString classFriendlyName)
                             : SearchClass(className, includeDerived,
                                           classFriendlyName)
{
}

QDomElement CreateClass::getElement()
{
    return SearchClass::getElement();
}
