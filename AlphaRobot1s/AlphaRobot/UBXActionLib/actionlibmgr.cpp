/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ActionLibMgr.cpp
* 创建时间：2016/03/03 18:20
* 文件标识：
* 文件摘要：动作库管理器
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/03/03 18:20
* 版本摘要：
*/
#include "stdafx.h"
#include "actionlibmgr.h"

ActionLibMgr::ActionLibMgr(QWidget *parent)
{
    m_pHttpOper = new QtHttpOper(parent);
}

ActionLibMgr::~ActionLibMgr()
{
    SAFE_DELETE(m_pHttpOper);
}

/**************************************************************************
* 函数名: DownloadFile
* 功能: 下载远程文件
* 参数:
*    @[in ] sURL: 文件的网络下载地址
*    @[in ] sSavePath: 下载的文件本地保存路径
*    @[out] pszErr: 失败时返回的错误内容
* 返回值: 成功返回true，失败返回false
* 时间: 2015/05/14 10:20
* 作者: ZDJ
*/
bool ActionLibMgr::DownloadFile(const QString& sURL, const QString& sSavePath, char *pszErr)
{
    if(m_pHttpOper)
        return m_pHttpOper->DownloadFile(sURL, sSavePath, pszErr);
    return false;
}

/**************************************************************************
* 函数名: RequestPost
* 功能: 发起post请求
* 参数:
*    @[in ] sURL: 请求地址
*    @[in ] pszParam: 请求参数
*    @[out] pszErr: 失败时返回的错误内容
* 返回值: 成功返回true，失败返回false
* 时间: 2015/05/14 10:20
* 作者: ZDJ
*/
bool ActionLibMgr::RequestPost(const QString& sURL, char* pszParam, char* pszErr)
{
    if(m_pHttpOper)
        return m_pHttpOper->ExecuteRequest(eRequestPost, sURL, pszParam, pszErr);
    return false;
}
