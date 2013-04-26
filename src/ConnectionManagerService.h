#ifndef __CONNECTION_MANAGER_SERVICE_H__
#define __CONNECTION_MANAGER_SERVICE_H__

#include "brisaservice.h"

using Brisa::BrisaService;
using Brisa::BrisaAction;

class ConnectionManagerService : public Brisa::BrisaService
{
	Q_OBJECT
public:
	/*!
	 * \internal
	 *
	 * Create a new instance object of this class.
	 */
	ConnectionManagerService();

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

#endif //__CONNECTION_MANAGER_SERVICE_H__
