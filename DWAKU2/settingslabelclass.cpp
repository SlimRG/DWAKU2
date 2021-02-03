#include "settingslabelclass.hpp"

SettingsLabelClass::SettingsLabelClass(QWidget *parent) :
    QLabel(parent)
{
}

void SettingsLabelClass::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        this->setStyleSheet("background-image: url(:/dynamic/settings/on_click.svg);");
        emit Mouse_Pressed();
    }
}

void SettingsLabelClass::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        this->setStyleSheet("background-image: url(:/dynamic/settings/on_enter.svg);");
    }
}

void SettingsLabelClass::enterEvent(QEvent *ev)
{
    this->setStyleSheet("background-image: url(:/dynamic/settings/on_enter.svg);");
}

void SettingsLabelClass::leaveEvent(QEvent *ev)
{
    this->setStyleSheet("background-image: url(:/dynamic/settings/main.svg);");
}


