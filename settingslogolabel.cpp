#include "settingslogolabel.hpp"

settingslogolabel::settingslogolabel(QWidget *parent) :
    QLabel(parent)
{
}

void settingslogolabel::mouseMoveEvent(QMouseEvent *ev)
{
    if ((ev->buttons() & Qt::LeftButton) && firstCIsNotNull){
        auto delta = ev->pos() - m_dragPosition;
        window()->move(window()->pos() + delta);
    }
}

void settingslogolabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
            m_dragPosition = ev->pos();
            firstCIsNotNull = true;
    }
}

void settingslogolabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
            firstCIsNotNull = false;
    }
}
