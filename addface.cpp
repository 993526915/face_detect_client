#include "addface.h"
#include "ui_addface.h"
#include<QMouseEvent>
#include<QDebug>
#include<QFileDialog>
#include<QMessageBox>
#include<QString>
#include<QDateTime>
#include<jsoncpp/json/json.h>
#include<opencv2/opencv.hpp>
using namespace cv;

int read_binary(char *filename, char *buffer) {

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("fopen failed\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int size = fread(buffer, 1, length, fp);
    if (size != length) {
        printf("fread faile\n");
        return size;
    }

    fclose(fp);

    return size;
}



addFace::addFace(shared_ptr<QSqlDatabase> db,shared_ptr<faceDetect> facedet,shared_ptr<CDBPool> dbPool,shared_ptr<int> fd,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addFace)
{
    m_db = db;
    m_CDBPool = dbPool;
    m_faceDetect = facedet;
    m_socketFd = fd;
    ui->setupUi(this);
}

void addFace::getParams(QString &user_id,QString &group_id,QString &user_info,QString &detect_quality)
{
    user_id = ui->lineEdit_user_id->text();
    user_info = ui->textEdit_user_info->toPlainText();
    group_id = ui->lineEdit_group_id->text();
    detect_quality = ui->comboBox_detect_quality->currentText();
}

void addFace::mousePressEvent(QMouseEvent *event)
{
    if(event->x() > ui->label_image->pos().x() &&
       event->x() < ui->label_image->pos().x()+ui->label_image->width() &&
       event->y() > ui->label_image->pos().y() &&
       event->y() < ui->label_image->pos().y()+ui->label_image->width() &&
       event->button() == Qt::LeftButton)
    {
        m_pictureName = QFileDialog::getOpenFileName(this,
                                                        tr("图片选择"),
                                                        "/home/wzx/detect",
                                                        tr("图片文件(*png *jpg)"));
        qDebug()<<"filename : "<<m_pictureName;
        QPixmap image(m_pictureName);
        ui->label_image->setPixmap(image);
    }
}

addFace::~addFace()
{
    delete ui;
}

void addFace::on_buttonBox_ok_cancel_accepted()
{
    QString user_id;
    QString group_id;
    QString user_info;
    QString detect_quality;
    if(ui->lineEdit_user_id->text().size() == 0 ||
       ui->lineEdit_group_id->text().size() == 0 ||
       ui->comboBox_detect_quality->currentText() == 0)
    {
        QMessageBox *message = new QMessageBox(
                    QMessageBox::NoIcon,
                "警告", "项目未填入",
                QMessageBox::Yes, NULL);
        message->show();
    }
    else
    {
        getParams(user_id,group_id,user_info,detect_quality);
        CDBConn *newConnect = m_CDBPool->GetDBConn();
        if(newConnect->StartTransaction()) //启动事务操作
        {

            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
            char buffer[1024*1024];
            char fileName[100];
            memset(buffer,0,sizeof (buffer));
            memset(fileName,0,sizeof (fileName));
            memcpy(fileName,m_pictureName.toStdString().c_str(),m_pictureName.size());
            //char *fileName = (char *)m_pictureName.toStdString().c_str();
            int length = read_binary(fileName,buffer);
            bool bRet = false;
            QString strSql;
            strSql = "insert into user_table(group_id, user_id, user_info,detect_quality,image,ctime) values('"+group_id+"','"+user_id+"','"+user_info+"','"+detect_quality+"',?,'"+current_date+"')";
            qDebug()<<strSql;
            string sql = strSql.toStdString();
            CPrepareStatement *stmt = new CPrepareStatement();
            if (stmt->Init(newConnect->GetMysql(),sql))
            {
                int index = 0;
                stmt->SetParam(index++, buffer);
                bRet = stmt->ExecuteUpdate(buffer,0,length);
                if (!bRet)
                {
                    m_CDBPool->RelDBConn(newConnect);
                    QMessageBox::warning(this,"错误","读取sql错误-----insert into user_table values(?,?,?,?,?,?);");
                }
                else
                {
                    Mat src = imread(m_pictureName.toStdString());
                    qDebug() << "group_id : " << group_id << endl;
                    qDebug() << "user_id : " << user_id << endl;
                    qDebug() << "user_info : " << user_info << endl;
                    qDebug() << "detect_quality : " << detect_quality << endl;
                    addFaceData *addface = new addFaceData;
                    addface->m_cmd = ADDFACE;
                    memcpy(addface->m_classnum,user_id.toStdString().c_str(),user_id.length());
                    memcpy(addface->m_account,group_id.toStdString().c_str(),group_id.length());
                    memcpy(addface->m_password,user_info.toStdString().c_str(),user_info.length());

                    memcpy(addface->m_facedata,src.data,sizeof (src));
                    SockUtil::co_write(*m_socketFd.get(),(const char *)addface,addface->m_dataLength);

//                    Json::Value root;
//                    Json::Value imageJs;
//                    Json::Value faceJs;
//                    //根节点属性
//                    root["cmd"] = Json::Value(ADDFACE);
//                    faceJs["group_id"]= Json::Value(group_id.toStdString());
//                    faceJs["user_id"] = Json::Value(user_id.toStdString());
//                    faceJs["user_info"] = Json::Value(user_info.toStdString());
//                    faceJs["detect_quality"] = Json::Value(detect_quality.toStdString());
//                    imageJs["pic_type"] = Json::Value(src.type());
//                    imageJs["pic_cols"] = Json::Value(src.cols);
//                    imageJs["pic_rows"] = Json::Value(src.rows);
//                    imageJs["pic_data"] = Json::Value((char *)src.data);
//                    root["image"]=Json::Value(imageJs);
//                    root["face"]=Json::Value(faceJs);
//                    string js =  root.toStyledString();
//                    SockUtil::co_write(*m_socketFd.get(),js.c_str(),sizeof(src));
                    char ret[1024];
                    memset(ret,0,sizeof(ret));
                    SockUtil::co_read(*m_socketFd.get(),ret,sizeof(ret));
                    Json::Value json_res;
                    string str = ret;
                    Json::Reader reader;
                    if (reader.parse(str, json_res))
                    {
                        cout << json_res << endl;
                        if(json_res["error_code"].asInt()!= 0)
                        {
                            newConnect->Rollback();
                            qDebug() << "error code----------on_buttonBox_ok_cancel_accepted()" << endl;
                        }
                        else
                        {
                            emit addTreeItems(group_id,user_id);
                            newConnect->Commit();
                        }
                        string res = "code :" + json_res["error_code"].asString() +"msg :" +json_res["error_msg"].asString();
                        QMessageBox *message = new QMessageBox(
                                    QMessageBox::NoIcon,
                                "调用信息", QString(res.c_str()),
                                QMessageBox::Yes, NULL);
                        message->show();
                    }
//                    int flag = m_faceDetect->add(json_res,src,group_id.toStdString(),
//                                      user_id.toStdString(),user_info.toStdString(),
//                                      detect_quality.toStdString());
                }
            }
        }
        //       if(m_db->transaction()) //启动事务操作
        //       {
        //           QSqlQuery query(*m_db.get());
        //           QDateTime current_date_time =QDateTime::currentDateTime();
        //           QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
        //           QByteArray data;
        //           QFile* file=new QFile(m_pictureName); //fileName为二进制数据文件名
        //           file->open(QIODevice::ReadOnly);
        //           data = file->readAll();
        //           file->close();
        //           QVariant var(data);
        //           query.prepare("insert into user_table values(?,?,?,?,?,?);");
        //           query.bindValue(0,group_id.toStdString().c_str());
        //           query.bindValue(1,user_id.toStdString().c_str());
        //           query.bindValue(2,user_info.toStdString().c_str());
        //           query.bindValue(3,detect_quality.toStdString().c_str());
        //           query.bindValue(4,var);
        //           query.bindValue(5,current_date.toStdString().c_str());
        //           query.exec();
        //           QSqlError sqlError = query.lastError();
        //           if(sqlError.type()  == QSqlError::NoError)
        //           {
        //               Json::Value json_res;
        //               Mat src = imread(m_pictureName.toStdString());
        //               qDebug() << "group_id : " << group_id << endl;
        //               qDebug() << "user_id : " << user_id << endl;
        //               qDebug() << "user_info : " << user_info << endl;
        //               qDebug() << "detect_quality : " << detect_quality << endl;

        //               int flag = m_faceDetect->add(json_res,src,group_id.toStdString(),
        //                                 user_id.toStdString(),user_info.toStdString(),
        //                                 detect_quality.toStdString());

        //               cout << json_res << endl;
        //               if(flag != 1 || json_res["error_code"].asInt()!= 0)
        //               {
        //                   m_db->rollback();
        //                   qDebug() << "error code----------on_buttonBox_ok_cancel_accepted()" << endl;
        //               }
        //               else
        //               {
        //                   emit addTreeItems(group_id,user_id);
        //                   m_db->commit();
        //               }
        //               string res = "code :" + json_res["error_code"].asString() +"msg :" +json_res["error_msg"].asString();
        //               QMessageBox *message = new QMessageBox(
        //                           QMessageBox::NoIcon,
        //                       "调用信息", QString(res.c_str()),
        //                       QMessageBox::Yes, NULL);
        //               message->show();
        //           }
        //           else
        //           {
        //               qDebug() << "mysql error -------------on_buttonBox_ok_cancel_accepted()"<<endl;
        //           }
        //       }
    }
}

