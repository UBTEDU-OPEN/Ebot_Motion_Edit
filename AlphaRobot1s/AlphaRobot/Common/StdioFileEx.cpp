// StdioFileEx.cpp: implementation of the CStdioFileEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StdioFileEx.h"

// #ifdef _DEBUG
// #undef THIS_FILE
// static char THIS_FILE[]=__FILE__;
// #define new DEBUG_NEW
// #endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*static*/ const  int  CStdioFileEx::PREDEFINEDSIZE=1024;
CStdioFileEx::CStdioFileEx()
	:CStdioFile()
{
	m_FileType=TextCodeTypeANSI;//指定默认类型
}
CStdioFileEx::CStdioFileEx(FILE* pOpenStream)
	:CStdioFile(pOpenStream)
{
	CString filepath = stringAnsiToCString(pOpenStream->_tmpfname);//?  尚不清楚File*结构
	m_FileType=GetFileType(filepath);
}
CStdioFileEx::CStdioFileEx(LPCTSTR lpszFileName, UINT nOpenFlags)
	:CStdioFile(lpszFileName,ProcessFlags(lpszFileName, nOpenFlags,m_FileType) )
{
}
CStdioFileEx::~CStdioFileEx()
{

}
// --------------------------------------------------------------------------------------------
//CStdioFileEx::GetFileType  静态方法  检测文本文件类型
// --------------------------------------------------------------------------------------------
/*static */ TextCodeType CStdioFileEx::GetFileType(LPCTSTR lpszFileName)
{
	CFile file;
	byte  buf[3];//unsigned char
	TextCodeType tc;
	try
	{
		if(file.Open(lpszFileName,CFile::modeRead|CFile::shareDenyNone|CFile::typeBinary))
		{   
			file.Read(buf,3);
			if(buf[0]==0xEF && buf[1]==0xBB && buf[2]==0xBF)
				tc=TextCodeTypeUTF8;
			else
				if(buf[0]==0xFF && buf[1]==0xFE )
					tc=TextCodeTypeUNICODE ;
				else
					if(buf[0]==0xFE && buf[1]==0xFF )
						tc=TextCodeTypeUNICODEBIGENDIAN;
					else
						tc=TextCodeTypeANSI;
		}
		else
			tc=TextCodeTypeFILEERROR;
	}
	catch (CFileException ex)
	{   
		CString errormsg;
		errormsg.Format(_T("操作文件%s时发生异常!"),ex.m_strFileName);
		AfxMessageBox(errormsg);
	}
	file.Close();
	return tc;
}
// --------------------------------------------------------------------------------------------
//CStdioFileEx::Readstring 按行读取文本文件
//根据不同文件类型 调用不同的读取函数
// --------------------------------------------------------------------------------------------
BOOL CStdioFileEx::ReadString(CString& rString)
{    
	BOOL flag=FALSE;
	switch(m_FileType)
	{
	case TextCodeTypeANSI:
		flag=ReadStringFromAnsiFile(rString);
		break;
	case TextCodeTypeUNICODE:
	case TextCodeTypeUNICODEBIGENDIAN:
		flag=ReadStringFromUnicodeFile(rString);
		break;
	case TextCodeTypeUTF8:
		flag=ReadStringFromUTF8File(rString);
		break;
	case TextCodeTypeFILEERROR:
		flag=FALSE;
		break;
	default:
		break;
	}
	return flag;
}
// --------------------------------------------------------------------------------------------
//CStdioFileEx::ReadstringFromAnsiFile  从ANSI文件读取字符串
// --------------------------------------------------------------------------------------------
BOOL CStdioFileEx::ReadStringFromAnsiFile(CString& rString)
{   
	BOOL flag;
	try
	{   
		flag=CStdioFile::ReadString(rString);
		rString+=L"\r\n";
	}
	catch(CFileException ex)
	{
		CString errormsg;
		errormsg.Format(_T("操作文件%s时发生异常!"),ex.m_strFileName);
		AfxMessageBox(errormsg);
	}
	return flag;
}
// --------------------------------------------------------------------------------------------
//CStdioFileEx::ReadstringFromUTF8File 从UTF8文件中按行读取 
//由于UTF-8编码多字节编码且各种字符长度不同,判断回车换行需要判断连续两个字节  
// --------------------------------------------------------------------------------------------
BOOL  CStdioFileEx::ReadStringFromUTF8File(CString& rString)
{
	long    index;
	byte    cr=0x0d;//回车换行符
	byte    lf=0x0a;
	byte    temp[2];
	byte    tempbyte;
	byte    *pbuf=new byte[PREDEFINEDSIZE+1];
	memset(pbuf,0,(PREDEFINEDSIZE+1)*sizeof(byte));
	UINT    readlen;
	try
	{   
		//跳过文件头 移动文件指针
		if (m_pStream && ( GetPosition() == 0))
		{
			CStdioFile::Seek(3*sizeof(byte),CFile::begin);
		}
		index=0;
		do
		{
			memset(temp,0,2*sizeof(byte));
			readlen=CFile::Read(temp,2);//CStdioFile::Read效果不同 将省去回车符0x0d
			if(!readlen)
				return FALSE;
			//元素存贮到字节数组中
			pbuf[index++]=temp[0];
			pbuf[index++]=temp[1];
			tempbyte=temp[1];
			//判断回车换行
			if( ( tempbyte==cr && temp[0]==lf) ||(temp[0]==cr && temp[1]==lf))
				break;
		} while (readlen==2 && index<PREDEFINEDSIZE );
		pbuf[index]=0;
		rString=UTF8ToUnicode(pbuf);//UTF8编码转换到UNICODE
	}
	catch (CFileException ex)
	{
		CString errormsg;
		errormsg.Format(_T("操作文件%s时发生异常!"),ex.m_strFileName);
		AfxMessageBox(errormsg);
	}
	delete[] pbuf;
	return TRUE;
}
// --------------------------------------------------------------------------------------------
//从UNICODE、UNICODE big endian文件按行读取
//当读取字节小于请求值(文件结束)或者超过预定义空间时无条件退出循环
//wChLine存放每行字符,wchtemp存放临时读取字符
//当编码为UNICODE big endian 时交换高低字节 ,将其转换成UNICODE字符串
// --------------------------------------------------------------------------------------------
BOOL  CStdioFileEx::ReadStringFromUnicodeFile(CString& rString)
{
	long    index;
	UINT    readlen;
	wchar_t wchcr=MAKEWORD(0x0d,0x00);;//回车符  MakeWord(低、高字节顺序) 
	wchar_t wchlf=MAKEWORD(0x0a,0x00);
	wchar_t *wChLine=new wchar_t[PREDEFINEDSIZE+1];
	memset(wChLine,0,(PREDEFINEDSIZE+1)*sizeof(wchar_t));
	wchar_t wchtemp[2];
	BOOL   flag=TRUE;
	try
	{   
		//跳过文件头 移动文件指针
		if (m_pStream && ( GetPosition() ==0))
		{
			Seek(2*sizeof(byte),CFile::begin);
		}
		index=0;
		do
		{   
			memset(wchtemp,0,2*sizeof(wchar_t));
			readlen=CFile::Read(wchtemp,sizeof(wchar_t)*2);//CStdioFile::Read效果不同
			if(!readlen)
				break;
			//UNICODE big endian交换高低字节
			if(TextCodeTypeUNICODEBIGENDIAN==m_FileType)
			{   
				unsigned char high, low;
				high = (wchtemp[0] & 0xFF00) >>8;
				low  = wchtemp[0] & 0x00FF;
				wchtemp[0] = ( low <<8) | high;
				high = (wchtemp[1] & 0xFF00) >>8;
				low  = wchtemp[1] & 0x00FF;
				wchtemp[1] = ( low <<8) | high;
			}
			wChLine[index++]=wchtemp[0];
			wChLine[index++]=wchtemp[1];
			//判断回车换行
			if(wchtemp[0]==wchcr && wchtemp[1]==wchlf)
				break;
		}
		while( (readlen==sizeof(wchar_t)*2) && index<PREDEFINEDSIZE );
		wChLine[index]=0;
		CString strtext(wChLine,index);
		rString=strtext;
		if(rString.IsEmpty())
			flag=FALSE;
	}
	catch (CFileException ex)
	{
		CString errormsg;
		errormsg.Format(_T("操作文件%s时发生异常!"),ex.m_strFileName);
		AfxMessageBox(errormsg);
	}
	delete[] wChLine;
	return flag;
}
// --------------------------------------------------------------------------------------------
//CStdioFileEx::UTF8ToUnicode  UTF-8字符串转换成UNICODE字符串
// --------------------------------------------------------------------------------------------
CString CStdioFileEx::UTF8ToUnicode(byte  *szUTF8)
{   
	CString strret;
	strret=_T("");
	if(!szUTF8)
		return strret;
	//获取转换后所需串空间的长度 
	int   wcsLen =  MultiByteToWideChar(CP_UTF8,0,(LPSTR)szUTF8,strlen((char*)szUTF8),NULL,0);
	LPWSTR   lpw=new   WCHAR[wcsLen+1]; 
	if(!lpw)
		return strret;
	memset(lpw,0,(wcsLen+1)*sizeof(wchar_t)); 
	//实施转换
	MultiByteToWideChar(CP_UTF8,0, (LPSTR)szUTF8, 
		strlen((char *)szUTF8),  (LPWSTR)lpw,  wcsLen); 
	CString str(lpw);
	delete[]   lpw;
	return str;
}
// --------------------------------------------------------------------------------------------
//CStdioFileEx::GetFileType获取文件类型
// --------------------------------------------------------------------------------------------
TextCodeType CStdioFileEx::GetFileType()
{   
	return m_FileType;
}
// --------------------------------------------------------------------------------------------
//CStdioFileEx::FileTypeToString 文件类型枚举值转换为字符串值
// --------------------------------------------------------------------------------------------
CString CStdioFileEx::FileTypeToString()
{   
	CString strtype;
	switch(m_FileType)
	{
	case TextCodeTypeANSI:
		strtype.Format(L"%s",_T("ANSI"));
		break;
	case TextCodeTypeUTF8:
		strtype.Format(L"%s",_T("UTF8"));
		break;
	case TextCodeTypeUNICODE:
		strtype.Format(L"%s",_T("UNICODE"));
		break;
	case TextCodeTypeUNICODEBIGENDIAN:
		strtype.Format(L"%s",_T("UNICODE big endian"));
		break;
	case TextCodeTypeFILEERROR:
		strtype.Format(L"%s",_T("FILEERROR"));
		break;
	default:
		break;
	}
	return strtype;
}
// --------------------------------------------------------------------------------------------
//CStdioFileEx::Open 重载父类的文件打开操作 改变不同类型文件的打开方式
// --------------------------------------------------------------------------------------------
BOOL CStdioFileEx::Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{   
	ProcessFlags(lpszFileName,nOpenFlags,m_FileType);//处理文件打开方式
	return CStdioFile::Open(lpszFileName, nOpenFlags,pError);
}
// --------------------------------------------------------------------------------------------
//CStdioFileEx::ProcessFlags 处理不同文件的打开方式
//ANSI文件采用文本读取，UNICODE、UNICDOE big endian、UTF-8采用二进制方式读取
// --------------------------------------------------------------------------------------------
UINT CStdioFileEx::ProcessFlags(LPCTSTR lpszFileName, UINT& nOpenFlags,TextCodeType &tc)
{
	tc=CStdioFileEx::GetFileType(lpszFileName);
	if ((nOpenFlags & CFile::modeReadWrite)|| (nOpenFlags & CFile::modeRead) )
	{
		switch(tc)
		{
		case TextCodeTypeANSI:
			nOpenFlags|= CFile::typeText;
			nOpenFlags&=~CFile::typeBinary;
			break;
		case TextCodeTypeUTF8:
			nOpenFlags |= CFile::typeBinary;
			nOpenFlags&= ~CFile::typeText;
			break;
		case TextCodeTypeUNICODE:
			nOpenFlags |= CFile::typeBinary;
			nOpenFlags&= ~CFile::typeText;
			break;
		case TextCodeTypeUNICODEBIGENDIAN:
			nOpenFlags |= CFile::typeBinary;
			nOpenFlags&= ~CFile::typeText;
			break;
		case TextCodeTypeFILEERROR:
			break;
		default:
			break;
		}
	}
	nOpenFlags|=CFile::shareDenyNone;
	return nOpenFlags;
}