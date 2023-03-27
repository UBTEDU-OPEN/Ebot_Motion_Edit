/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：ILibLogin.h
* 创建时间：2015/11/03 10:15
* 文件标识：
* 文件摘要：登录接口
* 
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/4/6 10:15
* 版本摘要：
*/
#ifndef ILIBLOGIN_H
#define ILIBLOGIN_H
#include "stdafx.h"
#include "ubxlogin_global.h"

class ILoginEvent
{
public:
    virtual void SetUserLoginStatus(QString userName, QString ActionType, bool isLogin) = 0;
    virtual void setUserNickName(const QString &strNickName) = 0;
};

class ILogin
{
public:
    /*************
    * 函数名:
    * 功能:
    * 参数:
    * 返回值:
    * 时间: 2016/01/01
    * 作者: 周志平
    *****************/
    virtual bool Connect(QWidget* pParent, ILoginEvent* pNotify) = 0;

    /*************
    * 函数名:
    * 功能:
    * 参数:
    * 返回值:
    * 时间: 2016/01/01
    * 作者: 周志平
    *****************/
    virtual void Disconnect() = 0;

    /*************
    * 函数名:
    * 功能:
    * 参数:
    * 返回值:
    * 时间: 2016/01/01
    * 作者: 周志平
    *****************/
    virtual QDialog* GetDialog() = 0;

    /**************************************************************************
    * 函数名: UserLoginOut
    * 功能: 用户注销登录
    * 时间: 2015/10/30 16:37
    * 作者: ZDJ
    */
    virtual bool UserLoginOut() = 0;

    /**************************************************************************
    * 函数名: UserMessageEdit
    * 功能: 用户修改个人信息
    * 时间: 2015/10/30 16:37
    * 作者: ZDJ
    */
    virtual bool UserMessageEdit() = 0;

    /**************************************************************************
    * 函数名: GetUserName
    * 功能: 获取用户名
    * 时间: 2015/10/30 16:37
    * 作者: ZDJ
    */
    virtual QString GetUserName() = 0;

    /**************************************************************************
    * 函数名: GetUserPwd
    * 功能: 获取用户密码
    * 时间: 2015/10/30 16:37
    * 作者: ZDJ
    */
    virtual QString GetUserPwd() = 0;

    /************************************
    * 名称: GetUserID
    * 功能: 获取用户ID
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    virtual int GetUserID() = 0;
};

//-------------------------------------------------------------------
UBXLOGINSHARED_EXPORT bool CreateLoginUI(ILogin** ppUBXLoginNotify, QWidget* pParent = NULL);
#endif // ILIBLOGIN_H
