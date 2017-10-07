/**
 * \class LogFile
 *
 *
 * \brief
 *      Writes messages to log files, using either QTextStream (ASCII format) or QDataStream (binary format). Also contains methods for parsing.
 *
 * The type of log (string, int, double etc.) and whether it should be logged binary or as ASCII is chosen in the constructor. The log can then be
 * written to, opened and closed via public functions. \n
 * Parsing of log files can be done using public static methods.
 * 
 * 
 * \author Samuel Orn
 *
 */

#ifndef LOGFILE_H
#define LOGFILE_H

#include <QString>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include "logenum.h"

class LogFile
{
public:
    /**
     * @brief
     * @param path
     * Path to where the log file is to be saved. Not that this class will add the suffix ".<type>.<encoding>" to the file name.
     * E.g. the path name for a binary int log will be: <path>.int.bin or for ASCII double: <path>.double.asc
     * @param binary
     * Whether the logging should be binary or not
     * @param type
     * The data type of the log: float, int etc.
     * @param unit
     * The unit of the stored data e.g. "Volt"
     */
    LogFile(QString path, bool binary, log_type::data_type type ,QString unit = "[-]");

    ~LogFile();

    template <class T>
    void write(const T data);
    void write(QString data);

    void open();
    void close();
    bool isOpen() const;

    /**
     * @brief
     * Parses a binary file.
     * @param path
     * Path to the file.
     * @param timestamps
     * The parser stores the timestamps in this list.
     * @param data
     * The parser stores the data in this list. The type most correspond to the log type.
     * @param unit
     * The parser stores what unit the data has in this string.
     */
    template <class T> static void parseBinary(QString path, QList<QDateTime>* timestamps, QList<T>* data, QString* unit);

    static void parseASCII(QString path, QList<QDateTime>* timestamps, QList<int>* data, QString* unit);
    static void parseASCII(QString path, QList<QDateTime>* timestamps, QList<short>* data, QString* unit);
    static void parseASCII(QString path, QList<QDateTime>* timestamps, QList<double>* data, QString* unit);
    static void parseASCII(QString path, QList<QDateTime>* timestamps, QList<float>* data, QString* unit);
    static void parseASCII(QString path, QList<QDateTime>* timestamps, QList<QString>* data, QString* unit);

    /**
     * @brief
     * Takes the path to a binary log file, and converts it to an ascii log file, readable with any texteditor
     * @param binary_read_path
     * Path to the binary file
     * @param acii_write_path
     * Path to the ascii file to be written. If left empty, the ascii file will have the same name as the binary file
     * but with .asc ending instead of .bin
     */
    static void toASCIIFile(const QString& binary_read_path, QString acii_write_path = "");

private:

    /// Returns true if the data provided matches the data type given in the constructor, otherwise false
    template <class T> bool isCorrectType(const T data) const;

    QFile file;
    QString unit;
    bool binary;
    log_type::data_type type;
};

#endif // LOGFILE_H
