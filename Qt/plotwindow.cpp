#include "plotwindow.h"
#include "ui_plotwindow.h"
#include <QDebug>


/******************************************************************************
 *                            CONSTRUCTORS                                    *
 *****************************************************************************/

PlotWindow::PlotWindow(bool x_timeaxis,QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow)
{
    ui->setupUi(this);

    key_handler = new PlotWindowKeyHandler(this);

    x_timeaxis = true;
    setTimeAxis(x_timeaxis);

    ui->plot->setInteractions(QCP::iSelectPlottables | QCP::iSelectAxes | QCP::iRangeDrag);

    x_axis_size = 50;
    x_axis_middle = x_axis_size/2;

    zoom_x = true;
    zoom_y = true;
    zoom_y2 = true;
    x_axis_time = x_timeaxis;


    track_buttons1 = new QButtonGroup();
    scale_buttons1 = new QButtonGroup();
    visible_buttons1 = new QButtonGroup();
    track_buttons1->setExclusive(false);
    scale_buttons1->setExclusive(false);
    visible_buttons1->setExclusive(false);

    track_buttons2 = new QButtonGroup();
    scale_buttons2 = new QButtonGroup();
    visible_buttons2 = new QButtonGroup();
    track_buttons2->setExclusive(false);
    scale_buttons2->setExclusive(false);
    visible_buttons2->setExclusive(false);

    connect(key_handler,SIGNAL(zoom(int)),this,SLOT(zoom(int)));
    connect(ui->plot,SIGNAL(mouseWheel(QWheelEvent*)),this,SLOT(zoomEvent(QWheelEvent*)));
    connect(ui->plot,SIGNAL(selectionChangedByUser()),this,SLOT(on_selectionChanged()));
    connect(ui->plot,SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)),this,SLOT(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)));
    connect(ui->plot,SIGNAL(plottableDoubleClick(QCPAbstractPlottable*,int,QMouseEvent*)),this,SLOT(plottableDoubleClick(QCPAbstractPlottable*,int,QMouseEvent*)));
    connectToLegend();

    // Debugging
    timer = new QTimer();
    timer->setInterval(100);
    rand_timer = new QTimer();
    rand_timer->setInterval(3000);

    addGraph("Current", "Time","Ampere");
    addGraph("Potential", "Time","Voltage");
    addGraph("Potential 2", "Time","Voltage");
    addGraph("Motorboard Debug","Time","Value");


    connect(timer,SIGNAL(timeout()),this,SLOT(plotTest()));
    connect(rand_timer,SIGNAL(timeout()),this,SLOT(randChange()));

    timer->start();
    rand_timer->start();

    point_coords.setParent(ui->plot);
    point_coords.setHidden(true);
    point_coords.setReadOnly(true);
    point_coords.setFrame(false);
    QFont font;
    font.setBold(true);
    point_coords.setFont(font);
}

PlotWindow::~PlotWindow()
{
    delete track_buttons1;
    delete scale_buttons1;
    delete visible_buttons1;
    delete track_buttons2;
    delete scale_buttons2;
    delete visible_buttons2;
    delete timer;
    delete rand_timer;
    delete key_handler;
    delete ui;
}

void PlotWindow::connectToLegend()
{
    connect(ui->plotLegend,SIGNAL(setAutoScaleX(bool)),this,SLOT(setAutoScaleX(bool)));
    connect(ui->plotLegend,SIGNAL(setAutoScaleYGlobal(bool,int)),this,SLOT(setAutoScaleYGlobal(bool,int)));
    connect(ui->plotLegend,SIGNAL(setGraphSelection(int,bool)),this,SLOT(setGraphSelected(int,bool)));
    connect(ui->plotLegend,SIGNAL(setGraphValueAxis(int,int)),this,SLOT(setGraphValueAxis(int,int)));
    connect(ui->plotLegend,SIGNAL(setGraphVisible(int,bool)),this,SLOT(setGraphVisible(int,bool)));
    connect(ui->plotLegend,SIGNAL(setRangeDrag(bool)),this,SLOT(setRangeDrag(bool)));
    connect(ui->plotLegend,SIGNAL(setXLabel(QString)),this,SLOT(setXLabel(QString)));
    connect(ui->plotLegend,SIGNAL(setYAxisVisible(bool,int)),this,SLOT(setYAxisVisible(bool,int)));
    connect(ui->plotLegend,SIGNAL(setYLabel(QString,int)),this,SLOT(setYLabel(QString,int)));
    connect(ui->plotLegend,SIGNAL(setTimeAxis(bool)),this,SLOT(setTimeAxis(bool)));
    connect(ui->plotLegend,SIGNAL(changeGraphColor(int,QColor)),this,SLOT(changeGraphColor(int,QColor)));
    connect(ui->plotLegend,SIGNAL(changeGraphLineStyle(int,Qt::PenStyle)),this,SLOT(changeGraphLine(int,Qt::PenStyle)));
    connect(ui->plotLegend,SIGNAL(changeGraphScatterStyle(int,QCPScatterStyle)),this,SLOT(changeGraphScatter(int,QCPScatterStyle)));

    ui->plotLegend->setButtonGroups(scale_buttons1,scale_buttons2,track_buttons1,track_buttons2);
}

/******************************************************************************
 *                            PUBLIC SLOTS                                    *
 *****************************************************************************/

void PlotWindow::setTimeAxis(bool time_axis)
{
    //ui->plot->xAxis->ticker().clear(); // Too dangerous; if data arrives while no ticker: crash!
    x_axis_time = time_axis;

    if(time_axis) // Set up x-axis to display time with second accuracy
    {
        QSharedPointer<QCPAxisTickerTime> time_ticker(new QCPAxisTickerTime);
        time_ticker.data()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssReadability);
        time_ticker.data()->setTimeFormat("%h:%m:%s");
        ui->plot->xAxis->setTicker(time_ticker);
    }
    else
    {
        QSharedPointer<QCPAxisTicker> ticker(new QCPAxisTicker);
        ticker.data()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssReadability);
        ui->plot->xAxis->setTicker(ticker);
    }
    ui->plot->replot();
}

void PlotWindow::addPoint(double x, double y,unsigned graph_num)
{
    if(this->isVisible())
    {
    }
    ui->plot->graph(graph_num)->addData(x,y);
    refresh();

}

void PlotWindow::addPoint(double x, double y, QString graph_name)
{
    addPoint(x,y,graphIndex(graph_name));
}

void PlotWindow::addPointTime(double val, unsigned graph_num)
{
    QTime current_time = QTime().currentTime();
    double d_time = current_time.hour()*3600+current_time.minute()*60+current_time.second()+current_time.msec()/(double)1000;
    addPoint(d_time,val,graph_num);
}

void PlotWindow::addPointTime(double val, QString graph_name)
{
    QTime current_time = QTime().currentTime();
    double d_time = current_time.hour()*3600+current_time.minute()*60+current_time.second()+current_time.msec()/(double)1000;
    addPoint(d_time,val,graph_name);
}

void PlotWindow::addPointTimeSafe(double val, QString graph_name)
{
    if(graphIndex(graph_name) == -1)
    {
        qDebug() << "No such graph " + graph_name;
        return;
    }

    addPointTime(val,graph_name);
}

void PlotWindow::addGraph(QString name, QString x_axis_name, QString y_axis_name,QColor color)
{

    if(color == Qt::transparent)
    {
        if(!colors.empty())
        {
            color = colors.first();
            colors.removeFirst();
        }
        else
        {
            color = Qt::black;
        }
    }
    else if(colors.contains(color))
    {
        colors.removeOne(color);
    }
    
    ui->plot->addGraph();

    ui->plotLegend->addGraph(name,color,x_axis_name,y_axis_name);

    int graph_number = ui->plot->graphCount()-1;

    ui->plot->graph(graph_number)->setName(name);
    ui->plot->graph(graph_number)->setPen(QPen(color));

    QCPSelectionDecorator* decorator = new QCPSelectionDecorator();
    QPen pen;
    pen.setColor(color);
    pen.setWidth(2);
    decorator->setPen(pen);
    ui->plot->graph(graph_number)->setSelectionDecorator(decorator);

    ui->plot->graph(graph_number)->setVisible(false);
}

void PlotWindow::addGraph(QString name, QString x_axis_name, QString y_axis_name)
{
    if(graphIndex(name) != -1) // Graph already exists
        return;

    addGraph(name,x_axis_name,y_axis_name,Qt::transparent);
}

void PlotWindow::removeGraph(QString name)
{
    int index = graphIndex(name);
    if(index == -1)
        return;

    colors.insert(0,ui->plot->graph(index)->selectionDecorator()->pen().color());
    ui->plot->removeGraph(index);

    ui->plotLegend->removeGraph(name);
}

int PlotWindow::graphIndex(const QString graph_name) const
{
    for(int i=0; i<ui->plot->graphCount(); i++)
    {
        if(ui->plot->graph(i)->name().compare(graph_name) == 0)
        {
            return i;
        }
    }

    return -1;
}

void PlotWindow::setRangeDrag(bool drag)
{
    ui->plot->setInteraction(QCP::iRangeDrag,drag);
}

void PlotWindow::setAutoScaleX(bool scale)
{
    autoscale_x_global = scale;
}

void PlotWindow::setGraphSelected(int graph_num, bool selected)
{
    QCPDataSelection selection;

    if(selected)
        selection.addDataRange(QCPDataRange(0,1));
    else
        selection.addDataRange(QCPDataRange(0,0));

    ui->plot->graph(graph_num)->setSelectable(QCP::stWhole);
    point_coords.clear();
    point_coords.setVisible(false);

    ui->plot->graph(graph_num)->setSelection(selection);
}

void PlotWindow::setAutoScaleYGlobal(bool scale, int axis)
{
    if(axis == 1)
        autoscale_y_global = scale;
    else if(axis==2)
        autoscale_y2_global = scale;
    else
        throw std::runtime_error("Axis must be 1 or 2");
}

void PlotWindow::setGraphVisible(int graph_num, bool visible)
{
    ui->plot->graph(graph_num)->setVisible(visible);
}

void PlotWindow::setXLabel(QString label)
{
    ui->plot->xAxis->setLabel(label);
}

void PlotWindow::setYLabel(QString label, int num)
{
    if(num == 1)
        ui->plot->yAxis->setLabel(label);
    else if (num == 2)
        ui->plot->yAxis2->setLabel(label);
    else
        throw std::runtime_error("No such yAxis; needs to be 1 or 2");
}\


void PlotWindow::setYAxisVisible(bool visible, int num)
{
    if(num == 1)
        ui->plot->yAxis->setVisible(visible);
    else if (num == 2)
        ui->plot->yAxis2->setVisible(visible);
    else
        throw std::runtime_error("No such yAxis; needs to be 1 or 2");
}

void PlotWindow::setGraphValueAxis(int graph_num, int axis_num)
{
    if(axis_num == 1)
        ui->plot->graph(graph_num)->setValueAxis(ui->plot->yAxis);
    else if (axis_num == 2)
        ui->plot->graph(graph_num)->setValueAxis(ui->plot->yAxis2);
    else
        throw std::runtime_error("No such yAxis; needs to be 1 or 2");
}

void PlotWindow::changeGraphColor(int graph_num, QColor color)
{
    QPen current_pen = ui->plot->graph(graph_num)->pen();
    current_pen.setColor(color);
    ui->plot->graph(graph_num)->setPen(current_pen);
    current_pen.setWidth(2);
    ui->plot->graph(graph_num)->selectionDecorator()->setPen(current_pen);
    QCPScatterStyle style = ui->plot->graph(graph_num)->scatterStyle();

    QPen current_color_pen(color);
    style.setPen(current_color_pen);
    ui->plot->graph(graph_num)->setScatterStyle(style);
    current_color_pen.setWidth(2);
    style.setPen(current_color_pen);
    ui->plot->graph(graph_num)->selectionDecorator()->setScatterStyle(style);

    refresh();
}

void PlotWindow::changeGraphLine(int graph_num, Qt::PenStyle line_style)
{
    QPen current_pen = ui->plot->graph(graph_num)->pen();
    current_pen.setStyle(line_style);
    ui->plot->graph(graph_num)->setPen(current_pen);
    current_pen.setWidth(2);
    ui->plot->graph(graph_num)->selectionDecorator()->setPen(current_pen);

    refresh();
}

void PlotWindow::changeGraphScatter(int graph_num, QCPScatterStyle scatter_style)
{
    QPen pen(ui->plot->graph(graph_num)->pen().color());
    scatter_style.setPen(pen);
    ui->plot->graph(graph_num)->setScatterStyle(scatter_style);
    pen.setWidth(2);
    scatter_style.setPen(pen);
    ui->plot->graph(graph_num)->selectionDecorator()->setScatterStyle(scatter_style);

    refresh();
}

/******************************************************************************
 *                               PRIVATE                                      *
 *****************************************************************************/
void PlotWindow::closeEvent(QCloseEvent *event)
{
    //on_actionClear_plot_triggered();

    QWidget::closeEvent(event);
}

void PlotWindow::refresh()
{

    if(autoscale_y_global)
    {
        QCPRange y_range = globalYRange(ui->plot->yAxis);
        ui->plot->yAxis->setRange(y_range.lower,y_range.upper);
    }
    else if(scale_buttons1->checkedId() != -1) // A scale button is checked
    {
        QCPRange y_range_x = localYRange(scale_buttons1);
        ui->plot->yAxis->setRange(y_range_x.lower,y_range_x.upper);
    }



    if(autoscale_y2_global)
    {
        QCPRange y_range = globalYRange(ui->plot->yAxis2);
        ui->plot->yAxis2->setRange(y_range.lower,y_range.upper);
    }
    else if(scale_buttons2->checkedId() != -1) // A scale button is checked
    {
        QCPRange y_range_x = localYRange(scale_buttons2);
        ui->plot->yAxis2->setRange(y_range_x.lower,y_range_x.upper);
    }



    if(autoscale_x_global)
    {
        QCPRange x_range = globalXRange();
        ui->plot->xAxis->setRange(x_range.lower,x_range.upper);
    }
    else if(track_buttons1->checkedId() != -1 || track_buttons2->checkedId() != -1)
    {
        trackX();
    }

    if(!point_coords.text().isEmpty())
    {
        QPointF point = ui->plot->graph(point_coord_graph)->coordsToPixels(point_coord_x,point_coord_y);
        point_coords.move(point.x()-point_coords.width()/2,point.y()-point_coords.height()*1.5);
        point_coords.show();
    }

    updateXAxisTicks();
    ui->plot->replot();

}

QCPRange PlotWindow::localYRange(const QButtonGroup* scale_buttons) const
{
    QCPRange range;
    bool found;
    double lower = std::numeric_limits<double>::max();
    double upper = -std::numeric_limits<double>::max();
    QList<QAbstractButton*> checkboxes = scale_buttons->buttons();
    QCPRange x_range = ui->plot->xAxis->range();

    for(int i=0; i < checkboxes.count(); i++)
    {
        int btn_id = scale_buttons->id(checkboxes.at(i));

        if(checkboxes.at(i)->isChecked() && ui->plot->graph(btn_id)->visible())
        {
            range = ui->plot->graph(btn_id)->getValueRange(found,QCP::sdBoth,QCPRange(x_range.lower,x_range.upper));
            if(range.lower < lower)
            {
                lower = range.lower;
            }
            if(range.upper > upper)
            {
                upper = range.upper;
            }
        }

    }

    if(lower == upper)
    {
        upper++;
        lower--;
    }

    return QCPRange(lower,upper);
}

QCPRange PlotWindow::globalYRange(const QCPAxis* axis) const
{
    QCPRange range;
    bool found;
    double lower = std::numeric_limits<double>::max();
    double upper = -std::numeric_limits<double>::max();

    for(int i=0; i < ui->plot->graphCount(); i++)
    {
        if(ui->plot->graph(i)->visible() && ui->plot->graph(i)->valueAxis() == axis)
        {
            range = ui->plot->graph(i)->getValueRange(found);
            if(range.lower < lower)
            {
                lower = range.lower;
            }
            if(range.upper > upper)
            {
                upper = range.upper;
            }
        }
    }

    if(lower == upper)
    {
        upper++;
        lower--;
    }

    return QCPRange(lower,upper);
}

QCPRange PlotWindow::globalXRange() const
{
    QCPRange range;
    bool found;
    double lower = std::numeric_limits<double>::max();
    double upper = -std::numeric_limits<double>::max();

    for(int i=0; i < ui->plot->graphCount(); i++)
    {
        if(ui->plot->graph(i)->visible())
        {
            range = ui->plot->graph(i)->getKeyRange(found);
            if(range.lower < lower)
            {
                lower = range.lower;
            }
            if(range.upper > upper)
            {
                upper = range.upper;
            }
        }

    }

    return QCPRange(lower,upper);
}

void PlotWindow::trackX()
{
    double size;
    bool found;
    double x_lower = std::numeric_limits<double>::max();
    double x_upper = -std::numeric_limits<double>::max();
    QList<QAbstractButton*> checkboxes = track_buttons1->buttons();
    checkboxes.append(track_buttons2->buttons());

    for(int i=0; i < checkboxes.count(); i++)
    {
        int btn_id = track_buttons1->id(checkboxes.at(i));
        if(btn_id == -1)
            btn_id = track_buttons2->id(checkboxes.at(i));

        if(checkboxes.at(i)->isChecked() && ui->plot->graph(btn_id)->visible())
        {

            size = ui->plot->graph(btn_id)->getKeyRange(found).upper;
            if(size < x_lower)
            {
                x_lower = size;
            }
            if(size > x_upper)
            {
                x_upper = size;
            }
        }

    }

    ui->plot->xAxis->setRange(x_lower-x_axis_size*0.9,x_upper+x_axis_size*0.1);
}

void PlotWindow::updateXAxisTicks()
{
    if(x_axis_time)
    {
        QSharedPointer<QCPAxisTickerTime> time_ticker(new QCPAxisTickerTime);
        if(x_axis_size < 2)
        {
            time_ticker.data()->setTimeFormat("%h:%m:%s.%z");
        }
        else
        {
            time_ticker.data()->setTimeFormat("%h:%m:%s");
        }
        ui->plot->xAxis->setTicker(time_ticker);
    }
}


/******************************************************************************
 *                          UI EVENT HANDLERS                                 *
 *****************************************************************************/

void PlotWindow::zoom(int zoom)
{
    QCPRange y_range1 = ui->plot->yAxis->range();
    QCPRange y_range2 = ui->plot->yAxis2->range();
    QCPRange x_range = ui->plot->xAxis->range();
    // Zoom in steps of 5% of axis length
    double x_change = x_range.size()*0.05;
    double y_change1 = y_range1.size()*0.05;
    double y_change2 = y_range2.size()*0.05;

    if(zoom < 0)
    {
        if(zoom_y)
            ui->plot->yAxis->setRange(y_range1.lower-y_change1/2,y_range1.upper+y_change1/2);

        if(zoom_y2)
            ui->plot->yAxis2->setRange(y_range2.lower-y_change2/2,y_range2.upper+y_change2/2);

        if(zoom_x)
        {
            x_axis_size += x_change;

            if(track_buttons1->checkedId() == -1 || track_buttons2->checkedId() == -1)
            {
                ui->plot->xAxis->setRange(x_range.lower-x_change/2,x_range.upper+x_change/2);
            }
        }

    }
    else
    {
        if(zoom_y)
            ui->plot->yAxis->setRange(y_range1.lower+y_change1/2,y_range1.upper-y_change1/2);

        if(zoom_y2)
            ui->plot->yAxis2->setRange(y_range2.lower+y_change2/2,y_range2.upper-y_change2/2);

        if(zoom_x)
        {
            x_axis_size -= x_change;


            if(x_axis_size < 0.1) // axis size cannot be negative
                x_axis_size = 0.1;


            if(track_buttons1->checkedId() == -1 || track_buttons2->checkedId() == -1)
            {
                ui->plot->xAxis->setRange(x_range.lower+x_change/2,x_range.upper-x_change/2);
            }
        }
    }

    refresh();
}

void PlotWindow::zoomEvent(QWheelEvent *event)
{
    zoom(event->angleDelta().y());
}

void PlotWindow::on_selectionChanged()
{
    if(ui->plot->selectedGraphs().isEmpty())
    {
        ui->plotLegend->clearBoldLegend();
        point_coords.clear();
        point_coords.setVisible(false);
    }

    if(!ui->plot->selectedAxes().isEmpty()) // New axis selected
    {
        QCPAxis* zoom_axis = ui->plot->selectedAxes().at(0);
        if(zoom_axis == ui->plot->yAxis)
        {
            zoom_x = false;
            zoom_y = true;
            zoom_y2 = false;
        }
        else if(zoom_axis == ui->plot->yAxis2)
        {
            zoom_x = false;
            zoom_y = false;
            zoom_y2 = true;
        }
        else if(zoom_axis == ui->plot->xAxis)
        {
            zoom_x = true;
            zoom_y = false;
            zoom_y2 = false;
        }
    }
    else // Default
    {
        zoom_x = true;
        zoom_y = true;
        zoom_y2 = true;
    }

    if(zoom_y)
    {
       ui->plot->axisRect()->setRangeDragAxes(ui->plot->xAxis,ui->plot->yAxis);
    }
    else
    {
       ui->plot->axisRect()->setRangeDragAxes(ui->plot->xAxis,ui->plot->yAxis2);
    }

}

void PlotWindow::on_actionClear_plot_triggered()
{
    QVector<double> empty;

    for(int i=0; i<ui->plot->graphCount(); i++)
    {
        ui->plot->graph(i)->setData(empty,empty);
    }
}


void PlotWindow::on_actionSave_plot_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save graph snapshot"),"",tr("Images (*.png)"));

    if(!fileName.isEmpty() && !fileName.isNull())
        ui->plot->savePng(fileName,0,0,1,100);

}

void PlotWindow::on_actionSave_pdf_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save graph snapshot"),"",tr("Pdf (*.pdf)"));

    if(!fileName.isEmpty() && !fileName.isNull())
        ui->plot->savePdf(fileName);
}

void PlotWindow::plottableDoubleClick(QCPAbstractPlottable* plot,int point_index,QMouseEvent* event)
{
    if(event->button() != Qt::LeftButton)
        return;

    QCPGraph* graph = (QCPGraph*) plot;

    if(graph)
    {
        if(graph->scatterStyle().shape() == QCPScatterStyle::ssNone)
        {
            return;
        }

        graph->setSelectable(QCP::stSingleData);

        point_coord_x = graph->data().data()->at(point_index)->key;
        point_coord_y = graph->data().data()->at(point_index)->value;
        double x_raw = point_coord_x;
        QString x;
        if(x_axis_time)
        {
            int h = (int)x_raw / 3600;
            x_raw -= h*3600;
            int min = (int)x_raw / 60;
            x_raw -= min*60;

            QString minute = QString::number(min);
            QString second = QString::number(x_raw);

            if(min < 10)
            {
                minute.prepend("0");
            }

            if(x_raw < 10)
            {
                second.prepend("0");
            }

            x = QString::number(h) + ":" + minute + ":" + second;
        }
        else
        {
            x = QString::number(x_raw);
        }

        QString coords = "(" + x + " ; " + QString::number(point_coord_y) + ")";

        point_coords.setText(coords);
        QFontMetrics fm = point_coords.fontMetrics();
        point_coords.resize(fm.boundingRect(coords+"D").width(),fm.boundingRect(coords).height()); // The D is needed here, otherwise not big enough
        point_coord_graph = graphIndex(graph->name());
        refresh();
    }
}

void PlotWindow::plottableClick(QCPAbstractPlottable* plot,int,QMouseEvent* event)
{
    if(event->button() != Qt::LeftButton)
        return;

    QCPGraph* graph = (QCPGraph*) plot;

    if(graph)
    {
        graph->setSelectable(QCP::stWhole);
        point_coords.clear();
        point_coords.setVisible(false);

        int table_row = ui->plotLegend->getTableRow(1,graph->name());
        if(table_row != -1)
            ui->plotLegend->changeBoldLegendRow(table_row,1);
        else
        {
            table_row = ui->plotLegend->getTableRow(2,graph->name());
            ui->plotLegend->changeBoldLegendRow(table_row,2);
        }
    }
}

/******************************************************************************
 *                      DEBUGGING AND TEST FUNCTIONS                          *
 *****************************************************************************/

void PlotWindow::plotTest()
{
    time+=0.1;
    x+=0.1;

    addPointTime(amp1*sin(amp2*time),"Current");
    addPointTime(-amp2*sin(amp1*time),"Potential"); //0.8*time
    addPointTime(-amp1*sin(-0.7*amp1*time),"Potential 2");
}

void PlotWindow::randChange()
{
    int rand1 = qrand() % 2;
    int rand2 = qrand() % 2;

    if(rand1 && amp1 < 3)
    {
        if(rand2)
            amp1 += 0.1;
        else
            amp1 -= 0.1;
    }

    if(rand2 && amp2 < 3)
    {
        if(rand1)
            amp2 += 0.1;
        else
            amp2 -= 0.1;
    }
}

