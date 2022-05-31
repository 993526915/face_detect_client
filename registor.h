#ifndef REGISTOR_H
#define REGISTOR_H

#include <QWidget>
#include<QMouseEvent>
#include<QFileDialog>
#include<QMessageBox>
#include"socktool.h"
#include"loginweight.h"
#include<opencv2/opencv.hpp>
using namespace cv;
namespace Ui {
class registor;
}

class registor : public QWidget
{
    Q_OBJECT

public:
    explicit registor(QWidget *parent = nullptr);
    ~registor();
protected:
    void mousePressEvent(QMouseEvent *event);
private slots:

    void on_registe_clicked();

private:
    Ui::registor *ui;
    QString m_pictureName;
};

#endif // REGISTOR_H
