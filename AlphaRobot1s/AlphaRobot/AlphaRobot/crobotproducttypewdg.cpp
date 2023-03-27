#include "crobotproducttypewdg.h"
#include "ui_crobotproducttypewdg.h"

RETRANSLATEUIFUNCIMPL(CRobotProductTypeWdg)
CRobotProductTypeWdg::CRobotProductTypeWdg(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::CRobotProductTypeWdg)
{
    ui->setupUi(this);
}

CRobotProductTypeWdg::~CRobotProductTypeWdg()
{
    delete ui;
}

void CRobotProductTypeWdg::on_pushButtonAuto_clicked()
{

}

void CRobotProductTypeWdg::on_pushButtonManual_clicked()
{

}
