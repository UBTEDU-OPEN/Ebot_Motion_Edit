#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>
#include "../zxLib/widget/ubxdialog.h"

namespace Ui {
class DialogAbout;
}

class DialogAbout : public CUBXDialog
{
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = 0);
    ~DialogAbout();

public:
    RETRANSLATEUIFUNC

private slots:
    void onCloseDlgClicked();
    void loadInfo();

private:
    Ui::DialogAbout *ui;
};

#endif // DIALOGABOUT_H
