#include "cameramodulesettingswindow.h"
#include "ui_cameramodulesettingswindow.h"
#include <QMessageBox>

CameraModuleSettingsWindow::CameraModuleSettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraModuleSettingsWindow)
{
    ui->setupUi(this);

    on_defualtButton_clicked();
    uint_validator.setBottom(0);
    port_validator.setRange(0,9999);
    is_pinging = false;
    is_requesting_rssi = false;
}

CameraModuleSettingsWindow::CameraModuleSettingsWindow(unsigned cam_port,QString cam_ip,quint32 bitrate, quint16 width, quint16 height, bool ping, bool rssi_request, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CameraModuleSettingsWindow)
{
    ui->setupUi(this);

    this->bitrate = bitrate;
    this->width = width;
    this->height = height;
    this->cam_port = cam_port;
    this->cam_ip = cam_ip;
    setFields();

    uint_validator.setBottom(0);

    port_validator.setRange(0,9999);

    is_pinging = ping;
    ui->pingCheckBox->setChecked(is_pinging);

    is_requesting_rssi = rssi_request;
    ui->rssiCheckBox->setChecked(is_requesting_rssi);

}

CameraModuleSettingsWindow::~CameraModuleSettingsWindow()
{
    delete ui;
}

void CameraModuleSettingsWindow::on_defualtButton_clicked()
{   
    ui->cameraIPEdit->setText(DEFAULT_CAM_IP);
    ui->cameraPortEdit->setText(QString::number(DEFAULT_CAM_PORT));
    ui->bitrateEdit->setText(QString::number(DEFAULT_BITRATE/1000));
    ui->heightEdit->setText(QString::number(DEFAULT_HEIGHT));
    ui->widthEdit->setText(QString::number(DEFAULT_WIDTH));
    ui->pingCheckBox->setChecked(false);
    ui->rssiCheckBox->setChecked(false);
}

void CameraModuleSettingsWindow::setFields()
{
    ui->bitrateEdit->setText(QString::number(bitrate/1000));
    ui->widthEdit->setText(QString::number(width));
    ui->heightEdit->setText(QString::number(height));
    ui->cameraPortEdit->setText(QString::number(cam_port));
    ui->cameraIPEdit->setText(cam_ip);
}

quint32 CameraModuleSettingsWindow::getBitrate() const
{
    return bitrate;
}

quint16 CameraModuleSettingsWindow::getHeight() const
{
    return height;
}

quint16 CameraModuleSettingsWindow::getWidth() const
{
    return width;
}

void CameraModuleSettingsWindow::on_cancelButton_clicked()
{
    setFields();
    ui->rssiCheckBox->setChecked(is_requesting_rssi);
    ui->pingCheckBox->setChecked(is_pinging);
    this->hide();
}

void CameraModuleSettingsWindow::on_okButton_clicked()
{
    if(on_applyButton_clicked())
        this->hide();
}

unsigned CameraModuleSettingsWindow::getCamPort() const
{
    return cam_port;
}

QString CameraModuleSettingsWindow::getCamIP() const
{
    return cam_ip;
}

void CameraModuleSettingsWindow::connectedToCamera(bool connected)
{
    this->connected = connected;
    ui->cameraIPEdit->setEnabled(!connected);
    ui->cameraPortEdit->setEnabled(!connected);
}

bool CameraModuleSettingsWindow::on_applyButton_clicked()
{
    QString bitrate_s = ui->bitrateEdit->text();
    QString width_s = ui->widthEdit->text();
    QString height_s = ui->heightEdit->text();
    QString camera = ui->cameraPortEdit->text();


    int pos = 0;

    if(uint_validator.validate(bitrate_s,pos) == QValidator::Acceptable)
    {
        bitrate = bitrate_s.toInt()*1000;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Input error");
        msgBox.setText("Error: bitrate must be a positive integer");
        msgBox.exec();
        return false;
    }

    if(uint_validator.validate(width_s,pos) == QValidator::Acceptable && width_s.toInt() < 1920)
    {
        width = width_s.toInt();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Input error");
        msgBox.setText("Error: width must be a positive integer less than 1920");
        msgBox.exec();
        return false;
    }

    if(uint_validator.validate(height_s,pos) == QValidator::Acceptable && height_s.toInt() < 1344)
    {
        height = height_s.toInt();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Input error");
        msgBox.setText("Error: height must be a positive integer less than 1344");
        msgBox.exec();
        return false;
    }

    if(port_validator.validate(camera,pos) == QValidator::Acceptable)
    {
        cam_port = camera.toInt();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Input error");
        msgBox.setText("Error: camera port must be a number between 0 and 9999.");
        msgBox.exec();
        return false;
    }

    cam_ip = ui->cameraIPEdit->text();

    is_pinging = ui->pingCheckBox->isChecked();
    emit ping(is_pinging,cam_ip);

    is_requesting_rssi = ui->rssiCheckBox->isChecked();
    emit requestRSSI(is_requesting_rssi);

    emit streamSettingsChanged(bitrate,width,height);

    return true;
}

bool CameraModuleSettingsWindow::isPinging() const
{
    return is_pinging;
}

bool CameraModuleSettingsWindow::isRequestingRSSI() const
{
    return is_requesting_rssi;
}
