/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：MyHidClass.h
* 创建时间：2015/10/23 19:42
* 文件标识：
* 文件摘要：HID通信类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/10/23 19:42
* 版本摘要：
*/
#ifndef MYHIDCLASS_H
#define MYHIDCLASS_H

extern "C" {
#include "hidapi.h"
}

#include <QCoreApplication>

//extern const UINT WM_MSG_HID_DATA_RECV;   //HID收到数据消息
extern const UINT WM_MSG_RXCHAR;         //接收字符


class CMyHidClass : public IMyPortInterface
{
public:
    CMyHidClass(QObject *hListener);
	virtual ~CMyHidClass(void);

	/**************************************************************************
	* 函数名: OpenPort
	* 功能: 打开端口
	* 参数: 
	*    @[in ] dwVID: dwVID值
	*    @[in ] dwPID: dwPID值
	* 返回值: 成功返回TRUE，失败返回FALSE
	* 时间: 2015/10/23 19:43
	* 作者: ZDJ
	*/
	virtual BOOL OpenPort(DWORD dwVID, DWORD dwPID);

	/**************************************************************************
	* 函数名: SendData
	* 功能: 发送数据
	* 参数: 
	*    @[in ] pData: 数据缓冲区
	*    @[in ] nDataLen: 数据长度
	* 返回值: 成功返回TRUE，失败返回FALSE
	* 时间: 2015/10/24 9:49
	* 作者: ZDJ
	*/
	virtual BOOL SendData(byte* pData, int nDataLen);

	/**************************************************************************
	* 函数名: ClosePort
	* 功能: 关闭端口
	* 时间: 2015/10/26 10:43
	* 作者: ZDJ
	*/
	virtual BOOL ClosePort();

	/**************************************************************************
	* 函数名: IsOpen
	* 功能: 端口是否打开
	* 时间: 2015/10/26 11:24
	* 作者: ZDJ
	*/
	virtual BOOL IsOpen();

	/**************************************************************************
	* 函数名: StartMonitoring
	* 功能: 启动监视
	* 时间: 2015/10/26 15:57
	* 作者: ZDJ
	*/
	virtual BOOL StartMonitoring();

	/**************************************************************************
	* 函数名: StopMonitoring
	* 功能: 停止监视
	* 时间: 2015/10/26 15:57
	* 作者: ZDJ
	*/
	virtual BOOL StopMonitoring();

	/**************************************************************************
	* 函数名: Init
	* 功能: 初始化
	* 时间: 2015/10/27 19:49
	* 作者: ZDJ
	*/
	virtual BOOL Init();

#ifdef Q_OS_MAC
    /**************************************************************************
    * 函数名: SetArrivalCbk
    * 功能: 设置设备到达消息回调地址
    * 参数:
    *    @[in ] pCbk：回调地址
    * 返回值: void
    * 时间: 2016/01/16 17:26
    * 作者: ZDJ
    */
    static void SetArrivalCbk(IOHIDDeviceCallback pCbk);

    /**************************************************************************
    * 函数名: SetRemovalCbk
    * 功能: 设置设备移除消息回调地址
    * 参数:
    *    @[in ] pCbk：回调地址
    * 返回值: void
    * 时间: 2016/01/16 17:26
    * 作者: ZDJ
    */
    static void SetRemovalCbk(IOHIDDeviceCallback pCbk);

#endif
    /**************************************************************************
    * 函数名: Req_QuitRecvThread
    * 功能: 请求退出接收线程
    * 时间: 2016/03/22 10:57
    * 作者: ZDJ
    */
    void Req_QuitRecvThread();

protected:
	/**************************************************************************
	* 函数名: ThreadRecv
	* 功能: 接收数据线程执行函数
	* 参数: 
	*    @[in ] lpParam: 线程参数
	* 返回值: 执行结果
	* 时间: 2015/10/23 20:17
	* 作者: ZDJ
	*/
#ifdef Q_OS_WIN
    static DWORD __stdcall ThreadRecv(LPVOID lpParam);
#else
    static void *ThreadRecv(LPVOID lpParam);
#endif

private:
    hid_device*     m_pHidDev;     //设备操作地址
	BOOL          m_bRecv;       //接收线程运行标识
#ifdef Q_OS_WIN
	HANDLE        m_hThreadRecv; //接收线程句柄

    CRITICAL_SECTION	m_csCommunicationSync;//for sync writeread
#else
    pthread_t        m_hThreadRecv;    
#endif
};

#endif
