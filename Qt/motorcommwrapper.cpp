#include "motorcommwrapper.h"

MotorCommWrapper::MotorCommWrapper(Logger* log, QObject* parent) : QObject(parent)
{
    motor_comms.append(new MotorComm(log,"ControlBoard1",true));
    motor_comms.append(new MotorComm(log,"ControlBoard2",false));
    master_index = 0;
    this->log = log;

    connect(motor_comms.at(0),SIGNAL(addGraph(QString,QString,QString)),this,SIGNAL(addGraph(QString,QString,QString)));
    connect(motor_comms.at(0),SIGNAL(removeGraph(QString)),this,SIGNAL(removeGraph(QString)));
    connect(motor_comms.at(0),SIGNAL(plot(double,QString)),this,SIGNAL(plot(double,QString)));
    connect(motor_comms.at(0),SIGNAL(disconnected()),this,SIGNAL(disconnected1()));
    connect(motor_comms.at(0),SIGNAL(new_rssi(double)),this,SIGNAL(new_rssi1(double)));

    connect(motor_comms.at(1),SIGNAL(addGraph(QString,QString,QString)),this,SIGNAL(addGraph(QString,QString,QString)));
    connect(motor_comms.at(1),SIGNAL(removeGraph(QString)),this,SIGNAL(removeGraph(QString)));
    connect(motor_comms.at(1),SIGNAL(plot(double,QString)),this,SIGNAL(plot(double,QString)));
    connect(motor_comms.at(1),SIGNAL(disconnected()),this,SIGNAL(disconnected2()));
    connect(motor_comms.at(1),SIGNAL(new_rssi(double)),this,SIGNAL(new_rssi2(double)));
}

MotorCommWrapper::~MotorCommWrapper()
{
    for(int i = 0; i < motor_comms.length(); i++)
    {
        delete(motor_comms.at(i));
    }
}

void MotorCommWrapper::init(QString host1,QString host2, quint16 port)
{
    try
    {
        motor_comms.at(0)->init(host1,port);
        motor_comms.at(1)->init(host2,port);
    }
    catch(const std::exception& e)
    {
        log->error("Could not connect to control boards: " + QString::fromStdString(e.what()));
    }
}

void MotorCommWrapper::setMotorPower(char power)
{
    motor_comms.at(master_index)->setMotorPower(power);
}

bool MotorCommWrapper::isConnected()
{
    return motor_comms.at(0)->isConnected() && motor_comms.at(1)->isConnected();
}

void MotorCommWrapper::close()
{
    motor_comms.at(0)->close();
    motor_comms.at(1)->close();
}

void MotorCommWrapper::steer(double angle_rad)
{
    motor_comms.at(master_index)->steer(angle_rad);
}

void MotorCommWrapper::sendMotorPower(int direction)
{
   motor_comms.at(master_index)->sendMotorPower(direction);
}

void MotorCommWrapper::sendReplay()
{
    motor_comms.at(master_index)->sendReplay();
}

void MotorCommWrapper::sendSettings(float kp, float ki, float kd, unsigned period, unsigned limit,unsigned frequency,
                  bool sample_motor_power, bool sample_PID, bool sample_speed, bool sample_steering_angle, bool sample_rssi,
                  unsigned replay_length, unsigned replay_period)
{
    for(int i = 0; i < motor_comms.length(); i++)
    {
        motor_comms.at(i)->sendSettings(kp,ki,kd,period,limit,frequency,sample_motor_power,sample_PID,sample_speed,sample_steering_angle,sample_rssi,replay_length,replay_period);
    }
}

void MotorCommWrapper::sendMotorCompensation(float compensation)
{
    for(int i = 0; i < motor_comms.length(); i++)
    {
        motor_comms.at(i)->sendMotorCompensation(compensation);
    }
}

void MotorCommWrapper::sendSteeringCompensation(float compensation, char offs)
{
    for(int i = 0; i < motor_comms.length(); i++)
    {
        motor_comms.at(i)->sendSteeringCompensation(compensation,offs);
    }
}

void MotorCommWrapper::requestErrorReport()
{
    for(int i = 0; i < motor_comms.length(); i++)
    {
        motor_comms.at(i)->requestErrorReport();
    }
}

void MotorCommWrapper::ping(unsigned board_num, bool do_it, QString ip)
{
    motor_comms.at(board_num)->ping(do_it,ip);
}

void MotorCommWrapper::requestReset(unsigned board_num)
{
        motor_comms.at(board_num)->requestReset();
}
