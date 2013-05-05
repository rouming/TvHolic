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

#include <QTcpSocket>
#include <QBuffer>

#include "httpsession.h"
#include "httpsessionmanager.h"

#define DBG_PREFIX "HttpConnection: "

#ifndef MAPPED_MEMORY_SIZE
#define MAPPED_MEMORY_SIZE 64
#endif

using namespace Brisa;

enum State {
	WAITING_FOR_REQUEST_LINE,
	WAITING_FOR_HEADERS,
	WAITING_FOR_ENTITY_BODY
};

//#define DUMP_NETWORK

enum DumpDir {
	DUMP_OUT = 0,
	DUMP_IN  = 1
};

void dump(const QByteArray& ba, QTcpSocket* sock, DumpDir dir)
{
	if (dir == DUMP_OUT)
		qDebug("<<<<<<<<<<<<<< OUT (%s:%u) <<<<<<<<<<<<<<<<<",
			   qPrintable(sock->peerAddress().toString()),
			   sock->peerPort());
	else
		qDebug(">>>>>>>>>>>>>>  IN (%s:%u) >>>>>>>>>>>>>>>>",
			   qPrintable(sock->peerAddress().toString()),
			   sock->peerPort());
	qDebug("%s", ba.constData());
}

HttpSession::HttpSession(HttpSessionManager *sessionManager, QObject *parent) :
	QObject(parent),
	lastSupportedHttpVersion(1, 1),
	sessionManager(sessionManager),
	socket(new QTcpSocket()),
	state(WAITING_FOR_REQUEST_LINE)
{
	connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(onConnectionDie()));
}

HttpSession::~HttpSession()
{
	delete socket;
}

void HttpSession::setSession(int socketDescriptor)
{
	socket->setSocketDescriptor(socketDescriptor);
	sessionStarted();
}

int HttpSession::isRequestSupported(const HttpRequest &request) const
{
	if (request.httpVersion() <= lastSupportedHttpVersion) {  //1.1
		return 0;
	} else {
		return HttpResponse::HTTP_VERSION_NOT_SUPPORTED;
	}
}

void HttpSession::writeResponse(HttpResponse r)
{
	QByteArray ba;
	QBuffer b(&ba);
	b.open(QBuffer::ReadWrite);

	prepareResponse(requestInfo, r);

	b.write(r.httpVersion().operator QByteArray());
	b.write(" ");
	b.write(QByteArray::number(r.statusCode()));
	b.write(" ");
	b.write(r.reasonPhrase());
	b.write("\r\n");

	for (QHash<QByteArray, QByteArray>::const_iterator i = r.headersBeginIterator(); i != r.headersEndIterator(); ++i) {
		b.write(i.key());
		if (!i.value().isNull()) {
			b.write(": ");
			b.write(i.value());
		} else {
			b.write(":");
		}
		b.write("\r\n");
	}

	b.write("\r\n");

	if (r.entityBody())
		writeEntityBody(r, &b);

	socket->write(ba);

#ifdef DUMP_NETWORK
	dump(ba, socket, DUMP_OUT);
#endif

	if (r.closeConnection())
		socket->close();
}

void HttpSession::prepareResponse(const HttpRequest &, HttpResponse &r)
{
	r.setHeader("CONTENT-LENGTH", QByteArray::number(r.entitySize()));
}

void HttpSession::writeEntityBody(const HttpResponse &r, QIODevice *io)
{
	QIODevice *body = r.entityBody();

	body->seek(0);
	while (!body->atEnd())
		io->write(body->read(MAPPED_MEMORY_SIZE));
}

void HttpSession::onReadyRead()
{
	QByteArray ba = socket->readAll();
#ifdef DUMP_NETWORK
	dump(ba, socket, DUMP_IN);
#endif
	buffer.append(ba);


	try {
		switch (state) {
		case WAITING_FOR_REQUEST_LINE: {

			while (buffer.startsWith("\r\n"))
				buffer.remove(0, 2);


			int i = buffer.indexOf("\r\n");
			if (i != -1) {

				QList<QByteArray> request = buffer.left(i).simplified().split(' ');
				buffer.remove(0, i + 2);

				if (request.size() != 3) {
					writeResponse(HttpResponse(lastSupportedHttpVersion, HttpResponse::BAD_REQUEST, true));
					return;
				}

				requestInfo.setMethod(request.at(0));
				requestInfo.setUri(request.at(1));

				{
					HttpVersion version(request.at(2));

					if (version) {
						requestInfo.setHttpVersion(version);
					} else {
						writeResponse(HttpResponse(lastSupportedHttpVersion, HttpResponse::BAD_REQUEST, true));
						return;
					}
				}

				{
					int statusCode = isRequestSupported(requestInfo);
					//qDebug() << "statusCode = isRequestSupported(requestInfo) = " << statusCode;
					if (statusCode) {
						writeResponse(HttpResponse(lastSupportedHttpVersion, statusCode, true));
						return;
					}
				}

				state = WAITING_FOR_HEADERS;

			} else {
				break;
			}
		}
		case WAITING_FOR_HEADERS: {

			for (int i = buffer.indexOf("\r\n") ; i != -1 ; i = buffer.indexOf("\r\n")) {
				// don't starts with \r\n
				if (i != 0) {
					QByteArray header = buffer.left(i);
					buffer.remove(0, i + 2);

					i = header.indexOf(':');
					if (i > 0) {
						if (i + 1 < header.size())
							requestInfo.setHeader(header.left(i).trimmed(), header.mid(i + 1).trimmed());
						else
							requestInfo.setHeader(header.left(i).trimmed(), QByteArray());
					} else {
						writeResponse(HttpResponse(requestInfo.httpVersion() < lastSupportedHttpVersion ?
												   requestInfo.httpVersion() : lastSupportedHttpVersion,
												   HttpResponse::BAD_REQUEST, true));
						return;
					}
				} else {
					buffer.remove(0, 2);

					if (hasEntityBody(requestInfo)) {

						state = WAITING_FOR_ENTITY_BODY;


					} else {

						state = WAITING_FOR_REQUEST_LINE;
						onRequest(requestInfo);
						requestInfo.clear();
					}
				}
			}

			if (state != WAITING_FOR_ENTITY_BODY)
				break;
		}
		case WAITING_FOR_ENTITY_BODY:

			if (atEnd(requestInfo, buffer)) {

				requestInfo.setEntityBody(buffer);
				buffer.clear();
				state = WAITING_FOR_REQUEST_LINE;

				onRequest(requestInfo);
				requestInfo.clear();
			}

		default:
			break;

		}

	} catch (HttpResponse &r) {
		r.setCloseConnection(true);
		writeResponse(r);
	}
}

void HttpSession::onConnectionDie()
{
	requestInfo.clear();
	buffer.clear();
	state = WAITING_FOR_REQUEST_LINE;

	sessionManager->releaseSession(this);
}
