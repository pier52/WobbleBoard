/**
 * \class PlotLegendLineDisplay
 *
 *
 * \brief
 *      Help class for PlotLegend. Displays the widget showing the formatting of the graph (e.g. dotted red line, brown squares etc.)
 *
 * Lets the user set what colour and line- and scatterstyle to be displayed using public functions.
 *
 * \author Samuel Orn
 *
 */

#ifndef PLOTLEGENDLINEDISPLAY_H
#define PLOTLEGENDLINEDISPLAY_H

#include <QWidget>
#include "qcustomplot.h"

class PlotLegendLineDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit PlotLegendLineDisplay(QColor color, QWidget *parent = 0);
    PlotLegendLineDisplay(QPen pen,QCPScatterStyle::ScatterShape scatter_style, QWidget *parent = 0);
    ~PlotLegendLineDisplay();

    void setColor(QColor color);
    void setLineStyle(Qt::PenStyle style);
    void setScatterStyle(QCPScatterStyle::ScatterShape style);

    /// Returns a copy of the object.
    PlotLegendLineDisplay* copy();

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QPen pen;
    QCPScatterStyle::ScatterShape scatter_style;
};

#endif // PLOTLEGENDLINEDISPLAY_H
