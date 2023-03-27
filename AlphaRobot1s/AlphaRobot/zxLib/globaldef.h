/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：globaldef.h
* 创建时间：2015/08/24 22:05
* 文件标识：
* 文件摘要：全局定义
* 
* 当前版本：1.0.0.0
* 作    者：zdj
* 完成时间：2015/08/24 22:05
* 版本摘要：
*/
#ifndef GLOBALDEF_H
#define GLOBALDEF_H

#include <set>
#include <map>
#include <vector>
#include <queue>
#include <string.h>
using namespace std;

#include <QtCore/qglobal.h>

#ifdef ZXLIB_EXPORTS
#define ZXLIB_DLL       Q_DECL_EXPORT
#define ZXLIB_TEMPLATE  Q_DECL_EXPORT
#else
#define ZXLIB_DLL       Q_DECL_IMPORT
#define ZXLIB_TEMPLATE
#endif


#ifdef Q_OS_WIN
#include <windows.h>
#include <Dbt.h>
#include <process.h>

#else

#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <CoreFoundation/CoreFoundation.h>

#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include <semaphore.h>
#include <pthread.h>

#endif

//#pragma region 数据类型定义

typedef unsigned char   byte;
typedef byte            int8;
typedef short           int16;
typedef int             int32;
typedef long long       int64;

typedef byte           BYTE;
//typedef char           TCHAR;
typedef unsigned long long  ULONGLONG;
//typedef unsigned long ULONG;

typedef unsigned int     UINT_PTR;
typedef ULONG        DWORD;
typedef DWORD       DWORD_PTR;
typedef unsigned short  WORD;
typedef unsigned int    UINT;
typedef UINT            WPARAM;
typedef long            LPARAM;
typedef int             BOOL;

typedef void*           PVOID;
typedef PVOID          LPVOID;
typedef LPVOID        HANDLE;

#ifndef Q_OS_WIN
typedef ULONGLONG   LRESULT;
typedef HANDLE       HWND;
typedef unsigned short   WCHAR;
#endif

typedef UINT          MMRESULT;

typedef void(*PVOIDFUNC)(void);

#define TRUE  1
#define FALSE 0
#define NULL  0

#define IN
#define OUT

#ifdef Q_OS_WIN
#define SELECTANY __declspec(selectany)
#else
#define SELECTANY
#endif

#define SAFE_DELETE(p)       if(p){ delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) if(p){ delete [] p; p = NULL;}
#define SAFE_FREE(p)         if(p){ free(p); p = NULL;}
#define SAFE_CLOSEHANDLE(p)  if(p){ CloseHandle(p); p = NULL;}

#define GETSTR(s) #s

#define STRING_LEN_32    32
#define STRING_LEN_64    64
#define STRING_LEN_128   128
#define STRING_LEN_256   256
#define STRING_LEN_512   512
#define STRING_LEN_1024  1024

#ifdef Q_OS_WIN
#define STDCALL __stdcall
#else
#define STDCALL
#define CALLBACK

#endif

#define STRUCT_INIT(T)\
T()\
{\
	memset(this, 0, sizeof(T));\
}\
T(const T& obj)\
{\
	if(this != &obj)\
		memcpy(this, &obj, sizeof(T));\
}\
T& operator = (const T& obj)\
{\
	if(this != &obj)\
		memcpy(this, &obj, sizeof(T));\
	return *this;\
}

#define STRUCT_CONSTRUCT_DEFAULT(T)\
T()\
{\
	memset(this, 0, sizeof(T));\
}

#define STRUCT_CONSTRUCT_COPY(T)\
T(const T& obj)\
{\
	if(this != &obj)\
		memcpy(this, &obj, sizeof(T));\
}

#define STRUCT_OPERATOR_ASSIGN(T)\
T& operator = (const T& obj)\
{\
	if(this != &obj)\
		memcpy(this, &obj, sizeof(T));\
	return *this;\
}

#define ZXPROPERTY(NAME, TYPE, VARIATE)\
	public:                   \
		TYPE Get##NAME()      \
		{					  \
			return VARIATE;   \
		}					  \
		void Set##NAME(TYPE var)\
		{					  \
			VARIATE = var;    \
		}

#define ZXPROPERTY_GET(NAME, TYPE, VARIATE)\
	public:                   \
		TYPE Get##NAME()      \
		{					  \
			return VARIATE;   \
		}

#define ZXPROPERTY_SET(NAME, TYPE, VARIATE)\
	public:                     \
		void Set##NAME(TYPE var)\
		{					    \
			VARIATE = var;      \
		}


#define ZXPROPERTY_REF(NAME, TYPE, VARIATE)\
	public:                   \
		TYPE& Get##NAME()     \
		{					  \
			return VARIATE;   \
		}					  \
		void Set##NAME(const TYPE& var)\
		{					  \
			VARIATE = var;    \
		}

#define ZXPROPERTY_REF_GET(NAME, TYPE, VARIATE)\
	public:                   \
		TYPE& Get##NAME()     \
		{					  \
			return VARIATE;   \
		}					  

#define ZXPROPERTY_REF_SET(NAME, TYPE, VARIATE)\
	public:                   \
		void Set##NAME(const TYPE& var)\
		{					  \
			VARIATE = var;    \
		}

//#pragma endregion

//#pragma region

//#pragma endregion

//#pragma region 全局函数声明

/**************************************************************************
* 函数名: print_msg
* 功能: 打印消息
* 参数: 
*    @[in ] pszMsg: 消息字符串
* 返回值: void
* 时间: 2015/08/27 14:07
* 作者: ZDJ
*/
ZXLIB_DLL void print_msg(char* pszFormt, ...);

/**************************************************************************
* 函数名: getrandom
* 功能: 获取指定范围的随机数
* 参数: 
*    @[in ] nMin: 最小值
*    @[in ] nMax: 最大值
* 返回值: 随机数
* 时间: 2015/08/28 12:02
* 作者: ZDJ
*/
ZXLIB_DLL int getrandom(int nMin, int nMax);

#ifdef Q_OS_WIN
/**************************************************************************
* 函数名: IsThreadActive
* 功能: 判断线程是否处于运行状态
* 参数: 
*    @[in ] hThread: 线程句柄
* 返回值: 运行返回TRUE，否则返回FALSE
* 时间: 2015/07/28 15:36
* 作者: ZDJ
*/
ZXLIB_DLL BOOL IsThreadActive(HANDLE hThread);
#else
/**************************************************************************
* 函数名: IsThreadActive
* 功能: 判断线程是否处于运行状态
* 参数:
*    @[in ] nThreadId: 线程ID
* 返回值: 运行返回TRUE，否则返回FALSE
* 时间: 2015/07/28 15:36
* 作者: ZDJ
*/
BOOL IsThreadActive(pthread_t nThreadId);

/**************************************************************************
* 函数名: Sleep
* 功能: 线程睡眠
* 参数:
*    @[in ] milsec：毫秒数
* 返回值: void
* 时间: 2016/01/08 10:35
* 作者: ZDJ
*/
void Sleep(qint64 milsec);

#endif
//#pragma endregion

#endif
