/**
 * \class RPiCamComm
 *
 *
 * \brief
 *      Implements communication with the Rasberry Pi carrying the cameras
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef RPICAMCOMM_H
#define RPICAMCOMM_H

#include <QObject>
#include "tcpclient.h"
#include "videostreamreader.h"
#include "mplayerwrapper.h"
#include "logger.h"
#include "networkpinger.h"

class RPiCamComm : public QObject
{
    Q_OBJECT
public:
    explicit RPiCamComm(Logger* log,QObject *parent = 0);
    ~RPiCamComm();
    void init(quint16 port, QString host);
    void startStream(unsigned port);
    bool toggleRecording(QString file_path="");
    void snapPhoto();

    bool isConnected() const;
    bool isRecording() const;
    void close();
    void savePhoto(QString path);

signals:
    void streamClosed();
    void wifiStrength(int strength);
    void plot(double value, QString graph);
    void addGraph(QString name, QString x_axis, QString y_axis);
    void removeGraph(QString name);


public slots:
    void stopStream();
    void setCamSettings(quint32 bitrate, quint16 width, quint16 height);
    void setCameraPhi(double phi);
    void setCameraTheta(double theta);
    void ping(bool do_it, QString ip);
    void requestWifiStrength(bool start);

private:
    // Stops video stream without emitting close signals
    void stopStreamSilent();

    // Starts stream without writing to logs
    void startStreamSilent(unsigned port);

    /// Restarts the videostream without emitting close signals and without writing to logs.
    void restartStream();
    TCPClient* tcp_client;
    VideoStreamReader* stream_reader;
    MPlayerWrapper* mplayer;
    Logger* log;
    NetworkPinger* pinger;
    bool is_streaming = false;
    unsigned port;

private slots:
    void mplayerClose();
    void pingResponse(double);
    void relaySignalStrength(int strenght);
};

#endif // RPICAMCOMM_H
