#ifndef CLOSELABELCLASS_HPP
#define CLOSELABELCLASS_HPP

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QEvent>
#include <QApplication>

class CloseLabelClass : public QLabel
{
    Q_OBJECT
public:
    explicit CloseLabelClass(QWidget *parent = 0);


    void mousePressEvent(QMouseEvent *ev);
    void enterEvent(QEvent* ev);
    void leaveEvent(QEvent* ev);

};

#endif // CLOSELABELCLASS_HPP
