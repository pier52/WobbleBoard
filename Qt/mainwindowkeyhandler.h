/**
 * \class MainWindowKeyHandler
 *
 *
 * \brief
 *      Handles all the user's key presses in the main window and forwards them to the correct class via signals
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef MAINWINDOWKEYHANDLER_H
#define MAINWINDOWKEYHANDLER_H

#include <QObject>
#include "ikeyboardhandler.h"

class MainWindowKeyHandler : public IKeyboardHandler
{
    Q_OBJECT
public:
    explicit MainWindowKeyHandler(QObject *parent);

signals:
    void turnLeft(unsigned amount);
    void turnRight(unsigned amount);
    void newSteerAngle(double angle);
    void camPhi(double phi);
    void camTheta(double theta);
    void motorDirection(int direction);
    void increaseMotorPower(int increment);

private slots:
    void handleKeyEvents() override;

private:
    double steer_angle;
    double cam_phi;
    double cam_theta;
};

#endif // MainWindowKeyHandler_H
