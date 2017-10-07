/**
 * \class MotorComm
 *
 *
 * \brief
 *      Handles the communication with the motor control board, via TCP or USB. Forwards sensor values to the plot window.
 *
 *
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef MOTORCOMM_H
#define MOTORCOMM_H

#include <QObject>
#include <QTcpSocket>
#include "logger.h"
#include <QTimer>
#include "networkpinger.h"

class MotorComm : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief MotorComm
     * Constructor for motor control board communication class
     * @param log
     * Pointer to logger class
     * @param board_name
     * Name of the board (preferably unique), e.g. "Control board", which is a way to distinguish which control board
     * data was sent from when logging etc.
     * @param parent
     */
    explicit MotorComm( Logger* log,QString board_name,bool is_master = true,QObject *parent = 0);
    ~MotorComm();

    /// Opens connection to the motor board
    void init(QString host, quint16 port);

    /// Sets the motor power (percentage). NOTE: does not send motor power to motor board
    void setMotorPower(char power);

    bool isConnected();
    void close();


signals:
    void disconnected();
    void plot(double value, QString graph_name);
    void new_rssi(double rssi);
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
    void ping(bool do_it, QString ip);
    void requestReset();


private slots:
    void on_tcpReceive();
    void on_disconnect();
    void on_connect();
    void on_pingResponse(double delay);

private:
    /// Puts wheels in angle <position>
    void steer(char position);

    void reconnect();
    void plotPWMVoltage(double voltage);
    void plotSteeringAngle(double angle);
    void plotWiFiRSSI(double rssi);
    void plotDebug(double debug);
    void plotAccel(double x, double y, double z);
    void plotGyro(double x, double y, double z);

    /// Converts float to byte array (4 bytes)
    void float2Bytes(unsigned char* bytes_temp,float float_variable);

    /// Converts short(int) to byte array (2 bytes)
    void short2Bytes(unsigned char* bytes_temp,short short_variable);


    QIODevice* socket;
    Logger* log;
    NetworkPinger* pinger;

    QString host;
    quint16 port;

    QString board_name;
    bool is_master;

    int steering_position;
    int motor_power;

    int old_motor_power = 0;
    int old_direction = 0;
    int old_steering_position = 0;
};

#endif // MOTORCOMM_H
