/*!
* \file	StringFunc.h
* \brief	Functions related to string handling.
* \author	Jo Hyeong-ryeol
* \since	2003.11.12
* \version	$LastChangedRevision: 99 $
*			$LastChangedDate: 2006-02-03 23:16:57 +0900 (금, 03 2 2006) $
*
* This file contains functions related to string handling.
* \n\n\n
* Copyright &copy; 2006 by <a href="mailto:hyeongryeol@gmail.com">Jo Hyeong-ryeol</a>\n
* Permission to copy, use, modify, sell and distribute this software is
* granted provided this copyright notice appears in all copies.
* This software is provided "as is" without express or implied warranty,
* and with no claim as to its suitability for any purpose.
*/
#pragma once

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>			// for String... functions
#include <crtdbg.h>				// for _ASSERTE 

#include <string>

/*!
 * \brief	The namespace of the Ryeol's library
 *
 * This is the namespace for source codes written by Jo Hyeong-ryeol.
 */

/*! \brief	Returns the number of bytes which is required to convert an UNICODE string into an ANSI string */
HRESULT NeededCharsForUnicode2Ansi(LPCWSTR wszSrc, int* pcbNeeded);
/*! \brief	Converts an UNICODE string into an ANSI string */
HRESULT Unicode2Ansi(LPSTR szDest, int cbDest, LPCWSTR wszSrc);
/*! \brief	Converts an UNICODE string into an ANSI string */
HRESULT Unicode2Ansi(LPSTR* pszDest, LPCWSTR wszSrc, HANDLE hHeap);

/*! \brief	Returns the number of bytes which is required to convert an UNICODE string into an UTF-8 string */
HRESULT NeededCharsForUnicode2UTF8(LPCWSTR wszSrc, int* pcbNeeded);
/*! \brief	Converts an UNICODE string into an UTF-8 string */
HRESULT Unicode2UTF8(LPSTR szDest, int cbDest, LPCWSTR wszSrc);
/*! \brief	Converts an UNICODE string into an UTF-8 string */
HRESULT Unicode2UTF8(LPSTR* pszDest, LPCWSTR wszSrc, HANDLE hHeap);

/*! \brief	Returns the number of characters which is required to convert an ANSI string into an UNICODE string */
HRESULT NeededCharsForAnsi2Unicode(LPCSTR szSrc, int* pcchNeeded);
/*! \brief	Converts an ANSI string into an UNICODE string */
HRESULT Ansi2Unicode(LPWSTR wszDest, int cchDest, LPCSTR szSrc);
/*! \brief	Converts an ANSI string into an UNICODE string */
HRESULT Ansi2Unicode(LPWSTR* pwszDest, LPCSTR szSrc, HANDLE hHeap);

HRESULT stringAnsiToUnicode(const std::string &str, std::wstring &wstr);
HRESULT stringUtf8ToUnicode(const std::string &str, std::wstring &wstr);
HRESULT stringUnicodeToAnsi(const std::wstring &wstr, std::string &str);
HRESULT stringUnicodeToUtf8(const std::wstring &wstr, std::string &str);
HRESULT stringAnsiToUtf8(const std::string &astr, std::string &ustr);
HRESULT stringUtf8ToAnsi(const std::string &ustr, std::string &astr);

HRESULT stringAnsiToCString(const std::string &str, CString &cstr);
HRESULT stringUtf8ToCString(const std::string &str, CString &cstr);
HRESULT CStringTostringAnsi(const CString &cstr, std::string &str);
HRESULT CStringTostringUtf8(const CString &cstr, std::string &str);

CString stringAnsiToCString(const std::string &str);
CString stringUtf8ToCString(const std::string &str);
std::string CStringTostringAnsi(const CString &str);
std::string CStringTostringUtf8(const CString &str);

// string object help api.
std::string& trim(std::string &str, const std::string &drop = " ");
std::string& ltrim(std::string &str, const std::string &drop = " ");
std::string& rtrim(std::string &str, const std::string &drop = " ");
std::string& trim_sp(std::string &str);
std::string& ltrim_sp(std::string &str);
std::string& rtrim_sp(std::string &str);
std::string& strtoupper(std::string &str);
std::string& strtolower(std::string &str);
size_t tstrsplit(std::basic_string< TCHAR > const& str, std::vector< std::basic_string< TCHAR > > &vec, const TCHAR delimiter = TEXT(' '));
size_t strsplit(const std::string &str, std::vector< std::string > &vec, const char spliter = ' ');
size_t wstrsplit(const std::wstring &wstr, std::vector< std::wstring > &vec, const wchar_t spliter = L' ');
std::string str_replace(const std::string& input, const std::string& find, const std::string& replace_with);
std::string& strformat(std::string &str, const char *szformat, ...);

// parse ipv4 address value
bool parse_ipaddr_V4(unsigned int * ip_val, const std::string &strip_v4);

// current time
std::string cur_time(void);
