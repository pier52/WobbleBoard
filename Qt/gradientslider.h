/**
 * \class GradientSlider
 *
 *
 * \brief
 * Widget which implements a graphical slider, which is filled by a color scheme gradient
 *
 *
 * \author Samuel Orn
 *
 */

#ifndef COLORSLIDER_H
#define COLORSLIDER_H

#include <QWidget>
#include <QBrush>
#include <QPen>

class GradientSlider : public QWidget
{
    Q_OBJECT
public:
    explicit GradientSlider(QWidget *parent = 0);
    void setFrame(bool visible);
    void setFrame(bool visible, QColor color);
    void setGradient(QLinearGradient gradient);

    /// Sets predefined gradient following the 'hot' gradient (black-red-yellow-white)
    void setGradientHot();
    void setBackground(QColor color);
    QLinearGradient getGradient() const;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

signals:

public slots:
    /// Sets how filled the slider is, from 0-100
    void setFillPercentage(double fill_percentage);

private:
    bool frame;
    double fill_percentage;
    QColor frame_color;
    QLinearGradient gradient;
};

#endif // COLORSLIDER_H
