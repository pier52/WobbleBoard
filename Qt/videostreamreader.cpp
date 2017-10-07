#include "videostreamreader.h"

VideoStreamReader::VideoStreamReader(MPlayerWrapper* mplayer,Logger* log,QObject *parent) : QObject(parent)
{
    this->mplayer = mplayer;
    this->log = log;
    socket = nullptr;
}

VideoStreamReader::~VideoStreamReader()
{
    close();
    socket->deleteLater();
    server->deleteLater();
}

bool VideoStreamReader::init(quint16 port)
{
    this->port = port;
    // start up mplayer
    mplayer->start();
    return open();
}

bool VideoStreamReader::open()
{
    server = new QTcpServer(this);
    // Listen on the proper port
    server->listen( QHostAddress::Any, port );

    // Hook up signal and slots
    connect(server, SIGNAL(newConnection()), this, SLOT(gotNewConnection()));
    connect(server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
    return true;
}


void VideoStreamReader::gotNewConnection()
{
    // Get the socket
    socket = server->nextPendingConnection();
    if(socket->state() != QTcpSocket::ConnectedState)
    {
        log->error("VideoStreamReader::Socket could not resolve pending connection");
        return;
    }

    // Hook up some signals / slots
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
}


void VideoStreamReader::error(QAbstractSocket::SocketError error)
{
    log->error("VideoStreamReader::An error occured in QTCPServer with error code: " + QString::number(error));
}

void VideoStreamReader::disconnected()
{
    if(socket->isOpen())
        close();
}

void VideoStreamReader::readyRead()
{
    QByteArray data = socket->readAll();
    socket->flush();
    mplayer->sendData(data);

    if(record)
    {
       save_file.write(data);
    }
}

void VideoStreamReader::close()
{
    socket->flush();
    socket->abort();
    server->close();
    stopRecording();
}

bool VideoStreamReader::isConnected() const
{
    if(socket == nullptr)
        return false;

    return socket->isOpen();
}

void VideoStreamReader::startRecording(QString file_path)
{
    save_file.setFileName(file_path);
    if(!save_file.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error("VideoStreamReader::could not open file");
    }
    record = true;
    log->info("Recording stream to: " + file_path);
}

void VideoStreamReader::stopRecording()
{
    if(record)
    {
        record = false;
        save_file.close();
        log->info("Recording stopped");
    }
}

bool VideoStreamReader::isRecording() const
{
    return record;
}


