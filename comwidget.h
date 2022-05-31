#ifndef COMWIDGET_H
#define COMWIDGET_H

#include <QWidget>
#include<memory>
#include<QDateTime>
#include"socktool.h"
#include<qnchatmessage.h>
#include<QListWidgetItem>
using namespace  std;
namespace Ui {
class comwidget;
}

class comwidget : public QWidget
{
    Q_OBJECT

public:
    explicit comwidget(int fd,string fromClassNum,string toClassNum,string fromAccount,string toAccount,string sendPic,string recvPic,QWidget *parent = nullptr);

    void initChatDialog();
    ~comwidget();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void getChatHistory();
    void on_closeWidget_clicked();
    void on_sendMessage_clicked();

    void on_transform_clicked();

private:
    void _dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, time_t time,  QNChatMessage::User_Type type);
    void _dealMessageTime(time_t curMsgTime);
    void _insertMsg(QString text,time_t time,QString leftPic,QString rightPic,QNChatMessage::User_Type type);
    QTimer *timer;
    shared_ptr<int> m_sockFd;
    string m_fromClassNum;
    string m_toClassNum;
    string m_fromAccount;
    string m_toAccount;
    string m_sendPic;
    string m_recvPic;
    int m_lastUpdateTime;
    bool m_sendHeadFlag;
    bool m_historyHeadFlag;
    Ui::comwidget *ui;
};

#endif // COMWIDGET_H
