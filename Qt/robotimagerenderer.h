/**
 * \class RobotImageRenderer
 *
 *
 * \brief
 *      Custom widget which renders an image displaying the current robot status.
 *
 * It shows the current motor load, wheel turn direction, whether it is driving (and which direction) or not and where the on board camera is pointing.
 * All of these quantities are continously animated in an image showing the robot, as the user gives inpit to change the properties. \n
 * The rendering works by rendering all movable parts individually using Qt rendering tools (wheels, camera, arrows etx.) and having a static image file displaying the static parts
 *
 * \author Samuel Orn
 *
 */

#ifndef ROBOTIMAGERENDERER_H
#define ROBOTIMAGERENDERER_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include "gradientslider.h"

class RobotImageRenderer : public QWidget
{
    Q_OBJECT
public:
    explicit RobotImageRenderer(QWidget *parent = 0);
    ~RobotImageRenderer();

signals:

public slots:
    void newWheelDirection(double angle);
    void newCameraTheta(double theta);
    void newCameraPhi(double phi);
    void newDriveDirection(int direction);
    void newMotorStress(double percentage);

    /// Scales the robot image after it's parent widget size
    void scaleAfterParent();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    void drawDirectionArrow(QPainter* painter,int x_origin, int y_origin,double angle, double scale, int direction);
    void drawFrontWheels(QPainter* painter, double angle);

    /// Draws the camera 'pie' that displays which way the camera is facing.
    void drawCamera(QPainter* painter, double phi,double theta);

    /// Scales the robot rendering to <scale> FROM ORIGINAL (i.e. scale(0.8) called consecutively after each other will not have any effect)
    void scale(double scale);

    /// Translate the upper left coordinate of the rendering
    void translate(QPoint new_pos);

    double scale_factor;
    double arrow_angle;
    double cam_theta;
    double cam_phi;
    int drive_direction;

    /// Static image of the robot. Pixel positions for rendering moving parts are stored in <pos_struct>.
    QPixmap static_image;
    GradientSlider* motor_stress;

    /// Holds the positions of the various parts of the rendering. Initially, they are pointing to pixel positions in the static
    /// image depicting the robot. These positions are then recalculated if a translation or scaling occurs.
    struct positions {
      const int WHEEL_OFFSET_X = 3;
      const int WHEEL_OFFSET_Y = 5;
      const int CAM_OFFSET_Y = 50;

      QSize size = QSize(135,163);
      QPoint start = QPoint(10,10);

      QPoint left_wheel = QPoint(start.x() + 22,start.y() + 19);
      QPoint right_wheel = QPoint(size.width() + start.x() - 22,left_wheel.y());
      QPoint camera = QPoint(start.x() + size.width()/2,CAM_OFFSET_Y);
      QPoint dir_arrow_start = QPoint(start.x() + size.width()/2,31);

      QRect left_wheel_rect = QRect(start.x() + WHEEL_OFFSET_X,start.y() + WHEEL_OFFSET_Y,23,32);
      QRect right_wheel_rect = QRect(size.width() + start.x() - WHEEL_OFFSET_X-23,start.y() + WHEEL_OFFSET_Y,23,32);
      QRect camera_rect = QRect(start.x(),-30,135,135);
      QRect motor_stress_rect = QRect(start.x()+40,start.y()+127,size.width()-80,10);
    } ;

    positions pos_struct;
};

#endif // ROBOTIMAGERENDERER_H
