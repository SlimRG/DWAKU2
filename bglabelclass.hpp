#ifndef BGLABELCLASS_HPP
#define BGLABELCLASS_HPP

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class BGLabelClass : public QLabel
{
    Q_OBJECT
public:
    explicit BGLabelClass(QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:
    QPoint m_dragPosition;
    bool firstCIsNotNull = true;
private:

};

#endif // BGLABELCLASS_HPP
