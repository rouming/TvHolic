#include "HttpSession.h"
#include "MencoderTranscoder.h"
#include "AsIsTranscoder.h"

HttpSession::HttpSession(int sock, QMutex* mutex) :
	Job(mutex),
	m_tcpSocket(this),
	m_transcoder(NULL)
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

	//m_tcpSocket.write(ba.constData(), ba.size());
	//m_transcoder = new MencoderTranscoder(m_tcpSocket, 256<<10, 1<<20);
	m_transcoder = new AsIsTranscoder(m_tcpSocket);
	m_transcoder->startTranscoding();
}

void HttpSession::onBytesWritten(qint64 wr)
{
	printf("bytes written %lld\n", wr);
	if (m_transcoder)
		m_transcoder->process();
}

void HttpSession::onDisconnected()
{
	if (m_transcoder)
		delete m_transcoder;

	deleteLater();
}
