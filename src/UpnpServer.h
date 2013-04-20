#ifndef __UPNP_SERVER_H__
#define __UPNP_SERVER_H__

#include <QUdpSocket>
#include <QTimer>

class UpnpServer : public QObject
{
	Q_OBJECT
public:
	enum {
		UPNP_PORT  = 1900,
		UPNP_GROUP = 0xeffffffa, /* 239.255.255.250 */
	};

	UpnpServer();
	~UpnpServer();

	bool init(quint16 tcpPort);
	void stop();

private:
	QString buildMessage(const QString& nt, const QString& msg);

	void sendSsdpAlive();
	void sendSsdpByeBye();
	void sendSsdpDiscover(const QHostAddress& addr,
						  quint16 port,
						  const QString& msg);

private slots:
	void onReadyRead();
	void onTimeout();

private:
	QUdpSocket m_udpSocket;
	QByteArray m_datagram;
	QTimer m_aliveTimer;
	quint16 m_tcpPort;
};

#endif //__UPNP_SERVER_H__
