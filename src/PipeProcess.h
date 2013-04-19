#ifndef __PIPE_PROCESS_H
#define __PIPE_PROCESS_H

#include <QString>
#include <QObject>
#include <QIODevice>
#include <QSocketNotifier>

#include "RingBuffer.h"

class PipeProcess : public QObject
{
	Q_OBJECT;
public:
	enum {
		DefaultBuffSz = 16<<10
	};

	PipeProcess(quint32 stdoutSz, quint32 stderrSz);
	~PipeProcess();

	bool start(const QString& program, const QStringList& args);
	void kill();
	bool isRunning() const;

	quint32 readFromStdout(quint32 sz, QIODevice* dev);
	quint32 readFromStderr(quint32 sz, QIODevice* dev);

	quint32 stdoutBytes() const;
	quint32 stderrBytes() const;

private:
	void __registerChild();
	void __unregisterChild();
	void __childDied(int);

private slots:
	void onChildDeathActivate();
	void onStdoutActivate();
	void onStderrActivate();

signals:
	void started();
	void finished(int exitCode, bool crashed);

	void readyReadStdout();
	void readyReadStderr();

private:
	friend void sigchld_handler(int);

	QSocketNotifier* m_deathNotif;
	QSocketNotifier* m_stdoutNotif;
	QSocketNotifier* m_stderrNotif;

	int m_deathPipes[2];
	int m_stdoutPipes[2];
	int m_stderrPipes[2];

	pid_t m_pid;
	int m_exitStatus;

	void* m_stdout;
	void* m_stderr;
	quint32 m_stdoutSz;
	quint32 m_stderrSz;

	ring_buffer m_stdout_ring;
	ring_buffer m_stderr_ring;
};

#endif //__PIPE_PROCESS_H
