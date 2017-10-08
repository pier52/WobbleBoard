#ifndef POSITIONWIDGET_H
#define POSITIONWIDGET_H

#include <QWidget>
#include <QDateTime>
#include <QMouseEvent>
#include <QWheelEvent>
#include <limits>
#include <QMutex>

namespace Ui {
class PositionWidget;
}

class PositionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PositionWidget(QWidget *parent = 0);
    ~PositionWidget();

public slots:
    void addPos(double x, double y, double vehicle_orientation,double camera_orientation, double uncertainty);
    void debug_step(int step_length,double orientation_change,double camera_orientation, double uncertainty);
    void track(bool track);
    void clear();
    void lengthToKeep(unsigned seconds);
    void samplesToKeep(int num_samples);
    void centerOnCurrentPos();
    void savePdf();

signals:
    void pointInfo(QString);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* event)Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent* event)Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event)Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;
private:
    Ui::PositionWidget *ui;
    struct PositionStruct {
        double x_meter;
        double y_meter;
        int x_pixel;
        int y_pixel;
        double uncertainty;
        double vehicle_orientation;
        double camera_orientation;
        QDateTime timestamp;

        PositionStruct(double x_meter, double y_meter, int x_pixel, int y_pixel,double vehicle_orientation, double camera_orientation,double uncertainty,QDateTime timestamp)
        {
            this->x_meter = x_meter;
            this->y_meter = y_meter;
            this->x_pixel = x_pixel;
            this->y_pixel = y_pixel;
            this->uncertainty = uncertainty;
            this->vehicle_orientation = vehicle_orientation;
            this->camera_orientation = camera_orientation;
            this->timestamp = timestamp;
        }

        void translate_pixel(QPoint translation)
        {
            this->x_pixel += translation.x();
            this->y_pixel += translation.y();
        }

        PositionStruct* copy()
        {
            return new PositionStruct(x_meter,y_meter,x_pixel,y_pixel,vehicle_orientation,camera_orientation,uncertainty,timestamp);
        }

        QString getInfo()
        {
            return QString::number(x_meter,'f',2) + "\n"
                    + QString::number(y_meter,'f',2) + "\n"
                    + QString::number((int)(vehicle_orientation*180/M_PI)%360) + "\n"
                    + QString::number(camera_orientation,'f',2) + "\n"
                    + QString::number(uncertainty,'f',2) + "\n";
        }
    };
    void addPos(double x_meter, double y_meter, double x_pixel,double y_pixel, double vehicle_orientation,double camera_orientation, double uncertainty);
    int findClosestPointIndex(QPointF pixel_point, double max_min_dist = std::numeric_limits<double>::max());
    double calcDistance(QPointF point1, QPointF point2);
    QVector<PositionStruct*> positions;
    void drawVehicleBlob(QPainter* painter,PositionStruct* pos);
    void drawPath(QPainter* painter);
    void drawArrow(QPainter *painter, double angle, int length,int x_start, int y_start);
    void drawXaxis(QPainter *painter);
    void drawYaxis(QPainter *painter);
    void drawPointInfo(QPainter *painter);
    void translate(QPoint translation);
    void scale(double scale);

    int pixels_per_meter_x = 5;
    int pixels_per_meter_y = 5;
    QPoint axis_min = QPoint(0,0);

    bool mouse_is_pressed = false;
    QPoint mouse_press_position = QPoint(0,0);

    double old_orientation = 0;
    int doubleclick_index = -1;

    bool is_tracking = false;
    int samples_to_keep = -1;
    int seconds_to_keep = -1;

    /// Oldsize in resizeEvent is derp, thus this variable is needed to keep track of resizing
    QSize widget_size;
    bool save_pdf = false;
};

#endif // POSITIONWIDGET_H
