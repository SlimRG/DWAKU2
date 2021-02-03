#include "bglabelclass.hpp"

BGLabelClass::BGLabelClass(QWidget *parent) :
    QLabel(parent)
{
}

void BGLabelClass::mouseMoveEvent(QMouseEvent *ev)
{
    if ((ev->buttons() & Qt::LeftButton) && firstCIsNotNull){
            window()->move( mapToGlobal(ev->pos() - m_dragPosition));
    }
}

void BGLabelClass::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
            m_dragPosition = ev->pos();
            firstCIsNotNull = true;
    }
}

void BGLabelClass::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
            firstCIsNotNull = false;
    }
}
