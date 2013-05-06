#include <QProcess>
#include <QFileInfo>

#include "MediaInfo.h"

#define MPLAYER_INDENTIFY "mplayer -vo null -ao null -frames 0 -identify %1"

bool MediaInfo::fillMediaInfo(const QString& path, MediaInfo& mediaInfo)
{
	QFileInfo file(path);
	QString width, height, len;
	QProcess ident;
	bool ok = true;
	bool audioEnc = false;

	QRegExp videoWidthRx("ID_VIDEO_WIDTH\\s*=\\s*(\\d+)");
	QRegExp videoHeightRx("ID_VIDEO_HEIGHT\\s*=\\s*(\\d+)");
	QRegExp lengthRx("ID_LENGTH\\s*=\\s*([\\d.]+)");
	QRegExp audioBitrateRx("ID_AUDIO_BITRATE\\s*=\\s*(\\d+)");
	QRegExp audioRateRx("ID_AUDIO_RATE\\s*=\\s*(\\d+)");
	QRegExp audioChannelsRx("ID_AUDIO_NCH\\s*=\\s*(\\d+)");

	QString audioEncLine("Opening audio decoder");

	if (!file.isFile())
		return false;

     ident.start(QString(MPLAYER_INDENTIFY).arg(file.absoluteFilePath()));
     if (!ident.waitForStarted())
         return false;

     ident.closeWriteChannel();
     if (!ident.waitForFinished())
         return false;

	 mediaInfo.fileSize = file.size();

	 mediaInfo.audioBitrate = -1;
	 mediaInfo.audioRate = -1;
	 mediaInfo.audioChannels = -1;

	 while (ident.canReadLine()) {
		 QString line = ident.readLine();

		 if (-1 != videoWidthRx.indexIn(line))
			 width = videoWidthRx.cap(1);
		 else if (-1 != videoHeightRx.indexIn(line))
			 height = videoHeightRx.cap(1);
		 else if (-1 != lengthRx.indexIn(line))
			 len = lengthRx.cap(1);
		 else if (!audioEnc && line.contains(audioEncLine))
			 audioEnc = true;
		 else if (audioEnc && -1 != audioBitrateRx.indexIn(line))
			 mediaInfo.audioBitrate = audioBitrateRx.cap(1).toInt(&ok);
		 else if (audioEnc && -1 != audioRateRx.indexIn(line))
			 mediaInfo.audioRate = audioRateRx.cap(1).toInt(&ok);
		 else if (audioEnc && -1 != audioChannelsRx.indexIn(line))
			 mediaInfo.audioChannels = audioChannelsRx.cap(1).toInt(&ok);

		 if (!ok)
			 return false;
	 }

	 if (!width.isEmpty() && !height.isEmpty())
		 mediaInfo.videoResolution = QString("%1x%2").arg(width).arg(height);
	 if (!len.isEmpty()) {
		 int secs = 0, msecs = 0;
		 QStringList lens = len.split(".");
		 if (!lens.size())
			 return false;
		 if (lens.size() > 0)
			 secs = lens[0].toInt(&ok);
		 if (lens.size() > 1)
			 msecs = lens[1].toInt(&ok);
		 if (!ok)
			 return false;

		 mediaInfo.duration = QString("%1:%2:%3:%4").
			 arg(secs / 3600,    2, 10, QChar('0')).
			 arg(secs / 60 % 60, 2, 10, QChar('0')).
			 arg(secs % 60,      2, 10, QChar('0')).
			 arg(msecs,          2, 10, QChar('0'));
	 }

	 return true;
}
