#ifndef __MEDIAINFO_H__
#define __MEDIAINFO_H__

struct MediaInfo
{
	qint64 fileSize;
	qint32 audioBitrate;
	qint32 audioRate;
	qint32 audioChannels;
	QString videoResolution;
	QString duration;

	static bool fillMediaInfo(const QString& path, MediaInfo& mediaInfo);
};

#endif //__MEDIAINFO_H__
