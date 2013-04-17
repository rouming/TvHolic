#ifndef _UPNP_PARSER_H_
#define _UPNP_PARSER_H_

//#include <QString>
//#include <QDateTime>
//#include <QByteArray>

struct UpnpOutputHeader
{
	QString contentType;
	QString acceptRanges;
	QString contentRange;
	QString contentLength;
	QString cacheControl;
	QDateTime expires;
	QString connection;
	QString transferMode;
};

class UpnpHandler
{
protected:
	void provideResponseUrl(const QStringList& methods,
							const QString& url);
};


class UpnpResourceHandler : public UpnpHandler
{
public:
	virtual void onGet(const QString& file, UpnpOutputHeader& out) = 0;
};


class UpnpConnectionManagerHandler : public UpnpHelper
{
public:
	void provideProtocolInfo();

	// For subscribtion
	void provideSinkProtocolInfo();
	void provideSourceProtocolInfo();
	void provideCurrentConnectionIDs();
};


class UpnpContentDirectoryHandler : public UpnpHelper
{
public:
	struct Input {
		QString objectID;
		QString containerID;
		QString browseFlag;
		int startingIndex;
		int requestedCount;
	};

	void provideSystemUpdateID();
	void provideFeatureList();
	void provideSortCapabilities();
	void provideSearchCapabilities();

	// For subscribtion
	void provideTransferIDs();
	void provideContainerUpdateIDs();

	virtual void onBrowse(const Input& in, QByteArray& out) = 0;
	virtual void onSearch(const Input& in, QByteArray& out) = 0;
};


class UpnpParser
{
public:
	enum Error {
		SUCCESS   = 0,
		NEED_MOAR = 1,
		NOT_UPNP  = 2
	};
	Error feed(const QByteArray&);

	void registerHandler(UpnpHandler*);
	void unregisterHandler(UpnpHandler*);
};

#endif // _UPNP_PARSER_H_
