/**
 * \class PlotLineMenu
 *
 *
 * \brief
 *      Help class for PlotLegend. Custom menu, which provides options for how data is visualised in a graph.
 *
 * Provides a menu with options to choose color, line style and scatter style for a QCP plot. Notifies of selection via signals.
 *
 * \author Samuel Orn
 *
 */

#ifndef PLOTLINEMENU_H
#define PLOTLINEMENU_H

#include <QMenu>
#include "qcustomplot.h"

class PlotLineMenu : public QMenu
{
    Q_OBJECT
public:
    PlotLineMenu();
    ~PlotLineMenu();

    /**
     * @brief
     * Opens the menu
     * @param legend
     * Number of which legend in PlotLegend it was called from (1 or 2)
     * @param row
     * The row number in the legend in PlotLegend it was called from
     * @param pos
     * The position where to display the menu
     */
    void start(int legend, int row, QPoint pos);

signals:
    void colorChange(int legend,int row, QColor color);
    void lineChange(int legend,int row, Qt::PenStyle line);
    void scatterChange(int legend,int row, QCPScatterStyle scatter);

private slots:
    void line_menu_triggered(QAction*);
    void color_menu_triggered(QAction*);
    void scatter_menu_triggered(QAction*);

private:
    int current_legend;
    int current_row;
    QMenu* color_menu;
    QMenu* line_menu;
    QMenu* scatter_menu;


    const QMap<QString,QColor> COLORS{{"Red", Qt::red}, {"Blue", Qt::blue}, {"Green", Qt::green},
                             {"Black", Qt::black}, {"Magenta", Qt::magenta}, {"Gray", Qt::gray}, {"Cyan", Qt::cyan}};

    const QMap<QString,Qt::PenStyle> LINE_STYLES{{"None", Qt::NoPen},{"Line",Qt::SolidLine},{"Dotted line", Qt::DotLine}, {"Dashed line", Qt::DashLine},
                                        {"Dotted line", Qt::DotLine}, {"Dash dot line", Qt::DashDotLine}, {"Dash dot dot line", Qt::DashDotDotLine}};

    const QMap<QString,QCPScatterStyle> SCATTER_STYLES{{"None", QCPScatterStyle::ssNone},{"Circle", QCPScatterStyle::ssCircle},{"Cross", QCPScatterStyle::ssCross},
                                           {"Diamond", QCPScatterStyle::ssDiamond}, {"Plus", QCPScatterStyle::ssPlus},{"Square", QCPScatterStyle::ssSquare},
                                           {"Star", QCPScatterStyle::ssStar},{"Triangle", QCPScatterStyle::ssTriangle},{"Filled circle", QCPScatterStyle::ssDisc}};

};

#endif // PLOTLINEMENU_H
