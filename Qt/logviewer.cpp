#include "logviewer.h"
#include "logfile.h"

LogViewer::LogViewer()
{
    this->setAttribute(Qt::WA_DeleteOnClose);
}

void LogViewer::loadLog(QString path)
{
    QStringList split_path = path.split(".");
    QString encoding = split_path.at(split_path.count()-1);
    QString type = split_path.at(split_path.count()-2);
    QString name = split_path.at(split_path.count()-4);

    QList<QDateTime> timestamps;
    QString unit;


    if(type == "int")
    {
        QList<int> data;
        if(encoding == "bin")
        {
            LogFile::parseBinary(path, &timestamps, &data, &unit);
        }
        else
        {
            LogFile::parseASCII(path, &timestamps, &data, &unit);
        }

        addGraph(name,"Time",unit);
        for(int i = 0; i < data.count(); i++)
        {
            addPoint(timestamps.at(i).time().msecsSinceStartOfDay()/(double)1000,(double) data.at(i),name);
        }
    }
    else if(type == "short")
    {
        QList<short> data;
        if(encoding == "bin")
        {
            LogFile::parseBinary(path, &timestamps, &data, &unit);
        }
        else
        {
            LogFile::parseASCII(path, &timestamps, &data, &unit);
        }

        addGraph(name,"Time",unit);
        for(int i = 0; i < data.count(); i++)
        {
            addPoint(timestamps.at(i).time().msecsSinceStartOfDay()/(double)1000,(double) data.at(i),name);
        }
    }
    else if(type == "double")
    {
        QList<double> data;
        if(encoding == "bin")
        {
            LogFile::parseBinary(path, &timestamps, &data, &unit);
        }
        else
        {
            LogFile::parseASCII(path, &timestamps, &data, &unit);
        }

        addGraph(name,"Time",unit);
        for(int i = 0; i < data.count(); i++)
        {
            addPoint(timestamps.at(i).time().msecsSinceStartOfDay()/(double)1000,(double) data.at(i),name);
        }
    }
    else if(type == "float")
    {
        QList<float> data;
        if(encoding == "bin")
        {
            LogFile::parseBinary(path, &timestamps, &data, &unit);
        }
        else
        {
            LogFile::parseASCII(path, &timestamps, &data, &unit);
        }

        addGraph(name,"Time",unit);
        for(int i = 0; i < data.count(); i++)
        {
            addPoint(timestamps.at(i).time().msecsSinceStartOfDay()/(double)1000,(double) data.at(i),name);
        }
    }
}

void LogViewer::loadLogs(QStringList paths)
{
    for(int i = 0; i < paths.count(); i++)
    {
        loadLog(paths.at(i));
    }
}


