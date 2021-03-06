/*
 * Universidade Federal de Campina Grande
 * Centro de Engenharia Elétrica e Informática
 * Laboratório de Sistemas Embarcados e Computação Pervasiva
 * BRisa Project / BRisa-Qt - http://brisa.garage.maemo.org
 * Filename:
 * Created:
 * Description:
 * Authors: Name <email> @since 2009
 *          Jeysibel de Sousa Dantas <jeysibel@gmail.com> @since 05/05/2010
 *
 *
 * Copyright (C) <2009> <Embbeded Systems and Pervasive Computing Laboratory>
 *
 * BRisa-Qt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtDebug>
#include <QIODevice>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include <time.h>
#include "brisanetwork.h"
#include "brisaconfig.h"

using namespace Brisa;

QBool isLoopbackIPv4Address(QString address)
{
	return QBool(!address.compare("127.0.0.1"));
}

QBool isLoopbackIPv6Address(QString address)
{
	return QBool(!address.compare("0:0:0:0:0:0:0:1"));
}

QBool isPromiscuousIPv4Address(QString address)
{
	return QBool(!address.compare("0.0.0.0"));
}

QBool isPromiscuousIPv6Address(QString address)
{
	return QBool(!address.compare("0:0:0:0:0:0:0:0") | !address.compare("::"));
}

QString getValidIP()
{
	/*#if defined(Q_OS_UNIX) || defined(Q_OS_ANDROID)
	    BrisaConfigurationManager *config = BrisaConfigurationManager::getInstance();
	    QString interfaceName = config->getParameter("network", "interface");
	    QString ip = getIp(interfaceName);
	    if (ip.isEmpty()) {
	        ip = config->getParameter("network", "ip");
	    }
	    if (ip.isEmpty()) {
	        ip = QHostAddress(QHostAddress::Any).toString();
	    }
	    return ip;
	#else*/
	QList<QNetworkInterface> nifs = QNetworkInterface::allInterfaces();
	foreach(QNetworkInterface nif, nifs) {
		QNetworkInterface::InterfaceFlags flags = nif.flags();
		if (!(flags & QNetworkInterface::IsUp &&
			  flags & QNetworkInterface::IsRunning &&
			  flags & QNetworkInterface::CanMulticast))
			continue;

		QList<QNetworkAddressEntry> aes = nif.addressEntries();
		foreach(QNetworkAddressEntry ae, aes) {
			QHostAddress addr = ae.ip();
			if (addr.protocol() != QAbstractSocket::IPv4Protocol)
				continue;
			return addr.toString();
		}
	}
	qDebug()
			<< "Couldn't acquire a non loopback IP  address,returning 127.0.0.1.";
	return "127.0.0.1";
//#endif
}

//TODO deprecated function
QString getIp(QString networkInterface)
{
	QNetworkInterface interface = QNetworkInterface::interfaceFromName(networkInterface);
	if (interface.isValid()) {
		return interface.addressEntries().first().ip().toString();
	}
	return "";
}


QBool isPortOpen(QString address, qint16 port, qint16 timeout)
{
	bool ret;
	QTcpSocket *socket = new QTcpSocket();
	socket->connectToHost(address, port);
	socket->waitForConnected(timeout);
	switch (socket->state()) {
	case QAbstractSocket::UnconnectedState:
		ret = false;
		break;

	case QAbstractSocket::ConnectingState:
		//stay waiting for some miliseconds to re-verify the state
		socket->waitForConnected(timeout);
		if (socket->state() == QAbstractSocket::ConnectedState)
			ret = true;
		else
			ret = false;
		break;

	case QAbstractSocket::ConnectedState:
		ret = true;
		break;

	default:
		ret = false;
		break;
	}

	delete socket;
	return QBool(ret);
}

quint16 getPort()
{
	srand( time(NULL));
	//Generate a port number in range [49152,65535]
	//TODO modify this expression to a more legible one
	quint16 randomPort =
		(49152 + rand()/ (RAND_MAX / (65535 - 49152 + 1) + 1));
	qDebug() <<  "Port value chosen:" << randomPort;
	while (isPortOpen(getValidIP(), randomPort, 5)) {
		qDebug() << "Port is already opened, trying another ";
		randomPort = (49152 + rand() / (RAND_MAX / (65535 - 49152 + 1) + 1));
	}
	return randomPort;
}

