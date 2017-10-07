/**
 * \class IKeyboardHandler
 *
 *
 * \brief
 *      Abstract base class, which installs an eventfilter in the parent class, intercepting all key presses.
 *      All keys (identified by their integer ID) which are held down are stored in an internal list, and removed once the key is released.
 *      The pure virtual function 'handleKeyEvents' (which must be implemented in the child class) is called with a certain configurable time
 *      interval, internally govern by a QTimer. Here, the currently pressed keys can be handled in the child class; for instance, signals can be
 *      sent. This is useful when a signal needs to be sent continously as long as a certain key is pressed.
 *
 * \author Samuel Orn
 *
 */

#ifndef IKEYBOARDHANDLER_H
#define IKEYBOARDHANDLER_H

#include <QObject>
#include <QTimer>
#include <QKeyEvent>

class IKeyboardHandler : public QObject
{
    Q_OBJECT
public:
    explicit IKeyboardHandler(int update_interval_ms,QObject *parent);
    const QVector<int>& getPressedKeys() const;
    ~IKeyboardHandler();

protected slots:
    virtual void handleKeyEvents() = 0;

private:
    bool eventFilter(QObject *obj, QEvent *event);
    QTimer* key_timer;
    QVector<int> pressed_keys;
};

#endif // IKEYBOARDHANDLER_H
