#ifndef BRISAMEDIASERVER_H
#define BRISAMEDIASERVER_H

#include "brisadevice.h"

#include "connectionmanager.h"
#include "contentdirectory.h"

using Brisa::BrisaDevice;
/*!
 * \internal
 *
 * \class Brisa::BrisaMediaServer
 *
 * \brief This class represents a UPnP Media Server device.
 */
class BrisaMediaServer: public BrisaDevice
{
	Q_OBJECT
public:
	/*!
	 * \internal
	 *
	 * Create a new instance object of this class.
	 */
	BrisaMediaServer();
	/*!
	 * \internal
	 *
	 * Return a reference to the Content Directory Service Instance.
	 */
	inline ContentDirectory* getContentDirectoryService() {
		return this->contentDirectoryService;
	}

public slots:
	/*!
	 * \internal
	 *
	 * Add an audio file to the media server control.
	 */
	void addAudioFile(QString file);
	/*!
	 * \internal
	 *
	 * Add an video file to the media server control.
	 */
	void addVideoFile(QString file);

private:
	//represents de content directory service
	ContentDirectory *contentDirectoryService;
};

#endif // BRISAMEDIASERVER_H
