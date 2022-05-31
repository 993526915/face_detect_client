#ifndef USERWEIGHT_H
#define USERWEIGHT_H

#include <QWidget>
#include<QString>
#include<string>
#include<vector>
#include<memory>
#include<QTreeWidgetItem>
#include<jsoncpp/json/json.h>
#include"faceDetect/facedetect.h"
#include"initparams.h"
#include"socktool.h"
#include"comwidget.h"
#include<QListWidget>
#include<QList>
#include<QLabel>
#include<QDebug>
#include"inform.h"
#include"DBpool.h"
using namespace  std;
namespace Ui {
class userweight;
}

class userweight : public QWidget
{
    Q_OBJECT

public:
    explicit userweight(QString classnum ,QString account,QWidget *parent = nullptr);

    void initUserTreeWidget();
    void sendOnline();
    int initSocket();
    ~userweight();
private slots:
    void mousePressEvent(QMouseEvent *event);
    void changeTouXiang(string touxiangPath);
    void slotDoubleClickItem(QTreeWidgetItem *item, int col);
    void checkOnline();
    void listWidgetDoubleClicked(QModelIndex pos);
    void initMessageTreeWidget();
protected :
    void closeEvent(QCloseEvent *event);
private:
    void _insertTreeWidget(string account,string touxiang,QTreeWidgetItem *thisItem);
    void _insertListWidget(QString userInfo,QString Msg,QString time,QString touxiang);
    Ui::userweight *ui;
    inform *m_inform;
    string m_classNum;
    string m_account;
    string m_touXiangPath;
    shared_ptr<int> m_sock;
};

#endif // USERWEIGHT_H
