/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：LanguageMangr.h
* 创建时间：2015/08/28 16:57
* 文件标识：
* 文件摘要：语言管理类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/08/28 16:57
* 版本摘要：
*/
#pragma once
#include <map>
using namespace std;

#pragma region 语言风格类声明

//语言类型
enum LangTypeEnum
{	
	eLangTypeOthers = 0,     //其他语言
	eLangTypeChineseSimp,    //简体中文
	eLangTypeEnglish,        //英文
	eLangTypeJapanese,       //日文
	eLangTypeChineseTran,    //繁体中文
	eLangTypeGerman,         //德语
	eLangTypeSpanish,        //西班牙语
	eLangTypeRussian,        //俄语
	eLangTypeArabic,         //阿拉伯语
	eLangTypeFrench,         //法语

	//...此处添加其他语言
	eLangTypeCount
}; 

typedef map<LangTypeEnum, CString> MapLangType;

//语言风格类
class CLanguageStyle
{
private:
	CLanguageStyle();

public:
	/**************************************************************************
	* 函数名: CLanguageStyle构造函数
	* 功能: 初始化语言风格
	* 参数: 
	*    @[in ] eLangType: 语言类型
	*    @[in ] sStylePath: 语言文件路径
	* 返回值: 
	* 时间: 2015/08/28 16:35
	* 作者: ZDJ
	*/
	CLanguageStyle(LangTypeEnum eLangType, const CString& sStylePath);

	/**************************************************************************
	* 函数名: GetLangString
	* 功能: 获取语言文本
	* 参数: 
	*    @[in ] sName: 文本键名
	* 返回值: 语言文本
	* 时间: 2015/08/28 16:36
	* 作者: ZDJ
	*/
	CString GetLangString(const CString& sKeyName);

private:

	/**************************************************************************
	* 函数名: LoadStyle
	* 功能: 加载风格文件
	* 参数: 
	*    @[in ] sStylePath: 风格文件路径
	* 返回值: 成功返回true，失败返回false
	* 时间: 2015/08/28 16:41
	* 作者: ZDJ
	*/
	bool LoadStyle(const CString& sStylePath);

	/**************************************************************************
	* 函数名: SetCurLang
	* 功能: 设置当前语言
	* 参数: 
	*    @[in ] eLangType: 语言类型
	* 返回值: void
	* 时间: 2015/08/28 16:40
	* 作者: ZDJ
	*/
	void SetCurLang(LangTypeEnum eLangType);

private:
	LangTypeEnum m_eCurLangType;           //当前语言类型
	
	typedef map<CString, CString> MapLangStrings;
	MapLangStrings m_mapStrings;           //字符串字典 <"File", "文件">

	
	static MapLangType m_mapLangTypeStr;   //语言类型字符串映射
};

#pragma endregion

#pragma region 语言管理类声明

//语言管理类
class CLanguageMangr
{
public:	
	CLanguageMangr();
	~CLanguageMangr();

	/**************************************************************************
	* 函数名: SetCurStyle
	* 功能: 设置当前语言风格
	* 参数: 
	*    @[in ] pNewStyle: 新风格实例
	* 返回值: void
	* 时间: 2015/08/28 16:54
	* 作者: ZDJ
	*/
	void SetCurStyle(CLanguageStyle* pNewStyle);

	/**************************************************************************
	* 函数名: SetCurStyle
	* 功能: 设置当前风格
	* 参数: 
	*    @[in ] eLangType: 语言类型
	*    @[in ] sStylePath: 语言风格文件路径
	* 返回值: void
	* 时间: 2015/08/28 16:55
	* 作者: ZDJ
	*/
	void SetCurStyle(LangTypeEnum eLangType, const CString& sStylePath);

	/**************************************************************************
	* 函数名: GetLangString
	* 功能: 获取语言文本
	* 参数: 
	*    @[in ] sKeyName: 文本键名
	* 返回值: 语言文本
	* 时间: 2015/08/28 16:53
	* 作者: ZDJ
	*/
	CString GetLangString(const CString& sKeyName);

	/**************************************************************************
	* 函数名: GetInstance
	* 功能: 获取语言管理器实例
	* 时间: 2015/08/28 16:53
	* 作者: ZDJ
	*/
	static CLanguageMangr* GetInstance();

private:
	CLanguageStyle* m_pStyle;          //当前语言风格
	static CLanguageMangr* m_pInstance;//语言管理器实例
};

#pragma endregion