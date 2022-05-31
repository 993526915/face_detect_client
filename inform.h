#ifndef INFORM_H
#define INFORM_H

#include <QDialog>
#include<QMouseEvent>
#include<QMessageBox>
#include"socktool.h"
#include"DBpool.h"
namespace Ui {
class inform;
}

class inform : public QDialog
{
    Q_OBJECT

public:
    explicit inform(int fd,string classNum ,string account,QWidget *parent = nullptr);
    ~inform();
signals:
    void touXiangPath(string touxiangPath);
private slots:
    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);
    void on_changeTouXiang_clicked();

    void on_Minimize_clicked();

    void on_close_clicked();

    void on_pic_1_clicked();

    void on_pic_2_clicked();

    void on_pic_3_clicked();

    void on_pic_4_clicked();

    void on_pic_5_clicked();

    void on_pic_6_clicked();

    void on_pic_7_clicked();

    void on_pic_8_clicked();

    void on_pic_9_clicked();

    void on_pic_10_clicked();

    void on_pic_11_clicked();

    void on_pic_12_clicked();

    void on_pic_13_clicked();

    void on_pic_14_clicked();

    void on_pic_15_clicked();

    void on_pic_16_clicked();

    void on_pic_17_clicked();

    void on_pic_18_clicked();

    void on_pic_19_clicked();

    void on_pic_20_clicked();

private:
    Ui::inform *ui;
    bool     m_pressed;
    QPoint   m_movePos;
    string m_classNum;
    string m_account;
    int m_sock;
};

#endif // INFORM_H
