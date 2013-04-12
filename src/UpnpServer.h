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

	bool init();
	void stop();

private:
	QString buildMessage(const QString& nt, const QString& msg);

	void sendSsdpAlive();
	void sendSsdpByeBye();

private slots:
	void onReadyRead();
	void onTimeout();

private:
	QUdpSocket m_udpSocket;
	QByteArray m_datagram;
	QTimer m_aliveTimer;
};

#endif //__UPNP_SERVER_H__
