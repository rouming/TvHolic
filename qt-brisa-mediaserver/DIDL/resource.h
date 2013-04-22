#ifndef RESOURCE_H
#define RESOURCE_H

#include <QString>
#include <QDomElement>

class Resource
{
public:
    Resource(QString value = "", QString protocol_info = "", QString import_uri = "",
             int size = -1, QString duration = "", int bitrate = -1,
             int sample_frequency = -1, int bits_per_sample = -1,
             int nr_audio_channels = -1, QString resolution = "", int color_depth = NULL,
             QString protection = "");

    QDomElement toDidlElement();
    QString toString();

private:
    QString value;
    QString protocolInfo;
    QString importUri;
    int size;
    QString duration;
    int bitrate;
    int sampleFrequency;
    int bitsPerSample;
    int nrAudioChannels;
    QString resolution;
    int colorDepth;
    QString protection;
};

#endif // RESOURCE_H
