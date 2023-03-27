/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：dialogusermessageedit.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块用户信息修改窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef DIALOGUSERMESSAGEEDIT_H
#define DIALOGUSERMESSAGEEDIT_H

#include <QDialog>
#include "loginlibmgr.h"
#include <QPainter>

namespace Ui {
class DialogUserMessageEdit;
}
//修改用户信息类
class DialogUserMessageEdit : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUserMessageEdit(QWidget *parent = 0, loginLibMgr *pLoginLibMgr = NULL);
    ~DialogUserMessageEdit();

    /*************
    * 函数名:initDialog
    * 功能:初始化对话框
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void initDialog();

    /*************
    * 函数名:setDefaultStyle
    * 功能:设置默认皮肤风格
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void setDefaultStyle();

    /*************
    * 函数名:SetDialogEnalbe
    * 功能:设置对话框是否可用
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void SetDialogEnalbe( bool bRet);

private slots:  

    /*************
    * 函数名:onEditUserInfoClicked
    * 功能:编辑用户信息按钮响应
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void onEditUserInfoClicked();

    /*************
    * 函数名:onEditUserInfoOk
    * 功能:修改信息确认按钮响应槽
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void onEditUserInfoOk();

    /*************
    * 函数名:mouseMoveEvent
    * 功能:鼠标移动事件
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    virtual void mouseMoveEvent(QMouseEvent*event);

    /*************
    * 函数名:mousePressEvent
    * 功能:鼠标按下事件
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    virtual void mousePressEvent(QMouseEvent*event);

    /*************
    * 函数名:mouseReleaseEvent
    * 功能:鼠标离开事件
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    virtual void mouseReleaseEvent(QMouseEvent*event);

    /*************
    * 函数名:InitNationCode
    * 功能:初始化国家码
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void InitNationCode();

    /*************
    * 函数名:on_cBNation_currentIndexChanged
    * 功能:当前国家码选择修改
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void on_cBNation_currentIndexChanged(int index);

signals:
    /*************
    * 函数名:sigNickNameChanged
    * 功能: 当前的昵称发生变化
    * 时间: 2016/06/22
    * 作者: hwx
    *****************/
    void sigNickNameChanged(const QString &sNickName);

protected:
    /*************
    * 函数名:paintEvent
    * 功能:重绘事件
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *)
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

private:
    Ui::DialogUserMessageEdit *ui;
    bool m_bIsNeedUpdateData;
    int m_nGenderType;
    int m_UserType;
    loginLibMgr* m_pLoginLibMgr;
    QPoint m_dragPosition;
    QString m_sCountryCode;
};

#endif // DIALOGUSERMESSAGEEDIT_H
