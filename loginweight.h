#ifndef LOGINWEIGHT_H
#define LOGINWEIGHT_H

#include <QWidget>
#include<QDebug>
#include<QTcpServer>
#include<memory>
#include<iostream>
#include<registor.h>
#include"socktool.h"
#include "registor.h"
#include"detectweight.h"

using namespace  std;
namespace Ui {
class loginWeight;
}

class loginWeight : public QWidget
{
    Q_OBJECT
public:
    explicit loginWeight(QWidget *parent = nullptr);
    ~loginWeight();
    int initSocket();

private slots:
    void on_Login_clicked();

    void on_registe_clicked();

private:
    Ui::loginWeight *ui;
    shared_ptr<int> m_socketFd;
};

#endif // LOGINWEIGHT_H
