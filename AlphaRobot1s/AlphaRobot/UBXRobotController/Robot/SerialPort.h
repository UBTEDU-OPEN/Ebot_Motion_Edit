/*
**	FILENAME			CSerialPort.h
**
**	PURPOSE				This class can read, write and watch one serial port.
**						It sends messages to its owner when something happends on the port
**						The class creates a thread for reading and writing so the main
**						program is not blocked.
**
**	CREATION DATE		15-09-1997
**	LAST MODIFICATION	12-11-1997
**
**	AUTHOR				Remon Spekreijse
**
**
************************************************************************************
**  author: mrlong date:2007-12-25
**
**  �Ľ�
**	1) ���� ClosePort
**	2) ���� WriteToPort ��������
**	3) ���� SendData �� RecvData ����
************************************************************************************
************************************************************************************
**  author��liquanhai date:2011-11-04
**
**  �Ľ�
**	1) ���� ClosePort �н�������Ȩ����ֹ��������
**	2) ���� ReceiveChar �з�ֹ�߳�����
************************************************************************************
************************************************************************************
**  author��viruscamp date:2013-12-04
**
**  �Ľ�
**	1) ���� IsOpen �ж��Ƿ�����
**	2) ���� InitPort �� parity Odd Even ����ȡֵ����
**	3) �޸� InitPort �� portnr ȡֵ��Χ��portnr>9 ʱ���⴦��
**	4) ȡ���� MFC ��������ʹ�� HWND ���� CWnd��ʹ�� win32 thread ���������� MFC ��
**	5) �����û���Ϣ�����Զ��壬�������� CnComm
*/

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#include "IMyPortInterface.h"
#include <QCoreApplication>

extern const UINT  WM_MSG_BREAK_DETECTED;     // A break was detected on input.
extern const UINT  WM_MSG_CTS_DETECTED;       // The CTS (clear-to-send) signal changed state.
extern const UINT  WM_MSG_DSR_DETECTED;       // The DSR (data-set-ready) signal changed state.
extern const UINT  WM_MSG_ERR_DETECTED;       // A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.
extern const UINT  WM_MSG_RING_DETECTED;      // A ring indicator was detected.
extern const UINT  WM_MSG_RLSD_DETECTED;      // The RLSD (receive-line-signal-detect) signal changed state.
extern const UINT  WM_MSG_RXCHAR;             // A character was received and placed in the input buffer.
extern const UINT  WM_MSG_RXFLAG_DETECTED;    // The event character was received and placed in the input buffer.
extern const UINT  WM_MSG_TXEMPTY_DETECTED;   // The last character in the output buffer was sent.
extern const UINT  WM_MSG_RXSTRING;           // A string was received and placed in the input buffer.

class CSerialPort : public IMyPortInterface
{														 
public:
	// contruction and destruction
    CSerialPort(QObject *hListener);
	virtual		~CSerialPort();

	// port initialisation											
	/*BOOL		InitPort(HWND pPortOwner, UINT portnr = 1, UINT baud = 19200, 
				char parity = 'N', UINT databits = 8, UINT stopsbits = 1, 
				DWORD dwCommEvents = EV_RXCHAR | EV_CTS, UINT nBufferSize = 512,
			
				DWORD ReadIntervalTimeout = 1000,
				DWORD ReadTotalTimeoutMultiplier = 1000,
				DWORD ReadTotalTimeoutConstant = 1000,
				DWORD WriteTotalTimeoutMultiplier = 1000,
				DWORD WriteTotalTimeoutConstant = 1000);*/

    BOOL		InitPort(const QString& sPortName, UINT baud = 19200,
		char parity = 'N', UINT databits = 8, UINT stopsbits = 1, 
		DWORD dwCommEvents = EV_RXCHAR | EV_CTS, UINT nBufferSize = 512,

		DWORD ReadIntervalTimeout = 1000,
		DWORD ReadTotalTimeoutMultiplier = 1000,
		DWORD ReadTotalTimeoutConstant = 1000,
		DWORD WriteTotalTimeoutMultiplier = 1000,
		DWORD WriteTotalTimeoutConstant = 1000);


	// start/stop comm watching
	virtual BOOL StartMonitoring();
	BOOL		RestartMonitoring();
	virtual BOOL StopMonitoring();

	DWORD		GetWriteBufferSize();
	DWORD		GetCommEvents();
	DCB&		GetDCB();

	void		WriteToPort(char* string);
	void		WriteToPort(char* string,int n); // add by mrlong 2007-12-25
	void		WriteToPort(LPCTSTR string);	 // add by mrlong 2007-12-25
	void		WriteToPort(BYTE* Buffer, int n);// add by mrlong
	
	
	virtual BOOL ClosePort();					 // add by mrlong 2007-12-2  
	virtual BOOL IsOpen();
	
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
	virtual BOOL SetParams(int nBandrate, ...);

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
	* 函数名: GetBandrate
	* 功能: 获取波特率
	* 时间: 2015/10/26 20:00
	* 作者: ZDJ
	*/
	int GetBandrate();

	/**************************************************************************
	* 函数名: SetBandrate
	* 功能: 设置波特率
	* 时间: 2015/10/26 20:01
	* 作者: ZDJ
	*/
	void SetBandrate(int nBandrate);

	/**************************************************************************
	* 函数名: Init
	* 功能: 初始化
	* 时间: 2015/10/27 19:49
	* 作者: ZDJ
	*/
	virtual BOOL Init();

	/**************************************************************************
	* 函数名: FindMyDevice
	* 功能: 查找设备
	* 参数: 
	*    @[in ] dwVID: VID值
	*    @[in ] dwPID: PID值
	*    @[out] sPortPath: 端口名称，如COM1
	* 返回值: 成功返回TRUE，失败返回FALSE
	* 时间: 2015/10/28 10:09
	* 作者: ZDJ
	*/
    BOOL FindMyDevice(DWORD dwVID, DWORD dwPID, QString& sPortName);


protected:
	//BOOL SendData(LPCTSTR lpszData, const int nLength);   //���ڷ��ͺ��� by mrlong 2008-2-15

#if 0
    BOOL RecvData(LPTSTR lpszData, const int nSize);	  //���ڽ��պ��� by mrlong 2008-2-15
#endif
	// protected memberfunctions
	void		ProcessErrorMessage(char* ErrorText);
	static DWORD WINAPI CommThread(LPVOID pParam);
	static void	ReceiveChar(CSerialPort* port);
	static void	WriteChar(CSerialPort* port);

	// thread
	//CWinThread*			m_Thread;
	HANDLE			    m_Thread;

	// synchronisation objects
	CRITICAL_SECTION	m_csCommunicationSync;
	BOOL				m_bThreadAlive;

	// handles
	HANDLE				m_hShutdownEvent;  //stop�������¼�
	HANDLE				m_hPort;		   // read  
	HANDLE				m_hWriteEvent;	 // write

	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				m_hEventArray[3];

	// structures
	OVERLAPPED			m_ov;
	COMMTIMEOUTS		m_CommTimeouts;
	DCB					m_dcb;

	// owner window
	//CWnd*				m_pOwner;
	//HWND				m_hListener;


	// misc
	UINT				m_nPortNr;		//?????
	char*				m_szWriteBuffer;
	//const wchar_t*				m_szWriteBuffer;
	DWORD				m_dwCommEvents;
	DWORD				m_nWriteBufferSize;

	int				    m_nWriteSize; //add by mrlong 2007-12-25

	int                 m_nBandrate;  //波特率
};

#endif __SERIALPORT_H__
