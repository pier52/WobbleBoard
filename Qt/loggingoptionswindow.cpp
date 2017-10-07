#include <QWidgetAction>

#include "loggingoptionswindow.h"
#include "ui_loggingoptionswindow.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include "logchooserdialog.h"
#include "logviewer.h"
#include "logfile.h"


LoggingOptionsWindow::LoggingOptionsWindow(QMap<QString,bool> logs_enabled,QString path,int clear_at_startup,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoggingOptionsWindow)
{
    ui->setupUi(this);
    data_logs = new PopupMenu(ui->dataButton, this);
    ui->dataButton->setMenu(data_logs);

    ui->pathEdit->setText(path);
    ui->clearRuleComboBox->setCurrentIndex(clear_at_startup);

    last_session_logs = logs_enabled;

    logs.insert("Error",ui->errorCheckBox);
    logs.insert("Warning",ui->warningCheckBox);
    logs.insert("Info",ui->infoCheckBox);
    logs.insert("Data",ui->dataCheckBox);

    QMapIterator<QString, QCheckBox *> i(logs);
    while(i.hasNext())
    {
        i.next();
        if(logs_enabled.contains(i.key()))
        {
            i.value()->setChecked(logs_enabled.value(i.key()));
        }
    }
}

LoggingOptionsWindow::~LoggingOptionsWindow()
{
    delete ui;
}

void LoggingOptionsWindow::addDataLog(QString name)
{
    if(logs.contains(name))
    {
        throw std::runtime_error("Such a log already exists");
    }

    QCheckBox* newBox = new QCheckBox(data_logs);
    newBox->setText(name);
    if(last_session_logs.contains(name))
    {
       newBox->setChecked(last_session_logs.value(name));
    }
    else
    {
       newBox->setChecked(ui->dataCheckBox->isChecked());
    }

    logs.insert(name,newBox);

    QWidgetAction *action = new QWidgetAction(data_logs);
    action->setDefaultWidget(newBox);
    data_logs->addAction(action);

    connect(newBox,SIGNAL(clicked(bool)),this,SLOT(nestedDataCheckBox_clicked(bool)));
}

void LoggingOptionsWindow::on_okButton_clicked()
{
    emit save(getCheckedLogNames());
    this->hide();
}

void LoggingOptionsWindow::on_cancelButton_clicked()
{
    restoreFields();
    this->close();
}

void LoggingOptionsWindow::on_dataButton_clicked()
{
    data_logs->show();
}

bool LoggingOptionsWindow::isStaticKey(QString key)
{
    return key == "Error" || key == "Warning" || key == "Info" || key == "Data";
}

void LoggingOptionsWindow::on_dataCheckBox_clicked(bool checked)
{
    QMapIterator<QString, QCheckBox *> i(logs);
    while(i.hasNext())
    {
        i.next();
        if(isStaticKey(i.key()))
            continue;

        i.value()->setChecked(checked);
    }
}

void LoggingOptionsWindow::nestedDataCheckBox_clicked(bool checked)
{
    if(checked)
    {
        ui->dataCheckBox->setChecked(true);
    }
    else // If all unchecked, uncheck data checkbox
    {
        QMapIterator<QString, QCheckBox *> i(logs);
        bool any_checked = false;
        while(i.hasNext())
        {
            i.next();
            if(isStaticKey(i.key()))
                continue;

            any_checked = any_checked || i.value()->isChecked();
        }
        ui->dataCheckBox->setChecked(any_checked);
    }

}
QStringList LoggingOptionsWindow::getCheckedLogNames()
{
    QStringList return_list;

    QMapIterator<QString, QCheckBox *> i(logs);
    while(i.hasNext())
    {
        i.next();
        if(i.value()->isChecked())
        {
            return_list.append(i.key());
        }
    }

    return return_list;
}

QString LoggingOptionsWindow::QStringList2QString(QStringList list, QString separator)
{
    QString out = "";
    for(int i=0; i<list.count(); i++)
    {
        out += list.at(i)+separator;
    }

    return out;
}

void LoggingOptionsWindow::on_clearButton_clicked()
{
    QMessageBox msgBox;
    QStringList checked_logs = getCheckedLogNames();
    checked_logs.removeOne("Data");

    if(checked_logs.empty())
        return;

    QString older_than = ui->clearSelectedComboBox->currentText();
    msgBox.setWindowTitle("Clear logs?");
    msgBox.setText("Are you sure you want to clear the following logs with rule: " + older_than + "? \n\n" + QStringList2QString(checked_logs));
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() == QMessageBox::Yes){
        switch(ui->clearSelectedComboBox->currentIndex())
        {
        case 0:
            emit clear(checked_logs,0);
            break;
        case 1:
            emit clear(checked_logs,1);
            break;
        case 2:
            emit clear(checked_logs,7);
            break;
        case 3:
            emit clear(checked_logs,30);
            break;
        default:
            break;
        }
    }
}

void LoggingOptionsWindow::on_choosePathButton_clicked()
{
    QString open_path = ui->pathEdit->text();
    if(open_path == "")
    {
        open_path = QDir::currentPath();
    }

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                open_path,
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    if(dir != "")
        ui->pathEdit->setText(dir);

}

void LoggingOptionsWindow::on_openFolderButton_clicked()
{
    QString to_open;
    if(ui->pathEdit->text() == "")
    {
        to_open = QDir::currentPath()+"/logs";
    }
    else
    {
        to_open = ui->pathEdit->text()+"/logs";
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(to_open));
}

QString LoggingOptionsWindow::getPath() const
{
    return ui->pathEdit->text();
}

int LoggingOptionsWindow::getStartupClearRule() const
{
    return ui->clearRuleComboBox->currentIndex();
}

void LoggingOptionsWindow::showAndRemeberStatus()
{
    log_status_at_show = getCheckedLogNames();
    path_at_show = getPath();
    clear_rule_at_show = getStartupClearRule();
    show();
}

void LoggingOptionsWindow::restoreFields()
{
    ui->pathEdit->setText(path_at_show);
    ui->clearRuleComboBox->setCurrentIndex(clear_rule_at_show);
    QMapIterator<QString, QCheckBox *> i(logs);
    while(i.hasNext())
    {
        i.next();
        if(log_status_at_show.contains(i.key()))
        {
            i.value()->setChecked(true);
        }
        else
        {
            i.value()->setChecked(false);
        }
    }
}

QMap<QString,QVariant> LoggingOptionsWindow::getLogList() const
{
    QMap<QString,QVariant> map;

    QMapIterator<QString, QCheckBox *> i(logs);
    while(i.hasNext())
    {
        i.next();

        map.insert(i.key(),QVariant(i.value()->isChecked()));
    }

    return map;
}

void LoggingOptionsWindow::on_visualiserButton_clicked()
{   
    QString path = ui->pathEdit->text();
    if(path == "")
    {
      path = QDir::currentPath();
    }

    LogChooserDialog* log_chooser = new LogChooserDialog(path);

    connect(log_chooser,SIGNAL(logPaths(QStringList)),this,SLOT(plotLogs(QStringList)));

    log_chooser->show();
}

void LoggingOptionsWindow::plotLogs(QStringList paths)
{
    LogViewer* viewer = new LogViewer();

    try
    {
        viewer->loadLogs(paths);
    }
    catch (const std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText("An error occured when trying to load logs into plot: " + QString::fromStdString(e.what()));
        msgBox.exec();
        delete viewer;
        return;
    }

    viewer->show();
}

void LoggingOptionsWindow::on_toASCIIButton_clicked()
{
    QString to_open;
    if(ui->pathEdit->text() == "")
    {
        to_open = "logs";
    }
    else
    {
        to_open = ui->pathEdit->text()+"/logs";
    }

    QString file = QFileDialog::getOpenFileName(this,
        tr("Choose binary log"), to_open, tr("Binary log files (*.bin)"));

    if(!file.isEmpty() && !file.isNull())
    {
        try
        {
            LogFile::toASCIIFile(file);
        }
        catch (const std::exception& e)
        {
            QMessageBox msgBox;
            msgBox.setText("An error occured when trying to decode the binary file:" + QString::fromStdString(e.what()));
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Error: could not read from the specified file.");
        msgBox.exec();
    }
}
