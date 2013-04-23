#include "connectionmanager.h"

using namespace Brisa;

#define TYPE "urn:schemas-upnp-org:service:ConnectionManager:1"
#define ID "ConnectionManager"
#define XML_PATH "/ConnectionManager/ConnectionManager1.xml"
#define CONTROL "/ConnectionManager/control"
#define EVENT_SUB "/ConnectionManager/eventSub"

ConnectionManager::ConnectionManager() :
	BrisaService(TYPE, ID, XML_PATH,
				 CONTROL, EVENT_SUB)
{
	this->setDescriptionFile("ConnectionManager1.xml");
}

BrisaOutArgument* ConnectionManager::getprotocolinfo(
	BrisaInArgument *const inArguments, BrisaAction *const action)
{
	Q_UNUSED(inArguments)
	Q_UNUSED(action)
	//TODO: everything!
	BrisaOutArgument *outArgs = new BrisaOutArgument();
	outArgs->insert("Source",
					this->getVariable("SourceProtocolInfo")->
					getAttribute(BrisaStateVariable::Value));
	outArgs->insert("Sink",
					this->getVariable("SinkProtocolInfo")->
					getAttribute(BrisaStateVariable::Value));
	return outArgs;
}

BrisaOutArgument* ConnectionManager::getcurrentconnectionids(
	BrisaInArgument *const inArguments, BrisaAction *const action)
{
	Q_UNUSED(inArguments)
	Q_UNUSED(action)
	//TODO: everything!
	BrisaOutArgument *outArgs = new BrisaOutArgument();
	outArgs->insert("ConnectionIDs", "");
	return outArgs;
}

BrisaOutArgument* ConnectionManager::getcurrentconnectioninfo(
	BrisaInArgument *const inArguments, BrisaAction *const action)
{
	Q_UNUSED(inArguments)
	Q_UNUSED(action)
	//There's an input argument called ConnectionID
	//TODO: everything!
	BrisaOutArgument *outArgs = new BrisaOutArgument();
	outArgs->insert("RcsID", "");
	outArgs->insert("AVTransportID", "");
	outArgs->insert("ProtocolInfo", "");
	outArgs->insert("PeerConnectionManager", "");
	outArgs->insert("PeerConnectionID", "");
	outArgs->insert("Direction", "");
	outArgs->insert("Status", "");
	return outArgs;
}
