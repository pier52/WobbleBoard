/**
 * \class LogViewer
 *
 *
 * \brief
 *      Subclass of PlotWindow, with added functionality to parse and display data from logs.
 *      This class is destructed automatically as the plotwindow is closed.
 *
 *
 * \author Samuel Orn
 *
 */


#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include "plotwindow.h"


class LogViewer : public PlotWindow
{
public:
    LogViewer();

    /// Parses the log in <path> and loads it into the plotwindow and shows the window
    /// Decodes which format the log is in (data type, binary/ascii) by looking at file name
    void loadLog(QString path);

    /// Parses the logs in <paths> and loads them into a plotwindow and shows the window
    void loadLogs(QStringList paths);
};

#endif // LOGVIEWER_H
