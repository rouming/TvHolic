/*
 * Universidade Federal de Campina Grande
 * Centro de Engenharia Elétrica e Informática
 * Laboratório de Sistemas Embarcados e Computação Pervasiva
 * BRisa Project / BRisa-Qt - http://brisa.garage.maemo.org
 * Filename: brisawebserver.cpp
 * Created:
 * Description: Implementation of BrisaWebServer class.
 * Authors: Andre Dieb Martins <andre.dieb@gmail.com> @since 2009
 *          Leandro Melo de Sales <leandroal@gmail.com> @since 2009-2010
 *          Vinícius dos Santos Oliveira <vini.ipsmaker@gmail.com> @since 2010
 *
 * Copyright (C) <2009> <Embbeded Systems and Pervasive Computing Laboratory>
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

#include "brisawebserver.h"
#include "brisawebstaticcontent.h"
#include "brisawebserversession.h"
#include "brisawebfile.h"
#include "brisawebserversession.h"

#define DEFAULT_PAGE "<html><body><h1>BRisa WebServer!</h1></body></html>\n"

using namespace Brisa;

BrisaWebserver::BrisaWebserver(const QHostAddress &host, quint16 port) :
	HttpServer(host, port),
	m_factory(this)
{
	addService("/", new BrisaWebStaticContent(DEFAULT_PAGE, this));
}

BrisaWebserver::~BrisaWebserver()
{
}

static QRegExp toRegExp(const QByteArray& path)
{
	QString str(path);

	if (!str.startsWith('/'))
		str.prepend('/');

	return QRegExp(QRegExp::escape(str));
}

void BrisaWebserver::addService(const QByteArray &path, BrisaWebService *service)
{
	addService(toRegExp(path), service);
}

void BrisaWebserver::addService(const QRegExp &rx, BrisaWebService *service)
{
	if (!service || rx.isEmpty())
		return;

	mutex.lock();

	services.append(qMakePair(rx, service));

	mutex.unlock();

	qDebug() << "Adding Service: " << rx.pattern();
}

void BrisaWebserver::removeService(const QByteArray &path)
{
	removeService(toRegExp(path));
}

void BrisaWebserver::removeService(const QRegExp &rx)
{
	if (rx.isEmpty())
		return;

	mutex.lock();

	for (QList<Pair>::Iterator it = services.begin();
		 it != services.end();
		 ++it) {
		Pair& pair = *it;
		if (pair.first == rx) {
			services.erase(it);
			break;
		}
	}

	mutex.unlock();
}

BrisaWebService *BrisaWebserver::service(QByteArray path) const
{
	BrisaWebService *serv = NULL;

	if (path.isEmpty())
		return NULL;

	if (!path.startsWith('/'))
		path.prepend('/');

	mutex.lock();

	for (QList<Pair>::ConstIterator it = services.begin();
		 it != services.end();
		 ++it) {
		const Pair& pair = *it;
		if (pair.first.exactMatch(QString(path))) {
			serv = pair.second;
			break;
		}
	}

	mutex.unlock();

	return serv;
}

HttpServerFactory &BrisaWebserver::factory()
{
	return m_factory;
}

HttpSession *BrisaWebserver::Factory::generateSessionHandler(HttpSessionManager *manager,
															 QObject *parent)
{
	return new BrisaWebserverSession(server, manager, parent);
}
