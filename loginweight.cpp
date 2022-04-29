#include "loginweight.h"
#include "ui_loginweight.h"
#include <QtNetwork>
#include <QMessageBox>
loginWeight::loginWeight(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::loginWeight)
{

    ui->setupUi(this);
}

loginWeight::~loginWeight()
{
    delete ui;
}
int loginWeight::initSocket()
{
    if (SockUtil::co_connect(*m_socketFd.get(),"127.0.0.1",8888) == -1) {
        qDebug() << "Error connecting" ;
        return 0;

    }    
    qDebug() << "connecting********************************" ;
    return 1;
}
void loginWeight::on_Login_clicked()
{
    qDebug() << ui->ClassNum->text();
    qDebug() << ui->UserName->text();
    qDebug() << ui->Password->text();
    if(ui->UserName->text()!=""&& ui->Password->text()!= ""&& ui->ClassNum->text()!= "")
    {//账号密码不能为空
       // initSocket();
        int fd = SockUtil::co_createSocket();
        m_socketFd = make_shared<int>(fd);
        if(!initSocket())
        {
            QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
            SockUtil::co_close(*m_socketFd.get());
        }
        else
        {//服务器连接成功
            ui->Login->setEnabled(false);
            loginData *logindata = new loginData;

            logindata->m_cmd = LOGIN;
            string classnum = ui->ClassNum->text().toStdString();
            string username = ui->UserName->text().toStdString();
            string password = ui->Password->text().toStdString();
            memcpy(logindata->m_classnum,classnum.c_str(),classnum.size());
            memcpy(logindata->m_username,username.c_str(),username.size());
            memcpy(logindata->m_password,password.c_str(),password.size());
            int len = SockUtil::co_write(*m_socketFd.get(),(const char *)logindata,logindata->m_dataLength);
            if(len < 0)
            {
                QMessageBox::warning(this, "Warning!", "write错误", QMessageBox::Yes);
                ui->Login->setEnabled(true);
                ui->UserName->clear();
                ui->Password->clear();
                ui->ClassNum->clear();
                ui->ClassNum->setFocus();
                SockUtil::co_close(*m_socketFd.get());
            }
            loginResData *resData = new loginResData;
            len = SockUtil::co_read(*m_socketFd.get(),resData,resData->m_dataLength);
            if(len < 0)
            {
                QMessageBox::warning(this, "Warning!", "write错误", QMessageBox::Yes);
                ui->Login->setEnabled(true);
                ui->UserName->clear();
                ui->Password->clear();
                ui->ClassNum->clear();
                ui->ClassNum->setFocus();
                SockUtil::co_close(*m_socketFd.get());
            }
            //qDebug() << resData->m_res ;
            if(resData->m_res == LOGIN_SUCCESS)
            {
                qDebug() << "LOGIN_SUCCESS" ;
                this->close();
                detectweight * detect = new detectweight(classnum,username,m_socketFd);
                detect->show();

            }
            else if(resData->m_res == LOGIN_NOUSER)
            {
                SockUtil::co_close(*m_socketFd.get());
                QMessageBox::warning(this, "Warning!", "账号错误", QMessageBox::Yes);
                ui->Login->setEnabled(true);
                ui->UserName->clear();
                ui->Password->clear();
                ui->ClassNum->clear();
                ui->ClassNum->setFocus();
            }
        }
    }
    else
    {
        QMessageBox::warning(this, "Warning!", "用户名或密码不为空", QMessageBox::Yes);
        ui->UserName->clear();
        ui->Password->clear();
        ui->ClassNum->clear();
        ui->ClassNum->setFocus();
    }
}

void loginWeight::on_registe_clicked()
{
    this->close();
    if(m_socketFd)
        SockUtil::co_close(*m_socketFd.get());
    registor *registorDialog = new registor;
    registorDialog->show();
}
