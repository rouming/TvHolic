#include "TranscoderService.h"
#include "AsIsTranscoder.h"

#include "httpresponse.h"

void TranscoderService::onRequest(const HttpRequest &request,
								  BrisaWebserverSession *session)
{
	QByteArray uri = request.uri();
	if (!uri.startsWith("/get/")) {
		qWarning("Error: transcoder service is responsible for special "
				 "get requests, uri %s is strange",
				 uri.constData());
		return;
	}
	uri.remove(0, QString("/get/").length());

	QString id = QString::fromUtf8(QByteArray::fromBase64(uri));
	QFileInfo finfo(id);
	if (!finfo.exists() || !finfo.isFile()) {
		qWarning("Error: transocoding service can't find file '%s'",
				 id.toUtf8().constData());
		return;
	}

	QMutexLocker locker(&m_mutex);

	if (m_sessions.contains(session)) {
		qWarning("Error: session is already exist, path is %s",
				 id.toUtf8().constData());
		return;
	}
	m_sessions[session] = id;

	locker.unlock();

	QObject::connect(session,
					 SIGNAL(onSocketBytesWritten(qint64, BrisaWebserverSession*)),
					 SLOT(onSocketBytesWritten(qint64, BrisaWebserverSession*)));

	HttpResponse response(request.httpVersion());

	//XXX
	response.setHeader("CONTENT-TYPE", "video/x-msvideo");

	if (request.header("ACCEPT-RANGES") == "bytes")
		response.setHeader("ACCEPT-RANGES", "bytes");

	response.setEntitySize(finfo.size());

	session->respond(response, false);
}

void TranscoderService::onSocketBytesWritten(
	qint64,
	BrisaWebserverSession* session)
{
	if (session->getSocket()->bytesToWrite())
		return;

	QObject::disconnect(session, 0, this, 0);

	QMutexLocker locker(&m_mutex);

	QFileInfo finfo(m_sessions.take(session));

	if (!finfo.exists() || !finfo.isFile()) {
		qWarning("Error: session does not exist or file does not exist");
		return;
	}

	locker.unlock();

	QTcpSocket *sock = session->getSocket();

	Transcoder *transcoder = new AsIsTranscoder(*sock);
	QObject::connect(sock,
					 SIGNAL(disconnected()),
					 transcoder,
					 SLOT(deleteLater()));

	transcoder->startTranscoding(finfo.absoluteFilePath());
}
