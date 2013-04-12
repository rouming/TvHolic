#include "UpnpServer.h"

const static QString SSDP_ALIVE  = "ssdp:alive";
const static QString SSDP_BYEBYE = "ssdp:byebye";

UpnpServer::UpnpServer()
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

bool UpnpServer::init()
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
}

static void processDatagram(const QHostAddress& sender, quint16 port,
							const QByteArray& datagram, quint32 sz)
{
	(void)datagram;

	//XXX
	//return;

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
		QHostAddress sender;
		quint16 senderPort;
		qint64 sz = m_udpSocket.pendingDatagramSize();
		qint64 rd;
		Q_ASSERT(sz > 0);

		if (m_datagram.size() < sz)
			m_datagram.resize(sz);

		rd = m_udpSocket.readDatagram(m_datagram.data(), m_datagram.size(),
									  &sender, &senderPort);
		if (rd != sz) {
			qFatal("Error: it is impossible! rd %lld != sz %lld", rd, sz);
			return;
		}
		processDatagram(sender, senderPort, m_datagram, (quint32)rd);
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

	s.append(QString("NOTIFY * HTTP/1.1\r\n"));
	s.append(QString("HOST: %1:%2").
			 arg(QHostAddress(UPNP_GROUP).toString()).
			 arg(UPNP_PORT));
	s.append(QString("NT: %1\r\n").arg(nt));
	s.append(QString("NTS: %1\r\n").arg(msg));

	if (msg == SSDP_ALIVE) {
		//XXX
		s.append(QString("LOCATION: http://%1:%2/description/fetch\r\n").
				 arg("m_host").
				 arg("m_port"));
		s.append(QString("CACHE-CONTROL: max-age=1800\r\n"));
		s.append(QString("SERVER: SomeOS, UPnP/1.0, TvHolic/%1\r\n").
				 arg(TVHOLIC_VERSION));
	}


	//XXX
	s.append(QString("USN: %1").arg("m_usn"));

	//XXX
	if (nt != "m_usn")
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
	//XXX
	msg2 = buildMessage("usn",
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
