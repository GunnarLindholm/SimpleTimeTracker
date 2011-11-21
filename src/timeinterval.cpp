#include "timeinterval.h"
#include <QDateTime>

timeinterval::timeinterval(qint64 startTime_ms,qint64 stopTime_ms)
{
    this->_startTime_ms = startTime_ms;
    this->_stopTime_ms = stopTime_ms;
    this->_when = QDateTime::fromMSecsSinceEpoch(this->_startTime_ms).toString("yyyy-MM-dd hh:mm:ss")+"  -  "+QDateTime::fromMSecsSinceEpoch(this->_stopTime_ms).toString("yyyy-MM-dd hh:mm:ss");
}

qint64 timeinterval::duration_ms()
{
    return this->_stopTime_ms-this->_startTime_ms;
}

QString timeinterval::when()
{
    return this->_when;
}
