#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include<QBitmap>
#include<QPainter>
#include<QMouseEvent>
#include<QDebug>
#include<memory>
#include<QMessageBox>
#include<DBpool.h>
#include"detectweight.h"
#include"socktool.h"
#include "registor.h"
using namespace  std;
namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();
private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void on_close_clicked();

    void on_minimized_clicked();

    void on_loginButton_clicked();

    void on_account_editingFinished();

    void on_registe_clicked();

private:
    int _initSocket();
    QPixmap _PixmapToRound(const QPixmap &src, int radius);
    void _initLoginWidget();
    Ui::loginDialog *ui;
    bool     m_pressed;
    QPoint   m_movePos;

    shared_ptr<int> m_socketFd;
};

#endif // LOGINDIALOG_H
