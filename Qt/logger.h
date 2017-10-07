/**
 * \class Logger
 *
 *
 * \brief
 *      Implements a logger. The logger logs the messages in files, and can optionally also display them in
 *      the command window.
 *
 * It handles 4 different main categories: \n
 *   ERROR - error messages; logged in ERROR, WARNING and INFO logs \n
 *   WARNING - warning messages; logged in WARNING and INFO logs \n
 *   INFO - informative messages; logged in INFO log \n
 *   DATA - logs data samples in a log \n \n
 * All logs have their messages timestamped with millisecond accuracy. \n
 * All write calls are protected by mutexes. \n
 *
 * \author Samuel Orn
 *
 */


#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <mutex>
#include <QDebug>
#include <QMap>
#include <QDir>
#include "logfile.h"

class Logger : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Logger
     * Constructs a logger class object
     * @param log_path
     * File path to the directory where the log directory tree will be created
     * @param clear_rule
     * Whether or not to clear logs on startup.
     * =1: Clear all old logs
     * =2: Clear logs older than 1 day
     * =3: Clear logs older than 7 days
     * =4: Clear logs older than 30 days
     * else: Don't clear
     * @param enabled_logs
     * Which logs are enabled on startup
     * @param parent
     */
    explicit Logger(QString log_path,int clear_rule,QStringList enabled_logs,QObject *parent = 0);
    ~Logger();

    /// Writes <msg> to INFO log. If <silent>, <msg> is not printed in prompt.
    void info(std::string msg, bool silent=false);
    /// Writes <msg> to ERROR log. If <silent>, <msg> is not printed in prompt.
    void error(std::string msg, bool silent=false);
    /// Writes <msg> to WARNING log. If <silent>, <msg> is not printed in prompt.
    void warning(std::string msg, bool silent=false);

    /// Writes <msg> to INFO log. If <silent>, <msg> is not printed in prompt.
    void info(QString msg, bool silent=false);
    /// Writes <msg> to ERROR log. If <silent>, <msg> is not printed in prompt.
    void error(QString msg, bool silent=false);
    /// Writes <msg> to WARNING log. If <silent>, <msg> is not printed in prompt.
    void warning(QString msg, bool silent=false);

    /// Writes <msg> to INFO log. If <silent>, <msg> is not printed in prompt.
    void info(const char* msg, bool silent=false);
    /// Writes <msg> to ERROR log. If <silent>, <msg> is not printed in prompt.
    void error(const char* msg, bool silent=false);
    /// Writes <msg> to WARNING log. If <silent>, <msg> is not printed in prompt.
    void warning(const char* msg, bool silent=false);

    /// Clears ALL the log file on drive
    void clearAllLogs();
    
    
    /**
     * @brief
     * Writes data to log
     * @param name
     * Name of the log
     * @param data
     * Data of type <int>, <short>, <QString>, <double> or <float>
     */
    template <class T>
    void data(QString name, T data);

    /**
     * @brief
     * Clears the log with the specified name
     * @param name
     * Log to be cleared
     * @param older_than
     * Only clear logs older than (in days) this. Counted from modified date.
     */
    void clearLog(QString name, int older_than);

signals:
    void infoMsg(QString msg, QString color);
    void errorMsg(QString msg, QString color);
    void warningMsg(QString msg, QString color);
    void newDataLog(QString name);
    void plot(QString name,double data);

public slots:
    /// Enables the writing of logs in <names>, disables the rest
    void enableLogs(QStringList names);

    /**
     * @brief
     * Adds a new data log
     * @param name
     * The name of the log
     * @param unit
     * The unit of the data, e.g. "Volt"
     * @param type
     * The data type, eg. int, double etc.
     */
    void addDataLog(QString name,QString unit,log_type::data_type type);


    /**
     * @brief
     * Sets which logs to allow writing to. If the log name is in <names>, messages will be
     * written to the log, otherwise they will not.
     * @param names
     * QStringList of names of graphs that will have their messages logged
     */
    void setLogsToSave(QStringList names);

    /**
     * @brief
     * Clears the logs with names in @param names
     */
    void clearLogs(QStringList names, int older_than);

private:
    /// Enables and disables the writing of a log
    void enableLog(QString name, bool enable);

    /// Writes a message to the specified log
    template <class T>
    void writeToLog(QString log, T msg);

    /// Gets the base path [user config path/year/date/day]
    QString getBasePath();

    /**
     * @brief
     * Finds all the files in a directory and subdirectories with a certain pattern, older than a specified number of days
     * @param path
     * Path to search in
     * @param pattern
     * Pattern to look for .e.g. *.jpg
     * @param result
     * QStringList containing the path to all files matching the criteria
     * @param older_than
     * Only include files older (counted from last modified) than this number of days. If =-1: include all.
     */
    void findRecursion(const QString &path, const QString &pattern, QStringList *result,int older_than = -1);

    /**
     * @brief
     * Cleans the log folder; removes all _files_ (irregardles if it is a log or not) older than specified and removes empty folders
     * @param older_than
     * Remove files older than this. If negative: remove all.
     */
    void cleanLogFolder(int older_than = -1);

    /// Removes all empty folders in a path
    void removeEmptyFolders(const QString& path);

    /// Determines whether data logs, when they are added dynamically, will get the property
    /// which allows them to be written to file
    bool allow_new_data_logs;

    QString log_path;

    std::mutex mutex;

    /// Structure for managing logs
    struct logItem{
        logItem(LogFile* log, bool save)
        {
            this->log = log;
            this->save = save;
        }

        ~logItem()
        {
            delete log;
        }
        /// The log stream
        LogFile* log;

        /// Determines whether the log messages are allowed to be written to file or not
        bool save;
    };

    QMap<QString,logItem*> logs;

    /// Contains the time of creation of the logger object (i.e. when the GUI was started)
    QString session_time;
};

#endif // LOGGER_H
