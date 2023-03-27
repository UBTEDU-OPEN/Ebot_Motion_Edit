/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：subwidgetphasfindpassword.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块手机找回密码子窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef SUBWIDGETPHASFINDPASSWORD_H
#define SUBWIDGETPHASFINDPASSWORD_H

#include <QWidget>
#include "loginlibmgr.h"
#include "widgetfindpassword.h"
#include "../zxLib/widget/ubxwidget.h"

namespace Ui {
class SubWidgetPHasFindpassword;
}

class SubWidgetPHasFindpassword : public UBXWidget
{
    Q_OBJECT

public:
    explicit SubWidgetPHasFindpassword(QWidget *parent = 0 , loginLibMgr * pLoginLibMgr = NULL);
    ~SubWidgetPHasFindpassword();

    /*************
    * 函数名:setPhoneNumber
    * 功能:设置手机号码
    * 参数:strPhoneNumber 手机号码
    * 返回值:
    * 时间: 2016/06/23
    * 作者: hwx
    *****************/
    void setPhoneNumber(const QString &strPhoneNumber);

    RETRANSLATEUIFUNC

public slots:

    /*************
    * 函数名:onStartTime
    * 功能:启动时钟
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
   void onStartTime();

protected:

   /**************************************************************************
   * 函数名:timerEvent
   * 功能:时钟事件
   * 参数:
   * 返回值:
   * 时间:2016/06/01
   * 作者: 周志平
   **************************************************************************/
    virtual void timerEvent(QTimerEvent *event);

private slots:

   /*************
   * 函数名:onResendMsgClicked
   * 功能:重新发送短信验证码
   * 参数:
   * 返回值:
   * 时间: 2016/06/01
   * 作者: 周志平
   *****************/
   void onResendMsgClicked();

   /*************
   * 函数名:onResetPasswordClicked
   * 功能:重置密码
   * 参数:
   * 返回值:
   * 时间: 2016/06/01
   * 作者: 周志平
   *****************/
   void onResetPasswordClicked();

private:
    Ui::SubWidgetPHasFindpassword *ui;

private:
    loginLibMgr* m_pLoginLibMgr;
    WidgetFindPassword* m_pFindPassword;

    int m_nTimeoutID; //超时时钟ID
    int m_nTimeout;   //超时时间
    int m_nTimeCount;
};

#endif // SUBWIDGETPHASFINDPASSWORD_H
