#ifndef __MENCODER_TRANDCODER_H__
#define __MENCODER_TRANDCODER_H__

#include <QObject>
#include "PipeProcess.h"
#include "Transcoder.h"

class MencoderTranscoder : public Transcoder
{
	Q_OBJECT
public:
	MencoderTranscoder(QTcpSocket&,
					   quint32 minThreshold,
					   quint32 maxThreshold);
	virtual ~MencoderTranscoder();

	virtual bool startTranscoding(const QString& path);
	virtual void terminateTranscoding();

	virtual State process();

private slots:
	void onStarted();
	void onFinished(int, bool);
	void onStdout();
	void onStderr();

private:
	PipeProcess m_mencoder;
	quint32 m_minThreshold;
	quint32 m_maxThreshold;
};

#endif //__MENCODER_TRANDCODER_H__
