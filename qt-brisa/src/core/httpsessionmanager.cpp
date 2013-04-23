/*
 * Universidade Federal de Alagoas
 * Instituto de Computação
 * Laboratório de Computação Pervasiva
 * BRisa Project / BRisa-Qt - http://brisa.garage.maemo.org
 * Authors: Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com> 2010
 *
 * Copyright (C) <2010> <Pervasive Computing Laboratory @ Federal
 * University of Alagoas>
 *
 * BRisa-Qt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QEventLoop>
#include <QTcpSocket>
#include "httpsessionmanager.h"
#include "httpsession.h"

using namespace Brisa;

HttpSessionManager::HttpSessionManager(HttpServer *parent) :
	QThread(),
	server(parent),
	connector(NULL)
{
	moveToThread(this);
}

void HttpSessionManager::waitForEventLoopStart()
{
	QMutexLocker locker(&mutex);
	if (!isRunning()) {
		Q_ASSERT(0);
		return;
	}
	if (connector == NULL)
		waitCond.wait(&mutex);
}

void HttpSessionManager::run()
{
	QMutexLocker locker(&mutex);

	connector = new Connector(this);
	connect(connector, SIGNAL(sig_newConnection(int)),
			connector, SLOT(onNewConnection(int)));
	waitCond.wakeOne();

	locker.unlock();

	QEventLoop eventLoop;
	eventLoop.exec();

	// ...
	// Here new connection can be accepted
	// this case will be handled specially
	// by processing pended events
	// ...

	locker.relock();
	delete connector;
	connector = NULL;
	locker.unlock();

	// We may have something pended
	while (eventLoop.processEvents())
		;
}

void HttpSessionManager::addSession(int socketDescriptor)
{
	QMutexLocker locker(&mutex);
	if (!isRunning() || connector == NULL) {
		qWarning("Error: can't accept incoming connection");
		QTcpSocket sock;
		sock.setSocketDescriptor(socketDescriptor);
		sock.close();

		return;
	}
	connector->newConnection(socketDescriptor);
}

void HttpSessionManager::newConnection(int socketDescriptor)
{
	// Very rare race, thread is going to be stopped
	if (connector == NULL) {
		qWarning("Warning: thread is going to be deleted, close accepted connection");
		QTcpSocket sock;
		sock.setSocketDescriptor(socketDescriptor);
		sock.close();
		return;
	}

	if (pool.size()) {
		pool.back()->setSession(socketDescriptor);
		pool.pop_back();
	} else {
		HttpSession *s = server->factory().generateSessionHandler(this);
		s->setSession(socketDescriptor);
	}
}

void HttpSessionManager::releaseSession(HttpSession *session)
{
	Q_ASSERT(QThread::currentThread() == this);
	pool.append(session);
}
