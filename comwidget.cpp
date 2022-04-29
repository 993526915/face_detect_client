#include "comwidget.h"
#include "ui_comwidget.h"
#include<QDebug>
#include<QTimer>
#include<QScrollBar>
comwidget::comwidget(int fd,string fromClassNum,string toClassNum,string fromAccount,string toAccount, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::comwidget)
{
    ui->setupUi(this);
    m_sockFd = make_shared<int>(fd);
    m_fromAccount =fromAccount;
    m_fromClassNum = fromClassNum;
    m_toAccount = toAccount;
    m_toClassNum = toClassNum;
    m_sendHeadFlag = false;
    m_historyHeadFlag = false;
    m_lastUpdateTime = 0;
    ui->readContent->clearHistory();


    ui->readContent->setStyleSheet("QListWidget{background-color: rgb(245, 249, 253);border:0px;}"
                                  "QListWidget{outline:0px;}"
                                  "QListWidget::item{background-color: rgb(245, 249, 253);}");
    ui->readContent->verticalScrollBar()->setStyleSheet("QScrollArea{border:0px solid;"
                                                       "border-radius: 5px;}"
                                                       "QScrollBar:vertical{width: 10px;border:0px solid;"
                                                       "border-radius: 5px;margin: 0px,0px,0px,0px;"
                                                       "background-color: transparent;background:#e1f0fa;}"
                                                       "QScrollBar:vertical:hover{width: 10px;"
                                                       "border:0px solid;margin: 0px,0px,0px,0px;background:#e1f0fa;}"
                                                       "QScrollBar::handle:vertical{width:10px;background:#c0ddee;"
                                                       "border-radius: 5px;height: 40px;}"
                                                       "QScrollBar::handle:vertical:hover{background:#c0ddee;"
                                                       "border-radius: 5px;}"
                                                       "QScrollBar::add-line:vertical{height:11px;background-color: transparent;"
                                                       "subcontrol-position:bottom;border:0px solid;"
                                                       "border-radius: 5px;}"
                                                       "QScrollBar::sub-line:vertical{height:11px;"
                                                       "background-color: transparent;subcontrol-position:top;"
                                                       "border:0px solid;border-radius: 5px;}"
                                                       "QScrollBar::add-page:vertical{background-color: #e1f0fa;"
                                                       "border:0px solid;border-radius: 5px;}"
                                                       "QScrollBar::sub-page:vertical{background-color: #e1f0fa;"
                                                       "border:0px solid;border-radius: 5px;}"
                                                       "QScrollBar::up-arrow:vertical{"
                                                       "border:0px solid;border-radius: 3px;}"
                                                       "QScrollBar::down-arrow:vertical {"
                                                       "border:0px solid;border-radius: 3px;}");

    getChatHistory();


    timer =new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(getChatHistory()));
    timer->start(1000);
}

void comwidget::getChatHistory()
{
    //ui->readContent->clear();
    messageHistoryData historyData;
    memcpy(historyData.fromClassNum,m_fromClassNum.c_str(),m_fromClassNum.length());
    memcpy(historyData.fromAccount,m_fromAccount.c_str(),m_fromAccount.length());
    memcpy(historyData.toClassNum,m_toClassNum.c_str(),m_toClassNum.length());
    memcpy(historyData.toAccount,m_toAccount.c_str(),m_toAccount.length());
    historyData.lastupdatetime=m_lastUpdateTime;
    SockUtil::co_write(*m_sockFd.get(),&historyData,historyData.m_dataLength);

    messageHistoryCountResData historyCountRes;
    SockUtil::co_read(*m_sockFd.get(),&historyCountRes,historyCountRes.m_dataLength);
    if(historyCountRes.res==MESSAGE_HISTORY_SUCCESS)
    {
        int messageCount = historyCountRes.m_count;
        for(int i=0;i<messageCount;i++)
        {
            messageSendData messageData;
            SockUtil::co_read(*m_sockFd.get(),&messageData,messageData.m_dataLength);
            string fromClassNum = messageData.fromClassNum;
            string fromAccount = messageData.fromAccount;
            string toClassNum = messageData.toClassNum;
            string toAccount = messageData.toAccount;
            time_t t = messageData.updatetime;
            bool test;
            if(fromClassNum == m_fromClassNum && fromAccount ==m_fromAccount)
            {
                QColor color;
                color.setRgb(78,154,6);
                ui->readContent->setTextColor(color);
                test = true;
            }
            else
            {
                ui->readContent->setTextColor(Qt::blue);
                m_sendHeadFlag = false;
                test = false;
            }
            if(m_historyHeadFlag ^ test == 1)
            {
                char* curr_time = ctime(&t);
                QString dateTime(curr_time);
                string messageHead;
                if(test)
                {
                    messageHead = m_fromClassNum +" : " +m_fromAccount +" -----> " +
                            m_toClassNum+" : "+m_toAccount + "  " +dateTime.toStdString();
                }
                else
                {
                    messageHead = m_toClassNum +" : " +m_toAccount +" -----> " +
                            m_fromClassNum+" : "+m_fromAccount + "  " +dateTime.toStdString();
                }
                messageHead.pop_back();
                ui->readContent->append(QString(messageHead.c_str()));
                m_historyHeadFlag = !m_historyHeadFlag;
            }
            QString messageBody = "> " + QString(messageData.messageContent);
            ui->readContent->append(messageBody);
            m_lastUpdateTime = (int)time(NULL);
            messageSendRes sendRes;
            sendRes.res = MESSAGE_SEND_SUCCESS;
            SockUtil::co_write(*m_sockFd.get(),&sendRes,sendRes.m_dataLength);
        }

    }
    else
    {
        cout << "no history!" <<endl;
    }

}

void comwidget::initChatDialog()
{

}

comwidget::~comwidget()
{
    delete ui;
}

void comwidget::on_closeWidget_clicked()
{
    this->close();
}

void comwidget::on_sendMessage_clicked()
{
    if(ui->sendContent->toPlainText()=="") return ;
    QColor color;
    color.setRgb(78,154,6);
    ui->readContent->setTextColor(color);
    
    
    string content = ui->sendContent->toPlainText().toStdString();
    messageSendData sendData;
    memcpy(sendData.fromClassNum,m_fromClassNum.c_str(),m_fromClassNum.length());
    memcpy(sendData.fromAccount,m_fromAccount.c_str(),m_fromAccount.length());
    memcpy(sendData.toClassNum,m_toClassNum.c_str(),m_toClassNum.length());
    memcpy(sendData.toAccount,m_toAccount.c_str(),m_toAccount.length());
    memcpy(sendData.messageContent,content.c_str(),content.length());
    SockUtil::co_write(*m_sockFd.get(),&sendData,sendData.m_dataLength);
    
    messageSendRes sendres;
    SockUtil::co_read(*m_sockFd.get(),&sendres,sendres.m_dataLength);
    ui->sendContent->clear();
    //getChatHistory();
}
void comwidget::closeEvent(QCloseEvent * event)
{
    timer->stop();
    delete timer;
    qDebug() << "closeEvent";

}
