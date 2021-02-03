#include "settingsframeclass.hpp"

SettingsFrameClass::SettingsFrameClass(QWidget *parent) : QFrame(parent)
{
}

void SettingsFrameClass::mouseMoveEvent(QMouseEvent *ev)
{
    if ((ev->buttons() & Qt::LeftButton) && firstCIsNotNull){
            window()->move( mapToGlobal(ev->pos() - m_dragPosition - this->geometry().topLeft()));
    }
}

void SettingsFrameClass::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
            m_dragPosition = ev->pos();
            firstCIsNotNull = true;
    }
}

void SettingsFrameClass::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
            firstCIsNotNull = false;
    }
}
