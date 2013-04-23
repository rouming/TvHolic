#include "mediaserver.h"

using namespace Brisa;

MediaServer::MediaServer(QWidget *parent) :
	QWidget(parent)
{
	this->addAudioButton = new QPushButton(tr("Add Audio"), this);
	this->addVideoButton = new QPushButton(tr("Add Video"), this);
	this->audioView = new QListView(this);
	this->videoView = new QListView(this);
	this->audioModel = new QStringListModel(this);
	this->videoModel = new QStringListModel(this);
	this->audioView->setModel(audioModel);
	this->videoView->setModel(videoModel);
	lay1 = new QVBoxLayout();
	lay1->addWidget(audioView);
	lay1->addWidget(addAudioButton);
	connect(addAudioButton, SIGNAL(clicked()), this, SLOT(addAudioFiles()));
	lay2 = new QVBoxLayout();
	lay2->addWidget(videoView);
	lay2->addWidget(addVideoButton);
	connect(addVideoButton, SIGNAL(clicked()), this, SLOT(addVideoFiles()));
	mainLayout = new QHBoxLayout(this);
	mainLayout->addLayout(lay1);
	mainLayout->addLayout(lay2);
	this->ms = new BrisaMediaServer();
	ms->start();
	connect(this->ms->getContentDirectoryService(), SIGNAL(audioFileAdded(QString)),
			this, SLOT(audioFileAdded(QString)));
	connect(this->ms->getContentDirectoryService(), SIGNAL(videoFileAdded(QString)),
			this, SLOT(videoFileAdded(QString)));
}

MediaServer::~MediaServer()
{
	delete lay1;
	delete lay2;
	delete mainLayout;
	delete ms;
}

void MediaServer::addAudioFiles()
{
	QStringList files = QFileDialog::getOpenFileNames(this,
						"Select the musics you want to add...",
						"/home",
						"Audio (*.mp3 *.wav);;All files (*.*)");
	foreach (QString file, files) {
		this->ms->addAudioFile(file);
	}
}

void MediaServer::addVideoFiles()
{
	QStringList files = QFileDialog::getOpenFileNames(this,
						"Select the musics you want to add...",
						"/home",
						"Video (*.mpeg *.avi);;All files (*.*)");
	foreach (QString file, files) {
		this->ms->addVideoFile(file);
	}
}

void MediaServer::audioFileAdded(QString file)
{
	audioFiles.append(file);
	audioModel->setStringList(audioFiles);
}

void MediaServer::videoFileAdded(QString file)
{
	videoFiles.append(file);
	videoModel->setStringList(videoFiles);
}
