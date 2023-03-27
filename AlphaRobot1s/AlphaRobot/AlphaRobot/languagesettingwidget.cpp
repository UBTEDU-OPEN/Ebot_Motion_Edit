/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：languagesettingwidget.cpp
* 创建时间：2016/06/07
* 文件标识：
* 文件摘要：参数设置控件
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/06/07
* 版本摘要：
*/

#include <QSettings>
#include <qlogging.h>
#include <QDebug>
#include <QListView>

#include "languagesettingwidget.h"
#include "ui_languagesettingwidget.h"
#include "UBXPublic.h"
#include "alpharobotlog.h"
#include "../zxLib/ciniconfig.h"
#include "../UBXBase/clanguagemgr.h"
#include "../zxLib/configs.h"

LanguageSettingWidget::LanguageSettingWidget(QWidget *parent) : QWidget(parent),
    ui(new Ui::languagesettingwidget)
{
    ui->setupUi(this);

    m_strLanguageKeyword = LANGUAGE_CHINESE_POSTFIX; // 默认是简体中文

    initUI();

    connect(ui->pComboBoxLanguage, SIGNAL(currentIndexChanged(int)), this, SLOT(slotOnCurrentItemChanged(int)));
    //connect(ui->pComboBoxLanguage, &QComboBox::currentIndexChanged, this, &LanguageSettingWidget::slotOnCurrentItemChanged);

    ui->pComboBoxLanguage->hide();
}

LanguageSettingWidget::~LanguageSettingWidget()
{
    delete ui;
}

/************************************
* 名称: saveToXML
* 功能: 保存当前配置到文件
* 参数：[in] node，节点
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void LanguageSettingWidget::saveToFile(const QString &strIniFile)
{  
//    QSettings *pSettings = new QSettings(strIniFile, QSettings::IniFormat);
//    pSettings->beginGroup(SETTING_LANGUAGE_SETTING_GROUP);
//    pSettings->setValue(SETTING_LANGUAGE_SETTING_GROUP_LANGUAGE_KEYWORD, m_strLanguageKeyword);
//    pSettings->endGroup();
    AlphaRobotLog::getInstance()->printDebug(QString("Save language %1 to file %2").arg(m_strLanguageKeyword).arg(strIniFile));
//    SAFE_DELETE(pSettings);

    CIniConfig::getInstance().setValueLanguage(m_strLanguageKeyword);

    CLanguageMgr::getInstance()->setLanguage(CConfigs::getLocalLanguageDir(),
                              CIniConfig::getInstance().getValueLanguage());

}

/************************************
* 名称: loadFromXML
* 功能: 从文件中导入配置
* 参数：[in] node，节点
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void LanguageSettingWidget::loadFromFile(const QString &strIniFile)
{       
    m_strLanguageKeyword = CIniConfig::getInstance().getValueLanguage();

    AlphaRobotLog::getInstance()->printDebug(QString("Load language %1 from file %2").arg(m_strLanguageKeyword).arg(strIniFile));

    setCurLanguage();
}

/************************************
* 名称: slotOnCurrentItemChanged
* 功能: 语言选择槽函数
* 参数：[in] index，combox当前项id
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void LanguageSettingWidget::slotOnCurrentItemChanged(int index)
{
    if (eLanguageChinese == index)
    {
        m_strLanguageKeyword = LANGUAGE_CHINESE_POSTFIX;
    }
    else if (eLanguagenglish == index)
    {
        m_strLanguageKeyword = LANGUAGE_ENGLISH_POSTFIX;
    }
    else if (eLanguageHungary == index)
    {
        m_strLanguageKeyword = LANGUAGE_HUNGARY_POSTFIX;
    }
}

/************************************
* 名称: initUI
* 功能: 初始化界面
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void LanguageSettingWidget::initUI()
{
    //ui->pComboBoxLanguage->setView(new QListView);
    ui->pComboBoxLanguage->setMaxVisibleItems(20);
    ui->pComboBoxLanguage->insertItem(eLanguageChinese, tr("Chinese"));
    ui->pComboBoxLanguage->insertItem(eLanguagenglish, tr("Englisn"));

    ui->pComboBoxLanguage->setCurrentIndex(eLanguageChinese);

}

/************************************
* 名称: setCurLanguage
* 功能: 设置当前语言
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void LanguageSettingWidget::setCurLanguage()
{
    if (0 == m_strLanguageKeyword.compare(LANGUAGE_CHINESE_POSTFIX))
    {
//        ui->pComboBoxLanguage->setCurrentIndex(eLanguageChinese);
        ui->radioButtonChinese->setChecked(true);
        ui->radioButtonEnglish->setChecked(false);
        ui->radioButtonHungary->setChecked(false);
    }
    else if (0 == m_strLanguageKeyword.compare(LANGUAGE_ENGLISH_POSTFIX))
    {
//        ui->pComboBoxLanguage->setCurrentIndex(eLanguagenglish);
        ui->radioButtonChinese->setChecked(false);
        ui->radioButtonEnglish->setChecked(true);
        ui->radioButtonHungary->setChecked(false);
    }
    else if (0 == m_strLanguageKeyword.compare(LANGUAGE_HUNGARY_POSTFIX))
    {
//        ui->pComboBoxLanguage->setCurrentIndex(eLanguagenglish);
        ui->radioButtonChinese->setChecked(false);
        ui->radioButtonEnglish->setChecked(false);
        ui->radioButtonHungary->setChecked(true);
    }
}

void LanguageSettingWidget::on_radioButtonChinese_clicked()
{
    m_strLanguageKeyword = LANGUAGE_CHINESE_POSTFIX;
}

void LanguageSettingWidget::on_radioButtonEnglish_clicked()
{
    m_strLanguageKeyword = LANGUAGE_ENGLISH_POSTFIX;
}

void LanguageSettingWidget::on_radioButtonHungary_clicked()
{
    m_strLanguageKeyword = LANGUAGE_HUNGARY_POSTFIX;
}
