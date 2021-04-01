#ifndef TCPCONN_H
#define TCPCONN_H

#include <vector>
#include <cmath>
#include <time.h>
#include <string>

using namespace std;
#define MARK_LEN  4
#define FLOAT_LEN  4
#define FLOAT_MAX 999999999999.99999


//求两点经纬度的距离 和 方向角
class  Bearing
{
    const double PI = 3.1415926;
    const double R = 6371000;  //单位米
public:
    typedef  unsigned char BYTE;
    void FillSendSenWithFloats(BYTE* pcSenToSend, float* pFloatArray, int iFloatNum);
    void recvFloat(BYTE* pcSenToSend, float* pFloatArray, int iFloatNum);
    void FloatToByte(float fValue, BYTE* arrByte);
    float  ByteToFloat(BYTE* arrByte);
    double Radians(double x);

    //lat  纬度  lon 经度  计算方向角(指北经线起顺时针的夹角)
    double ComplexF(double lat1, double lon1, double lat2, double lon2);

    //计算距离
    double ComplexG(double lat1, double lon1, double lat2, double lon2);
};
#endif // TCPCONN_H
