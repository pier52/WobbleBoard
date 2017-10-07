#include "rpicamcomm.h"

RPiCamComm::RPiCamComm(Logger* log,QObject *parent) : QObject(parent)
{
    tcp_client = new TCPClient(log);
    mplayer = new MPlayerWrapper(log);
    stream_reader = new VideoStreamReader(mplayer,log);
    pinger = new NetworkPinger(log);
    this->log = log;

    connect(mplayer, SIGNAL(windowClosed()), this, SLOT(mplayerClose()));
    connect(tcp_client,SIGNAL(newSignalStrength(int)),this,SLOT(relaySignalStrength(int)));
    connect(pinger,SIGNAL(response(double)),this,SLOT(pingResponse(double)));

    log->addDataLog("RPi WiFi RSSI","%",log_type::type_int);
    log->addDataLog("RPi ping","ms",log_type::type_float);
}

RPiCamComm::~RPiCamComm()
{
    delete tcp_client;
    pinger->deleteLater();
}

void RPiCamComm::init(quint16 port, QString host)
{
    tcp_client->init(port,host);

    //Send handshake
    tcp_client->sendCommand("Hello");
}

void RPiCamComm::startStream(unsigned port)
{
    this->port = port;
    stream_reader->init(port);
    tcp_client->sendCommand("start");
    log->info("Video stream started");
    is_streaming = true;
}

void RPiCamComm::mplayerClose()
{
    if(stream_reader->isConnected())
    {
        tcp_client->sendCommand("stop");
        stream_reader->close();
        emit streamClosed();
        log->info("Video stream stopped");
        is_streaming = false;
    }

}

void RPiCamComm::stopStream()
{
    mplayer->stop();
    is_streaming = false;
}

void RPiCamComm::setCamSettings(quint32 bitrate, quint16 width, quint16 height)
{
    if(!isConnected())
        return;

    QString bitrate_s = QString::number(bitrate);
    QString width_s = QString::number(width);
    QString height_s = QString::number(height);

    QJsonObject send_object;
    send_object["msg"] = "video_settings";
    send_object["bitrate"] = bitrate_s;
    send_object["width"] = width_s;
    send_object["height"] = height_s;

    try
    {
        tcp_client->send(send_object);
    }
    catch(const std::exception& e)
    {
        log->error("Could not send cam settings: " + QString::fromStdString(e.what()));
        return;
    }

    if(is_streaming) // restart stream for new settings to take effect
    {
        try
        {
            tcp_client->sendCommand("stop");
            stream_reader->close();

            stream_reader->open();
            tcp_client->sendCommand("start");
        }
        catch(const std::exception& e)
        {
            log->error("Could not restart cam module: " + QString::fromStdString(e.what()));
            return;
        }
    }

    log->info("Changed camera settings: \n bitrate: " + bitrate_s + "\n width: " + width_s + "\n height: " + height_s);
}

void RPiCamComm::setCameraTheta(double theta)
{
    if(!isConnected())
        return;

    QString theta_s = QString::number((int) (theta*180/M_PI) % 360);

    QJsonObject send_object;
    send_object["msg"] = "cam_theta";
    send_object["theta"] = theta_s;

    try
    {
        tcp_client->send(send_object);
    }
    catch(const std::exception& e)
    {
        log->error("Could not send camera theta: " + QString::fromStdString(e.what()));
    }
}

void RPiCamComm::setCameraPhi(double phi)
{
    if(!isConnected())
        return;

    QString phi_s = QString::number((int) (phi*180/M_PI) % 360);

    QJsonObject send_object;
    send_object["msg"] = "cam_phi";
    send_object["phi"] = phi_s;

    try
    {
        tcp_client->send(send_object);
    }
    catch(const std::exception& e)
    {
        log->error("Could not send camera phi: " + QString::fromStdString(e.what()));
    }
}

bool RPiCamComm::isConnected() const
{
    return tcp_client->isConnected();
}

void RPiCamComm::close()
{
    if(tcp_client->isConnected())
    {
        if(stream_reader->isConnected())
            stopStream();
        tcp_client->close();
    }
    log->info("Closed connection to camera");
    emit removeGraph("RPi WiFi");
    emit wifiStrength(0);
}

void RPiCamComm::relaySignalStrength(int strength)
{
    emit wifiStrength(strength);
    emit plot(double(strength),"RPi WiFi");
    log->data("RPi WiFi RSSI",strength);
}

void RPiCamComm::ping(bool do_it, QString ip)
{
    if(do_it)
    {
        emit addGraph("RPi ping", "Time", "Response time [ms]");
        pinger->startPinging(ip);
    }
    else
    {
        emit removeGraph("RPi ping");
        pinger->stopPinging();
    }
}

void RPiCamComm::pingResponse(double delay)
{
    emit plot(delay,"RPi ping");
    log->data("RPi ping",(float) delay);
}

void RPiCamComm::requestWifiStrength(bool start)
{
    if(!isConnected())
        return;

    if(start)
    {
        emit addGraph("RPi WiFi","Time","Signal Strength [%]");

        try
        {
            tcp_client->sendCommand("start_rssi");
        }
        catch(const std::exception& e)
        {
            log->error("Could not request start camera rssi: " + QString::fromStdString(e.what()));
        }
    }
    else
    {  
       try
       {
           tcp_client->sendCommand("stop_rssi");
       }
       catch(const std::exception& e)
       {
           log->error("Could not request stop camera rssi:" + QString::fromStdString(e.what()));
       }
    }
}

void RPiCamComm::snapPhoto()
{
    if(isRecording())
    {
        toggleRecording();
    }

    if(is_streaming)
    {
       stopStreamSilent();
       tcp_client->sendCommand("snap_photo");
       startStreamSilent(port);
    }
    else
    {
       tcp_client->sendCommand("snap_photo");
    }
}

bool RPiCamComm::toggleRecording(QString file_path)
{
    if(stream_reader->isRecording())
    {
        stream_reader->stopRecording();
    }
    else
    {
        restartStream(); // Need to restart the stream in order to capture .h264 headers
        stream_reader->startRecording(file_path);
    }

    return isRecording();
}

bool RPiCamComm::isRecording() const
{
    return stream_reader->isRecording();
}

void RPiCamComm::startStreamSilent(unsigned port)
{
    // Start stream without emitting signals
    stream_reader->init(port);
    tcp_client->sendCommand("start");
    is_streaming = true;
}

void RPiCamComm::stopStreamSilent()
{
    // Stop stream without emitting signals
    mplayer->blockSignals(true);
    mplayer->stop();
    mplayer->blockSignals(false);
    tcp_client->sendCommand("stop");
    stream_reader->close();
    is_streaming = false;
}

void RPiCamComm::restartStream()
{
    stopStreamSilent();
    startStreamSilent(port);
}

void RPiCamComm::savePhoto(QString path)
{
    if(tcp_client->saveImage(path))
    {
        log->info("Image successfully saved to: " + path);
    }
    else
    {
        log->error("Unable to save image to: " + path);
    }
}

