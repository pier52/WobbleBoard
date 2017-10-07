/**
 * \class TCPClient
 *
 *
 * \brief
 *      Implements a simple TCP client
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <iostream>
#include <QTcpSocket>
#include <QtCore>
#include <string>
#include <stdexcept>
#include "logger.h"


class TCPClient : public QThread
{
    Q_OBJECT

public:
    TCPClient(Logger* log,QObject *parent = 0);
    ~TCPClient();

    void init(quint16 port, QString host);
    void send(std::string msg, std::string key);
    void close(void);
    void send(QJsonObject msg);
    void sendCommand(QString);
    bool isConnected() const;

    // Saves the data stored in local variable <image_data> to file
    bool saveImage(QString path);

signals:
    void newSignalStrength(int strength);

private:
    QTcpSocket* socket;
    Logger* log;
    bool is_connected = false;
    QByteArray image_data;
    bool is_receiving_image = false;

private slots:
    void readTcpData();
    void connected();
    void disconnected();


};

#endif // TCPSOCKET_H
