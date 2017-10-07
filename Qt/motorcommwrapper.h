#ifndef MOTORCOMMWRAPPER_H
#define MOTORCOMMWRAPPER_H

#include <QObject>
#include "MotorComm.h"
#include "logger.h"

class MotorCommWrapper: public QObject
{
    Q_OBJECT

public:
    MotorCommWrapper(Logger* log, QObject* parent);
    ~MotorCommWrapper();

    /// Opens connection to the motor board
    void init(QString host1,QString host2, quint16 port);

    /// Sets the motor power (percentage). NOTE: does not send motor power to motor board
    void setMotorPower(char power);

    bool isConnected();
    void close();


signals:
    void disconnected1();
    void disconnected2();
    void plot(double value, QString graph_name);
    void new_rssi1(double rssi);
    void new_rssi2(double rssi);
    void addGraph(QString graph_name,QString x_axis, QString y_axis);
    void removeGraph(QString graph_name);


public slots:
    void steer(double angle_rad);
    void sendMotorPower(int direction);
    void sendReplay();
    void sendSettings(float kp, float ki, float kd, unsigned period, unsigned limit,unsigned frequency,
                      bool sample_motor_power, bool sample_PID, bool sample_speed, bool sample_steering_angle, bool sample_rssi,
                      unsigned replay_length, unsigned replay_period);
    void sendMotorCompensation(float compensation);
    void sendSteeringCompensation(float compensation, char offs);
    void requestErrorReport();
    void ping(unsigned board_num,bool do_it, QString ip);
    void requestReset(unsigned board_num);

private:
    QList<MotorComm*> motor_comms;
    unsigned master_index;
    Logger* log;

};

#endif // MOTORCOMMWRAPPER_H
