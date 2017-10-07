#include "MotorComm.h"
#include <QSerialPort>
#include <cstring>


/******************************************************************************
 *                             CONSTRUCTORS                                   *
 *****************************************************************************/

MotorComm::MotorComm(Logger* log, QString board_name,bool is_master, QObject *parent) : QObject(parent)
{
    socket = nullptr;
    this->log=log;
    steering_position = 0;
    motor_power = 0;
    old_motor_power = 0;
    pinger = new NetworkPinger(log);

    this->board_name = board_name;
    this->is_master = is_master;

    connect(pinger,SIGNAL(response(double)),this,SLOT(on_pingResponse(double)));

    //connect(sample_timer,SIGNAL(timeout()),this,SLOT(requestSamples()));

    log->addDataLog(board_name + " WiFi RSSI", "dBm", log_type::type_short);
    log->addDataLog(board_name + " ping","ms", log_type::type_float);
    log->addDataLog("Steering angle", "degrees", log_type::type_short);
    log->addDataLog("Motor power","%", log_type::type_short);
    log->addDataLog("Debug","value", log_type::type_int);
}

MotorComm::~MotorComm()
{
    close();
    socket->deleteLater();
    pinger->deleteLater();
}



/******************************************************************************
 *                               PUBLIC                                       *
 *****************************************************************************/

void MotorComm::init(QString host, quint16 port)
{
    this->port = port;
    this->host = host;
    QString usb_port;
    QString linux_msg = "";

    // Standard serial port name depends on operating system
    #if defined(Q_OS_WIN)
        usb_port = "COM";
    #elif defined(Q_OS_LINUX)
        usb_port = "tty";
        linux_msg = " Might have to run chmod 666 /dev/"+ host + " to get permission";
    #endif

    if(!host.contains(usb_port))
    {
        socket = new QTcpSocket();
        connect(dynamic_cast<QTcpSocket*>(socket),SIGNAL(connected()),this,SLOT(on_connect()));

        dynamic_cast<QTcpSocket*>(socket)->connectToHost(host, port);

        if (!dynamic_cast<QTcpSocket*>(socket)->waitForConnected(5000)) {
            std::string msg = socket->errorString().toStdString();
            delete socket;
            socket = nullptr;
            throw std::runtime_error(msg);
            return;
        }

        /*
        // Notify motorboard which Serial connection to use for communication
        char msg[4];
        msg[0]= '#';
        msg[1]= 'c';
        msg[2]= 1;
        msg[3]= '\r';
        socket->write(msg,4);
        */
    }
    else
    {
        socket = new QSerialPort();
        dynamic_cast<QSerialPort*>(socket)->setBaudRate(port);
        dynamic_cast<QSerialPort*>(socket)->setPortName(host);
        if (!dynamic_cast<QSerialPort*>(socket)->open(QIODevice::ReadWrite)) {
            throw std::runtime_error(socket->errorString().toStdString() + linux_msg.toStdString());

        }

        /*
        // Notify motorboard which Serial connection to use for communication
        char msg[4];
        msg[0]= '#';
        msg[1]= 'c';
        msg[2]= 0;
        msg[3]= '\r';
        socket->write(msg,4);
        */

    }

    emit addGraph("Accelerometer x","Time","AccXYZ");
    emit addGraph("Accelerometer y","Time","AccXYZ");
    emit addGraph("Accelerometer z","Time","AccXYZ");
    emit addGraph("Gyro x","Time","GyroXYZ");
    emit addGraph("Gyro y","Time","GyroXYZ");
    emit addGraph("Gyro z","Time","GyroXYZ");
    connect(socket,SIGNAL(readyRead()),this,SLOT(on_tcpReceive()));

}

void MotorComm::setMotorPower(char power)
{
    motor_power = power;
}

void MotorComm::sendMotorPower(int direction)
{
    if((direction == old_direction) && (direction == 0))
    {
        return;
    }

    if(!is_master)
    {
        log->error("Motorboard slave not allowed to send motor power");
        return;
    }

    if(!isConnected())
        return;

    char msg[4];
    msg[0]= '#';
    msg[1]= 'm';
    if(direction < 0)
        msg[2]= -motor_power;
    else if(direction == 0)
        msg[2]= 0;
    else
        msg[2]= motor_power;
    msg[3]= '\r';
    try
    {
        socket->write(msg,4);
    }
    catch(const std::exception& e)
    {
        log->error(board_name + ": Couldn't send motor power: " + QString::fromStdString(e.what()));
        return;
    }

    if(!((motor_power == old_motor_power)&& direction == old_direction ))
    {
        qDebug("Sent motor power: %d", msg[2]);
    }
    old_motor_power = motor_power;
    old_direction = direction;
}

void MotorComm::steer(char position)
{
    if(!is_master)
    {
        log->error("Motorboard slave not allowed to send steering angle");
        return;
    }

    if(!isConnected())
        return;

    if(position == steering_position)
        return;

    char msg[4];
    msg[0]= '#';
    msg[1]= 's';
    msg[2]= position;
    msg[3]= '\r';
    try
    {
        socket->write(msg,4);
    }
    catch(const std::exception& e)
    {
        log->error(board_name + ": Couldn't send steering angle: " + QString::fromStdString(e.what()));
        return;
    }

    qDebug("Steering to position: %d", msg[2]);
    steering_position = position;
}

void MotorComm::requestReset()
{
    if(!isConnected())
        return;

    char msg[3];
    msg[0]= '#';
    msg[1]= 'q';
    msg[2]= '\r';

    try
    {
        socket->write(msg,4);
    }
    catch(const std::exception& e)
    {
        log->error(board_name + ": Couldn't send reset request: " + QString::fromStdString(e.what()));
    }

}

void MotorComm::sendSettings(float kp, float ki, float kd, unsigned period, unsigned limit,
                             unsigned frequency, bool sample_motor_power, bool sample_PID, bool sample_speed, bool sample_steering_angle,bool sample_rssi,
                             unsigned replay_length, unsigned replay_period)
{
    if(isConnected())
    {
        unsigned char float_conv[4];

        char send[26];
        send[0] = '#';
        send[1] = 'd';

        float2Bytes(float_conv,kp);
        send[2] = float_conv[0];
        send[3] = float_conv[1];
        send[4] = float_conv[2];
        send[5] = float_conv[3];

        float2Bytes(float_conv,ki);
        send[6] = float_conv[0];
        send[7] = float_conv[1];
        send[8] = float_conv[2];
        send[9] = float_conv[3];

        float2Bytes(float_conv,kd);
        send[10] = float_conv[0];
        send[11] = float_conv[1];
        send[12] = float_conv[2];
        send[13] = float_conv[3];

        short2Bytes(float_conv,(short) period);
        send[14] = float_conv[0];
        send[15] = float_conv[1];


        send[16] = (unsigned char) limit;
        send[17] = (unsigned char) frequency;
        send[18] = (unsigned char) sample_motor_power;
        send[19] = (unsigned char) sample_PID;
        send[20] = (unsigned char) sample_speed;
        send[21] = (unsigned char) sample_steering_angle;
        send[22] = (unsigned char) replay_length;
        send[23] = (unsigned char) replay_period;
        send[24] = (unsigned char) sample_rssi;
        send[25] = '\r';

        try
        {
            socket->write(send,26);
        }
        catch(const std::exception& e)
        {
            log->error(board_name + ": Couldn't send settings: " + QString::fromStdString(e.what()));
        }
    }
}

void MotorComm::sendMotorCompensation(float compensation)
{
    unsigned char float_conv[4];
    char send[7];

    float2Bytes(float_conv,compensation);

    send[0] = '#';
    send[1] = 'p';
    send[2] = float_conv[0];
    send[3] = float_conv[1];
    send[4] = float_conv[2];
    send[5] = float_conv[3];
    send[6] = '\r';

    try
    {
        socket->write(send,7);
    }
    catch(const std::exception& e)
    {
        log->error(board_name + ": Couldn't send motor compensation: " + QString::fromStdString(e.what()));
    }
}

void MotorComm::requestErrorReport()
{
    char send[3];

    send[0] = '#';
    send[1] = 'e';
    send[2] = '\r';

    try
    {
        socket->write(send,3);
    }
    catch(const std::exception& e)
    {
        log->error(board_name + ": Couldn't send request error report: " + QString::fromStdString(e.what()));
    }
}

void MotorComm::sendSteeringCompensation(float compensation,char offs)
{
    unsigned char float_conv[4];
    char send[8];

    float2Bytes(float_conv,compensation);

    send[0] = '#';
    send[1] = 'q';
    send[2] = float_conv[0];
    send[3] = float_conv[1];
    send[4] = float_conv[2];
    send[5] = float_conv[3];
    send[6] = offs;
    send[7] = '\r';

    try
    {
        socket->write(send,8);
    }
    catch(const std::exception& e)
    {
        log->error(board_name + ": Couldn't send steering compensation: " + QString::fromStdString(e.what()));
    }
}

void MotorComm::steer(double angle_rad)
{
    double angle_deg = angle_rad * 180/M_PI;

    if(angle_deg > 90)
    {
        angle_deg = 90;
    }
    else if(angle_deg < -90)
    {
        angle_deg = -90;
    }

    steer((char) angle_deg);
}

void MotorComm::ping(bool do_it, QString ip)
{
    if(do_it)
    {
        pinger->startPinging(ip);
        emit addGraph(board_name+ " ping","Time", "Response time [ms]");
    }
    else
    {
        pinger->stopPinging();
        emit removeGraph(board_name + " ping");
    }
}

bool MotorComm::isConnected()
{
    if(socket == nullptr)
        return false;

    return socket->isOpen();
}



/******************************************************************************
 *                            PRIVATE SLOTS                                   *
 *****************************************************************************/

void MotorComm::on_disconnect()
{
    socket->deleteLater();
    emit disconnected();
}

void MotorComm::on_tcpReceive()
{
    char line[200];

    while(socket->canReadLine())
    {
        socket->readLine(line,200);

        QByteArray msg = QByteArray(line);
        //log->info("Got a new message from motor board: " + msg.toStdString());

        msg.chop(2); // remove carriage return and newline

        switch (msg[0]) {

        case '*': //incoming sensor data, format "*key*data"
        {
            msg.remove(0,1); // remove first tag

            if(!msg.contains('*')) //if second tag is not present: broken message
            {
                log->warning(board_name + ":: Broken sensor data: " + QString::fromStdString(msg.toStdString()));
                continue;
            }

            std::string key;
            QByteArray data;
            try
            {
            QList<QByteArray> space_split = msg.split('*');
            key = space_split.at(0).toStdString();
            data = space_split.at(1);
            }
            catch(const std::exception& e)
            {
                log->error(board_name + ": Couldn't decode incoming sensor data: " + QString::fromStdString(e.what()));
                return;
            }

            if(key.compare("m") == 0)
            {
                plotPWMVoltage((double)data.toInt());
            }
            else if(key.compare("s") == 0)
            {
                plotSteeringAngle((double)data.toInt());
            }
            else if(key.compare("r") == 0)
            {
                plotWiFiRSSI((double)data.toInt());
            }
            else if(key.compare("d") == 0)
            {
                plotDebug((double)data.toInt());
            }
            else if(key.compare("u") == 0) // Accel
            {

                try{
                QList<QByteArray> cart = data.split('|');
                plotAccel((double) cart.at(0).toShort(),
                          (double) cart.at(1).toShort(),
                          (double) cart.at(2).toShort());
                }
                catch(std::exception& e)
                {
                    qDebug() << data.length();
                }
            }
            else if(key.compare("i") == 0) // Gyro
            {
                try{
                QList<QByteArray> cart = data.split('|');
                plotGyro((double) cart.at(0).toShort(),
                          (double) cart.at(1).toShort(),
                          (double) cart.at(2).toShort());
                }
                catch(std::exception& e)
                {
                    qDebug() << data.length();
                }
            }
            else
            {
                log->warning("Received unknown sensor data key: " + QString::fromStdString(msg.toStdString()) + " key: " + QString::fromStdString(key) + " from " + board_name);
            }
            break;
        }
        case '!': // Error
        {
             msg.remove(0,1); // remove first tag
             log->error(board_name.toStdString() + ": " + msg.toStdString());
             break;
        }
        case '@': // Warning
        {
             msg.remove(0,1); // remove first tag
             log->warning(board_name.toStdString() + ": " + msg.toStdString());
             break;
        }
        default:
            log->info(board_name.toStdString() + ": " + msg.toStdString());
            break;
        }
    }
}

void MotorComm::on_connect()
{
    connect(socket, SIGNAL(disconnected()),this,SLOT(on_disconnect()));
    log->info("Successfully connected to " + board_name + " @ " + host);
}

void MotorComm::on_pingResponse(double delay)
{
    emit plot(delay,board_name + " ping");
    log->data(board_name + " ping",(float) delay);
}



/******************************************************************************
 *                               PRIVATE                                      *
 *****************************************************************************/

void MotorComm::float2Bytes(unsigned char* bytes_temp,float float_variable){
  std::memcpy(bytes_temp, (unsigned char*) (&float_variable), 4);
}

void MotorComm::short2Bytes(unsigned char* bytes_temp,short short_variable){
  std::memcpy(bytes_temp, (unsigned char*) (&short_variable), 2);
}

void MotorComm::reconnect()
{
    log->warning(board_name + " @ " + host +" disconnected. Trying to reestablish connection.");

    disconnect(socket, SIGNAL(disconnected()),this,SLOT(on_disconnect())); // disconnect, otherwise init will emit a disconnected signal and we end up in an infinity loop

    try{
        socket->close();
        init(host,port);
    }
    catch(std::exception e)
    {
        log->error("Failed to reestablish connection to " + host + ". Aborting...");
        socket->close();
        emit disconnected();
        return;
    }

    log->info("Successfully reconnected to " + host);
}

void MotorComm::close()
{
    if(isConnected())
    {
        socket->blockSignals(true); // disconnect, otherwise init will emit a disconnected signal and we end up in an infinity loop
        socket->close();
        log->info("Closed connection to" + board_name + " @ " + host);
        socket->blockSignals(false);
        delete socket;
        socket = nullptr;
        emit removeGraph(host + " WiFi RSSI");
    }
}

void MotorComm::plotPWMVoltage(double voltage)
{
    if(!is_master)
    {
        log->warning("Slave motorboard " + board_name + "received motor power sample");
        return;
    }

    emit plot(voltage,"Motor power");
    log->data("Motor power",(short) voltage);
}
void MotorComm::plotSteeringAngle(double angle)
{
    if(!is_master)
    {
        log->warning("Slave motorboard " + board_name + "received steering sample");
        return;
    }

    emit plot(angle,"Steering angle");
    log->data("Steering angle",(short) angle);
}

void MotorComm::plotWiFiRSSI(double rssi)
{
    emit plot(rssi,host + " WiFi RSSI");
    emit new_rssi(rssi);
    log->data(board_name + " WiFi RSSI",(short) rssi);
}

void MotorComm::plotDebug(double debug)
{
    emit plot(debug,"Motorboard Debug");
    log->data("Debug",(int) debug);
}

void MotorComm::plotAccel(double x, double y, double z)
{
    emit plot(x,"Accelerometer x");
    emit plot(y,"Accelerometer y");
    emit plot(z,"Accelerometer z");
}

void MotorComm::plotGyro(double x, double y, double z)
{
    emit plot(x,"Gyro x");
    emit plot(y,"Gyro y");
    emit plot(z,"Gyro z");
}

void MotorComm::sendReplay()
{
    if(!is_master)
    {
        log->error("Cannot request replay from slave");
        return;
    }

    char msg[3];
    msg[0]= '#';
    msg[1]= 'r';
    msg[2]= '\r';

    socket->write(msg,3);
    qDebug("Requested replay");
}
