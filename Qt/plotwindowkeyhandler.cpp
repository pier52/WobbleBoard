#include "plotwindowkeyhandler.h"

PlotWindowKeyHandler::PlotWindowKeyHandler(QObject* parent) : IKeyboardHandler(50,parent)
{

}

void PlotWindowKeyHandler::handleKeyEvents()
{
    QVector<int> pressed_keys = getPressedKeys();
    // Adjust zoom

    if(pressed_keys.contains(43)) // +
    {
        emit zoom(1);
    }
    else if(pressed_keys.contains(45)) // -
    {
        emit zoom(-1);
    }
}
