#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class PaintWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaintWidget(QWidget *parent = 0);
    QImage image;
    QRgb backColor;
    void Paint(QImage &theImage);
public  slots:


protected:
    void paintEvent(QPaintEvent *);

signals:

private:


};


#endif // PAINTWIDGET_H
