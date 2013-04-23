#ifndef CONTENTDIRECTORY_H
#define CONTENTDIRECTORY_H

#include <QDirModel>

#include "brisaservice.h"

#include "pluginmanager.h"
#include "DIDL/audioitem.h"
#include "DIDL/videoitem.h"

using Brisa::BrisaService;
using Brisa::BrisaAction;
/*!
 * \internal
 *
 * \class Brisa::BrisaMediaServerAbstractPlugin
 *
 * \brief This class represents an Connection Directory entity,
 *  following the UPnP media server specs.
 */
class ContentDirectory: public BrisaService
{
	Q_OBJECT
public:
	/*!
	 * \internal
	 *
	 * Create a new instance object of this class.
	 */
	ContentDirectory();

	virtual ~ContentDirectory();
	/*!
	 * \internal
	 *
	 * .
	 */
	BrisaOutArgument* browseFile(BrisaInArgument * const arguments,
								 Container* parent);
	/*!
	 * \internal
	 *
	 * Search on Media Server for a media file.
	 */
	BrisaOutArgument* searchFile(BrisaInArgument * const arguments,
								 Container* parent);
	/*!
	 * \internal
	 *
	 * add an audio file to the media server.
	 */
	void addAudioFile(QString file, QString parentId);
	/*!
	 * \internal
	 *
	 * add and video file to be controlled by media server.
	 */
	void addVideoFile(QString file, QString parentId);
	/*!
	 * \internal
	 *
	 * Get the container, given the correct id.
	 */
	Container* getContainerById(QString id, Container *startContainer);
	/*!
	 * \internal
	 *
	 * Create a new one.
	 */
	Container* createContainer(QString id, Container *root);

public slots:

	BrisaOutArgument* browse(BrisaInArgument * const inArguments,
							 BrisaAction * const action);

	BrisaOutArgument* search(BrisaInArgument * const inArguments,
							 BrisaAction * const action);

	BrisaOutArgument* getsearchcapabilities(BrisaInArgument * const inArguments,
											BrisaAction * const action);

	BrisaOutArgument* getsortcapabilities(BrisaInArgument * const inArguments,
										  BrisaAction * const action);

	BrisaOutArgument* getsystemupdateid(BrisaInArgument * const inArguments,
										BrisaAction * const action);

signals:
	/*!
	 * \internal
	 *
	 * Signal that's emitted when a new file has been added.
	 */
	void fileAdded(QString file);
	/*!
	 * \internal
	 *
	 * Signal that's emitted when a new audio file has been added.
	 */
	void audioFileAdded(QString file);
	/*!
	 * \internal
	 *
	 * Signal that's emitted when a new video file has been added..
	 */
	void videoFileAdded(QString file);

private:

	PluginManager *pluginManager;

	Container *audioRoot;

	Container *videoRoot;

};

#endif // CONTENTDIRECTORY_H
