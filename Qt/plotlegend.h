/**
 * \class PlotLegend
 *
 *
 * \brief
 *      Custom widget which creates a legend for the PlotWindow.
 *
 * Connected to the plotwindow, and is where the user decides which axes to show and which plots to display,
 * track and scale the graph window for, \n
 * Graphs corresponding to the selected axes are shown in a table, where attributes
 * (such as track, auto scale y-axis etc.) for each separate graph can be selected \n
 * See PlotWindow class documentation for mor info.
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef PLOTLEGEND_H
#define PLOTLEGEND_H

#include <QWidget>
#include <QTableWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include "qcustomplot.h"
#include "plotlinemenu.h"
#include "plotlegendlinedisplay.h"

class PlotLegend : public QWidget
{
    Q_OBJECT
public:
    explicit PlotLegend(QWidget *parent = 0);
    ~PlotLegend();
    void addGraph(QString name, QColor color, QString x_axis_name, QString y_axis_name);
    void removeGraph(QString name);
    void setButtonGroups(QButtonGroup* scale_buttons1,QButtonGroup* scale_buttons2,QButtonGroup* track_buttons1,QButtonGroup* track_buttons2);

    /// Resets the bold row in the legend to normal font
    void clearBoldLegend();

    /**
     * @brief
     * Changes which row in the legend is bold
     * @param row
     * The row to be bold
     * @param table_num
     * Which of the two tables (1 or 2, belonging to Y-axis 1 or 2)
     */
    void changeBoldLegendRow(int row,int table_num);

    /**
     * @brief
     * Gets the table row number of a graph in one of the 2 graph tables
     * @param table_num
     * The table number, 1 for Y1 axis 2 for Y2 axis
     * @param graph_name
     * The name of the graph
     * @return
     * The table row
     */
    int getTableRow(int table_num, QString graph_name) const;

signals:
    void setRangeDrag(bool);
    void setAutoScaleX(bool);
    void setGraphSelection(int,bool);
    void setAutoScaleYGlobal(bool,int);
    void setGraphVisible(int, bool);
    void setXLabel(QString);
    void setYLabel(QString,int);
    void setYAxisVisible(bool,int);
    void setGraphValueAxis(int, int);
    void setTimeAxis(bool);
    void changeGraphLineStyle(int graph_num, Qt::PenStyle line_style);
    void changeGraphScatterStyle(int graph_num, QCPScatterStyle scatter_style);
    void changeGraphColor(int graph_num, QColor color);

private slots:

    void on_autoscaleXGlobalBox_clicked(bool checked);

    void trackButton_toggled(int,bool);

    void visibleButton_toggled(int,bool);

    void on_graphTableWidget_clicked(const QModelIndex &index);

    void on_autoscaleYGlobalBox_clicked(bool checked);

    void on_autoscaleY2GlobalBox_clicked(bool checked);

    void on_xaxisComboBox_currentTextChanged(const QString &arg1);

    void on_yaxis1ComboBox_currentTextChanged(const QString &arg1);

    void on_yaxis2ComboBox_currentTextChanged(const QString &arg1);

    void on_graphTableWidget_2_clicked(const QModelIndex &index);

    void graphTableWidgetContextMenuRequested(QPoint);

    void graphTableWidgetContextMenuRequested2(QPoint);

    void changeLineStyle(int legend,int row,Qt::PenStyle style);

    void changeColor(int legend, int row, QColor color);

    void changeScatterStyle(int legend, int row, QCPScatterStyle style);

public slots:

private:
    /// Initializes the layout, i.e. places all child widgets etc.
    void initLayout();

    /// Convinience function to get the number of graphs
    int graphCount() const;

    void populateLegend(QTableWidget* table, QString axis);

    /**
     * @brief enableButtons
     * Enables/disables button in a list of buttons
     * @param buttons
     * The list of buttons
     * @param enabled
     * Whether to enable(=true) or disable(=false)
     * @param condition_buttons
     * The buttons will only be disabled/enabled if the corresponding button in <condition_buttons> is enabled
     */
    void enableButtons(QList<QAbstractButton*> buttons, bool enabled,const QList<QAbstractButton*> condition_buttons);

    /// Clears a QButtonGroup (because apparently that is not a built in fucntion...)
    void clearButtonGroup(QButtonGroup* group);

    int getTableRow(const QTableWidget* table, const QString graph_name) const;
    int getGraphIndex(const QTableWidget* table, int table_index) const;

    /**
     * @brief
     * Gets the table (legend) corresponding to Y1 or Y2 axis
     * @param legend_num
     * 1 for Y1 axis or 2 for Y2 axis
     * @return
     * Pointer to the table
     */
    QTableWidget* getTable(int legend_num) const;

    /**
     * @brief graphTableWidget_clicked
     * Helper to the corresponding slots
     * @param row
     * The row that is clicked
     * @param table
     * Which table the row belongs to
     * @param bold_legend_row
     * Reference to the variable which holds which row currently is bold
     */
    void graphTableWidget_clicked(int row,QTableWidget* table,int& bold_legend_row);

    /// Clears a legend. It's table, all its contents and its corresponding QButtonGroup gets cleared.
    /// Note that clearing a QTableWidget means that all members are destructed.
    void clearLegend(QTableWidget* table);

    /// Alphabetically sorts the items in a QComboBox ascendenly
    void sortComboBox(QComboBox* box);

    /// Gets the index of an item with text <name> in QComboBox <box>
    int comboBoxIndex(const QComboBox* box, const QString name) const;

    /// Populates the YComboBoxes with the graphs which has X-axis <x_axis_name>
    void populateYComboBoxes(QString x_axis_name);

    void clearYComboBoxes();

    int tableIndex(const QTableWidget* legend, const QString graph_name) const;

    /// Removes button with <id> from <group>. Because apparently that is not a built in function...
    void removeFromButtonGroup(QButtonGroup* group,int id);

    void changeBoldLegendRow(int row, QTableWidget* graphTableWidget);

    /// Sets the specified row in a legend to normal (non-bold) font
    void resetBoldLegendRow(int row, QTableWidget* graphTableWidget);

    /**
     * @brief
     * Helper to the 2 corresponding slots. Displays the graphs with yaxis <text> in the corresponding legend, and removes
     * that yAxis from the other yaxiscombobox
     * @param text
     * What the combo box text changed to
     * @param combo_box
     * The other yaxisComboBox
     * (if this function is called from yaxiscombobox1, this should be a reference to yaxiscombobox2 and vide versa)
     * @param legend
     * Which table (1 or 2) this is called from
     * @param axis
     * Which axos (1 or 2) the combobox corresponds to
     * @param old_axis
     * Reference to the name of the old axis, before the user change. Updated in this function.
     */
    void yaxisComboBox_currentTextChanged(const QString text,QComboBox* combo_box,QTableWidget* legend, int axis,QString& old_axis);

    /// Updates the visibility accordning to the settings in visibile_buttons# QButtonGroup
    void updateVisibility();

    QList<QString> y_axis;
    QList<QString> x_axis;

    int bold_legend_row;
    int bold_legend_row2;

    QButtonGroup* track_buttons1;
    QButtonGroup* scale_buttons1;
    QButtonGroup* visible_buttons1;

    QButtonGroup* track_buttons2;
    QButtonGroup* scale_buttons2;
    QButtonGroup* visible_buttons2;

    QMap<QString,int> legend_buttons1;
    QMap<QString,int> legend_buttons2;

    bool autoscale_y_global = false;
    bool autoscale_x_global = false;

    QString old_yaxis1;
    QString old_yaxis2;

    QList<QString> graph_names;
    QList<PlotLegendLineDisplay*> graph_styles;

    QTableWidget* legend1;
    QTableWidget* legend2;

    QCheckBox* globalScaleY1CheckBox;
    QComboBox* y1ComboBox;

    QCheckBox* globalScaleY2CheckBox;
    QComboBox* y2ComboBox;

    QLineEdit* y1axisText;
    QLineEdit* y2axisText;

    QCheckBox* globalScaleXCheckBox;
    QComboBox* xComboBox;

    QLineEdit* xaxisText;

    PlotLineMenu* line_menu;

};

#endif // PLOTLEGEND_H
