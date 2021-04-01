#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <iostream>
#include <QImage>
#include <QWebEngineView>
#include "tcpconn.h"
#include <fstream>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    image = QImage(800,700,QImage::Format_RGB32);
    QRgb backColor = qRgb(255,255,255);
    image.fill(backColor);
    QPainter painter(&image);
    ui->Paint->Paint(image);

    //部署web控件   用于显示百度地图
    webobj = new WebObj;
    QWebChannel *webchannel = new  QWebChannel;
    webchannel->registerObject("webobj",webobj);
    web1 = new QWebEngineView(this);
    web1->setGeometry(ui->Paint->x(),ui->Paint->y(),ui->Paint->width(),ui->Paint->height());
//    web1->page()->load(QUrl((qApp->applicationDirPath()+"/ditu/ditu.html")));
    web1->page()->load(QUrl(("C:/Users/submer/Desktop/ditu/ditu.html")));

    web1->page()->setWebChannel(webchannel);

    initWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
    emit endSock();
    emit deleteCont();
    delete thread;
}

//轨迹绘画
void MainWindow::paint(QList<Lat_lon> lat_lon, int length, float lat_obj, float lon_obj)
{
    mutex.lock();
    this->obj_lat = lat_obj;
    this->obj_lon = lon_obj;
    QString str = "";
    if(isFirst){
        isFirst = 0;
        initLat = lat_obj;
        initLon = lon_obj;
        emit webobj->initMap(lat_obj,lon_obj);
    }
    QPainter PP(&this->image);
    Bearing B;
    float  y = initLon, x = initLat;
    const double PI =  3.14159265;
    QImage image = QImage(800,700,QImage::Format_RGB32);
    QRgb backColor = qRgb(255,255,255);
    image.fill(backColor);
    QPainter painter(&image);
    int  Rx, Ry;
    float len, jiao;

    for(int i = 0; i < length; i++){

        float lat = lat_lon.at(i).lat;
        float lon = lat_lon.at(i).lon;
        str += QString::number(lat,'g',10)+"|";
        str += QString::number(lon,'g',10)+"|";
//        qDebug() << "boat:" << lat << " " << lon;
        Rx = 400, Ry = 350;

        len = B.ComplexG(x,y,lat,lon)*0.1;
        jiao = B.ComplexF(x,y,lat,lon);

        Rx += len*sin(jiao*PI/180);
        Ry += len*cos(jiao*PI/180);
        painter.drawEllipse(Rx,Ry,5,5);
        PP.drawEllipse(Rx,Ry,2,2);
    }
//    qDebug() << "obj" << lat_obj << " " << lon_obj;
//    str += QString::number(lat_obj,'g',10)+"|";
//    str += QString::number(lon_obj,'g',10)+"|";
    painter.setPen(Qt::red);
    Rx = 400, Ry = 350;
    len = B.ComplexG(x,y,lat_obj,lon_obj)*0.1;
    jiao = B.ComplexF(x,y,lat_obj,lon_obj);
    Rx += len*sin(jiao*PI/180);
    Ry += len*cos(jiao*PI/180);
    painter.drawEllipse(Rx,Ry,5,5);
    PP.drawEllipse(Rx,Ry,2,2);
    ui->Paint->Paint(image);
    emit webobj->paint(str,length, lat_obj, lon_obj);
    mutex.unlock();
}


//lableshow1 - lableshow3 是界面状态 控制当前哪些按钮可以点击等

//初始化状态，结束状态
void  MainWindow::lableShow1()
{
    ui->deleteButton->setEnabled(0);
    ui->ipEdit->setEnabled(1);
    ui->portEdit->setEnabled(1);
    ui->connButton->setEnabled(1);
    ui->addButton->setEnabled(0);
    ui->starButton->setEnabled(0);
    ui->endButton->setEnabled(0);
    ui->conncnt->setNum(this->tcpConCnt);
}

//点击‘连接’按钮后的状态
void  MainWindow::lableShow2()
{
    ui->deleteButton->setEnabled(1);
    ui->connButton->setEnabled(1);
    ui->addButton->setEnabled(1);
    ui->starButton->setEnabled(1);
    ui->endButton->setEnabled(0);
    ui->conncnt->setNum(this->tcpConCnt);
}

//点击‘开始’后的状态
void  MainWindow::lableShow3()
{
    ui->deleteButton->setEnabled(0);
    ui->connButton->setEnabled(0);
    ui->addButton->setEnabled(0);
    ui->starButton->setEnabled(0);
    ui->endButton->setEnabled(1);
    ui->conncnt->setNum(this->tcpConCnt);
}
void  MainWindow::lableShow4()
{

}

//界面初始化
void MainWindow::initWindow()
{
    this->tcpConCnt = 0;   //连接服务器的数量为0
    isFirst = 1;     //在绘画路径的时候会用到

    //构造一个子线程  客户端与服务端的通信都是在这个线程中进行的
    this->thread = new tcpThread(this);

    //连接信号和槽  用于状态栏显示连接是否成功
    connect(this->thread,SIGNAL(showMessage(QString)),this,SLOT(showMessage(QString)));
    this->thread->startThread();  //启动线程
    lableShow1();

}

//连接服务器
void MainWindow::on_connButton_clicked()
{

    QString ip = ui->ipEdit->text();
    int port = ui->portEdit->text().toInt();
    emit conntSocket(ip, port);

}

//断开所有服务器
void MainWindow::on_addButton_clicked()
{
    emit deleteCont();
    ui->comboBox->clear();
    this->isFirst = 1;
    this->tcpConCnt = 0;
    this->lableShow1();
}

void MainWindow::on_starButton_clicked()
{
    emit startSock();
    this->lableShow3();
}

void MainWindow::on_endButton_clicked()
{
    this->lableShow2();
    emit endSock();
}

void MainWindow::success(QString ip, int port)
{
    ui->statusbar->showMessage(tr("连接成功"),2000);
    ui->comboBox->addItem(ip+" "+QString::number(port));
    this->tcpConCnt++;
    emit weibuR(0);
    this->lableShow2();
}

void MainWindow::fail()
{
    ui->statusbar->showMessage(tr("连接失败"),2000);
}

void MainWindow::showMessage(QString str)
{
    ui->statusbar->showMessage(str,2000);
}


//删除下框显示的服务器
void MainWindow::on_deleteButton_clicked()
{
    int index = ui->comboBox->currentIndex();
    if(index >= 0){
        emit deleteCont(index);
        ui->comboBox->removeItem(index);
        this->tcpConCnt--;
        ui->conncnt->setNum(tcpConCnt);
    }
}

//设置围捕半径
void MainWindow::on_pushButton_clicked()
{
    double t = ui->weibuREdit->text().toDouble();
    emit weibuR(t);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->Paint->Paint(image);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->Paint->show();
    web1->hide();
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->Paint->hide();
    web1->show();
}

void MainWindow::on_pushButton_5_clicked()
{
    emit  webobj->pathClear();
    QRgb backColor = qRgb(255,255,255);
    image.fill(backColor);
    ui->Paint->Paint(image);
}

void MainWindow::on_pushButton_6_clicked()
{
    emit  webobj->initMap(obj_lat, obj_lon);
}
