#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<jsoncpp/json/json.h>
#include<QDebug>
#include<QSqlQuery>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    m_initParams=make_shared<initParams>();
    m_initParams->init("/home/wzx/face_detect_client/params.json");
    m_faceDetect = make_shared<faceDetect>(m_initParams);
    m_proComImg = make_shared<proComImg>();
    m_comThread = make_shared<comsumePro>(m_proComImg);
    m_proThread = make_shared<producePro>(m_proComImg);
    //m_detectThread = make_shared<detectPro>(m_proComImg);

    m_printPixMap = make_shared<PrintQPixMap>(m_proComImg);
    //图片获取
    void(proComImg::*image)(Mat) = &proComImg::getImage;
    connect(m_proComImg.get(),image,m_printPixMap.get(),&PrintQPixMap::showImage);
    //图片显示
    void(PrintQPixMap::*showUp)(QPixmap) = &PrintQPixMap::printImg;
    connect(m_printPixMap.get(),showUp,this,&MainWindow::showUp);
    //识别显示信息
    void(proComImg::*showeDetectFaceInfo)(QString,QString) = &proComImg::detectFace;
    connect(m_proComImg.get(),showeDetectFaceInfo,this,&MainWindow::showDetectFaceInfo);

    m_comThread->start();
    m_proThread->start();
   // m_detectThread->start();

    initDataBase();

    ui->setupUi(this);
    ui->pushButton_right_wrong->setVisible(false);
    initSocket();
    initTreeWidget();

    //单击显示信息
    connect(ui->treeWidget,&QTreeWidget::itemSelectionChanged,this,&MainWindow::showFaceDetails);


}
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int x1 = ui->widget_camera->pos().x();
    int y1 = ui->widget_camera->pos().y();
    painter.drawPixmap(x1,y1,ui->widget_camera->width(),ui->widget_camera->height(),m_picture);
}

void MainWindow::initTreeWidget()
{
    ui->treeWidget->clear();
    getGroupData *groupData = new getGroupData;
    groupData->m_cmd = GETGROUP;
//    Json::Value root;
    //根节点属性
//    root["cmd"] = Json::Value(GETGROUP);
//    root["start"]= Json::Value("0");
//    root["length"] = Json::Value("100");
//    string js =  root.toStyledString();
    SockUtil::co_write(*m_socketFd.get(),(const char *)groupData,groupData->m_dataLength);
    char ret[1024];
    memset(ret,0,sizeof(ret));
    SockUtil::co_read(*m_socketFd.get(),ret,sizeof(ret));
    Json::Reader reader;
    Json::Value json_group_res;
    string str = ret;
    //从字符串中读取数据
    if (reader.parse(str, json_group_res))
    {

        Json::Value group_id_list = json_group_res["result"]["group_id_list"];
        QList<QTreeWidgetItem *> rootItem;
        for(int i=0;i<group_id_list.size();i++)
        {
            QTreeWidgetItem *groupItem = new QTreeWidgetItem(ui->treeWidget,QStringList(QString(group_id_list[i].asString().c_str())));

            getUserData *userData = new getUserData;
            userData->m_cmd = GETUSER;
            memcpy(userData->m_group_id,group_id_list[i].asCString(),sizeof(group_id_list[i].asCString()));
            //userData->m_group_id = group_id_list[i].asCString();
            SockUtil::co_write(*m_socketFd.get(),(const char *)userData,userData->m_dataLength);
            memset(ret,0,sizeof(ret));
            SockUtil::co_read(*m_socketFd.get(),ret,sizeof(ret));
            str = ret;
            Json::Value Json_user_res;
            if (reader.parse(str, Json_user_res))
            {
                cout << Json_user_res << "-------------groupGetusers(Json_user_res,group_id_list[i].asString())"<<endl;
                Json::Value user_id_list = Json_user_res["result"]["user_id_list"];
                for(int j=0;j<user_id_list.size();j++)
                {
                    QTreeWidgetItem *userItem = new QTreeWidgetItem(groupItem,QStringList(QString(user_id_list[j].asString().c_str())));
                    groupItem->addChild(userItem);
                }
                rootItem.append(groupItem);
                ui->treeWidget->insertTopLevelItem(0,groupItem);
            }
        }
    }
}
void MainWindow::initSocket()
{
    while(1)
    {
        int fd;
        if (SockUtil::co_connect(fd,"127.0.0.1",8888) == -1) {
            qDebug() << "Error connecting" ;
            QThread::sleep(1);
            continue;
        }
        m_socketFd = make_shared<int>(fd);
        break;
    }
    qDebug() << "connecting********************************" ;
}
void MainWindow::initDataBase()
{
    shared_ptr<mysqlParams> mysqlParams;
    mysqlParams = m_initParams->getMysqlParams();
    int DBPoolMaxNum = stoi(mysqlParams->DBPoolMaxNum);
    int port = stoi(mysqlParams->HostPort);
    m_CDBPool = make_shared<CDBPool>(mysqlParams->DBPoolName.c_str(),mysqlParams->HostIp.c_str(),port,
                                     mysqlParams->UserName.c_str(),mysqlParams->PassWord.c_str(),mysqlParams->DataBaseName.c_str(),DBPoolMaxNum);
    if (m_CDBPool->Init())
    {
        string err = "init db instance failed: ";
        err.append(mysqlParams->DBPoolName.c_str());
        QMessageBox::warning(this,"错误",err.c_str());
       return ;
    }
    else
    {
        qDebug() << "db setok !" << endl;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showUp(QPixmap pixMap)
{
    this->m_picture =pixMap;
    update();
}


bool getFaceDetail(CDBConn *pDBConn, string group_id,string user_id,string &user_info,string &detect_quality,QPixmap &image)
{
    bool bRet = false;
    string strSql;
    strSql = "select * from user_table where group_id = ? and user_id = ?";
    CPrepareStatement *stmt = new CPrepareStatement();
    if (stmt->Init(pDBConn->GetMysql(), strSql))
    {
        int index = 0;
        stmt->SetParam(index++, group_id);
        stmt->SetParam(index++, user_id);
        bRet = stmt->ExecuteUpdate();
        if (!bRet)
        {
            printf("insert user failed: %s\n", strSql.c_str());
            return false;
        }
        else
        {
            MYSQL_BIND  bind[6];                /* 条件变量 */
            /* 初始化参数 */
            char detect_quality_db[STRING_SIZE];
            char image_db[1024*1024];
            char user_info_db[STRING_SIZE];
            memset(detect_quality_db,0,sizeof(detect_quality_db));
            memset(image_db,0,sizeof(image_db));
            memset(user_info_db,0,sizeof(user_info_db));
            memset( bind, 0, sizeof(bind) );
            bool is_null[6];
            /* INTEGER COLUMN */
            bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
            bind[2].buffer      = (char *) &user_info_db;
            bind[2].is_null     = &is_null[2];
            bind[2].buffer_length   = STRING_SIZE;

            /* name COLUMN */
            bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
            bind[3].buffer      = (char *)&detect_quality_db;
            bind[3].buffer_length   = STRING_SIZE;
           
            /* loc COLUMN */
            unsigned long total_length = 0;
            bind[4].buffer_type = MYSQL_TYPE_LONG_BLOB;
            bind[4].length   = &total_length;

            /* 绑定数据 */
            mysql_stmt_bind_result( stmt->getStmt(), bind );
            mysql_stmt_store_result(stmt->getStmt() ); /*  */

            int row_count = 0;
            /* 查询的结果 */
            row_count = mysql_stmt_num_rows(stmt->getStmt());
            /*
             * 打印数据
             * mysql_stmt_fetch(stmt);
             * mysql_stmt_fetch(stmt);
             */
            printf( "row_unm = %ld\n", row_count );
            for ( int i = 0; i < row_count; i++ )
            {
                mysql_stmt_fetch(stmt->getStmt());
                user_info= user_info_db;
                detect_quality = detect_quality_db;
                int start = 0;
                while (start < (int)total_length)
                {

                    bind[4].buffer = image_db+start;
                    bind[4].buffer_length = 3;

                    int ret = mysql_stmt_fetch_column(stmt->getStmt(), &bind[4], 4, start);
                    if (ret!=0)
                    {
                        printf("msg=%s", mysql_error(pDBConn->GetMysql()));
                        return 0;
                    }
                    start += bind[4].buffer_length;
                }
                image.loadFromData((uchar *)image_db,total_length);
                //qDebug() << write_binary("/home/wzx/netlearning/mysql/a.jpg", image_db, total_length);
            }
        }
    }
    else return false;
    delete stmt;
    return true;
}
void MainWindow::showFaceDetails()
{

    ui->label_image->clear();
    QList<QTreeWidgetItem *> selectItemList = ui->treeWidget->selectedItems();
    QTreeWidgetItem *selectItem = selectItemList[0];
    QTreeWidgetItem * root = selectItem->parent();
    if(root)
    {
        qDebug() <<"group_id : " <<root->text(0) << "user_id : " << selectItem->text(0) << endl;
        string user_info,detect_quality;
        QPixmap photo;
        CDBConn *newConnection = m_CDBPool->GetDBConn();
        if(getFaceDetail(newConnection,root->text(0).toStdString(),selectItem->text(0).toStdString(),user_info,detect_quality,photo))
        {
            m_CDBPool->RelDBConn(newConnection);
            ui->lineEdit_group_id->setText(root->text(0));
            ui->lineEdit_user_id->setText(selectItem->text(0));
            ui->textEdit_user_info->setText(QString::fromStdString(user_info));
            ui->comboBox_detect_quality->setEditText(QString::fromStdString(detect_quality));
            ui->label_image->setPixmap(photo);
        }
        else
        {
            m_CDBPool->RelDBConn(newConnection);
            QMessageBox::warning(this,"错误","读取sql错误-----select * from user_table where group_id = ? and user_id = ?");
        }
    }

}


void MainWindow::on_detectFace_clicked()
{
    if(m_proComImg->getIsDetect() == true)
    {
        qDebug() << "turn false" << endl;
        m_proComImg->setIsDetect(false);
    }
    else if(m_proComImg->getIsDetect() == false)
    {
        qDebug() << "turn true" << endl;
        m_proComImg->setIsDetect(true);
    }
}

void MainWindow::on_pushButton_add_user_clicked()
{
    addFace *addwidget = new addFace(m_db,m_faceDetect,m_CDBPool,m_socketFd);
    void(addFace::*addFaceItems)(QString,QString) = &addFace::addTreeItems;
    connect(addwidget,addFaceItems,this,&MainWindow::addUser);
    addwidget->show();
}

void MainWindow::on_pushButton_delete_user_clicked()
{

    QList<QTreeWidgetItem*> selectItems = ui->treeWidget->selectedItems();
    for(int i=0;i<selectItems.size();i++)
    {
        if(selectItems[i]->parent() == NULL) continue;
        else
        {
            QTreeWidgetItem* group_id = selectItems[i]->parent();
            QTreeWidgetItem* user_id = selectItems[i];
            qDebug() <<"group_id : " <<group_id->text(0)<< "user_id : " << user_id->text(0) << endl;
            CDBConn *newConnect =  m_CDBPool->GetDBConn();
            if(newConnect->StartTransaction())
            {
                bool bRet = false;
                string strSql;
                strSql = "delete from user_table where group_id = ? and user_id = ?";
                CPrepareStatement *stmt = new CPrepareStatement();
                if (stmt->Init(newConnect->GetMysql(), strSql))
                {
                    int index = 0;
                    stmt->SetParam(index++, group_id->text(0).toStdString());
                    stmt->SetParam(index++, user_id->text(0).toStdString());
                    bRet = stmt->ExecuteUpdate();
                    if (!bRet)
                    {
                        m_CDBPool->RelDBConn(newConnect);
                        QMessageBox::warning(this,"错误","删除sql错误-----delete from user_table where user_id = ? and group_id = ?");
                    }
                    else
                    {
                        delUserData *delUser = new delUserData;
                        delUser->m_cmd = DELUSER;
                        memcpy(delUser->m_user_id,user_id->text(0).toStdString().c_str(),user_id->text(0).toStdString().length());
                        memcpy(delUser->m_group_id,group_id->text(0).toStdString().c_str(),group_id->text(0).toStdString().length()); 
                        SockUtil::co_write(*m_socketFd.get(),(const char *)delUser,delUser->m_dataLength);
                        char ret[1024];
                        memset(ret,0,sizeof(ret));
                        SockUtil::co_read(*m_socketFd.get(),ret,sizeof(ret));
                        Json::Value json_res;
                        string str = ret;
                        Json::Reader reader;
                        if (reader.parse(str, json_res))
                        {
                            if(json_res["error_code"].asInt()!= 0)
                            {
                                newConnect->Rollback();
                                string res = "code :" + json_res["error_code"].asString() +"msg :" +json_res["error_msg"].asString();
                                QMessageBox *message = new QMessageBox(
                                            QMessageBox::NoIcon,
                                        "调用信息", QString(res.c_str()),
                                        QMessageBox::Yes, NULL);
                                message->show();
                            }
                            else
                            {
                                selectItems[i]->parent()->removeChild(ui->treeWidget->currentItem());
                                newConnect->Commit();
                            }
                        }
                    }
                }
            }
            m_CDBPool->RelDBConn(newConnect);
        }
    }
}

void MainWindow::addUser(QString group_id,QString user_id)
{
    int flag = 0;
    int num = ui->treeWidget->topLevelItemCount();
    for(int i=0;i<num;i++)
    {
        QTreeWidgetItem *group_id_item = ui->treeWidget->topLevelItem(i);
        if(group_id_item->text(0) == group_id)
        {
            flag =1;
            QTreeWidgetItem *user_id_item = new QTreeWidgetItem;
            int userNum= group_id_item->columnCount();
            user_id_item->setText(userNum-1,user_id);
            group_id_item->addChild(user_id_item);
            break;
        }
    }
    if(flag == 0)
    {
        QTreeWidgetItem *group_id_item = new QTreeWidgetItem;
        group_id_item->setText(0,group_id);
        QTreeWidgetItem *user_id_item = new QTreeWidgetItem;
        user_id_item->setText(0,user_id);
        group_id_item->addChild(user_id_item);
        ui->treeWidget->insertTopLevelItem(ui->treeWidget->columnCount()-1,group_id_item);
    }
}

void MainWindow::on_pushButton_delete_face_group_clicked()
{
    QList<QTreeWidgetItem*> selectItems = ui->treeWidget->selectedItems();
    for(int i=0;i<selectItems.size();i++)
    {
        if(selectItems[i]->parent() != NULL) continue;
        else
        {
            QTreeWidgetItem* group_id = selectItems[i];
            CDBConn *newConnect = m_CDBPool->GetDBConn();
            if(newConnect->StartTransaction())
            {
                bool bRet = false;
                string strSql;
                strSql = "delete from user_table where group_id = ?";
                CPrepareStatement *stmt = new CPrepareStatement();
                if (stmt->Init(newConnect->GetMysql(), strSql))
                {
                    int index = 0;
                    stmt->SetParam(index++, group_id->text(0).toStdString());
                    bRet = stmt->ExecuteUpdate();
                    if (!bRet)
                    {
                        m_CDBPool->RelDBConn(newConnect);
                        QMessageBox::warning(this,"错误","删除sql错误-----delete from user_table where group_id = ?");
                    }
                    else
                    {
                        delGroupData *delGroup = new delGroupData;
                        delGroup->m_cmd = DELGROUP;
                        memcpy(delGroup->m_group_id,group_id->text(0).toStdString().c_str(),group_id->text(0).toStdString().length());
                        SockUtil::co_write(*m_socketFd.get(),(const char *)delGroup,delGroup->m_dataLength);
                        char ret[1024];
                        memset(ret,0,sizeof(ret));
                        SockUtil::co_read(*m_socketFd.get(),ret,sizeof(ret));
                        Json::Reader reader;
                        Json::Value json_res;
                        string str = ret;
                        //从字符串中读取数据
                        if (reader.parse(str, json_res))
                        {
                            qDebug() << QString(str.c_str());
                            if(json_res["error_code"].asInt()!= 0)
                            {
                                newConnect->Rollback();
                            }
                            else
                            {
                                qDebug() << selectItems.size() << endl;
                                int count = group_id->childCount();
                                if(count==0)//没有子节点，直接删除
                                {
                                    delete group_id;
                                    return;
                                }

                                for(int i=0; i<count; i++)
                                {
                                    QTreeWidgetItem *childItem = group_id->child(i);//删除子节点
                                    delete childItem;
                                }
                                delete group_id;
                                newConnect->Commit();
                                m_CDBPool->RelDBConn(newConnect);
                            }
                            string res = "code :" + json_res["error_code"].asString() +"msg :" +json_res["error_msg"].asString();
                            QMessageBox *message = new QMessageBox(
                                        QMessageBox::NoIcon,
                                    "调用信息", QString(res.c_str()),
                                    QMessageBox::Yes, NULL);
                            message->show();
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::showDetectFaceInfo(QString group_id,QString user_id)
{
    qDebug() << "group_id2 : " <<group_id  << "user_id2 : " << user_id << endl;
    ui->label_image->clear();
    string user_info,detect_quality;
    QPixmap photo;
    CDBConn *newConnect = m_CDBPool->GetDBConn();
    getFaceDetail(newConnect,group_id.toStdString(),user_id.toStdString(),user_info,detect_quality,photo);
    m_CDBPool->RelDBConn(newConnect);
    qDebug() << "group_id : " << group_id << endl;
    qDebug() << "user_id : " << user_id << endl;
    qDebug() << "user_info : " << QString::fromStdString(user_info) << endl;
    qDebug() << "detect_quality : " << QString::fromStdString(detect_quality) << endl;

    ui->lineEdit_group_id->setText(group_id);
    ui->lineEdit_user_id->setText(user_id);
    ui->textEdit_user_info->setText(QString::fromStdString(user_info));
    ui->comboBox_detect_quality->setEditText(QString::fromStdString(detect_quality));
    ui->label_image->setPixmap(photo);
    ui->pushButton_right_wrong->setVisible(true);
    ui->pushButton_right_wrong->setText("√");
    ui->pushButton_right_wrong->setStyleSheet("background:rgb(138, 226, 52)");
    QTreeWidgetItem *chooseItem;
    int num = ui->treeWidget->columnCount();
    for(int i=0;i<num;i++)
    {
        if(ui->treeWidget->topLevelItem(i)->text(0) == group_id)
        {
            QTreeWidgetItem *temp = ui->treeWidget->topLevelItem(i);
            int num2 = temp->columnCount();
            for(int j=0;j<num2;j++)
            {
                if(temp->child(i)->text(0) == user_id)
                {
                    chooseItem = temp->child(i);
                }
            }
        }
    }
    if(chooseItem != NULL)
        ui->treeWidget->setCurrentItem(chooseItem);
}
