/**
 * \class MainWindow
 *
 *
 * \brief
 *  Implements the logic behind the main window of the GUI. Owns most other classes.
 *
 *
 * \author Samuel Orn
 *
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rpicamcomm.h"
#include "cameramodulesettingswindow.h"
#include <QScrollBar>
#include <MotorComm.h>
#include <mainwindowkeyhandler.h>
#include <QSettings>
#include "logger.h"
#include "plotwindow.h"
#include "motorboardsettingswindow.h"
#include "mouseclickdetector.h"
#include "networkpinger.h"
#include "loggingoptionswindow.h"
#include "mapwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void on_streamButton_clicked();

    /// Called when video stream has been closed
    void streamClosed();

    void on_connectWiFiButton_clicked();

    /// Displays <msg> in the message area
    void display(QString msg);

    /// Displays <msg> in <color> in the message area
    void display(QString msg, QString color);

    void disconnectWiFi();

    void on_actionPlot_triggered();

    void increaseMotorSpeed(int increment);

    void on_speedSlider_valueChanged(int value);

    void on_replayButton_clicked();

    void on_actionMotor_Board_Settings_triggered();

    void on_action_Stream_settings_triggered();

    void detectArduinoUSB();


    void on_sendMotorCompButton_clicked();

    void on_sendSteeringCompButton_clicked();

    void on_errorButton_clicked();

    void on_action_Manual_triggered();

    void on_action_Doxygen_triggered();

    void on_action_Logging_options_triggered();

    void on_recordButton_clicked();

    void on_photoButton_clicked();

    void on_connectWiFiButton_2_clicked();

    void on_resetButton_clicked();

    void on_actionMap_triggered();

private:
    bool event(QEvent *e) override
    {
        switch(e->type())
        {
            case QEvent::WindowDeactivate :
                keyhandler->clear();
                break ;
            default:
                break;
        }

        return QMainWindow::event(e) ;
    }

    void closeEvent(QCloseEvent *event) override;
    void connectRPi(QString ip);
    void disconnectRPi();
    void connectWiFi(QString ip,unsigned port);
    bool comboBoxContains(QComboBox* box, QString item_name);

    Ui::MainWindow *ui;
    RPiCamComm* rpi;
    CameraModuleSettingsWindow* camera_settings;
    MotorComm* motor_comm;
    MotorComm* motor_comm2;
    MainWindowKeyHandler* keyhandler;
    QSettings* settings;
    Logger* log;
    PlotWindow* plot_window;
    MotorBoardSettingsWindow* motorboard_settings;
    MouseClickDetector* mouseclick_detector;
    LoggingOptionsWindow* logging_options;
    MapWindow* map_window;

    bool is_streaming;
    QString arduino_port;

    NetworkPinger* pinger;

};

#endif // MAINWINDOW_H
