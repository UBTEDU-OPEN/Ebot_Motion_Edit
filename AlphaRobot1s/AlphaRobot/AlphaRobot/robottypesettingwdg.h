#ifndef ROBOTTYPESETTINGWDG_H
#define ROBOTTYPESETTINGWDG_H

#include <QWidget>

namespace Ui {
class robotTypeSettingWdg;
}

class robotTypeSettingWdg : public QWidget
{
    Q_OBJECT

public:
    explicit robotTypeSettingWdg(QWidget *parent = 0);
    ~robotTypeSettingWdg();

public:
    void saveToFile();
    void initUI();

private slots:
    void on_radioButtonRobot1P_clicked();
    void on_radioButtonRobot1E_clicked();

private:
    Ui::robotTypeSettingWdg *ui;

    QString m_strRobotType;
};

#endif // ROBOTTYPESETTINGWDG_H
