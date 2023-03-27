#ifndef CROBOTPRODUCTTYPEWDG_H
#define CROBOTPRODUCTTYPEWDG_H

#include <QWidget>
#include "../zxLib/widget/ubxwidget.h"


namespace Ui {
class CRobotProductTypeWdg;
}

class CRobotProductTypeWdg : public UBXWidget
{
    Q_OBJECT

public:
    explicit CRobotProductTypeWdg(QWidget *parent = 0);
    ~CRobotProductTypeWdg();


public:
    RETRANSLATEUIFUNC

private slots:
    void on_pushButtonAuto_clicked();
    void on_pushButtonManual_clicked();

private:
    Ui::CRobotProductTypeWdg *ui;
};

#endif // CROBOTPRODUCTTYPEWDG_H
