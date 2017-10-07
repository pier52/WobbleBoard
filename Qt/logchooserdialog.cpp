#include "logchooserdialog.h"
#include "ui_logchooserdialog.h"
#include <QDir>

LogChooserDialog::LogChooserDialog(QString base_path,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogChooserDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->base_path = base_path;
    populateComboBoxWithDirs(base_path+"/logs",ui->yearComboBox);
    on_yearComboBox_currentIndexChanged(ui->yearComboBox->currentText());
}

LogChooserDialog::~LogChooserDialog()
{
    delete ui;
}

void LogChooserDialog::on_cancelButton_clicked()
{
    this->close();
}

void LogChooserDialog::populateComboBoxWithDirs(QString path, QComboBox* to_populate)
{
    QDir current_dir(path);
    QStringList dirs = current_dir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    to_populate->blockSignals(true);
    to_populate->clear();
    to_populate->addItems(dirs);

    to_populate->blockSignals(false);
}

void LogChooserDialog::populateComboBoxWithSessions(QString path, QComboBox *to_populate)
{
    QDir log_dir(path);

    QStringList logs = log_dir.entryList(QStringList("*.DATA*"), QDir::Files | QDir::NoSymLinks);
    QString session_time;
    QStringList session_times;

    for(int i = 0; i < logs.count(); i++)
    {
        session_time = logs.at(i).split("session_").at(1).split("]").at(0);
        if(!session_times.contains(session_time))
        {
            session_times.append(session_time);
        }

    }
    to_populate->blockSignals(true);
    to_populate->clear();
    to_populate->addItems(session_times);
    to_populate->blockSignals(false);
}

void LogChooserDialog::on_yearComboBox_currentIndexChanged(const QString &arg1)
{
    populateComboBoxWithDirs(base_path+"/logs/"+arg1,ui->monthComboBox);

    ui->dayComboBox->blockSignals(true);
    ui->dayComboBox->clear();
    ui->dayComboBox->blockSignals(false);

    ui->sessionComboBox->blockSignals(true);
    ui->sessionComboBox->clear();
    ui->sessionComboBox->blockSignals(false);

    on_monthComboBox_currentIndexChanged(ui->monthComboBox->currentText());
}

void LogChooserDialog::on_monthComboBox_currentIndexChanged(const QString &arg1)
{
    populateComboBoxWithDirs(base_path+"/logs/"+ui->yearComboBox->currentText()+"/" + arg1,ui->dayComboBox);

    ui->sessionComboBox->blockSignals(true);
    ui->sessionComboBox->clear();
    ui->sessionComboBox->blockSignals(false);

    on_dayComboBox_currentIndexChanged(ui->dayComboBox->currentText());
}

void LogChooserDialog::on_dayComboBox_currentIndexChanged(const QString &arg1)
{
    populateComboBoxWithSessions(base_path+"/logs/"+ui->yearComboBox->currentText()+"/" + ui->monthComboBox->currentText() + "/" + arg1,ui->sessionComboBox);
    on_sessionComboBox_currentIndexChanged(ui->sessionComboBox->currentText());
}


void LogChooserDialog::on_sessionComboBox_currentIndexChanged(const QString &arg1)
{
    QDir log_dir(base_path+"/logs/"+ui->yearComboBox->currentText()+"/" + ui->monthComboBox->currentText() + "/" + ui->dayComboBox->currentText());

    QStringList logs = log_dir.entryList(QStringList("*.DATA*"), QDir::Files | QDir::NoSymLinks);
    log_paths.clear();

    for(int i = 0; i < logs.count(); i++)
    {
        if(logs.at(i).contains(arg1))
            log_paths.append(log_dir.path() + "/" + logs.at(i));
    }

}

void LogChooserDialog::on_okButton_clicked()
{
    if(ui->sessionComboBox->currentIndex() >= 0)
        emit logPaths(log_paths);

    this->close();
}
