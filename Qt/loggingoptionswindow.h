/**
 * \class LoggingOptionsWindow
 *
 *
 * \brief
 *      Window which lets the user set logging options.
 *
 * \author Samuel Orn
 *
 */

#ifndef LOGGINGOPTIONSWINDOW_H
#define LOGGINGOPTIONSWINDOW_H

#include <QDialog>
#include <QMap>
#include <QCheckBox>
#include <QMenu>
#include "popupmenu.h"

namespace Ui {
class LoggingOptionsWindow;
}

class LoggingOptionsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoggingOptionsWindow(QMap<QString,bool> logs_enabled,QString path,int clear_at_startup,QWidget *parent = 0);
    ~LoggingOptionsWindow();
    QString getPath() const;
    QMap<QString,QVariant> getLogList() const;
    int getStartupClearRule() const;
    QStringList getCheckedLogNames();

public slots:
    /**
     * @brief
     * Adds a data log to the data log menu.
     * @param name
     * Name of the log.
     */
    void addDataLog(QString name);

    /**
     * @brief
     * Same as show(), but saves the status of all fields using saveFields() before window is shown.
     */
    void showAndRemeberStatus();

signals:
    void clear(QStringList names, int older_than);
    void save(QStringList names);

private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_dataButton_clicked();

    void on_dataCheckBox_clicked(bool checked);

    void nestedDataCheckBox_clicked(bool checked);

    void on_clearButton_clicked();

    void on_choosePathButton_clicked();

    void on_openFolderButton_clicked();

    void on_visualiserButton_clicked();

    void plotLogs(QStringList paths);

    void on_toASCIIButton_clicked();

private:
    /// Returns true if <key> is any of the static keys "Error", "Warning" or "Info"
    bool isStaticKey(QString key);

    /// Saves the current settings in the fields
    void saveFields();

    /// Restores the settings which were in the fields when saveFields() was called.
    void restoreFields();

    /**
     * @brief
     * Conversts a QStringList to a QString
     * @param list
     * The QStringList
     * @param separator
     * Which separator to use between elements in the QStringList
     * @return
     * Merged QString, containing all elements in <list>, separated by <separator>
     */
    QString QStringList2QString(QStringList list, QString separator = "\n");

    QMap<QString,bool> last_session_logs;
    QStringList log_status_at_show;
    QString path_at_show;
    int clear_rule_at_show;

    Ui::LoggingOptionsWindow *ui;
    QMap<QString,QCheckBox*> logs;
    PopupMenu* data_logs;
};

#endif // LOGGINGOPTIONSWINDOW_H
