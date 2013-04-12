#ifndef __HTTP_SESSION_H__
#define __HTTP_SESSION_H__

#include <QTcpSocket>
#include "TcpServer.h"
#include "ThreadPool.h"

class HttpSession : public QObject,
					public Job
{
	Q_OBJECT
public:
	HttpSession(int sock, QMutex* mutex);
	virtual ~HttpSession();

private slots:
	void onReadyRead();
	void onBytesWritten(qint64 bytes);
	void onDisconnected();

private:
	QTcpSocket m_tcpSocket;
};

#endif //__HTTP_SESSION_H__
