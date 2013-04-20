#ifndef __ASIS_TRANDCODER_H__
#define __ASIS_TRANDCODER_H__

#include <QSocketNotifier>
#include "Transcoder.h"

class AsIsTranscoder : public QObject, public Transcoder
{
	Q_OBJECT
public:
	enum {
		MaxSendChunk = 256<<10
	};

	AsIsTranscoder(QTcpSocket&);
	virtual ~AsIsTranscoder();

	virtual bool startTranscoding();
	virtual void terminateTranscoding();

	virtual State process();

private slots:
	void onWriteActivated();

private:
	QSocketNotifier m_writeNotif;
	int m_fd;
	quint64 m_off;
};

#endif //__ASIS_TRANDCODER_H__
