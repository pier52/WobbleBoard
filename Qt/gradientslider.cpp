#include "gradientslider.h"
#include <QPainter>

GradientSlider::GradientSlider(QWidget *parent) : QWidget(parent)
{
    frame = true;
    frame_color = Qt::black;
    setGradientHot();
    fill_percentage = 0;
}

void GradientSlider::paintEvent(QPaintEvent *)
{
    QPainter* painter = new QPainter(this);

    if(frame)
    {
        painter->setPen(frame_color);
        QRect rect = this->rect();
        rect.setHeight(this->rect().height()-1); // Otherwise lower boundary will be hidden
        rect.setWidth(this->rect().width()-1);
        painter->drawRect(rect);
    }
    else
    {
        painter->setPen(Qt::NoPen);
    }

    painter->setBrush(QBrush(gradient));

    QRect draw_rect = this->rect();
    draw_rect.setWidth(this->rect().width()*fill_percentage-1);
    draw_rect.setHeight(this->rect().height()-1);
    painter->drawRect(draw_rect);

    delete painter;
}

void GradientSlider::setGradient(QLinearGradient gradient)
{
    this->gradient = gradient;
}

QLinearGradient GradientSlider::getGradient() const
{
    return gradient;
}

void GradientSlider::setGradientHot()
{
    gradient = QLinearGradient(this->rect().left(),this->rect().center().y(),this->rect().right(),this->rect().center().y());

    gradient.setColorAt(0.0,Qt::black);
    gradient.setColorAt(0.3,Qt::red);
    gradient.setColorAt(1.0,Qt::yellow);
    //gradient.setColorAt(1.0,Qt::white);
}

void GradientSlider::setFillPercentage(double fill_percentage)
{
    this->fill_percentage = fill_percentage;
    update();
}

void GradientSlider::setBackground(QColor color)
{
    QPalette palette;
    palette.setColor(QPalette::Background,color);
    this->setAutoFillBackground(true);
    this->setPalette(palette);
}

void GradientSlider::setFrame(bool visible)
{
    frame = visible;
}

void GradientSlider::setFrame(bool visible, QColor color)
{
    frame = visible;
    frame_color = color;
}
