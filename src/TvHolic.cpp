#include <stdio.h>
#include <unistd.h>

#include <signal.h>
#include <fcntl.h>

#include <QCoreApplication>
#include <QSocketNotifier>
#include "TvHolic.h"

using namespace Brisa;

#define STR(str) #str

#define TYPE              "urn:schemas-upnp-org:device:MediaServer:1"
#define FRIENDLY_NAME     "TvHolic"
#define MANUFACTURER      "Home Labroratory Inc."
#define MANUFACTURER_URL  ""
#define MODEL_DESCRIPTION "An UPnP/DLNA Media Server Device"
#define MODEL_NAME        "Media Server Device"
#define MODEL_NUMBER      STR(TVHOLIC_VERSION)
#define MODEL_URL         ""
#define SERIAL_NUMBER     STR(TVHOLIC_VERSION)

struct SigHandle
{
	QSocketNotifier* notif;
	int pipes[2];
	struct sigaction sa_old_sigterm_handler;
	struct sigaction sa_old_sigint_handler;

} s_sig;

static void sig_handler(int signum, void (*old_handler)(int))
{
	if (s_sig.notif)
		::write(s_sig.pipes[1], "", 1);

	if (old_handler && old_handler != SIG_IGN)
		old_handler(signum);
}

static void sigterm_handler(int signum)
{
	sig_handler(signum, s_sig.sa_old_sigterm_handler.sa_handler);
}

static void sigint_handler(int signum)
{
	sig_handler(signum, s_sig.sa_old_sigint_handler.sa_handler);
}

static void initSIGTERM()
{
	if (::pipe(s_sig.pipes) < 0)
		abort();

	::fcntl(s_sig.pipes[0], F_SETFD, FD_CLOEXEC);
	::fcntl(s_sig.pipes[1], F_SETFD, FD_CLOEXEC);
	::fcntl(s_sig.pipes[0], F_SETFL, ::fcntl(s_sig.pipes[0], F_GETFL) | O_NONBLOCK);
	::fcntl(s_sig.pipes[1], F_SETFL, ::fcntl(s_sig.pipes[1], F_GETFL) | O_NONBLOCK);

	s_sig.notif = new QSocketNotifier(s_sig.pipes[0], QSocketNotifier::Read);

	bool r = QObject::connect(s_sig.notif,
							  SIGNAL(activated(int)),
							  QCoreApplication::instance(),
							  SLOT(quit()));
	if (!r)
		abort();

	struct sigaction action;

	memset(&action, 0, sizeof(action));
	action.sa_handler = sigterm_handler;
	if (::sigaction(SIGTERM, &action, &s_sig.sa_old_sigterm_handler) < 0)
		abort();
	memset(&action, 0, sizeof(action));
	action.sa_handler = sigint_handler;
	if (::sigaction(SIGINT, &action, &s_sig.sa_old_sigint_handler) < 0)
		abort();
}

TvHolic::TvHolic() :
	BrisaDevice(TYPE,FRIENDLY_NAME, MANUFACTURER,
				MANUFACTURER_URL, MODEL_DESCRIPTION,
				MODEL_NAME, MODEL_NUMBER, MODEL_URL,
				SERIAL_NUMBER, getCompleteUuid()),
	m_contDirService(new ContentDirectoryService(getAttribute(BrisaDevice::UrlBase))),
	m_connMngService(new ConnectionManagerService),
	m_transcoderService(new TranscoderService)
{
	addService(m_connMngService);
	addService(m_contDirService);
	getWebserver()->addService(QRegExp("/get/.*"), m_transcoderService);
}

TvHolic::~TvHolic()
{
	stop();
}

void TvHolic::addPath(const QString& path)
{
	m_contDirService->addPath(path);
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("Usage: <path to some folder with media files>\n");
		return -1;
	}

	QCoreApplication app(argc, argv);
	TvHolic tvHolic;

	initSIGTERM();

	tvHolic.addPath(argv[1]);
	tvHolic.start();

	return app.exec();
}
