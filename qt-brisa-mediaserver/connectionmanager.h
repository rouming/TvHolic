#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "brisaservice.h"

using Brisa::BrisaService;
using Brisa::BrisaAction;
/*!
 * \internal
 *
 * \class Brisa::BrisaMediaServerAbstractPlugin
 *
 * \brief This class represents an Connection Manager entity,
 *  which must provide connection support to the Media Server.
 */
class ConnectionManager: public BrisaService
{
	Q_OBJECT
public:
	/*!
	 * \internal
	 *
	 * Create a new instance object of this class.
	 */
	ConnectionManager();

public slots:
	/*!
	 * \internal
	 *
	 * Return the info about protocols to this Server.
	 */
	BrisaOutArgument* getprotocolinfo(BrisaInArgument * const inArguments, BrisaAction * const action);
	/*!
	 * \internal
	 *
	 * Return the connection id.
	 */
	BrisaOutArgument* getcurrentconnectionids(
		BrisaInArgument * const inArguments, BrisaAction * const action);
	/*!
	 * \internal
	 *
	 * Return the current connection info.
	 */
	BrisaOutArgument* getcurrentconnectioninfo(
		BrisaInArgument * const inArguments, BrisaAction * const action);
};

#endif // CONNECTIONMANAGER_H
