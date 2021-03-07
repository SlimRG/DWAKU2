#ifndef SETTINGSLOGOLABEL_H
#define SETTINGSLOGOLABEL_H

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class settingslogolabel : public QLabel
{
    Q_OBJECT
public:
    explicit settingslogolabel(QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:
    QPoint m_dragPosition;
    bool firstCIsNotNull = true;
private:
};

#endif // SETTINGSLOGOLABEL_H
