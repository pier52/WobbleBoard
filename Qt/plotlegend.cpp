#include "plotlegend.h"
#include <QLayout>
#include <QHeaderView>
#include <QStringList>


/******************************************************************************
 *                            CONSTRUCTORS                                    *
 *****************************************************************************/

PlotLegend::PlotLegend(QWidget *parent) : QWidget(parent)
{
    initLayout();

    bold_legend_row = -1;
    bold_legend_row2 = -1;

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

    line_menu = new PlotLineMenu();

    connect(line_menu,SIGNAL(colorChange(int,int,QColor)),this,SLOT(changeColor(int,int,QColor)));
    connect(line_menu,SIGNAL(lineChange(int,int,Qt::PenStyle)),this,SLOT(changeLineStyle(int,int,Qt::PenStyle)));
    connect(line_menu,SIGNAL(scatterChange(int,int,QCPScatterStyle)),this,SLOT(changeScatterStyle(int,int,QCPScatterStyle)));

    this->show();
}


PlotLegend::~PlotLegend()
{
    /*
    delete scale_buttons1;
    delete scale_buttons2;
    delete track_buttons1;
    delete track_buttons2;
    delete visible_buttons1;
    delete visible_buttons2;
    */

    delete xComboBox;
    delete y1ComboBox;
    delete y2ComboBox;
    delete globalScaleXCheckBox;
    delete globalScaleY1CheckBox;
    delete globalScaleY2CheckBox;
    delete y1axisText;
    delete y2axisText;
    delete xaxisText;

    delete legend1;
    delete legend2;
}

void PlotLegend::initLayout()
{
    QPalette transparent_base;
    transparent_base.setColor(QPalette::Base,QColor(0, 0, 0, 0));


    legend1 = new QTableWidget(0,0);
    legend1->setFrameShape(QFrame::NoFrame);
    legend1->setEditTriggers(0);
    legend1->setSelectionMode(QAbstractItemView::NoSelection);
    legend1->setShowGrid(false);
    legend1->horizontalHeader()->setVisible(false);
    legend1->verticalHeader()->setVisible(false);
    legend1->setPalette(transparent_base);
    connect(legend1,SIGNAL(activated(QModelIndex)),this,SLOT(on_graphTableWidget_clicked(QModelIndex)));
    legend1->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(legend1,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(graphTableWidgetContextMenuRequested(QPoint)));


    legend2 = new QTableWidget(0,0);
    legend2->setFrameShape(QFrame::NoFrame);
    legend2->setEditTriggers(0);
    legend2->setSelectionMode(QAbstractItemView::NoSelection);
    legend2->setShowGrid(false);
    legend2->horizontalHeader()->setVisible(false);
    legend2->verticalHeader()->setVisible(false);
    legend2->setPalette(transparent_base);
    connect(legend2,SIGNAL(activated(QModelIndex)),this,SLOT(on_graphTableWidget_2_clicked(QModelIndex)));
    legend2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(legend2,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(graphTableWidgetContextMenuRequested2(QPoint)));

    globalScaleY1CheckBox = new QCheckBox("Scale Y1");
    connect(globalScaleY1CheckBox,SIGNAL(clicked(bool)),this,SLOT(on_autoscaleYGlobalBox_clicked(bool)));

    globalScaleY2CheckBox = new QCheckBox("Scale Y2");
    connect(globalScaleY2CheckBox,SIGNAL(clicked(bool)),this,SLOT(on_autoscaleY2GlobalBox_clicked(bool)));

    globalScaleXCheckBox = new QCheckBox("Scale X");
    connect(globalScaleXCheckBox,SIGNAL(clicked(bool)),this,SLOT(on_autoscaleXGlobalBox_clicked(bool)));

    QFont font("Helvetica [Cronyx]", 0);
    QFontMetrics fm(font);

    y1axisText = new QLineEdit("Y1 axis");
    y1axisText->setReadOnly(true);
    y1axisText->setFrame(QFrame::NoFrame);
    y1axisText->setFixedWidth(fm.width("Y1 axis"));
    y1axisText->setPalette(transparent_base);

    y2axisText = new QLineEdit("Y2 axis");
    y2axisText->setReadOnly(true);
    y2axisText->setFrame(QFrame::NoFrame);
    y2axisText->setFixedWidth(fm.width("Y2 axis"));
    y2axisText->setPalette(transparent_base);

    xaxisText = new QLineEdit("X axis");
    xaxisText->setReadOnly(true);
    xaxisText->setFrame(QFrame::NoFrame);
    xaxisText->setFixedWidth(fm.width("Y2 axis"));
    xaxisText->setPalette(transparent_base);



    y1ComboBox = new QComboBox();
    y1ComboBox->setFixedHeight(fm.height());
    y1ComboBox->addItem("");
    connect(y1ComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(on_yaxis1ComboBox_currentTextChanged(QString)));

    y2ComboBox = new QComboBox();
    y2ComboBox->setFixedHeight(fm.height());
    y2ComboBox->addItem("");
    connect(y2ComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(on_yaxis2ComboBox_currentTextChanged(QString)));

    xComboBox = new QComboBox();
    xComboBox->setFixedHeight(fm.height());
    //xComboBox->setMaximumWidth(150);
    xComboBox->addItem("");
    connect(xComboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(on_xaxisComboBox_currentTextChanged(QString)));



    QGridLayout* layout = new QGridLayout(this);


    layout->addWidget(xaxisText,0,0,1,1);
    layout->addWidget(xaxisText,0,0,1,1);
    layout->addWidget(xComboBox,0,1,1,1);
    layout->addWidget(globalScaleXCheckBox,0,2,1,1);

    layout->addWidget(y1axisText,1,0,1,1);
    layout->addWidget(y1ComboBox,1,1,1,1);
    layout->addWidget(globalScaleY1CheckBox,1,2,1,1);
    layout->addWidget(legend1,2,0,1,3);

    layout->addWidget(y2axisText,6,0,1,1);
    layout->addWidget(y2ComboBox,6,1,1,1);
    layout->addWidget(globalScaleY2CheckBox,6,2,1,1);
    layout->addWidget(legend2,7,0,1,3);


    this->setLayout(layout);
}


/******************************************************************************
 *                               PUBLIC                                       *
 *****************************************************************************/

void PlotLegend::setButtonGroups(QButtonGroup *scale_buttons1, QButtonGroup *scale_buttons2, QButtonGroup *track_buttons1, QButtonGroup *track_buttons2)
{
    this->scale_buttons1 = scale_buttons1;
    this->scale_buttons2 = scale_buttons2;
    this->track_buttons1 = track_buttons1;
    this->track_buttons2 = track_buttons2;
}

int PlotLegend::graphCount() const
{
   return graph_names.count();
}

void PlotLegend::addGraph(QString name, QColor color, QString x_axis_name, QString y_axis_name)
{
    graph_names.append(name);
    graph_styles.append(new PlotLegendLineDisplay(color));

    if(!y_axis.contains(y_axis_name) && xComboBox->currentText() == x_axis_name)
    {
        y1ComboBox->addItem(y_axis_name);
        y2ComboBox->addItem(y_axis_name);
    }

    y_axis.append(y_axis_name);

    if(!x_axis.contains(x_axis_name))
    {
        xComboBox->addItem(x_axis_name);
    }

    x_axis.append(x_axis_name);

    on_yaxis1ComboBox_currentTextChanged(y1ComboBox->currentText());
    on_yaxis2ComboBox_currentTextChanged(y2ComboBox->currentText());
}

void PlotLegend::populateYComboBoxes(QString x_axis_name)
{
    clearYComboBoxes();

    QList<QString> added;

    for(int i=0; i<y_axis.count(); i++)
    {
        if(x_axis_name == x_axis.at(i) && !added.contains(y_axis.at(i)))
        {
            y1ComboBox->addItem(y_axis.at(i));
            y2ComboBox->addItem(y_axis.at(i));
            added.append(y_axis.at(i));
        }
    }

    sortComboBox(y1ComboBox);
    sortComboBox(y2ComboBox);

    on_yaxis1ComboBox_currentTextChanged(y1ComboBox->currentText());
    on_yaxis2ComboBox_currentTextChanged(y2ComboBox->currentText());
}

void PlotLegend::clearYComboBoxes()
{
    y1ComboBox->setCurrentIndex(0);
    y2ComboBox->setCurrentIndex(0);

    y1ComboBox->clear();
    y2ComboBox->clear();

    y1ComboBox->addItem("");
    y2ComboBox->addItem("");

    old_yaxis1 = "";
    old_yaxis2 = "";

}

void PlotLegend::removeGraph(QString name)
{
    int index = graph_names.indexOf(name);
    graph_names.removeAt(index);
    delete graph_styles.at(index);
    graph_styles.removeAt(index);

    QString xaxis = x_axis.at(index);
    x_axis.removeAt(index);

    if(!x_axis.contains(xaxis))
    {
        xComboBox->removeItem(comboBoxIndex(xComboBox,xaxis));
    }

    QString yaxis = y_axis.at(index);
    y_axis.removeAt(index);

    if(!y_axis.contains(yaxis))
    {
        int combo_index = comboBoxIndex(y1ComboBox,yaxis);
        if(combo_index >= 0)
        {
            if(y1ComboBox->currentText() == yaxis)
            {
                old_yaxis1 = "";
                y1ComboBox->setCurrentIndex(0);
            }

            y1ComboBox->removeItem(combo_index);

        }

        combo_index = comboBoxIndex(y2ComboBox,yaxis);
        if(combo_index >= 0)
        {
            if(y2ComboBox->currentText() == yaxis)
            {
                old_yaxis2 = "";
                y2ComboBox->setCurrentIndex(0);
            }

            y2ComboBox->removeItem(combo_index);
        }
    }

    if(legend_buttons1.contains(name))
    {
        int id = legend_buttons1.value(name);
        removeFromButtonGroup(scale_buttons1,id);
        removeFromButtonGroup(visible_buttons1,id);
        removeFromButtonGroup(track_buttons1,id);
    }

    if(legend_buttons2.contains(name))
    {
        int id = legend_buttons2.value(name);
        removeFromButtonGroup(scale_buttons2,id);
        removeFromButtonGroup(visible_buttons2,id);
        removeFromButtonGroup(track_buttons2,id);
    }

    // Refresh
    on_yaxis1ComboBox_currentTextChanged(y1ComboBox->currentText());
    on_yaxis2ComboBox_currentTextChanged(y2ComboBox->currentText());

}

int PlotLegend::comboBoxIndex(const QComboBox *box, const QString name) const
{
    for(int i=0; i<box->count(); i++)
    {
        if(!box->itemText(i).compare(name))
            return i;
    }

    return -1;
}

int PlotLegend::getTableRow(int table_num, QString graph_name) const
{
        return getTableRow(getTable(table_num),graph_name);
}

void PlotLegend::changeBoldLegendRow(int row, int table_num)
{
        changeBoldLegendRow(row,getTable(table_num));
}


/******************************************************************************
 *                               PRIVATE                                      *
 *****************************************************************************/

void PlotLegend::removeFromButtonGroup(QButtonGroup *group, int id)
{
    // Remove the button

    group->removeButton(group->button(id));

    QList<QAbstractButton*> buttons = group->buttons();

    int btn_id;
    for(int i = 0; i < buttons.count(); i++)
    {
        btn_id = group->id(buttons.at(i));
        if( btn_id > id) // Need to decrement id
        {
            group->setId(buttons.at(i),btn_id - 1);
        }
    }
}

int PlotLegend::tableIndex(const QTableWidget* legend, const QString graph_name) const
{
    QString name;
    qDebug() << QString::number(legend->rowCount());
    for(int i = 0; i < legend->rowCount(); i++)
    {
        legend->item(i,0);
        name = ((QLineEdit*) legend->cellWidget(i,0))->text();
        qDebug() << name;
        if(name.compare(graph_name) == 0)
        {
            return i;
        }
    }

    return -1;
}

void PlotLegend::clearButtonGroup(QButtonGroup *group)
{
    QAbstractButton* button;
    for(int i=0; i<group->buttons().size(); i++)
    {
        button = group->buttons().at(i);
        group->removeButton(button);
        delete button;
    }
}

// Will enable <buttons> only if <conditon_buttons> are checked
void PlotLegend::enableButtons(QList<QAbstractButton *> buttons, bool enabled,const QList<QAbstractButton*> condition_buttons)
{
    for(int i=0; i<buttons.size(); i++)
    {
        if(condition_buttons.at(i)->isChecked())
            buttons.at(i)->setEnabled(enabled);
    }
}

void PlotLegend::clearLegend(QTableWidget *table)
{


    QButtonGroup* track_buttons;
    QButtonGroup* scale_buttons;
    QButtonGroup* visible_buttons;

    if(table == legend1)
    {
        track_buttons = track_buttons1;
        scale_buttons = scale_buttons1;
        visible_buttons = visible_buttons1;
        legend_buttons1.clear();
    }
    else if(table == legend2)
    {
        track_buttons = track_buttons2;
        scale_buttons = scale_buttons2;
        visible_buttons = visible_buttons2;
        legend_buttons2.clear();
    }
    else
    {
        throw std::runtime_error("Unknown graph table");
        return;
    }

    // Save copy of plotlegendlinedisplay, because fucking QTableWidget takes ownership of child widgets and deletes them
    for(int i = 0; i<table->rowCount(); i++)
    {
        PlotLegendLineDisplay* line = (PlotLegendLineDisplay*) table->cellWidget(i,4);
        for(int j = 0; j < graph_styles.count(); j++)
        {
            if(graph_styles.at(j) == line)
            {
                graph_styles.replace(j,line->copy());
                break;
            }
        }
    }

    table->setRowCount(0);
    table->clear();



    if(!track_buttons->buttons().isEmpty())
    {
        disconnect(track_buttons,SIGNAL(buttonToggled(int,bool)),this,SLOT(trackButton_toggled(int,bool)));
        disconnect(visible_buttons,SIGNAL(buttonToggled(int,bool)),this,SLOT(visibleButton_toggled(int,bool)));
        clearButtonGroup(track_buttons);
        clearButtonGroup(visible_buttons);
        clearButtonGroup(scale_buttons);

    }

}

void PlotLegend::populateLegend(QTableWidget* table, QString axis)
{
    QButtonGroup* track_buttons;
    QButtonGroup* scale_buttons;
    QButtonGroup* visible_buttons;
    QMap<QString,int>* legend_buttons;

    int width = 20;
    int height = 0;

    QTableWidgetItem* item;
    QPalette transparent_base;
    transparent_base.setColor(QPalette::Base,QColor(0, 0, 0, 0));

    if(table == legend1)
    {
        track_buttons = track_buttons1;
        scale_buttons = scale_buttons1;
        visible_buttons = visible_buttons1;
        legend_buttons = &legend_buttons1;
    }
    else if(table == legend2)
    {
        track_buttons = track_buttons2;
        scale_buttons = scale_buttons2;
        visible_buttons = visible_buttons2;
        legend_buttons = &legend_buttons2;
    }
    else
    {
        throw std::runtime_error("Unknown graph table");
        return;
    }

    clearLegend(table);

    table->setColumnCount(5);

    int k = 0;
    for(int i=0; i < graphCount(); i++)
    {
        if(!y_axis.at(i).compare(axis))
        {
            table->setRowCount(k+1);
            QString name = graph_names.at(i);



            item = new QTableWidgetItem();

            QLineEdit* description = new QLineEdit(name);
            description->setFrame(false);
            description->setReadOnly(true);
            description->setPalette(transparent_base);

            /*
            QFrame* line = new QFrame();
            line->setFrameShape(QFrame::HLine);
            line->setPalette(palette);
            */

            PlotLegendLineDisplay* line = graph_styles.at(i);
            table->setCellWidget(k,4,(QWidget*) line);
            table->setItem(k,4,item);
            table->setColumnWidth(4,20);

            QCheckBox* visibleButton = new QCheckBox("Visible");
            visibleButton->setChecked(true);
            emit setGraphVisible(i, true);
            visible_buttons->addButton(visibleButton,i);
            item = new QTableWidgetItem();
            table->setCellWidget(k,3,visibleButton);
            table->setItem(k,3,item);


            QCheckBox* scaleButton = new QCheckBox("Autoscale y");
            scale_buttons->addButton(scaleButton,i);
            scaleButton->setChecked(true);
            item = new QTableWidgetItem();
            table->setCellWidget(k,2,scaleButton);
            table->setItem(k,2,item);

            QCheckBox* trackButton = new QCheckBox("Track");
            track_buttons->addButton(trackButton,i);
            if(k == 0) // Only track first graph
                trackButton->setChecked(true);
            item = new QTableWidgetItem();
            table->setCellWidget(k,1,trackButton);
            table->setItem(k,1,item);

            item = new QTableWidgetItem();
            table->setCellWidget(k,0,description);
            table->setItem(k,0,item);

            height += table->rowHeight(k);

            k++;

            legend_buttons->insert(name,i);
        }

    }

    for(int i=1; i < 4; i++)
    {
        table->resizeColumnToContents(i);
        table->setColumnWidth(i,table->columnWidth(i)+4);
        width += table->columnWidth(i);
    }

    if(height > table->height())
    {

       table->setColumnWidth(0,table->width()-width-22);
    }
    else
    {
       table->setColumnWidth(0,table->width()-width);
    }

    if(!track_buttons->buttons().isEmpty())
    {
        connect(track_buttons,SIGNAL(buttonToggled(int,bool)),this,SLOT(trackButton_toggled(int,bool)));
        connect(visible_buttons,SIGNAL(buttonToggled(int,bool)),this,SLOT(visibleButton_toggled(int,bool)));
    }

}

void PlotLegend::clearBoldLegend()
{
    resetBoldLegendRow(bold_legend_row,legend1);
    resetBoldLegendRow(bold_legend_row2,legend2);
    bold_legend_row = -1;
    bold_legend_row2 = -1;
}

void PlotLegend::changeBoldLegendRow(int row, QTableWidget* graphTableWidget)
{
    clearBoldLegend();

    for(int i=0; i<graphTableWidget->columnCount(); i++)
    {
        graphTableWidget->cellWidget(row,i)->setStyleSheet("font-weight: bold; background-color: rgba(255, 255, 255, 0)");
    }

    if(graphTableWidget == legend1)
        bold_legend_row = row;
    else
        bold_legend_row2 = row;
}

void PlotLegend::resetBoldLegendRow(int row, QTableWidget* graphTableWidget)
{
    if(row < 0) // No row is bold
        return;

    for(int i=0; i<graphTableWidget->columnCount(); i++)
    {
        graphTableWidget->cellWidget(row,i)->setStyleSheet("font-weight: normal;background-color: rgba(255, 255, 255, 0)");
    }
}

int PlotLegend::getTableRow(const QTableWidget* table, const QString graph_name) const
{
    for(int i=0; i<table->rowCount(); i++)
    {
        if(!((QLineEdit*) table->cellWidget(i,0))->text().compare(graph_name))
            return i;
    }

    return -1;
}

int PlotLegend::getGraphIndex(const QTableWidget *table, int table_row) const
{
    QString graph_name = ((QLineEdit*) table->cellWidget(table_row,0))->text();

    for(int i=0; i<graphCount(); i++)
    {
        if(!graph_names.at(i).compare(graph_name))
            return i;
    }

    return -1;
}

QTableWidget* PlotLegend::getTable(int legend_num) const
{
    QTableWidget* table;
    if(legend_num == 1)
    {
        table = legend1;
    }
    else if(legend_num == 2)
    {
        table = legend2;
    }
    else
    {
        throw std::runtime_error("No such legend");
    }

    return table;
}

void PlotLegend::updateVisibility()
{
    QList<int> graphs;
    int index;

    for(int i=0; i < legend1->rowCount(); i++)
    {
        index = getGraphIndex(legend1, i);
        if(!graphs.contains(index))
        {
            graphs.append(index);
        }
    }

    for(int i=0; i < legend2->rowCount(); i++)
    {
        index = getGraphIndex(legend2, i);
        if(!graphs.contains(index))
        {
            graphs.append(index);
        }
    }

    for(int i=0; i<graphCount(); i++)
    {
        if(graphs.contains(i))
        {
            if(visible_buttons1->button(i) != 0 && visible_buttons1->button(i)->isChecked())
            {
                if(visible_buttons2->button(i) != 0 && !visible_buttons2->button(i)->isChecked())
                {
                    emit setGraphVisible(i, true);
                    visible_buttons1->button(i)->setChecked(false);
                }
                else
                {
                  emit setGraphVisible(i, true);
                }

            }
            else if(visible_buttons2->button(i) != 0 && visible_buttons2->button(i)->isChecked())
            {
                if(visible_buttons1->button(i) != 0 && !visible_buttons1->button(i)->isChecked())
                {
                    emit setGraphVisible(i, false);
                    visible_buttons1->button(i)->setChecked(false);
                }
                else
                {
                  emit setGraphVisible(i, true);
                }
            }


        }
        else
        {
           emit setGraphVisible(i, false);


           if(visible_buttons1->button(i) != 0)
               visible_buttons1->button(i)->setChecked(false);

           if(visible_buttons2->button(i) != 0)
               visible_buttons2->button(i)->setChecked(false);
        }
    }

}

/******************************************************************************
 *                          UI EVENT HANDLERS                                 *
 *****************************************************************************/
void PlotLegend::graphTableWidgetContextMenuRequested(QPoint point)
{
    if(legend1->indexAt(point).column() == 4) // Only display menu if leftmost column (the line column) is clicked
    {
        line_menu->start(1,legend1->indexAt(point).row(),legend1->mapToGlobal(point));
    }
}

void PlotLegend::graphTableWidgetContextMenuRequested2(QPoint point)
{
    if(legend2->indexAt(point).column() == 4) // Only display menu if leftmost column (the line column) is clicked
    {
        line_menu->start(2,legend2->indexAt(point).row(),legend2->mapToGlobal(point));
    }
}

void PlotLegend::changeColor(int legend, int row, QColor color)
{
    QTableWidget* table = getTable(legend);
    ((PlotLegendLineDisplay*) table->cellWidget(row,4))->setColor(color);
    emit changeGraphColor(getGraphIndex(table,row),color);
}

void PlotLegend::changeLineStyle(int legend, int row, Qt::PenStyle style)
{
    QTableWidget* table = getTable(legend);
    ((PlotLegendLineDisplay*) table->cellWidget(row,4))->setLineStyle(style);
    emit changeGraphLineStyle(getGraphIndex(table,row),style);
}

void PlotLegend::changeScatterStyle(int legend, int row, QCPScatterStyle style)
{
    QTableWidget* table = getTable(legend);
    ((PlotLegendLineDisplay*) table->cellWidget(row,4))->setScatterStyle(style.shape());
    emit changeGraphScatterStyle(getGraphIndex(table,row),style);
}

void PlotLegend::trackButton_toggled(int, bool)
{
    if(track_buttons1->checkedId() != -1 || track_buttons2->checkedId() != -1) // At least one button is checked
    {
        emit setRangeDrag(false);
    }
    else
    {
        emit setRangeDrag(true);
    }
}

void PlotLegend::on_autoscaleXGlobalBox_clicked(bool checked)
{
    emit setAutoScaleX(checked);

    enableButtons(track_buttons1->buttons(),!checked,visible_buttons1->buttons());
    enableButtons(track_buttons2->buttons(),!checked,visible_buttons2->buttons());
}

void PlotLegend::on_graphTableWidget_clicked(const QModelIndex &index)
{
    graphTableWidget_clicked(index.row(),legend1,bold_legend_row);
}

void PlotLegend::on_graphTableWidget_2_clicked(const QModelIndex &index)
{
    graphTableWidget_clicked(index.row(),legend2,bold_legend_row2);
}

void PlotLegend::graphTableWidget_clicked(int row,QTableWidget* table,int& bold_legend_row)
{
    int graph_index = getGraphIndex(table,row);

    if(bold_legend_row == row)
    {
        emit setGraphSelection(graph_index,false);
        clearBoldLegend();
        return;
    }

    changeBoldLegendRow(row,table);

    for(int i=0; i<graphCount(); i++)
    {
        if(i == graph_index)
            emit setGraphSelection(i,true);
        else
            emit setGraphSelection(i,false);
    }
}



void PlotLegend::on_autoscaleYGlobalBox_clicked(bool checked)
{
    emit setAutoScaleYGlobal(checked,1);

    enableButtons(scale_buttons1->buttons(),!checked,visible_buttons1->buttons());
}

void PlotLegend::on_autoscaleY2GlobalBox_clicked(bool checked)
{
    emit setAutoScaleYGlobal(checked,2);

    enableButtons(scale_buttons2->buttons(),!checked,visible_buttons2->buttons());
}

void PlotLegend::visibleButton_toggled(int btn_num, bool checked)
{
    emit setGraphVisible(btn_num, checked);

    if(!autoscale_x_global)
    {
        if(track_buttons1->button(btn_num))
        {
            track_buttons1->button(btn_num)->setEnabled(checked);
            visible_buttons1->button(btn_num)->setChecked(checked);
        }


        if(track_buttons2->button(btn_num))
        {
            track_buttons2->button(btn_num)->setEnabled(checked);
            visible_buttons2->button(btn_num)->setChecked(checked);
        }
    }
    if(!autoscale_y_global)
    {
       if(scale_buttons1->button(btn_num))
       {
            scale_buttons1->button(btn_num)->setEnabled(checked);
            visible_buttons1->button(btn_num)->setChecked(checked);
       }

       if(scale_buttons2->button(btn_num))
       {
            scale_buttons2->button(btn_num)->setEnabled(checked);
            visible_buttons2->button(btn_num)->setChecked(checked);
       }
    }
}

void PlotLegend::on_xaxisComboBox_currentTextChanged(const QString &arg1)
{
    emit setXLabel(arg1);
    populateYComboBoxes(arg1);

    if( (!arg1.compare("Time")) || (!arg1.compare("time")) || (!arg1.compare("t")))
    {
        emit setTimeAxis(true);
    }
    else
    {
        emit setTimeAxis(false);
    }
}

void PlotLegend::on_yaxis1ComboBox_currentTextChanged(const QString &arg1)
{
    yaxisComboBox_currentTextChanged(arg1,y2ComboBox,legend1,1,old_yaxis1);
}

void PlotLegend::on_yaxis2ComboBox_currentTextChanged(const QString &arg1)
{
    yaxisComboBox_currentTextChanged(arg1,y1ComboBox,legend2,2,old_yaxis2);
}

void PlotLegend::yaxisComboBox_currentTextChanged(const QString arg1,QComboBox* combo_box,QTableWidget* legend, int axis, QString& old_axis)
{
    //qDebug() << "Axis " + QString::number(axis) + " " + old_axis;

    int graph_index;

    // If not empty selection
    if(arg1.compare(""))
    {
        // Remove item from other combo box (same item should not be selectable in both)
        for(int i=0; i<combo_box->count(); i++)
        {
            if(!arg1.compare(combo_box->itemText(i)))
            {
                combo_box->blockSignals(true);
                combo_box->removeItem(i);
                combo_box->blockSignals(false);
                break;
            }
        }

        populateLegend(legend,arg1);

        for(int i=0; i<legend->rowCount(); i++)
        {
            graph_index = getGraphIndex(legend,i);
            emit setGraphValueAxis(graph_index, axis);
        }

        emit setYLabel(arg1,axis);
        emit setYAxisVisible(true,axis);
    }
    else
    {
        clearLegend(legend);

        emit setYAxisVisible(false,axis);
    }

   updateVisibility();

    // Add the previously removed item to the other combo box
    if(old_axis.compare(""))
    {
        combo_box->blockSignals(true);
        combo_box->addItem(old_axis);
        combo_box->blockSignals(false);

    }

    old_axis = arg1;

    sortComboBox(combo_box);
}

void PlotLegend::sortComboBox(QComboBox *box)
{
    box->blockSignals(true);

    QStringList list;
    for(int i=0; i<box->count(); i++)
    {
        list.append(box->itemText(i));
    }

    list.sort();
    QString selected = box->currentText();
    box->clear();

    for(int i=0; i<list.count(); i++)
    {
        box->addItem(list.at(i));
        if(!list.at(i).compare(selected))
            box->setCurrentIndex(i);
    }

    box->blockSignals(false);
}
