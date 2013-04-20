#include <QUuid>
#include <QNetworkInterface>

#include "Global.h"

QString Global::getUsn()
{
	static QString s_usn;
	if (s_usn.isEmpty()) {
		s_usn = QUuid::createUuid().toString();
		s_usn.remove(0, 1).chop(1);
		s_usn.prepend("uuid:");
	}
	return s_usn;
}

QString Global::getDescriptionXmlUrl()
{
	static QString s_url = "description.xml";
	return s_url;
}

QHostAddress Global::getDefaultIPv4LocalAddr()
{
	static QHostAddress s_addr;

	if (s_addr.isNull()) {
		QList<QNetworkInterface> nifs = QNetworkInterface::allInterfaces();
		foreach(QNetworkInterface nif, nifs) {
			QNetworkInterface::InterfaceFlags flags = nif.flags();
			if (!(flags & QNetworkInterface::IsUp &&
				  flags & QNetworkInterface::IsRunning &&
				  flags & QNetworkInterface::CanMulticast))
				continue;

			QList<QNetworkAddressEntry> aes = nif.addressEntries();
			foreach(QNetworkAddressEntry ae, aes) {
				QHostAddress addr = ae.ip();
				if (addr.protocol() != QAbstractSocket::IPv4Protocol)
					continue;
				s_addr = addr;
				goto out;
			}
		}
	}

out:
	return s_addr;
}
