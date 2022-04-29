#ifndef ADDFACE_H
#define ADDFACE_H

#include <QDialog>
#include<string>
#include<memory>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlDriver>
#include<QSqlError>
#include"socktool.h"
#include"DBpool.h"
#include"faceDetect/facedetect.h"
using namespace  std;
namespace Ui {
class addFace;
}

class addFace : public QDialog
{
    Q_OBJECT

public:
    explicit addFace(shared_ptr<QSqlDatabase> db,shared_ptr<faceDetect> facedet,shared_ptr<CDBPool> dbPool,shared_ptr<int> fd,QWidget *parent = nullptr);
    void getParams(QString &user_id,QString &group_id,QString &user_info,QString &detect_quality);
    ~addFace();
protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void addTreeItems(QString group_id,QString user_id);

private slots:
    void on_buttonBox_ok_cancel_accepted();

private:
    Ui::addFace *ui;
    shared_ptr<QSqlDatabase> m_db;
    shared_ptr<CDBPool> m_CDBPool;
    shared_ptr<faceDetect> m_faceDetect;
    shared_ptr<int> m_socketFd;
    QString m_pictureName;
};

#endif // ADDFACE_H
