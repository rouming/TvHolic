#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "AsIsTranscoder.h"

AsIsTranscoder::AsIsTranscoder(QTcpSocket& socket) :
	Transcoder(socket),
	m_writeNotif(socket.socketDescriptor(), QSocketNotifier::Write),
	m_fd(-1),
	m_off(0)
{
	bool r = QObject::connect(&m_writeNotif,
							  SIGNAL(activated(int)),
							  SLOT(onWriteActivated()));
	(void)r;
	Q_ASSERT(r);

	m_writeNotif.setEnabled(false);
}

AsIsTranscoder::~AsIsTranscoder()
{
	if (m_state != Stopped) {
		qWarning("Error: transcoding is in process! Terminate it");
		terminateTranscoding();
	}
}

bool AsIsTranscoder::startTranscoding()
{
	if (m_state != Stopped)
		return false;

	do {
		//XXX
		m_fd = ::open("/home/roman/Desktop/GLD.avi", O_CLOEXEC, O_RDONLY);
	} while (m_fd == -1 && errno == EINTR);

	if (m_fd < 0) {
		qWarning("Error: can't open file for reading, err %d", errno);
		return false;
	}

	m_state = Transcoding;

	process();
	return true;
}

void AsIsTranscoder::terminateTranscoding()
{
	int ret;

	if (m_state != Transcoding)
		return;
	do {
		ret = ::close(m_fd);
	} while (ret == -1 && errno == EINTR);

	m_writeNotif.setEnabled(false);
	m_fd = -1;
	m_off = 0;
	m_state = Stopped;
}

Transcoder::State AsIsTranscoder::process()
{
	int ret;

	do {
		ret = ::sendfile64(m_tcpSocket.socketDescriptor(), m_fd,
						   (off_t *)&m_off, MaxSendChunk);
	} while (ret == -1 && errno == EINTR);

	// Try again later
	if (ret < 0 && errno == EAGAIN)
		m_writeNotif.setEnabled(true);
	// Something was written, will return later
	else if (ret > 0)
		m_writeNotif.setEnabled(true);
	// Error happened
	else if (ret < 0) {
		qWarning("Error: sendfile failed, err %d", errno);
		terminateTranscoding();
	}
	// EOF
	else if (ret == 0)
		terminateTranscoding();

	return m_state;
}

void AsIsTranscoder::onWriteActivated()
{
	process();
}
