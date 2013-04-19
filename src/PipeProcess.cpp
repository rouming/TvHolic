#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#include <QMutex>
#include <QFile>
#include <QHash>
#include <QStringList>

#include "PipeProcess.h"

struct ProcessManager {
	QMutex mutex;
	bool inited;
	QHash<pid_t, PipeProcess*> pids;
	struct sigaction sa_old_sigchld_handler;
	sigset_t mask;
};

static ProcessManager s_mng;

#define EINTR_LOOP(var, cmd)                    \
    do {                                        \
        var = cmd;                              \
    } while (var == -1 && errno == EINTR)

static quint32 round_buffer_size(quint32 buffer_size)
{
	if (!((buffer_size - 1) & buffer_size))
		return buffer_size;

	buffer_size--;
	buffer_size |= buffer_size >> 1;
	buffer_size |= buffer_size >> 2;
	buffer_size |= buffer_size >> 4;
	buffer_size |= buffer_size >> 8;
	buffer_size |= buffer_size >> 16;
	buffer_size = (buffer_size + 1) >> 1;

	return buffer_size;
}

static inline qint32 safe_write(int fd, const void *data, qint32 len)
{
    qint32 ret = 0;
    EINTR_LOOP(ret, ::write(fd, data, len));
    return ret;
}

static inline qint32 safe_read(int fd, void *data, qint32 len)
{
    qint32 ret = 0;
    EINTR_LOOP(ret, ::read(fd, data, len));
    return ret;
}

static inline int safe_close(int fd)
{
    int ret;
    EINTR_LOOP(ret, ::close(fd));
    return ret;
}

static inline int safe_dup2(int fd1, int fd2)
{
    int ret;
    EINTR_LOOP(ret, ::dup2(fd1, fd2));
    return ret;
}

static inline int safe_pipe(int pipes[2], int flags)
{
	int err = ::pipe(pipes);
	if (err != 0)
		return err;

    ::fcntl(pipes[0], F_SETFD, FD_CLOEXEC);
    ::fcntl(pipes[1], F_SETFD, FD_CLOEXEC);

    // set non-block too?
    if (flags & O_NONBLOCK) {
        ::fcntl(pipes[0], F_SETFL, ::fcntl(pipes[0], F_GETFL) | O_NONBLOCK);
        ::fcntl(pipes[1], F_SETFL, ::fcntl(pipes[1], F_GETFL) | O_NONBLOCK);
    }

	return 0;
}

static bool sigchild_lock()
{
	sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, &s_mng.mask) == -1)
		return false;

	s_mng.mutex.lock();
	return true;
}

static bool sigchild_unlock()
{
    if (sigprocmask(SIG_SETMASK, &s_mng.mask, NULL) == -1)
		return false;

	s_mng.mutex.unlock();
	return true;
}

void sigchld_handler(int signum)
{
	pid_t p;
    int status;
	QMutexLocker locker(&s_mng.mutex);

    while ((p = ::waitpid(-1, &status, WNOHANG)) != -1) {
		QHash<pid_t, PipeProcess*>::iterator it = s_mng.pids.find(p);
		if (it == s_mng.pids.end())
			continue;
		PipeProcess* proc = it.value();
		proc->__childDied(status);
    }

    // load it as volatile
    void (*oldAction)(int) = ((volatile struct sigaction *)&s_mng.sa_old_sigchld_handler)->sa_handler;
    if (oldAction && oldAction != SIG_IGN)
        oldAction(signum);
}

static void init_child_sigaction()
{
	if (s_mng.inited)
		return;

	// set up the SIGCHLD handler, which writes a single byte to the dead
	// child pipe every time a child dies.
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler = sigchld_handler;
	action.sa_flags = SA_NOCLDSTOP;
	int err = ::sigaction(SIGCHLD, &action, &s_mng.sa_old_sigchld_handler);
	if (err != 0) {
		qWarning("Error: can't set sigaction for SIGCHLD %d\n", err);
		return;
	}

	s_mng.inited = true;
}

static void closePipes(int pipes[2], QSocketNotifier*& notifier)
{
	for (int i = 0; i < 2; ++i) {
		if (pipes[i] != -1) {
			safe_close(pipes[i]);
			pipes[i] = -1;
		}
	}

	delete notifier;
	notifier = NULL;
}

static bool createPipes(int pipes[2],
						QSocketNotifier*& notifier,
						QObject* receiver, const char* slot)
{
	Q_ASSERT(pipes[0] == -1);
	Q_ASSERT(pipes[1] == -1);
	Q_ASSERT(notifier == NULL);

	int err = safe_pipe(pipes, O_NONBLOCK);
	if (err != 0) {
		qWarning("Error: can't init pipes %d\n", err);
		return false;
	}

	notifier = new QSocketNotifier(pipes[0], QSocketNotifier::Read);
	bool r = QObject::connect(notifier,
							  SIGNAL(activated(int)),
							  receiver,
							  slot);
	(void)r;
	Q_ASSERT(r);

	return true;
}

static quint32 readToRing(struct ring_buffer* ring, int pipe)
{
	quint32 done = 0;
	while (ring_buffer_free_size(ring)) {
		void *p1, *p2;
		quint32 sz1, sz2;
		ring_buffer_write_ptr(ring, ring_buffer_free_size(ring),
							  &p1, &sz1, &p2, &sz2);
		Q_ASSERT(p1 && sz1);
		(void)p2;
		(void)sz2;

		qint32 ret = safe_read(pipe, p1, sz1);
		if (ret <= 0) {
			if (ret < 0 && errno != EAGAIN)
				qWarning("Error: error on read, err %u", errno);
			break;
		}
		ring_buffer_write_advance(ring, (quint32)ret);
		done += ret;
	}

	return done;
}

static quint32 readFromRing(struct ring_buffer* ring, quint32 sz, QIODevice* dev)
{
	quint32 done = 0;
	void *p1, *p2;
	quint32 sz1, sz2;
	quint32 to_read = ring_buffer_used_size(ring);

	to_read = qMin(sz, to_read);
	if (to_read == 0)
		return 0;

	ring_buffer_read_ptr(ring, to_read,
						 &p1, &sz1, &p2, &sz2);
	if (p1 && sz1)
		done += dev->write((const char*)p1, sz1);
	if (p2 && sz2)
		done += dev->write((const char*)p2, sz2);

	ring_buffer_read_advance(ring, done);

	return done;
}

PipeProcess::PipeProcess(quint32 stdoutSz, quint32 stderrSz) :
	m_deathNotif(NULL),
	m_stdoutNotif(NULL),
	m_stderrNotif(NULL),
	m_pid(0),
	m_exitStatus(0),
	m_stdout(NULL),
	m_stderr(NULL),
	m_stdoutSz(round_buffer_size(stdoutSz ? stdoutSz : (quint32)DefaultBuffSz)),
	m_stderrSz(round_buffer_size(stderrSz ? stderrSz : (quint32)DefaultBuffSz))
{
	m_deathPipes[0] = -1;
	m_deathPipes[1] = -1;
	m_stdoutPipes[0] = -1;
	m_stdoutPipes[1] = -1;
	m_stderrPipes[0] = -1;
	m_stderrPipes[1] = -1;

	ring_buffer_init(&m_stdout_ring, NULL, 0);
	ring_buffer_init(&m_stderr_ring, NULL, 0);

	init_child_sigaction();
}

PipeProcess::~PipeProcess()
{
	sigchild_lock();

	if (m_pid > 0) {
		qWarning("Error: process is alive, pid %u!", m_pid);
		__unregisterChild();
	}

	if (m_stdout)
		munmap(m_stdout, m_stdoutSz);
	if (m_stderr)
		munmap(m_stderr, m_stderrSz);

	closePipes(m_deathPipes, m_deathNotif);
	closePipes(m_stdoutPipes, m_stdoutNotif);
	closePipes(m_stderrPipes, m_stderrNotif);

	sigchild_unlock();
}

bool PipeProcess::start(const QString& program, const QStringList& args)
{
	sigchild_lock();

	if (m_pid > 0) {
		qWarning("Error: can't start process, it is already running, pid %u",
				 m_pid);
		goto err;
	}

	if (m_stdout == NULL) {
		m_stdout = ::mmap(0, m_stdoutSz, PROT_READ | PROT_WRITE,
						  MAP_ANONYMOUS | MAP_SHARED, -1, 0);
		if (m_stdout == MAP_FAILED) {
			m_stdout = NULL;
			qWarning("Error: Can't allocate memory for stdout, err %u", errno);
			goto err;
		}
		ring_buffer_init(&m_stdout_ring, m_stdout, m_stdoutSz);
	}
	if (m_stderr == NULL) {
		m_stderr = ::mmap(0, m_stderrSz, PROT_READ | PROT_WRITE,
						  MAP_ANONYMOUS | MAP_SHARED, -1, 0);
		if (m_stderr == MAP_FAILED) {
			m_stderr = NULL;
			qWarning("Error: Can't allocate memory for stderr, err %u", errno);
			goto err;
		}
		ring_buffer_init(&m_stderr_ring, m_stderr, m_stderrSz);
	}

	if (m_deathNotif == NULL && !createPipes(m_deathPipes, m_deathNotif,
											 this,
											 SLOT(onChildDeathActivate()))) {
		qWarning("Error: Can't create pipes");
		goto err;
	}
	if (m_stdoutNotif == NULL && !createPipes(m_stdoutPipes, m_stdoutNotif,
											  this,
											  SLOT(onStdoutActivate()))) {
		qWarning("Error: Can't create pipes");
		goto err;
	}
	if (m_stderrNotif == NULL && !createPipes(m_stderrPipes, m_stderrNotif,
											  this,
											  SLOT(onStderrActivate()))) {
		qWarning("Error: Can't create pipes");
		goto err;
	}

	m_pid = ::fork();
	if (m_pid < 0) {
		m_pid = 0;
		qWarning("Error: can't create process, err %u", errno);
		goto err;
	}
	else if (m_pid == 0) {
		QByteArray encProgram = QFile::encodeName(program);
		char **argv = new char*[args.count() + 2];
		argv[args.count() + 1] = 0;
		argv[0] = encProgram.data();
		for (int i = 0; i < args.count(); ++i) {
#ifdef _MAC_
			argv[i + 1] = ::strdup(args[i].toUtf8().constData());
#else
			argv[i + 1] = ::strdup(args[i].toLocal8Bit().constData());
#endif
		}
		safe_dup2(m_stdoutPipes[1], 1);
		safe_dup2(m_stderrPipes[1], 2);

		execv(argv[0], argv);
		_exit(-1);
	}

	__registerChild();

	// NOTE: started is being called with mutex lock to guarantee order of
	//       started/finished calls
	emit started();

	sigchild_unlock();
	return true;

err:
	sigchild_unlock();
	return false;
}

void PipeProcess::__registerChild()
{
	Q_ASSERT(m_pid > 0);
	Q_ASSERT(!s_mng.pids.contains(m_pid));
	s_mng.pids[m_pid] = this;
}

void PipeProcess::__unregisterChild()
{
	Q_ASSERT(m_pid > 0);
	Q_ASSERT(s_mng.pids.contains(m_pid));
	s_mng.pids.remove(m_pid);
	m_pid = 0;
}

void PipeProcess::__childDied(int exitStatus)
{
	__unregisterChild();
	m_exitStatus = exitStatus;
	safe_write(m_deathPipes[1], "", 1);
}

void PipeProcess::onChildDeathActivate()
{
	char c;
	safe_read(m_deathPipes[0], &c, 1);
	emit finished(WEXITSTATUS(m_exitStatus), !WIFEXITED(m_exitStatus));
}

void PipeProcess::onStdoutActivate()
{
	readToRing(&m_stdout_ring, m_stdoutPipes[0]);
	emit readyReadStdout();
}

void PipeProcess::onStderrActivate()
{
	readToRing(&m_stderr_ring, m_stderrPipes[0]);
	emit readyReadStderr();
}

void PipeProcess::kill()
{
	sigchild_lock();

	if (m_pid > 0)
		::kill(m_pid, SIGTERM);

	sigchild_unlock();

}

bool PipeProcess::isRunning() const
{
	sigchild_lock();
	bool ret = (m_pid > 0);
	sigchild_unlock();

	return ret;
}

quint32 PipeProcess::readFromStdout(quint32 sz, QIODevice* dev)
{
	return readFromRing(&m_stdout_ring, sz, dev);
}

quint32 PipeProcess::readFromStderr(quint32 sz, QIODevice* dev)
{
	return readFromRing(&m_stderr_ring, sz, dev);
}

quint32 PipeProcess::stdoutBytes() const
{
	return ring_buffer_used_size(&m_stdout_ring);
}

quint32 PipeProcess::stderrBytes() const
{
	return ring_buffer_used_size(&m_stderr_ring);
}
