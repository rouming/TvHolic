#ifndef __CONTENT_DIRECTORY_SERVICE_H__
#define __CONTENT_DIRECTORY_SERVICE_H__

#include <QDirModel>

#include "brisaservice.h"

#include "DIDL/audioitem.h"
#include "DIDL/videoitem.h"

using Brisa::BrisaService;
using Brisa::BrisaAction;

class ContentDirectoryService : public BrisaService
{
	Q_OBJECT
public:
	/*!
	 * \internal
	 *
	 * Create a new instance object of this class.
	 */
	ContentDirectoryService();

	virtual ~ContentDirectoryService();
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
	 * add an media file to the media server.
	 */
	void addFile(QString file, QString parentId);

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

private:
	Container* m_root;

};

#endif //__CONTENT_DIRECTORY_SERVICE_H__
