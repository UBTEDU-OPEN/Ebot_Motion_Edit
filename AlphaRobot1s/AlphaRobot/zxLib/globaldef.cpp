#include "stdafx.h"
#include "globaldef.h"

//#pragma region 全局函数实现

/**************************************************************************
* 函数名: print_msg
* 功能: 打印消息
* 参数: 
*    @[in ] pszMsg: 消息字符串
* 返回值: void
* 时间: 2015/08/27 14:07
* 作者: ZDJ
*/
void print_msg(char* pszFormt, ...)
{	
#if 1
	char szBuffer[STRING_LEN_1024*8] = {0};

	va_list vlArgs;
	va_start(vlArgs, pszFormt);
    vsnprintf(szBuffer, sizeof(szBuffer)-sizeof(char), pszFormt, vlArgs);
	va_end(vlArgs);

	printf(szBuffer);

#ifdef Q_OS_WIN
    OutputDebugStringA(szBuffer);
#endif
#endif
}

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
int getrandom(int nMin, int nMax)
{
	return rand() % (nMax - nMin + 1) + nMin;
}

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
BOOL IsThreadActive(HANDLE hThread)
{
	DWORD nCode;
	BOOL bRes = GetExitCodeThread(hThread, &nCode);
	if (bRes && nCode == STILL_ACTIVE)//线程还活着
		return TRUE;
	return FALSE;
}

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
BOOL IsThreadActive(pthread_t nThreadId)
{
    int nRet = pthread_kill(nThreadId, 0);
    if(nRet == ESRCH || nRet == EINVAL)
        return FALSE;
    return TRUE;
}

/**************************************************************************
* 函数名: Sleep
* 功能: 线程睡眠
* 参数:
*    @[in ] milsec：毫秒数
* 返回值: void
* 时间: 2016/01/08 10:35
* 作者: ZDJ
*/
void Sleep(qint64 milsec)
{
    usleep(milsec * 1000);
}
#endif

//#pragma endregion
