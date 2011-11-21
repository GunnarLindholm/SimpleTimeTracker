#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtDebug"
#include "QTimer"
#include "listitem.h"
#include "QDir"
#include "QFile"
#include "QTextStream"
#include <QStringList>
#include "timeinterval.h"
#include "taskitem.h"
#include <QMessageBox>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->trackTime=false;
    ui->setupUi(this);
    this->totalTime_ms = 0;
    this->timer = new QTimer(this);
    this->_modell.setColumnCount(2);
    ui->treeView->setModel(&(this->_modell));
    this->loadOrCreateDataFile();

    connect(this->timer,SIGNAL(timeout()),this,SLOT(displayCurrentRunningTime()));
    connect(ui->toggleTimer,SIGNAL(clicked()),this,SLOT(toggleTimer()));
    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(itemSelected(QModelIndex)));
}


void MainWindow::itemSelected(QModelIndex index)
{
    if (!this->trackTime){
        QStandardItem* sitem = this->_modell.itemFromIndex(index);
        if (sitem->column()==0 && sitem->parent()== 0)
        {
            taskitem* item = (taskitem*) sitem;
            ui->taskNameEdit->setText(item->getTaskName());
        }
    }
}

bool MainWindow::loadOrCreateDataFile()
{
    QDir workDir(QDir::home());
    if (!workDir.cd(".simpletimetracker")){
        qDebug()<<"Could not switch to dir .simpletimetracker";
        if (!workDir.exists(".simpletimetracker")){
            qDebug()<<"no such dir "+workDir.absolutePath()+"/.simpletimetracker";
            if (!workDir.mkdir(".simpletimetracker")) {
                qDebug()<<"Could not create the dir .simpletimetracker";
                return false;
            }
        }
    }
    workDir.cd(QDir::home().absolutePath());
    if (!workDir.cd(".simpletimetracker")) {
        qDebug()<<"Could not switch to dir "<<workDir.absolutePath()<<QDir::separator()<<".simpletimetracker";
        return false;
    }
    QFile dataFile(workDir.absolutePath()+QDir::separator()+"data.txt");
    if (!dataFile.exists()){
        qDebug()<<"File "<<workDir.absolutePath()+QDir::separator()+"data.txt"<<" does not exist.. continue";
        return false;
    }
    else {
        if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;
        QTextStream in(&dataFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            process_line(line);
        }
        return true;
    }
}

void MainWindow::process_line(QString line)
{
    QString name;
    bool ok1,ok2;
    qint64 startTime,stopTime;
    QStringList list = line.split(";;",QString::SkipEmptyParts);
    if (list.size()==1) // a single record
    {
        name = list.at(0).trimmed();
        if (!name.isEmpty())
        {
            this->getTask(name);
        }
    }
    else if (list.size()==3 ){
        name = list.at(0).trimmed();
        startTime = list.at(1).toLongLong(&ok1);
        stopTime = list.at(2).toLongLong(&ok2);
        if (!name.isEmpty() && startTime <= stopTime && ok1 && ok2){
            taskitem* task = this->getTask(name);
            task->addTimeInterval(timeinterval(startTime,stopTime));
            this->totalTime_ms += stopTime-startTime;
            this->displayTotalRunningTime();
        }
    }
    //else invalid line, just ignore it
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    QString taskName;
    taskitem* task;
    QList<QStandardItem*> items;
    QDir workDir(QDir::home());
    if (!workDir.cd(".simpletimetracker")){
        qDebug()<<"Could not switch to dir .simpletimetracker";
        if (!workDir.exists(".simpletimetracker")){
            qDebug()<<"no such dir "+workDir.absolutePath()+"/.simpletimetracker";
            if (!workDir.mkdir(".simpletimetracker")) {
                qDebug()<<"Could not create the dir .simpletimetracker";
            }
        }
    }
    workDir.cd(QDir::home().absolutePath());
    if (!workDir.cd(".simpletimetracker")) {
        qDebug()<<"Could not switch to dir "<<workDir.absolutePath()<<QDir::separator()<<".simpletimetracker";
    }
    QFile dataFile(workDir.absolutePath()+QDir::separator()+"data.txt");
    if (dataFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&dataFile);
        foreach (taskName,this->_explicitListOfTasks)
        {
            items = this->_modell.findItems(taskName,Qt::MatchExactly);
            if (items.count()!=1)
                qDebug()<<"Strange, there SHOULD be exactly ONE task named "<<taskName;
            else
            {
                task = (taskitem*) items.at(0);
                out << task->getTaskName()<<";;0;;"<<task->getTotalTime_ms();
            }
        }
        dataFile.close();
    }
    else
        qDebug()<<"Could not open the file "<<workDir.absolutePath()<<QDir::separator()<<"data.txt";
    event->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

taskitem* MainWindow::getTask(QString taskName)
{
    taskitem* item;
    QList<QStandardItem*> items = this->_modell.findItems(this->_currentTaskName,Qt::MatchExactly);
    if (items.empty())
    {
        item = new taskitem(taskName);
        this->_explicitListOfTasks.append(taskName);
        this->_modell.appendRow(item);
        // add the total counter
        QStandardItem* sitem =new QStandardItem("");
        sitem->setEditable(false);
        this->_modell.setItem(item->row(),1,sitem);
    }
    else
        item = (taskitem*)items.at(0);
    return item;
}

void MainWindow::toggleTimer()
{
    if (this->trackTime){
        qint64 stopPoint_msSinceEpoch = QDateTime::currentMSecsSinceEpoch();
        this->trackTime = false;
        // Try to find existing task item, else create it
        {
            taskitem* item = this->getTask(this->_currentTaskName);
            item->addTimeInterval(timeinterval(this->startingPoint_msSinceEpoch,stopPoint_msSinceEpoch));
        }
        this->timer->stop();
        ui->toggleTimer->setText("Start timer");

        this->totalTime_ms += (stopPoint_msSinceEpoch-startingPoint_msSinceEpoch);
        this->displayTotalRunningTime();
    }else {
        this->_currentTaskName = (ui->taskNameEdit->text()).trimmed();
        if (this->_currentTaskName.isEmpty())
        {
            QMessageBox error(this);
            error.setText("You must set a non-empty name for the task");
            error.exec();
        }
        else
        {
            this->startingPoint_msSinceEpoch = QDateTime::currentMSecsSinceEpoch();
            this->timer->start(200);
            this->time.start();
            this->trackTime = true;
            ui->toggleTimer->setText("Stop timer");
        }
    }
}

void MainWindow::displayTotalRunningTime()
{
    int days = this->totalTime_ms/86400000;
    QTime t;
    QTime time = t.addMSecs(this->totalTime_ms%86400000);
    if (days > 0)
        ui->statusBar->showMessage(QString::number(days)+" days "+time.toString("hh:mm:ss.zzz"));
    else
        ui->statusBar->showMessage(time.toString("hh:mm:ss.zzz"));
}

void MainWindow::displayCurrentRunningTime()
{
    if (this->trackTime){
        ui->label->setText(QString::number((int)(this->time.elapsed())/1000));
    }
}
