#include "logfile.h"
#include <QDateTime>

LogFile::LogFile(QString path, bool binary, log_type::data_type type ,QString unit)
{
    this->type = type;

    QString path_end;


    if(type == log_type::type_int)
    {
        path_end = ".int";
    }
    else if (type == log_type::type_float)
    {
        path_end = ".float";
    }
    else if (type == log_type::type_double)
    {
        path_end = ".double";
    }
    else if (type == log_type::type_short)
    {
        path_end = ".short";
    }
    else if (type == log_type::type_string)
    {
        path_end = ".str";
    }
    else
    {
        throw std::logic_error("Cannot log the given data type, must be <int>,<float>,<double>,<short>,<long> or <string>");
    }


    if(binary)
    {
        path_end += ".bin";
    }
    else
    {
        path_end += ".asc";
    }

    file.setFileName(path + path_end);
    this->binary = binary;
    this->unit = unit;
}

LogFile::~LogFile()
{
    close();
}


template <class T> void LogFile::write(const T data)
{
    if(!file.isOpen())
        return;

    if(!isCorrectType(data))
    {
        throw std::runtime_error("Incorrect logging type");
    }

    if(binary)
    {
        QDataStream out(&file);
        out << QDateTime::currentDateTime();
        out << data;   
    }
    else
    {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz: ") + QString::number(data) + "\n";
    }

    file.flush();
}


void LogFile::write(QString data)
{
    if(!file.isOpen())
        return;

    if(!isCorrectType(data))
    {
        throw std::runtime_error("Incorrect logging type");
    }

    if(binary)
    {
        QDataStream out(&file);
        out << QDateTime::currentDateTime();
        out << data;
    }
    else
    {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz: ") + data + "\n";
    }

    file.flush();
}

void LogFile::open()
{
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    if(!binary)
    {

        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz: ") + " | Logging started, unit is " + unit + "\n";
    }
    else
    {
        QDataStream out(&file);
        out << unit;
    }
}

void LogFile::close()
{
    if(isOpen())
    {
        if(!binary)
        {
            QTextStream out(&file);
            out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz: ") + " | Logging stopped. \n";
        }
        file.flush();
        file.close();
    }
}

template <class T> void LogFile::parseBinary(QString path, QList<QDateTime> *timestamps, QList<T>* data, QString *unit)
{
    QFile file(path);
    QDataStream in(&file);
    file.open(QFile::ReadOnly);

    in >> *unit; // Unit is first data entry

    QDateTime timestamp;
    T data_;
    while(!in.atEnd())
    {
        in >> timestamp >> data_;
        timestamps->append(timestamp);
        data->append(data_);
    }

    file.close();
}

void LogFile::parseASCII(QString path, QList<QDateTime> *timestamps, QList<int>* data, QString *unit)
{
    QFile file(path);
    QTextStream in(&file);
    file.open(QFile::ReadOnly);

    *unit = in.readLine().split("Unit is: ").at(1); // Unit is first data entry

    QStringList content;

    while(!in.atEnd())
    {
        content = in.readLine().split(" ");
        timestamps->append(QDateTime::fromString(content.at(0)+content.at(1),"yyyy.MM.ddhh:mm:ss.zzz:"));
        data->append(content.at(2).toInt());
    }
    file.close();
}

void LogFile::parseASCII(QString path, QList<QDateTime> *timestamps, QList<short>* data, QString *unit)
{
    QFile file(path);
    QTextStream in(&file);
    file.open(QFile::ReadOnly);

    *unit = in.readLine().split("Unit is: ").at(1); // Unit is first data entry

    QStringList content;

    while(!in.atEnd())
    {
        content = in.readLine().split(" ");
        timestamps->append(QDateTime::fromString(content.at(0)+content.at(1),"yyyy.MM.ddhh:mm:ss.zzz:"));
        data->append(content.at(2).toShort());
    }
    file.close();
}

void LogFile::parseASCII(QString path, QList<QDateTime> *timestamps, QList<double>* data, QString *unit)
{
    QFile file(path);
    QTextStream in(&file);
    file.open(QFile::ReadOnly);

    *unit = in.readLine().split("Unit is: ").at(1); // Unit is first data entry

    QStringList content;

    while(!in.atEnd())
    {
        content = in.readLine().split(" ");
        timestamps->append(QDateTime::fromString(content.at(0)+content.at(1),"yyyy.MM.ddhh:mm:ss.zzz:"));
        data->append(content.at(2).toDouble());
    }
    file.close();
}

void LogFile::parseASCII(QString path, QList<QDateTime> *timestamps, QList<float>* data, QString *unit)
{
    QFile file(path);
    QTextStream in(&file);
    file.open(QFile::ReadOnly);
    *unit = in.readLine().toLower().split("Unit is: ").at(1); // Unit is first data entry

    QStringList content;

    while(!in.atEnd())
    {
        content = in.readLine().split(" ");
        timestamps->append(QDateTime::fromString(content.at(0)+content.at(1),"yyyy.MM.ddhh:mm:ss.zzz:"));

        data->append(content.at(2).toFloat());
    }
    file.close();
}

void LogFile::parseASCII(QString path, QList<QDateTime> *timestamps, QList<QString>* data, QString *unit)
{
    QFile file(path);
    QTextStream in(&file);
    file.open(QFile::ReadOnly);

    *unit = in.readLine().split("Unit is: ").at(1); // Unit is first data entry

    QStringList content;

    while(!in.atEnd())
    {
        content = in.readLine().split(" ");
        timestamps->append(QDateTime::fromString(content.at(0)+content.at(1),"yyyy.MM.ddhh:mm:ss.zzz:"));
        data->append(content.at(2));
    }
    file.close();
}



template <class T> bool LogFile::isCorrectType(const T) const
{
    if(std::is_same<T,int>::value)
    {
        return type == log_type::type_int;
    }
    else if(std::is_same<T,float>::value)
    {
        return type == log_type::type_float;
    }
    else if(std::is_same<T,QString>::value)
    {
        return type == log_type::type_string;
    }
    else if(std::is_same<T,double>::value)
    {
        return type == log_type::type_double;
    }
    else if(std::is_same<T,short>::value)
    {
        return type == log_type::type_short;
    }
    else
    {
        return false;
    }
}


void LogFile::toASCIIFile(const QString& binary_read_path, QString ascii_write_path)
{
    QStringList split_path = binary_read_path.split(".");
    if(ascii_write_path == "")
    {
        for(int i = 0; i<split_path.count()-1; i++)
        {
            ascii_write_path += split_path.at(i) + ".";
        }
        ascii_write_path += "asc";
    }

    QString type = split_path.at(split_path.count()-2);


    QFile file_out(ascii_write_path);
    QTextStream out(&file_out);
    file_out.open(QFile::WriteOnly);

    QList<QDateTime> timestamps;

    QString unit;

    if(type == "int")
    {
        QList<int> data;
        parseBinary(binary_read_path,&timestamps,&data,&unit);

        out << "Unit is: " + unit + "\n";
        for(int i = 0; i<timestamps.count(); i++)
        {
            out << timestamps.at(i).toString("yyyy.MM.dd hh:mm:ss.zzz: ") + QString::number(data.at(i)) + "\n";
        }
    }
    else if(type == "short")
    {
        QList<short> data;
        parseBinary(binary_read_path,&timestamps,&data,&unit);

        out << "Unit is: " + unit + "\n";
        for(int i = 0; i<timestamps.count(); i++)
        {
            out << timestamps.at(i).toString("yyyy.MM.dd hh:mm:ss.zzz: ") + QString::number(data.at(i)) + "\n";
        }
    }
    else if(type == "double")
    {
        QList<double> data;
        parseBinary(binary_read_path,&timestamps,&data,&unit);

        out << "Unit is: " + unit + "\n";
        for(int i = 0; i<timestamps.count(); i++)
        {
            out << timestamps.at(i).toString("yyyy.MM.dd hh:mm:ss.zzz: ") + QString::number(data.at(i)) + "\n";
        }
    }
    else if(type == "float")
    {
        QList<float> data;
        parseBinary(binary_read_path,&timestamps,&data,&unit);

        out << "Unit is: " + unit + "\n";
        for(int i = 0; i<timestamps.count(); i++)
        {
            out << timestamps.at(i).toString("yyyy.MM.dd hh:mm:ss.zzz: ") + QString::number(data.at(i)) + "\n";
        }
    }
    else if(type == "str")
    {
        QList<QString> data;
        parseBinary(binary_read_path,&timestamps,&data,&unit);

        out << "Unit is: " + unit + "\n";
        for(int i = 0; i<timestamps.count(); i++)
        {
            out << timestamps.at(i).toString("yyyy.MM.dd hh:mm:ss.zzz: ") + data.at(i) + "\n";
        }

    }

    file_out.close();
}

bool LogFile::isOpen() const
{
    return file.isOpen();
}

template void LogFile::write(int);
template void LogFile::write(short);
template void LogFile::write(double);
template void LogFile::write(float);

template void LogFile::parseBinary(QString,QList<QDateTime>*,QList<int>*,QString*);
template void LogFile::parseBinary(QString,QList<QDateTime>*,QList<short>*,QString*);
template void LogFile::parseBinary(QString,QList<QDateTime>*,QList<float>*,QString*);
template void LogFile::parseBinary(QString,QList<QDateTime>*,QList<double>*,QString*);
template void LogFile::parseBinary(QString,QList<QDateTime>*,QList<QString>*,QString*);


