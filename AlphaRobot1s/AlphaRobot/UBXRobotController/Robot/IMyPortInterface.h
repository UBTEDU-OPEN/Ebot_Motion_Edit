/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：IMyPortInterface.h
* 创建时间：2015/10/24 12:34
* 文件标识：
* 文件摘要：端口基类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/10/24 12:34
* 版本摘要：
*/
#ifndef IMYPORTINTERFACE_H
#define IMYPORTINTERFACE_H

typedef int (*PortRecvDataFCB)(void* pUser, byte* pData, int nDataLen);


class IMyPortInterface
{
public:
    IMyPortInterface(QObject* hListener, int nPortType)
	{
		m_hListener = hListener;
		m_nPortType = nPortType;

        m_pPortRecvDataFCB = NULL;
        m_pUser = NULL;
	}

    virtual ~IMyPortInterface()
    {
        m_pPortRecvDataFCB = NULL;
        m_pUser = NULL;
    }

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
	virtual BOOL SendData(byte* pData, int nDataLen) = 0;

	/**************************************************************************
	* 函数名: SetParams
	* 功能: 设置端口参数
	* 参数: 
	*    @[in ] nBandrate: 波特率
	*    @[in ] ...: 变长参数
	* 返回值: 成功返回TRUE，失败返回FALSE
	* 时间: 2015/10/26 11:22
	* 作者: ZDJ
	*/
	virtual BOOL SetParams(int nBandrate, ...) { return TRUE; }

	/**************************************************************************
	* 函数名: IsOpen
	* 功能: 端口是否打开
	* 时间: 2015/10/26 11:24
	* 作者: ZDJ
	*/
	virtual BOOL IsOpen() = 0;

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
	virtual BOOL OpenPort(DWORD dwVID, DWORD dwPID) = 0;

	/**************************************************************************
	* 函数名: ClosePort
	* 功能: 关闭端口
	* 返回值: 成功返回TRUE，失败返回FALSE
	* 时间: 2015/10/26 13:40
	* 作者: ZDJ
	*/
	virtual BOOL ClosePort() = 0;

	/**************************************************************************
	* 函数名: StartMonitoring
	* 功能: 启动监视
	* 时间: 2015/10/26 15:57
	* 作者: ZDJ
	*/
	virtual BOOL StartMonitoring() = 0;

	/**************************************************************************
	* 函数名: StopMonitoring
	* 功能: 停止监视
	* 时间: 2015/10/26 15:57
	* 作者: ZDJ
	*/
	virtual BOOL StopMonitoring() = 0;

	/**************************************************************************
	* 函数名: Init
	* 功能: 初始化
	* 时间: 2015/10/27 19:49
	* 作者: ZDJ
	*/
	virtual BOOL Init() = 0;

	/**************************************************************************
	* 函数名: GetPortType
	* 功能: 获取端口类型
	* 时间: 2015/10/28 17:37
	* 作者: ZDJ
	*/
	int GetPortType() const
	{
		return m_nPortType;
	}

public:
    PortRecvDataFCB m_pPortRecvDataFCB;
    void* m_pUser;

protected:
    QObject*		  m_hListener;     //监听者
	int           m_nPortType;     //端口类型
};

#endif
