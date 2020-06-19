#include "mainwindow.h"
#include "ui_mainwindow.h"

//停车位
Car car[PARKINGSIZE];
Queue queue;

QStringList strList_lot;
QStringList strList_queue;
QStringListModel *model_lot;
QStringListModel *model_queue;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //将输入框设置为不可编辑
    ui->textEdit_2->setReadOnly(true);
    ui->textEdit_3->setReadOnly(true);
    ui->textEdit_4->setReadOnly(true);
    ui->textEdit_5->setReadOnly(true);

    //将列表设置为不可编辑
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //初始化计时器
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_3->setDateTime(QDateTime::currentDateTime());

    //初始化车位数组
    InitCarList();
    InitQueue(&queue);
    model_lot = new QStringListModel(strList_lot);
    ui->listView->setModel(model_lot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitCarList(){
    //初始化车位数组
    Car init_car;
    init_car.regnumber = "空车位";
    init_car.inTime = QDateTime::currentDateTime();
    for(int i=0;i<PARKINGSIZE;i++){
        car[i] = init_car;
        strList_lot.append(init_car.regnumber);
    }
    ui->textEdit_3->append("停车场初始化成功,位置数为:"+QString::number(PARKINGSIZE)+"\n");
}

void MainWindow::InitQueue(Queue *cq)
{
    cq->front = cq->rear =0;
    ui->textEdit_3->append("停车队列初始化成功,位置数为:"+QString::number(QUEUESIZE-1)+"\n");
}

int MainWindow::QueueLength(Queue *cq)
{
    return ((cq->rear - cq->front + QUEUESIZE)%QUEUESIZE);

}


//车进入队尾
int MainWindow::EnQueue(Queue *cq, Car car)
{
    if(cq->front==(cq->rear+1)%QUEUESIZE)
           return 0;  /*is full?*/
    cq->base[cq->rear]= car;
    cq->rear=(cq->rear+1)%QUEUESIZE;
    return 1;
}
//第一辆车出队
int MainWindow::DeQueue(Queue *cq, Car *car)
{
    if(cq->rear==cq->front) return 0; /* is empty*/
    *car = cq->base[cq->front];
     cq->front=(cq->front+1)%QUEUESIZE;
     return 1;
}

//进入停车场按钮
void MainWindow::on_pushButton_clicked()
{
    Car temp_car;
    temp_car.regnumber = ui->textEdit->toPlainText();
    temp_car.inTime = ui->dateTimeEdit->dateTime();

    //判断车辆进入停车场还是队列还是无法进入
    for(int i=0;i<PARKINGSIZE;i++){
        //判断停车场是否有空位
        if(car[i].regnumber == "空车位"){
            car[i] = temp_car;
            ui->textEdit_3->append(temp_car.inTime.toString()+" 进入停车场，车牌号:"+temp_car.regnumber+"\n");

            //清空列表显示
            strList_lot.clear();
            //显示列表
            for(int j=0;j<PARKINGSIZE;j++){
                strList_lot.append(car[j].regnumber);
            }
            model_lot = new QStringListModel(strList_lot);
            ui->listView->setModel(model_lot);
            break;
        }
        //停车场满了，判断队列
        if(i+1 == PARKINGSIZE){
            if(EnQueue(&queue, temp_car)){
                ui->textEdit_3->append(temp_car.inTime.toString()+" 停车场已满，进入等待队伍，车牌号:"+temp_car.regnumber+"\n");
                strList_queue.append(temp_car.regnumber);
                model_queue = new QStringListModel(strList_queue);
                ui->listView_2->setModel(model_queue);
            }else{
                ui->textEdit_3->append(temp_car.inTime.toString()+" 停车场和等待队伍已满，无法进入，车牌号:"+temp_car.regnumber+"\n");
            }
        }
    }

}

//计算费用按钮
void MainWindow::on_pushButton_3_clicked()
{
    QString car_id;
    QDateTime car_in_time;
    QDateTime car_out_time;

    int price;

    car_out_time = ui->dateTimeEdit_3->dateTime();

    QModelIndexList modelIndexList = ui->listView->selectionModel()->selectedIndexes();
    foreach(QModelIndex modelIndex, modelIndexList){
        car_id = model_lot->data(modelIndex).toString();
        //判断是否为空车位，如果是则不显示，如果不是则显示车牌号，进场时间，价格
        if(car_id != "空车位"){
            //显示车牌号
            ui->textEdit_2->setPlainText(car_id);
            //获取进场时间
            for(int i=0;i<PARKINGSIZE;i++){
                if(car[i].regnumber == car_id){
                    car_in_time = car[i].inTime;
                    ui->textEdit_4->setPlainText(car_in_time.toString());
                }
            }
            //计算和显示价格
            price = car_in_time.secsTo(car_out_time)/60*PARKINGPRICE;
            ui->textEdit_5->setPlainText(QString::number(price));
        }
    }
}

//缴费完成按钮
void MainWindow::on_pushButton_2_clicked()
{
    QString car_id = ui->textEdit_2->toPlainText();

    Car temp_car;
    for(int i=0;i<PARKINGSIZE;i++){
        //删除该车并将车位置为空车位
        if(car[i].regnumber == car_id){
            car[i].regnumber = "空车位";
            car[i].inTime = QDateTime::currentDateTime();
            ui->textEdit_3->append(ui->dateTimeEdit_3->dateTime().toString()+" 车已出场，车牌号:"+car_id+"  收费:"+ui->textEdit_5->toPlainText()+"元\n");
            //判断队列中是否有等待车辆
            if(QueueLength(&queue) != 0){
                //将车离队
                DeQueue(&queue, &temp_car);
                strList_queue.removeFirst();
                model_queue = new QStringListModel(strList_queue);
                ui->listView_2->setModel(model_queue);
                //将车入场
                car[i].regnumber = temp_car.regnumber;
                car[i].inTime = temp_car.inTime;
                ui->textEdit_3->append(temp_car.inTime.toString()+" 车已离队入场，车牌号:"+temp_car.regnumber+"\n");
            }

            //清空列表显示
            strList_lot.clear();
            //显示列表
            for(int j=0;j<PARKINGSIZE;j++){
                strList_lot.append(car[j].regnumber);
            }
            model_lot = new QStringListModel(strList_lot);
            ui->listView->setModel(model_lot);
            break;
        }
    }
}
