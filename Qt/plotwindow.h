/**
 * \class PlotWindow
 *
 *
 * \brief
 *      Logic class for the plotwindow form. Displays plots, display option configured together with the custom widget PlotLegend.
 *
 * The plotwindow can plot samples in 2D, either X and  Y coordinates are provided, or the X-axis is chosen to be time
 * which means the class timestamps and displays the time the sample was plotted as it's X value. \n
 * The plot area can display one X-axis and 2 Y-axes simultaneously. Graphs can be zoomed, dragged, selected, tracked etc.
 * (see also PlotLegend). Furthermore a snapshot of the graph area can be saved as a .png image file. \n
 * New graphs are added via the <addGraph> function. This can be done anytime during runtime. Analoguosly, graphs can at any time be removed via the <removeGraph> slot. \n \n
 * The graph canvas and its surrounding functions are provided by the QCustomPlot library: http://www.qcustomplot.com/
 *
 * \author Samuel Orn
 *
 */

#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget>
#include <QVector>
#include <QTimer>
#include <math.h>
#include "qcustomplot.h"
#include <limits>
#include <QList>
#include <QColor>
#include "plotwindowkeyhandler.h"

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(bool x_timeaxis = false,QMainWindow *parent = 0);
    ~PlotWindow();

public slots:

    /// Add point (x,y) to graph <graph_name>. If <graph_name> does not exist an error is thrown.
    void addPoint(double x, double y, QString graph_name);

    /// Same as addPoint, but unly takes a y value and assign the current time as the X-value
    void addPointTime(double val, QString graph_name);

    /// Same as addPoint, but never throws
    void addPointTimeSafe(double val, QString graph_name);

    /// Adds a graph, with identifier <name> and specifying its X-axis (use 't', 'time' or 'Time' for time axis),
    /// Y-axis and color. Notifies PlotLegend to add the graph to the legend.
    void addGraph(QString name, QString x_axis_name, QString y_axis_name,QColor color);

    /// Same as addGraph, but assigns a unique color to the graph (given that there are no more than 13 graphs)
    void addGraph(QString name, QString x_axis_name, QString y_axis_name);

    /// Removes the graph <name>. Notifies PlotLegend to remove the graph from the legend.
    void removeGraph(QString name);

    /// Sets whether it is possible to drag the plots or not
    void setRangeDrag(bool drag);

    /// Sets whether the x-axis is auto scaled (meaning X-axis is scaled so that all data is shown)
    void setAutoScaleX(bool scale);

    /// Sets whether the y-axis (1 or 2) is auto scaled (meaning Y-axis is scaled so that all data in the current X-range is shown)
    void setAutoScaleYGlobal(bool scale,int axis);

    /// Sets a graph selected, meaning it is displayed thick, and its legend entry is bold
    void setGraphSelected(int graph,bool selected);

    /// Sets the visibilty of a graph
    void setGraphVisible(int graph_num, bool visible);

    /// Sets the X-axis label
    void setXLabel(QString label);

    /// Sets the Y-axis label (for axis number 1 or 2)
    void setYLabel(QString label, int num);

    /// Sets the visibility of Y-axis 1 or 2
    void setYAxisVisible(bool visible, int num);

    /// Assigns a certain graph to Y-axis 1 or 2
    void setGraphValueAxis(int graph_num,int axis_num);

    /// Sets whether the X-axis is to display time or not
    void setTimeAxis(bool time_axis);

    /// Changes the color of a graph
    void changeGraphColor(int graph_num, QColor color);

    /// Changes the line style of a graph
    void changeGraphLine(int graph_num, Qt::PenStyle line_style);

    /// Changes the scatter style of a graph
    void changeGraphScatter(int graph_num, QCPScatterStyle scatter_style);


private slots:

    /// Debugging
    void plotTest();

    /// Debugging
    void randChange();

    void zoomEvent(QWheelEvent* event);

    void zoom(int zoom);

    void on_selectionChanged();

    void on_actionClear_plot_triggered();

    void on_actionSave_plot_triggered();

    void on_actionSave_pdf_triggered();

    void plottableClick(QCPAbstractPlottable*,int,QMouseEvent*);
    void plottableDoubleClick(QCPAbstractPlottable* plot,int point_index,QMouseEvent*);

private:
    void addPoint(double x, double y, unsigned graph_num);
    void addPointTime(double val, unsigned graph_num);

    void closeEvent(QCloseEvent *event) override;

    /// Refreshes the plot canvas
    void refresh();

    /// Tracks the X-axis (i.e. the x-axis moves 'forward' as new data comes in)
    void trackX();

    /// Changes the resolution of the ticks on the X-axis, depending on the zoom
    void updateXAxisTicks();

    /// Helper to connect all signals to PlotLegend at startup
    void connectToLegend();

    /// Returns the internal index of the graph, given the graph name
    int graphIndex(const QString graph_name) const;

    /// Returns the Y-range of all data on <axis>, no matter if it is visible in the canvas or not
    QCPRange globalYRange(const QCPAxis* axis) const;

    /// Returns the -range of all data, no matter if it is visible in the canvas or not
    QCPRange globalXRange() const ;

    /// Returns the Y-range of all data which is visible on canvas on <axis>
    QCPRange localYRange(const QButtonGroup* scale_buttons) const;

    PlotWindowKeyHandler* key_handler;

    Ui::PlotWindow *ui;

    // These buttongroups are shared with PlotLegend
    QButtonGroup* track_buttons1;
    QButtonGroup* scale_buttons1;
    QButtonGroup* visible_buttons1;

    QButtonGroup* track_buttons2;
    QButtonGroup* scale_buttons2;
    QButtonGroup* visible_buttons2;



    double x_axis_size;
    double x_axis_middle;

    bool autoscale_y_global = false;
    bool autoscale_y2_global = false;
    bool autoscale_x_global = false;
    bool zoom_x;
    bool zoom_y;
    bool zoom_y2;

    bool x_axis_time;

    // Debugging
    QTimer* timer;
    QTimer* rand_timer;
    double time = 0;
    double amp1 = 1;
    double amp2 = 1;
    double x = 0;

    QList<QColor> colors = QList<QColor>() << Qt::red << Qt::blue << Qt::green << Qt::black << Qt::magenta << Qt::gray << Qt::cyan
                                                 << Qt::darkRed << Qt::darkBlue << Qt::darkGreen << Qt::darkMagenta << Qt::darkGray << Qt::darkCyan;

    QLineEdit point_coords;
    double point_coord_x;
    double point_coord_y;
    int point_coord_graph;
};

#endif // PLOTWINDOW_H
