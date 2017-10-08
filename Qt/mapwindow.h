#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"

namespace Ui {
class MapWindow;
}

class MapWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapWindow(QWidget *parent = 0);
    ~MapWindow();

public slots:
    void debug_step(int step_length);
    void debug_step_angle(double orientation_change);

private slots:

    void on_track_checkBox_clicked(bool checked);

    void on_clear_Button_clicked();

    void on_centerButton_clicked();

    void on_store_apply_Button_clicked();

    void on_action_Clear_triggered();

    void newPointInfo(QString info);

signals:
     void debug_step_forward(int step_length,double orientation_change,double camera_orientation, double uncertainty);

private:

    Ui::MapWindow *ui;

    double old_uncertainty;
};

#endif // MAPWINDOW_H
