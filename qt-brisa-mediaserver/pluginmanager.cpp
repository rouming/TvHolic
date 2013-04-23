#include "pluginmanager.h"

PluginManager::PluginManager(QObject *parent) :
	QObject(parent)
{
}

PluginManager::~PluginManager()
{
	qDeleteAll(this->plugins);
	this->plugins.clear();
}

void PluginManager::setupPlugins()
{
}
