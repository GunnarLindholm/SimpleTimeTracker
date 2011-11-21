#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QModelIndex>
#include <QStandardItemModel>
#include "taskitem.h"
#include <QCloseEvent>
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
protected slots:
    void toggleTimer();
    void displayCurrentRunningTime();
    void itemSelected(QModelIndex);
public slots:
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    bool trackTime;
    QTime time;
    qint64 startingPoint_msSinceEpoch;
    QTimer *timer;
    qint64 totalTime_ms;
    void displayTotalRunningTime();
    /**Load the datafile or creates a new one. Return true on success, else false*/
    bool loadOrCreateDataFile();
    void process_line(QString line);
    QString _currentTaskName;
    QStandardItemModel _modell;
    QList<QString> _explicitListOfTasks; // we need this list since there is really no nice easy way of traversing all items.

    /**Return the already existing task or create a new one.
    @return pointer to the item*/
    taskitem* getTask(QString text);
};

#endif // MAINWINDOW_H
