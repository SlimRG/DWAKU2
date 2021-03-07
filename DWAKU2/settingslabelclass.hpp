#ifndef SETTINGSLABELCLASS_HPP
#define SETTINGSLABELCLASS_HPP

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QDebug>

class SettingsLabelClass : public QLabel
{
    Q_OBJECT
public:
    explicit SettingsLabelClass(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void enterEvent(QEvent *ev);
    void leaveEvent(QEvent *ev);

signals:

    void Mouse_Pressed();
};

#endif // SETTINGSLABELCLASS_HPP
