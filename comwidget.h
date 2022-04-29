#ifndef COMWIDGET_H
#define COMWIDGET_H

#include <QWidget>
#include<memory>
#include<QDateTime>
#include"socktool.h"
using namespace  std;
namespace Ui {
class comwidget;
}

class comwidget : public QWidget
{
    Q_OBJECT

public:
    explicit comwidget(int fd,string fromClassNum,string toClassNum,string fromAccount,string toAccount,QWidget *parent = nullptr);

    void initChatDialog();
    ~comwidget();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void getChatHistory();
    void on_closeWidget_clicked();
    void on_sendMessage_clicked();

private:
    QTimer *timer;
    shared_ptr<int> m_sockFd;
    string m_fromClassNum;
    string m_toClassNum;
    string m_fromAccount;
    string m_toAccount;
    int m_lastUpdateTime;
    bool m_sendHeadFlag;
    bool m_historyHeadFlag;
    Ui::comwidget *ui;
};

#endif // COMWIDGET_H
