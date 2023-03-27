#pragma once

#define IP_PLACE_LEN 4  //IP位数

class CIPClass
{
public:
	CIPClass(void);
	CIPClass(const CString& sIP);
	virtual ~CIPClass(void);

public:
	/**************************************************************************
	* 函数名: GetIPStr
	* 功能: 获取IP字符串，如192.168.1.1
	* 时间: 2015/09/21 13:19
	* 作者: ZDJ
	*/
	CString GetIPStr();

	/**************************************************************************
	* 函数名: SetIPStr
	* 功能: 设置IP
	* 参数: 
	*    @[in ] sIP: IP字符串，如192.168.1.1
	* 返回值: void
	* 时间: 2015/09/21 13:19
	* 作者: ZDJ
	*/
	void SetIPStr(const CString& sIP);

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
	void SetIPStr(BYTE field1, BYTE field2, BYTE field3, BYTE field4);

	/**************************************************************************
	* 函数名: operator <
	* 功能: 重载小于号
	* 参数: 
	*    @[in ] ipobj: 要比较的IP对象
	* 返回值: 小于返回TRUE，否则返回FALSE
	* 时间: 2015/09/21 13:20
	* 作者: ZDJ
	*/
	BOOL operator < (const CIPClass& ipobj);

	/**************************************************************************
	* 函数名: operator >
	* 功能: 重载大于号
	* 参数: 
	*    @[in ] ipobj: 要比较的IP对象
	* 返回值: 大于返回TRUE，否则返回FALSE
	* 时间: 2015/09/21 13:20
	* 作者: ZDJ
	*/
	BOOL operator > (const CIPClass& ipobj);

	/**************************************************************************
	* 函数名: operator ==
	* 功能: 重载等于号
	* 参数: 
	*    @[in ] ipobj: 要比较的IP对象
	* 返回值: 等于返回TRUE，否则返回FALSE
	* 时间: 2015/09/21 13:20
	* 作者: ZDJ
	*/
	BOOL operator == (const CIPClass& ipobj);

	/**************************************************************************
	* 函数名: operator []
	* 功能: 重载下标运算
	* 参数: 
	*    @[in ] index: IP位索引
	* 返回值: IP位
	* 时间: 2015/09/21 13:20
	* 作者: ZDJ
	*/
	BYTE& operator [] (const int index);

	/**************************************************************************
	* 函数名: Compare
	* 功能: 比较IP对象
	* 参数: 
	*    @[in ] ipobj: IP对象
	* 返回值: 小于返回-1，大于返回1，等于返回0
	* 时间: 2015/09/21 13:30
	* 作者: ZDJ
	*/
	int Compare(const CIPClass& ipobj);

	/**************************************************************************
	* 函数名: IsValid
	* 功能: IP是否合法
	* 时间: 2015/09/21 13:30
	* 作者: ZDJ
	*/
	BOOL IsValid();

	/**************************************************************************
	* 函数名: dwIP2strIP
	* 功能: 长整型IP转换为字符串
	* 参数: 
	*    @[in ] dwIP: 长整型IP
	* 返回值: IP字符串
	* 时间: 2015/09/21 13:29
	* 作者: ZDJ
	*/
	static CString dwIP2strIP(DWORD dwIP);

	/**************************************************************************
	* 函数名: strIP2dwIP
	* 功能: IP字符串转换为长整型
	* 参数: 
	*    @[in ] sIP: IP字符串
	* 返回值: 长整型IP
	* 时间: 2015/09/21 13:28
	* 作者: ZDJ
	*/
	static DWORD strIP2dwIP(const CString& sIP);

private:
	BYTE m_fields[IP_PLACE_LEN];  //IP数据

};

