#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_ok_2_clicked();
    void on_cancel_2_clicked();

signals:
    void  add(QString ip, int port);

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
