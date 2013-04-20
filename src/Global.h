#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <QHostAddress>

namespace Global
{
	QString getUsn();
	QString getDescriptionXmlUrl();
	QHostAddress getDefaultIPv4LocalAddr();
}

#endif //__GLOBAL_H__
