#ifndef DETECTFACE_H
#define DETECTFACE_H

#include <QWidget>

namespace Ui {
class detectface;
}

class detectface : public QWidget
{
    Q_OBJECT

public:
    explicit detectface(QWidget *parent = nullptr);
    ~detectface();

private:
    Ui::detectface *ui;
};

#endif // DETECTFACE_H
