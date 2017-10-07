#include "networkpinger.h"
#include <QDebug>


NetworkPinger::NetworkPinger(Logger* log, QObject *parent) : QObject(parent)
{
    ping_process = new QProcess(this);
    connect(&timer,SIGNAL(timeout()),this,SLOT(timerTrig()));
    this->log = log;
}

NetworkPinger::~NetworkPinger()
{
    stopPinging();
    ping_process->deleteLater();
}

void NetworkPinger::startPinging(QString IP)
{
    if(ping_process->state() != QProcess::NotRunning)
        return;

    connect(ping_process,SIGNAL(readyRead()),this,SLOT(read()));
    connect(ping_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    connect(ping_process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));

    #if defined(Q_OS_WIN)
        command = "ping";
        args =  QStringList() << QString(IP);
    #elif defined(Q_OS_LINUX)
        command = "ping";
        args =  QStringList() << QString(IP);
    #endif

    ping_process->start(command,args);
}

void NetworkPinger::finished(int exit_code,QProcess::ExitStatus)
{
    if(exit_code == 2)
    {
        qDebug("Could not find host");
        emit response(-100);
        timer.start(1000); // Otherwise, might spam out messages fast
        return;
    }
    ping_process->start(command,args);
}

void NetworkPinger::timerTrig()
{
    timer.stop();
    ping_process->start(command,args);
}

void NetworkPinger::stopPinging()
{
    disconnect(ping_process,SIGNAL(readyRead()),this,SLOT(read()));
    disconnect(ping_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    disconnect(ping_process,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(errorOccurred(QProcess::ProcessError)));
    ping_process->close();
    ping_process->terminate();
}

void NetworkPinger::read()
{
    QStringList lines = QString::fromStdString(ping_process->readAll().toStdString()).split("\n",QString::SkipEmptyParts);
    QString line;
    for(int i=0; i<lines.count(); i++)
    {
        line = lines.at(i);

        if(line.contains("time="))
        {
            QString delay = line.split("time=").at(1).section("m",0,0);
            emit response(delay.toFloat());
        }
        else if(line.contains("timed out",Qt::CaseInsensitive))
        {
            emit response(-1);
        }
        else if(line.contains("host unreachable",Qt::CaseInsensitive))
        {
            emit response(-10);
        }
        else if(line.contains("could not find host",Qt::CaseInsensitive))
        {
            emit response(-100);
        }
        else if(line.contains("ping",Qt::CaseInsensitive))
        {
            continue;
        }
        else
        {
            log->error("NetworkPinger::Could not decode ping response. Aborting.");
            qDebug() << line;
            stopPinging();
        }
    }
}

void NetworkPinger::errorOccurred(QProcess::ProcessError error)
{
    log->error("NetworkPinger::Could not start pinging process. QProcess error code: " + QString::number(error));
    stopPinging();
}
