#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPainter>
#include<QSqlDatabase>
#include<QSqlError>
#include<QMessageBox>
#include<map>
#include<initparams.h>
#include"camera/camera.h"
#include"procomimg.h"
#include"addface.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initTreeWidget();
    void initDataBase();

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void showUp(QPixmap pixMap);
    void showDetectFaceInfo(QString group_id,QString user_id);
    void showFaceDetails();
    void addUser(QString group_id,QString user_id);
    void on_detectFace_clicked();    
    void on_pushButton_add_user_clicked();
    void on_pushButton_delete_user_clicked();

    void on_pushButton_delete_face_group_clicked();

private:
    shared_ptr<proComImg> m_proComImg;
    shared_ptr<comsumePro> m_comThread;
    shared_ptr<producePro> m_proThread;
    shared_ptr<detectPro> m_detectThread;
    shared_ptr<PrintQPixMap> m_printPixMap;
    shared_ptr<faceDetect> m_faceDetect;
    shared_ptr<initParams> m_initParams;
    shared_ptr<QSqlDatabase> m_db;
    shared_ptr<CDBPool> m_CDBPool;
    QPixmap m_picture;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
