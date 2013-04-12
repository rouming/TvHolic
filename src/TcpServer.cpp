#include "TcpServer.h"
#include "ThreadPool.h"
#include "HttpSession.h"

TcpServer::TcpServer()
{}

TcpServer::~TcpServer()
{
	stop();
}

bool TcpServer::listen()
{
	bool res;
	res = QTcpServer::listen();
	if (!res) {
		qWarning("Error: can't listen to tcp socket on any host and any port");
		goto err;
	}

	res = m_threadPool.init();
	if (!res) {
		qWarning("Error: can't init thread pool");
		goto err;
	}

	//XXX
	printf("SERV PORT: %d\n",
		   serverPort());


	return true;

err:
	stop();
	return false;
}

void TcpServer::stop()
{
	QTcpServer::close();
	m_threadPool.stop();
}

void TcpServer::incomingConnection(int sock)
{
	//XXX
	printf("#%lld SOCK: %d\n",
		   (quint64)QThread::currentThreadId(),
		   sock);


	bool res;
	HttpSession* http = new HttpSession(sock, &m_mutex);

	// Append job to some worker thread
	res = m_threadPool.appendJob(http);
	if (!res) {
		qWarning("Error: can't handle incoming tcp connection");
		delete http;
		return;
	}
	Q_ASSERT(http->m_worker);

	// Http session completely belongs to thread from pool
	http->moveToThread(http->m_worker);
}
