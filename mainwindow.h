#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define OK 0
#define PARKINGSIZE 5
#define PARKINGPRICE 0.1
#define QUEUESIZE 4
// Car
struct Car
{
    QString regnumber; // Registration number
    QDateTime inTime; // Parking time
};

// Circular Queue
typedef struct
{
    Car base[QUEUESIZE]; // array
    int front; // front subscript
    int rear; // rear subscript
}Queue;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();


    void InitCarList();
    void InitQueue(Queue *);
    int EnQueue(Queue *, Car);
    int DeQueue(Queue *, Car *);
    int QueueLength(Queue *);


    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
