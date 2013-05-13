#ifndef __TRANSCODER_SERVICE_H__
#define __TRANSCODER_SERVICE_H__

#include "httprequest.h"
#include "brisawebservice.h"
#include "brisawebserversession.h"

using namespace Brisa;

class TranscoderService : public BrisaWebService
{
	Q_OBJECT
public:

protected:
	void onRequest(const HttpRequest &request,
				   BrisaWebserverSession *session);

private slots:
	void onSocketBytesWritten(qint64, BrisaWebserverSession*);

private:
	QMutex m_mutex;
	QHash<BrisaWebserverSession*, QString> m_sessions;
};

#endif //__TRANSCODER_SERVICE_H__
