#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "tcpthread.h"
#include <QTcpSocket>
#include "dialog.h"
#include <QtWebChannel>
#include <QWebEngineView>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class WebObj: public QObject
{
    Q_OBJECT
public:

public slots:

signals:
    void pathClear();
    void paint(QString lat_lon, int lat_lon_len,float lat, float lon);
    void initMap(float lat, float lon);
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    WebObj *webobj;
    QWebEngineView  *web1;
    QImage image;

    tcpList head = NULL;
    bool isFirst = 1;
    float  initLat, initLon;
    float  obj_lat, obj_lon;
    void  lableShow1();
    void  lableShow2();
    void  lableShow3();
    void  lableShow4();
    void  initWindow();

    ~MainWindow();

private slots:
    void on_connButton_clicked();

    void on_addButton_clicked();

    void on_starButton_clicked();

    void on_endButton_clicked();

    void on_deleteButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

public  slots:
    void success(QString ip, int port);
    void fail();
    void showMessage(QString str);
    void paint(QList<Lat_lon> lat_lon, int lat_lon_len, float lat_obj, float lon_obj);

signals:
    void  conntSocket(QString ip, int port);
    void  startSock();   //开始集群
    void  endSock();     //停止集群
    void  deleteCont();
    void  deleteCont(int index);
    void  weibuR(double R);
    void  test();
private:
    Ui::MainWindow *ui;
    int  tcpConCnt;
    tcpThread  *thread;
    QMutex mutex;

};
#endif // MAINWINDOW_H
