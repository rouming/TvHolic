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

ContentDirectoryService::ContentDirectoryService() :
	BrisaService(TYPE, ID, XML_PATH,
				 CONTROL, EVENT_SUB),
	m_root(new Container("Root"))
{
	setDescriptionFile("ContentDirectory1.xml");
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
	qDebug() << "Browsing " << id;
	Container *video = getContainerById(id, m_root);
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

BrisaOutArgument* ContentDirectoryService::search(BrisaInArgument *const inArguments, BrisaAction *const action)
{
	(void)action;

	BrisaOutArgument *outArgs = new BrisaOutArgument();
	QString id = inArguments->value(CONTAINER_ID);
	qDebug() << "Searching " << id;
	Container *container = getContainerById(id, m_root);
	QString result = "";
	if (container) {
		qDebug() << container->getId();
		result += container->toString();
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

void ContentDirectoryService::addFile(QString file, QString parentId)
{
	QStringList parentPath = parentId.split("/");
	Container *parent = getContainerById(parentPath.last(), m_root);
	if (!parent) {
		parent = createContainer(file.section("/", 0, parentPath.size() - 1), m_root);
	}
	QString title = file;
	VideoItem *video = new VideoItem(file, parentId);
	parent->addItem(video);
	getVariable("SystemUpdateID")->setAttribute(BrisaStateVariable::Value,
				getVariable("SystemUpdateID")->getAttribute(
					BrisaStateVariable::Value).toInt() + 1);
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

Container* ContentDirectoryService::createContainer(QString id, Container *root)
{
	QStack<Container*> containers;
	QStringList containersId = id.split("/");
	for (int i = containersId.size() - 1; i > 0; i--) {
		QString containerId = containersId.at(i);
		Container *container = getContainerById(containerId, root);
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
