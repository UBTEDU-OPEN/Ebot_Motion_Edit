/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：languagesettingwidget.h
* 创建时间：2016/06/07
* 文件标识：
* 文件摘要：参数设置控件
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/06/07
* 版本摘要：
*/

#ifndef LANGUAGESETTINGWIDGET_H
#define LANGUAGESETTINGWIDGET_H

#include <QWidget>

#include "alpharobotconst.h"

namespace Ui {
class languagesettingwidget;
}

class LanguageSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LanguageSettingWidget(QWidget *parent = 0);
    ~LanguageSettingWidget();

    /************************************
    * 名称: saveToXML
    * 功能: 保存当前配置到文件
    * 参数：[in] node，节点
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void saveToFile(const QString &strIniFile);

    /************************************
    * 名称: loadFromXML
    * 功能: 从文件中导入配置
    * 参数：[in] node，节点
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void loadFromFile(const QString &strIniFile);

public slots:

    /************************************
    * 名称: slotOnCurrentItemChanged
    * 功能: 语言选择槽函数
    * 参数：[in] index，combox当前项id
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void slotOnCurrentItemChanged(int index);

private slots:
    void on_radioButtonChinese_clicked();
    void on_radioButtonEnglish_clicked();
    void on_radioButtonHungary_clicked();

private:
    /************************************
    * 名称: initUI
    * 功能: 初始化界面
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void initUI();

    /************************************
    * 名称: setCurLanguage
    * 功能: 设置当前语言
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void setCurLanguage();

private:
    Ui::languagesettingwidget *ui;

    QString m_strLanguageKeyword;  // 多语言关键字，中文：zh_CN， 英文：en
};

#endif // LANGUAGESETTINGWIDGET_H
