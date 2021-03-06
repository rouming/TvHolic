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

#include "brisawebserversession.h"
#include "brisawebserver.h"
#include "brisawebservice.h"

#include <QTimer>
#include <QMetaType>

#ifndef MAPPED_MEMORY_SIZE
#define MAPPED_MEMORY_SIZE 64
#endif

// in miliseconds
#ifndef SESSION_TIMEOUT
#define SESSION_TIMEOUT 180000
#endif

using namespace Brisa;

enum State {
	WAITING_FOR_CHUNK_SIZE = 1,
	WAITING_FOR_CHUNK,
	WAITING_FOR_TRAILLING_HEADERS
};

BrisaWebserverSession::BrisaWebserverSession(BrisaWebserver *server,
											 HttpSessionManager *manager,
											 QObject *parent) :
	HttpSession(manager, parent),
	server(server),
	timer(new QTimer)
{
	qRegisterMetaType<HttpResponse>("HttpResponse");

	lastSupportedHttpVersion = HttpVersion(1, 1);
	connect(this, SIGNAL(responsePosted(HttpResponse)), this, SLOT(writeResponse(HttpResponse)));
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
	connect(getSocket(), SIGNAL(bytesWritten(qint64)),
			this, SLOT(onSocketBytesWritten(qint64)));
}

BrisaWebserverSession::~BrisaWebserverSession()
{
	delete timer;
}

void BrisaWebserverSession::respond(HttpResponse r, bool chunkedResponse)
{
	useChunkedResponse = chunkedResponse;
	emit responsePosted(r);
}

int BrisaWebserverSession::isRequestSupported(const HttpRequest &request) const
{
	if ((request.httpVersion().major() != 1)
		|| (request.httpVersion().minor() != 0 && request.httpVersion().minor() != 1)) {
		return HttpResponse::HTTP_VERSION_NOT_SUPPORTED;
	}

	if (request.method() != "GET" && request.method() != "POST"
		&& request.method() != "SUBSCRIBE" && request.method() != "UNSUBSCRIBE"
		&& request.method() != "NOTIFY") {
		return HttpResponse::NOT_IMPLEMENTED;
	}
	return 0;
}

bool BrisaWebserverSession::hasEntityBody(const HttpRequest &request) throw(HttpResponse)
{
	if (request.method() == "POST") {
		// REQUIRED. Field value MUST be text/xml; charset="utf-8" for description documents.
		if (request.header("CONTENT-TYPE").isNull()) {
			throw HttpResponse(request.httpVersion(), HttpResponse::BAD_REQUEST);
		}/* else if(request.header("CONTENT-TYPE") != "text/xml; charset=\"utf-8\"") {
            throw HttpResponse(request.httpVersion(), HttpResponse::BAD_REQUEST);
        }*/

		if (!request.header("CONTENT-LENGTH").isNull()) {
			bool ok;
			entitySize = request.header("CONTENT-LENGTH").toInt(&ok);

			if (!ok)
				throw HttpResponse(request.httpVersion(), HttpResponse::BAD_REQUEST);

			chunkedEntity = false;
		} else if (request.header("TRANSFER-ENCODING") == "chunked") {
			chunkedEntity = true;
		} else {
			throw HttpResponse(request.httpVersion(), HttpResponse::LENGTH_REQUIRED);
		}

		return true;

	} else if (request.method() == "NOTIFY") {
		if (request.header("CONTENT-TYPE").isNull()) {
			throw HttpResponse(request.httpVersion(), HttpResponse::BAD_REQUEST);
		}

		if (!request.header("CONTENT-LENGTH").isNull()) {
			bool ok;
			entitySize = request.header("CONTENT-LENGTH").toInt(&ok);

			if (!ok)
				throw HttpResponse(request.httpVersion(), HttpResponse::BAD_REQUEST);

			chunkedEntity = false;
		} else if (request.header("TRANSFER-ENCODING") == "chunked") {
			chunkedEntity = true;
		} else {
			throw HttpResponse(request.httpVersion(), HttpResponse::LENGTH_REQUIRED);
		}

		return true;
	} else {
		return false;
	}
}

bool BrisaWebserverSession::atEnd(HttpRequest &request, QByteArray &buffer) throw(HttpResponse)
{
	while (true) {
		switch (chunkedEntity) {
		case WAITING_FOR_CHUNK_SIZE: {
			int i = buffer.indexOf("\r\n");
			if (i != -1) {
				QByteArray n = buffer.left(i);
				buffer.remove(0, i + 2);

				bool ok;
				entitySize = n.toInt(&ok, 16);

				if (!ok)
					throw HttpResponse(request.httpVersion(), HttpResponse::BAD_REQUEST, true);

				if (entitySize)
					chunkedEntity = WAITING_FOR_CHUNK;
				else
					chunkedEntity = WAITING_FOR_TRAILLING_HEADERS;

				break;
			} else {
				return false;
			}
		}
		case WAITING_FOR_CHUNK: {
			if (buffer.size() >= entitySize) {
				chunksBuffer.append(buffer.left(entitySize));
				buffer.remove(0, entitySize);
				chunkedEntity = WAITING_FOR_CHUNK_SIZE;
				break;
			} else {
				return false;
			}
		}
		case WAITING_FOR_TRAILLING_HEADERS: {
			for (int i = buffer.indexOf("\r\n") ; i != -1 ; i = buffer.indexOf("\r\n")) {
				// don't starts with \r\n
				if (i != 0) {
					QByteArray header = buffer.left(i);
					buffer.remove(0, i + 2);

					i = header.indexOf(':');
					if (i > 0) {
						if (i + 1 < header.size())
							request.setHeader(header.left(i).trimmed(), header.mid(i + 1).trimmed());
						else
							request.setHeader(header.left(i).trimmed(), QByteArray());
					} else {
						throw HttpResponse(request.httpVersion() < lastSupportedHttpVersion ?
										   request.httpVersion() : lastSupportedHttpVersion,
										   HttpResponse::BAD_REQUEST, true);
					}
				} else {
					buffer = chunksBuffer;
					chunksBuffer.clear();

					return true;
				}
			}
		}

		// not a chunked entity
		case false:
			if (buffer.size() >= entitySize) {
				buffer.chop(buffer.size() - entitySize);
				return true;
			} else {
				return false;
			}
		}
	}
}

void BrisaWebserverSession::onRequest(const HttpRequest &request)
{

	timer->start(SESSION_TIMEOUT);
	if (request.httpVersion() == lastSupportedHttpVersion &&
		request.header("HOST").isNull()) {
		writeResponse(HttpResponse(lastSupportedHttpVersion, HttpResponse::BAD_REQUEST, true));
		timer->stop();
		return;
	}
	if (BrisaWebService *service = server->service(request.uri())) {
		service->postRequest(request, this);
	} else if (request.method() == "NOTIFY") {
		if (BrisaWebService *service = server->service("/")) {
			qDebug() << request.entityBody();
			if(request.entityBody()) {
				service->postRequest(request, this);
			}
		} else {
			qWarning() << "Service is NULL!";
		}

	} else {
		writeResponse(HttpResponse(request.httpVersion(), HttpResponse::NOT_FOUND, true));
		timer->stop();
	}
}

void BrisaWebserverSession::prepareResponse(const HttpRequest &req, HttpResponse &r)
{
	if (req.httpVersion().minor() == 0 || req.header("CONNECTION") == "close")
		r.setCloseConnection(true);

	QPair<qlonglong, qlonglong> range = r.range();

	if (!r.headerExists("CONTENT-TYPE"))
		r.setHeader("CONTENT-TYPE", "text/plain");

	if (useChunkedResponse) {
		r.setHeader("TRANSFER-ENCODING", "chunked");
		r.setHeader("CONTENT-LENGTH", QByteArray());
	} else {
		if (r.useRange()) {
			r.setHeader("CONTENT-LENGTH", QByteArray::number(1 + range.second
						- range.first));
		} else {
			r.setHeader("CONTENT-LENGTH", QByteArray::number(r.entitySize()));
		}
	}

	if (r.useRange()) {
		r.setHeader("CONTENT-RANGE",
					QByteArray("bytes ")
					+ QByteArray::number(range.first) + '-'
					+ QByteArray::number(range.second) + '/'
					+ QByteArray::number(r.entitySize()));
	}
}

void BrisaWebserverSession::writeEntityBody(const HttpResponse &r, QTcpSocket *s)
{
	QIODevice *body = r.entityBody();
	QPair<qlonglong, qlonglong> range = r.range();

	if (!r.useRange()) {
		range.first = 0;
		range.second = body->size() - 1;
	}
	body->seek(range.first);
	qlonglong remainingBytes = 1 + range.second - range.first;

	if (useChunkedResponse) {
		QByteArray buffer;

		while (remainingBytes) {
			buffer = body->read((MAPPED_MEMORY_SIZE > remainingBytes)
								? remainingBytes : MAPPED_MEMORY_SIZE);
			s->write(QByteArray::number(buffer.size(), 16).toUpper());
			s->write("\r\n");
			s->write(buffer);
			remainingBytes -= buffer.size();
		}

		s->write("0\r\n\r\n");
	} else {
		while (remainingBytes) {
			QByteArray buffer = body->read((MAPPED_MEMORY_SIZE > remainingBytes)
										   ? remainingBytes : MAPPED_MEMORY_SIZE);
			s->write(buffer);
			remainingBytes -= buffer.size();
		}
	}
}

void BrisaWebserverSession::sessionStarted()
{
	timer->start(SESSION_TIMEOUT);
}

bool BrisaWebserverSession::keepAlive()
{
	chunksBuffer.clear();
	return true;
}

void BrisaWebserverSession::onTimeout()
{
	writeResponse(HttpResponse(HttpVersion(1, 1), HttpResponse::REQUEST_TIMEOUT, true));
}

void BrisaWebserverSession::onSocketBytesWritten(qint64 sz)
{
	emit onSocketBytesWritten(sz, this);
}
