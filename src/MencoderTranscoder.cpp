#include <QStringList>
#include <QBuffer>

#include "MencoderTranscoder.h"

MencoderTranscoder::MencoderTranscoder(QTcpSocket& socket,
									   quint32 minThreshold,
									   quint32 maxThreshold) :
	Transcoder(socket),
	m_mencoder(maxThreshold, 16<<10),
	m_minThreshold(minThreshold),
	m_maxThreshold(maxThreshold)
{
	Q_ASSERT(minThreshold < maxThreshold);

	bool r;
	(void)r;
	r = QObject::connect(&m_mencoder,
						 SIGNAL(started()),
						 SLOT(onStarted()));
	Q_ASSERT(r);

	r = QObject::connect(&m_mencoder,
						 SIGNAL(finished(int, bool)),
						 SLOT(onFinished(int, bool)));
	Q_ASSERT(r);

	r = QObject::connect(&m_mencoder,
						 SIGNAL(readyReadStdout()),
						 SLOT(onStdout()));
	Q_ASSERT(r);
	r = QObject::connect(&m_mencoder,
						 SIGNAL(readyReadStderr()),
						 SLOT(onStderr()));
	Q_ASSERT(r);
}

MencoderTranscoder::~MencoderTranscoder()
{
	if (m_mencoder.isRunning()) {
		qWarning("Error: process is running! Terminate it");
		terminateTranscoding();
	}
}

bool MencoderTranscoder::startTranscoding()
{
	if (m_state != Stopped)
		return false;

	//XXX
	QString mencoder("-really-quiet -of mpeg -ovc lavc -lavcopts vcodec=mpeg2video:mbd=2:keyint=25:vrc_minrate=600:vbitrate=2500 -oac lavc -af lavcresample=44100 -srate 44100 -mpegopts format=mpeg2 /home/roman/Desktop/GLD.avi -o -");

	return m_mencoder.start("/usr/bin/mencoder", mencoder.split(" "));
}

void MencoderTranscoder::terminateTranscoding()
{
	if (m_state != Transcoding)
		return;
	m_mencoder.kill();
	m_state = Stopped;
}

Transcoder::State MencoderTranscoder::process()
{
	quint32 written = m_tcpSocket.bytesToWrite();
	quint32 can_rd = m_mencoder.stdoutBytes();
	quint32 can_wr;

	// Nothing to do
	if (can_rd == 0)
		return m_state;

	// Socket is already full
	if (written >= m_maxThreshold)
		return Transcoding;

	// Not enough data if process is still alive
	if (can_rd <= m_minThreshold && m_mencoder.isRunning())
		return Transcoding;

	can_wr = m_maxThreshold - written;
	can_rd = (can_rd <= m_minThreshold ? can_rd : can_rd - m_minThreshold);

	m_mencoder.readFromStdout(qMin(can_wr, can_rd), &m_tcpSocket);

	return Transcoding;
}

void MencoderTranscoder::onStarted()
{
	m_state = Transcoding;
}

void MencoderTranscoder::onFinished(int exitCode, bool crashed)
{
	(void)exitCode;
	if (crashed) {
		qWarning("Error: mencoder process exited with crash");
	}
	m_state = Stopped;
	process();
}

void MencoderTranscoder::onStdout()
{
	process();
}

void MencoderTranscoder::onStderr()
{
	QByteArray ba;
	QBuffer out(&ba);
	out.open(QBuffer::WriteOnly);

	m_mencoder.readFromStderr(m_mencoder.stderrBytes(), &out);
	qWarning("-------------- error output %u ------------------\n", ba.size());
	qWarning("%s", ba.constData());
	qWarning("----------------------------------------------\n");
}
