#ifndef RGSN_H
#define RGSN_H
#include <iostream>
#include <vector>
#include <cmath>
#include <deque>
using namespace  std;
typedef pair<double, double> Coordinate; //东北坐标  第个横坐标 第二个纵坐标
class  RGSN{
public:
    const double MINvalue = 1e-6;
    int GGC = 15;   //引力增益系数
    int PGC = 5;   //斥力增益系数
    Coordinate  qg = Coordinate(100,100); //目标坐标
    Coordinate  q = Coordinate(0,0);  //机器人坐标
    vector<Coordinate>  q0_List;// 障碍物坐标
    double  n = 2;//斥力因子
    int  BORDER = 10; //斥力边界
    int  step = 2;  //步长


        vector<Coordinate>  Path;
        //参数顺序  (引力增益系数 斥力增益系数 目标坐标  机器人坐标 障碍物坐标 斥力边界)
        RGSN(Coordinate qg = Coordinate(0,0), Coordinate q=Coordinate(100,100), vector<Coordinate>  q0_List = {});
        float Run(); //路径规划
        int  T;
        Coordinate getG(); //返回引力
        Coordinate getP(int i); //放回斥力
        double jili_pow(Coordinate  a, Coordinate b);
        void  setStep(int step);
        void  setBorder(int Border);
        void  setGGC(int  GGC);
        void  setPGC(int  PGC);
        void  setN(int n);
        void  setQg(double x, double y);
        void  setQ(double x, double y);
        void  setT(int t);
        void  jixiao(double &rad);
};
#endif // RGSN_H
