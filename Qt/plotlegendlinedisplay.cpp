#include "plotlegendlinedisplay.h"

PlotLegendLineDisplay::PlotLegendLineDisplay(QColor color, QWidget *parent) : QWidget(parent)
{
    setColor(color);
}

PlotLegendLineDisplay::PlotLegendLineDisplay(QPen pen,QCPScatterStyle::ScatterShape scatter_style, QWidget *parent) : QWidget(parent)
{
    this->pen = pen;
    this->scatter_style = scatter_style;
}

PlotLegendLineDisplay::~PlotLegendLineDisplay()
{
}

void PlotLegendLineDisplay::paintEvent(QPaintEvent*)
{
    QPainter* painter = new QPainter(this);

    painter->setPen(pen);

    painter->drawLine(QPoint(0,this->height()/2),QPoint(this->width(),this->height()/2));

    painter->setPen(QPen(pen.color()));

    QPoint middle(width()/2,height()/2);

    switch(scatter_style)
    {
    case QCPScatterStyle::ssCircle:
        painter->drawEllipse(middle,3,3);
        break;
    case QCPScatterStyle::ssDisc:
        painter->setBrush(QBrush(pen.color()));
        painter->drawEllipse(middle,3,3);
        break;
    case QCPScatterStyle::ssCross:
        painter->drawLine(QPoint(middle.x()-3,middle.y()-3),QPoint(middle.x()+3,middle.y()+3));
        painter->drawLine(QPoint(middle.x()-3,middle.y()+3),QPoint(middle.x()+3,middle.y()-3));
        break;
    case QCPScatterStyle::ssDiamond:
        painter->drawLine(QPoint(middle.x()-3,middle.y()),QPoint(middle.x(),middle.y()+3));
        painter->drawLine(QPoint(middle.x()-3,middle.y()),QPoint(middle.x(),middle.y()-3));
        painter->drawLine(QPoint(middle.x()+3,middle.y()),QPoint(middle.x(),middle.y()+3));
        painter->drawLine(QPoint(middle.x()+3,middle.y()),QPoint(middle.x(),middle.y()-3));
        break;
    case QCPScatterStyle::ssDot:
        painter->setBrush(QBrush(pen.color()));
        painter->drawEllipse(QPoint(width()/2,height()/2),1,1);
        break;
    case QCPScatterStyle::ssPlus:
        painter->drawLine(QPoint(middle.x()-3,middle.y()),QPoint(middle.x()+3,middle.y()));
        painter->drawLine(QPoint(middle.x(),middle.y()+3),QPoint(middle.x(),middle.y()-3));
        break;
    case QCPScatterStyle::ssSquare:
        painter->drawRect(QRect(middle.x()-3,middle.y()-3,6,6));
        break;
    case QCPScatterStyle::ssStar:
        painter->drawLine(QPoint(middle.x()-3,middle.y()),QPoint(middle.x()+3,middle.y()));
        painter->drawLine(QPoint(middle.x(),middle.y()+3),QPoint(middle.x(),middle.y()-3));
        painter->drawLine(QPoint(middle.x()-3,middle.y()-3),QPoint(middle.x()+3,middle.y()+3));
        painter->drawLine(QPoint(middle.x()-3,middle.y()+3),QPoint(middle.x()+3,middle.y()-3));
        break;
    case QCPScatterStyle::ssTriangle:
        painter->drawLine(QPoint(middle.x(),middle.y()-3),QPoint(middle.x()-3,middle.y()+3));
        painter->drawLine(QPoint(middle.x(),middle.y()-3),QPoint(middle.x()+3,middle.y()+3));
        painter->drawLine(QPoint(middle.x()-3,middle.y()+3),QPoint(middle.x()+3,middle.y()+3));
        break;
    default:
        break;

    }

    delete painter;
}

void PlotLegendLineDisplay::setColor(QColor color)
{
    pen.setColor(color);
    update();
}

void PlotLegendLineDisplay::setLineStyle(Qt::PenStyle style)
{
    pen.setStyle(style);
    update();
}

void PlotLegendLineDisplay::setScatterStyle(QCPScatterStyle::ScatterShape style)
{
    scatter_style = style;
    update();
}

PlotLegendLineDisplay* PlotLegendLineDisplay::copy()
{
    return new PlotLegendLineDisplay(pen,scatter_style);
}

