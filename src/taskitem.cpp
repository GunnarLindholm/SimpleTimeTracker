#include "taskitem.h"
#include <QDebug>
taskitem::taskitem(QString taskName) :
    QStandardItem()
{
    this->_taskName = taskName;
    this->setText(taskName);
    this->_totalTime_ms = 0;
    this->setEditable(false);
}

int taskitem::type() const
{
    return QStandardItem::UserType+50;
}

void taskitem::addTimeInterval(timeinterval timespan)
{
    this->_timespanList.append(timespan);
    this->_totalTime_ms += timespan.duration_ms();
    this->updateText();
}

QString taskitem::getTaskName()
{
    return this->_taskName;
}

qint64 taskitem::getTotalTime_ms()
{
    return this->_totalTime_ms;
}

QString humanreadable(qint64 time_ms)
{
    QString retval;
    int ms = time_ms%1000;
    retval.prepend(QString::number(ms).rightJustified(3,'0'));
    retval.prepend('.');
    time_ms /= 1000;
    int s = time_ms%60;
    retval.prepend(QString::number(s).rightJustified(2,'0'));
    retval.prepend(':');
    time_ms /= 60;
    int m = time_ms%60;
    retval.prepend(QString::number(m).rightJustified(2,'0'));
    retval.prepend(':');
    time_ms /= 60;
    int h = time_ms%24;
    retval.prepend(QString::number(h).rightJustified(2,'0'));
    time_ms /=24;
    int d = time_ms;
    if (d>0)
        retval.prepend(QString::number(d)+" days ");
    return retval;
}

void taskitem::updateText()
{
    this->setText(this->_taskName); //+" ==> "+humanreadable(this->_totalTime_ms));
    QStandardItem* total = this->model()->item(this->row(),1);
    total->setText(humanreadable(this->_totalTime_ms));
}
