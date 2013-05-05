#include "ContentDirectoryService.h"

using namespace Brisa;

#define TYPE "urn:schemas-upnp-org:service:ContentDirectory:1"
#define ID "ContentDirectory"
#define XML_PATH "/ContentDirectory/ContentDirectory1.xml"
#define CONTROL "/ContentDirectory/control"
#define EVENT_SUB "/ContentDirectory/eventSub"

#define CONTAINER_ID "ContainerID"
#define RESULT "Result"
#define TOTAL_MATCHES "TotalMatches"
#define NUMBER_RETURNED "NumberReturned"
#define UPDATE_ID "UpdateID"
#define OBJECT_ID "ObjectID"
#define BROWSE_FLAG "BrowseFlag"

#define VIDEO_FILES "*.avi,*.mp4,*.mkv,*.mpeg"
#define AUDIO_FILES "*.mp3"

ContentDirectoryService::ContentDirectoryService() :
	BrisaService(TYPE, ID, XML_PATH,
				 CONTROL, EVENT_SUB),
	// Root container with '0' as id
	m_root(new Container("0"))
{
	setDescriptionFile(":/resources/ContentDirectory1.xml");
}

ContentDirectoryService::~ContentDirectoryService()
{
	delete m_root;
}

BrisaOutArgument* ContentDirectoryService::getsearchcapabilities(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)inArguments;
	(void)action;

	qDebug("@@@@ getsearchcapabilities");

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	outArgs->insert("SearchCaps", "");
	return outArgs;
}

BrisaOutArgument* ContentDirectoryService::getsortcapabilities(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)inArguments;
	(void)action;

	qDebug("@@@@ getsortcapabilities");

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	outArgs->insert("SortCaps", "");
	return outArgs;
}

BrisaOutArgument* ContentDirectoryService::getsystemupdateid(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)inArguments;
	(void)action;

	qDebug("@@@@ getsystemupdateid");

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	outArgs->insert("Id",
					getVariable("SystemUpdateID")->getAttribute(BrisaStateVariable::Value));
	return outArgs;
}

BrisaOutArgument* ContentDirectoryService::browse(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)action;
	BrisaOutArgument *outArgs = new BrisaOutArgument();
	QString id = inArguments->value(OBJECT_ID);
	QString browseFlag = inArguments->value(BROWSE_FLAG);

	Container *container = getContainerById(id, m_root);
	QString result;
	int numberReturned = 0;
	int totalMatches = 0;
	if (container) {
		QDomDocument doc;

		foreach (Container *c, container->getContainers())
			result += c->toString(doc);
		foreach (Item *i, container->getItems())
			result += i->toString(doc);

		numberReturned = container->getChildCount();

		// According to upnp spec
		if (browseFlag.contains("BrowseMetadata"))
			totalMatches = 1;
		else
			totalMatches = container->getChildCount();
	}

	result.replace(">", "&gt;");
	result.replace("<", "&lt;");

	outArgs->insert(RESULT, result);
	outArgs->insert(NUMBER_RETURNED, QByteArray::number(numberReturned));
	outArgs->insert(TOTAL_MATCHES, QByteArray::number(totalMatches));
	outArgs->insert(UPDATE_ID,
					getVariable("SystemUpdateID")->getAttribute(BrisaStateVariable::Value));

	return outArgs;
}

BrisaOutArgument* ContentDirectoryService::search(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)action;

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	QString id = inArguments->value(CONTAINER_ID);
	qDebug() << "Searching " << id;
	Container *container = getContainerById(id, m_root);
	QString result = "";
	if (container) {
		QDomDocument doc;
		qDebug() << container->getId();
		result += container->toString(doc);
	}
	qDebug() << "Result:\n" << result;
	outArgs->insert(RESULT, result);
	return outArgs;
}

BrisaOutArgument* ContentDirectoryService::browseFile(BrisaInArgument *const arguments, Container *parent)
{
	(void)arguments;
	(void)parent;

	qDebug("@@@@ browseFile");

	return new BrisaOutArgument();
}

BrisaOutArgument* ContentDirectoryService::searchFile(BrisaInArgument *const arguments, Container *parent)
{
	(void)arguments;
	(void)parent;

	qDebug("@@@@ searchFile");

	return new BrisaOutArgument();
}

bool ContentDirectoryService::addPath(QString path)
{
	QDir dir(path);
	if (!dir.exists())
		return false;
	if (!dir.makeAbsolute())
		return false;

	QString absPath = dir.absolutePath();
	QStringList dirs = absPath.split(QDir::separator(),
									 QString::SkipEmptyParts);
	if (dirs.size() == 0)
		return false;

	Container *container = m_root;
	foreach (QString dir, dirs) {
		Container *c = getContainerById(dir, container);
		if (c == NULL) {
			c = new Container(dir, container->getId(), dir);
			container->addContainer(c);
		}
		container = c;
	}

/* XXX
	QStringList audioFiles = dir.entryList(
		QString(AUDIO_FILES).split(","),
		QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files,
		QDir::Time);
	foreach (QString file, audioFiles) {
		AudioItem *audio = new AudioItem(file, container->getId());
		container->addItem(audio);
		getVariable("SystemUpdateID")->setAttribute(
			BrisaStateVariable::Value,
			getVariable("SystemUpdateID")->getAttribute(
				BrisaStateVariable::Value).toInt() + 1);
	}
*/

	QStringList videoFiles = dir.entryList(
		QString(VIDEO_FILES).split(","),
		QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files,
		QDir::Time);
	foreach (QString file, videoFiles) {
		VideoItem *video = new VideoItem(file, container->getId(), file);
		//XXX
		Resource *res = new Resource("http://192.168.1.80:65500/get/0$1$33/GLD.avi",
									 "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=01",
									 "", // import uri
									 -1,  // size
									 "00:11:29:00", // duration
									 116736, // bitrate
									 48000, // sample frequency
									 -1, // bits per sample
									 2, // audio channels
									 "720x368", // resolution
									 -1, // color depth
									 "" // protection
			);
		video->addResource(res);

		container->addItem(video);
		getVariable("SystemUpdateID")->setAttribute(
			BrisaStateVariable::Value,
			getVariable("SystemUpdateID")->getAttribute(
				BrisaStateVariable::Value).toInt() + 1);
	}

	return true;
}

Container* ContentDirectoryService::getContainerById(QString id, Container *startContainer)
{
	if (startContainer->getId() == id) {
		return startContainer;
	}
	foreach (Container *c, startContainer->getContainers()) {
		if (c->getId() == id) {
			return c;
		} else {
			Container *search = getContainerById(id, c);
			if (search) {
				return search;
			}
		}
	}
	return 0;
}
