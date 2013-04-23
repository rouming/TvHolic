#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QVector>
#include "brisamediaserverabstractplugin.h"

class PluginManager : public QObject
{
	Q_OBJECT
public:
	explicit PluginManager(QObject *parent = 0);
	~PluginManager();

	inline QVector<BrisaMediaServerAbstractPlugin*> getPlugins() {
		return this->plugins;
	}

signals:

public slots:
	void setupPlugins();

private:
	QVector<BrisaMediaServerAbstractPlugin*> plugins;
};

#endif // PLUGINMANAGER_H
