//此文件需要重写//

// avsock.cpp
#ifdef WIN32
    #include <winsock2.h>
#endif

#include "stable.h"
#include "UBXSocket.h"
//#include <iostream.h>

void UBXSocket::Cleanup (void)
{
    if (m_hSocket == 0 || m_hSocket == INVALID_SOCKET)
        return;

#ifdef WIN32
    // Set Linger
    linger   lig;
    lig.l_onoff=1;
    lig.l_linger=0;
    int   ilen=sizeof(linger);
    setsockopt(m_hSocket,SOL_SOCKET,SO_LINGER,(char*)&lig,ilen);

    shutdown(m_hSocket,2);
    closesocket (m_hSocket);
#else
    // Set Linger
    linger   lig;
    lig.l_onoff=1;
    lig.l_linger=0;
    int   ilen=sizeof(linger);
    setsockopt(m_hSocket,SOL_SOCKET,SO_LINGER,(char*)&lig,ilen);

    shutdown(m_hSocket,SHUT_RDWR);
    close(m_hSocket);
#endif
    m_hSocket = 0;
}

bool UBXSocket::Create (int nType)
{
    if (m_hSocket != 0) return true;

    if((m_hSocket = socket (AF_INET, nType, 0)) == INVALID_SOCKET) return false;

    return true;
}

bool UBXSocket::Bind(LPCSOCKADDR psa)
{
    if (m_hSocket == 0)	return false;

    if (bind (m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) return false;

    return true;
}

bool UBXSocket::Bind(char*szIp,unsigned short usPort)
{
    SOCKADDR_IN sa;
    memset(&sa,0,sizeof(SOCKADDR));
    sa.sin_family = PF_INET;
    sa.sin_addr.s_addr = szIp == 0 ? htonl(INADDR_ANY) : inet_addr(szIp);
    sa.sin_port = htons(usPort);

    return Bind((SOCKADDR*)&sa);
}

bool UBXSocket::Listen (void)
{
    if (m_hSocket == 0)	return false;

    if(listen(m_hSocket, 10240) == SOCKET_ERROR) return false;

    return true;
}

bool UBXSocket::Accept (UBXSocket& sConnect, LPSOCKADDR psa)
{
    if (m_hSocket == 0)	return false;

    if (sConnect.m_hSocket != 0)	return false;

#ifdef WIN32
    int nLengthAddr = sizeof (SOCKADDR);
#else
    socklen_t nLengthAddr = sizeof (SOCKADDR);
#endif

    sConnect.m_hSocket = accept (m_hSocket, psa, &nLengthAddr);
    if (sConnect == INVALID_SOCKET)	return false;

    return true;
}

void UBXSocket::Close (void)
{
    if (0 == m_hSocket) return;

#ifdef WIN32
    shutdown(m_hSocket,2);
    closesocket (m_hSocket);
#else
    shutdown(m_hSocket,SHUT_RDWR);
    close(m_hSocket);
#endif
    m_hSocket = 0;
}

void UBXSocket::Close(SOCKET hSocket)
{
    if (0 == hSocket) return;

#ifdef WIN32
    shutdown(hSocket,2);
    closesocket (hSocket);
#else
    shutdown(hSocket,SHUT_RDWR);
    close(hSocket);
#endif
    hSocket = 0;
}

bool UBXSocket::Connect (LPCSOCKADDR psa)
{
    if (m_hSocket == 0) return false;

    if (connect(m_hSocket, psa, sizeof(SOCKADDR)) == SOCKET_ERROR) return false;

    return true;
}

bool UBXSocket::Connect(LPCSOCKADDR psa,int nSecs)
{
#ifndef WIN32
    return Connect(psa);
#else
    //set the socket in non-blocking mode
    unsigned long	argp = 1;

    ioctlsocket(m_hSocket,FIONBIO,&argp);

    connect(m_hSocket,psa,sizeof(SOCKADDR_IN));
    //FD_SET fd = {1, m_hSocket};
    fd_set fd = {1, m_hSocket};
    timeval tv = {nSecs, 0};
    int	nResult = select(0, 0, &fd, 0, &tv);

    //set the socket in blocking mode again.
    argp = 0;

    ioctlsocket(m_hSocket,FIONBIO,&argp);
    return (nResult!=0);
#endif
}


int UBXSocket::Send (const char* pch, const int nSize)
{
    //char *p = NULL;

    if (m_hSocket == 0 || nSize == 0)
        return 0;

    /*p = new char[nSize];
    if (p == NULL)
        return 0;

    memcpy(p, pch, nSize);
    EncDec(p, nSize);*/

    int nBytesSent;
    if ((nBytesSent = send (m_hSocket, pch, nSize,
#ifdef WIN32
        0
#else
#ifdef UNIX
        0
#else
        0
#endif
#endif
        )) == SOCKET_ERROR)
        nBytesSent=0;

    //delete []p;
    return nBytesSent;
}

int UBXSocket::Send (const char* pch, const int nSize, const int nSecs)
{
    //return Send(pch,nSize);
    //char *p = NULL;

    if (m_hSocket == 0 || nSize == 0)
        return 0;

#ifdef WIN32
    FD_SET fd = {1, m_hSocket};
    TIMEVAL tv = {nSecs, 0};
    if (select (0, NULL, &fd, NULL, &tv) == 0)
        return 0;
#else
    fd_set  set;
    struct  timeval tv;
    int     err;

    tv.tv_sec = nSecs;
    tv.tv_usec = 0;

    FD_ZERO(&set);

    FD_SET(m_hSocket, &set);

    err = select(m_hSocket+1, &set, NULL, NULL, &tv);
    if (err <= 0)
        return 0;
#endif

    /*p = new char[nSize];
    if (p == NULL)
        return 0;

    memcpy(p, pch, nSize);
    EncDec(p, nSize);*/

    int nBytesSent=0;
    if ((nBytesSent = send (m_hSocket, pch, nSize,
#ifdef WIN32
        0
#else
#ifdef UNIX
        0
#else
        0
#endif
#endif
        )) == SOCKET_ERROR)
    {
        nBytesSent = 0;
    }

    //delete []p;
    return nBytesSent;
}

int UBXSocket::Receive (char* pch, const int nSize)
{
    if (m_hSocket == 0)
        return 0;


    int nBytesReceived;
    if ((nBytesReceived = recv (m_hSocket, pch, nSize,
#ifdef WIN32
        0
#else
#ifdef  UNIX
        0
#else
        0
#endif
#endif
        )) == SOCKET_ERROR) return 0;

    //EncDec(pch, nBytesReceived);
    return nBytesReceived;
}

int UBXSocket::Receive (char* pch, const int nSize, const int nSeconds)
{
    int nSecs=nSeconds;
    if (m_hSocket == 0)
        return -1;

    int     err;
#ifdef WIN32
    FD_SET fd = {1, m_hSocket};
    TIMEVAL tv = {0, nSecs};
    err = select (0, &fd, NULL, NULL, &tv);
    if (err == 0)
        return 0;
    else if (err < 0)
        return -1;
#else
    fd_set  set;
    struct  timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = nSecs;

    FD_ZERO(&set);

    FD_SET(m_hSocket, &set);

    err = select(m_hSocket+1, &set, NULL, NULL, &tv);
    if (err <= 0)
        return 0;
#endif

    int nBytesReceived;
    if ((nBytesReceived = recv (m_hSocket, pch, nSize,
#ifdef WIN32
        0
#else
#ifdef  UNIX
        0
#else
        0
#endif
#endif
        )) == SOCKET_ERROR)
        return -1;

    //EncDec(pch, nBytesReceived);
    return nBytesReceived;
}

bool UBXSocket::GetPeerAddr (LPSOCKADDR psa)
{
    if (m_hSocket == 0)
        return false;

#ifdef WIN32
    int nLengthAddr = sizeof (SOCKADDR);
#else
    socklen_t nLengthAddr = sizeof (SOCKADDR);
#endif
    if (getpeername (m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR) return false;

    return true;
}

bool UBXSocket::GetSockAddr (LPSOCKADDR psa)
{
    if (m_hSocket == 0)
        return false;

#ifdef WIN32
    int nLengthAddr = sizeof (SOCKADDR);
#else
    socklen_t nLengthAddr = sizeof (SOCKADDR);
#endif
    if (getsockname (m_hSocket, psa, &nLengthAddr) == SOCKET_ERROR) return false;

    return true;
}

UBXSockAddr UBXSocket::GetHostByName (const char* pchName, const USHORT ushPort)
{
    SOCKADDR_IN sockTemp;
    hostent* pHostEnt = gethostbyname (pchName);
    if (pHostEnt == 0) return sockTemp;

    ULONG* pulAddr = (ULONG*) pHostEnt -> h_addr_list[0];
    sockTemp.sin_family = AF_INET;
    sockTemp.sin_port = htons (ushPort);
    sockTemp.sin_addr.s_addr = *pulAddr;

    return sockTemp;
}

const char* UBXSocket::GetHostbyAddr (LPCSOCKADDR psa)
{
    hostent* pHostEnt = gethostbyaddr ((char*)&((LPSOCKADDR_IN)psa) -> sin_addr.s_addr, 4,
        PF_INET);
    if (pHostEnt == 0)
        return 0;

    return pHostEnt -> h_name;
}


bool UBXSocket::GetHostIPAddressByName(const char* cszHostName, char* lpszHostIPAddress)
{
    SOCKADDR_IN sockTemp;
    hostent* pHostEnt = gethostbyname(cszHostName);
    if (pHostEnt == 0) return false;

    ULONG* pulAddr = (ULONG*) pHostEnt -> h_addr_list[0];
    sockTemp.sin_family = AF_INET;
    sockTemp.sin_port = htons (0);
    sockTemp.sin_addr.s_addr = *pulAddr;

    UBXSockAddr sa=sockTemp;

    strcpy(lpszHostIPAddress,sa.DottedDecimal());

    return true;
}

int UBXSocket::ReceiveAll(char *pch, const int nSize)
{
    if (!m_hSocket)
        return 0;

    int	nRead		= Receive(pch, nSize);
    int	nRawSize	= nRead;

    if (nRead && (nRead < nSize))
    {
        int nMore=nSize-nRawSize;

        while (nMore>0)
        {
            int nRead=Receive(pch+nRawSize, nMore);
            if (nRead<=0)
                break;
            nRawSize+=nRead;
            nMore-=nRead;
        }
    }

    return nRawSize;
}

int UBXSocket::ReceiveAll(char *pch, const int nSize, int nSecs)
{
    if (m_hSocket == 0)
        return 0;

    int	nRead		= Receive(pch, nSize, nSecs);
    int	nRawSize	= nRead;

    if (nRead && (nRead < nSize))
    {
        int nMore=nSize-nRawSize;

        while (nMore>0)
        {
            int nRead=Receive(pch+nRawSize, nMore, nSecs);
            if (nRead<=0)
                break;
            nRawSize+=nRead;
            nMore-=nRead;
        }
    }

    return nRawSize;
}

unsigned long UBXGetTickCount(void)
{
#ifdef WIN32
    return GetTickCount();//毫秒
#else
//	struct timeval now;
//	gettimeofday(&now,NULL);
//	return now.tv_sec*1000+now.tv_usec/1000;
    return 0.0;
#endif
}

unsigned long GetLocalHostName(int nIndex)
{
    char strHostName[256];
    int  nRetCode;

    if (nIndex < 0)
        nIndex = 0;
    if (nIndex > 10)
        nIndex = 10;
    nRetCode = gethostname(strHostName, 256);
    if (nRetCode != 0)
        return 0;

    hostent* pHostEnt = gethostbyname (strHostName);
    if (pHostEnt == 0)
        return 0;

    ULONG*			pulAddr		= (ULONG*) pHostEnt -> h_addr_list[0];
    unsigned long	ulIPAddress	= ntohl (*pulAddr);

    for (int i=0; i<10; i++)
    {
        pulAddr	= (ULONG*) pHostEnt -> h_addr_list[i];
        if (pulAddr)
        {
            if (i == nIndex)
            {
                ulIPAddress	= ntohl (*pulAddr);
                break;
            }
        }
        else
        {
            break;
        }
    }
    return ulIPAddress;
}

char* UBX_strncpy(char* str1, const char* str2, int max)
{
    if (strlen(str2) < max)
    {
        strcpy(str1, str2);
    }
    else
    {
        strncpy(str1, str2, max-1);
        str1[max-1] = 0;
    }
    return str1;
}

/**************************************************************************
* 函数名:UBX_strncpy
* 功能:宽字符拷贝
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
WCHAR* UBX_strncpy(WCHAR* str1, const WCHAR* str2, int max)
{
    int nLen = WCSLen((WCHAR*)str2);
    if (nLen < max)
    {
        memcpy(str1, str2,nLen);
    }
    else
    {
        memcpy(str1, str2,max-1);
//        wcsncpy(str1, str2, max-1);
        str1[max-1] = 0;
    }
    return str1;
}

void UBX_sleep(unsigned long ulTime)
{
#ifdef WIN32
    Sleep(ulTime);
#else
    usleep(ulTime*1000);
#endif
}

void UBXSocket::EncDec(char *pch, int nSize)
{
    if (pch == NULL || nSize <= 0)
        return;

    for (int i = 0; i < nSize; i++)
        pch[i] = pch[i] ^ 0x55;
}
