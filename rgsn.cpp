#include "rgsn.h"
#include <QDebug>
RGSN::RGSN(Coordinate qg, Coordinate q, vector<Coordinate>  q0_List){
    this->qg = qg;
    this->q = q;
    this->q0_List = q0_List;

}
void  RGSN::setGGC(int  GGC){
    this->GGC = GGC;
}
void  RGSN::setPGC(int  PGC){
    this->PGC = PGC;
}

void  RGSN::setStep(int  step){
    this->step = step;
}
void  RGSN::setBorder(int Border){
    this->BORDER = Border;
}
void  RGSN::setQ(double x, double y){
    q.first = x;
    q.second = y;
}
void  RGSN::setQg(double x, double y){
    qg.first = x;
    qg.second = y;
}
void  RGSN::setN(int n){
    this->n = n;
}
void RGSN::setT(int t){
    this->T = t;
}
double  RGSN::jili_pow(Coordinate  a, Coordinate b){
    double x = abs(a.first-b.first),
           y = abs(a.second-b.second);
    int k = 1;
    while(x >= 10 || y >= 10){
        k*=10;
        x/=10;
        y/=10;
    }
    return (sqrt(x*x+y*y)*k);
}
Coordinate  RGSN::getG(){
    Coordinate  t;
    double  rad = atan2((q.second-qg.second)*-1,(q.first-qg.first)*-1);
    double y = abs(q.second-qg.second),
           x = abs(q.first-qg.first);
    int k = 1;
    while(x >= 10 || y>=10){
        k*=10;
        x/=10;
        y/=10;
    }
    double len = sqrt(x*x+y*y)*k;
    t.first = len*cos(rad)*GGC;
    t.second = len*sin(rad)*GGC;

    return  t;
}
Coordinate RGSN::getP(int i){
    Coordinate  t, q0 = this->q0_List[i];
    double x = q.first-q0.first,
           y = q.second-q0.second;
    double rad = atan2(y,x);

    x = abs(x), y = abs(y);
    int k = 1;
    while(x >= 10|| y >= 10){
        k*=10;
        x/=10;
        y/=10;
    }
    double len = sqrt(x*x+y*y)*k;

    //  距离大于障碍的距离斥力为0
    if(len > this->BORDER){
        t.first = 0;
        t.second = 0;
    }
    else{
        double  xg = this->qg.first - q.first,
                yg = this->qg.second - q.second;
        double radg = atan2(yg,xg);

        xg = abs(xg), yg = abs(yg);
        int kg = 1;
        while(xg > 10|| yg > 10){
            kg*=10;
            xg/=10;
            yg/=10;
        }
        double leng = sqrt(xg*xg + yg*yg) * kg;

        //公式求解斥力分量
        //障碍物指向机器人的斥力分量
        double f = PGC*(1/len-1/this->BORDER)*pow(leng,n)/(len*len);
        t.first = f*cos(rad),   t.second = f*sin(rad);

        //机器人指向目标的引力分量
        f = n/2*PGC*pow((1/len-1/this->BORDER),2)*pow(leng,n-1);
        t.first += f*cos(radg);  t.second += f*sin(radg);

    }

    return  t;
}
float  RGSN::Run(){

        vector<Coordinate> F;

        F.push_back(getG());
        vector<Coordinate>  q0_list = this->q0_List;
        for(unsigned int i = 0; i < q0_list.size(); i++){
            Coordinate  k = getP(i);
            F.push_back(k);
        }

        //合力方向
        Coordinate  FF = {0,0};
        for(unsigned int i = 0; i < F.size(); i++){
            FF.first += F[i].first;
            FF.second  += F[i].second;
        }

        double rad = atan2(FF.second, FF.first);

        jixiao(rad);
        this->q.first  +=  step*cos(rad);
        this->q.second  +=  step*sin(rad);
        return  rad;

}

void RGSN::jixiao(double &rad){
    for(unsigned  int i = 0; i < q0_List.size(); i++){
        int len = jili_pow(q,q0_List[i]);
        if(len < this->BORDER*3/4){
            rad = atan2(q0_List[i].second-qg.second,q0_List[i].first-qg.first)+(3.1415926/4);
        }
        if(len < this->BORDER/2){
            rad = atan2(q0_List[i].second-qg.second,q0_List[i].first-qg.first)-(3.1415926/4);
            return;
        }
    }
}

