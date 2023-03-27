/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：UBXUpdateAdministrator.cpp
* 创建时间：2016/07/23 16:17
* 文件标识：
* 文件摘要：更新模块管理
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/07/23 16:17
* 版本摘要：
*/

#include "ubxupdateadministrator.h"

#include "ciniconfig.h"

/**************************************************************************
* 函数名: getInstance
* 功能: 获取更新模块管理
* 时间: 2016/07/23 16:39
* 作者: hwx
*/
UBXUpdateAdministrator *UBXUpdateAdministrator::getInstance()
{
    static UBXUpdateAdministrator updateAdm;
    return &updateAdm;
}

QString UBXUpdateAdministrator::getUpdateBasicUrlRobotUpdateLoad()
{
   //http://10.10.20.30:8020/file/download?access_token=94d67d4f94c24616b704ffbdadb32930&model=1-v1.0&robot_id=NBA19123ECC7A26A341B2E51ACC08AB2&robot_type=zqd
   // 下载升级文件接口示例：
   // http://120.25.57.42:8084/upgrade/file/download?access_token=94d67d4f94c24616b704ffbdadb32930&model=1-v1.0&robot_id=NBA19123ECC7A26A341B2E51ACC08AB2&robot_type=zqd

    QString strServer;
    CIniConfig::getInstance().getValueServerRobotUpate(strServer);

    QString strUpateURL = strServer + "/file/download";
    return strUpateURL;
}

QString UBXUpdateAdministrator::getUpdateBasicUrlRobotUpdateToken()
{
    // http://10.10.20.30:8020/oauth/token?client_id=mobile_1&client_secret=secret_1&grant_type=password&username=aa&password=aa
    // 正式环境获取token接口：
    // http://120.25.57.42:8084/upgrade/oauth/token?client_id=mobile_1&client_secret=secret_1&grant_type=password&username=aa&password=aa

     QString strServer;
     CIniConfig::getInstance().getValueServerRobotUpate(strServer);

     QString strUpateURL = strServer + "/oauth/token";
     return strUpateURL;
}

/**************************************************************************
* 函数名: OnNotify
* 功能: 通知消息
* 参数:
*    @[in ] pSubject: 被观察者
*    @[in ] nMsgID: 消息ID
*    @[in ] dwContext: 消息上下文
*    @[in ] wParam: 参数1
*    @[in ] lParam: 参数2
* 返回值: void
* 时间: 2015/08/22 19:09
* 作者: zdj
*/
void UBXUpdateAdministrator::OnNotify(const zxObserverSubjectBase *pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    if(nMsgID == WM_MSG_ROBOT_SWITCHMODE_RESULT)
    {
        if (m_pSysEmbedUpdateWidget && m_pSysEmbedUpdateWidget->isForceCopyUpateVersion())
        {
            // 切换U盘模式
           emit sigOnNotifyChangedUdiskResult((bool) wParam);
        }
    }
    else if (nMsgID == WM_MSG_ROBOT_CONNECT)
    {
        if (wParam)
        {
             AlphaRobotLog::getInstance()->printDebug("Get robot connect in UBXUpdateAdministrator");
             getSysEmbedUpdateWidget();  // 构建对象，为后续使用准备，因为连接上机器人后，软件会自动触发查询嵌入式版本的命令，自动走嵌入式版本更新流程
        }
    }
    else if (nMsgID == WM_MSG_ROBOT_DISCONNECT)  // 断开连接
    {
        // 移除设备，非正常断开连接
        if (lParam)
        {
            getSysEmbedUpdateWidget()->onRemoveDevice();
        }
    }
}

void UBXUpdateAdministrator::onExit()
{
    CancelDownload();
}

void UBXUpdateAdministrator::updateSystemEmbedVersion()
{
    SystemEmbeddedUpdateWidget *pUpdateWidget = getSysEmbedUpdateWidget();
    if (pUpdateWidget)
    {
        pUpdateWidget->slotOnUpdate();
    }
}

UBXUpdateAdministrator::UBXUpdateAdministrator(QObject *parent) : QObject(parent)
   ,m_pSysEmbedUpdateWidget(NULL)
   ,m_pSystemPCSoftVersionUI(NULL)
   ,m_pVideoTutorialHandle(NULL)
{
    if(g_pRobotController)
    {
        AddSubscribe(g_pRobotController); //添加通信消息订阅
    }
}

UBXUpdateAdministrator::~UBXUpdateAdministrator()
{
    SAFE_DELETE(m_pSysEmbedUpdateWidget);
    SAFE_DELETE(m_pSystemPCSoftVersionUI);
}

CVideoTutorialHandle *UBXUpdateAdministrator::getVideoTutorialHandle()
{
    if(NULL == m_pVideoTutorialHandle)
    {
        m_pVideoTutorialHandle = new CVideoTutorialHandle();
    }

    return m_pVideoTutorialHandle;
}

SystemEmbeddedUpdateWidget *UBXUpdateAdministrator::getSysEmbedUpdateWidget()
{
    if(NULL == m_pSysEmbedUpdateWidget)
    {
        m_pSysEmbedUpdateWidget = new SystemEmbeddedUpdateWidget();

        connect(m_pSysEmbedUpdateWidget, &SystemEmbeddedUpdateWidget::sigSystemEmbedVersionUpdateStatus,
                this, &UBXUpdateAdministrator::sigSystemEmbedVersionUpdateStatus);
        connect(m_pSysEmbedUpdateWidget, &SystemEmbeddedUpdateWidget::sigOnGetSystemEmbedVersionInfo,
                     this, &UBXUpdateAdministrator::sigOnGetSystemEmbedVersionInfo);
        connect(m_pSysEmbedUpdateWidget, &SystemEmbeddedUpdateWidget::sigIsSystemEmbedVersionNeedCopyUpdate,
                this, &UBXUpdateAdministrator::sigIsSystemEmbedVersionNeedCopyUpdate);
        connect(this, &UBXUpdateAdministrator::sigOnNotifyChangedUdiskResult, m_pSysEmbedUpdateWidget, &SystemEmbeddedUpdateWidget::slotOnChangedToUDiskResult);
    }

    return m_pSysEmbedUpdateWidget;
}

void UBXUpdateAdministrator::slotOnTimeout()
{
      getSysEmbedUpdateWidget()->doCheckUpdate(false);
}

systemPCSoftVersionUI *UBXUpdateAdministrator::getSystemPCSoftVersionUI()
{
    if(NULL == m_pSystemPCSoftVersionUI)
    {
        m_pSystemPCSoftVersionUI = new systemPCSoftVersionUI();
    }

    return m_pSystemPCSoftVersionUI;
}

/**************************************************************************
* 函数名: checkUpdateSystemsEmbedded
* 功能: 检查嵌入式版本更新
* 时间: 2016/07/23 16:39
* 作者: hwx
*/
bool UBXUpdateAdministrator::checkUpdateSystemsEmbedded(bool bCanShowMBox)
{
    getSysEmbedUpdateWidget()->doCheckUpdate(bCanShowMBox);
    return true;
}


void UBXUpdateAdministrator::showSytemPCSoftVersionUI()
{
    getSystemPCSoftVersionUI()->initUI();
    getSystemPCSoftVersionUI()->show();
}
