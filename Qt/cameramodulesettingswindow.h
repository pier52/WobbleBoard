/**
 * \class CameraModuleSettingsWindow
 *
 *
 * \brief
 * This class provides the logic behind the Camera Module Settings form, where the user can
 * decide things like camera capture resolution, bitrate and how to connect to the camera module.
 *
 *
 *
 * \author Samuel Orn
 *
 */


#ifndef CAMERAMODULESETTINGSWINDOW_H
#define CAMERAMODULESETTINGSWINDOW_H

#include <QDialog>
#include <QIntValidator>

namespace Ui {
class CameraModuleSettingsWindow;
}

class CameraModuleSettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CameraModuleSettingsWindow(QWidget *parent = 0);
    explicit CameraModuleSettingsWindow(unsigned cam_port,QString cam_ip,quint32 bitrate, quint16 width, quint16 height, bool ping, bool rssi_request,QWidget *parent = 0);
    ~CameraModuleSettingsWindow();

    quint32 getBitrate() const;
    quint16 getHeight() const;
    quint16 getWidth() const;

    unsigned getCamPort() const;
    QString getCamIP() const;

    bool isPinging() const;
    bool isRequestingRSSI() const;

    /// Sets whether we are connected to the camera or not. This determines what fields are editable for the user.
    void connectedToCamera(bool connected);

signals:
    void streamSettingsChanged(quint32 bitrate, quint16 width, quint16 height);
    void ping(bool do_it, QString ip);
    void requestRSSI(bool do_it);

private slots:

    void on_defualtButton_clicked();

    void on_cancelButton_clicked();

    void on_okButton_clicked();

    bool on_applyButton_clicked();

private:
    /// Displays the internally stored values in the text fields
    void setFields();


    Ui::CameraModuleSettingsWindow *ui;
    quint32 bitrate;
    quint16 height;
    quint16 width;
    const quint32 DEFAULT_BITRATE = 2000000;
    const quint16 DEFAULT_WIDTH = 1280;
    const quint16 DEFAULT_HEIGHT = 700;

    QIntValidator uint_validator;
    QIntValidator port_validator;

    unsigned cam_port;
    QString cam_ip;
    const QString DEFAULT_CAM_IP = "CMS-ROBOT-CAM-4";
    const unsigned DEFAULT_CAM_PORT = 5000;
    bool is_pinging;
    bool is_requesting_rssi;
    bool connected = false;

};
#endif // CameraModuleSettingsWindow_H
