#ifndef __TVHOLIC_H__
#define __TVHOLIC_H__

#include "TcpServer.h"
#include "UpnpServer.h"

class TvHolic : public QObject
{
	Q_OBJECT
public:
	TvHolic();
	~TvHolic();

	bool init();
	void stop();

private:
	TcpServer m_tcpServer;
	UpnpServer m_upnpServer;
};

#endif //__TVHOLIC_H__
