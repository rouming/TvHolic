#ifndef BRISAMEDIASERVERABSTRACTPLUGIN_H
#define BRISAMEDIASERVERABSTRACTPLUGIN_H

#include <QObject>

#include "brisaaction.h"

using Brisa::BrisaAction;
/*!
 * \internal
 *
 * \class Brisa::BrisaMediaServerAbstractPlugin
 *
 * \brief This class represents an abstract plugin basis.
 */
class BrisaMediaServerAbstractPlugin: public QObject
{
	Q_OBJECT
public:
	/*!
	 * \internal
	 *
	 * Create a new instance object of this class.
	 */
	explicit BrisaMediaServerAbstractPlugin(QObject *parent = 0);

signals:

public slots:
	/*!
	 * \internal
	 *
	 * Implement the Browse method described by UPnP specification.
	 */
	virtual BrisaOutArgument* browse(BrisaInArgument * const inArguments,
									 BrisaAction * const action) = 0;
	/*!
	 * \internal
	 *
	 * Implement the Search method described by UPnP specification.
	 */
	virtual BrisaOutArgument* search(BrisaInArgument * const inArguments,
									 BrisaAction * const action) = 0;

};

#endif // BRISAMEDIASERVERABSTRACTPLUGIN_H
