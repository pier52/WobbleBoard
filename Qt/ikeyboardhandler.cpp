#include "ikeyboardhandler.h"

IKeyboardHandler::IKeyboardHandler(int update_interval_ms,QObject *parent) : QObject(parent)
{
    key_timer = new QTimer();
    key_timer->setInterval(update_interval_ms);
    parent->installEventFilter(this);

    connect(key_timer,SIGNAL(timeout()),this,SLOT(handleKeyEvents()));
}

IKeyboardHandler::~IKeyboardHandler()
{
    delete key_timer;
}

bool IKeyboardHandler::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if(keyEvent->isAutoRepeat()) // Key is held down
            return true;

        pressed_keys.push_back(keyEvent->key());

        if(key_timer->isSingleShot())
        {
            key_timer->setSingleShot(false);
        }

        if(!key_timer->isActive())
        {
            key_timer->start();
        }

        return true;

    }
    else if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->isAutoRepeat())
            return true;

        pressed_keys.removeAll(keyEvent->key());

        if(pressed_keys.isEmpty())
        {
            key_timer->setSingleShot(true);
        }

        return true;
    }

    else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

const QVector<int>& IKeyboardHandler::getPressedKeys() const
{
    return pressed_keys;
}
