#include "detectweight.h"
#include "ui_detectweight.h"
#include<QDebug>
detectweight::detectweight(string classnum,string account,shared_ptr<int> fd,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::detectweight)
{

    m_sockFd = fd;
    m_proComImg = make_shared<proComImg>();
    m_comThread = make_shared<comsumePro>(m_proComImg);
    m_proThread = make_shared<producePro>(m_proComImg);
    m_detectThread = make_shared<detectPro>(m_proComImg,m_sockFd,classnum,account);
    m_printPixMap = make_shared<PrintQPixMap>(m_proComImg);
    m_classnum =classnum;
    m_account = account;

    //图片获取
    void(proComImg::*image)(Mat) = &proComImg::getImage;
    connect(m_proComImg.get(),image,m_printPixMap.get(),&PrintQPixMap::showImage);
    //图片显示
    void(PrintQPixMap::*showUp)(QPixmap) = &PrintQPixMap::printImg;
    connect(m_printPixMap.get(),showUp,this,&detectweight::showUp);
    //识别成功
    void(proComImg::*success)() = &proComImg::faceSuccess;
    connect(m_proComImg.get(),success,this,&detectweight::detectSuccess);
    //识别失败
    void(proComImg::*failed)() = &proComImg::faceFailed;
    connect(m_proComImg.get(),failed,this,&detectweight::detectFailed);

    m_comThread->start();
    m_proThread->start();
    m_detectThread->start();

    ui->setupUi(this);
    ui->text->setText("请正视屏幕");
    ui->right->setVisible(false);

    //时间的实时显示（添加信号槽的连接）
   timer =new QTimer(this);
   connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
   timer->start(1000);

}
void detectweight::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int x1 = ui->picture->pos().x();
    int y1 = ui->picture->pos().y();
    painter.drawPixmap(x1,y1,ui->picture->width(),ui->picture->height(),m_picture);
}

detectweight::~detectweight()
{
    SockUtil::co_close(*m_sockFd.get());
    delete ui;
    delete timer;
}

void detectweight::detectSuccess()
{
    static int successNum = 0;
    successNum++;
    static int failedNum = 0;
    if(successNum == 2)
    {
        ui->text->setVisible(false);
        ui->right->setVisible(true);
    }
    if(successNum == 3)
    {
        qDebug() << "detectSuccess--------------------------";
        successNum = 0;
        SockUtil::co_close(*m_sockFd.get());

        this->close();
        m_proComImg->setIsDetect(false);
        m_proComImg->setIsProduce(false);
        m_proComImg->setIsComsume(false);
        string s = "王则";
        userweight *user = new userweight(QString(m_classnum.c_str()),QString(m_account.c_str()));
        user->show();
    }
}
void detectweight::detectFailed()
{
    static int failedNum = 0;
    failedNum++;
    if(failedNum == 2)
    {
        qDebug() << "detectFailed--------------------------";
        SockUtil::co_close(*m_sockFd.get());
        QMessageBox::warning(this, "Warning!", "人脸识别错误", QMessageBox::Yes);
        this->close();
        m_proComImg->setIsDetect(false);
        m_proComImg->setIsProduce(false);
        m_proComImg->setIsComsume(false);
        loginDialog * login = new loginDialog;
        login->show();

    }
}
void detectweight::showUp(QPixmap pixMap)
{
    this->m_picture =pixMap;
    ui->picture->setPixmap(this->m_picture);
    update();
}

void detectweight::timerUpdate()
{
    QDateTime time =QDateTime::currentDateTime();
    QString str=time.toString("yyyy-MM-dd dddd hh:mm:ss");
    ui->time->setText(str); // ui->date1 是界面控件名称
}
