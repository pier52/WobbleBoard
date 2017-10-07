#include "mainwindowkeyhandler.h"
#include <math.h>

MainWindowKeyHandler::MainWindowKeyHandler(QObject *parent) : IKeyboardHandler(50,parent)
{
    steer_angle = 0;
    cam_phi = 0;
    cam_theta = 0;
}

void MainWindowKeyHandler::handleKeyEvents()
{
    QVector<int> pressed_keys = getPressedKeys();
    // Turn commands
    if(pressed_keys.contains(65)) // A
    {
        steer_angle -= M_PI/180;
        if(steer_angle < -M_PI/6)
            steer_angle = -M_PI/6;

        emit newSteerAngle(steer_angle);
    }
    else if (pressed_keys.contains(68)) // D
    {
        steer_angle += M_PI/180;
        if(steer_angle > M_PI/6)
            steer_angle = M_PI/6;
        emit newSteerAngle(steer_angle);
    }

    // Throttle commands
    if(pressed_keys.contains(87)) // W
    {
        emit motorDirection(1);
    }
    else if (pressed_keys.contains(83)) // S
    {
        emit motorDirection(-1);
    }
    else
    {
        emit motorDirection(0);
    }

    // Camera phi
    if(pressed_keys.contains(74)) // J
    {
        cam_phi += 0.1;
        if(cam_phi > M_PI/2)
            cam_phi = M_PI/2;


        emit camPhi(cam_phi);
    }
    else if (pressed_keys.contains(76)) // L
    {
        cam_phi -= 0.1;
        if(cam_phi < -M_PI/2)
            cam_phi = -M_PI/2;

        emit camPhi(cam_phi);
    }

    // Camera theta
    if(pressed_keys.contains(73)) // I
    {
        cam_theta += 0.1;
        if(cam_theta > M_PI/2)
            cam_theta = M_PI/2;

        emit camTheta(cam_theta);
    }
    else if (pressed_keys.contains(75)) // K
    {
        cam_theta -= 0.1;
        if(cam_theta < -M_PI/2)
            cam_theta = -M_PI/2;

        emit camTheta(cam_theta);
    }

    // Stop and reset steering and cam position

    if(pressed_keys.contains(16777248)) // Shift
    {
        cam_theta = 0;
        cam_phi = 0;
        emit camPhi(0);
        emit camTheta(0);
        emit motorDirection(0);
        steer_angle = 0;
        emit newSteerAngle(0);
    }

    // Reset just cam position

    if(pressed_keys.contains(16777249)) // Ctrl
    {
        cam_theta = 0;
        cam_phi = 0;
        emit camPhi(0);
        emit camTheta(0);
    }

    // Adjust speed

    if(pressed_keys.contains(43)) // +
    {
        emit increaseMotorPower(1);
    }
    else if(pressed_keys.contains(45)) // -
    {
        emit increaseMotorPower(-1);
    }


}
