#include "motorboardsettingswindow.h"
#include "ui_motorboardsettingswindow.h"
#include <QMessageBox>

MotorBoardSettingsWindow::MotorBoardSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotorBoardSettingsWindow)
{
    ui->setupUi(this);
    on_defualtButton_clicked();
}

MotorBoardSettingsWindow::MotorBoardSettingsWindow(unsigned motor_port, unsigned arduino_baud, QString motor_ip,QString motor_ip2,
                                                   float kp, float ki, float kd, unsigned period, unsigned limit,
                                                   unsigned sample_frequency, bool sample_motor_power, bool sample_steering_angle, bool sample_rssi,
                                                   unsigned replay_length, unsigned replay_period, bool pinging1,bool pinging2, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotorBoardSettingsWindow)
{
    ui->setupUi(this);
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    this->period = period;
    this->limit = limit;

    this->motor_ip = motor_ip;
    this->motor_ip2 = motor_ip2;
    this->motor_port = motor_port;
    this->arduino_baud = arduino_baud;

    this->sample_frequency = sample_frequency;
    this->sample_motor_power = sample_motor_power;
    this->sample_steering_angle = sample_steering_angle;
    this->sample_rssi = sample_rssi;

    this->replay_length = replay_length;
    this->replay_period = replay_period;

    this->is_pinging1 = pinging1;
    this->is_pinging2 = pinging2;

    updateFields();
}

MotorBoardSettingsWindow::~MotorBoardSettingsWindow()
{
    delete ui;
}

bool MotorBoardSettingsWindow::apply()
{
    bool all_ok = true;
    bool ok;

    kp = ui->kpEdit->text().toFloat(&ok);
    all_ok = all_ok && ok;
    ki = ui->kiEdit->text().toFloat(&ok);
    all_ok = all_ok && ok;
    kd = ui->kdEdit->text().toFloat(&ok);
    all_ok = all_ok && ok;
    period = ui->periodEdit->text().toUInt(&ok);
    all_ok = all_ok && ok;
    limit = ui->limitEdit->text().toUInt(&ok);
    if(!(ok && limit <= 100))
        all_ok = false;

    motor_port = ui->motorPortEdit->text().toUInt(&ok);
    if(!(ok && limit <= 9999))
        all_ok = false;

    replay_length = ui->replayLengthEdit->text().toUInt(&ok);
    replay_period = ui->replayIntervalEdit->text().toUInt(&ok);

    arduino_baud = ui->arduinoBaudEdit->text().toUInt(&ok);
    all_ok = all_ok && ok;

    motor_ip = ui->motorIPEdit->text();
    motor_ip2 = ui->motorIPEdit_2->text();

    sample_motor_power = ui->motorPowerCheckBox->isChecked();
    sample_PID = ui->PIDCheckBox->isChecked();
    sample_speed = ui->SpeedCheckBox->isChecked();
    sample_steering_angle = ui->SteeringAngleCheckBox->isChecked();
    sample_rssi = ui->RSSICheckBox->isChecked();
    sample_frequency = ui->frequencySpinBox->value();

    if(!all_ok)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Input error");
        msgBox.setText("Error: invalid input. PID constants must be numbers, period must be a positive integer, limit must be an integer between 0 and 100");
        msgBox.exec();
    }

    is_pinging1 = ui->pingCheckBox->isChecked();
    emit ping(is_pinging1,motor_ip);

    is_pinging2 = ui->pingCheckBox->isChecked();
    emit ping(is_pinging2,motor_ip2);

    return all_ok;
}

void MotorBoardSettingsWindow::sendToMotorBoard()
{
    emit settingsChanged(kp,ki,kd,period,limit,sample_frequency,sample_motor_power, sample_PID, sample_speed, sample_steering_angle, sample_rssi, replay_length, replay_period);

    if(sample_motor_power)
    {
        emit addGraph("Motor power","Time","%");
    }
    else
    {
        emit removeGraph("Motor power");
    }

    if(sample_steering_angle)
    {
        emit addGraph("Steering angle","Time","Degrees");
    }
    else
    {
        emit removeGraph("Steering angle");
    }

    if(sample_rssi)
    {
        emit addGraph(motor_ip + " WiFi RSSI","Time","RSSI");
    }
    else
    {
        emit removeGraph("Motorboard WiFi RSSI");
    }
}

void MotorBoardSettingsWindow::updateFields()
{
    ui->kpEdit->setText(QString::number(kp));
    ui->kiEdit->setText(QString::number(ki));
    ui->kdEdit->setText(QString::number(kd));

    ui->limitEdit->setText(QString::number(limit));
    ui->periodEdit->setText(QString::number(period));

    ui->motorPortEdit->setText(QString::number(motor_port));
    ui->arduinoBaudEdit->setText(QString::number(arduino_baud));
    ui->motorIPEdit->setText(motor_ip);
    ui->motorIPEdit_2->setText(motor_ip2);

    ui->SteeringAngleCheckBox->setChecked(sample_steering_angle);
    ui->motorPowerCheckBox->setChecked(sample_motor_power);
    ui->frequencySpinBox->setValue(sample_frequency);
    ui->RSSICheckBox->setChecked(sample_rssi);

    ui->replayIntervalEdit->setText(QString::number(replay_period));
    ui->replayLengthEdit->setText(QString::number(replay_length));

    ui->pingCheckBox->setChecked(is_pinging1);
    ui->pingCheckBox_2->setChecked(is_pinging2);
}

double MotorBoardSettingsWindow::getKp() const
{
    return kp;
}

double MotorBoardSettingsWindow::getKi() const
{
    return ki;
}

double MotorBoardSettingsWindow::getKd() const
{
    return kd;
}

unsigned MotorBoardSettingsWindow::getLimit() const
{
    return limit;
}

unsigned MotorBoardSettingsWindow::getPeriod() const
{
    return period;
}

unsigned MotorBoardSettingsWindow::getReplayLength() const
{
    return replay_length;
}

unsigned MotorBoardSettingsWindow::getReplayPeriod() const
{
    return replay_period;
}

void MotorBoardSettingsWindow::on_applyButton_clicked()
{
    apply();
    sendToMotorBoard();
}

void MotorBoardSettingsWindow::on_okButton_clicked()
{
    if(apply())
    {
        sendToMotorBoard();
        this->hide();
    }
}

void MotorBoardSettingsWindow::on_cancelButton_clicked()
{
    updateFields();
    this->hide();
}

void MotorBoardSettingsWindow::on_defualtButton_clicked()
{
    ui->kpEdit->setText(QString::number(DEFAULT_KP));
    ui->kdEdit->setText(QString::number(DEFAULT_KD));
    ui->kiEdit->setText(QString::number(DEFAULT_KI));

    ui->limitEdit->setText(QString::number(DEFAULT_LIMIT));
    ui->periodEdit->setText(QString::number(DEFAULT_PERIOD));

    ui->motorIPEdit->setText(DEFAULT_MOTOR_IP);
    ui->motorPortEdit->setText(QString::number(DEFAULT_MOTOR_PORT));
    ui->arduinoBaudEdit->setText(QString::number(DEFAULT_ARDUINO_BAUD));

    ui->replayLengthEdit->setText(QString::number(DEFAULT_REPLAY_LENGTH));
    ui->replayIntervalEdit->setText(QString::number(DEFAULT_REPLAY_PERIOD));

    ui->pingCheckBox->setChecked(false);
}

unsigned MotorBoardSettingsWindow::getMotorPort() const
{
    return motor_port;
}

QString MotorBoardSettingsWindow::getMotorIP() const
{
    return motor_ip;
}

QString MotorBoardSettingsWindow::getMotorIP2() const
{
    return motor_ip2;
}

void MotorBoardSettingsWindow::connectedToMotor(bool connected)
{
    ui->motorIPEdit->setEnabled(!connected);
    ui->motorPortEdit->setEnabled(!connected);
    ui->arduinoBaudEdit->setEnabled(!connected);
}

unsigned MotorBoardSettingsWindow::getArduinoBaud() const
{
    return arduino_baud;
}

unsigned MotorBoardSettingsWindow::getSampleFrequency() const
{
    return sample_frequency;
}

bool MotorBoardSettingsWindow::getSampleMotorPower() const
{
    return sample_motor_power;
}

bool MotorBoardSettingsWindow::getSampleSteeringAngle() const
{
    return sample_steering_angle;
}

bool MotorBoardSettingsWindow::getSampleRSSI() const
{
    return sample_rssi;
}

bool MotorBoardSettingsWindow::isPinging1() const
{
    return is_pinging1;
}

bool MotorBoardSettingsWindow::isPinging2() const
{
    return is_pinging2;
}
