/**
 * \class MPlayerWrapper
 *
 *
 * \brief
 *      A wrapper to use the media player MPlayer.
 *
 * The wrapper executes the MPlayer executable in a QProcess, which data can be piped into. \n
 * Note that certain aspects of this class change depending on operating system (Linux vs. Windows), such as which commands are piped to the QProcess to start the MPlayer binary \n
 * Note also that the MPlayer executable must be present in the variable <path>.
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef MPLAYERWRAPPER_H
#define MPLAYERWRAPPER_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QtGlobal>
#include <QDebug>
#include <stdexcept>
#include "logger.h"

class MPlayerWrapper : public QObject
{
    Q_OBJECT

public:
    MPlayerWrapper(QString path,Logger* log);
    MPlayerWrapper(Logger* log);
    ~MPlayerWrapper();
    void setPath(QString path);
    void sendData(QByteArray data);
    void start();
    bool is_playing();

public slots:
    void stop();

signals:
    /// Sent when the mplayer window is closed.
    void windowClosed();

private:
    /// Path to the MPlayer executable.
    QString path;
    QString command;
    QProcess* mplayer;
    Logger* log;

private slots:
    void error(QProcess::ProcessError error);
};

#endif // MPLAYERWRAPPER_H
