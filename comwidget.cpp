#include "comwidget.h"
#include "ui_comwidget.h"
#include<QDebug>
#include<QTimer>
#include<QScrollBar>

void comwidget::_dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, time_t time,  QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(ui->listWidget->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type);
    ui->listWidget->setItemWidget(item, messageW);
}

void comwidget::_dealMessageTime(time_t curMsgTime)
{
    bool isShowTime = false;
    if(ui->listWidget->count() > 0)
    {
        QListWidgetItem* lastItem = ui->listWidget->item(ui->listWidget->count() - 1);
        QNChatMessage* messageW = qobject_cast<QNChatMessage*>(ui->listWidget->itemWidget(lastItem));
        int lastTime = messageW->time();
        int curTime = curMsgTime;
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
//        isShowTime = true;
    }
    else
    {
        isShowTime = true;
    }
    if(isShowTime)
    {
        QNChatMessage* messageTime = new QNChatMessage(ui->listWidget->parentWidget(),":/icon/Me.png",":/icon/Me.png");
        QListWidgetItem* itemTime = new QListWidgetItem(ui->listWidget);
        QSize size = QSize(ui->listWidget->width(), 40);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        char *currentTime = ctime(&curMsgTime);
        messageTime->setText(QString(currentTime), curMsgTime, size, QNChatMessage::User_Time);
        ui->listWidget->setItemWidget(itemTime, messageTime);
    }

}

void comwidget::_insertMsg(QString text,time_t time,QString leftPic,QString rightPic,QNChatMessage::User_Type type)
{
    QNChatMessage* messageW = new QNChatMessage(ui->listWidget->parentWidget(),leftPic,rightPic);
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    _dealMessage(messageW, item,text, time, type);
}
comwidget::comwidget(int fd,string fromClassNum,string toClassNum,string fromAccount,string toAccount,string sendPic,string recvPic, QWidget *parent) :
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
    m_sendPic = sendPic;
    m_recvPic = recvPic;
    ui->readContent->clearHistory();

    initChatDialog();
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
            QString message = messageData.messageContent;


            bool test;
            if(fromClassNum == m_fromClassNum && fromAccount ==m_fromAccount)
            {
                QColor color;
                color.setRgb(78,154,6);
                ui->readContent->setTextColor(color);
                test = true;
                _dealMessageTime(t);
                _insertMsg(message,t,QString(m_sendPic.c_str()),QString(m_recvPic.c_str()),QNChatMessage::User_Me);
            }
            else
            {
                ui->readContent->setTextColor(Qt::blue);
                m_sendHeadFlag = false;
                test = false;
                _dealMessageTime(t);
                _insertMsg(message,t,QString(m_sendPic.c_str()),QString(m_recvPic.c_str()),QNChatMessage::User_She);
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
//            messageSendRes sendRes;
//            sendRes.res = MESSAGE_SEND_SUCCESS;
//            SockUtil::co_write(*m_sockFd.get(),&sendRes,sendRes.m_dataLength);
        }

    }
    else
    {
        cout << "no history!" <<endl;
    }

}

void comwidget::initChatDialog()
{
    
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
    ui->listWidget->setStyleSheet("QListWidget{background-color: rgb(245, 249, 253);border:0px;}"
                                  "QListWidget{outline:0px;}"
                                  "QListWidget::item{background-color: rgb(245, 249, 253);}");
    ui->listWidget->verticalScrollBar()->setStyleSheet("QScrollArea{border:0px solid;"
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
    
//    messageSendRes sendres;
//    SockUtil::co_read(*m_sockFd.get(),&sendres,sendres.m_dataLength);
    ui->sendContent->clear();
    //getChatHistory();
}
void comwidget::closeEvent(QCloseEvent * event)
{
    timer->stop();
    delete timer;
    qDebug() << "closeEvent";

}

void comwidget::on_transform_clicked()
{
    if(ui->listWidget->isHidden())
    {
        ui->listWidget->setHidden(false);
    }
    else
    {
        ui->listWidget->setHidden(true);
    }
}
