#include "mapwindow.h"
#include "ui_mapwindow.h"
#include <QtGlobal>

MapWindow::MapWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MapWindow)
{
    ui->setupUi(this);

    connect(this,SIGNAL(debug_step_forward(int,double,double,double)),ui->widget,SLOT(debug_step(int,double,double,double)));
    connect(ui->widget,SIGNAL(pointInfo(QString)),this,SLOT(newPointInfo(QString)));
}

MapWindow::~MapWindow()
{
    delete ui;
}


void MapWindow::debug_step(int step_length)
{
    if(step_length == 0)
        return;
    double uncertainty = (rand() % 100)/100.0;
    old_uncertainty = uncertainty;
    emit debug_step_forward(step_length,0,0,uncertainty);
}

void MapWindow::debug_step_angle(double orientation)
{
    emit debug_step_forward(0,orientation,0,old_uncertainty);
}

void MapWindow::on_track_checkBox_clicked(bool checked)
{
    ui->widget->track(checked);
}

void MapWindow::on_clear_Button_clicked()
{
    ui->widget->clear();
}

void MapWindow::on_centerButton_clicked()
{
    ui->widget->centerOnCurrentPos();
}

void MapWindow::on_store_apply_Button_clicked()
{
    ui->widget->samplesToKeep(ui->to_store_lineEdit->text().toInt());
}

void MapWindow::on_action_Clear_triggered()
{
    ui->widget->savePdf();
}

void MapWindow::newPointInfo(QString info)
{
    ui->pointInfoEdit->setPlainText(info);
}
