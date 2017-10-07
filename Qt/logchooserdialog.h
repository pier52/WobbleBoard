/**
 * \class LogChooserDialog
 *
 *
 * \brief
 *      Dialog which lets the user choose a log session via comboboxes
 *
 * 4 comboboxes are presented: Year, Month, Day and Session. These are populated according to which
 * data logs are available in the log path.
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef LOGCHOOSERDIALOG_H
#define LOGCHOOSERDIALOG_H

#include <QDialog>
#include <QComboBox>

namespace Ui {
class LogChooserDialog;
}

class LogChooserDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief
     * @param base_path
     * The path to the log folder, which is searched through for logs
     */
    explicit LogChooserDialog(QString base_path, QWidget *parent = 0);
    ~LogChooserDialog();

signals:
    void logPaths(QStringList paths);

private slots:
    void on_cancelButton_clicked();

    void on_yearComboBox_currentIndexChanged(const QString &arg1);

    void on_monthComboBox_currentIndexChanged(const QString &arg1);

    void on_dayComboBox_currentIndexChanged(const QString &arg1);

    void on_sessionComboBox_currentIndexChanged(const QString &arg1);

    void on_okButton_clicked();

private:
    void populateComboBoxWithDirs(QString path, QComboBox* to_populate);
    void populateComboBoxWithSessions(QString path, QComboBox* to_populate);


    Ui::LogChooserDialog *ui;
    QString base_path;
    QStringList log_paths;
};

#endif // LOGCHOOSERDIALOG_H
