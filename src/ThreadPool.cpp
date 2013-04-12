#include <QMetaType>
#include "ThreadPool.h"

WorkerThread::WorkerThread() :
	m_njobs(0)
{
	cd_list_init(&m_entry);
	cd_list_init(&m_jobsList);
}

WorkerThread::~WorkerThread()
{
	Q_ASSERT(m_njobs == 0);
}

void WorkerThread::onAppendJob(Job* job)
{
	QMutexLocker locker(job->m_mutex);
	// Check that this thread is the first
	if (job->m_worker)
		return;
	job->m_worker = this;
	cd_list_add_tail(&job->m_entry, &m_jobsList);
	m_njobs++;
}

void WorkerThread::onFinished()
{
	Q_ASSERT(QThread::currentThread() == this);

	Job *job, *tmp;
	cd_list_for_each_entry_safe(Job, job, tmp, &m_jobsList, m_entry)
		// Good job will call remove
		delete job;

	Q_ASSERT(m_njobs == 0);
	Q_ASSERT(cd_list_empty(&m_jobsList));
}

void WorkerThread::removeJob(Job* job)
{
	QMutexLocker locker(job->m_mutex);
	// Check that this job belongs to this thread
	if (job->m_worker != this)
		return;
	job->m_worker = NULL;
	cd_list_del(&job->m_entry);
	Q_ASSERT(m_njobs);
	m_njobs--;
}

ThreadPool::ThreadPool()
{
	cd_list_init(&m_threads);

	qRegisterMetaType<Job*>("Job*");
}

bool ThreadPool::init()
{
	for (unsigned i = 0; i < MIN_THREADS; ++i) {
		WorkerThread* wt = new WorkerThread;
		// Move this object to itself
		wt->moveToThread(wt);

		QObject::connect(this,
						 SIGNAL(sig_appendJob(Job*)),
						 wt,
						 SLOT(onAppendJob(Job*)),
						 Qt::BlockingQueuedConnection);
		QObject::connect(wt,
						 SIGNAL(finished()),
						 wt,
						 SLOT(onFinished()));

		cd_list_add_tail(&wt->m_entry, &m_threads);
		wt->start();

		if (!wt->isRunning()) {
			qWarning("Error: can't start thread");
			stop();
			return false;
		}
	}

	return true;
}

bool ThreadPool::appendJob(Job* job)
{
	if (!job || job->m_worker)
		return false;
	// We emit sync signal and some thread will catch this job
	emit sig_appendJob(job);
	Q_ASSERT(job->m_worker);
	return true;
}

void ThreadPool::stop()
{
	WorkerThread *wt, *tmp;
	cd_list_for_each_entry_safe(WorkerThread, wt, tmp, &m_threads, m_entry) {
		wt->exit();
		wt->wait();
		cd_list_del(&wt->m_entry);
		delete wt;
	}
}
