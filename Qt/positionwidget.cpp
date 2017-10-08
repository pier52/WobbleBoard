#include "positionwidget.h"
#include "ui_positionwidget.h"
#include <QPainter>
#include <math.h>
#include <QDebug>
#include <QPdfWriter>

PositionWidget::PositionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PositionWidget)
{
    ui->setupUi(this);
    widget_size = this->size();
    addPos(10,10,0,0,0);

}

PositionWidget::~PositionWidget()
{
    for(int i = 0; i<positions.count(); i++)
    {
        delete positions.at(i);
    }
    positions.clear();
    delete ui;
}

void PositionWidget::paintEvent(QPaintEvent*)
{
    QPainter* painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing);

    if(samples_to_keep > 0) // Don't ask me why I need to do that, but otherwise it enters when samples_to_keep is =-1 !?!?
    {
        int samples_to_delete = positions.count() - samples_to_keep;
        if(samples_to_delete > 0)
        {
            for(int i = 0; i<samples_to_delete; i++)
            {
                delete positions.first();
                positions.removeFirst();
                doubleclick_index--;
            }


        }
    }


    if(is_tracking && positions.count() > 2 && !mouse_is_pressed)
    {
        PositionStruct* current_pos = positions.at(positions.count()-1);
        PositionStruct* previous_pos = positions.at(positions.count()-2);
        translate(-QPoint(current_pos->x_pixel-previous_pos->x_pixel,current_pos->y_pixel-previous_pos->y_pixel));
    }

    drawPath(painter);
    drawVehicleBlob(painter,positions.at(positions.count() - 1));
    drawXaxis(painter);
    drawYaxis(painter);

    if(doubleclick_index  >= 0)
    {
        drawPointInfo(painter);
        emit pointInfo(positions.at(doubleclick_index)->getInfo());
    }
    else
    {
        emit pointInfo(positions.last()->getInfo());
    }

    delete painter;
}

void PositionWidget::addPos(double x_meter, double y_meter, double x_pixel,double y_pixel, double vehicle_orientation,double camera_orientation, double uncertainty)
{
    PositionStruct* pos = new PositionStruct(x_meter,y_meter,x_pixel,this->height()-y_pixel,vehicle_orientation,camera_orientation,uncertainty,QDateTime::currentDateTime());
    positions.append(pos);

    update();
}

void PositionWidget::addPos(double x, double y, double vehicle_orientation,double camera_orientation, double uncertainty)
{
    addPos(x,y,pixels_per_meter_x*x-axis_min.x(),pixels_per_meter_y*y+axis_min.y(),vehicle_orientation,camera_orientation,uncertainty);
}

void PositionWidget::drawPath(QPainter* painter)
{
    painter->save();
    QPointF p1;
    QPointF p2;
    QPen pen;
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidth(3);
    QLinearGradient gradient;

    for(int i = 0; i < positions.count()-1; i++)
    {
        p1.setX(positions.at(i)->x_pixel);
        p1.setY(positions.at(i)->y_pixel);
        p2.setX(positions.at(i+1)->x_pixel);
        p2.setY(positions.at(i+1)->y_pixel);


        gradient.setStart(p1);
        gradient.setFinalStop(p2);
        gradient.setColorAt(0,QColor(0,0,255-255*positions.at(i)->uncertainty,255-255*positions.at(i)->uncertainty));
        gradient.setColorAt(1,QColor(0,0,255-255*positions.at(i+1)->uncertainty,255-255*positions.at(i+1)->uncertainty));
        pen.setBrush(QBrush(gradient));
        painter->setPen(pen);
        painter->drawLine(p1,p2);
    }
    painter->restore();
}

void PositionWidget::drawVehicleBlob(QPainter* painter,PositionStruct* pos)
{
    painter->save();
    painter->setPen(QColor(0,0,255-255*pos->uncertainty,255-255*pos->uncertainty));
    painter->setBrush(QColor(0,0,255-255*pos->uncertainty,255-255*pos->uncertainty));
    QPointF center;
    center.setX(pos->x_pixel);
    center.setY(pos->y_pixel);
    painter->drawEllipse(center,5,5);
    painter->restore();
    drawArrow(painter,pos->vehicle_orientation,20,center.x(),center.y());
}

void PositionWidget::drawArrow(QPainter *painter, double angle, int length,int x_start, int y_start)
{
    painter->save();
    painter->setPen(QPen(Qt::red, 2, Qt::DashDotLine, Qt::RoundCap));
    int x_end = x_start + length*std::cos(angle);
    int y_end = y_start - length*std::sin(angle);

    // Draw line
    painter->drawLine(x_start,y_start,x_end,y_end);

    // Draw arrow head with 30 degree point
    painter->drawLine(x_end,y_end,x_end + 8*std::cos(angle+M_PI - M_PI/6),y_end - 8*std::sin(angle+M_PI - M_PI/6));
    painter->drawLine(x_end,y_end,x_end + 8*std::cos(angle+M_PI + M_PI/6),y_end - 8*std::sin(angle+M_PI + M_PI/6));
    painter->restore();
}

void PositionWidget::debug_step(int step_length,double orientation,double camera_orientation, double uncertainty)
{
    double orientation_change;
    if(orientation == 0)
    {
        orientation_change = 0;
    }
    else if(orientation > 0)
    {
        if(orientation >= old_orientation)
            orientation_change = -0.1;
        else
            orientation_change = 0.1;
        old_orientation = orientation;
    }
    else if(orientation < 0)
    {
        if(orientation > old_orientation)
            orientation_change = -0.1;
        else
            orientation_change = 0.1;
        old_orientation = orientation;
    }

    PositionStruct current_pos = *positions.at(positions.count()-1);
    double new_x = current_pos.x_meter + step_length*std::cos(current_pos.vehicle_orientation);
    double new_y = current_pos.y_meter + step_length*std::sin(current_pos.vehicle_orientation);
    double new_orientation = current_pos.vehicle_orientation+orientation_change;
    addPos(new_x,new_y,new_orientation,camera_orientation+orientation_change,uncertainty);
}

void PositionWidget::drawXaxis(QPainter *painter)
{
    painter->drawLine(0,this->height()-5,this->width(),this->height()-5);
    painter->drawLine(this->width(),this->height()-5,this->width()-10,this->height());
    painter->drawLine(this->width(),this->height()-5,this->width()-10,this->height()-10);

    // Have ticks every 50 pixels
    QRect text_rect;
    text_rect.setY(this->height()-32);
    text_rect.setHeight(20);
    for(int i = 50; i<this->width(); i+=50)
    {
        painter->drawLine(i,this->height(),i,this->height()-10);
        text_rect.setLeft(i-25);
        text_rect.setRight(i+25);
        painter->drawText(text_rect,Qt::AlignHCenter|Qt::AlignBottom,QString::number((axis_min.x()+i)/(double) pixels_per_meter_x,'f',2));
    }
}

void PositionWidget::drawYaxis(QPainter *painter)
{
    painter->drawLine(0,this->height(),0,0);

    // Have ticks every 50 pixels
    QRect text_rect;
    text_rect.setX(18);
    text_rect.setWidth(50);
    for(int i = this->height()-50; i>0; i-=50)
    {
        painter->drawLine(0,i,10,i);
        text_rect.setBottom(i-25);
        text_rect.setTop(i+25);
        painter->drawText(text_rect,Qt::AlignVCenter|Qt::AlignLeft,QString::number((this->height()-(axis_min.y()+i))/(double) pixels_per_meter_y,'f',2));
    }
}

void PositionWidget::mousePressEvent(QMouseEvent *event)
{
    mouse_is_pressed = true;
    mouse_press_position = event->localPos().toPoint();
}

void PositionWidget::mouseReleaseEvent(QMouseEvent*)
{
    mouse_is_pressed = false;
}

void PositionWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(mouse_is_pressed)
    {
        translate(event->localPos().toPoint() - mouse_press_position);
        update();
        mouse_press_position = event->localPos().toPoint();
    }
}

void PositionWidget::wheelEvent(QWheelEvent *event)
{
    QPointF old_center_meter((this->width()/2.0+axis_min.x())/pixels_per_meter_x,(this->height()/2.0 - axis_min.y())/pixels_per_meter_y);
    pixels_per_meter_x += event->angleDelta().y()/60;
    if(pixels_per_meter_x < 1)
    {
        pixels_per_meter_x = 1;
    }
    pixels_per_meter_y += event->angleDelta().y()/60;
    if(pixels_per_meter_y < 1)
    {
        pixels_per_meter_y = 1;
    }



    for(int i = 0; i < positions.count(); i++)
    {
        positions.at(i)->x_pixel = pixels_per_meter_x * positions.at(i)->x_meter - axis_min.x();
        positions.at(i)->y_pixel = this->height() - (pixels_per_meter_y*positions.at(i)->y_meter + axis_min.y());
    }

    QPointF new_center_meter((this->width()/2.0+axis_min.x())/pixels_per_meter_x,(this->height()/2.0 - axis_min.y())/pixels_per_meter_y);
    QPointF translation  = old_center_meter-new_center_meter;
    translation.setX((int) translation.x()*pixels_per_meter_x);
    translation.setY((int) translation.y()*pixels_per_meter_y);
    translate(translation.toPoint()); //Zoom around center of canvas area
    update();
}

void PositionWidget::translate(QPoint translation)
{
    axis_min -= translation;
    for(int i = 0; i<positions.count(); i++)
    {
        positions.at(i)->translate_pixel(translation);
    }
}

void PositionWidget::resizeEvent(QResizeEvent *)
{
    for(int i = 0; i<positions.count(); i++)
    {
        positions.at(i)->translate_pixel(QPoint(0,this->height() - widget_size.height()));
    }

    widget_size = this->size();
}

void PositionWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    doubleclick_index = findClosestPointIndex(event->pos(),20);
    update();
}

int PositionWidget::findClosestPointIndex(QPointF pixel_point, double max_min_dist)
{
    double min_distance = max_min_dist;
    double distance;
    int min_distance_index = -1;
    for(int i = 0; i < positions.count(); i++)
    {
        distance = calcDistance(pixel_point,QPointF(positions.at(i)->x_pixel,positions.at(i)->y_pixel));
        if(distance < min_distance)
        {
            min_distance = distance;
            min_distance_index = i;
        }
    }

    return min_distance_index;
}

double PositionWidget::calcDistance(QPointF point1, QPointF point2)
{
    return std::sqrt((point1.x()-point2.x())*(point1.x()-point2.x()) + (point1.y()-point2.y())*(point1.y()-point2.y()));
}

void PositionWidget::drawPointInfo(QPainter *painter)
{
    PositionStruct* point = positions.at(doubleclick_index);
    drawVehicleBlob(painter,point);
}

void PositionWidget::clear()
{
    PositionStruct* last = positions.at(positions.count()-1)->copy();
    positions.clear();
    positions.append(last);
    update();
}

void PositionWidget::track(bool track)
{
    is_tracking = track;
}

void PositionWidget::lengthToKeep(unsigned seconds)
{
    seconds_to_keep = seconds;
    samples_to_keep = -1;
}

void PositionWidget::samplesToKeep(int num_samples)
{
    samples_to_keep = num_samples;
    seconds_to_keep = -1;
}

void PositionWidget::centerOnCurrentPos()
{
    PositionStruct* current_pos = positions.last();

    QPoint middle = QPoint(this->width()/2.0,this->height()/2.0);

    translate(middle - QPoint(current_pos->x_pixel,current_pos->y_pixel));
    update();
}

void PositionWidget::savePdf()
{

}
