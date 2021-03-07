#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <QEvent>
#include <QString>

enum EventTypes {
    InternetConnectionError
};

class MyEvent : public QEvent
{
public:
  MyEvent(const EventTypes _type) : QEvent(QEvent::User) {_localType = _type;}
 ~MyEvent() {}

  int localType() const {return _localType;}


private:
  int _localType;
};

#endif // EVENTS_HPP
