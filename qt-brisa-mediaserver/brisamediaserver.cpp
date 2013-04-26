#include "brisamediaserver.h"
#include "brisawebfile.h"

using namespace Brisa;

#define TYPE              "urn:schemas-upnp-org:device:MediaServer:1"
#define FRIENDLY_NAME     "Brisa Media Server"
#define MANUFACTURER      "Brisa Team. Embedded Laboratory and INdT Brazil"
#define MANUFACTURER_URL  "https://garage.maemo.org/projects/brisa"
#define MODEL_DESCRIPTION "An UPnP Media Server Device"
#define MODEL_NAME        "Media Server Device"
#define MODEL_NUMBER      "1.0"
#define MODEL_URL         "https://garage.maemo.org/projects/brisa"
#define SERIAL_NUMBER     "1.0"

BrisaMediaServer::BrisaMediaServer() :
	BrisaDevice(TYPE,FRIENDLY_NAME, MANUFACTURER,
				MANUFACTURER_URL, MODEL_DESCRIPTION,
				MODEL_NAME, MODEL_NUMBER, MODEL_URL,
				SERIAL_NUMBER, getCompleteUuid())
{
	this->addService(new ConnectionManager());
	this->addService(new ContentDirectory());
	this->contentDirectoryService = dynamic_cast<ContentDirectory *>(this->getServiceById("ContentDirectory"));
}

void BrisaMediaServer::addAudioFile(QString file)
{

	QString parentId = file.section("/", 0, file.count("/") - 1);
	this->contentDirectoryService->addAudioFile(file, parentId);
	this->getWebserver()->addService(file.toUtf8(), new BrisaWebFile(file));
	qDebug() << this->getAttribute(BrisaDevice::UrlBase);
}

void BrisaMediaServer::addVideoFile(QString file)
{
	QString parentId = file.section("/", 0, file.count("/") - 1);
	this->contentDirectoryService->addVideoFile(file, parentId);
	this->getWebserver()->addService(file.toUtf8(), new BrisaWebFile(file));
	qDebug() << this->getAttribute(BrisaDevice::UrlBase);
}
