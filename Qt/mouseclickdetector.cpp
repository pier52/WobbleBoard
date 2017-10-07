#include "mouseclickdetector.h"
#include <QEvent>

MouseClickDetector::MouseClickDetector(QObject *parent) : QObject(parent)
{

}

bool MouseClickDetector::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        emit objectClicked(); 
    }
    return QObject::eventFilter(obj, event);
}
