#include "StdAfx.h"
#include "IPClass.h"
#include "Util.h"

CIPClass::CIPClass(void)
{
	memset(m_fields, 0, sizeof(m_fields));
}

CIPClass::CIPClass(const CString& sIP)
{
	SetIPStr(sIP);
}


CIPClass::~CIPClass(void)
{
}

/**************************************************************************
* 函数名: GetIPStr
* 功能: 获取IP字符串，如192.168.1.1
* 时间: 2015/09/21 13:19
* 作者: ZDJ
*/
CString CIPClass::GetIPStr()
{
	CString sStr;
	sStr.Format(_T("%d.%d.%d.%d"),m_fields[0], m_fields[1], m_fields[2], m_fields[3]);
	return sStr;
}

/**************************************************************************
* 函数名: SetIPStr
* 功能: 设置IP
* 参数: 
*    @[in ] sIP: IP字符串，如192.168.1.1
* 返回值: void
* 时间: 2015/09/21 13:19
* 作者: ZDJ
*/
void CIPClass::SetIPStr(const CString& sIP)
{
	vector<CString> vecIpData = CUtil::StrSplit(sIP, '.');
	ASSERT(vecIpData.size() == IP_PLACE_LEN);

	for(int i = 0; i < IP_PLACE_LEN; i++)
		m_fields[i] = _tstoi(vecIpData[i]);
}

/**************************************************************************
* 函数名: SetIPStr
* 功能: 设置IP
* 参数: 
*    @[in ] field1: IP第1位
*    @[in ] field2: IP第2位
*    @[in ] field3: IP第3位
*    @[in ] field4: IP第4位
* 返回值: void
* 时间: 2015/09/21 13:19
* 作者: ZDJ
*/
void CIPClass::SetIPStr(BYTE field1, BYTE field2, BYTE field3, BYTE field4)
{
	m_fields[0] = field1;
	m_fields[1] = field2;
	m_fields[2] = field3;
	m_fields[3] = field4;
}

/**************************************************************************
* 函数名: operator <
* 功能: 重载小于号
* 参数: 
*    @[in ] ipobj: 要比较的IP对象
* 返回值: 小于返回TRUE，否则返回FALSE
* 时间: 2015/09/21 13:20
* 作者: ZDJ
*/
BOOL CIPClass::operator < (const CIPClass& ipobj)
{
	return Compare(ipobj) < 0;
}

/**************************************************************************
* 函数名: operator >
* 功能: 重载大于号
* 参数: 
*    @[in ] ipobj: 要比较的IP对象
* 返回值: 大于返回TRUE，否则返回FALSE
* 时间: 2015/09/21 13:20
* 作者: ZDJ
*/
BOOL CIPClass::operator > (const CIPClass& ipobj)
{
	return Compare(ipobj) > 0;
}

/**************************************************************************
* 函数名: operator ==
* 功能: 重载等于号
* 参数: 
*    @[in ] ipobj: 要比较的IP对象
* 返回值: 等于返回TRUE，否则返回FALSE
* 时间: 2015/09/21 13:20
* 作者: ZDJ
*/
BOOL CIPClass::operator == (const CIPClass& ipobj)
{	
	return Compare(ipobj) == 0;
}

/**************************************************************************
* 函数名: operator []
* 功能: 重载下标运算
* 参数: 
*    @[in ] index: IP位索引
* 返回值: IP位
* 时间: 2015/09/21 13:20
* 作者: ZDJ
*/
BYTE& CIPClass::operator [] (const int index)
{
	return m_fields[index];
}

/**************************************************************************
* 函数名: Compare
* 功能: 比较IP对象
* 参数: 
*    @[in ] ipobj: IP对象
* 返回值: 小于返回-1，大于返回1，等于返回0
* 时间: 2015/09/21 13:30
* 作者: ZDJ
*/
int CIPClass::Compare(const CIPClass& ipobj)
{
	for(int i = 0; i < IP_PLACE_LEN; i++)
	{
		if(m_fields[i] < ipobj.m_fields[i])
			return -1;
		else if(m_fields[i] > ipobj.m_fields[i])
			return 1;
	}

	return 0;
}

/**************************************************************************
* 函数名: IsValid
* 功能: IP是否合法
* 时间: 2015/09/21 13:30
* 作者: ZDJ
*/
BOOL CIPClass::IsValid()
{


	return TRUE;
}

/**************************************************************************
* 函数名: dwIP2strIP
* 功能: 长整型IP转换为字符串
* 参数: 
*    @[in ] dwIP: 长整型IP
* 返回值: IP字符串
* 时间: 2015/09/21 13:29
* 作者: ZDJ
*/
CString CIPClass::dwIP2strIP(DWORD dwIP)
{
	CString sIP;

	BYTE* iparr = (BYTE*)&dwIP;
	sIP.Format(_T("%u.%u.%u.%u"),iparr[3],iparr[2],iparr[1],iparr[0]);
	return sIP;
}

/**************************************************************************
* 函数名: strIP2dwIP
* 功能: IP字符串转换为长整型
* 参数: 
*    @[in ] sIP: IP字符串
* 返回值: 长整型IP
* 时间: 2015/09/21 13:28
* 作者: ZDJ
*/
DWORD CIPClass::strIP2dwIP(const CString& sIP)
{
	DWORD dwip = 0;
#ifdef _UNICODE
	USES_CONVERSION;
	dwip = inet_addr(W2A(sIP));
#else
	dwip = inet_addr(sIP);
#endif
	//sIP.ReleaseBuffer();
	dwip = ntohl(dwip);
	return dwip;
};