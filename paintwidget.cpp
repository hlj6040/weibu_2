#include "paintwidget.h"


PaintWidget::PaintWidget(QWidget *parent) :
    QWidget(parent)
{
    image = QImage(800,700,QImage::Format_RGB32);
    backColor = qRgb(255,255,255);
    image.fill(backColor);
}

void PaintWidget :: paintEvent(QPaintEvent *){

    QPainter painter(this);
    painter.drawImage(0,0,image);
}



void PaintWidget :: Paint (QImage &theImage){
    image = theImage;
    update();
}
