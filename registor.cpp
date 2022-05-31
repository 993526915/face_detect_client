#include "registor.h"
#include "ui_registor.h"

registor::registor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registor)
{
    m_pictureName="";
    ui->setupUi(this);
    ui->rePassWord->setEchoMode(QLineEdit::Password);
    ui->PassWord->setEchoMode(QLineEdit::Password);
}

registor::~registor()
{

    delete ui;
}
void registor::mousePressEvent(QMouseEvent *event)
{

    if(event->x() > ui->addPicture->pos().x() &&
       event->x() < ui->addPicture->pos().x()+ui->addPicture->width() &&
       event->y() > ui->addPicture->pos().y() &&
       event->y() < ui->addPicture->pos().y()+ui->addPicture->width() &&
       event->button() == Qt::LeftButton)
    {
        m_pictureName = QFileDialog::getOpenFileName(this,
                                                        tr("图片选择"),
                                                        "/home/wzx/netlearning/wzxServer",
                                                        tr("图片文件(*png *jpg)"));
        qDebug()<<"filename : "<<m_pictureName;
        QPixmap image(m_pictureName);
        ui->addPicture->setPixmap(image);
    }
}

void registor::on_registe_clicked()
{
    int fd = SockUtil::co_createSocket();
    if(SockUtil::co_connect(fd,"127.0.0.1",8888) == -1) {
        qDebug() << "Error connecting" ;
        return ;
    }
    qDebug() << "connecting********************************" ;
    if(ui->classNumber->text()==""||ui->UserName->text()==""||ui->PassWord->text()==""||
            ui->rePassWord->text()==""||m_pictureName.size()==0)
    {
        QMessageBox::warning(this, "Warning!", "用户名或密码不为空", QMessageBox::Yes);
        ui->classNumber->clear();
        ui->UserName->clear();
        ui->PassWord->clear();
        ui->rePassWord->clear();
        ui->classNumber->setFocus();
    }
    QString username = ui->UserName->text();
    QString password = ui->PassWord->text();
    QString repassword = ui->rePassWord->text();
    QString classnum = ui->classNumber->text();
    if(password!= repassword)
    {
        QMessageBox::warning(this, "Warning!", "密码不一致", QMessageBox::Yes);
        ui->PassWord->clear();
        ui->rePassWord->clear();
        ui->PassWord->setFocus();
    }
    Mat src = imread(m_pictureName.toStdString().c_str());
    if(src.rows*src.cols > 1024 *1024)
    {
        QMessageBox::warning(this, "Warning!", "图片大小不能超过1M", QMessageBox::Yes);
        ui->addPicture->clear();
        this->m_pictureName="";
        SockUtil::co_close(fd);
        return;
    }
    if(src.empty())
    {
        qDebug() << "empty picture !";
        return ;
    }
    addFaceData *addFace = new addFaceData;
    memcpy(addFace->m_classnum,classnum.toStdString().c_str(),classnum.size());
    memcpy(addFace->m_account,username.toStdString().c_str(),classnum.size());
    memcpy(addFace->m_password,password.toStdString().c_str(),classnum.size());
    addFace->m_cmd = ADDFACE;

    memcpy(addFace->m_facedata,src.data,src.cols * src.rows * src.elemSize());
    addFace->m_cols = src.cols;
    addFace->m_rows = src.rows;
    addFace->m_type = src.type();
    string touxiang = ":/touxiang/dog.jpg";
    memcpy(addFace->m_touxiang,touxiang.c_str(),touxiang.length());
    int len = SockUtil::co_write(fd,(const char *)addFace,addFace->m_dataLength);
    if(len < 0)
    {
        qDebug() << "write err!";
        return ;
    }
    registerResData *addFaceRes = new registerResData;
    SockUtil::co_read(fd,addFaceRes,addFaceRes->m_dataLength);
    if(addFaceRes->m_res == REGISTER_USER_ALREADY_EXIST)
    {
        QMessageBox::warning(this, "Warning!", "用户已经存在", QMessageBox::Yes);
        SockUtil::co_close(fd);
        this->close();
        loginDialog * logindialog = new loginDialog;
        logindialog->show();
    }
    else if(addFaceRes->m_res == REGISTER_SUCCESS)
    {
        SockUtil::co_close(fd);
        this->close();
        loginDialog * logindialog = new loginDialog;
        logindialog->show();
    }

}
