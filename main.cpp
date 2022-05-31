#include"registor.h"
#include"loginweight.h"
#include"detectweight.h"
#include"userweight.h"
#include"comwidget.h"
#include <QApplication>
#include"inform.h"
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

    loginDialog* k = new loginDialog;
    k->show();


    loginDialog* m = new loginDialog;
    m->show();

//    registor *r  = new registor;
//    r->show();

//    int fd = SockUtil::co_createSocket();
//    if (SockUtil::co_connect(fd,"127.0.0.1",8888) == -1) {
//        qDebug() << "Error connecting" ;
//        return 0;

//    }

//    inform *i = new inform(fd,"1","1");
//    i->show();

//    shared_ptr<int> m = make_shared<int>(fd);
//    detectweight w("18063111","18062037",m);
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


//    userweight *user = new userweight("18063111","18062037");
//    user->show();

//    userweight *user2 = new userweight("18063111","王则");
//    user2->show();

//    userweight *user3 = new userweight("18064026","王伟");
//    user3->show();
    return a.exec();



}

