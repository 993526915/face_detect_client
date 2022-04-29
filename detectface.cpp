#include "detectface.h"
#include "ui_detectface.h"

detectface::detectface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::detectface)
{
    ui->setupUi(this);
}

detectface::~detectface()
{
    delete ui;
}
