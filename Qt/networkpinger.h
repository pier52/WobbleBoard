/**
 * \class NetworkPinger
 *
 *
 * \brief
 * Uses the operating system's (support for Windows and Linux) built in ping function to ping an IP address.
 *
 * \author Samuel Orn
 *
 */

#ifndef NETWORKPINGER_H
#define NETWORKPINGER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include "logger.h"

class NetworkPinger : public QObject
{
    Q_OBJECT
public:
    explicit NetworkPinger(Logger* log,QObject *parent = 0);
    ~NetworkPinger();

signals:
    void response(double delay);

public slots:
    /**
     * @brief
     * Starts continously pinging the specified IP. Pingin is stopped via the stopPingning() function.
     * Emits response signal with the delay in ms. In cases of no respons, negative response values are emitted:\n
     * -1: Timeout\n
     * -10: Host unreachable \n
     * -100: Host unavailable \n
     *
     * @param IP
     * IP address to ping
     */
    void startPinging(QString IP);
    void stopPinging();
    void finished(int,QProcess::ExitStatus);

private slots:
    void read();
    void timerTrig();
    void errorOccurred(QProcess::ProcessError error);

private:
    QProcess* ping_process;
    QString command;
    QStringList args;
    QTimer timer;
    Logger* log;
};

#endif // NETWORKPINGER_H
