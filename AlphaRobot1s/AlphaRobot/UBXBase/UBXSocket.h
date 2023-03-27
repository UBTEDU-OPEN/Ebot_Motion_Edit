// AVSocket.h
//
#ifndef __UBX_SOCKET_H__
#define __UBX_SOCKET_H__

#ifdef WIN32
typedef const struct sockaddr* LPCSOCKADDR;
#else


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include "stable.h"

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr_in  *LPSOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr  *LPSOCKADDR;
typedef const struct sockaddr* LPCSOCKADDR;
typedef struct in_addr IN_ADDR;
typedef struct in_addr *PIN_ADDR;
typedef struct in_addr  *LPIN_ADDR;

typedef  int    SOCKET;
typedef  unsigned short USHORT;
//typedef  unsigned long  ULONG;
typedef  const char*    LPCSTR;
typedef  char*			LPSTR;
typedef  const char*    LPCTSTR;
typedef  unsigned int   uint;
typedef  char* 	LPTSTR;
//typedef  unsigned long HANDLE;
typedef char TCHAR;	

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)

#endif

class UBXSockAddr : public sockaddr_in
{
// constructors
public:
	UBXSockAddr (void)
	{
		sin_family = AF_INET;
		sin_port = 0;
		sin_addr.s_addr = 0;
	}

	UBXSockAddr (const SOCKADDR& sa)
	{
		memcpy (this, &sa, sizeof(SOCKADDR));
	}

	UBXSockAddr (const SOCKADDR_IN& sin)
	{
		memcpy (this, &sin, sizeof (SOCKADDR_IN));
	}

	UBXSockAddr (const ULONG ulAddr, const USHORT ushPort = 0)
	{
		sin_family = AF_INET;
		sin_port = htons (ushPort);
		sin_addr.s_addr = htonl (ulAddr);
	}

	UBXSockAddr (const char* pchIP, const USHORT ushPort = 0)
	{
		sin_family = AF_INET;
		sin_port = htons (ushPort);
		sin_addr.s_addr = inet_addr(pchIP);
	}

public:	
	// Return the address in dotted-decimal format
	LPSTR DottedDecimal (void)
	{
		return inet_ntoa (sin_addr);
	}
	
	// Get port and address (even though they're public)
	USHORT Port (void) const
	{
		return ntohs (sin_port);
	}

	ULONG IPAddr (void) const
	{
		return ntohl (sin_addr.s_addr);
	}
	
	// operators added for efficiency
	const UBXSockAddr& operator=(const SOCKADDR& sa)
	{
		memcpy (this, &sa, sizeof(SOCKADDR));
		return *this;
	}

	const UBXSockAddr& operator=(const SOCKADDR_IN& sin)
	{
		memcpy (this, &sin, sizeof(SOCKADDR_IN));
		return *this;
	}

	operator SOCKADDR()
	{
		return *((LPSOCKADDR) this);
	}

	operator LPSOCKADDR()
	{
		return (LPSOCKADDR) this;
	}

	operator LPSOCKADDR_IN()
	{
		return (LPSOCKADDR_IN) this;
	}
};

// member functions truly block and must not be used in UI threads
// use this class as an alternative to the MFC CSocket class
class UBXSocket
{
// constructors
public:
	UBXSocket (void)
	{
		m_hSocket = 0;
	}

public:	
	void Cleanup (void);
	bool Create (int nType = SOCK_STREAM);
	void Close (void);
	static void Close(SOCKET hSocket);
	bool Bind(LPCSOCKADDR psa);
	bool Bind(char*szIp,unsigned short usPort);
	bool Listen();
	bool Connect(LPCSOCKADDR psa);
	bool Connect(LPCSOCKADDR psa,int nSecs);
	bool Accept (UBXSocket& s, LPSOCKADDR psa);
	int Send (const char* pch, const int nSize);
	int Send (const char* pch, const int nSize, const int nSecs);
	int Receive (char* pch, const int nSize);
	int Receive (char* pch, const int nSize, const int nSecs);
	bool GetPeerAddr (LPSOCKADDR psa);
	bool GetSockAddr (LPSOCKADDR psa);
	static UBXSockAddr GetHostByName (const char* pchName, const USHORT ushPort = 0);
	static const char* GetHostbyAddr (LPCSOCKADDR psa);
	static bool GetHostIPAddressByName(const char* cszHostName, char* lpszHostIPAddress);
	operator SOCKET ()
	{
		return m_hSocket;
	}

public:
	int ReceiveAll (char* pch, const int nSize);
	int ReceiveAll (char* pch, const int nSize, int nSecs);
	SOCKET m_hSocket;

private:
	void EncDec(char *pch, int nSize);
};

unsigned long UBXGetTickCount(void);
unsigned long GetLocalHostName(int nIndex);
char* UBX_strncpy(char* str1, const char* str2, int max);
WCHAR* UBX_strncpy(WCHAR* str1, const WCHAR* str2, int max);
void UBX_sleep(unsigned long ulTime);

#endif
