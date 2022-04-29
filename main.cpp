
#include"loginweight.h"
#include"detectweight.h"
#include"userweight.h"
#include"comwidget.h"
#include <QApplication>
#include"logindialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    shared_ptr<int> fd = make_shared<int>(3);
////    detectweight w(fd);
////    w.setWindowTitle("人脸识别");
////    w.show();
//    loginWeight *w = new loginWeight;
//    w->show();

//    loginDialog* w = new loginDialog;
//    w->show();

//    int fd = SockUtil::co_createSocket();
//    if (SockUtil::co_connect(fd,"127.0.0.1",8888) == -1) {
//        qDebug() << "Error connecting" ;
//        return 0;

//    }
//    shared_ptr<int> m = make_shared<int>(fd);
//    detectweight w("2","2",m);
//    w.setWindowTitle("人脸识别");
//    w.show();


//    comwidget * comm = new comwidget(fd,"1","11","1","11");
//    comm->show();
//    comm->setWindowTitle("1");

//    int fd2 = SockUtil::co_createSocket();
//    if (SockUtil::co_connect(fd2,"127.0.0.1",8888) == -1) {
//        qDebug() << "Error connecting" ;
//        return 0;

//    }
//    comwidget * comm2 = new comwidget(fd2,"11","1","11","1");
//    comm2->show();
//    comm2->setWindowTitle("11");


//    userweight *user = new userweight("2","2");
//    user->show();

    userweight *user2 = new userweight("1","1");
    user2->show();

    return a.exec();



}

