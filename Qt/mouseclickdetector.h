/**
 * \class MouseClickDetector
 *
 *
 * \brief
 *      Class that eats QEvents and emits a signal if the event is a mouseclick
 *
 * \author Samuel Orn
 *
 */

#ifndef MOUSECLICKDETECTOR_H
#define MOUSECLICKDETECTOR_H

#include <QObject>

class MouseClickDetector : public QObject
{
    Q_OBJECT

public:
    explicit MouseClickDetector(QObject *parent = 0);
signals:
    void objectClicked();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MOUSECLICKDETECTOR_H
