#include "tcpclient.h"
#include <QThread>
#include <QMutex>

// Constructors and destructors

TCPClient::TCPClient(Logger* log,QObject *parent)
{
    socket = new QTcpSocket();
    this->log = log;

    connect(socket,SIGNAL(connected()),this,SLOT(connected()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readTcpData()));
}

TCPClient::~TCPClient()
{
    close();
    delete socket;
}

void TCPClient::connected()
{
    is_connected = true;
}

void TCPClient::disconnected()
{
    is_connected = false;
}

void TCPClient::init(quint16 port, QString host)
{
    socket->connectToHost(host, port);

    if (!socket->waitForConnected(5000)) {
        throw std::runtime_error(socket->errorString().toStdString());
        socket->close();
    }
}

void TCPClient::sendCommand(QString command)
{
    QJsonObject send_object;
    send_object["msg"] = command;

    send(send_object);
}

void TCPClient::send(QJsonObject json)
{
    QString request_key = json.value("msg").toString();

    QJsonDocument doc(json);
    QString qstr_json(doc.toJson(QJsonDocument::Compact));

    send(qstr_json.toStdString(),request_key.toStdString());
}

void TCPClient::send(std::string msg, std::string request_key)
{
    socket->write(msg.c_str(),msg.length());

    if(!socket->waitForBytesWritten(1000))
    {
        throw std::runtime_error("Camera Module::Could not send data!");
        return;
    }


    if(!socket->waitForReadyRead(3000))
    {
        throw std::runtime_error("Camera Module::No answer from server!");
        return;
    }

    std::string answer = socket->readAll().toStdString();

    // Expects answer to be same as request_key
    if(!strcmp(answer.c_str(),("$" + request_key).c_str()))
        throw std::runtime_error("Camera Module::Wrong answer from server! Got: " + answer + " Shoud have been: " + request_key);
}

void TCPClient::close()
{
    socket->close();
}

void TCPClient::readTcpData()
{

    char peeper[1];
    socket->peek(&peeper[0],1);
    if(peeper[0] == '$')
        return;
    else if(peeper[0] == '*') // Wifi signal level
    {
        std::string sig_level_str = socket->readAll().toStdString();
        int signal_level = std::stoi(sig_level_str.substr(1));
        emit newSignalStrength(signal_level);
    }
    else if(peeper[0] == '&') // Image data
    {
        socket->read(1); // Remove tag
        qDebug() << "Receiving photo";
        is_receiving_image = true;
        image_data = socket->readAll();

        while(!image_data.endsWith(QByteArray("HALT")))
        {
                  if(!socket->waitForReadyRead(1000))
                  {
                      log->error("Timeout while receiving photo");
                      is_receiving_image = false;
                      image_data.clear();
                      return;
                  }
                  image_data.append(socket->readAll());
        }

        // Save file
        image_data.chop(4); // Remove halt message

        qDebug() << "Finsihed receiving photo";
        is_receiving_image = false;
    }
    else
    {
        std::string answer = socket->readAll().toStdString();

        log->error("Unexpected TCP message from Camera Module: " + answer);
    }




    //if(answer.toStdString() != sent_request_key)
    //    throw new std::runtime_error("Error::Wrong answer from server! Got: " + answer.toStdString() + " Shoud have been: ");
}

bool TCPClient::saveImage(QString path)
{
    if(image_data.isEmpty())
        return false;

    while(is_receiving_image)
    {
        // Wait, but don't block
        QCoreApplication::processEvents(QEventLoop::AllEvents,1000);
    }

    QFile file(path);

    try{
        file.open(QIODevice::WriteOnly);
        file.write(image_data);
        file.close();
        image_data.clear();
    }
    catch(std::exception)
    {
        return false;
    }

    return true;
}

bool TCPClient::isConnected() const
{
    return is_connected;
}




