#include "mplayerwrapper.h"

MPlayerWrapper::MPlayerWrapper(Logger* log)
{

    // Default path depending on operating system
    #if defined(Q_OS_WIN)
        path="../GUI/mplayer_win/mplayer.exe";
        command = " -fps 60 -cache 1024 -zoom -";
    #elif defined(Q_OS_LINUX)
        path = "mplayer";
        command = " -fps 60 -cache 1024 -vo x11 -zoom -";
    #endif

    mplayer = new QProcess();
    this->log = log;
}

MPlayerWrapper::MPlayerWrapper(QString path,Logger* log)
{
    this->path = path;
    #if defined(Q_OS_WIN)
        command = " -fps 60 -cache 1024 -zoom -";
    #elif defined(Q_OS_LINUX)
        command = " -fps 60 -cache 1024 -vo x11 -zoom -";
    #endif

    this->log = log;
    mplayer = new QProcess();
    connect(mplayer, SIGNAL(finished(int exitCode, QProcess::ExitStatus exitStatus)), this, SLOT(stop()));
    connect(mplayer, SIGNAL(errorOccurred(QProcess::ProcessError error)), this, SLOT(error(QProcess::ProcessError error)));
    connect(mplayer, SIGNAL(stateChanged(QProcess::ProcessState newState)), this, SLOT(stop()));
}

MPlayerWrapper::~MPlayerWrapper()
{
    stop();
}

void MPlayerWrapper::setPath(QString path)
{
    this->path = path;
}

void MPlayerWrapper::sendData(QByteArray data)
{
    if(mplayer->state() == QProcess::NotRunning)
    {
        stop();
        return;
    }

    mplayer->write(data);
}

void MPlayerWrapper::start()
{
    mplayer->start(path + command);
}

void MPlayerWrapper::stop()
{
    mplayer->close();
    mplayer->terminate();
    emit windowClosed();
}

bool MPlayerWrapper::is_playing()
{
    return mplayer->isOpen();
}

void MPlayerWrapper::error(QProcess::ProcessError error)
{
    log->error("MPlayer process exited with error code: " + error);
}
