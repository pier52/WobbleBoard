/**
 * \class VideoStreamReader
 *
 *
 * \brief
 *      Reads the video stream coming from the RPi camera module and forwards it to a mediaplayer. Also has the ability to save the stream to disc.
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef VIDEOSTREAMREADER_H
#define VIDEOSTREAMREADER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <stdexcept>
#include "mplayerwrapper.h"
#include "logger.h"

class VideoStreamReader : public QObject
{
    Q_OBJECT
public:
    explicit VideoStreamReader(MPlayerWrapper* mplayer,Logger* log,QObject *parent = 0);
    bool init(quint16 port);
    bool open();
    ~VideoStreamReader();
    void stop();
    bool isConnected() const;
    void startRecording(QString file_path);
    void stopRecording();
    bool isRecording() const;

signals:

public slots:
    void close();

private:
    QTcpServer* server;
    QTcpSocket* socket;
    MPlayerWrapper* mplayer;
    Logger* log;
    quint16 port;
    QFile save_file;
    bool record = false;

private slots:
    void error(QAbstractSocket::SocketError error);
    void gotNewConnection();
    void disconnected();
    void readyRead();



};

#endif // VIDEOSTREAMREADER_H
