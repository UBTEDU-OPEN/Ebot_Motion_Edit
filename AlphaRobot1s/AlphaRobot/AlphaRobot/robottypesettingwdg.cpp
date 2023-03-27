#include "robottypesettingwdg.h"
#include "ui_robottypesettingwdg.h"

#include "../Common/robotDefine.h"
#include "../zxLib/ciniconfig.h"
#include "../UBXBase/UBXBase.h"

robotTypeSettingWdg::robotTypeSettingWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::robotTypeSettingWdg)
{
    ui->setupUi(this);

    m_strRobotType.clear();
}

robotTypeSettingWdg::~robotTypeSettingWdg()
{
    delete ui;
}

void robotTypeSettingWdg::saveToFile()
{
    CIniConfig::getInstance().setValueRobotType(m_strRobotType);    
    UBXBase::getInstance()->setCurrentRobotProductType(m_strRobotType);
}

void robotTypeSettingWdg::initUI()
{
    QString strRobotType;
    CIniConfig::getInstance().getValueRobotType(strRobotType);

    m_strRobotType = strRobotType;

    if((m_strRobotType != ROBOT_TYPE_ROBOT1P) &&
            (m_strRobotType != ROBOT_TYPE_ROBOT1E))
    {        
        m_strRobotType = ROBOT_TYPE_ROBOT1P;
        CIniConfig::getInstance().setValueRobotType(m_strRobotType);
    }

    if(m_strRobotType == ROBOT_TYPE_ROBOT1P)
    {
        ui->radioButtonRobot1P->setChecked(true);
    }

    if(m_strRobotType == ROBOT_TYPE_ROBOT1E)
    {
        ui->radioButtonRobot1E->setChecked(true);
    }
}

void robotTypeSettingWdg::on_radioButtonRobot1P_clicked()
{
    m_strRobotType = ROBOT_TYPE_ROBOT1P;
}

void robotTypeSettingWdg::on_radioButtonRobot1E_clicked()
{
    m_strRobotType = ROBOT_TYPE_ROBOT1E;
}
