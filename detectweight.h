#ifndef DETECTWEIGHT_H
#define DETECTWEIGHT_H

#include <QWidget>
#include<QTimer>
#include<QPixmap>
#include<QPainter>
#include<QDateTime>
#include"logindialog.h"
#include"userweight.h"
#include<QMessageBox>
#include"procomimg.h"
namespace Ui {
class detectweight;
}

class detectweight : public QWidget
{
    Q_OBJECT

public:
    explicit detectweight(string classnum,string account,shared_ptr<int> fd,QWidget *parent = nullptr);
    ~detectweight();
protected:
    void paintEvent(QPaintEvent *);
private slots:
    void showUp(QPixmap pixMap);
    void timerUpdate(); //实时更新时间
    void detectSuccess();
    void detectFailed();
private:
    Ui::detectweight *ui;
    shared_ptr<proComImg> m_proComImg;
    shared_ptr<comsumePro> m_comThread;
    shared_ptr<producePro> m_proThread;
    shared_ptr<detectPro> m_detectThread;
    shared_ptr<PrintQPixMap> m_printPixMap;
    QPixmap m_picture;
    shared_ptr<int> m_sockFd;
    string m_classnum;
    string m_account;
};

#endif // DETECTWEIGHT_H
