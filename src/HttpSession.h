#ifndef __HTTP_SESSION_H__
#define __HTTP_SESSION_H__

#include <QTcpSocket>
#include "TcpServer.h"
#include "ThreadPool.h"
#include "Transcoder.h"

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
	Transcoder* m_transcoder;
};

#endif //__HTTP_SESSION_H__
