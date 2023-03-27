/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
* 
* 文件名称：LanguageMangr.cpp
* 创建时间：2015/08/28 16:57
* 文件标识：
* 文件摘要：语言管理类
* 
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/08/28 16:57
* 版本摘要：
*/
#include "StdAfx.h"
#include "LanguageMangr.h"
#include "Markup.h"

#pragma region 语言风格类实现

MapLangType CLanguageStyle::m_mapLangTypeStr;

CLanguageStyle::CLanguageStyle()
{
	m_eCurLangType = eLangTypeEnglish;  //当前语言
}
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
CLanguageStyle::CLanguageStyle(LangTypeEnum eLangType, const CString& sStylePath)
{
	m_eCurLangType = eLangTypeEnglish;  //当前语言

	m_mapLangTypeStr[eLangTypeChineseSimp] = "CHS";//语言类型字符串
	m_mapLangTypeStr[eLangTypeChineseTran] = "CHT";
	m_mapLangTypeStr[eLangTypeJapanese] = "JPA";
	m_mapLangTypeStr[eLangTypeEnglish] = "ENG";
	m_mapLangTypeStr[eLangTypeGerman] = "GER";
	m_mapLangTypeStr[eLangTypeSpanish] = "SPA";
	m_mapLangTypeStr[eLangTypeRussian] = "RUS";
	m_mapLangTypeStr[eLangTypeArabic] = "ARB";
	m_mapLangTypeStr[eLangTypeFrench] = "FRE";

	SetCurLang(eLangType);
	LoadStyle(sStylePath);            
}

/**************************************************************************
* 函数名: GetLangString
* 功能: 获取语言文本
* 参数: 
*    @[in ] sName: 文本键名
* 返回值: 语言文本
* 时间: 2015/08/28 16:36
* 作者: ZDJ
*/
CString CLanguageStyle::GetLangString(const CString& sKeyName)
{
	CString sRet;

	MapLangStrings::iterator itor = m_mapStrings.find(sKeyName);
	if(itor != m_mapStrings.end())
		sRet = itor->second;          
	return sRet;
}

/**************************************************************************
* 函数名: LoadStyle
* 功能: 加载风格文件
* 参数: 
*    @[in ] sStylePath: 风格文件路径
* 返回值: 成功返回true，失败返回false
* 时间: 2015/08/28 16:41
* 作者: ZDJ
*/
bool CLanguageStyle::LoadStyle(const CString& sStylePath)
{	
	bool bRet = false;
	do
	{
		CMarkup xmlloader;
		if(!xmlloader.Load(sStylePath))
			break;

		if(xmlloader.FindElem(L"ROOT"))
		{
			xmlloader.IntoElem();

			if(xmlloader.FindElem(L"STRINGLIST"))	
			{
				xmlloader.IntoElem();

				CString sCurLangName;
				MapLangType::iterator itor = m_mapLangTypeStr.find(m_eCurLangType);
				if (itor != m_mapLangTypeStr.end())
					sCurLangName = itor->second;

				while(xmlloader.FindElem(L"STRING"))
				{
					CString sStrName = xmlloader.GetAttrib(L"NAME");

					xmlloader.IntoElem();

					if(xmlloader.FindElem(L"VALUES"))			
					{
						xmlloader.IntoElem();								
					
						itor = m_mapLangTypeStr.begin();
						while(itor != m_mapLangTypeStr.end())
						{
							if(itor->second == sCurLangName)
							{
								if(xmlloader.FindElem(itor->second))																					
									m_mapStrings[sStrName] = xmlloader.GetData();								
							}
							itor++;
						}					

						xmlloader.OutOfElem();
					}

					xmlloader.OutOfElem();
				}
				xmlloader.OutOfElem();
			}
			xmlloader.OutOfElem();
		}

		bRet = true;
	} while (false);

	return bRet;
}

/**************************************************************************
* 函数名: SetCurLang
* 功能: 设置当前语言
* 参数: 
*    @[in ] eLangType: 语言类型
* 返回值: void
* 时间: 2015/08/28 16:40
* 作者: ZDJ
*/
void CLanguageStyle::SetCurLang(LangTypeEnum eLangType)
{
	m_eCurLangType = eLangType;
}		
#pragma endregion

#pragma region 语言管理类实现

CLanguageMangr* CLanguageMangr::m_pInstance = NULL;

CLanguageMangr::CLanguageMangr()
{
	m_pStyle = NULL;
}

CLanguageMangr::~CLanguageMangr()
{
	SAFE_DELETE(m_pStyle);
}

/**************************************************************************
* 函数名: SetCurStyle
* 功能: 设置当前语言风格
* 参数: 
*    @[in ] pNewStyle: 新风格实例
* 返回值: void
* 时间: 2015/08/28 16:54
* 作者: ZDJ
*/
void CLanguageMangr::SetCurStyle(CLanguageStyle* pNewStyle)
{
	if(m_pStyle)
		SAFE_DELETE(m_pStyle);   
	m_pStyle = pNewStyle;
}

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
void CLanguageMangr::SetCurStyle(LangTypeEnum eLangType, const CString& sStylePath)
{
	SetCurStyle(new CLanguageStyle(eLangType, sStylePath));     
}

/**************************************************************************
* 函数名: GetLangString
* 功能: 获取语言文本
* 参数: 
*    @[in ] sKeyName: 文本键名
* 返回值: 语言文本
* 时间: 2015/08/28 16:53
* 作者: ZDJ
*/
CString CLanguageMangr::GetLangString(const CString& sKeyName)
{
	return m_pStyle->GetLangString(sKeyName);
}

/**************************************************************************
* 函数名: GetInstance
* 功能: 获取语言管理器实例
* 时间: 2015/08/28 16:53
* 作者: ZDJ
*/
CLanguageMangr* CLanguageMangr::GetInstance()
{
	if(!m_pInstance)
		m_pInstance = new CLanguageMangr;
	return m_pInstance;
}

#pragma endregion
