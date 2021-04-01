#ifndef TCPTHREAD_H
#define TCPTHREAD_H
#include <QThread>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTimer>
struct Lat_lon{
    float lat;
    float lon;
};
class  socketList : public QObject
{
    Q_OBJECT
private:
    QMutex mutex;
    static const int MAX_CNT = 6;
    QTimer *timer;
    int connted_cnt = 0;
    void sendCset();
    int kk = 0;
    struct boat{
        int  id;
        float lat, lon; //纬度  经度
        float lat_obj, lon_obj; // 目标纬度  经度
        float jiaodu;// 目标在船的角度  北东坐标系
        float hangjiao; //航行角
        float len; //目标与船的距离
        float sudu; //船的速率
        float gloaSudu; //目标速率
        float sudujiao; //目标速度角
    };
    struct QTcpSock{
        QTcpSocket  *sock = new QTcpSocket;
        boat *Boat = new (boat);
    }*sock[MAX_CNT], *ss[MAX_CNT];

    void initlat_lot(float &lat, float &lon, char *spos);
    void inithangjiao(float &hangjiao, char *scag);
    void initLen_jiaodu(float &len, float &jiaodu, float hangjiao, char *sobj);
    void initgloasudu_jiao(float &gloaSudu,float &sudujiao,float hangjiao,char* scbj);
    float Len(float x, float y);
    void initsudu(float &sudu, char* sssp);
    static bool cmp(boat x, boat y);
    void sendData(QTcpSock *sock, float x, float y);
    void  moveLat_Lon(float &lat, float &lon, float rad ,float d);
public:
    double weibuR = 1000;
    explicit socketList(QObject *parent = 0);
    ~socketList();

    void  Read(QTcpSock *sock);
public  slots:
    void sendCend();
    void  guihua();
    void deleteSocklist();
    void addSocket(QString ip, int port);
    void startSock();
    void deletConn();
    void deleteCont(int index);
    void ss0Read();
    void ss1Read();
    void ss2Read();
    void ss3Read();
    void ss4Read();
    void ss5Read();


signals:
    void success(QString  ip, int port);
    void fail();
    void paint(QList<Lat_lon> lat_lon, int lat_lon_len, float lat_obj, float lon_obj);
};


typedef struct  tcpNode{
    QTcpSocket*  thread = NULL;
    tcpNode *next = NULL;
} *tcpList;

class tcpThread  : public QThread
{
    Q_OBJECT
    QString  ip;
    int  port;
    QByteArray str;
public:
    explicit tcpThread(QObject *parent = 0);
    ~tcpThread();
    socketList  *socklist = nullptr;
    tcpList head = NULL;
    void startThread();
    static const int MaxCnt = 6;
protected:
    void run();

public slots:
    void deleteCont();
    void endThread();
    void weibuR(double R);

signals:
    void endSock();
    void deleteSocklist();
    void showMessage(QString str);
    void success(QString ip, int port);
    void fail();

private:
    QMutex mutex;
    QWaitCondition cond;
    double *R;
};

#endif // TCPTHREAD_H
