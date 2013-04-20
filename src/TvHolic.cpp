#include <QCoreApplication>
#include "TvHolic.h"

TvHolic::TvHolic()
{}

TvHolic::~TvHolic()
{
	stop();
}

bool TvHolic::init()
{
	bool res;
	quint16 port;
	res = m_tcpServer.listen(&port);
	if (!res) {
		qWarning("Error: can't listen on tcp server");
		goto err;
	}

	qDebug("Upnp/Http server on port: %u\n", port);

	res = m_upnpServer.init(port);
	if (!res) {
		qWarning("Error: can't init upnp server");
		goto err;
	}

	return true;

err:
	stop();
	return false;
}

void TvHolic::stop()
{
	m_upnpServer.stop();
	m_tcpServer.stop();
}

int main(int argc, char* argv[])
{
	bool res;
	QCoreApplication app(argc, argv);
	TvHolic tvHolic;

	res = tvHolic.init();
	if (!res)
		return -1;

	return app.exec();
}
