/*
**	FILENAME			CSerialPort.cpp
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
**  2007-12-25 mrlong https://code.google.com/p/mycom/
**  2011-11-06 liquanhai http://blog.csdn.net/liquanhai/article/details/6941574
**  2013-12-04 viruscamp http://github.com/viruscamp/CSerialPort
**
*/

#include "stdafx.h"
#include "SerialPort.h"
#include "RobotController.h"
#include <assert.h>
 
const UINT  WM_MSG_BREAK_DETECTED =   QEvent::registerEventType();     // A break was detected on input.
const UINT  WM_MSG_CTS_DETECTED =     QEvent::registerEventType();       // The CTS (clear-to-send) signal changed state.
const UINT  WM_MSG_DSR_DETECTED =     QEvent::registerEventType();       // The DSR (data-set-ready) signal changed state.
const UINT  WM_MSG_ERR_DETECTED =     QEvent::registerEventType();       // A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.
const UINT  WM_MSG_RING_DETECTED =    QEvent::registerEventType();      // A ring indicator was detected.
const UINT  WM_MSG_RLSD_DETECTED =    QEvent::registerEventType();      // The RLSD (receive-line-signal-detect) signal changed state.
const UINT  WM_MSG_RXFLAG_DETECTED =  QEvent::registerEventType();    // The event character was received and placed in the input buffer.
const UINT  WM_MSG_TXEMPTY_DETECTED = QEvent::registerEventType();   // The last character in the output buffer was sent.
const UINT  WM_MSG_RXSTRING =         QEvent::registerEventType();           // A string was received and placed in the input buffer.

//
// Constructor
//
CSerialPort::CSerialPort(QObject* hListener)
	: IMyPortInterface(hListener, ePortTypeVCP)
{
	m_hPort = NULL;

	// initialize overlapped structure members to zero
	m_ov.Offset = 0;
	m_ov.OffsetHigh = 0;
	//memset(&m_ov, 0, sizeof(m_ov));

	// create events
	m_ov.hEvent = NULL;
	m_hWriteEvent = NULL;
	m_hShutdownEvent = NULL;

	m_szWriteBuffer = NULL;

	m_bThreadAlive = FALSE;
	m_nWriteSize = 1;

	///
	m_Thread = NULL;
	m_hShutdownEvent = NULL;

	memset(m_hEventArray, 0, sizeof(m_hEventArray));
	//memset(&m_CommTimeouts, 0, sizeof(m_CommTimeouts));
	//memset(&m_dcb, 0, sizeof(m_dcb));

	m_nPortNr = 0;		//?????
	m_dwCommEvents = 0;
	m_nWriteBufferSize = 0;
	m_nBandrate = 115200;
}

//
// Delete dynamic memory
//
CSerialPort::~CSerialPort()
{
	do
	{
		SetEvent(m_hShutdownEvent);
	} while (m_bThreadAlive);

	if (m_hPort != NULL)
	{
		CloseHandle(m_hPort);
		m_hPort = NULL;
	}
	// Close Handles  
	if(m_hShutdownEvent!=NULL)
	{
		CloseHandle( m_hShutdownEvent); 
		m_hShutdownEvent = NULL;
	}
	if(m_ov.hEvent!=NULL)
	{
		CloseHandle( m_ov.hEvent ); 
		m_ov.hEvent = NULL;
	}
	if(m_hWriteEvent!=NULL)
	{
		CloseHandle( m_hWriteEvent ); 
		m_hWriteEvent = NULL;
	}

	//TRACE("Thread ended\n");

	SAFE_DELETE_ARRAY(m_szWriteBuffer);
}

BOOL CSerialPort::InitPort(const QString &sPortName,  //portname , COM1
    UINT  baud,			// baudrate
    char  parity,		// parity
    UINT  databits,		// databits
    UINT  stopbits,		// stopbits
    DWORD dwCommEvents,	// EV_RXCHAR, EV_CTS etc
    UINT  nBufferSize, // size to the writebuffer

    DWORD   ReadIntervalTimeout,
    DWORD   ReadTotalTimeoutMultiplier,
    DWORD   ReadTotalTimeoutConstant,
    DWORD   WriteTotalTimeoutMultiplier,
    DWORD   WriteTotalTimeoutConstant )

{
	BOOL bRet = FALSE;

	if (m_bThreadAlive)
	{
		do
		{
			SetEvent(m_hShutdownEvent);
		} while (m_bThreadAlive);
	}

	// create events
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	else
		m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	else
		m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	else
		m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// initialize the event objects
	m_hEventArray[0] = m_hShutdownEvent;	// highest priority
	m_hEventArray[1] = m_ov.hEvent;
	m_hEventArray[2] = m_hWriteEvent;

	// initialize critical section
	InitializeCriticalSection(&m_csCommunicationSync);

	m_nBandrate = baud;

	SAFE_DELETE_ARRAY(m_szWriteBuffer);
	m_szWriteBuffer = new char[nBufferSize];

	m_nWriteBufferSize = nBufferSize;
	m_dwCommEvents = dwCommEvents;

	char szBaud[64] = {0};

	// now it critical!
	EnterCriticalSection(&m_csCommunicationSync);

	// if the port is already opened: close it
	if (m_hPort != NULL)
	{
		CloseHandle(m_hPort);
		m_hPort = NULL;
	}

	// prepare port strings
    QString strSzPort = "\\\\.\\" + sPortName;
    //strSzPort.Format(_T("\\\\.\\%s"), sPortName);

    QString sPortNum = sPortName.mid(3);
    m_nPortNr = sPortNum.toInt(); //COM3取3

	// stop is index 0 = 1 1=1.5 2=2
	int mystop = 0;
	int myparity = 0;
	switch(stopbits)
	{
	case 0:
		mystop = ONESTOPBIT;
		break;
	case 1:
		mystop = ONE5STOPBITS;
		break;
	case 2:
		mystop = TWOSTOPBITS;
		break;
	}

	myparity = 0;
	parity = toupper(parity);
	switch(parity)
	{
	case 'N':
		myparity = 0;
		break;
	case 'O':
		myparity = 1;
		break;
	case 'E':
		myparity = 2;
		break;
	case 'M':
		myparity = 3;
		break;
	case 'S':
		myparity = 4;
		break;
	}

	sprintf(szBaud, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, mystop);

    WCHAR wszPortName[STRING_LEN_512] = {0};
    strSzPort.toWCharArray(wszPortName);

	do 
	{
		// get a handle to the port
        m_hPort = CreateFile(wszPortName,						// communication port string (COMX)
			GENERIC_READ | GENERIC_WRITE,	// read/write types
			0/*FILE_SHARE_READ | FILE_SHARE_WRITE*/,								// comm devices must be opened with exclusive access
			NULL,							// no security attributes
			OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
			FILE_FLAG_OVERLAPPED,			// Async I/O
			0);							// template must be 0 for comm devices

		if (m_hPort == INVALID_HANDLE_VALUE)
		{
			DWORD dwLastErr = GetLastError();
#ifndef QT_NO_DEBUG
            qDebug("CreateFile ERROR:%X\n", dwLastErr);
#endif
			CloseHandle(m_hPort);
			m_hPort = NULL;

			break;
		}

		// set the timeout values
		m_CommTimeouts.ReadIntervalTimeout		 = ReadIntervalTimeout;
		m_CommTimeouts.ReadTotalTimeoutMultiplier  = ReadTotalTimeoutMultiplier;
		m_CommTimeouts.ReadTotalTimeoutConstant	= ReadTotalTimeoutConstant;
		m_CommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;
		m_CommTimeouts.WriteTotalTimeoutConstant   = WriteTotalTimeoutConstant;

		// configure
		if (!SetCommTimeouts(m_hPort, &m_CommTimeouts))
		{
			ProcessErrorMessage("SetCommTimeouts()");
			break;
		}
		if (!SetCommMask(m_hPort, dwCommEvents))
		{
			ProcessErrorMessage("SetCommMask()");
			break;
		}
				
		if(!GetCommState(m_hPort, &m_dcb))
		{
			ProcessErrorMessage("GetCommState()");
			break;
		}

		m_dcb.EvtChar = 'q';
		m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		// set RTS bit high!
		m_dcb.BaudRate = baud;  // add by mrlong
		m_dcb.Parity   = myparity;
		m_dcb.ByteSize = databits;
		m_dcb.StopBits = mystop;

		if(!SetCommState(m_hPort, &m_dcb))
		{
			ProcessErrorMessage("SetCommState()");
			break;
		}

		// flush the port
		if(!PurgeComm(m_hPort, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT))
		{
			ProcessErrorMessage("PurgeComm()");
			break;
		}

		bRet = TRUE;
	} while (0);

	// release critical section
	LeaveCriticalSection(&m_csCommunicationSync);

	if(!bRet)
	{
		CloseHandle(m_hPort);
		m_hPort = NULL;
	}

	return bRet;
}

//
//  The CommThread Function.
//
DWORD WINAPI CSerialPort::CommThread(LPVOID pParam)
{
	// Cast the void pointer passed to the thread back to
	// a pointer of CSerialPort class
	CSerialPort *port = (CSerialPort*)pParam;
	
	// Set the status variable in the dialog class to
	// TRUE to indicate the thread is running.
	port->m_bThreadAlive = TRUE;	
		
	// Misc. variables
	DWORD BytesTransfered = 0; 
	DWORD Event = 0;
	DWORD CommEvent = 0;
	DWORD dwError = 0;
	COMSTAT comstat;
	BOOL  bResult = TRUE;
		
	// Clear comm buffers at startup
	if (port->m_hPort)		// check if the port is opened
		PurgeComm(port->m_hPort, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// begin forever loop.  This loop will run as long as the thread is alive.
	for (;;) 
	{ 

		// Make a call to WaitCommEvent().  This call will return immediatly
		// because our port was created as an async port (FILE_FLAG_OVERLAPPED
		// and an m_OverlappedStructerlapped structure specified).  This call will cause the 
		// m_OverlappedStructerlapped element m_OverlappedStruct.hEvent, which is part of the m_hEventArray to 
		// be placed in a non-signeled state if there are no bytes available to be read,
		// or to a signeled state if there are bytes available.  If this event handle 
		// is set to the non-signeled state, it will be set to signeled when a 
		// character arrives at the port.

		// we do this for each port!

		bResult = WaitCommEvent(port->m_hPort, &Event, &port->m_ov);

		if (!bResult)  
		{ 
			// If WaitCommEvent() returns FALSE, process the last error to determin
			// the reason..
			switch (dwError = GetLastError()) 
			{ 
			case ERROR_IO_PENDING: 	
				{ 
					// This is a normal return value if there are no bytes
					// to read at the port.
					// Do nothing and continue
					break;
				}
			case 87:
				{
					// Under Windows NT, this value is returned for some reason.
					// I have not investigated why, but it is also a valid reply
					// Also do nothing and continue.
					break;
				}
			default:
				{
					// All other error codes indicate a serious error has
					// occured.  Process this error.
					port->ProcessErrorMessage("WaitCommEvent()");
					break;
				}
			}
		}
		else
		{
			// If WaitCommEvent() returns TRUE, check to be sure there are
			// actually bytes in the buffer to read.  
			//
			// If you are reading more than one byte at a time from the buffer 
			// (which this program does not do) you will have the situation occur 
			// where the first byte to arrive will cause the WaitForMultipleObjects() 
			// function to stop waiting.  The WaitForMultipleObjects() function 
			// resets the event handle in m_OverlappedStruct.hEvent to the non-signelead state
			// as it returns.  
			//
			// If in the time between the reset of this event and the call to 
			// ReadFile() more bytes arrive, the m_OverlappedStruct.hEvent handle will be set again
			// to the signeled state. When the call to ReadFile() occurs, it will 
			// read all of the bytes from the buffer, and the program will
			// loop back around to WaitCommEvent().
			// 
			// At this point you will be in the situation where m_OverlappedStruct.hEvent is set,
			// but there are no bytes available to read.  If you proceed and call
			// ReadFile(), it will return immediatly due to the async port setup, but
			// GetOverlappedResults() will not return until the next character arrives.
			//
			// It is not desirable for the GetOverlappedResults() function to be in 
			// this state.  The thread shutdown event (event 0) and the WriteFile()
			// event (Event2) will not work if the thread is blocked by GetOverlappedResults().
			//
			// The solution to this is to check the buffer with a call to ClearCommError().
			// This call will reset the event handle, and if there are no bytes to read
			// we can loop back through WaitCommEvent() again, then proceed.
			// If there are really bytes to read, do nothing and proceed.
		
			bResult = ClearCommError(port->m_hPort, &dwError, &comstat);

			if (comstat.cbInQue == 0)
				continue;
		}	// end if bResult

		// Main wait function.  This function will normally block the thread
		// until one of nine events occur that require action.
		Event = WaitForMultipleObjects(3, port->m_hEventArray, FALSE, INFINITE);

		switch (Event)
		{
		case 0:
			{
				// Shutdown event.  This is event zero so it will be
				// the higest priority and be serviced first.

			 	port->m_bThreadAlive = FALSE;
				
				// Kill this thread.  break is not needed, but makes me feel better.
				//AfxEndThread(100);
				::ExitThread(100);

				break;
			}
		case 1:	// read event
			{
				GetCommMask(port->m_hPort, &CommEvent);
				if ((CommEvent & EV_RXCHAR ) || (CommEvent & EV_CTS)) //���յ��ַ������������뻺������ 
					ReceiveChar(port);
				
				//if (CommEvent & EV_CTS) //CTS�ź�״̬�����仯
					//::SendMessage(port->m_pOwner, WM_MSG_CTS_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);

				if (CommEvent & EV_RXFLAG) //���յ��¼��ַ������������뻺������ 
                    //::SendMessage(port->m_hListener, WM_MSG_RXFLAG_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
                    QtPostMessage(port->m_hListener, WM_MSG_RXFLAG_DETECTED, NULL, 1, port->m_nPortNr);
                if (CommEvent & EV_BREAK)  //�����з����ж�
                    //::SendMessage(port->m_hListener, WM_MSG_BREAK_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
                    QtPostMessage(port->m_hListener, WM_MSG_BREAK_DETECTED, NULL, 1, port->m_nPortNr);
                if (CommEvent & EV_ERR) //������·״̬��������·״̬��������CE_FRAME,CE_OVERRUN��CE_RXPARITY
                    //::SendMessage(port->m_hListener, WM_MSG_ERR_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
                    QtPostMessage(port->m_hListener, WM_MSG_ERR_DETECTED, NULL, 1, port->m_nPortNr);
                if (CommEvent & EV_RING) //���⵽����ָʾ
                    //::SendMessage(port->m_hListener, WM_MSG_RING_DETECTED, (WPARAM) 0, (LPARAM) port->m_nPortNr);
                    QtPostMessage(port->m_hListener, WM_MSG_RING_DETECTED, NULL, 1, port->m_nPortNr);
				break;
			}  
		case 2: // write event
			{
				// Write character event from port
				WriteChar(port);
				break;
			}

		} // end switch

	} // close forever loop

	return 0;
}

//
// start comm watching
//
BOOL CSerialPort::StartMonitoring()
{
	//if (!(m_Thread = AfxBeginThread(CommThread, this)))
	if (!(m_Thread = ::CreateThread (NULL, 0, CommThread, this, 0, NULL )))
		return FALSE;
	//TRACE("Thread started\n");
	return TRUE;	
}

//
// Restart the comm thread
//
BOOL CSerialPort::RestartMonitoring()
{
	//TRACE("Thread resumed\n");
	//m_Thread->ResumeThread();
	::ResumeThread(m_Thread);
	return TRUE;	
}


//
// Suspend the comm thread
//
BOOL CSerialPort::StopMonitoring()
{
	//TRACE("Thread suspended\n");
	//m_Thread->SuspendThread();
	::SuspendThread(m_Thread);
	return TRUE;
}


//
// If there is a error, give the right message
//
void CSerialPort::ProcessErrorMessage(char* ErrorText)
{
	char *Temp = new char[200];
	
	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);

	sprintf(Temp, "WARNING:  %s Failed with the following error: \n%s\nPort: %d\n", (char*)ErrorText, lpMsgBuf, m_nPortNr); 
	//MessageBox(NULL, (LPCWSTR)Temp, _T("Application Error"), MB_ICONSTOP);

	LocalFree(lpMsgBuf);
	delete[] Temp;
}

//
// Write a character.
//
void CSerialPort::WriteChar(CSerialPort* port)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;

	DWORD BytesSent = 0;
	DWORD SendLen   = port->m_nWriteSize;
	ResetEvent(port->m_hWriteEvent);


	// Gain ownership of the critical section
	EnterCriticalSection(&port->m_csCommunicationSync);

	if (bWrite)
	{
		// Initailize variables
		port->m_ov.Offset = 0;
		port->m_ov.OffsetHigh = 0;
	 
		// Clear buffer
		PurgeComm(port->m_hPort, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

		bResult = WriteFile(port->m_hPort,							// Handle to COMM Port
							port->m_szWriteBuffer,					// Pointer to message buffer in calling finction
							SendLen,	// add by mrlong
							//strlen((char*)port->m_szWriteBuffer),	// Length of message to send
							&BytesSent,								// Where to store the number of bytes sent
							&port->m_ov);							// Overlapped structure

		// deal with any error codes
		if (!bResult)  
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
				case ERROR_IO_PENDING:
					{
						// continue to GetOverlappedResults()
						BytesSent = 0;
						bWrite = FALSE;
						break;
					}
				default:
					{
						// all other error codes
						port->ProcessErrorMessage("WriteFile()");
					}
			}
		} 
		else
		{
			LeaveCriticalSection(&port->m_csCommunicationSync);
		}
	} // end if(bWrite)

	if (!bWrite)
	{
		bWrite = TRUE;
	
		bResult = GetOverlappedResult(port->m_hPort,	// Handle to COMM port 
									  &port->m_ov,		// Overlapped structure
									  &BytesSent,		// Stores number of bytes sent
									  TRUE); 			// Wait flag

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// deal with the error code 
		if (!bResult)  
		{
			port->ProcessErrorMessage("GetOverlappedResults() in WriteFile()");
		}	
	} // end if (!bWrite)

	// Verify that the data size send equals what we tried to send
	if (BytesSent != SendLen /*strlen((char*)port->m_szWriteBuffer)*/)  // add by 
	{
		//TRACE("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", BytesSent, strlen((char*)port->m_szWriteBuffer));
	}
}

//
// Character received. Inform the owner
//
void CSerialPort::ReceiveChar(CSerialPort* port)
{
	Sleep(10);
	BOOL  bRead = TRUE; 
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	COMSTAT comstat;
	unsigned char RXBuff;
	//static vector<byte> szStrBuf;

	for (;;) 
	{ 
		//add by liquanhai 2011-11-06  ��ֹ����
		if(WaitForSingleObject(port->m_hShutdownEvent,0)==WAIT_OBJECT_0)
			return;

		// Gain ownership of the comm port critical section.
		// This process guarantees no other part of this program 
		// is using the port object. 
		
		EnterCriticalSection(&port->m_csCommunicationSync);

		// ClearCommError() will update the COMSTAT structure and
		// clear any other errors.
		
		bResult = ClearCommError(port->m_hPort, &dwError, &comstat);

		LeaveCriticalSection(&port->m_csCommunicationSync);

		// start forever loop.  I use this type of loop because I
		// do not know at runtime how many loops this will have to
		// run. My solution is to start a forever loop and to
		// break out of it when I have processed all of the
		// data available.  Be careful with this approach and
		// be sure your loop will exit.
		// My reasons for this are not as clear in this sample 
		// as it is in my production code, but I have found this 
		// solutiion to be the most efficient way to do this.
		
		if (comstat.cbInQue == 0)
		{
#if 0
			// break out when all bytes have been read
			if(szStrBuf.size() > 0)
			{

				byte* newbuf = new byte[szStrBuf.size()];
				memcpy(newbuf, szStrBuf.data(), szStrBuf.size());
				MYDATA* mydata = new MYDATA(newbuf, szStrBuf.size());

				::SendMessage(/*(port->m_pOwner)*/::AfxGetApp()->m_pMainWnd->m_hWnd, WM_MSG_RXSTRING, (WPARAM)mydata, (LPARAM) port->m_nPortNr);

				szStrBuf.clear();

			}
#endif
			break;
		}
						
		EnterCriticalSection(&port->m_csCommunicationSync);

		if (bRead)
		{
			bResult = ReadFile(port->m_hPort,		// Handle to COMM port 
							   &RXBuff,				// RX Buffer Pointer
							   1,					// Read one byte
							   &BytesRead,			// Stores number of bytes read
							   &port->m_ov);		// pointer to the m_ov structure
			// deal with the error code 
			if (!bResult)  
			{ 
				switch (dwError = GetLastError()) 
				{ 
					case ERROR_IO_PENDING: 	
						{ 
							// asynchronous i/o is still in progress 
							// Proceed on to GetOverlappedResults();
							bRead = FALSE;
							break;
						}
					default:
						{
							// Another error has occured.  Process this error.
							port->ProcessErrorMessage("ReadFile()");
							break;
						} 
				}
			}
			else
			{
				// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
				bRead = TRUE;
			}
		}  // close if (bRead)

		if (!bRead)
		{
			bRead = TRUE;
			bResult = GetOverlappedResult(port->m_hPort,	// Handle to COMM port 
										  &port->m_ov,		// Overlapped structure
										  &BytesRead,		// Stores number of bytes read
										  TRUE); 			// Wait flag

			// deal with the error code 
			if (!bResult)  
			{
				port->ProcessErrorMessage("GetOverlappedResults() in ReadFile()");
			}	
		}  // close if (!bRead)
				
		LeaveCriticalSection(&port->m_csCommunicationSync);

		// notify parent that a byte was received
      //  QtPostMessage(port->m_hListener, WM_MSG_RXCHAR, port->m_nPortNr, 1, RXBuff);
        QtPostMessage(port->m_hListener, WM_MSG_RXCHAR, port->m_nPortNr, 1, (LPARAM)&RXBuff);//by asu


		//port->NotifyObservers(WM_MSG_RXCHAR, NULL, RXBuff, port->m_nPortNr);
		//szStrBuf.push_back(RXBuff);
	} // end forever loop

}

//
// Write a string to the port
//
void CSerialPort::WriteToPort(char* string)
{		
	assert(m_hPort != 0);

	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
	strcpy(m_szWriteBuffer, string);
	m_nWriteSize=strlen(string); // add by mrlong
	// set event for write
	SetEvent(m_hWriteEvent);
}

//
// Return the device control block
//
DCB& CSerialPort::GetDCB()
{
	return m_dcb;
}

//
// Return the communication event masks
//
DWORD CSerialPort::GetCommEvents()
{
	return m_dwCommEvents;
}

//
// Return the output buffer size
//
DWORD CSerialPort::GetWriteBufferSize()
{
	return m_nWriteBufferSize;
}

BOOL CSerialPort::IsOpen()
{
	return m_hPort != NULL;
}

BOOL CSerialPort::ClosePort()
{
	MSG message;
	do
	{
		SetEvent(m_hShutdownEvent);		

        if(::PeekMessage(&message, (HWND)((QWidget*)m_hListener)->winId(), 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	} while (m_bThreadAlive);

	// if the port is still opened: close it 
	if (m_hPort != NULL)
	{
		CloseHandle(m_hPort);
		m_hPort = NULL;
	}
	
	// Close Handles  
	if(m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	if(m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	if(m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);			
		
	SAFE_DELETE_ARRAY(m_szWriteBuffer);

	return TRUE;
}

void CSerialPort::WriteToPort(char* string,int n)
{
	assert(m_hPort != 0);
	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
	memcpy(m_szWriteBuffer, string, n);
	m_nWriteSize = n;

	// set event for write
	SetEvent(m_hWriteEvent);
}

void CSerialPort::WriteToPort(LPCTSTR string)
{
	assert(m_hPort != 0);
	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
	//_tcscmp(m_szWriteBuffer, string);
	strcpy(m_szWriteBuffer, (char*)string);
	m_nWriteSize=strlen((char*)string);
	// set event for write
	SetEvent(m_hWriteEvent);
}

void CSerialPort::WriteToPort(BYTE* Buffer, int n)
{
	assert(m_hPort != 0);
	memset(m_szWriteBuffer, 0, sizeof(m_szWriteBuffer));
	int i;
	for(i=0; i<n; i++)
	{
		m_szWriteBuffer[i] = Buffer[i];
	}
	m_nWriteSize=n;

	// set event for write
	SetEvent(m_hWriteEvent);
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
BOOL CSerialPort::SendData(byte* pData, int nDataLen)
{
	assert(m_hPort != 0);	

	memset(m_szWriteBuffer, 0, nDataLen);
	//strcpy(m_szWriteBuffer, (char*)lpszData);
	memcpy(m_szWriteBuffer, pData, nDataLen);
	m_nWriteSize = nDataLen;
	// set event for write
	DWORD dwByteWrite;

	BOOL bRet = WriteFile(this->m_hPort,							// Handle to COMM Port
		m_szWriteBuffer,					// Pointer to message buffer in calling finction
		m_nWriteSize,	// add by mrlong
		//strlen((char*)port->m_szWriteBuffer),	// Length of message to send
		&dwByteWrite,								// Where to store the number of bytes sent
		&m_ov);	

	//SetEvent(m_hWriteEvent);
	//Sleep(1);
	//if(!bRet)
	//	return FALSE;

#ifndef QT_NO_DEBUG
    QString sStr = "VCP sended ";
	for(int i = 0; i < nDataLen; i++)
	{
        QString sTemp;
        sTemp.sprintf("%02X ", (byte)pData[i]);
        sStr += sTemp;
	}
    //sStr += "\n";
    qDebug(sStr.toStdString().c_str());
#endif

    ////////////////////////////////////////////////Log//////////////
     QString sLog = "VCP sended ";
     for(int i = 0; i < nDataLen; i++)
     {
         QString sTemp;
         sTemp.sprintf("%02X ", (byte)pData[i]);
         sLog += sTemp;
     }
     AlphaRobotLog::getInstance()->printDebug(sLog);
     ////////////////////////////////////////////////Log//////////////

    if(g_bIsRecordLogValue)
    {
        QString sStr = "VCP sended ";
        for(int i = 0; i < nDataLen; i++)
        {
            QString sTemp;
            sTemp.sprintf("%02X ", (byte)pData[i]);
            sStr += sTemp;
        }
        sStr += "\n";
        AlphaRobotLog::getInstance()->printInfo(sStr);
    }

	return TRUE;
}

#if 0
BOOL CSerialPort::RecvData(LPTSTR lpszData, const int nSize)
{
	//
	//��������
	//
	assert(m_hPort!=0);
	memset(lpszData,0,nSize);
	DWORD mylen  = 0;
	DWORD mylen2 = 0;
	while (mylen<nSize) {
		if(!ReadFile(m_hPort,lpszData,nSize,&mylen2,NULL)) 
			return FALSE;
		mylen += mylen2;

	}
	
	return TRUE;
}
#endif

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
BOOL CSerialPort::SetParams(int nBandrate, ...)
{
	/*va_list va_args;
	va_start(va_args, nBandrate);

	UINT nPort = va_arg(va_args, UINT);
	UINT nBandrate = va_arg(va_args, UINT);

	va_end(va_args);

	return InitPort(hWndListener, nPort, nBandrate);*/

	SetBandrate(nBandrate);

	return TRUE;
}

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
BOOL CSerialPort::OpenPort(DWORD dwVID, DWORD dwPID)
{
	//CArray<SSerInfo, SSerInfo&> arrPortList;
	//EnumSerialPorts(arrPortList, FALSE);

// 	for(int i = 0; i < arrPortList.GetCount(); i++)
// 	{
// 		//CString sDevPath = arrPortList.GetAt(i).;
// 
// 		
// 	}

    QString sPortName;
	if(!FindMyDevice(dwVID, dwPID, sPortName))
		return FALSE;
	
	if(!InitPort(sPortName, m_nBandrate))
		return FALSE;

	StartMonitoring();

	return TRUE;
}

/**************************************************************************
* 函数名: GetBandrate
* 功能: 获取波特率
* 时间: 2015/10/26 20:00
* 作者: ZDJ
*/
int CSerialPort::GetBandrate()
{
	return m_nBandrate;
}

/**************************************************************************
* 函数名: SetBandrate
* 功能: 设置波特率
* 时间: 2015/10/26 20:01
* 作者: ZDJ
*/
void CSerialPort::SetBandrate(int nBandrate)
{
	m_nBandrate = nBandrate;
}

/**************************************************************************
* 函数名: Init
* 功能: 初始化
* 时间: 2015/10/27 19:49
* 作者: ZDJ
*/
BOOL CSerialPort::Init()
{	
	return TRUE;
}

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
BOOL CSerialPort::FindMyDevice(DWORD dwVID, DWORD dwPID, QString &sPortName)
{
	BOOL bRet = FALSE;
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;

	do 
	{
		DWORD dwFlag = DIGCF_ALLCLASSES | DIGCF_PRESENT;
		hDevInfo = SetupDiGetClassDevs(NULL, L"USB", NULL,dwFlag);
		if( INVALID_HANDLE_VALUE == hDevInfo ) 
		{
			break;
		}

		SP_DEVINFO_DATA spDevInfoData;
		spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for(int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++) 
		{		
			DWORD dwDataType = 0;
			DWORD dwSize = 0 ;
            WCHAR szBuf[MAX_PATH] = {0};

			BOOL bTempRet = SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_HARDWAREID, &dwDataType, (PBYTE)szBuf, sizeof(szBuf), &dwSize);
			if(bTempRet)
			{
                QString sTemp = QString::fromWCharArray(szBuf);
                sTemp.toUpper();

				//USB\VID_0483&PID_5740&REV_0200

				int nTempVID = 0;
				int nTempPID = 0;
				int nTempREV = 0;

                sscanf(sTemp.toStdString().c_str(), "USB\\VID_%04X&PID_%04X", &nTempVID, &nTempPID);

				if(nTempVID == dwVID && nTempPID == dwPID)
				{
					memset(szBuf, 0, sizeof(szBuf));
					bTempRet = SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, SPDRP_FRIENDLYNAME, &dwDataType, (PBYTE)szBuf, sizeof(szBuf), &dwSize);
					if(bTempRet)
					{
                        //sTemp.Format(_T("%s"), szBuf);
                        sTemp = QString::fromWCharArray(szBuf);

                        int nIndex = sTemp.indexOf("(");
						if(nIndex != -1)
						{
                            sTemp = sTemp.mid(nIndex + 1);
                            nIndex = sTemp.indexOf(")");
							if(nIndex != -1)
							{
                                sTemp = sTemp.mid(0, nIndex);
                                sTemp.trimmed();

								sPortName = sTemp;
								bRet = TRUE;
								break;
							}
						}
					}
				}
			}
		}

	} while (0);

	if(hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	return bRet;
}
