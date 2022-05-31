#include "userweight.h"
#include "ui_userweight.h"
#include<QMessageBox>
#include<QTimer>
#include<QLineEdit>
#include<QScrollBar>
#include<qnchatmessage.h>
userweight::userweight(QString classnum,QString account,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userweight)
{
    ui->setupUi(this);
    m_classNum = classnum.toStdString();
    m_account = account.toStdString();

    this->setWindowTitle(account);
    ui->name->setText(account);
    ui->userTreeWidget->headerItem()->setHidden(true);
    int fd = SockUtil::co_createSocket();
    m_sock = make_shared<int>(fd);
    if(!initSocket())
    {
        cout << "网络错误" << endl;
        SockUtil::co_offline_close(*m_sock.get(),m_classNum,m_account);
        return ;
    }


    getTouXiangData touxiang;
    memcpy(touxiang.m_classNum,m_classNum.c_str(),m_classNum.length());
    memcpy(touxiang.m_account,m_account.c_str(),m_account.length());
    SockUtil::co_write(*m_sock.get(),&touxiang,touxiang.m_dataLength);

    getTouXiangRes touxiangRes;
    SockUtil::co_read(*m_sock.get(),&touxiangRes,touxiangRes.m_dataLength);
    ui->pic->setPixmap(QPixmap(touxiangRes.m_touxiang));
    m_touXiangPath = touxiangRes.m_touxiang;

    ui->userTreeWidget->setFrameShape(QListWidget::NoFrame);

    ui->userTreeWidget->setStyleSheet("QTreeWidget{background:rgb(238, 247, 255);}"
                                  "QTreeWidget::item:hover{background-color:rgb(200, 230, 250);}"
                                  "QTreeWidget::item:selected{background-color:rgb(170, 210, 250);}"
                                  "QTreeWidget{outline:0px;}"
                                  "QTreeWidget::item{background-color:rgb(225, 240, 250);}");
    ui->userTreeWidget->verticalScrollBar()->setStyleSheet("QScrollArea{border:0px solid;"
                                                       "border-radius: 2px;}"
                                                       "QScrollBar:vertical{width: 4px;border:0px solid;"
                                                       "border-radius: 2px;margin: 0px,0px,0px,0px;"
                                                       "background-color: transparent;background:#e1f0fa;}"
                                                       "QScrollBar:vertical:hover{width: 8px;"
                                                       "border:0px solid;margin: 0px,0px,0px,0px;background:#e1f0fa;}"
                                                       "QScrollBar::handle:vertical{width:8px;background:#c0ddee;"
                                                       "border-radius: 8px;height: 80px;}"
                                                       "QScrollBar::handle:vertical:hover{background:#c0ddee;"
                                                       "border-radius: 8px;}"
                                                       "QScrollBar::add-line:vertical{height:11px;background-color: transparent;"
                                                       "subcontrol-position:bottom;border:0px solid;"
                                                       "border-radius: 8px;}"
                                                       "QScrollBar::sub-line:vertical{height:11px;"
                                                       "background-color: transparent;subcontrol-position:top;"
                                                       "border:0px solid;border-radius: 2px;}"
                                                       "QScrollBar::add-page:vertical{background-color: #e1f0fa;"
                                                       "border:0px solid;border-radius: 2px;}"
                                                       "QScrollBar::sub-page:vertical{background-color: #e1f0fa;"
                                                       "border:0px solid;border-radius: 2px;}"
                                                       "QScrollBar::up-arrow:vertical{"
                                                       "border:0px solid;border-radius: 3px;}"
                                                       "QScrollBar::down-arrow:vertical {"
                                                       "border:0px solid;border-radius: 3px;}");

    ui->messageListWidget->setStyleSheet("QListWidget{background:rgb(238, 247, 255);border:0px;}"
                                  "QListWidget::item:hover{background-color:rgb(200, 230, 250);}"
                                  "QListWidget::item:selected{background-color:rgb(170, 210, 250);}"
                                  "QListWidget{outline:0px;}"
                                  "QListWidget::item{background-color:rgb(225, 240, 250);}");
    ui->messageListWidget->verticalScrollBar()->setStyleSheet("QScrollArea{border:0px solid;"
                                                       "border-radius: 2px;}"
                                                       "QScrollBar:vertical{width: 4px;border:0px solid;"
                                                       "border-radius: 2px;margin: 0px,0px,0px,0px;"
                                                       "background-color: transparent;background:#e1f0fa;}"
                                                       "QScrollBar:vertical:hover{width: 4px;"
                                                       "border:0px solid;margin: 0px,0px,0px,0px;background:#e1f0fa;}"
                                                       "QScrollBar::handle:vertical{width:4px;background:#c0ddee;"
                                                       "border-radius: 2px;height: 40px;}"
                                                       "QScrollBar::handle:vertical:hover{background:#c0ddee;"
                                                       "border-radius: 2px;}"
                                                       "QScrollBar::add-line:vertical{height:11px;background-color: transparent;"
                                                       "subcontrol-position:bottom;border:0px solid;"
                                                       "border-radius: 2px;}"
                                                       "QScrollBar::sub-line:vertical{height:11px;"
                                                       "background-color: transparent;subcontrol-position:top;"
                                                       "border:0px solid;border-radius: 2px;}"
                                                       "QScrollBar::add-page:vertical{background-color: #e1f0fa;"
                                                       "border:0px solid;border-radius: 2px;}"
                                                       "QScrollBar::sub-page:vertical{background-color: #e1f0fa;"
                                                       "border:0px solid;border-radius: 2px;}"
                                                       "QScrollBar::up-arrow:vertical{"
                                                       "border:0px solid;border-radius: 3px;}"
                                                       "QScrollBar::down-arrow:vertical {"
                                                       "border:0px solid;border-radius: 3px;}");

    void(userweight::*doubleClicked)(QModelIndex pos) = &userweight::listWidgetDoubleClicked;
    connect(ui->messageListWidget,&QListWidget::doubleClicked,this,doubleClicked);

    connect(ui->userTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this,SLOT(slotDoubleClickItem(QTreeWidgetItem* ,int)));

    m_inform =  new inform(*m_sock.get(),m_classNum,m_account);
    void(inform::*Touxiang)(string touXiangPath) = &inform::touXiangPath;
    connect(userweight::m_inform,Touxiang,this,&userweight::changeTouXiang);

    sendOnline();
    initUserTreeWidget();
    initMessageTreeWidget();

    QTimer *timer =new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(initMessageTreeWidget()));
    timer->start(1000);

    QTimer *timerOnline =new QTimer(this);
    connect(timerOnline,SIGNAL(timeout()),this,SLOT(checkOnline()));
    timerOnline->start(5000);

}
void userweight::_insertTreeWidget(string account,string touxiang,QTreeWidgetItem *thisItem)
{

    QWidget *widget=new QWidget;
    QLabel *touLabel = new QLabel(widget);
    touLabel->setObjectName("touLabel");
    QLabel *namelabel=new QLabel(widget);
    namelabel->setObjectName("namelabel");
    QLineEdit *idlinet=new QLineEdit(widget);
    idlinet->hide();
    //设置不同控件的样式
    widget->setStyleSheet(" background: transparent");
    namelabel->setFixedSize(180,60);
    namelabel->move(80,10);
    namelabel->setText(account.c_str());
    namelabel->setAlignment(Qt::AlignLeft);
    namelabel->setAlignment(Qt::AlignVCenter);
    QFont font("微软雅黑",16,50);
    namelabel->setFont(font);
    namelabel->setStyleSheet("color: rgb(70,70,70)");
    touLabel->setFixedSize(50,50);
    touLabel->move(5,15);
    QPixmap pixmap=QPixmap(touxiang.c_str());
    QPixmap fitpixmap = pixmap.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    touLabel->setPixmap(fitpixmap);
    touLabel-> setScaledContents(true);
    QSize size = thisItem->sizeHint(0);
    thisItem->setSizeHint(0,QSize(size.width(), 80));
    widget->setSizeIncrement(size.width(), 80);
    ui->userTreeWidget->setItemWidget(thisItem,0, widget);
}
void userweight::_insertListWidget(QString userInfo,QString Msg,QString time,QString touxiang)
{

    QWidget *widget=new QWidget;

    QLabel *touLabel = new QLabel(widget);
    touLabel->setObjectName("touLabel");

    QLabel *userInfolabel=new QLabel(widget);
    userInfolabel->setObjectName("userInfolabel");

    QLabel *timelabel=new QLabel(widget);
    timelabel->setObjectName("timelabel");

    QLabel *Msglabel=new QLabel(widget);
    Msglabel->setObjectName("Msglabel");

    widget->setStyleSheet(" background: transparent");


    //userInfo设置
    userInfolabel->setFixedSize(180,60);
    userInfolabel->move(80,0);
    userInfolabel->setText(userInfo);
    userInfolabel->setAlignment(Qt::AlignLeft);
    userInfolabel->setAlignment(Qt::AlignVCenter);
    QFont accountfont("微软雅黑",18,50);
    userInfolabel->setFont(accountfont);
    userInfolabel->setStyleSheet("color: rgb(170,70,70)");


    //time设置
    timelabel->setFixedSize(180,60);
    timelabel->move(300,0);
    timelabel->setText(time);
    timelabel->setAlignment(Qt::AlignLeft);
    timelabel->setAlignment(Qt::AlignVCenter);
    QFont timefont("微软雅黑",18,50);
    timelabel->setFont(timefont);
    timelabel->setStyleSheet("color: rgb(70,70,70)");

    //Msg设置
    Msglabel->setText(Msg);
    Msglabel->setFixedSize(180,60);
    Msglabel->move(80,30);
    Msglabel->setAlignment(Qt::AlignLeft);
    Msglabel->setAlignment(Qt::AlignVCenter);
    QFont classNumfont("微软雅黑",18,100);
    Msglabel->setFont(classNumfont);
    Msglabel->setStyleSheet("color: rgb(70,70,70);font-size:18px;");

    //头像设置
    touLabel->setFixedSize(60,60);
    touLabel->move(10,10);
    QPixmap pixmap=QPixmap(touxiang);
    QPixmap fitpixmap = pixmap.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    touLabel->setPixmap(fitpixmap);
    touLabel-> setScaledContents(true);


    QListWidgetItem *item=new QListWidgetItem;
    QSize size = item->sizeHint();
    item->setSizeHint(QSize(size.width(), 80));
    ui->messageListWidget->addItem(item);
    widget->setSizeIncrement(size.width(), 80);
    ui->messageListWidget->setItemWidget(item, widget);
}
void userweight::initMessageTreeWidget()
{
    ui->messageListWidget->clear();
    messageRecentSessionData recentSession;
    memcpy(recentSession.classNum,m_classNum.c_str(),m_classNum.length());
    memcpy(recentSession.account,m_account.c_str(),m_account.length());
    recentSession.lastupdatetime = (int)time(NULL);
    SockUtil::co_write(*m_sock.get(),&recentSession,recentSession.m_dataLength);
    messageRecentSessionResData sessionRes;
    SockUtil::co_read(*m_sock.get(),&sessionRes,sessionRes.m_dataLength);
    if(sessionRes.res == MESSAGR_RECENT_SESSION_SUCCESS)
    {
        for(int i=0;i<sessionRes.m_count;i++)
        {
            messageRecentSessionData temp;
            SockUtil::co_read(*m_sock.get(),&temp,temp.m_dataLength);
            time_t lastTime = temp.lastupdatetime;
            tm* curr_tm = localtime(&lastTime);
            char ntime[20];
            memset(ntime,0,sizeof (ntime));
            sprintf(ntime,"%02d : %02d",curr_tm->tm_hour,curr_tm->tm_min);
            QString userInfo = QString(temp.classNum) + ":"+QString(temp.account);
            //qDebug() << QString(ntime);
            QString peerTouXiang = temp.touxiang;
             _insertListWidget(userInfo,QString(temp.lastMessage),QString(ntime),peerTouXiang);
             messageRecentSessionResData sessionRes;
             sessionRes.m_count = 0;
             sessionRes.res = MESSAGR_RECENT_SESSION_SUCCESS;
             int t = SockUtil::co_write(*m_sock.get(),&sessionRes,sessionRes.m_dataLength);
             if(t < 0)
             {
                 qDebug() << "q";
             }
        }
    }
    else
    {
        qDebug() << "no session!" ;
    }
}

void userweight::listWidgetDoubleClicked(QModelIndex pos)
{
    int row = pos.row();
    QListWidgetItem *item = ui->messageListWidget->item(row);

    QWidget *t = ui->messageListWidget->itemWidget(item);

    QList<QLabel *> labelList = t->findChildren<QLabel *>();
    for(int i=0;i<labelList.size();i++)
    {
        if(labelList[i]->objectName() == "userInfolabel")
        {
            QString str = labelList[i]->text();
            QStringList l = str.split(':');
            string classNum = l[0].toStdString();
            string account = l[1].toStdString();
            getTouXiangData touxiang;
            memcpy(touxiang.m_account,account.c_str(),account.length());
            memcpy(touxiang.m_classNum,classNum.c_str(),classNum.length());
            SockUtil::co_write(*m_sock.get(),&touxiang,touxiang.m_dataLength);

            getTouXiangRes touxiangRes;
            SockUtil::co_read(*m_sock.get(),&touxiangRes,touxiangRes.m_dataLength);
            if(touxiangRes.m_res == GET_TOUXIANG_SUCCESS)
            {
                string sendTouXiangPath = touxiangRes.m_touxiang;
                comwidget * comm = new comwidget(*m_sock.get(),m_classNum,classNum,m_account,account,m_touXiangPath,sendTouXiangPath);
                string title = classNum+':'+account;
                comm->show();
                comm->setWindowTitle(QString(title.c_str()));
            }
            break;
        }
    }
}


void userweight::checkOnline()
{
    checkOnlineData check;
    SockUtil::co_write(*m_sock.get(),&check,check.m_dataLength);
    char ret[1024];
    memset(ret,0,sizeof(ret));
    SockUtil::co_read(*m_sock.get(),ret,sizeof(ret));
    Json::Value json_check_res;
    string str = ret;
    Json::Reader reader;
    //从字符串中读取数据
    if (reader.parse(str, json_check_res))
    {
        for(int i=0;i<json_check_res.size();i++)
        {
            Json::Value temp = json_check_res[i];
            string classnum = temp["classnum"].asString();
            string account = temp["account"].asString();
            int islogin = temp["islogin"].asInt();
            qDebug() << QString(classnum.c_str()) << "    " << QString(account.c_str()) << "    " << islogin;
            QList<QTreeWidgetItem *> L;
            L = ui->userTreeWidget->findItems(QString(classnum.c_str()),Qt::MatchExactly);
            for(int j=0; j<L.size();j++)
            {
                if(L[j]->text(0) == QString(account.c_str()) && islogin == 1)
                {
                    L[j]->setDisabled(false);
                }
                else
                {
                    L[j]->setDisabled(true);
                }
            }
        }
    }
}
void userweight::sendOnline()
{
    while(1)
    {
        onlineData online;
        memcpy(online.m_classnum,m_classNum.c_str(),m_classNum.length());
        memcpy(online.m_account,m_account.c_str(),m_account.length());

        SockUtil::co_write(*m_sock.get(),&online,online.m_dataLength);
        onlineResData onlineRes;
        SockUtil::co_read(*m_sock.get(),&onlineRes,onlineRes.m_dataLength);
        if(onlineRes.res == ONLINE_SUCCESS) break;
    }


}
int userweight::initSocket()
{
    if (SockUtil::co_connect(*m_sock.get(),"127.0.0.1",8888) == -1) {
        qDebug() << "Error connecting" ;
        return 0;

    }
    qDebug() << "connecting********************************" ;
    return 1;
}
void userweight::initUserTreeWidget()
{
    ui->userTreeWidget->clear();
    getGroupData *group = new getGroupData;
    SockUtil::co_write(*m_sock.get(),group,group->m_dataLength);

    char ret[1024];
    memset(ret,0,sizeof(ret));
    SockUtil::co_read(*m_sock.get(),ret,sizeof(ret));
    Json::Value json_group_res;
    string str = ret;
    Json::Reader reader;
    //从字符串中读取数据
    if (reader.parse(str, json_group_res))
    {
        Json::Value group_id_list = json_group_res["result"]["group_id_list"];
        for(int i=0;i<group_id_list.size();i++)
        {
            string group_id = group_id_list[i].asString();
            QTreeWidgetItem *groupItem = new QTreeWidgetItem(ui->userTreeWidget,QStringList(QString(group_id.c_str())));
            groupItem->setSizeHint(0,QSize(90,60));
            QFont font("微软雅黑",15,50);
            groupItem->setFont(0,font);
            getUserData *user = new getUserData;
            memcpy(user->m_group_id,group_id.c_str(),group_id.length());
            SockUtil::co_write(*m_sock.get(),user,user->m_dataLength);
            memset(ret,0,sizeof (ret));
            SockUtil::co_read(*m_sock.get(),ret,sizeof(ret));
            Json::Value json_user_res;
            str = ret;
            if (reader.parse(str, json_user_res))
            {
                Json::Value user_id_list = json_user_res["result"]["user_id_list"];
                for(int j=0;j<user_id_list.size();j++)
                {
                    string user_id = user_id_list[j]["account"].asString();
                    string touxiang = user_id_list[j]["touxiang"].asString();
                    QTreeWidgetItem *userItem = new QTreeWidgetItem(groupItem,QStringList(QString(user_id.c_str())));
                    userItem->setSizeHint(0,QSize(10,10));
                    //userItem->setIcon(0,QIcon(QString(":/icon/Me.png")));
                    _insertTreeWidget(user_id,touxiang,userItem);
                    groupItem->addChild(userItem);
                }
                ui->userTreeWidget->insertTopLevelItem(0,groupItem);
            }
        }
    }
}

void userweight::slotDoubleClickItem(QTreeWidgetItem *item, int col) {
    if(item->parent()==NULL) return ;
    string toClassNum = item->parent()->text(0).toStdString();
    string toAccount = item->text(0).toStdString();
    getTouXiangData touxiang;
    memcpy(touxiang.m_account,toAccount.c_str(),toAccount.length());
    memcpy(touxiang.m_classNum,toClassNum.c_str(),toClassNum.length());
    SockUtil::co_write(*m_sock.get(),&touxiang,touxiang.m_dataLength);

    getTouXiangRes touxiangRes;
    SockUtil::co_read(*m_sock.get(),&touxiangRes,touxiangRes.m_dataLength);
    if(touxiangRes.m_res == GET_TOUXIANG_SUCCESS)
    {
        string peerTouXiang = touxiangRes.m_touxiang;
        messageCreateData messageCreate;
        memcpy(messageCreate.m_fromclassnum,m_classNum.c_str(),m_classNum.length());
        memcpy(messageCreate.m_fromaccount,m_account.c_str(),m_account.length());
        memcpy(messageCreate.m_toclassnum,toClassNum.c_str(),toClassNum.length());
        memcpy(messageCreate.m_toaccount,toAccount.c_str(),toAccount.length());
        memcpy(messageCreate.m_peertouxiang,peerTouXiang.c_str(),peerTouXiang.length());
        SockUtil::co_write(*m_sock.get(),&messageCreate,messageCreate.m_dataLength);

        messageCreateResData *messageCreateRes = new messageCreateResData;
        SockUtil::co_read(*m_sock.get(),messageCreateRes,messageCreateRes->m_dataLength);
        if(messageCreateRes->res == MESSAGE_CREATE_SUCCESS)
        {
            comwidget *communicate = new comwidget(*m_sock.get(),m_classNum,toClassNum,m_account,toAccount,m_touXiangPath,peerTouXiang);
            communicate->show();
            communicate->setWindowTitle(item->text(0));
        }
        else
        {
            QMessageBox::warning(this, "Warning!", "创建对话失败", QMessageBox::Yes);
        }
    }

}
userweight::~userweight()
{
    SockUtil::co_offline_close(*m_sock.get(),m_classNum,m_account);
    this->close();
    delete ui;
}
void userweight::closeEvent(QCloseEvent * event)
{
    qDebug() << "closeEvent";
    SockUtil::co_offline_close(*m_sock.get(),m_classNum,m_account);
}
void userweight::mousePressEvent(QMouseEvent *event)
{

    if(event->x() > ui->pic->pos().x() &&
       event->x() < ui->pic->pos().x()+ui->pic->width() &&
       event->y() > ui->pic->pos().y() &&
       event->y() < ui->pic->pos().y()+ui->pic->width() &&
       event->button() == Qt::LeftButton)
    {
//        m_inform =  new inform(*m_sock.get(),m_classNum,m_account);
        m_inform->show();
    }
}

void userweight::changeTouXiang(string touxiangPath)
{
    qDebug() << "------------" << QString(touxiangPath.c_str());
    m_touXiangPath = touxiangPath;
    ui->pic->setPixmap(QPixmap(touxiangPath.c_str()));
    initUserTreeWidget();
    initMessageTreeWidget();
    int count = ui->userTreeWidget->topLevelItemCount();
//    for(int i=0;i<count;i++)
//    {
//        QTreeWidgetItem *item = ui->userTreeWidget->takeTopLevelItem(i);
//        if(item->text(0).toStdString() == m_classNum)
//        {
//            int childCount = item->childCount();
//            for(int j=0;j<childCount;j++)
//            {
//                QTreeWidgetItem *itemchild = item->child(j);
//                if(itemchild->text(0).toStdString() ==m_account)
//                {
//                    _insertTreeWidget(m_account,touxiangPath,itemchild);
//                    break;
//                }
//            }
//        }
//    }
}
