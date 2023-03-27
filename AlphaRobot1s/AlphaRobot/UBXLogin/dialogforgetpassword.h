#ifndef DIALOGFORGETPASSWORD_H
#define DIALOGFORGETPASSWORD_H

#include <QDialog>

namespace Ui {
class DialogForgetPassword;
}

class DialogForgetPassword : public QDialog
{
    Q_OBJECT

public:
    explicit DialogForgetPassword(QWidget *parent = 0);
    ~DialogForgetPassword();

private:
    Ui::DialogForgetPassword *ui;
};

#endif // DIALOGFORGETPASSWORD_H
