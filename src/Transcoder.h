#ifndef __TRANDCODER_H__
#define __TRANDCODER_H__

#include <QTcpSocket>

class Transcoder
{
public:
	enum State {
		Stopped     = 0,
		Transcoding = 1
	};

	Transcoder(QTcpSocket& socket) :
		m_tcpSocket(socket),
		m_state(Stopped)
	{}

	virtual ~Transcoder() {}

	virtual bool startTranscoding() = 0;
	virtual void terminateTranscoding() = 0;

	virtual State process() = 0;

protected:
	QTcpSocket& m_tcpSocket;
	State m_state;
};

#endif //__TRANDCODER_H__
