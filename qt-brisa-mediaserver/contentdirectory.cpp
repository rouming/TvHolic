#include "contentdirectory.h"

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

ContentDirectory::ContentDirectory() :
	BrisaService(TYPE, ID, XML_PATH,
				 CONTROL, EVENT_SUB)
{
	this->setDescriptionFile("ContentDirectory1.xml");
	this->audioRoot = new Container("AudioRoot");
	this->videoRoot = new Container("VideoRoots");
	this->pluginManager = new PluginManager();
	this->pluginManager->setupPlugins();
}

ContentDirectory::~ContentDirectory()
{
	delete this->audioRoot;
	delete this->videoRoot;
	delete this->pluginManager;
}

BrisaOutArgument* ContentDirectory::getsearchcapabilities(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)inArguments;
	(void)action;

	qDebug("@@@@ getsearchcapabilities");

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	outArgs->insert("SearchCaps", "");
	return outArgs;
}

BrisaOutArgument* ContentDirectory::getsortcapabilities(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)inArguments;
	(void)action;

	qDebug("@@@@ getsortcapabilities");

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	outArgs->insert("SortCaps", "");
	return outArgs;
}

BrisaOutArgument* ContentDirectory::getsystemupdateid(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)inArguments;
	(void)action;

	qDebug("@@@@ getsystemupdateid");

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	outArgs->insert("Id",
					this->getVariable("SystemUpdateID")->
					getAttribute(BrisaStateVariable::Value));
	return outArgs;
}

BrisaOutArgument* ContentDirectory::browse(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)action;

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	QString id = inArguments->value(OBJECT_ID);
	qDebug() << "Browsing " << id;
	Container *video = this->getContainerById(id, videoRoot);
	QString result = "";
	int numberReturned = 0;
	int totalMatches = 0;
	if (video) {
		qDebug() << video->getId();
		result += video->toString();
		numberReturned += video->getChildCount();
		totalMatches += video->getChildCount();
	}
	qDebug() << "Result:\n" << result;
	outArgs->insert(RESULT, result);
	outArgs->insert(NUMBER_RETURNED, QByteArray::number(numberReturned));
	outArgs->insert(TOTAL_MATCHES, QByteArray::number(totalMatches));
	outArgs->insert(UPDATE_ID,
					getVariable("SystemUpdateID")->getAttribute(BrisaStateVariable::Value));

	return outArgs;
}

BrisaOutArgument* ContentDirectory::search(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)action;

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	QString id = inArguments->value(CONTAINER_ID);
	qDebug() << "Searching " << id;
	Container *audio = this->getContainerById(id, audioRoot);
	Container *video = this->getContainerById(id, videoRoot);
	QString result = "";
	if (audio) {
		qDebug() << audio->getId();
		result += audio->toString();
	}
	if (video) {
		qDebug() << video->getId();
		result += video->toString();
	}
	qDebug() << "Result:\n" << result;
	outArgs->insert(RESULT, result);
	return outArgs;
}

BrisaOutArgument* ContentDirectory::browseFile(BrisaInArgument *const arguments, Container *parent)
{
	(void)arguments;
	(void)parent;

	qDebug("@@@@ browseFile");

	return new BrisaOutArgument();
}

BrisaOutArgument* ContentDirectory::searchFile(BrisaInArgument *const arguments, Container *parent)
{
	(void)arguments;
	(void)parent;

	qDebug("@@@@ searchFile");

	return new BrisaOutArgument();
}

void ContentDirectory::addAudioFile(QString file, QString parentId)
{
	QStringList parentPath = parentId.split("/");
	Container *parent = getContainerById(parentPath.last(), audioRoot);
	if (!parent) {
		parent = this->createContainer(file.section("/", 0, parentPath.size() - 1), this->audioRoot);
	}
	QString title = file;
	AudioItem *audio = new AudioItem(file, parentId);
	parent->addItem(audio);
	this->getVariable("SystemUpdateID")->
	setAttribute(BrisaStateVariable::Value,
				 this->getVariable("SystemUpdateID")->
				 getAttribute(BrisaStateVariable::Value).toInt() + 1);
	emit audioFileAdded(file);
	emit fileAdded(file);
}

void ContentDirectory::addVideoFile(QString file, QString parentId)
{
	QStringList parentPath = parentId.split("/");
	Container *parent = getContainerById(parentPath.last(), audioRoot);
	if (!parent) {
		parent = this->createContainer(file.section("/", 0, parentPath.size() - 1), this->videoRoot);
	}
	QString title = file;
	VideoItem *video = new VideoItem(file, parentId);
	parent->addItem(video);
	this->getVariable("SystemUpdateID")->
	setAttribute(BrisaStateVariable::Value,
				 this->getVariable("SystemUpdateID")->
				 getAttribute(BrisaStateVariable::Value).toInt() + 1);
	emit videoFileAdded(file);
	emit fileAdded(file);
}

Container* ContentDirectory::getContainerById(QString id, Container *startContainer)
{
	if (startContainer->getId() == id) {
		return startContainer;
	}
	foreach (Container *c, startContainer->getContainers()) {
		if (c->getId() == id) {
			return c;
		} else {
			Container *search = this->getContainerById(id, c);
			if (search) {
				return search;
			}
		}
	}
	return 0;
}

Container* ContentDirectory::createContainer(QString id, Container *root)
{
	QStack<Container*> containers;
	QStringList containersId = id.split("/");
	for (int i = containersId.size() - 1; i > 0; i--) {
		QString containerId = containersId.at(i);
		Container *container = this->getContainerById(containerId, root);
		if (!container) {
			containers.push(new Container(containerId));
		} else {
			containers.push(container);
			break;
		}
	}
	root->addContainer(containers.top());
	while (containers.size() > 1) {
		Container *parent = containers.pop();
		parent->addContainer(containers.top());
	}
	Container *created = containers.top();
	qDebug() << "Created: " << created->getId();
	return created;
}
