/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：dialogloginpage.h
* 创建时间：2016/01/01
* 文件标识：
* 文件摘要：登录模块主窗体头文件
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2016/04/16 14:00
* 版本摘要：
*/
#ifndef DIALOGLOGINPAGE_H
#define DIALOGLOGINPAGE_H

#include <QDialog>
#include "loginlibmgr.h"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QPoint>
#include <QStringList>

class WidgetLogin;
class Widgetregister;
class WidgetFindPassword;
class WidgetResetPassword;

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFile;
class QLabel;
class QLineEdit;
class QProgressDialog;
class QPushButton;
class QSslError;
class QAuthenticator;
class QNetworkReply;
QT_END_NAMESPACE

namespace Ui {
class DialogLoginPage;
}
//登录方式
enum  LOGINWAY
{
    LOGIN_ERROR = -1,   //无效登录
    PHONE_LOGION,       //手机登录
    MAIL_LOGIN          //邮箱登录
};

//子窗口类型
enum CHILD_WIDGET
{
    LOGIN_WIDGET = 0,    //登录窗口
    REGISTER_WIDGET = 1, //注册窗口
    FINDPASSWORD_WIDGET, //找回密码窗口
    RESETPASSWORD_WIDGET //重置密码窗口
};

//登录模块对话框
class DialogLoginPage : public QDialog,public ILogin
{
    Q_OBJECT

public:
    explicit DialogLoginPage(QWidget *parent = 0);
    ~DialogLoginPage();
//实现ILogin接口
public:

    /*************
    * 函数名:Connect
    * 功能:连接到对话框
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    virtual bool Connect(QWidget* pParent, ILoginEvent* pNotify);

    /*************
    * 函数名:Disconnect
    * 功能:删除连接并销毁对话框
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    virtual void Disconnect();

    /*************
    * 函数名:getDefaultNationPairList
    * 功能:获取国家码列表信息
    * 参数:
    * 返回值:
    * 时间: 2016/07/02
    * 作者: hwx
    *****************/
    static QList<QPair<QString, QVariant> > getDefaultNationPairList();

    /*************
    * 函数名:getNationID
    * 功能:从带有国家码ID的信息文本中获取国家码ID
    * 参数:strNationTextWithID （as: tr("Angola(+244)"）
    * 返回值:
    * 时间: 20161008
    * 作者: asu
    *****************/
    static int getNationID(const QString& strNationTextWithID);

    /*************
    * 函数名:getNationIDByNation
    * 功能:获取国家码ID
    * 参数:strNationWithOutID （as: "Angola"）
    * 返回值:
    * 时间: 20161008
    * 作者: asu
    *****************/
    static int getNationIDByNation(const QString& strNationWithOutID);


    /*************
    * 函数名:getNationByNationID
    * 功能：利用国家码ID获取国家码
    * 参数:nNationID
    * 返回值:
    * 时间: 20161008
    * 作者: asu
    *****************/
    static QString getNationByNationID(int nNationID);

    /*************
    * 函数名:getDefaultNationText
    * 功能:获取默认国家文本，如China
    * 参数:
    * 返回值:
    * 时间: 2016/07/04
    * 作者: hwx
    *****************/
    static QString getDefaultNationText();

    /*************
    * 函数名:getNationPair
    * 功能: 根据国家码获取NationPair
    * 参数: strNationCode：国家码
    * 返回值:
    * 时间: 2016/07/06
    * 作者: hwx
    *****************/
    QPair<QString, QVariant> getNationPair(const QString &strNationCode);

    /*************
    * 函数名:GetDialog
    * 功能:获取对话框指针
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    virtual QDialog* GetDialog();

protected:

    /*************
    * 函数名:paintEvent
    * 功能:重绘事件
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    virtual void paintEvent(QPaintEvent *);

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

public:

    /**************************************************************************
    * 函数名: UserLoginOut
    * 功能: 用户注销登录
    * 时间: 2015/10/30 16:37
    * 作者: ZDJ
    */
    virtual bool UserLoginOut();

    /**************************************************************************
    * 函数名: UserMessageEdit
    * 功能: 用户修改个人信息
    * 时间: 2015/10/30 16:37
    * 作者: ZDJ
    */
    virtual bool UserMessageEdit();

    /**************************************************************************
    * 函数名: GetUserName
    * 功能: 获取用户名
    * 时间: 2015/10/30 16:37
    * 作者: ZDJ
    */
    virtual QString GetUserName();

    /**************************************************************************
    * 函数名: GetUserPwd
    * 功能: 获取用户密码
    * 时间: 2015/10/30 16:37
    * 作者: ZDJ
    */
    virtual QString GetUserPwd();

    /*************
      * 函数名:SetResetWidgetInfo
      * 功能:重置密码窗口的信息设置
      * 参数:
      * 返回值:
      * 时间: 2016/06/01
      * 作者: 周志平
      *****************/

    void SetResetWidgetInfo(QString strName,QString strVervifyCode);

    /************************************
    * 名称: GetUserID
    * 功能: 获取用户ID
    * 返回:   void
    * 时间:   2016/04/13
    * 作者:   hels
    ************************************/
    virtual int GetUserID();

    /************************************
    * 名称: getLoginWidget
    * 功能: 获取登录窗口
    * 返回:   void
    * 时间:   2016/07/02
    * 作者:   hwx
    ************************************/
    WidgetLogin * getLoginWidget();

    /************************************
    * 名称: getRegisterWidget
    * 功能: 获取注册窗口
    * 返回:   void
    * 时间:   2016/07/02
    * 作者:   hwx
    ************************************/
    Widgetregister *getRegisterWidget();

    /************************************
    * 名称: getFindPasswordWidget
    * 功能: 获取找回密码窗口
    * 返回:   void
    * 时间:   2016/07/02
    * 作者:   hwx
    ************************************/
    WidgetFindPassword *getFindPasswordWidget();

    /************************************
    * 名称: getResetPasswordWidget
    * 功能: 获取重置密码窗口
    * 返回:   void
    * 时间:   2016/07/02
    * 作者:   hwx
    ************************************/
    WidgetResetPassword *getResetPasswordWidget();

public:

    /*************
    * 函数名:setLoginStatus
    * 功能:设置登录状态
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void setLoginStatus(bool bRet);

    /*************
    * 函数名:setLoginWay
    * 功能:设置登录方式
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void setLoginWay(int nWay);

    /*************
    * 函数名:getLoginWay
    * 功能:获取登录方式
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    int getLoginWay();

    /*************
    * 函数名:ChangeChildWidget
    * 功能:根据子窗体的类型显示响应的子窗体
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void ChangeChildWidget(int nType);

protected slots:
    /*************
    * 函数名:slotNickNameChanged
    * 功能: 昵称发生变化的槽函数
    * 参数:
    * 返回值:
    * 时间: 2016/06/22
    * 作者: hwx
    *****************/
    void slotNickNameChanged(const QString &strNickName);

signals:

    /*************
    * 函数名:SigUpdataMessage
    * 功能:信号，用于更新用户名和密码信息
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void SigUpdataMessage(QString strName, QString strPassword);

private:

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
    * 功能:设置默认的皮肤风格
    * 参数:
    * 返回值:
    * 时间: 2016/06/01
    * 作者: 周志平
    *****************/
    void setDefaultStyle();

private:
    ILoginEvent* m_nEvent;
    QString m_UserName;
    QString m_strPassword;
    int m_nLoginWay;
    QPoint m_dragPosition;

    WidgetLogin * m_pWidgetLogin;
    Widgetregister* m_pWidgetRegister;
    WidgetFindPassword* m_pWidgetFindPassword;
    WidgetResetPassword* m_pWidgetResetPassword;

private:
    Ui::DialogLoginPage *ui;
    loginLibMgr* m_pLoginLibMgr;

};

#endif // DIALOGLOGINPAGE_H
