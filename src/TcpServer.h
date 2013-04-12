#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <QTcpServer>
#include "ThreadPool.h"

class TcpServer : protected QTcpServer
{
	Q_OBJECT
public:
	TcpServer();
	~TcpServer();

	bool listen();
	void stop();

private:
	void incomingConnection(int sock);

private:
	friend class HttpSession;

	ThreadPool m_threadPool;
	QMutex m_mutex;
};

#endif // __TCP_SERVER_H__
