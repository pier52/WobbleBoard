#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QDesktopServices>
#include "logviewer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Restore fields from previous session
    settings = new QSettings(QDir::currentPath()+"/settings.ini", QSettings::IniFormat);
    unsigned bitrate = settings->value("bitrate",0).toUInt();
    unsigned width = settings->value("width",0).toUInt();
    unsigned height = settings->value("height",0).toUInt();
    unsigned cam_port = settings->value("cam_port",0).toUInt();
    bool cam_ping = settings->value("cam_ping",false).toBool();
    bool cam_rssi = settings->value("cam_rssi",false).toBool();
    unsigned motor_port = settings->value("motor_port",0).toUInt();
    unsigned arduino_baud = settings->value("arduino_baud",115200).toUInt();
    QString cam_ip = settings->value("cam_ip","CMS-ROBOT-CAM-4").toString();
    QString motor_ip = settings->value("motor_ip","CMS-ROBOT-WIFI-1").toString();
    QString motor_ip2 = settings->value("motor_ip2","").toString();
    double kp = settings->value("kp",-1).toDouble();
    double ki = settings->value("ki",-1).toDouble();
    double kd = settings->value("kd",-1).toDouble();
    unsigned pid_limit = settings->value("pid_limit",0).toUInt();
    unsigned pid_period = settings->value("pid_period",0).toUInt();
    unsigned sample_frequency = settings->value("sample_frequency",10).toUInt();
    bool sample_motor_power = settings->value("sample_motor_power",false).toBool();
    bool sample_steering_angle = settings->value("sample_steering_angle",false).toBool();
    bool sample_rssi = settings->value("sample_rssi",false).toBool();
    bool mb_pinging1 = settings->value("mb_pinging1",false).toBool();
    bool mb_pinging2 = settings->value("mb_pinging2",false).toBool();
    unsigned replay_period = settings->value("replay_period",50).toUInt();
    unsigned replay_length = settings->value("replay_length",10).toUInt();
    QString log_path = settings->value("log_path","").toString();
    int log_clear_rule = settings->value("log_clear_rule",0).toInt();
    QMap<QString,QVariant> enabled_logs_qvariant = settings->value("enabled_logs").toMap();
    QMap<QString,bool> enabled_logs;
    QMapIterator<QString, QVariant> log_iterator(enabled_logs_qvariant);
    while(log_iterator.hasNext())
    {
        log_iterator.next();
        enabled_logs.insert(log_iterator.key(),log_iterator.value().toBool());
    }

    // Class construction

    // Use settings if available, else default constructor
    if(bitrate != 0)
        camera_settings = new CameraModuleSettingsWindow(cam_port,cam_ip,bitrate,width,height,cam_ping,cam_rssi);
    else
        camera_settings = new CameraModuleSettingsWindow();

    if(kp != -1)
        motorboard_settings = new MotorBoardSettingsWindow(motor_port,arduino_baud,motor_ip,motor_ip2,kp,ki,kd,pid_period,pid_limit,
                                                           sample_frequency,sample_motor_power,sample_steering_angle,sample_rssi,replay_length,replay_period,mb_pinging1,mb_pinging2);
    else
        motorboard_settings = new MotorBoardSettingsWindow();

    plot_window = new PlotWindow(true);


    logging_options = new LoggingOptionsWindow(enabled_logs,log_path,log_clear_rule);
    log = new Logger(logging_options->getPath(),logging_options->getStartupClearRule(),logging_options->getCheckedLogNames());
    connect(log,SIGNAL(newDataLog(QString)),logging_options,SLOT(addDataLog(QString)));
    rpi = new RPiCamComm(log);
    motor_comm = new MotorComm(log, "ControlBoard1");
    motor_comm2 = new MotorComm(log, "ControlBoard2");


    keyhandler = new MainWindowKeyHandler(this);
    mouseclick_detector = new MouseClickDetector(this);
    ui->motorConnectionComboBox->installEventFilter(mouseclick_detector);
    connect(mouseclick_detector,SIGNAL(objectClicked()),this,SLOT(detectArduinoUSB()));



    // Simple variables

    is_streaming = false;

    // Connect stuff

    connect(rpi, SIGNAL(streamClosed()), this, SLOT(streamClosed()));
    connect(rpi,SIGNAL(wifiStrength(int)),ui->wifiIconRPI,SLOT(displayLevel(int)));
    connect(rpi,SIGNAL(plot(double,QString)),plot_window,SLOT(addPointTimeSafe(double,QString)));
    connect(rpi,SIGNAL(addGraph(QString,QString,QString)),plot_window,SLOT(addGraph(QString,QString,QString)));
    connect(rpi,SIGNAL(removeGraph(QString)),plot_window,SLOT(removeGraph(QString)));

    connect(camera_settings, SIGNAL(streamSettingsChanged(quint32,quint16,quint16)), rpi, SLOT(setCamSettings(quint32,quint16,quint16)));
    connect(camera_settings,SIGNAL(ping(bool,QString)),rpi,SLOT(ping(bool,QString)));
    connect(camera_settings,SIGNAL(requestRSSI(bool)),rpi,SLOT(requestWifiStrength(bool)));

    connect(log,SIGNAL(errorMsg(QString,QString)),this,SLOT(display(QString,QString)));
    connect(log,SIGNAL(warningMsg(QString,QString)),this,SLOT(display(QString,QString)));
    connect(log,SIGNAL(infoMsg(QString,QString)),this,SLOT(display(QString,QString)));

    connect(motor_comm,SIGNAL(disconnected()),this,SLOT(disconnectWiFi()));
    connect(motor_comm,SIGNAL(plot(double,QString)),plot_window,SLOT(addPointTimeSafe(double,QString)));
    connect(motor_comm,SIGNAL(new_rssi(double)),ui->wifiIcon,SLOT(display(double)));
    connect(motor_comm,SIGNAL(addGraph(QString,QString,QString)),plot_window,SLOT(addGraph(QString,QString,QString)));
    connect(motor_comm,SIGNAL(removeGraph(QString)),plot_window,SLOT(removeGraph(QString)));

    connect(keyhandler,SIGNAL(newSteerAngle(double)),ui->robotRenderer,SLOT(newWheelDirection(double)));
    connect(keyhandler,SIGNAL(newSteerAngle(double)),motor_comm,SLOT(steer(double)));
    connect(keyhandler,SIGNAL(camPhi(double)),ui->robotRenderer,SLOT(newCameraPhi(double)));
    connect(keyhandler,SIGNAL(camTheta(double)),ui->robotRenderer,SLOT(newCameraTheta(double)));
    connect(keyhandler,SIGNAL(motorDirection(int)),ui->robotRenderer,SLOT(newDriveDirection(int)));
    connect(keyhandler,SIGNAL(motorDirection(int)),motor_comm,SLOT(sendMotorPower(int)));
    connect(keyhandler,SIGNAL(camPhi(double)),rpi,SLOT(setCameraPhi(double)));
    connect(keyhandler,SIGNAL(camTheta(double)),rpi,SLOT(setCameraTheta(double)));
    connect(keyhandler,SIGNAL(increaseMotorPower(int)),this,SLOT(increaseMotorSpeed(int)));

    connect(motorboard_settings,SIGNAL(addGraph(QString,QString,QString)),plot_window,SLOT(addGraph(QString,QString,QString)));
    connect(motorboard_settings,SIGNAL(removeGraph(QString)),plot_window,SLOT(removeGraph(QString)));
    connect(motorboard_settings,SIGNAL(settingsChanged(float,float,float,uint,uint,uint,bool,bool,bool,bool,bool,unsigned,unsigned)),motor_comm,SLOT(sendSettings(float,float,float,uint,uint,uint,bool,bool,bool,bool,bool,unsigned,unsigned)));
    connect(motorboard_settings,SIGNAL(ping(bool,QString)),motor_comm,SLOT(ping(bool,QString)));
    motorboard_settings->apply();

    connect(logging_options,SIGNAL(save(QStringList)),log,SLOT(enableLogs(QStringList)));
    connect(logging_options,SIGNAL(clear(QStringList,int)),log,SLOT(clearLogs(QStringList,int)));

    ui->robotRenderer->scaleAfterParent();
    ui->speedSlider->setValue(settings->value("motor_speed",20).toInt());
}

MainWindow::~MainWindow()
{
    // Save settings to file
    settings->setValue("bitrate",camera_settings->getBitrate());
    settings->setValue("width",camera_settings->getWidth());
    settings->setValue("height",camera_settings->getHeight());
    settings->setValue("cam_port",camera_settings->getCamPort());
    settings->setValue("cam_ping",camera_settings->isPinging());
    settings->setValue("cam_rssi",camera_settings->isRequestingRSSI());
    settings->setValue("motor_port",motorboard_settings->getMotorPort());
    settings->setValue("cam_ip",camera_settings->getCamIP());
    settings->setValue("motor_ip",motorboard_settings->getMotorIP());
    settings->setValue("motor_ip2",motorboard_settings->getMotorIP2());
    settings->setValue("arduino_baud",motorboard_settings->getArduinoBaud());
    settings->setValue("pid_limit",motorboard_settings->getLimit());
    settings->setValue("pid_period",motorboard_settings->getPeriod());
    settings->setValue("kp",motorboard_settings->getKp());
    settings->setValue("ki",motorboard_settings->getKi());
    settings->setValue("kd",motorboard_settings->getKd());
    settings->setValue("sample_frequency",motorboard_settings->getSampleFrequency());
    settings->setValue("sample_motor_power",motorboard_settings->getSampleMotorPower());
    settings->setValue("sample_steering_angle",motorboard_settings->getSampleSteeringAngle());
    settings->setValue("sample_rssi",motorboard_settings->getSampleRSSI());
    settings->setValue("replay_period",motorboard_settings->getReplayPeriod());
    settings->setValue("replay_length",motorboard_settings->getReplayLength());
    settings->setValue("mb_pinging1",motorboard_settings->isPinging1());
    settings->setValue("mb_pinging2",motorboard_settings->isPinging2());
    settings->setValue("log_path",logging_options->getPath());
    settings->setValue("log_clear_rule",logging_options->getStartupClearRule());
    settings->setValue("enabled_logs",logging_options->getLogList());
    settings->setValue("motor_speed",ui->speedSlider->value());
    settings->sync();

    // Call destructors
    settings->deleteLater();
    rpi->deleteLater();
    camera_settings->deleteLater();
    motor_comm->deleteLater();
    motor_comm2->deleteLater();
    keyhandler->deleteLater();
    plot_window->deleteLater();
    motorboard_settings->deleteLater();
    delete log;
    delete mouseclick_detector;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    plot_window->close();
    motorboard_settings->close();
    camera_settings->close();
    logging_options->close();
    QWidget::closeEvent(event);
}

void MainWindow::on_connectButton_clicked()
{
    QString ip = camera_settings->getCamIP();

    if(rpi->isConnected())
        disconnectRPi();
    else
        connectRPi(ip);

}

void MainWindow::connectRPi(QString ip)
{
    try{
        rpi->init(camera_settings->getCamPort(),ip);
    }
    catch(std::runtime_error e)
    {
        log->error(e.what());
        return;
    }


    ui->streamButton->setEnabled(true);
    ui->photoButton->setEnabled(true);
    ui->connectButton->setText("Disconnect");
    camera_settings->connectedToCamera(true);
}

void MainWindow::disconnectRPi()
{
    try{
        rpi->close();
    }
    catch(std::runtime_error e)
    {
        log->error(e.what());
        return;
    }


    ui->streamButton->setEnabled(false);
    ui->streamButton->setText("Start stream");
    ui->recordButton->setEnabled(false);
    ui->recordButton->setText("Start recording");
     ui->photoButton->setEnabled(false);
    ui->connectButton->setText("Connect");
    camera_settings->connectedToCamera(false);
}

void MainWindow::on_streamButton_clicked()
{
    if(rpi->isConnected())
    {
        if(is_streaming)
        {
            ui->streamButton->setText("Start stream");
            ui->recordButton->setEnabled(false);
            if(rpi->isRecording())
            {
                rpi->toggleRecording();
                ui->recordButton->setText("Start recording");
            }
            is_streaming = false;
            rpi->stopStream();
        }
        else
        {
            ui->streamButton->setText("Stop stream");
            ui->recordButton->setEnabled(true);
            is_streaming = true;
            rpi->startStream(camera_settings->getCamPort());
        }
    }

}

void MainWindow::streamClosed()
{
    ui->streamButton->setText("Start stream");
    ui->recordButton->setEnabled(false);
    ui->recordButton->setText("Start recording");
    if(rpi->isRecording())
    {
        rpi->toggleRecording();
    }
    is_streaming = false;
}

void MainWindow::detectArduinoUSB()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    for(int i=0; i<ports.count(); i++)
    {
        if(ports.at(i).description().contains("Arduino"))
        {
            arduino_port = ports.at(i).portName();
            QString arduino_name = ports.at(i).description();
            if(arduino_name != "" && !comboBoxContains(ui->motorConnectionComboBox,arduino_name))
            {
                ui->motorConnectionComboBox->addItem(arduino_name);
            }
            return;
        }
    }

    // No arduino detected, remove arduino if present
    if(ui->motorConnectionComboBox->count() > 1)
        ui->motorConnectionComboBox->removeItem(1);

}

void MainWindow::on_connectWiFiButton_clicked()
{
    QString ip;
    unsigned port;
    if(!ui->motorConnectionComboBox->currentText().compare("WiFi"))
    {
        ip = motorboard_settings->getMotorIP();
        port = motorboard_settings->getMotorPort();
    }
    else
    {
        ip = arduino_port;
        port = motorboard_settings->getArduinoBaud();
    }

    if(!motor_comm->isConnected())
    {
        connectWiFi(ip,port);
        motorboard_settings->sendToMotorBoard();
    }
    else
    {
        disconnectWiFi();
    }

}

void MainWindow::disconnectWiFi()
{
    try{
        motor_comm->close();
    }
    catch(std::runtime_error e)
    {
        log->error(e.what());
        return;
    }

    ui->connectWiFiButton->setText("Connect");
    motorboard_settings->connectedToMotor(false);
    ui->wifiIcon->clearBars();
}

void MainWindow::connectWiFi(QString ip,unsigned port)
{
    try{
        motor_comm->init(ip,port);
    }
    catch(std::runtime_error e)
    {
        log->error(e.what());
        return;
    }

    ui->connectWiFiButton->setText("Disconnect");
    motorboard_settings->connectedToMotor(true);
}


void MainWindow::display(QString msg)
{
    display(msg,"black");
}

void MainWindow::display(QString msg, QString color)
{
    QTextCursor cursor( ui->messageEdit->textCursor());

    QTextCharFormat format;
    format.setForeground( QBrush( QColor( color ) ) );
    cursor.setCharFormat( format );

    cursor.insertText(">> " + msg + "\n" );

    if(ui->autoscroll_checkBox->isChecked())
    {
        ui->messageEdit->verticalScrollBar()->setSliderPosition(ui->messageEdit->verticalScrollBar()->maximum());
    }
}

void MainWindow::on_actionPlot_triggered()
{
    plot_window->show();
}


void MainWindow::on_speedSlider_valueChanged(int value)
{
    ui->motorSliderEdit->setText(QString::number(value)+"%");
    ui->robotRenderer->newMotorStress((double)value/100);
    motor_comm->setMotorPower(value);
}

void MainWindow::increaseMotorSpeed(int increment)
{
    int current_speed = ui->speedSlider->value();
    ui->speedSlider->setValue(current_speed+increment);
}

void MainWindow::on_replayButton_clicked()
{
    try
    {
        motor_comm->sendReplay();
    }
    catch(const std::exception& e)
    {
        log->error("Could not request replay: " + QString::fromStdString(e.what()));
    }
}

void MainWindow::on_actionMotor_Board_Settings_triggered()
{
    motorboard_settings->show();
}

void MainWindow::on_action_Stream_settings_triggered()
{
    camera_settings->show();
}

bool MainWindow::comboBoxContains(QComboBox *box, QString item_name)
{
    for(int i=0; i<box->count(); i++)
    {
        if(box->itemText(i) == item_name)
            return true;
    }

    return false;
}

void MainWindow::on_sendMotorCompButton_clicked()
{
    motor_comm->sendMotorCompensation(ui->motorCompEdit->text().toFloat());
}

void MainWindow::on_sendSteeringCompButton_clicked()
{
    motor_comm->sendSteeringCompensation(ui->steeringCompLinEdit->text().toFloat(),(char) ui->steeringCompOffsEdit_2->text().toInt());
}

void MainWindow::on_errorButton_clicked()
{
    motor_comm->requestErrorReport();
}

void MainWindow::on_action_Manual_triggered()
{
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile("../Documentation/Manual/index.html")))
        log->error("Could not open manual");
}

void MainWindow::on_action_Doxygen_triggered()
{
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile("//cern.ch/dfs/Users/s/sorn/Documents/doxygen/out/html/index.html")))
        log->error("Could not open doxygen");
}

void MainWindow::on_action_Logging_options_triggered()
{
    logging_options->showAndRemeberStatus();
}

void MainWindow::on_recordButton_clicked()
{
    if(rpi->isRecording())
    {
        rpi->toggleRecording();
        ui->recordButton->setText("Start recording");
    }
    else
    {
        QString file = QFileDialog::getSaveFileName(this, tr("Save video stream"),"",tr("Video file (*.h264)"));
        if(!file.contains("."))
        {
            file.append(".h264");
        }

        try
        {
            rpi->toggleRecording(file);
        }
        catch(const std::exception& e)
        {
            log->error("Could not start recording: " + QString::fromStdString(e.what()));
            return;
        }

        ui->recordButton->setText("Stop recording");
    }
}

void MainWindow::on_photoButton_clicked()
{
    if(rpi->isRecording())
    {
        ui->recordButton->setText("Start recording");
    }

    rpi->snapPhoto();
    QString file = QFileDialog::getSaveFileName(this, tr("Save photo"),"",tr("Image file (*.jpg)"));
    if(!file.contains("."))
        file.append(".jpg");
    rpi->savePhoto(file);

}

void MainWindow::on_connectWiFiButton_2_clicked()
{
    if(!motor_comm2->isConnected())
    {
        try{
            motor_comm2->init(motorboard_settings->getMotorIP2(),motorboard_settings->getMotorPort());
        }
        catch(std::runtime_error e)
        {
            log->error(e.what());
            return;
        }

        ui->connectWiFiButton_2->setText("Disconnect");
        ui->resetButton->setEnabled(true);
    }
    else
    {
        try{
            motor_comm2->close();
        }
        catch(std::runtime_error e)
        {
            log->error(e.what());
            return;
        }

        ui->connectWiFiButton_2->setText("Connect");
        ui->resetButton->setEnabled(false);
    }

}

void MainWindow::on_resetButton_clicked()
{
    motor_comm2->requestReset();
}
