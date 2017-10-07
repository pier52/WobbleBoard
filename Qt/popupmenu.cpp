#include "popupmenu.h"
#include <QPushButton>

PopupMenu::PopupMenu(QWidget* button, QWidget* parent) : QMenu(parent), b(button)
{

}

void PopupMenu::showEvent(QShowEvent*)
{
    this->move(this->parentWidget()->geometry().topLeft()+b->geometry().bottomLeft());
}
