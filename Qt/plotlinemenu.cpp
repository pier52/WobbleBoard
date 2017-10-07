#include "plotlinemenu.h"

PlotLineMenu::PlotLineMenu()
{
    color_menu = this->addMenu("Color");
    QMapIterator<QString, QColor> i(COLORS);
    while(i.hasNext())
    {
        i.next();
        color_menu->addAction(i.key());
    }

    line_menu = this->addMenu("Line style");
    QMapIterator<QString, Qt::PenStyle> j(LINE_STYLES);
    while(j.hasNext())
    {
        j.next();
        line_menu->addAction(j.key());
    }

    scatter_menu = this->addMenu("Scatter style");
    QMapIterator<QString, QCPScatterStyle> k(SCATTER_STYLES);
    while(k.hasNext())
    {
        k.next();
        scatter_menu->addAction(k.key());
    }

    connect(line_menu,SIGNAL(triggered(QAction*)),this,SLOT(line_menu_triggered(QAction*)));
    connect(color_menu,SIGNAL(triggered(QAction*)),this,SLOT(color_menu_triggered(QAction*)));
    connect(scatter_menu,SIGNAL(triggered(QAction*)),this,SLOT(scatter_menu_triggered(QAction*)));
}

PlotLineMenu::~PlotLineMenu()
{
    delete color_menu;
    delete line_menu;
}

void PlotLineMenu::color_menu_triggered(QAction *action)
{
    emit colorChange(current_legend,current_row,COLORS.value(action->text()));
}

void PlotLineMenu::line_menu_triggered(QAction *action)
{
    emit lineChange(current_legend,current_row,LINE_STYLES.value(action->text()));
}

void PlotLineMenu::scatter_menu_triggered(QAction *action)
{
    emit scatterChange(current_legend,current_row,SCATTER_STYLES.value(action->text()));
}

void PlotLineMenu::start(int legend, int row, QPoint pos)
{
    current_legend = legend;
    current_row = row;

    this->exec(pos);
}
