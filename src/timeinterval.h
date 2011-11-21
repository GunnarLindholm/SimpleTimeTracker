#ifndef TIMEINTERVAL_H
#define TIMEINTERVAL_H
#include <QString>
class timeinterval
{
public:
    timeinterval(qint64 startTime_ms,qint64 stopTime_ms);
    qint64 duration_ms();
    QString when();
private:
    qint64 _startTime_ms;
    qint64 _stopTime_ms;
    QString _when;
};

#endif // TIMEINTERVAL_H
