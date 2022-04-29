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
    void slotDoubleClickItem(QTreeWidgetItem *item, int col);
    void checkOnline();
    void listWidgetDoubleClicked(QModelIndex pos);
    void initMessageTreeWidget();
protected :
    void closeEvent(QCloseEvent *event);
private:
    void _insertTreeWidget(string account,string touxiang,QTreeWidgetItem *thisItem);
    void _insertListWidget(QString userInfo,QString Msg,QString time);
    Ui::userweight *ui;
    string m_classNum;
    string m_account;
    shared_ptr<faceDetect> m_faceDetect;
    shared_ptr<initParams> m_params;
    shared_ptr<int> m_sock;
};

#endif // USERWEIGHT_H
