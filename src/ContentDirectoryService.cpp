#include "ContentDirectoryService.h"
#include "MediaInfo.h"

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

#define ROOT_ID "0"
#define VIDEO_FILES ".*\\.(avi|mp4|mkv|mpeg)"
#define AUDIO_FILES ".*\\.(mp3)"

ContentDirectoryService::ContentDirectoryService(const QString &urlBase) :
	BrisaService(TYPE, ID, XML_PATH,
				 CONTROL, EVENT_SUB),
	m_urlBase(urlBase)
{
	setDescriptionFile(":/resources/ContentDirectory1.xml");
}

ContentDirectoryService::~ContentDirectoryService()
{
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

bool ContentDirectoryService::fillContainer(Container *&container,
											const QString &dir,
											const QStringList &children)
{
	if (dir == ROOT_ID)
		container = new Container(ROOT_ID);
	else {
		QFileInfo finfo(dir);
		Q_ASSERT(finfo.isDir());

		QString path = finfo.absolutePath();
		QString fileName = finfo.fileName();
		QString filePath = finfo.absoluteFilePath();

		// Parent root path?
		if (m_rootPaths.contains(path))
			path = ROOT_ID;

		container = new Container(filePath.toUtf8().toBase64(),
								  path.toUtf8().toBase64(),
								  fileName);
	}

	foreach (QString chPath, children) {
		QFileInfo finfo(chPath);
		QString path = finfo.absolutePath();
		QString fileName = finfo.fileName();
		QString filePath = finfo.absoluteFilePath();

		if (finfo.isDir())
			container->addChild(new Container(filePath.toUtf8().toBase64(),
											  path.toUtf8().toBase64(),
											  fileName));
		else if (finfo.isFile()) {
			if (fileName.contains(QRegExp(VIDEO_FILES))) {
				MediaInfo mi;
				if (!MediaInfo::fillMediaInfo(filePath, mi))
					qWarning("Error: can't fill media info for file %s",
							 filePath.toUtf8().constData());

				VideoItem *item = new VideoItem(filePath.toUtf8().toBase64(),
												path.toUtf8().toBase64(),
												fileName);
				Resource *res =
					new Resource(QString("%1/get/%2").
									 arg(m_urlBase).arg(item->getId()),
								 "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=01",
								 "", // import uri
								 mi.fileSize,
								 mi.duration,
								 mi.audioBitrate,
								 mi.audioRate,
								 -1, // bits per sample
								 mi.audioChannels,
								 mi.videoResolution,
								 -1, // color depth
								 "" // protection
						);
				item->addResource(res);
				container->addChild(item);
			}
			else if (fileName.contains(QRegExp(AUDIO_FILES))) {
				//XXX TODO:
				continue;
			}
			else
				continue;
		}
	}

	return true;
}

BrisaOutArgument* ContentDirectoryService::browse(BrisaInArgument *const inArgs,
												  BrisaAction *const action)
{
	(void)action;
	QString result;
	int numberReturned = 0;
	int totalMatches = 0;
	BrisaOutArgument *outArgs = new BrisaOutArgument();
	QString id = inArgs->value(OBJECT_ID);
	bool browseChildren = inArgs->value(BROWSE_FLAG).contains("BrowseDirectChildren");
	QStringList children;
	Container *container = NULL;
	bool res;

	// Root folder?
	if (id == ROOT_ID) {
		children = m_rootPaths;
	}
	else {
		id = QString::fromUtf8(QByteArray::fromBase64(id.toUtf8()));

		QFileInfo finfo(id);

		if (!finfo.exists()) {
			qWarning("Error: id %s does not exist", id.toUtf8().constData());
			goto out;
		}
		if (browseChildren && !finfo.isDir()) {
			qWarning("Error: id %s is not a dir, but browse children flag is "
					 "specified", id.toUtf8().constData());
			goto out;
		}

		if (finfo.isFile()) {
			Q_ASSERT(!browseChildren);
			children << finfo.absoluteFilePath();
			// Get parent path
			id = finfo.absolutePath();
		}
		else if (finfo.isDir()) {
			children = finfo.dir().entryList(
				QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files,
				QDir::Time);
		}
		else {
			qWarning("Error: unknown id type %s", id.toUtf8().constData());
			goto out;
		}
	}

	res = fillContainer(container, id, children);
	if (!res) {
		qWarning("Error: filling container failed");
		goto out;
	}

	foreach (DidlObject *c, container->getChildren()) {
		QDomDocument doc;
		result += c->toString(doc);
	}

	numberReturned = container->getChildCount();
	//TODO: for now we assume total matches == number returned
	totalMatches = container->getChildCount();

	if (!browseChildren) {
		Q_ASSERT(numberReturned == 0 || numberReturned == 1);
		Q_ASSERT(totalMatches == 0 || totalMatches == 1);
	}

out:
	delete container;

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
	QString result = "";
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
	if (m_rootPaths.contains(absPath))
		return false;
	m_rootPaths.append(absPath);
	return true;
}
