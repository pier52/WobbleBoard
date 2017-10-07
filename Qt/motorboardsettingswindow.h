/**
 * \class MotorBoardSettingsWindow
 *
 *
 * \brief
 *      Implements the logic behind the motor board settings form
 *
 * Stores and lets the user change things such as PID parameters, which data samples to be requested and communication parameters. /n
 * All fields are checked so they contain meaningful data when OK or Apply button is clicked. Default values for all fields are also sored in the class.
 *
 *
 * \author Samuel Orn
 *
 */


#ifndef MOTORBOARDSETTINGSWINDOW_H
#define MOTORBOARDSETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class MotorBoardSettingsWindow;
}

class MotorBoardSettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MotorBoardSettingsWindow(QWidget *parent = 0);
    MotorBoardSettingsWindow(unsigned motor_port,unsigned arduino_baud,QString motor_ip, QString motor_ip2,
                             float kp, float ki, float kd, unsigned period, unsigned limit,
                             unsigned sample_frequency, bool sample_motor_power, bool sample_steering_angle, bool sample_rssi,
                             unsigned replay_length, unsigned replay_period, bool pinging1,bool pinging2, QWidget *parent = 0);
    ~MotorBoardSettingsWindow();

    double getKp() const;
    double getKi() const;
    double getKd() const;
    unsigned getLimit() const;
    unsigned getPeriod() const;
    unsigned getMotorPort() const;
    unsigned getArduinoBaud() const;
    unsigned getSampleFrequency() const;
    unsigned getReplayLength() const;
    unsigned getReplayPeriod() const;
    bool getSampleMotorPower() const;
    bool getSampleSteeringAngle() const;
    bool getSampleRSSI() const;
    QString getMotorIP() const;
    QString getMotorIP2() const;
    bool isPinging1() const;
    bool isPinging2() const;
    void connectedToMotor(bool connected);
    bool apply();
    void sendToMotorBoard();


signals:
    void settingsChanged(float kp, float ki, float kd, unsigned period, unsigned limit,unsigned frequency,
                         bool sample_motor_power, bool sample_PID, bool sample_speed, bool sample_steering_angle, bool sample_rssi,
                         unsigned replay_length, unsigned replay_period);
    void addGraph(QString name, QString xaxis, QString yaxis);
    void removeGraph(QString name);
    void ping(bool do_it, QString ip);


private slots:


    void on_applyButton_clicked();

    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_defualtButton_clicked();

private:
    Ui::MotorBoardSettingsWindow *ui;
    void updateFields();


    float kp;
    float ki;
    float kd;
    /// The period (in ms), in which the PID runs
    unsigned period;
    /// The limit (0-100%), which the PID can put out. I.e. if limit = 80, output is capped at 80%.
    unsigned limit;

    const double DEFAULT_KP = 0;
    const double DEFAULT_KI = 0;
    const double DEFAULT_KD = 0;
    const unsigned DEFAULT_PERIOD = 100;
    const unsigned DEFAULT_LIMIT = 100;

    bool sample_motor_power;
    bool sample_PID;
    bool sample_speed;
    bool sample_steering_angle;
    bool sample_rssi;
    bool is_pinging1;
    bool is_pinging2;
    unsigned sample_frequency;
    unsigned replay_period;
    unsigned replay_length;

    unsigned motor_port;
    unsigned arduino_baud;
    QString motor_ip;
    QString motor_ip2;


    const unsigned DEFAULT_REPLAY_LENGTH = 10;
    const unsigned DEFAULT_REPLAY_PERIOD = 50;
    const unsigned DEFAULT_MOTOR_PORT = 2000;
    const unsigned DEFAULT_ARDUINO_BAUD= 115200;
    const QString DEFAULT_MOTOR_IP = "CMS-ROBOT-WIFI-1";

};

#endif // MOTORBOARDSETTINGSWINDOW_H
