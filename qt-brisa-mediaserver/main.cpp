#include <QApplication>

#include "mediaserver.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	MediaServer ms;
	ms.show();

	return a.exec();
}
