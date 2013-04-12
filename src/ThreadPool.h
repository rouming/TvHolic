#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <QThread>
#include <QMutex>
#include "List.h"

struct Job {
	class WorkerThread* m_worker;

	Job(QMutex* mutex) :
		m_worker(NULL),
		m_mutex(mutex)
	{
		cd_list_init(&m_entry);
	}

	virtual ~Job() {}

private:
	friend class WorkerThread;

	cd_list m_entry;
	QMutex* m_mutex;
};

class WorkerThread : public QThread
{
	Q_OBJECT
public:
	WorkerThread();
	~WorkerThread();

	void removeJob(Job* data);

private slots:
	void onAppendJob(Job* data);
	void onFinished();

private:
	friend class ThreadPool;

	cd_list m_entry;
	cd_list m_jobsList;
	unsigned m_njobs;
};

class ThreadPool : public QObject
{
	Q_OBJECT
public:
	enum {
		MIN_THREADS = 4,
	};

	ThreadPool();

	bool init();
	void stop();

	bool appendJob(Job* data);

signals:
	bool sig_appendJob(Job* data);

private:
	cd_list m_threads;
};

#endif // __THREADPOOL_H__
