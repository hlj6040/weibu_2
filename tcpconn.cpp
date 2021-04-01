#include "tcpconn.h"
#include <QtDebug>

double Bearing::Radians(double x){
        return x*PI/180;
    }

//lat  纬度  lon 经度  计算方向角(指北经线起顺时针的夹角)
double Bearing::ComplexF(double lat1, double lon1, double lat2, double lon2)
{
    double numerator = sin(Radians(lon2 - lon1)) * cos(Radians(lat2));
    double denominator = cos(Radians(lat1)) * sin(Radians(lat2)) - sin(Radians(lat1)) * cos(Radians(lat2)) * cos(Radians(lon2 - lon1));

    double x = atan2(abs(numerator), abs(denominator));
    double result = x;

    if (lon2 > lon1) // right quadrant
    {
        if (lat2 > lat1) // first quadrant
            result = x;
        else if (lat2 < lat1) // forth quadrant
            result = PI - x;
        else
            result = PI / 2; // in positive-x axis
    }
    else if (lon2 < lon1) // left quadrant
    {
        if (lat2 > lat1) // second quadrant
            result = 2 * PI - x;
        else if (lat2 < lat1) // third quadrant
            result = PI + x;
        else
            result = PI * 3 / 2; // in negative-x axis
    }
    else // same longitude
    {
        if (lat2 > lat1) // in positive-y axis
            result = 0;
        else if (lat2 < lat1)
            result = PI; // in negative-y axis


    }

    return result * 180 / PI;
}

//计算距离
double Bearing::ComplexG(double lat1, double lon1, double lat2, double lon2)
{
    double  havLat = sin(Radians(lat1 - lat2) / 2);
    double  havLon = sin(Radians(lon1 - lon2) / 2);

    double a = havLat * havLat + cos(Radians(lat1)) * cos(Radians(lat2)) * havLon * havLon;

    return 2 * R * atan2(sqrt(a), sqrt(1 - a));
}

void Bearing::FillSendSenWithFloats(BYTE* pcSenToSend, float* pFloatArray, int iFloatNum)
{
    if (NULL == pcSenToSend || NULL == pFloatArray || iFloatNum <= 0)
    {
        qDebug() << "ERROR CALLED!";
        return;
    }

    int iFltStartPos = MARK_LEN + 1;

    int byteIndex = iFltStartPos;

    for (int iFltIndex = 0; iFltIndex < iFloatNum; iFltIndex++)
    {
        float fi = pFloatArray[iFltIndex];

        BYTE cFltBytes[FLOAT_LEN] = { 0, 0, 0, 0 }; //用四字节表示的浮点数

        FloatToByte(fi, cFltBytes);

        for (int j = 0; j < FLOAT_LEN; j++)
        {
            pcSenToSend[iFltStartPos + j] = cFltBytes[j];

            byteIndex = iFltStartPos + j;
        }

        iFltStartPos += FLOAT_LEN;
    }

    //两个校验位
    pcSenToSend[byteIndex + 1] = '*';
    pcSenToSend[byteIndex + 2] = '*';
    //一个结束标志位
    pcSenToSend[byteIndex + 3] = '#';

}
void Bearing::recvFloat(BYTE* pcSenToSend, float* pFloatArray, int iFloatNum)
{
    if (NULL == pcSenToSend || NULL == pFloatArray || iFloatNum <= 0)
    {
        qDebug() << "ERROR CALLED!";
        return;
    }

    int iFltStartPos = MARK_LEN + 1;


    for (int iFltIndex = 0; iFltIndex < iFloatNum; iFltIndex++)
    {
        float fi;
        BYTE cFltBytes[FLOAT_LEN] = {0,0,0,0}; //用四字节表示的浮点
        for (int j = 0; j < FLOAT_LEN; j++)
        {
            cFltBytes[j] = pcSenToSend[iFltStartPos + j];
        }
        iFltStartPos += FLOAT_LEN;
        fi = ByteToFloat(cFltBytes);
        pFloatArray[iFltIndex] = fi;
    }
}


void Bearing::FloatToByte(float fValue, BYTE* arrByte)
{
    BYTE *pf, *px;
    BYTE i;
    pf = (BYTE*)&fValue;
    px = arrByte;
    for (i = 0; i<4; i++)
    {
        *(px + i) = *(pf + i);
    }
}



//把接收到的数据转化为浮点数的例子
/*
接收无人艇经纬度、速度、航向角解析代码示例：
*/
float  Bearing::ByteToFloat(BYTE* arrByte)
{
    float fRet = *((float*)arrByte);
    return fRet;
}


