#include "logger.h"
#include <QDateTime>
#include <QDataStream>
#include <QTextStream>

Logger::Logger(QString log_path, int clear_rule,QStringList enabled_logs, QObject *parent) : QObject(parent)
{
    this->log_path = log_path;

    session_time = "[session_" + QDateTime::currentDateTime().toString("hh.mm]");

    if(!QDir(log_path+"/logs").exists())
    {
        QDir(log_path).mkdir("logs");
    }

    switch(clear_rule)
    {
    case 1:
        cleanLogFolder();
        break;
    case 2:
        cleanLogFolder(1);
        break;
    case 3:
        cleanLogFolder(7);
        break;
    case 4:
        cleanLogFolder(30);
        break;
    default:
        break;
    }

    if(enabled_logs.contains("Error"))
    {
       logs.insert("Error",new logItem(new LogFile(getBasePath() + session_time + ".ERROR",false,log_type::type_string),true));
       logs.value("Error")->log->open();
    }
    else
    {
        logs.insert("Error",new logItem(new LogFile(getBasePath() + session_time + ".ERROR",false,log_type::type_string),false));
    }

    if(enabled_logs.contains("Warning"))
    {
       logs.insert("Warning",new logItem(new LogFile(getBasePath() + session_time + ".WARNING",false,log_type::type_string),true));
       logs.value("Warning")->log->open();
    }
    else
    {
        logs.insert("Warning",new logItem(new LogFile(getBasePath() + session_time + ".WARNING",false,log_type::type_string),false));
    }

    if(enabled_logs.contains("Info"))
    {
        logs.insert("Info",new logItem(new LogFile(getBasePath() + session_time + ".INFO",false,log_type::type_string),true));
        logs.value("Info")->log->open();
    }
    else
    {
        logs.insert("Info",new logItem(new LogFile(getBasePath() + session_time + ".INFO",false,log_type::type_string),false));
    }

    allow_new_data_logs = enabled_logs.contains("Data");
}

Logger::~Logger()
{
    QMapIterator<QString, logItem *> i(logs);
    while(i.hasNext())
    {
        i.next();
        delete i.value();
    }
}

void Logger::error(QString msg, bool silent)
{
    QString log_msg = " || ERROR: " + msg + "\n";

    writeToLog("Info",log_msg);
    writeToLog("Warning",log_msg);
    writeToLog("Error",log_msg);

    if(!silent)
        emit errorMsg("ERROR: " + msg,"red");
}

void Logger::warning(QString msg, bool silent)
{
    QString log_msg = " || WARNING: " + msg + "\n";

    writeToLog("Info",log_msg);
    writeToLog("Warning",log_msg);

    if(!silent)
        emit warningMsg("WARNING: " + msg,"blue");
}

void Logger::info(QString msg,bool silent)
{
    QString log_msg = " || INFO: " + msg + "\n";

    writeToLog("Info",log_msg);

    if(!silent)
        emit infoMsg(msg,"black");
}

void Logger::clearAllLogs()
{
    QMapIterator<QString, logItem *> i(logs);
    while(i.hasNext())
    {
        i.next();
        clearLog(i.key(),0);
    }
}

void Logger::clearLog(QString name, int older_than)
{
    if(!logs.contains(name))
        return;

    logs.value(name)->log->close();

    QStringList to_be_deleted;

    findRecursion(log_path+"/logs","*"+name.toUpper().replace(" ","_") + "*",&to_be_deleted,older_than);

    mutex.lock();
    for(int i=0; i<to_be_deleted.count(); i++)
    {
        QFile(to_be_deleted.at(i)).remove();
    }
    mutex.unlock();

    removeEmptyFolders(log_path+"/logs");
}

void Logger::error(std::string msg, bool silent)
{
    error(QString::fromStdString(msg),silent);
}

void Logger::warning(std::string msg, bool silent)
{
    warning(QString::fromStdString(msg),silent);
}

void Logger::info(std::string msg, bool silent)
{
    info(QString::fromStdString(msg),silent);
}

void Logger::info(const char *msg,bool silent)
{
    info(QString::fromUtf8(msg),silent);
}

void Logger::warning(const char *msg,bool silent)
{
    warning(QString::fromUtf8(msg),silent);
}

void Logger::error(const char *msg, bool silent)
{
    error(QString::fromUtf8(msg),silent);
}

template <class T> void Logger::data(QString name, T data)
{
    //addDataLog(name,allow_new_data_logs);
    writeToLog(name,data);
}

template <class T> void Logger::writeToLog(QString log, T msg)
{
    if(logs.value(log)->save)
    {
        if(!logs.value(log)->log->isOpen())
        {
            logs.value(log)->log->open();
        }
        logs.value(log)->log->write(msg);
    }
}

void Logger::addDataLog(QString name, QString unit, log_type::data_type type)
{
    if(!logs.contains(name))
    {
        logs.insert(name,new logItem(new LogFile(getBasePath()+ session_time + "." + name.toUpper().replace(" ","_")+".DATA",true,type,unit),allow_new_data_logs));

        emit newDataLog(name);
    }
}

void Logger::setLogsToSave(QStringList names)
{
    QMapIterator<QString, logItem *> i(logs);
    while(i.hasNext())
    {
        i.next();
        if(names.contains(i.key()))
        {
            i.value()->save = true;
        }
        else
        {
            i.value()->save = false;
        }
    }
}

void Logger::clearLogs(QStringList names, int older_than)
{
    for(int i = 0; i<names.count(); i++)
    {
        clearLog(names.at(i), older_than);
    }
}

QString Logger::getBasePath()
{
    QString log_folder;
    if(log_path == "")
    {
        log_folder = "logs/";
    }
    else
    {
        log_folder = log_path + "/logs/";
    }

    QString year = QDateTime::currentDateTime().toString("yyyy");
    if(!QDir(log_folder + year).exists())
    {
        QDir(log_folder).mkdir(year);
    }

    QString month =  QLocale().monthName(QDateTime::currentDateTime().date().month()); // Month name depends on locale, force so always in english
    if(!QDir(log_folder + year + "/" + month).exists())
    {
        QDir(log_folder + year).mkdir(month);
    }

    QString day = QDateTime::currentDateTime().toString("dd");
    if(!QDir(log_folder + year + "/" + month + "/" + day).exists())
    {
        QDir(log_folder + year + "/" + month).mkdir(day);
    }

    return log_folder + year + "/" + month + "/" + day + "/";
}

void Logger::findRecursion(const QString &path, const QString &pattern, QStringList *result, int older_than)
{
    QDir currentDir(path);
    const QString prefix = path + QLatin1Char('/');
    const QDateTime now = QDateTime::currentDateTime();

    foreach (const QString &match, currentDir.entryList(QStringList(pattern), QDir::Files | QDir::NoSymLinks))
    {
        if(QFileInfo(prefix+match).lastModified().daysTo(now) >= older_than)
        {
            result->append(prefix + match);
        }
    }
    foreach (const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
        findRecursion(prefix + dir, pattern, result);
}

void Logger::removeEmptyFolders(const QString& path)
{
    QDir currentDir(path);
    const QString prefix = path + QLatin1Char('/');
    foreach (const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
    {
        if(QDir(prefix+dir).entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() == 0) // is empty
        {
            currentDir.rmdir(dir);
        }
        else
        {
            removeEmptyFolders(prefix + dir);
        }
    }
}

void Logger::cleanLogFolder(int older_than)
{
    QStringList to_be_deleted;
    findRecursion(log_path+"/logs","*",&to_be_deleted,older_than);

    for(int i=0; i<to_be_deleted.count(); i++)
    {
        QFile(to_be_deleted.at(i)).remove();
    }

    removeEmptyFolders(log_path+"/logs");
}

void Logger::enableLog(QString name, bool enable)
{
    if(logs.value(name)->save == enable)
        return;

    if(enable)
    {
        logs.value(name)->save = enable;
    }
    else
    {
        logs.value(name)->log->close();
        logs.value(name)->save = enable;
    }
}

void Logger::enableLogs(QStringList names)
{
    QMapIterator<QString, logItem *> i(logs);
    while(i.hasNext())
    {
        i.next();
        enableLog(i.key(),names.contains(i.key()));
    }

    allow_new_data_logs = names.contains("Data");
}

template void Logger::data(QString,int);
template void Logger::data(QString,float);
template void Logger::data(QString,short);
template void Logger::data(QString,QString);

template void Logger::writeToLog(QString,int);
template void Logger::writeToLog(QString,float);
template void Logger::writeToLog(QString,short);
template void Logger::writeToLog(QString,QString);





