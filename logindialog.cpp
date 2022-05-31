#include "logindialog.h"
#include "ui_logindialog.h"
#include<QGraphicsDropShadowEffect>
loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    QPixmap img1;
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    char strSql[100];
    memset(strSql,0,sizeof (strSql));
    sprintf(strSql,"select classnum,account,password,ifrem from user_table;");
    CResultSet *res =  newConnect->ExecuteQuery(strSql);
    if(res && res->Next())
    {
        string classnum = res->GetString("classnum");
        string account = res->GetString("account");
        string password = res->GetString("password");
        int ifrem = res->GetInt("ifrem");
        if(ifrem)
        {
            ui->account->setText(QString(account.c_str()));
            ui->ClassNum->setText(QString(classnum.c_str()));
            ui->Password->setText(QString(password.c_str()));
            ui->rememberPassword->setCheckState(Qt::Checked);
        }
        memset(strSql,0,sizeof (strSql));
        sprintf(strSql,"select touxiang from user_table where classnum='%s' and account = '%s';",classnum.c_str(),account.c_str());
        CResultSet *touxiangRes =  newConnect->ExecuteQuery(strSql);
        if(touxiangRes && touxiangRes->Next())
        {
            string pic ;
            char *r =touxiangRes->GetString("touxiang");
            if(r)
            {
                pic = r;
                img1.load(QString(pic.c_str()));
            }
            else
            {
                img1.load(":/icon/victoricon2.png");
            }
        }
        else
        {
            img1.load(":/icon/victoricon2.png");
        }
    }
    else
    {
        img1.load(":/icon/victoricon2.png");
    }
    QPixmap pixMap= img1.scaled(90,90, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pixMap =  _PixmapToRound(pixMap, 45);
    ui->uiPicture->setPixmap(pixMap);
    CDBPool::GetInstance().RelDBConn(newConnect);
    _initLoginWidget();
    
}

loginDialog::~loginDialog()
{
    delete ui;
}
void loginDialog::_initLoginWidget()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    setFixedSize(this->width(),this->height());

    setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(0, 0);          //设置向哪个方向产生阴影效果(dx,dy)，特别地，(0,0)代表向四周发散
    effect->setColor(Qt::gray);       //设置阴影颜色，也可以setColor(QColor(220,220,220))
    effect->setBlurRadius(20);        //设定阴影的模糊半径，数值越大越模糊
    ui->frame->setGraphicsEffect(effect);
    ui->Password->setEchoMode(QLineEdit::Password);

}
QPixmap loginDialog::_PixmapToRound(const QPixmap &src, int radius)
{
    if (src.isNull()) {
        return QPixmap();
    }
    QSize size(2*radius, 2*radius);
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(0, 0, size.width(), size.height(), 99, 99);
    QPixmap image = src.scaled(size);
    image.setMask(mask);
    return image;
}
void loginDialog::mousePressEvent(QMouseEvent *event)
{
    m_pressed = true;
    m_movePos = event->globalPos() - pos();

    return QDialog::mousePressEvent(event);
}
void loginDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pressed && (event->buttons()== Qt::LeftButton)
        && (event->globalPos() - m_movePos).manhattanLength() > QApplication::startDragDistance())
    {
        QPoint movePos = event->globalPos() - m_movePos;
        this->move(movePos);
        m_movePos = event->globalPos() - pos();
    }

    return QDialog::mouseMoveEvent(event);
}
void loginDialog::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressed = false;

    return QDialog::mouseReleaseEvent(event);
}

void loginDialog::on_close_clicked()
{
    this->close();
}

void loginDialog::on_minimized_clicked()
{
    this->minimumSize();
}

int loginDialog::_initSocket()
{
    if (SockUtil::co_connect(*m_socketFd.get(),"127.0.0.1",8888) == -1) {
        qDebug() << "Error connecting" ;
        return 0;

    }
    qDebug() << "connecting********************************" ;
    return 1;
}
void loginDialog::on_loginButton_clicked()
{
    qDebug() << ui->ClassNum->text();
    qDebug() << ui->account->text();
    qDebug() << ui->Password->text();
    if(ui->account->text()!=""&& ui->Password->text()!= ""&& ui->ClassNum->text()!= "")
    {//账号密码不能为空
       // initSocket();
        int fd = SockUtil::co_createSocket();
        m_socketFd = make_shared<int>(fd);
        if(!_initSocket())
        {
            QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
            SockUtil::co_close(*m_socketFd.get());
        }
        else
        {//服务器连接成功
            int ifrem = 0;
            string classnum = ui->ClassNum->text().toStdString();
            string username = ui->account->text().toStdString();
            string password = ui->Password->text().toStdString();
            if(ui->rememberPassword->isChecked())
            {
                ifrem = 1;
            }
            CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
            char strSql[200];
            memset(strSql,0,sizeof(strSql));
            sprintf(strSql,"update user_table set classnum = '%s' , account = '%s' , password = '%s' , ifrem = %d ; ",
                    classnum.c_str(),username.c_str(),password.c_str(),ifrem) ;

            newConnect->ExecuteQuery(strSql);
            CDBPool::GetInstance().RelDBConn(newConnect);

            loginData *logindata = new loginData;
            logindata->m_cmd = LOGIN;
            memcpy(logindata->m_classnum,classnum.c_str(),classnum.size());
            memcpy(logindata->m_username,username.c_str(),username.size());
            memcpy(logindata->m_password,password.c_str(),password.size());
            int len = SockUtil::co_write(*m_socketFd.get(),(const char *)logindata,logindata->m_dataLength);
            if(len < 0)
            {
                QMessageBox::warning(this, "Warning!", "write错误", QMessageBox::Yes);
                ui->ClassNum->setFocus();
                SockUtil::co_close(*m_socketFd.get());
                return ;
            }
            loginResData *resData = new loginResData;
            len = SockUtil::co_read(*m_socketFd.get(),resData,resData->m_dataLength);
            if(len < 0)
            {
                QMessageBox::warning(this, "Warning!", "write错误", QMessageBox::Yes);
                ui->ClassNum->setFocus();
                SockUtil::co_close(*m_socketFd.get());
                return ;
            }
            if(resData->m_res == LOGIN_SUCCESS)
            {
                qDebug() << "LOGIN_SUCCESS" ;
                //this->close();
                this->close();
                detectweight * detect = new detectweight(classnum,username,m_socketFd);
                detect->show();
                //SockUtil::co_close(*m_socketFd.get());

            }
            else if(resData->m_res == LOGIN_NOUSER)
            {

                QMessageBox::warning(this, "Warning!", "账号错误", QMessageBox::Yes);
                ui->ClassNum->setFocus();
                SockUtil::co_close(*m_socketFd.get());
            }

        }
    }
    else
    {
        QMessageBox::warning(this, "Warning!", "用户名或密码不为空", QMessageBox::Yes);
        ui->account->clear();
        ui->Password->clear();
        ui->ClassNum->clear();
        ui->ClassNum->setFocus();
    }


}

void loginDialog::on_account_editingFinished()
{
    int fd = SockUtil::co_createSocket();
    m_socketFd = make_shared<int>(fd);
    if(!_initSocket())
    {
        QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
        SockUtil::co_close(*m_socketFd.get());
        return ;
    }
    CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
    QString classnum = ui->ClassNum->text();
    QString account = ui->account->text();
    char strSql[100];
    memset(strSql,0,sizeof (strSql));
    sprintf(strSql,"select touxiang from user_table where classnum='%s' and account = '%s' ;",classnum.toStdString().c_str(),account.toStdString().c_str());
    CResultSet *res =  newConnect->ExecuteQuery(strSql);
    if(res && res->Next())
    {
        char *r = res->GetString("touxiang");
        if(r)
        {
            string touxiang = r;
            QPixmap img1;
            img1.load(QString(touxiang.c_str()));
            //100,100为QLabel的宽高
            QPixmap pixMap= img1.scaled(90,90, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            //50为圆形的半径
            pixMap =  _PixmapToRound(pixMap, 45);

            ui->uiPicture->setPixmap(pixMap);
        }
        else
        {
            loginTouXiang getPic;
            memcpy(getPic.m_classNum,classnum.toStdString().c_str(),classnum.length());
            memcpy(getPic.m_account,account.toStdString().c_str(),account.length());
            SockUtil::co_write(*m_socketFd.get(),&getPic,getPic.m_dataLength);

            loginTouXiangRes getPicRes;
            SockUtil::co_read(*m_socketFd.get(),&getPicRes,getPicRes.m_dataLength);
            if(getPicRes.res == LOGIN_TOUXIANG_SUCCESS)
            {
                string pic = getPicRes.m_touxiang;
                QPixmap img1;
                img1.load(QString(pic.c_str()));
                //100,100为QLabel的宽高
                QPixmap pixMap= img1.scaled(90,90, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                //50为圆形的半径
                pixMap =  _PixmapToRound(pixMap, 45);

                ui->uiPicture->setPixmap(pixMap);

                memset(strSql,0,sizeof (strSql));
                sprintf(strSql,"update user_table set touxiang ='%s';",pic.c_str());
                newConnect->ExecuteQuery(strSql);

            }
            else
            {
                qDebug() << "no youxxiang !";
                return;
            }
        }
    }
    else
    {
        QPixmap img1;
        img1.load(":/icon/victoricon2.png");
        ui->uiPicture->setPixmap(img1);
    }

    CDBPool::GetInstance().RelDBConn(newConnect);
    SockUtil::co_close(*m_socketFd.get());
}

void loginDialog::on_registe_clicked()
{
    this->close();
    registor *r = new registor;
    r->show();
}
