/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：loginlibmgr.h
* 创建时间：2016/4/16 8:52
* 文件标识：
* 文件摘要：用户登录函数定义头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/4/16 8:52
* 版本摘要：
*/
#ifndef LOGINLIBMGR_H
#define LOGINLIBMGR_H

#ifdef WINDOW
#include <afxtempl.h>
#endif
#include "ILibLogin.h"
#include "QtHttpOperSync.h"
#define STR_LEN 64
#define URL_LEN 1024

#define ALPHA_APP_TYPE 1   //客户端类型
//#define CCODE  _T("86")    //国家码


//#define g_sServer QString(_T("http://10.10.1.12:8080"))  //服务器域名
//#define g_sServer QString(_T("http://services.ubtrobot.com:8080"))  //服务器域名

UBXLOGINSHARED_EXPORT extern FILEVERINFO g_exefileinfo;  //程序版本
//UBXLOGINSHARED_EXPORT extern QString     g_sServer;      //服务器地址

UBXLOGINSHARED_EXPORT extern QString m_ZoneCode;             //手机区号
UBXLOGINSHARED_EXPORT extern int     m_userIdInfo;
UBXLOGINSHARED_EXPORT extern QString m_userToken;
UBXLOGINSHARED_EXPORT extern QString m_userNameInfo;
UBXLOGINSHARED_EXPORT extern QString m_userNickName;
UBXLOGINSHARED_EXPORT extern QString m_userGender;

// 请求登录消息
extern UBXLOGINSHARED_EXPORT const UINT NOTIFY_MSG_LOGIN_REQUEST;

//帐号类型
enum emAccountType
{
    PHONENAME = 0,// 手机号码方式, 注意该顺气与Tabwidget的index对应，不要随意调换顺序
    EMAILNAME,           //  邮箱方式
    USERNAME            // 用户名方式
};

enum emGenderType
{
    NOTKNOW,
    MALE,
    FEMALE,
};

//用户数据类型
UBXLOGINSHARED_EXPORT typedef struct USERINFO
{
    int     m_nUserID;       //用户IDrtSC
    QString m_sUserAccount;  //用户帐号
    QString m_sUserPwd;      //用户密码
    bool    m_bLogin;        //是否已登录


    USERINFO()
    {
        m_nUserID = 0;
        m_sUserAccount = NULLSTR;
        m_sUserPwd = NULLSTR;
        m_bLogin = FALSE;
    }

    USERINFO(int nId, const char* lpszAccount, const char* lpszPwd, bool bLogin)
    {
        m_nUserID = nId;
        m_sUserAccount = lpszAccount;
        m_sUserPwd = lpszPwd;
        m_bLogin = bLogin;
    }
}USERINFO;

UBXLOGINSHARED_EXPORT extern USERINFO  g_userinfo;//全局用户
extern zxCriticalSection  g_crisec;  //全局临界区

enum RequestInfo
{
    REQUEST_CONNECT_FAIL = -1,              //请求连接失败
    REQUEST_SUCCESS = 0,					//请求成功
    REQUEST_NO_RESPONSES = 1,               //请求没有回应
    USERNAMNE_OR_PASSWORD_ERROR = 1001,     //用户名或者密码不正确
    PASSWORD_ERROR = 1002,					//密码不正确
    USERNAME_ALREADY_EXIST = 1003,			//用户名已经存在
    USERNAME_NONE_ACTIVE = 1004,			//用户未激活
    USERNAME_OUTOF_DATE = 1005,				//用户已失效
    USERNAME_NONE_REGISTER = 1006,			//该邮件没注册
    QUERY_USER_IS_EXIST = 1111,             //查询用户，该用户存在
    DEVICE_CODE_NOT_EXIST = 2001,			//设备序列号不存在
    DEVICE_ALREADY_ACTIVE = 2002,			//该设备已经激活了
    BACKSTAGE_SYSTEM_ERROR = 9999,			//后台系统错误
    USER_IS_NOT_EXIST = 3001
};

extern const UINT WM_MSG_USER_REGISTER_RESULT;  //用户注册结果消息

typedef struct _ACTIONINFO
{
    int nActionId;			//动作ID
    char name[STR_LEN];		//动作名称
    int nActionType;			//动作类型
    char	url[URL_LEN];		//下载地址  如http://10.10.1.14:8080/action/9/0/1.rar
}ActionInfo,*pActionInfo;


class UBXLOGINSHARED_EXPORT loginLibMgr: public QObject
{
    Q_OBJECT
public:
    loginLibMgr(QWidget* parent = NULL);
    virtual ~loginLibMgr();

public:

    /**************************************************************************
    * 函数名: GetRequestKey
    * 功能: 获取MD5加密密钥
    * 参数:
    * 	@[in] szTime:	当前系统时间
    * 	@[in]	 nTimeLen:  时间字符串长度
    * 	@[out]  szKey:		加密密钥
    * 	@[in]	 nKeyLen:   密钥长度
    * 返回值: 加密成功返回TRUE， 否则返回FALSE
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool GetRequestKey(char * szTime,int nTimeLen, char* szKey, int nKeyLen);

    /**************************************************************************
    * 函数名: LoginMail
    * 功能: 邮箱登陆
    * 参数:
    * 	@[in ] sLoginURL: 网络地址，如http://www.test.com/version.xml
    * 	@[in]  sUserName: 用户名
    * 	@[in ] sPassWord: 密码
    * 	@[in ] sGender:   性别
    * 	@[in ] sUserID:   ID
    * 返回值: 登陆成功返回0， 其余请查看错误码
    * 时间: 2016/3/15 9:32
    * 作者: 周志平
    */
    int LoginMail(const QString& sLoginURL, const QString& sUserName, const QString& sPassWord, const QString& sGender, int& sUserID);

    /**************************************************************************
    * 函数名: LoginP
    * 功能: 手机登陆
    * 参数:
    * 	@[in ] sLoginURL: 网络地址，如http://www.test.com/version.xml
    * 	@[in]  countryCode: 国家码
    * 	@[in]  sUserName: 用户名
    * 	@[in ] sPassWord: 密码
    * 	@[in ] sGender:   性别
    ** 	@[in ] sUserID:   ID
    * 返回值: 登陆成功返回0， 其余请查看错误码
    * 时间: 2016/3/15 9:32
    * 作者: 周志平
    */
    int LoginP(const QString& sLoginURL, const QString& countryCode, const QString& sUserName, const QString& sPassWord, const QString& sGender, int& sUserID);

    /**************************************************************************
    * 函数名: LoginOut
    * 功能: 退出登录
    * 参数:
    * 	@[in ] sLoginOutURL: 退出登录网络地址，如http://10.10.1.14:8080/ubx/user/loginOut
    * 返回值: 退出登录成功返回TRUE，否则返回FALSE
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool LoginOut(const QString& sLoginOutURL);

    /**************************************************************************
    * 函数名: RegisterMail
    * 功能: 用户邮箱注册
    * 参数:
    * 	@[in ] sLoginURL: 注册地址
    * 	@[in] sUserName: 用户名
    * 	@[in ] sPassWord: 密码
    * 	@[in ] sRegisterType: 注册类型
    * 返回值: 注册成功返回0，其余看错误码
    * 时间: 2016/03/15 9:32
    * 作者: 周志平
    */
    int RegisterMail(const QString& sRegisterURL, const QString& sUserName, const QString& sPassWord, int sRegisterType);

    /**************************************************************************
    * 函数名: RegisterP
    * 功能: 用户手机注册
    * 参数:
    * 	@[in ] sLoginURL: 注册地址
    * 	@[in] countryCode: 国家码
    * 	@[in] sUserName: 用户名
    * 	@[in ] sPassWord: 密码
    * 	@[in ] sRegisterType: 注册类型
    * 返回值: 注册成功返回0，其余看错误码
    * 时间: 2016/03/15 9:32
    * 作者: 周志平
    */
    int RegisterP(const QString& sRegisterURL, const QString& countryCode, const QString& sUserName, const QString& sPassWord, int sRegisterType);

    /*************
    * 函数名:
    * 功能:
    * 参数:
    * 返回值:
    * 时间: 2016/01/01
    * 作者: 周志平
    *****************/
    int GetUserInfo(const QString& sGetUserInfoURL, const QString& sUserName, const QString& sNickName, const QString& sUserGender);

    /**************************************************************************
    * 函数名:SetUserInfo
    * 功能:设置用户信息
    * 参数:
    * 返回值:
    * 时间: 2016/01/01
    * 作者: 周志平
    **************************************************************************/
    int SetUserInfo(const QString& sSetUserInfoURL, const QString& sUserName, const QString& sNickName, const QString& sUserGender, const QString &sCountryCode);

    /**************************************************************************
    * 函数名:SetUserInfoP
    * 功能:设置手机用户信息
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    **************************************************************************/
    int SetUserInfoP(const QString& sSetUserInfoURL, const QString& sUserName, const QString& sNickName, const QString& sUserGender);

    //    bool ActionQuery(const QString& sQueryURL, int  nPage, int nPageSize, ActionInfo actionInfo[], int &nActionCount);

    /**************************************************************************
    * 函数名: FindPassWord
    * 功能: 找回密码
    * 参数:
    * 	@[in ] sFindPassWordURL: 找回密码地址
    * 参数:
    * 	@[in ] sUserName: 用户名
    * * 参数:
    * 	@[in ] nType: 密码
    * 返回值: 找回成功返回true，否则返回false
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool FindPassWord(const QString& sFindPassWordyURL, const QString& sUserName,int nType );

    /**************************************************************************
    * 函数名: SetPassWord
    * 功能: 设置密码
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool SetPassWord(const QString& sSetPassWordyURL,const QString&sUserName, const QString&sPassWord, int ntype, const QString& sVerifyCode);

    /**************************************************************************
    * 函数名: GetSmsCode
    * 功能: 获取短信验证码
    * strLanguageKey such as EN CN
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool GetSmsCode(const QString& sSetSmsCodeURL,const QString&sMobilePhone ,const QString & strLanguageKey);

    /**************************************************************************
    * 函数名: VerifySmsCode
    * 功能: 校验短信验证码
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool VerifySmsCode(const QString& sSetSmsCodeURL,const QString&sMobilePhone,const QString &scode);

    /**************************************************************************
    * 函数名: QueryUser
    * 功能: 查询用户
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    int QueryUser(const QString& sQueryUserURL,const QString&sName, int nType);

    /**************************************************************************
    * 函数名: GetPersonalInfomation
    * 功能: 获取个人信息
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool GetPersonalInfomation(const QString& sQueryUserURL/*,const QString&userID*/, QString& userName, QString& phoneNum, QString& userMail, int& nSex);

    /**************************************************************************
    * 函数名: EditPersonalInfo
    * 功能: 编辑个人信息
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool EditPersonalInfo(const QString& sQueryUserURL,const QString&sName, const QString&sEmail, const QString&sPhone, const QString&sSex/*,const QString&sUserID*/);

    /**************************************************************************
    * 函数名: QueryMyCollect
    * 功能: 查询我的收藏
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool QueryMyCollect(const QString& sQueryUserURL);

    /**************************************************************************
    * 函数名: EditPersonalImage
    * 功能: 编辑个人个头像
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    bool EditPersonalImage(const QString& sQueryUserURL, const QString&sHeadImage);

private:
    QtHttpOperSync* m_pHttpOper;
    int m_RequestTask;
public slots:
    /**************************************************************************
    * 函数名: onRequestResult
    * 功能: 请求结果响应槽
    * 时间: 2015/09/22 9:32
    * 作者: 周志平
    */
    void onRequestResult(bool bRet, char* pData, int nDataLen);

};
#endif // LOGINLIBMGR_H
