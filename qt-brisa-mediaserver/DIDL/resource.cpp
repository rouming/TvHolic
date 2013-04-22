#include "resource.h"

#include <QDebug>

Resource::Resource(QString value, QString protocolInfo, QString importUri,
                   int size, QString duration, int bitrate, int sampleFrequency,
                   int bitsPerSample, int nrAudioChannels, QString resolution,
                   int colorDepth, QString protection)
{
    this->value = value;
    this->protocolInfo = protocolInfo;
    this->importUri = importUri;
    this->size = size;
    this->duration = duration;
    this->bitrate = bitrate;
    this->sampleFrequency = sampleFrequency;
    this->bitsPerSample = bitsPerSample;
    this->nrAudioChannels = nrAudioChannels;
    this->resolution = resolution;
    this->colorDepth = colorDepth;
    this->protection = protection;
}

QDomElement Resource::toDidlElement()
{
    if (this->protocolInfo == "") {
        qWarning() << "Could not create element from this resource: protocolInfo not set (required).";
        //TODO exception?
        return QDomElement();
    }
    QDomDocument doc("resource");
    QDomElement root = doc.createElement("res");
    // Required
    root.setAttribute("protocolInfo", this->protocolInfo);

    // Optional
    if (!this->importUri.isEmpty())
        root.setAttribute("importUri", this->importUri);
    if (this->size >= 0)
        root.setAttribute("size", this->size);
    if (this->duration.isEmpty())
        root.setAttribute("size", this->size);
    if (this->bitrate >= 0)
        root.setAttribute("bitrate", this->bitrate);
    if (this->sampleFrequency >= 0)
        root.setAttribute("sampleFrequency", this->sampleFrequency);
    if (this->bitsPerSample >= 0)
        root.setAttribute("bitsPerSample", this->bitsPerSample);
    if (this->nrAudioChannels >= 0)
        root.setAttribute("nrAudioChannel", this->nrAudioChannels);
    if (!this->resolution.isEmpty())
        root.setAttribute("resolution", this->resolution);
    if (this->colorDepth >= 0)
        root.setAttribute("colorDepth", this->colorDepth);
    if (!this->protection.isEmpty())
        root.setAttribute("protection", this->protection);

    return root;
}

QString Resource::toString()
{
    QDomDocument doc;
    doc.appendChild(this->toDidlElement());
    return doc.toString();
}

