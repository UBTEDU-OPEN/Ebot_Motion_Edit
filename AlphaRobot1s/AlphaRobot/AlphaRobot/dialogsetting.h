/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：dialogsetting.h
* 创建时间：2016/06/07
* 文件标识：
* 文件摘要：参数设置控件
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/06/07
* 版本摘要：
*/

#ifndef DIALOGSETTING_H
#define DIALOGSETTING_H

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>

#include "languagesettingwidget.h"
#include "robottypesettingwdg.h"
#include "../zxLib/widget/ubxdialog.h"


namespace Ui {
class dialogsetting;
}

class SettingDialog : public CUBXDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(const QString &strIniFile, QWidget *parent = 0);
    ~SettingDialog();

    /************************************
    * 名称: loadAllSettingFromFile
    * 功能: 导入所有配置信息
    * 参数：strIniFile，文件名称
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void loadAllSettingFromFile(const QString &strIniFile);

protected:
    RETRANSLATEUIFUNC

    /*************
    * 函数名:mouseMoveEvent
    * 功能:鼠标移动事件
    * 参数:
    * 返回值:
    * 时间: 2016/06/07
    * 作者: hwx
    *****************/
    virtual void mouseMoveEvent(QMouseEvent*event);

    /*************
    * 函数名:mousePressEvent
    * 功能:鼠标按下事件
    * 参数:
    * 返回值:
    * 时间: 2016/06/07
    * 作者: hwx
    *****************/
    virtual void mousePressEvent(QMouseEvent*event);

    /*************
    * 函数名:mouseReleaseEvent
    * 功能:鼠标离开事件
    * 参数:
    * 返回值:
    * 时间: 2016/06/07
    * 作者: hwx
    *****************/
    virtual void mouseReleaseEvent(QMouseEvent*event);

    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

public slots:

    /************************************
    * 名称: slotCurItemChanged
    * 功能: 当前项发生改变
    * 参数：item，当前项
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void slotItemClicked(QListWidgetItem *item);

    /************************************
    * 名称: slotOnClickedConform
    * 功能: 点击确认按钮槽函数
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void slotOnClickedSave();

    /************************************
    * 名称: slotOnClickedCancel
    * 功能: 点击取消按钮槽函数
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void slotOnClickedCancel();

    /************************************
    * 名称: slotOnClickedClosed
    * 功能: 点击取消按钮槽函数
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void slotOnClickedClosed();

private:
    /************************************
    * 名称: initUI
    * 功能: 初始化界面
    * 参数：
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void initUI();

    /************************************
    * 名称:
    * 功能: 显示提示信息，软件重新打开后生效
    * 参数：
    * 返回:   void
    * 时间:   2016/06/12
    * 作者:   hwx
    ************************************/
    void displayNotice();

    /************************************
    * 名称: initStackedWidget
    * 功能: 初始化stackwidget
    * 参数：
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void initStackedWidget();

    /************************************
    * 名称: writeAllSettingToFile
    * 功能: 保存所有配置信息
    * 参数：strIniFile，文件名称
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void writeAllSettingToFile(const QString &strIniFile);

    /************************************
    * 名称: setDefaultStyle
    * 功能: 设置样式
    * 参数：
    * 返回:   void
    * 时间:   2016/06/07
    * 作者:   hwx
    ************************************/
    void setDefaultStyle();

private:
    Ui::dialogsetting *ui;

    QString  m_strIniFile; // 配置文件

    QPoint   m_dragPosition;

    LanguageSettingWidget  *m_pLanguageSettingWidget; // 语言配置控件
    robotTypeSettingWdg *m_pRobotTypeSettingWdg;

    QListWidget                    *m_plistWidget; // 左边显示类型列表
    QStackedWidget             *m_pStackWidget; // 显示各类型的配置控件
};

#endif // DIALOGSETTING_H
