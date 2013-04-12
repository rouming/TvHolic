#include "HttpSession.h"

HttpSession::HttpSession(int sock, QMutex* mutex) :
	Job(mutex),
	m_tcpSocket(this)
{
	QObject::connect(&m_tcpSocket,
					 SIGNAL(readyRead()),
					 SLOT(onReadyRead()));
	QObject::connect(&m_tcpSocket,
					 SIGNAL(bytesWritten(qint64)),
					 SLOT(onBytesWritten(qint64)));
	QObject::connect(&m_tcpSocket,
					 SIGNAL(disconnected()),
					 SLOT(onDisconnected()));

	bool res = m_tcpSocket.setSocketDescriptor(sock);
	(void)res;
	Q_ASSERT(res);
}

HttpSession::~HttpSession()
{
	m_worker->removeJob(this);
}

void HttpSession::onReadyRead()
{
	if (!m_tcpSocket.canReadLine())
		return;

	QByteArray ba = m_tcpSocket.readLine();
	QString str(ba);

	printf("#%lld LINE: %s\n",
		   (quint64)QThread::currentThreadId(),
		   qPrintable(str));

	m_tcpSocket.write(ba.constData(), ba.size());
}

void HttpSession::onBytesWritten(qint64 wr)
{
	printf("bytes written %lld\n", wr);
}

void HttpSession::onDisconnected()
{
	deleteLater();
}
