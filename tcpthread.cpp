#include "tcpthread.h"
#include "tcpconn.h"
#include <algorithm>
#include <QTimer>
#include <rgsn.h>
#include <stdlib.h>
#include <time.h>
bool socketList::cmp(boat x, boat y)
{
    if(abs(x.jiaodu-y.jiaodu)<1e-6){
        return x.len < y.len;
    }
    return x.jiaodu < y.jiaodu;
}
void  socketList::initgloasudu_jiao(float &gloaSudu,float &sudujiao,float hangjiao,char* scbj)
{
    const float pi = 3.1415926;
    Bearing B;
    float zuo[2];
    B.recvFloat((unsigned char*)scbj, zuo, 2);
    gloaSudu = Len(zuo[0],zuo[1]);
    sudujiao = hangjiao+atan2(zuo[1],zuo[0]) * 180 / pi;
    while(sudujiao < 0){
        sudujiao += 360;
    }
}

void socketList::initsudu(float &sudu, char* sssp)
{
    Bearing B;
    B.recvFloat((unsigned char*) sssp, &sudu,1);
}

void socketList::initLen_jiaodu(float &len, float &jiaodu, float hangjiao,char *sobj)
{
    const float pi = 3.1415926;
    float zuo[2];
    Bearing B;
    B.recvFloat((unsigned char*)sobj,zuo,2);
    len = Len(zuo[0],zuo[1]);
    jiaodu = atan2(zuo[1],zuo[0]) * 180. / pi;
    jiaodu += hangjiao;
    while(jiaodu < 0)
        jiaodu += 360;
}

void socketList::inithangjiao(float &hangjiao, char *scag)
{
    Bearing B;
    B.recvFloat((unsigned char*)scag,&hangjiao,1);
}

void socketList::initlat_lot(float &lat, float &lon, char *spos)
{
    Bearing B;
    float fl[2];
    B.recvFloat((unsigned char*)spos,fl,2);
    lon = fl[0];
    lat = fl[1];

}

//求解两点长度
float socketList::Len(float x, float y)
{

    int k = 1;
    while(x >= 10 || y >= 10){
        k *= 10;
        x /= 10;
        y /= 10;
    }
    return (sqrt(x*x+y*y)*k);
}
socketList::socketList(QObject *parent):QObject(parent)
{
    qRegisterMetaType<QList<Lat_lon>>("QList<Lat_lon>");

    //设置定时器，定时发送数据给服务器
    timer = new QTimer;
    timer->setInterval(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(guihua()));
    for(int i = 0; i < MAX_CNT; i++){
        sock[i] = new QTcpSock;
        ss[i] = sock[i];
    }
    connect(ss[0]->sock,SIGNAL(readyRead()),this,SLOT(ss0Read()));
    connect(ss[1]->sock,SIGNAL(readyRead()),this,SLOT(ss1Read()));
    connect(ss[2]->sock,SIGNAL(readyRead()),this,SLOT(ss2Read()));
    connect(ss[3]->sock,SIGNAL(readyRead()),this,SLOT(ss3Read()));
    connect(ss[4]->sock,SIGNAL(readyRead()),this,SLOT(ss4Read()));
    connect(ss[5]->sock,SIGNAL(readyRead()),this,SLOT(ss5Read()));
}
void socketList::deletConn()
{
    for(int i = 0; i < this->connted_cnt; i++){
        sock[i]->sock->disconnectFromHost();
    }
    connted_cnt = 0;
}
void socketList::deleteCont(int index)
{
    sock[index]->sock->disconnectFromHost();
    QTcpSock *p = sock[index];

    for(; index < connted_cnt-1; index++)
        sock[index] = sock[index+1],
        sock[index]->Boat->id = index;
    if(connted_cnt)
        sock[--connted_cnt] = p;

}
void socketList::addSocket(QString ip, int port)
{
    int index = this->connted_cnt;
    this->sock[index]->sock->connectToHost(ip, port);
    this->sock[index]->Boat->id = index;
    if(!this->sock[index]->sock->waitForConnected(10)){
        emit fail();
        return;
    }
    emit success(ip,port);
    this->connted_cnt++;

}

tcpThread::tcpThread(QObject *parent):QThread(parent)
{
    head = new tcpNode;
}


void socketList::startSock()
{
    timer->start();
    sendCset();
}

void  socketList::guihua()
{

    mutex.lock();
    QList<Lat_lon> lat_lon;
    vector<Coordinate> q0_list;
    for(int i = 0; i < this->connted_cnt; i++){
        float len = sock[i]->Boat->len;
        float rad = sock[i]->Boat->jiaodu*3.1415926/180.;
         q0_list.push_back(Coordinate(-len*cos(rad),-len*sin(rad)));
    }
    q0_list.push_back(Coordinate(0,0));
    float lat_obj, lon_obj;
    //规划下一步

    for(int i = 0; i < this->connted_cnt; i++){
        q0_list[i].first = 0;
        q0_list[i].second = 0;
        float  addjiao = 3.1415926*2/connted_cnt;
        int x, y;
        double  len = sock[i]->Boat->len;
        double  kk = (sock[i]->Boat->jiaodu)*3.1415926/180.;
        y = weibuR*sin(addjiao*i);
        x = weibuR*cos(addjiao*i);
        RGSN g = RGSN(Coordinate(x,y),Coordinate(-len*cos(kk),-len*sin(kk)),q0_list);
        y += len*sin(kk);
        x += len*cos(kk);
        len = abs(x) + abs(y);
        kk = g.Run();
        y = len*sin(kk);
        x = len*cos(kk);

        sendData(sock[i],x,y);
        lat_obj = sock[i]->Boat->lat_obj;
        lon_obj = sock[i]->Boat->lon_obj;
        Lat_lon test;
        test.lat = sock[i]->Boat->lat,
        test.lon = sock[i]->Boat->lon;
        lat_lon.append(test);
        q0_list[i].first = -sock[i]->Boat->len*cos(sock[i]->Boat->jiaodu);
        q0_list[i].second = -sock[i]->Boat->len*sin(sock[i]->Boat->jiaodu);
    }
    emit paint(lat_lon, lat_lon.length(), lat_obj, lon_obj);
    mutex.unlock();

}

void  socketList::moveLat_Lon(float &lat, float &lon, float rad ,float d){
    const  double ARC = 6371393;
    const  double PI = 3.1415926;
    rad = rad/180*PI;
    lon += d*sin(rad)/(ARC*cos(lat*PI/180)*PI/180);
    lat += d*cos(rad)/(ARC*PI/180);
}

void socketList::sendData(QTcpSock *sock, float x, float y)
{
    float fssp = sock->Boat->gloaSudu+5;
    float scag = atan2(y,x) * 180 / 3.1415926;

    if(scag < 0)
        scag += 360;

    if(abs(x)+abs(y) < 5){
        fssp = sock->Boat->gloaSudu;

    }
    if(abs(sock->Boat->hangjiao-scag)>20){
        fssp = 0.1;
    }

    //将fssp，  scag转换二进制数发送
    Bearing B;
    unsigned char str[] = "$fssp0000**#\r\n";
    B.FillSendSenWithFloats(str,&fssp,1);
    QByteArray sendBytes = QByteArray((const char*)str, sizeof(str));
    unsigned char str1[] = "$fcag0000**#\r\n";
    B.FillSendSenWithFloats(str1,&scag,1);

    sendBytes += QByteArray((const char*)str1, sizeof(str1));
    sock->sock->write(sendBytes, sendBytes.length());
    sock->sock->flush();

}

void tcpThread::run()
{
    socklist = new socketList;
    R = &socklist->weibuR;

    connect(socklist,SIGNAL(success(QString, int )),this->parent(),SLOT(success(QString, int)));
    connect(socklist,SIGNAL(fail()),this->parent(),SLOT(fail()));
    connect(this->parent(),SIGNAL(conntSocket(QString, int)), socklist, SLOT(addSocket(QString, int)));
    connect(this->parent(),SIGNAL(startSock()), socklist, SLOT(startSock()));
    connect(this->parent(),SIGNAL(endSock()), this, SLOT(endThread()));
    connect(this->parent(),SIGNAL(deleteCont()),socklist,SLOT(deletConn()));
    connect(this->parent(),SIGNAL(weibuR(double)),this,SLOT(weibuR(double)));
    connect(socklist,SIGNAL(paint(QList<Lat_lon>, int, float, float)),this->parent(),SLOT(paint(QList<Lat_lon>, int, float, float)));
    connect(this->parent(),SIGNAL(deleteCont(int)),socklist,SLOT(deleteCont(int)));
    connect(this,SIGNAL(endSock()),socklist,SLOT(sendCend()));
    exec();
}

void tcpThread::weibuR(double R)
{
    mutex.lock();
    *this->R = R;
    mutex.unlock();
}
void socketList::sendCset()
{
    Bearing B;
    unsigned char str[] = "$cset0000**#\r\n";
    float value = 1;
    B.FillSendSenWithFloats(str,&value,1);
    QByteArray sendBytes = QByteArray((const char*)str, sizeof(str));
    for(int i = 0; i < this->connted_cnt; i++){
        sock[i]->sock->write(sendBytes, sendBytes.length());
        sock[i]->sock->flush();
    }

    value = 8;
    B.FillSendSenWithFloats(str,&value,1);
    sendBytes = QByteArray((const char*)str, sizeof(str));
    for(int i = 0; i < this->connted_cnt; i++){
        sock[i]->sock->write(sendBytes, sendBytes.length());
        sock[i]->sock->flush();
    }

}

void socketList::sendCend()
{
    timer->stop();
    Bearing B;
    unsigned char str[] = "$cend0000**#\r\n";
    float value = 8;
    B.FillSendSenWithFloats(str,&value,1);
    QByteArray sendBytes = QByteArray((const char*)str, sizeof(str));

    for(int i = 0; i < this->connted_cnt; i++){
        sock[i]->sock->write(sendBytes, sendBytes.length());
        sock[i]->sock->flush();
    }

}


void tcpThread::startThread()
{
    QMutexLocker locker(&mutex);
    if (!isRunning())
    {
        start();
    }
    else
        cond.wakeOne();
}

void tcpThread::deleteCont()
{
    tcpList p = head->next;
    while(p){
        tcpList q = p;
        p = p->next;
        delete q->thread;
        delete q;
    }
    head->next = NULL;
}
void tcpThread::endThread()
{
    emit  endSock();
}
tcpThread::~tcpThread()
{
    connect(this,SIGNAL(deleteSocklist()),socklist,SLOT(deleteSocklist()),Qt::BlockingQueuedConnection);
    if(socklist)
        emit deleteSocklist();
    quit();
    wait();

}
void socketList::deleteSocklist()
{
    for(int i = 0; i < MAX_CNT; i++){
        delete  sock[i]->sock;
    }

}
socketList::~socketList()
{

}

void socketList::ss0Read()
{
    Read(ss[0]);
}
void socketList::ss1Read()
{
    Read(ss[1]);
}
void socketList::ss2Read()
{
    Read(ss[2]);
}
void socketList::ss3Read()
{
    Read(ss[3]);
}
void socketList::ss4Read()
{
    Read(ss[4]);
}
void socketList::ss5Read()
{
    Read(ss[5]);
}

void socketList::Read(QTcpSock *sock)
{
    QByteArray data = sock->sock->readAll();
    mutex.lock();
    //解析
    QByteArray spos = "$spos00000000**#";  // 无人艇经纬度
    QByteArray scag = "$scag0000**#"; //	无人艇航向角
    QByteArray sssp = ("$sssp0000**#"); // 无人艇航速
    QByteArray sobj = ("$sobj00000000**#"); //	围捕对象相对船的位置
    QByteArray scbj = ("$scbj00000000**#"); //	围捕对象相对船的速度
    QByteArray ssbj = ("$ssbj0000**#"); // 围捕对象类型
    bool b_spos = 0, b_scag = 0, b_sssp = 0, b_sobj = 0, b_scbj = 0,b_ssbj = 0;
    QByteArray  str;
    boat *boatGroup = sock->Boat;
    for(int i = 0; i < data.length(); i++){
        if(data[i] == '$'){
            str.clear();
            int j;
            for(j = 0; j < 5 && i+j < data.length(); j++)
                str += data[i+j];
            if(j != 5)
                continue;

            if(str == "$spos"){
                for(j = 0; j < 11 && i+j+5 < data.length(); j++)
                    str += data[i+j+5];
                if(j != 11)
                    continue;
                if(str[13] != '*')
                    continue;
                if(str[14] != '*')
                    continue;
                if(str[15] != '#')
                    continue;
                spos = str;
                b_spos = 1;
                i += 15;
            }
            else if(str == "$scag")    {
                for(j = 0; j < 7 && i+j+5 < data.length(); j++)
                    str += data[i+j+5];
                if(j != 7)
                    continue;
                if(str[9] != '*')
                    continue;
                if(str[10] != '*')
                    continue;
                if(str[11] != '#')
                    continue;
                scag = str;
                b_scag = 1;
                i += 11;
            }
            else if(str == "$sssp")   {
                for(j = 0; j < 7 && i+j+5 < data.length(); j++)
                    str += data[i+j+5];
                if(j != 7)
                    continue;
                if(str[9] != '*')
                    continue;
                if(str[10] != '*')
                    continue;
                if(str[11] != '#')
                    continue;
                sssp = str;
                b_sssp = 1;
                i += 11;
            }
            else if(str == "$sobj")    {
                for(j = 0; j < 11 && i+j+5 < data.length(); j++)
                    str += data[i+j+5];
                if(j != 11)
                    continue;
                if(str[13] != '*')
                    continue;
                if(str[14] != '*')
                    continue;
                if(str[15] != '#')
                    continue;
                sobj = str;
                b_sobj = 1;
                i += 15;
            }
            else if(str == "$scbj")    {
                for(j = 0; j < 11 && i+j+5 < data.length(); j++)
                    str += data[i+j+5];
                if(j != 11)
                    continue;
                if(str[13] != '*')
                    continue;
                if(str[14] != '*')
                    continue;
                if(str[15] != '#')
                    continue;
                scbj = str;
                b_scbj = 1;
                i += 15;
            }
            else if(str == "$ssbj")    {
                for(j = 0; j < 7 && i+j+5 < data.length(); j++)
                    str += data[i+j+5];
                if(j != 7)
                    continue;
                if(str[9] != '*')
                    continue;
                if(str[10] != '*')
                    continue;
                if(str[11] != '#')
                    continue;
                ssbj = str;
                b_ssbj = 1;
                i += 11;
            }
        }

    }


    if(b_spos)
        initlat_lot(boatGroup->lat,boatGroup->lon,spos.data());
    if(b_scag)
        inithangjiao(boatGroup->hangjiao,scag.data());
    if(b_sobj){
        initLen_jiaodu(boatGroup->len,boatGroup->jiaodu,boatGroup->hangjiao, sobj.data());
        boatGroup->lat_obj = boatGroup->lat;
        boatGroup->lon_obj = boatGroup->lon;
        moveLat_Lon(boatGroup->lat_obj, boatGroup->lon_obj, boatGroup->jiaodu, boatGroup->len);
    }
    if(b_sssp)
        initsudu(boatGroup->sudu,sssp.data());
    if(b_scbj)
        initgloasudu_jiao(boatGroup->gloaSudu,boatGroup->sudujiao,boatGroup[0].hangjiao,scbj.data());
    mutex.unlock();

}
