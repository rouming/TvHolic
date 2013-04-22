#ifndef MEDIASERVER_H
#define MEDIASERVER_H

#include <QFileDialog>
#include <QWidget>
#include <QPushButton>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringListModel>

#include "brisamediaserver.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringListModel>
/*!
 * \internal
 *
 * \class Brisa::MediaServer
 *
 * \brief This class represents an Connection Directory entity,
 *  following the UPnP media server specs.
 */
class MediaServer : public QWidget
{
    Q_OBJECT

public:
    explicit MediaServer(QWidget *parent = 0);
    ~MediaServer();

public slots:
    void addAudioFiles();
    void addVideoFiles();
    void audioFileAdded(QString file);
    void videoFileAdded(QString file);

private:
    BrisaMediaServer *ms;
    QPushButton *addAudioButton;
    QPushButton *addVideoButton;
    QListView *audioView;
    QListView *videoView;
    QVBoxLayout *lay1;
    QVBoxLayout *lay2;
    QHBoxLayout *mainLayout;
    QStringListModel *audioModel;
    QStringListModel *videoModel;
    QStringList audioFiles;
    QStringList videoFiles;
};

#endif // MEDIASERVER_H
