#ifndef __UPNP_SERVER_H__
#define __UPNP_SERVER_H__

#include <QUdpSocket>

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

private slots:
	void onReadyRead();

private:
	QUdpSocket m_udpSocket;
	QByteArray m_datagram;
};

#endif //__UPNP_SERVER_H__
