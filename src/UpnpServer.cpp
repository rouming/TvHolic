#include <QDateTime>

#include "UpnpServer.h"
#include "Global.h"

const static QString SSDP_ALIVE  = "ssdp:alive";
const static QString SSDP_BYEBYE = "ssdp:byebye";

UpnpServer::UpnpServer() :
	m_tcpPort(0)
{
	bool res;
	res = QObject::connect(&m_udpSocket,
						   SIGNAL(readyRead()), SLOT(onReadyRead()));
	(void)res;
	Q_ASSERT(res);

	res = QObject::connect(&m_aliveTimer,
						   SIGNAL(timeout()), SLOT(onTimeout()));
	(void)res;
	Q_ASSERT(res);
}

UpnpServer::~UpnpServer()
{
	stop();
}

bool UpnpServer::init(quint16 tcpPort)
{
	bool res;
	res = m_udpSocket.bind(UPNP_PORT,
						   QUdpSocket::ShareAddress |
						   QUdpSocket::ReuseAddressHint);
	if (!res) {
		qWarning("Error: can't bind udp socket to %u", UPNP_PORT);
		goto err;
	}

	res = m_udpSocket.joinMulticastGroup(QHostAddress(UPNP_GROUP));
	if (!res) {
		qWarning("Error: can't join udp socket to multicast group");
		goto err;
	}

	m_aliveTimer.start(10000);
	m_tcpPort = tcpPort;

	return true;

err:
	m_udpSocket.close();
	return false;
}

void UpnpServer::stop()
{
	sendSsdpByeBye();

	m_aliveTimer.stop();
	m_udpSocket.close();
	m_tcpPort = 0;
}

void processDatagram(const QHostAddress& sender, quint16 port,
							const QByteArray& datagram, quint32 sz)
{
	(void)datagram;

	printf("\nsender %s, port %u, datagram sz %u\n",
		   qPrintable(sender.toString()),
		   port,
		   sz);
	printf("-------------------------------\n");
	printf("%.*s", sz,  (const char*)datagram.constData());
	printf("-------------------------------\n");
}


void UpnpServer::onReadyRead()
{
	while (m_udpSocket.hasPendingDatagrams()) {
		QHostAddress remoteAddr;
		quint16 remotePort;
		qint64 sz = m_udpSocket.pendingDatagramSize();
		qint64 rd;
		QByteArray ba;
		Q_ASSERT(sz > 0);

		if (m_datagram.size() < sz)
			m_datagram.resize(sz);

		rd = m_udpSocket.readDatagram(m_datagram.data(), m_datagram.size(),
									  &remoteAddr, &remotePort);
		if (rd != sz) {
			qFatal("Error: it is impossible! rd %lld != sz %lld", rd, sz);
			return;
		}

		ba = QByteArray::fromRawData(m_datagram.constData(), rd);

		//XXX
#if 0
		processDatagram(remoteAddr, remotePort,
						 ba, (quint32)rd);
#endif

		if (!ba.startsWith("M-SEARCH"))
			continue;

		if (ba.indexOf("urn:schemas-upnp-org:service:ContentDirectory:1") > 0)
			sendSsdpDiscover(remoteAddr, remotePort,
							 "urn:schemas-upnp-org:service:ContentDirectory:1");
		else if (ba.indexOf("upnp:rootdevice") > 0)
			sendSsdpDiscover(remoteAddr, remotePort,
							 "upnp:rootdevice");
		else if (ba.indexOf("urn:schemas-upnp-org:device:MediaServer:1") > 0)
			sendSsdpDiscover(remoteAddr, remotePort,
							 "urn:schemas-upnp-org:device:MediaServer:1");
		else if (ba.indexOf("ssdp:all") > 0)
			sendSsdpDiscover(remoteAddr, remotePort,
							 "urn:schemas-upnp-org:device:MediaServer:1");
		else if (ba.indexOf(Global::getUsn()) > 0)
			sendSsdpDiscover(remoteAddr, remotePort, Global::getUsn());
	}
}

void UpnpServer::onTimeout()
{
	sendSsdpAlive();
}

QString UpnpServer::buildMessage(const QString& nt,
								 const QString& msg)
{
	QString s;
	QString usn = Global::getUsn();
	QString serverHost = Global::getDefaultIPv4LocalAddr().toString();

	s.append(QString("NOTIFY * HTTP/1.1\r\n"));
	s.append(QString("HOST: %1:%2\r\n").
			 arg(QHostAddress(UPNP_GROUP).toString()).
			 arg(UPNP_PORT));
	s.append(QString("NT: %1\r\n").arg(nt));
	s.append(QString("NTS: %1\r\n").arg(msg));

	if (msg == SSDP_ALIVE) {
		s.append(QString("LOCATION: http://%1:%2/%3\r\n").
				 arg(serverHost).
				 arg(m_tcpPort).
				 arg(Global::getDescriptionXmlUrl()));
		s.append(QString("CACHE-CONTROL: max-age=1800\r\n"));
		s.append(QString("SERVER: SomeOS, UPnP/1.0, TvHolic/%1\r\n").
				 arg(TVHOLIC_VERSION));
	}


	s.append(QString("USN: %1").arg(usn));
	if (nt != usn)
		s.append(QString("::%1").arg(nt));

	s.append(QString("\r\n"));

	return s;
}

/*
  WE IGNORE THIS RECOMMENDATAIONS:

  UPnP-arch-DeviceArchitecture-v1.1.pdf
  1. Devices SHOULD wait a random interval (e.g. between 0 and 100milliseconds)
  before sending an initial set of advertisements in order to reduce the
  likelihood of network storms;

  2. Due to the unreliable nature of UDP, devices SHOULD send the entire set of
  discovery messages more than once with some delay between sets e.g. a few
  hundred milliseconds.
*/
void UpnpServer::sendSsdpAlive()
{
	QString msg1, msg2, msg3, msg4, msg5;

	msg1 = buildMessage("upnp:rootdevice",
						SSDP_ALIVE);
	msg2 = buildMessage(Global::getUsn(),
						SSDP_ALIVE);
	msg3 = buildMessage("urn:schemas-upnp-org:device:MediaServer:1",
						SSDP_ALIVE);
	msg4 = buildMessage("urn:schemas-upnp-org:device:ContentDirectory:1",
						SSDP_ALIVE);
	msg5 = buildMessage("urn:schemas-upnp-org:device:ConnectionManager:1",
						SSDP_ALIVE);

	m_udpSocket.writeDatagram(msg1.toAscii(),
							  QHostAddress(UPNP_GROUP),
							  UPNP_PORT);
	m_udpSocket.writeDatagram(msg2.toAscii(),
							  QHostAddress(UPNP_GROUP),
							  UPNP_PORT);
	m_udpSocket.writeDatagram(msg3.toAscii(),
							  QHostAddress(UPNP_GROUP),
							  UPNP_PORT);
	m_udpSocket.writeDatagram(msg4.toAscii(),
							  QHostAddress(UPNP_GROUP),
							  UPNP_PORT);
	m_udpSocket.writeDatagram(msg5.toAscii(),
							  QHostAddress(UPNP_GROUP),
							  UPNP_PORT);
}

void UpnpServer::sendSsdpByeBye()
{
	QString msg1, msg2, msg3, msg4;

	msg1 = buildMessage("upnp:rootdevice",
						SSDP_BYEBYE);
	msg2 = buildMessage("urn:schemas-upnp-org:device:MediaServer:1",
						SSDP_BYEBYE);
	msg3 = buildMessage("urn:schemas-upnp-org:device:ContentDirectory:1",
						SSDP_BYEBYE);
	msg4 = buildMessage("urn:schemas-upnp-org:device:ConnectionManager:1",
						SSDP_BYEBYE);

	m_udpSocket.writeDatagram(msg1.toAscii(),
							  QHostAddress(UPNP_GROUP),
							  UPNP_PORT);
	m_udpSocket.writeDatagram(msg2.toAscii(),
							  QHostAddress(UPNP_GROUP),
							  UPNP_PORT);
	m_udpSocket.writeDatagram(msg3.toAscii(),
							  QHostAddress(UPNP_GROUP),
							  UPNP_PORT);
	m_udpSocket.writeDatagram(msg4.toAscii(),
							  QHostAddress(UPNP_GROUP),
							  UPNP_PORT);
}

void UpnpServer::sendSsdpDiscover(const QHostAddress& addr,
								  quint16 port,
								  const QString& msg)
{
	QString s;
	QString usn = Global::getUsn();
	QString serverHost = Global::getDefaultIPv4LocalAddr().toString();
	QString dateTime = QDateTime::currentDateTime().toString(
		"ddd, dd MMM yyyy hh:mm:ss");

	if (msg != usn)
		usn.append(QString("::%1").arg(msg));

	s.append(QString("HTTP/1.1 200 OK\r\n"));
	s.append(QString("CACHE-CONTROL: max-age=1200\r\n"));
	s.append(QString("DATE: %1 UTC\r\n").arg(dateTime));
	s.append(QString("LOCATION: http://%1:%2/%3\r\n").
			 arg(serverHost).
			 arg(m_tcpPort).
			 arg(Global::getDescriptionXmlUrl()));
	s.append(QString("SERVER: SomeOS, UPnP/1.0, TvHolic/%1\r\n").
			 arg(TVHOLIC_VERSION));
	s.append(QString("ST: %1\r\n").arg(msg));
	s.append(QString("EXT: \r\n"));
	s.append(QString("USN: %1\r\n").arg(usn));
	s.append(QString("Content-Length: 0\r\n"));
	s.append(QString("\r\n"));

	m_udpSocket.writeDatagram(s.toAscii(), addr, port);
}
