#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_ok_2_clicked()
{
    QString ip = ui->ip_2->text();
    int port  = ui->port_2->text().toInt();
    emit add(ip,port);
}

void Dialog::on_cancel_2_clicked()
{
    this->close();
}
