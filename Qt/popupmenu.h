/**
 * \class PopupMenu
 *
 *
 * \brief
 *      A small extension of QMenu to make it a popup menu.
 *
 * \author Samuel Orn
 *
 */

#ifndef POPUPMENU_H
#define POPUPMENU_H

#include <QMenu>

//class QPushButton;
//class QWidget;

class PopupMenu : public QMenu
{
    Q_OBJECT
public:
    explicit PopupMenu(QWidget* button, QWidget* parent = 0);
    void showEvent(QShowEvent* event);
private:
    QWidget* b;
};

#endif // POPUPMENU_H
