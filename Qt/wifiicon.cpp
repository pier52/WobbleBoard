#include "wifiicon.h"
#include <QDebug>

WiFiIcon::WiFiIcon(QWidget *parent,QColor fill_color,QColor background_color, QColor contour_color) : QWidget(parent)
{
    fill_pen.setColor(fill_color);
    background_pen.setColor(background_color);
    contour_pen.setColor(contour_color);

    update();
}

WiFiIcon::~WiFiIcon()
{

}

void WiFiIcon::paintEvent(QPaintEvent *)
{
    QPainter* painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing);

    drawWifiIcon(painter);

    delete painter;
}

void WiFiIcon::drawWifiIcon(QPainter* painter)
{
    if(number_of_bars > 4)
    {
        throw std::runtime_error("Cannot have that many bars");
        return;
    }

    int square_side = std::min(this->rect().height(),this->rect().width());

    // Note: the formula for the 3 variables below where empirically determined
    int x_translation = square_side/7; // How much x translates (i.e. how much the confining rectangle for the arcs) between each arc
    int pen_width = x_translation/1.5;
    int draw_y = pen_width;

    fill_pen.setWidth(pen_width*0.9);
    background_pen.setWidth(pen_width*0.9);
    contour_pen.setWidth(pen_width);

    for(int i=0; i<3; i++)
    {
        painter->setPen(contour_pen);
        painter->drawArc(x_translation*i,draw_y,square_side,square_side,40*16,100*16);

        if(i >= (int)(4-number_of_bars))
        {
            painter->setPen(fill_pen);
            painter->drawArc(x_translation*i,draw_y,square_side,square_side,40*16,100*16);
        }
        else
        {
            painter->setPen(background_pen);
            painter->drawArc(x_translation*i,draw_y,square_side,square_side,40*16,100*16);
        }

        draw_y += x_translation;
        square_side -= 2*x_translation;
    }

    draw_y -= pen_width/2;
    painter->setPen(contour_pen);
    painter->drawEllipse(QRectF(3*x_translation,draw_y,square_side,square_side).center(),square_side/4,square_side/4);

    if(number_of_bars == 0)
    {
        painter->setPen(background_pen);
        painter->setBrush(background_pen.color());
        painter->drawEllipse(QRectF(3*x_translation,draw_y,square_side,square_side).center(),square_side/4,square_side/4);
    }
    else
    {
        painter->setPen(fill_pen);
        painter->setBrush(fill_pen.color());
        painter->drawEllipse(QRectF(3*x_translation,draw_y,square_side,square_side).center(),square_side/4,square_side/4);
    }

}

void WiFiIcon::display(double rssi)
{
    if(rssi > -50)
    {
        number_of_bars = 4;
    }
    else if(rssi > -55)
    {
        if(number_of_bars != 4)
            number_of_bars = 3;
    }
    else if(rssi > -60)
    {
        number_of_bars = 3;
    }
    else if(rssi > -68)
    {
        if(number_of_bars != 3)
            number_of_bars = 2;
    }
    else if(rssi > -72)
    {
       number_of_bars = 2;
    }
    else if(rssi > -75)
    {
        if(number_of_bars != 2)
            number_of_bars = 1;
    }
    else if(rssi > -78)
    {
       number_of_bars = 1;
    }
    else if(rssi > -81)
    {
        if(number_of_bars != 1)
            number_of_bars = 0;
    }
    else
    {
        number_of_bars = 0;
    }

    update();
}

void WiFiIcon::displayLevel(int level)
{
    if(level > 90)
    {
        number_of_bars = 4;
    }
    else if(level > 80)
    {
        if(number_of_bars != 4)
            number_of_bars = 3;
    }
    else if(level > 70)
    {
        number_of_bars = 3;
    }
    else if(level > 60)
    {
        if(number_of_bars != 3)
            number_of_bars = 2;
    }
    else if(level > 50)
    {
       number_of_bars = 2;
    }
    else if(level > 40)
    {
        if(number_of_bars != 2)
            number_of_bars = 1;
    }
    else if(level > 30)
    {
       number_of_bars = 1;
    }
    else if(level > 20)
    {
        if(number_of_bars != 1)
            number_of_bars = 0;
    }
    else
    {
        number_of_bars = 0;
    }

    update();
}

void WiFiIcon::clearBars()
{
    number_of_bars = 0;
    update();
}
