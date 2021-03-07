#include "closelabelclass.hpp"

CloseLabelClass::CloseLabelClass(QWidget *parent) :
    QLabel(parent)
{
}

void CloseLabelClass::enterEvent(QEvent *ev)
{
        this->setStyleSheet("background-image: url(:/dynamic/close/on_enter.svg);");
        this->update();
}

void CloseLabelClass::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        this->setStyleSheet("background-image: url(:/dynamic/close/on_click.svg);");
        QApplication::quit();
    }
}

void CloseLabelClass::leaveEvent(QEvent *ev)
{
        this->setStyleSheet("background-image: url(:/dynamic/close/main.svg);");
}


