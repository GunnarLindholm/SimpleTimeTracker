#ifndef TASKITEM_H
#define TASKITEM_H

#include <QStandardItem>
#include <QString>
#include <QList>
#include "timeinterval.h"
class taskitem : public QStandardItem
{
public:
    explicit taskitem(QString taskName);
    int type() const;
    void addTimeInterval(timeinterval timespan);
    void updateText();
    QString getTaskName();
    qint64 getTotalTime_ms();
private:
    QList<timeinterval> _timespanList;
    QString _taskName;
    qint64 _totalTime_ms;

};

#endif // TASKITEM_H
