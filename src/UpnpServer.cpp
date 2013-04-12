#include "UpnpServer.h"

UpnpServer::UpnpServer()
{
	bool res;
	res = QObject::connect(&m_udpSocket,
						   SIGNAL(readyRead()), SLOT(onReadyRead()));
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

	return true;

err:
	m_udpSocket.close();
	return false;
}

void UpnpServer::stop()
{
	m_udpSocket.close();
}

static void processDatagram(const QHostAddress& sender, quint16 port,
							const QByteArray& datagram, quint32 sz)
{
	(void)datagram;

	//XXX
	return;

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
