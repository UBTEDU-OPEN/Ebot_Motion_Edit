#include "dialogforgetpassword.h"
#include "ui_dialogforgetpassword.h"

DialogForgetPassword::DialogForgetPassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogForgetPassword)
{
    ui->setupUi(this);
}

DialogForgetPassword::~DialogForgetPassword()
{
    delete ui;
}
