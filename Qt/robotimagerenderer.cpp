#include "robotimagerenderer.h"
#include <QPainter>
#include <math.h>

/******************************************************************************
 *                            CONSTRUCTORS                                    *
 *****************************************************************************/

RobotImageRenderer::RobotImageRenderer(QWidget *parent) : QWidget(parent)
{
    if(!static_image.load("../GUI/assets/static_vehicle.png"))
    {
        // TODO: Should notify user here somehow...
    }

    arrow_angle = 0;
    scale_factor = 0;
    drive_direction = 0;
    cam_phi = 0;
    cam_theta = 0;

    motor_stress = new GradientSlider(this);
}

RobotImageRenderer::~RobotImageRenderer()
{
    delete motor_stress;
}


/******************************************************************************
 *                            PUBLIC SLOTS                                    *
 *****************************************************************************/


void RobotImageRenderer::newWheelDirection(double angle)
{
    arrow_angle = angle;

    update();
}

void RobotImageRenderer::newCameraTheta(double theta)
{
    cam_theta = theta;

    update();
}

void RobotImageRenderer::newCameraPhi(double phi)
{
    cam_phi = phi;

    update();
}

void RobotImageRenderer::newDriveDirection(int direction)
{
    if(direction != drive_direction)
    {
        drive_direction = direction;
        update();
    }

}

void RobotImageRenderer::newMotorStress(double percentage)
{
    motor_stress->setFillPercentage(percentage);
}

void RobotImageRenderer::scaleAfterParent()
{
    int width = pos_struct.size.width()+pos_struct.start.x();
    int height = pos_struct.size.height()+90; // extra height is for direction arrow (pos_struct.size is only for robot image)
    double scale_x = (double)this->width()/width;
    double scale_y = (double)this->height()/height;

    // Center the image
    if(scale_x < scale_y)
    {
        scale(scale_x);
        translate(QPoint(9,90*scale_factor+(this->height()-height*scale_factor)/2));
    }
    else if(scale_x > scale_y)
    {
        scale(scale_y);
        translate(QPoint((this->width()-width*scale_factor)/2,90*scale_factor));
    }

    // Add motor stress slider
    motor_stress->setGeometry(pos_struct.motor_stress_rect);
    motor_stress->setGradientHot();
    motor_stress->setBackground(Qt::white);
    motor_stress->show();

    //qDebug("Scale: %f, size (%d,%d)", scale_factor, this->width(),this->height());
}


/******************************************************************************
 *                              PRIVATE                                       *
 *****************************************************************************/

void RobotImageRenderer::paintEvent(QPaintEvent*)
{
    QPainter* painter = new QPainter(this);

    painter->drawPixmap(pos_struct.start.x(),pos_struct.start.y(),pos_struct.size.width(),pos_struct.size.height(),static_image);

    drawDirectionArrow(painter,pos_struct.dir_arrow_start.x(),pos_struct.dir_arrow_start.y(),M_PI-arrow_angle,scale_factor,drive_direction);

    drawFrontWheels(painter,M_PI-arrow_angle);

    drawCamera(painter,cam_phi,cam_theta);

    delete painter;
}

void RobotImageRenderer::translate(QPoint new_pos)
{
    int dx = new_pos.x() - pos_struct.start.x();
    int dy = new_pos.y() - pos_struct.start.y();
    QPoint dpos(dx,dy);

    pos_struct.start += dpos;
    pos_struct.camera += dpos;
    pos_struct.left_wheel += dpos;
    pos_struct.right_wheel += dpos;
    pos_struct.dir_arrow_start += dpos;

    pos_struct.left_wheel_rect.translate(dx,dy);
    pos_struct.right_wheel_rect.translate(dx,dy);
    pos_struct.camera_rect.translate(dx,dy);
    pos_struct.motor_stress_rect.translate(dx,dy);

    update();
}

void RobotImageRenderer::scale(double scale)
{
    // If you have to read this...sorry. But all it does is scale the size
    // of everything in pos_struct

    QSize new_size(pos_struct.size.width()*scale,pos_struct.size.height()*scale);


    pos_struct.left_wheel = QPoint(pos_struct.start.x() + scale*(pos_struct.left_wheel.x()-pos_struct.start.x()),
                                   pos_struct.start.y() + scale*(pos_struct.left_wheel.y()-pos_struct.start.y()));

    pos_struct.right_wheel = QPoint((pos_struct.right_wheel.x()-(pos_struct.start.x()+pos_struct.size.width()))*scale + new_size.width() + pos_struct.start.x(),
                                    pos_struct.left_wheel.y());

    pos_struct.camera = QPoint(pos_struct.start.x() + new_size.width()/2,(pos_struct.camera.y()-pos_struct.start.y())*scale + pos_struct.start.y());
    pos_struct.dir_arrow_start = QPoint(pos_struct.start.x() + new_size.width()/2,pos_struct.dir_arrow_start.y()*scale);

    pos_struct.left_wheel_rect.moveTopLeft(QPoint(pos_struct.start.x() + scale*(pos_struct.left_wheel_rect.topLeft().x()-pos_struct.start.x()),
                                                 pos_struct.start.y() + scale*(pos_struct.left_wheel_rect.topLeft().y()-pos_struct.start.y())));
    pos_struct.left_wheel_rect.setWidth(pos_struct.left_wheel_rect.width()*scale);
    pos_struct.left_wheel_rect.setHeight(pos_struct.left_wheel_rect.height()*scale);

    pos_struct.right_wheel_rect.moveTopLeft(QPoint((pos_struct.right_wheel_rect.topLeft().x()-(pos_struct.start.x()+pos_struct.size.width()))*scale + new_size.width() + pos_struct.start.x(),
                                                 pos_struct.start.y() + scale*(pos_struct.right_wheel_rect.topLeft().y()-pos_struct.start.y())));

    pos_struct.right_wheel_rect.setWidth(pos_struct.left_wheel_rect.width());
    pos_struct.right_wheel_rect.setHeight(pos_struct.right_wheel_rect.height()*scale);

    pos_struct.camera_rect.moveTopLeft(QPoint(pos_struct.start.x() + scale*(pos_struct.camera_rect.topLeft().x()-pos_struct.start.x()),
                                                 pos_struct.start.y() + scale*(pos_struct.camera_rect.topLeft().y()-pos_struct.start.y())));
    pos_struct.camera_rect.setWidth(pos_struct.camera_rect.width()*scale);
    pos_struct.camera_rect.setHeight(pos_struct.camera_rect.height()*scale);

    pos_struct.motor_stress_rect.moveTopLeft(QPoint(pos_struct.start.x() + scale*(pos_struct.motor_stress_rect.topLeft().x()-pos_struct.start.x()),
                                                 pos_struct.start.y() + scale*(pos_struct.motor_stress_rect.topLeft().y()-pos_struct.start.y())));
    pos_struct.motor_stress_rect.setWidth(pos_struct.motor_stress_rect.width()*scale);
    pos_struct.motor_stress_rect.setHeight(pos_struct.motor_stress_rect.height()*scale);

    pos_struct.size = new_size;

    scale_factor = scale;

    update();
}

void RobotImageRenderer::drawDirectionArrow(QPainter *painter, int x_origin, int y_origin, double angle, double scale, int drive_direction)
{
    if(drive_direction < 0) // reverse
        angle += M_PI;

    const int ARROWHEAD_HEIGHT = 25;
    const int ARROWHEAD_WIDTH = 12;
    const int BASE_THICKNESS = 2;
    const int LENGTH = 80;

    int length = LENGTH*scale;

    // Make sure arrow is always visible
    int arrowhead_height = ARROWHEAD_HEIGHT*scale;
    if(arrowhead_height < 15)
        arrowhead_height = 15;

    int arrowhead_width = ARROWHEAD_WIDTH*scale;
    if(arrowhead_width < 8)
        arrowhead_width = 8;

    int base_thickness = BASE_THICKNESS*scale;
    if(base_thickness < 2)
        base_thickness = 2;

    QLine line(x_origin,y_origin,x_origin+length*std::sin(angle),y_origin+length*std::cos(angle));

    x_origin += line.dx()/std::sqrt(line.dx()*line.dx() + line.dy()*line.dy())*30*scale;
    y_origin += line.dy()/std::sqrt(line.dx()*line.dx() + line.dy()*line.dy())*30*scale;

    // Arrowhead

    //Normal vector
    double norm = std::sqrt(line.dy()*line.dy() + line.dx()*line.dx());
    QLine normal(QPoint(0,0),QPoint(-line.dy(),line.dx()));


    QLine arrowhead_base(line.x2() + normal.dx()*arrowhead_width/norm,line.y2() + normal.dy()*arrowhead_width/norm,line.x2() - normal.dx()*arrowhead_width/norm,line.y2()-normal.dy()*arrowhead_width/norm);
    QLine arrowhead_line1(arrowhead_base.x1(),arrowhead_base.y1(),line.x2()+arrowhead_height*std::sin(angle),line.y2() + arrowhead_height*std::cos(angle));
    QLine arrowhead_line2(arrowhead_base.x2(),arrowhead_base.y2(),line.x2()+arrowhead_height*std::sin(angle),line.y2() + arrowhead_height*std::cos(angle));

    QPainterPath path;

    path.moveTo(x_origin+base_thickness*normal.dx()/norm,y_origin+base_thickness*normal.dy()/norm);
    path.lineTo(line.x2()+base_thickness*normal.dx()/norm,line.y2()+base_thickness*normal.dy()/norm);

    path.lineTo (arrowhead_line1.x1(), arrowhead_line1.y1());
    path.lineTo (arrowhead_line1.x2(), arrowhead_line1.y2());
    path.lineTo (arrowhead_line2.x2(), arrowhead_line2.y2());
    path.lineTo (arrowhead_line2.x1(), arrowhead_line2.y1());

    path.lineTo(line.x2()-base_thickness*normal.dx()/norm,line.y2()-base_thickness*normal.dy()/norm);
    path.lineTo(x_origin-base_thickness*normal.dx()/norm,y_origin-base_thickness*normal.dy()/norm);
    path.lineTo(x_origin+base_thickness*normal.dx()/norm,y_origin+base_thickness*normal.dy()/norm);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen (Qt :: red);
    painter->drawPath(path);
    if(drive_direction != 0)
        painter->fillPath (path, QBrush (QColor ("red")));
    painter->setPen (Qt :: NoPen);
}

void RobotImageRenderer::drawFrontWheels(QPainter *painter, double angle)
{
    QPoint center = pos_struct.left_wheel_rect.center();
    QPen pen(Qt::black);
    pen.setWidth(2);

    painter->save();
    painter->setPen(pen);
    painter->translate(pos_struct.left_wheel.x(), center.y());
    painter->rotate(-angle*360/(2*3.1415)+180);
    painter->translate(-pos_struct.left_wheel.x(),-center.y());
    painter->setBrush(Qt::black);
    painter->drawRoundedRect(pos_struct.left_wheel_rect,5,5);
    painter->restore();

    center = pos_struct.right_wheel_rect.center();
    painter->save();
    painter->setPen(pen);
    painter->translate(pos_struct.right_wheel.x(), center.y());
    painter->rotate(-angle*360/(2*3.1415)+180);
    painter->translate(-pos_struct.right_wheel.x(), -center.y());
    painter->setBrush(Qt::black);
    painter->drawRoundedRect(pos_struct.right_wheel_rect,5,5);
    painter->restore();
}

void RobotImageRenderer::drawCamera(QPainter *painter, double phi,double theta)
{
    QRadialGradient radialGradient(pos_struct.camera_rect.center(),pos_struct.camera_rect.width());
    double theta_scale;
    double phi_draw;
    /*
    if(theta < 0)
    {
        radialGradient.setColorAt(1.0-(theta+M_PI/2)/(M_PI), QColor(0,0,0,0));
        radialGradient.setColorAt(0.0, QColor(255,0,0,255));
        phi_draw = ((phi+M_PI)*360/(M_PI*2)+45)*16;
        theta_scale = 1-1.4*(M_PI/2+theta)/(M_PI);
    }
    else
    {
        radialGradient.setColorAt(1.0-(M_PI/2-theta)/M_PI, QColor(0,0,0,0));
        radialGradient.setColorAt(0.0, QColor(0,0,255,255));
        phi_draw = (phi*360/(M_PI*2)+45)*16;
        theta_scale = 1-1.4*(M_PI/2-theta)/M_PI;
    }
    */

    radialGradient.setColorAt(1.0-(M_PI/2-theta)/(2*M_PI), QColor(0,0,0,0));
    radialGradient.setColorAt(0.0, QColor(0,0,255,255));
    phi_draw = (phi*360/(M_PI*2)+45)*16;
    theta_scale = 1-1.5*(M_PI/2-theta)/(2*M_PI);

    QPen pen(Qt::black);
    pen.setWidth(2);
    QBrush brush(radialGradient);
    painter->save();
    painter->setBrush(brush);


    QLine arc_scale(pos_struct.camera_rect.center(),QPoint(pos_struct.camera_rect.x(),(pos_struct.camera_rect.y())));
    painter->drawPie(pos_struct.camera_rect,phi_draw,100*16);
    painter->setPen(Qt::black);
    painter->drawArc((int) arc_scale.x1() + arc_scale.dx()*theta_scale,
                     (int) arc_scale.y1() + arc_scale.dy()*theta_scale,
                     (int) pos_struct.camera_rect.width()*theta_scale,
                     (int) pos_struct.camera_rect.height()*theta_scale,
                     phi_draw, 100*16); // Arc is drawn in 16:th of degrees (Qt is funny sometimes)
    painter->restore();

}
