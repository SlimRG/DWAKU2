#ifndef SETTINGSFRAMECLASS_H
#define SETTINGSFRAMECLASS_H

#include <QFrame>
#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class SettingsFrameClass: public QFrame
{
    Q_OBJECT
public:
    explicit SettingsFrameClass(QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:
    QPoint m_dragPosition;
    bool firstCIsNotNull = true;
};

#endif // SETTINGSFRAMECLASS_H
