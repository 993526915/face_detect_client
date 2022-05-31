#include "inform.h"
#include "ui_inform.h"
#include"userweight.h"
#include<QGraphicsDropShadowEffect>
#include<QScrollBar>
inform::inform(int fd,string classNum ,string account,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inform)
{
    m_sock =fd;
    m_classNum = classNum;
    m_account = account;
    ui->setupUi(this);
    ui->touxiangPath->hide();
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    setFixedSize(this->width(),this->height());

    setAttribute(Qt::WA_TranslucentBackground);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(0, 0);          //设置向哪个方向产生阴影效果(dx,dy)，特别地，(0,0)代表向四周发散
    effect->setColor(Qt::gray);       //设置阴影颜色，也可以setColor(QColor(220,220,220))
    effect->setBlurRadius(20);        //设定阴影的模糊半径，数值越大越模糊
    ui->scrollArea->verticalScrollBar()->setStyleSheet("QScrollArea{border:0px solid;"
                                                       "border-radius: 4px;}"
                                                       "QScrollBar:vertical{width: 8px;border:0px solid;"
                                                       "border-radius: 4px;margin: 0px,0px,0px,0px;"
                                                       "background-color: transparent;background:#e1f0fa;}"
                                                       "QScrollBar:vertical:hover{width: 8px;"
                                                       "border:0px solid;margin: 0px,0px,0px,0px;background:#e1f0fa;}"
                                                       "QScrollBar::handle:vertical{width:4px;background:#c0ddee;"
                                                       "border-radius: 4px;height: 40px;}"
                                                       "QScrollBar::handle:vertical:hover{background:#c0ddee;"
                                                       "border-radius: 4px;}"
                                                       "QScrollBar::add-line:vertical{height:11px;background-color: transparent;"
                                                       "subcontrol-position:bottom;border:0px solid;"
                                                       "border-radius: 4px;}"
                                                       "QScrollBar::sub-line:vertical{height:11px;"
                                                       "background-color: transparent;subcontrol-position:top;"
                                                       "border:0px solid;border-radius: 4px;}"
                                                       "QScrollBar::add-page:vertical{background-color: #e1f0fa;"
                                                       "border:0px solid;border-radius: 4px;}"
                                                       "QScrollBar::sub-page:vertical{background-color: #e1f0fa;"
                                                       "border:0px solid;border-radius: 4px;}"
                                                       "QScrollBar::up-arrow:vertical{"
                                                       "border:0px solid;border-radius: 3px;}"
                                                       "QScrollBar::down-arrow:vertical {"
                                                       "border:0px solid;border-radius: 3px;}");

}
void inform::mousePressEvent(QMouseEvent *event)
{
    if(ui->widget_2->geometry().contains(event->pos())
            ||ui->widget_2->geometry().contains(event->pos())
            ||ui->label_16->geometry().contains(event->pos()))
    {
        m_pressed = true;
        m_movePos = event->globalPos() - pos();
    }

    return QDialog::mousePressEvent(event);
}
void inform::mouseMoveEvent(QMouseEvent *event)
{
    if(ui->widget_2->geometry().contains(event->pos())
            ||ui->label_17->geometry().contains(event->pos())
            ||ui->label_16->geometry().contains(event->pos()))
    {
        if (m_pressed && (event->buttons()== Qt::LeftButton)
            && (event->globalPos() - m_movePos).manhattanLength() > QApplication::startDragDistance())
        {
            QPoint movePos = event->globalPos() - m_movePos;
            this->move(movePos);
            m_movePos = event->globalPos() - pos();
        }
    }

    return QDialog::mouseMoveEvent(event);
}
void inform::mouseReleaseEvent(QMouseEvent *event)
{
    m_pressed = false;

    return QDialog::mouseReleaseEvent(event);
}
inform::~inform()
{
    delete ui;
}

void inform::on_changeTouXiang_clicked()
{
    changeTouXiangData change;
    memcpy(change.m_classNum,m_classNum.c_str(),m_classNum.length());
    memcpy(change.m_account,m_account.c_str(),m_account.length());
    string touxiang = ui->touxiangPath->text().toStdString();
    memcpy(change.m_touxiang,touxiang.c_str(),touxiang.length());
    SockUtil::co_write(m_sock,&change,change.m_dataLength);

    changeTouXiangRes changeRes;
    SockUtil::co_read(m_sock,&changeRes,changeRes.m_dataLength);
    if(changeRes.m_res == CHANGE_TOUXIANG_SUCCESS)
    {
        CDBConn *newConnect = CDBPool::GetInstance().GetDBConn();
        char strSql[200];
        memset(strSql,0,sizeof(strSql));
        sprintf(strSql,"update user_table set touxiang = '%s' where classnum = '%s' and account = '%s';",
                touxiang.c_str(),m_classNum.c_str(),m_account.c_str());
        newConnect->ExecuteQuery(strSql);
        CDBPool::GetInstance().RelDBConn(newConnect);
        emit touXiangPath(touxiang);
        this->close();
    }
    else
    {
        QMessageBox::warning(this, "Warning!", "头像修改失败", QMessageBox::Yes);
    }

}


void inform::on_Minimize_clicked()
{
    this->minimumSize();
}

void inform::on_close_clicked()
{
    this->close();
}

void inform::on_pic_1_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/beizi.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/beizi.jpg");
}

void inform::on_pic_2_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/blownbear.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/blownbear.jpg");
}

void inform::on_pic_3_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/panda.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/panda.jpg");
}

void inform::on_pic_4_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/icebear.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/icebear.jpg");
}


void inform::on_pic_5_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/dog.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/dog.jpg");
}


void inform::on_pic_6_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/duolaameng.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/duolaameng.jpg");
}

void inform::on_pic_7_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/fenhongbao.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/fenhongbao.jpg");
}

void inform::on_pic_8_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/girl.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/girl.jpg");
}


void inform::on_pic_9_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/girl2.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/girl2.jpg");
}

void inform::on_pic_10_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/girl3.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/girl3.jpg");
}

void inform::on_pic_11_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/girl4.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/girl4.jpg");
}


void inform::on_pic_12_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/haimianbob.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/haimianbob.jpg");
}

void inform::on_pic_13_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/paidaxing.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/paidaxing.jpg");
}



void inform::on_pic_14_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/pig.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/pig.jpg");
}

void inform::on_pic_15_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/shuaya.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/shuaya.jpg");
}

void inform::on_pic_16_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/tom.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/tom.jpg");
}


void inform::on_pic_17_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/xiaowanzi.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/xiaowanzi.jpg");
}

void inform::on_pic_18_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/xiaoxin.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/xiaoxin.jpg");
}


void inform::on_pic_19_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/yazi.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/yazi.jpg");
}

void inform::on_pic_20_clicked()
{
    int with = ui->touxiang->width();
    int height = ui->touxiang->height();
    QPixmap pixmap=QPixmap(":/touxiang/Simpson.jpg");
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->touxiang->setPixmap(fitpixmap);
    ui->touxiangPath->setText(":/touxiang/Simpson.jpg");
}
