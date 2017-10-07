/**
 * \class PlotWindowKeyHandler
 *
 *
 * \brief
 *      Handles all the user's key presses in the plot window. Currently only used for zooming.
 *
 *
 * \author Samuel Orn
 *
 */


#ifndef PLOTWINDOWKEYHANDLER_H
#define PLOTWINDOWKEYHANDLER_H

#include "ikeyboardhandler.h"

class PlotWindowKeyHandler : public IKeyboardHandler
{
    Q_OBJECT
public:
    explicit PlotWindowKeyHandler(QObject *parent);

signals:
    void zoom(int);

private slots:
    void handleKeyEvents() override;
};

#endif // PLOTWINDOWKEYHANDLER_H
