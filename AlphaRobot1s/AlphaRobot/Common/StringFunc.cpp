/*!
* \file	StringFunc.cpp
* \brief	Implementations of functions related to string handling.
* \author	Jo Hyeong-ryeol
* \since	2003.07.07
* \version	$LastChangedRevision: 99 $
*			$LastChangedDate: 2006-02-03 23:16:57 +0900 (금, 03 2 2006) $
*
* This file contains implementations of functions related to string handling.
* \n\n\n
* Copyright &copy; 2006 by <a href="mailto:hyeongryeol@gmail.com">Jo Hyeong-ryeol</a>\n
* Permission to copy, use, modify, sell and distribute this software is
* granted provided this copyright notice appears in all copies.
* This software is provided "as is" without express or implied warranty,
* and with no claim as to its suitability for any purpose.
*/
#include "stdafx.h"
#include "StringFunc.h"

#include <time.h>

/*!
 * \brief	The namespace of the Ryeol's library
 *
 * This is the namespace for source codes written by Jo Hyeong-ryeol.
 */

/*!
 * This function returns the number of bytes which is required
 * to convert an UNICODE string into an ANSI string.
 * The returned value does not include the terminating NULL character.
 * If an error occurrs, you can use the ::GetLastError function for error information.
 *
 * \param[in] wszSrc		An UNICODE string.
 * \param[out] pcbNeeded	The number of bytes required.
 *							(Not including the terminating NULL character)
 * \return					A HRESULT value
 */
HRESULT NeededCharsForUnicode2Ansi(LPCWSTR wszSrc, int* pcbNeeded)
{
	// Checks the input parameters
	_ASSERTE((wszSrc != NULL) && (pcbNeeded != NULL));

	size_t cchSrc;
	if (FAILED(::StringCchLengthW(wszSrc, STRSAFE_MAX_CCH, &cchSrc)))
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return E_INVALIDARG;
	}

	// If the length of the string is zero
	if (cchSrc == 0)
	{
		*pcbNeeded = 0;
		return S_OK;
	}

	// Calculates the required memory
	if (0 == (*pcbNeeded = ::WideCharToMultiByte(CP_ACP, 0, wszSrc, static_cast<int> (cchSrc + 1), NULL, 0, NULL, NULL)))
		return E_FAIL;

	*pcbNeeded -= 1;
	return S_OK;
}

/*!
 * This function converts an UNICODE string into an ANSI string.
 * If an error occurrs, you can use the ::GetLastError function for error information.
 * The buffer pointed to by the szDest must be enough to copy the converted string.
 *
 * \param[in] szDest		A buffer to save the converted string.
 * \param[in] cbDest		The number of bytes the buffer can hold.
 * \param[in] wszSrc		An UNICODE string to convert.
 * \return					A HRESULT value
 */
HRESULT Unicode2Ansi(LPSTR szDest, int cbDest, LPCWSTR wszSrc)
{
	_ASSERTE(!(szDest == NULL || cbDest == 0 || wszSrc == NULL));

	// Converts into ANSI format.
	if (0 == ::WideCharToMultiByte(CP_ACP, 0, wszSrc, -1, szDest, cbDest, NULL, NULL))
		return E_FAIL;
	return S_OK;
}

/*!
 * This function converts an UNICODE string into an ANSI string.
 * If an error occurrs, you can use the ::GetLastError function for error information.
 * The string returned by the pszDest must be released by using the ::HeapFree function.
 *
 * \param[out] pszDest		A pointer to receive the converted string.
 * \param[in] wszSrc		An UNICODE string to convert.
 * \param[in] hHeap			A heap handle which is used to allocate memory.
 *							If this is NULL, the process heap is used.
 * \return					A HRESULT value
 */
HRESULT Unicode2Ansi(LPSTR* pszDest, LPCWSTR wszSrc, HANDLE hHeap)
{
	// Checks the input parameters
	_ASSERTE(!(pszDest == NULL || wszSrc == NULL));

	*pszDest = NULL;

	if (hHeap == NULL && (hHeap = ::GetProcessHeap()) == NULL)
		return E_FAIL;

	HRESULT hResult;

	int cchNeeded;
	if (FAILED(hResult = NeededCharsForUnicode2Ansi(wszSrc, &cchNeeded)))
		return hResult;

	// Allocates memory
	*pszDest = static_cast<LPSTR> (::HeapAlloc(hHeap, 0, cchNeeded + 1));
	if (*pszDest == NULL)
	{
		::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return E_OUTOFMEMORY;
	}

	// Converts into ANSI format
	if (FAILED(hResult = Unicode2Ansi(*pszDest, cchNeeded + 1, wszSrc)))
	{
		DWORD dwLastErr = ::GetLastError();
		::HeapFree(hHeap, 0, *pszDest);
		::SetLastError(dwLastErr);
		return hResult;
	}

	return S_OK;
}


/*!
 * This function returns the number of bytes which is required
 * to convert an UNICODE string into an UTF-8 string.
 * The returned value does not include the terminating NULL character.
 * If an error occurrs, you can use the ::GetLastError function for error information.
 *
 * \param[in] wszSrc		An UNICODE string.
 * \param[out] pcbNeeded	The number of bytes required.
 *							(Not including the terminating NULL character)
 * \return					A HRESULT value
 */
HRESULT NeededCharsForUnicode2UTF8(LPCWSTR wszSrc, int* pcbNeeded)
{
	// Checks the input parameters
	_ASSERTE((wszSrc != NULL) && (pcbNeeded != NULL));

	size_t cchSrc;
	if (FAILED(::StringCchLengthW(wszSrc, STRSAFE_MAX_CCH, &cchSrc)))
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return E_INVALIDARG;
	}

	// If the length of the string is zero
	if (cchSrc == 0)
	{
		*pcbNeeded = 0;
		return S_OK;
	}

	// Calculates the required memory
	if (0 == (*pcbNeeded = ::WideCharToMultiByte(CP_UTF8, 0, wszSrc, static_cast<int> (cchSrc + 1), NULL, 0, NULL, NULL)))
		return E_FAIL;

	*pcbNeeded -= 1;
	return S_OK;
}

/*!
 * This function converts an UNICODE string into an UTF-8 string.
 * If an error occurrs, you can use the ::GetLastError function for error information.
 * The buffer pointed to by the szDest must be enough to copy the converted string.
 *
 * \param[in] szDest		A buffer to save the converted string.
 * \param[in] cbDest		The number of bytes the buffer can hold.
 * \param[in] wszSrc		An UNICODE string to convert.
 * \return					A HRESULT value
 */
HRESULT Unicode2UTF8(LPSTR szDest, int cbDest, LPCWSTR wszSrc)
{
	_ASSERTE(!(szDest == NULL || cbDest == 0 || wszSrc == NULL));

	// Converts into UTF-8 format.
	if (0 == ::WideCharToMultiByte(CP_UTF8, 0, wszSrc, -1, szDest, cbDest, NULL, NULL))
		return E_FAIL;
	return S_OK;
}

/*!
 * This function converts an UNICODE string into an UTF-8 string.
 * If an error occurrs, you can use the ::GetLastError function for error information.
 * The string returned by the pszDest must be released by using the ::HeapFree function.
 *
 * \param[out] pszDest		A pointer to receive the converted string.
 * \param[in] wszSrc		An UNICODE string to convert.
 * \param[in] hHeap			A heap handle which is used to allocate memory.
 *							If this is NULL, the process heap is used.
 * \return					A HRESULT value
 */
HRESULT Unicode2UTF8(LPSTR* pszDest, LPCWSTR wszSrc, HANDLE hHeap)
{
	// Checks the input parameters
	_ASSERTE(!(pszDest == NULL || wszSrc == NULL));

	*pszDest = NULL;

	if (hHeap == NULL && (hHeap = ::GetProcessHeap()) == NULL)
		return E_FAIL;

	HRESULT hResult;

	int cchNeeded;
	if (FAILED(hResult = NeededCharsForUnicode2UTF8(wszSrc, &cchNeeded)))
		return hResult;

	// Allocates memory
	*pszDest = static_cast<LPSTR> (::HeapAlloc(hHeap, 0, cchNeeded + 1));
	if (*pszDest == NULL)
	{
		::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return E_OUTOFMEMORY;
	}

	// Converts into UTF-8 format
	if (FAILED(hResult = Unicode2UTF8(*pszDest, cchNeeded + 1, wszSrc)))
	{
		DWORD dwLastErr = ::GetLastError();
		::HeapFree(hHeap, 0, *pszDest);
		::SetLastError(dwLastErr);
		return hResult;
	}

	return S_OK;
}


/*!
 * This function returns the number of characters which is required
 * to convert an ANSI string into an UNICODE string.
 * The returned value does not include the terminating NULL character.
 * If an error occurrs, you can use the ::GetLastError function for error information.
 *
 * \param[in] szSrc			An ANSI string.
 * \param[out] pcchNeeded	The number of characters required.
 *							(Not including the terminating NULL character)
 * \return					A HRESULT value
 */
HRESULT NeededCharsForAnsi2Unicode(LPCSTR szSrc, int* pcchNeeded)
{
	// Checks the input parameters
	_ASSERTE((szSrc != NULL) && (pcchNeeded != NULL));

	size_t cchSrc;
	if (FAILED(::StringCchLengthA(szSrc, STRSAFE_MAX_CCH, &cchSrc)))
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return E_INVALIDARG;
	}

	// If the length of the string is zero
	if (cchSrc == 0)
	{
		*pcchNeeded = 0;
		return S_OK;
	}

	// Calculates the required memory
	if (0 == (*pcchNeeded = ::MultiByteToWideChar(CP_ACP, 0, szSrc, static_cast<int> (cchSrc + 1), NULL, 0)))
		return E_FAIL;

	*pcchNeeded -= 1;
	return S_OK;
}

/*!
 * This function converts an ANSI string into an UNICODE string.
 * If an error occurrs, you can use the ::GetLastError function for error information.
 * The buffer pointed to by the wszDest must be enough to copy the converted string.
 *
 * \param[in] wszDest		A buffer to save the converted string.
 * \param[in] cchDest		The number of characters the buffer can hold.
 * \param[in] szSrc			An ANSI string to convert.
 * \return					A HRESULT value
 */
HRESULT Ansi2Unicode(LPWSTR wszDest, int cchDest, LPCSTR szSrc)
{
	_ASSERTE(!(wszDest == NULL || cchDest == 0 || szSrc == NULL));

	// Converts into UNICODE format.
	if (0 == ::MultiByteToWideChar(CP_ACP, 0, szSrc, -1, wszDest, cchDest))
		return E_FAIL;

	return S_OK;
}

/*!
 * This function converts an ANSI string into an UNICODE string.
 * If an error occurrs, you can use the ::GetLastError function for error information.
 * The string returned by the pwszDest must be released by using the ::HeapFree function.
 *
 * \param[out] pwszDest		A pointer to receive the converted string.
 * \param[in] szSrc			An ANSI string to convert.
 * \param[in] hHeap			A heap handle which is used to allocate memory.
 *							If this is NULL, the process heap is used.
 * \return					A HRESULT value
 */
HRESULT Ansi2Unicode(LPWSTR* pwszDest, LPCSTR szSrc, HANDLE hHeap)
{
	// Checks the input parameters
	_ASSERTE(!(pwszDest == NULL || szSrc == NULL));

	*pwszDest = NULL;

	if (hHeap == NULL && (hHeap = ::GetProcessHeap()) == NULL)
		return E_FAIL;

	HRESULT hResult;
	int cchNeeded;

	if (FAILED(hResult = NeededCharsForAnsi2Unicode(szSrc, &cchNeeded)))
		return hResult;

	// Allocates memory
	*pwszDest = static_cast<LPWSTR> (::HeapAlloc(hHeap, 0, (cchNeeded + 1) * sizeof(WCHAR)));
	if (*pwszDest == NULL)
	{
		::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return E_OUTOFMEMORY;
	}

	// Converts into UNICODE format
	if (FAILED(hResult = Ansi2Unicode(*pwszDest, cchNeeded + 1, szSrc)))
	{
		DWORD dwLastErr = ::GetLastError();
		::HeapFree(hHeap, 0, *pwszDest);
		::SetLastError(dwLastErr);
		return E_FAIL;
	}

	return S_OK;
}

//***********************************************************************

HRESULT stringAnsiToUnicode(const std::string &str, std::wstring &wstr)
{
	wchar_t *szTemp = NULL;
	HRESULT hr;
	size_t st_len = (str.size() + 1) * sizeof(char);

	if (str.empty())
	{
		wstr = L"";
		return S_OK;
	}

	szTemp = new wchar_t[st_len];
	memset(szTemp, 0, st_len);
	hr = Ansi2Unicode(szTemp, st_len, str.c_str());
	if (FAILED(hr))
	{
		wstr = L"";
		delete szTemp;
		return E_FAIL;
	}

	wstr = szTemp;
	delete []szTemp;

	return S_OK;
}

HRESULT stringUtf8ToUnicode(const std::string &str, std::wstring &wstr)
{
	wchar_t *szTemp = NULL;
	HRESULT hr;
	size_t st_len = (str.size() + 1) * sizeof(char);

	if (str.empty())
	{
		wstr = L"";
		return S_OK;
	}

	szTemp = new wchar_t[st_len];
	memset(szTemp, 0, st_len);
	// Converts into UNICODE format.
	hr = (0 == ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szTemp, st_len)) ? E_FAIL : S_OK;
	if (FAILED(hr))
	{
		wstr = L"";
		delete []szTemp;
		return E_FAIL;
	}
	wstr = szTemp;
	delete []szTemp;

	return S_OK;
}

HRESULT stringUnicodeToAnsi(const std::wstring &wstr, std::string &str)
{
	char *szTemp = NULL;
	HRESULT hr;
	size_t st_len = (wstr.size() + 1) * 4;

	if (wstr.empty())
	{
		str = "";
		return S_OK;
	}

	szTemp = new char[st_len];
	memset(szTemp, 0, st_len);
	hr = ::Unicode2Ansi(szTemp, st_len, wstr.c_str());
	if (FAILED(hr))
	{
		str = "";
		delete []szTemp;
		return E_FAIL;
	}

	str = szTemp;
	delete []szTemp;

	return S_OK;
}

HRESULT stringUnicodeToUtf8(const std::wstring &wstr, std::string &str)
{
	char *szTemp = NULL;
	HRESULT hr;
	size_t st_len = (wstr.size() + 1) * 4;

	if (wstr.empty())
	{
		str = "";
		return S_OK;
	}

	szTemp = new char[st_len];
	memset(szTemp, 0, st_len);
	hr = ::Unicode2UTF8(szTemp, st_len, wstr.c_str());
	if (FAILED(hr))
	{
		str = "";
		delete []szTemp;
		return E_FAIL;
	}

	str = szTemp;
	delete []szTemp;

	return S_OK;
}

HRESULT stringAnsiToUtf8(const std::string &astr, std::string &ustr)
{
	HRESULT hr;

	std::wstring wstr;
	hr = stringAnsiToUnicode(astr, wstr);
	if (FAILED(hr))
	{
		ustr = "";
		return hr;
	}

	hr = stringUnicodeToUtf8(wstr, ustr);
	return hr;
}

HRESULT stringUtf8ToAnsi(const std::string &ustr, std::string &astr)
{
	HRESULT hr;

	std::wstring wstr;
	hr = stringUtf8ToUnicode(ustr, wstr);
	if (FAILED(hr))
	{
		astr = "";
		return hr;
	}

	hr = stringUnicodeToAnsi(wstr, astr);
	return hr;
}

//***********************************************************************

HRESULT stringAnsiToCString(const std::string &str, CString &cstr)
{
	HRESULT hr = S_OK;
	if (str.empty())
	{
		cstr = TEXT("");
		return hr;
	}

#ifdef _UNICODE
	wchar_t *szTemp = NULL;

	size_t st_len = (str.size() + 1) * sizeof(char);

	szTemp = new wchar_t[st_len];
	memset(szTemp, 0, st_len);
	hr = ::Ansi2Unicode(szTemp, st_len, str.c_str());
	ASSERT(SUCCEEDED(hr));
	cstr = szTemp;
	delete []szTemp;
#else
	cstr = str.c_str();
#endif

	return hr;
}

HRESULT stringUtf8ToCString(const std::string &str, CString &cstr)
{
	HRESULT hr = S_OK;
	if (str.empty())
	{
		cstr = TEXT("");
		return hr;
	}

#ifdef _UNICODE
	wchar_t *szTemp = NULL;
	size_t st_len = (str.size() + 1) * sizeof(char);

	szTemp = new wchar_t[st_len];
	memset(szTemp, 0, st_len);
	// Converts into UNICODE format.
	hr = (0 == ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szTemp, st_len)) ? E_FAIL : S_OK;
	ASSERT(SUCCEEDED(hr));
	cstr = szTemp;
	delete []szTemp;
#else
	cstr = str.c_str();
#endif

	return hr;
}

HRESULT CStringTostringAnsi(const CString &cstr, std::string &str)
{
	HRESULT hr = S_OK;
	if (cstr.IsEmpty())
	{
		str = "";
		return hr;
	}

#ifdef _UNICODE
	size_t st_len = (cstr.GetLength() + 1) * 4;
	str.resize(st_len, '\0');
	char *szTemp = (char *)str.c_str();
	hr = ::Unicode2Ansi(szTemp, st_len, (LPCTSTR)cstr);
	ASSERT(SUCCEEDED(hr));
#else
	str = (LPCSTR)cstr;
#endif

	return hr;
}

HRESULT CStringTostringUtf8(const CString &cstr, std::string &str)
{
	HRESULT hr = S_OK;
	if (cstr.IsEmpty())
	{
		str = "";
		return hr;
	}

#ifdef _UNICODE
	size_t st_len = (cstr.GetLength() + 1) * 4;
	str.resize(st_len, '\0');
	char *szTemp = (char *)str.c_str();
	hr = ::Unicode2UTF8(szTemp, st_len, (LPCTSTR)cstr);
	ASSERT(SUCCEEDED(hr));
#else
	str = (LPCSTR)cstr;
#endif

	return hr;
}

//***********************************************************************

CString stringAnsiToCString(const std::string &str)
{
	CString strUnicode;
	if (str.empty())
		return TEXT("");

#ifdef _UNICODE
	wchar_t *szTemp = NULL;
	HRESULT hr;
	size_t st_len = (str.size() + 1) * sizeof(char);

	szTemp = new wchar_t[st_len];
	memset(szTemp, 0, st_len);
	hr = ::Ansi2Unicode(szTemp, st_len, str.c_str());
	ASSERT(SUCCEEDED(hr));
	strUnicode = szTemp;
	delete []szTemp;
#else
	strUnicode = str.c_str();
#endif

	return strUnicode;
}

CString stringUtf8ToCString(const std::string &str)
{
	CString strUnicode;
	if (str.empty())
		return TEXT("");

#ifdef _UNICODE
	wchar_t *szTemp = NULL;
	HRESULT hr;
	size_t st_len = (str.size() + 1) * sizeof(char);

	szTemp = new wchar_t[st_len];
	memset(szTemp, 0, st_len);
	// Converts into UNICODE format.
	hr = (0 == ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szTemp, st_len)) ? E_FAIL : S_OK;
	ASSERT(SUCCEEDED(hr));
	strUnicode = szTemp;
	delete []szTemp;
#else
	strUnicode = str.c_str();
#endif

	return strUnicode;
}

std::string CStringTostringAnsi(const CString &str)
{
	std::string strAnsi;

	if (str.IsEmpty())
		return strAnsi;

#ifdef _UNICODE
	char *szTemp = NULL;
	HRESULT hr;
	size_t st_len = (str.GetLength() + 1) * 4;

	szTemp = new char[st_len];
	memset(szTemp, 0, st_len);
	hr = ::Unicode2Ansi(szTemp, st_len, (LPCTSTR)str);
	ASSERT(SUCCEEDED(hr));
	strAnsi = szTemp;
	delete []szTemp;
#else
	strAnsi = (LPCSTR)str;
#endif

	return strAnsi;
}

std::string CStringTostringUtf8(const CString &str)
{
	std::string strAnsi;

	if (str.IsEmpty())
		return strAnsi;

#ifdef _UNICODE
	char *szTemp = NULL;
	HRESULT hr;
	size_t st_len = (str.GetLength() + 1) * 4;

	szTemp = new char[st_len];
	memset(szTemp, 0, st_len);
	hr = ::Unicode2UTF8(szTemp, st_len, (LPCTSTR)str);
	ASSERT(SUCCEEDED(hr));
	strAnsi = szTemp;
	delete []szTemp;
#else
	strAnsi = (LPCSTR)str;
#endif

	return strAnsi;
}

//////////////////////////////////////////////////////////////////////////
// string object help api.

std::string& trim(std::string &str, const std::string &drop /*= " "*/)
{
	size_t pos = str.find_last_not_of(drop);
	if (pos != std::string::npos)
		str.erase(pos + 1);

	return ltrim(str, drop);
}
std::string& ltrim(std::string &str, const std::string &drop /*= " "*/)
{
	size_t pos = str.find_first_not_of(drop);
	if (pos == std::string::npos)
		return str.erase(0, str.size());

	return str.erase(0, pos);
}
std::string& rtrim(std::string &str, const std::string &drop /*= " "*/)
{
	size_t pos = str.find_last_not_of(drop);
	if (pos == std::string::npos)
		return str.erase(0, str.size());

	return str.erase(pos + 1);
}

std::string& trim_sp(std::string &str)
{
	rtrim_sp(str);
	return ltrim_sp(str);
}
std::string& ltrim_sp(std::string &str)
{
	size_t pos = 0;
	while (pos < str.size() && (isspace(str.at(pos)) || ('\0' == str.at(pos))))
		++pos;

	return str.erase(0, pos);
}
std::string& rtrim_sp(std::string &str)
{
	size_t pos = str.size();
	while ((pos-- > 0) && (isspace(str.at(pos)) || ('\0' == str.at(pos))));

	return str.erase(pos + 1);
}

std::string& strtoupper(std::string &str)
{
	std::string::iterator it = str.begin();
	while (it != str.end())
	{
		if (*it >= 'a' && *it <= 'z')
			*it = *it - ('a' - 'A');
		++it;
	}
	return str;
}

std::string& strtolower(std::string &str)
{
	std::string::iterator it = str.begin();
	while (it != str.end())
	{
		if (*it >= 'A' && *it <= 'Z')
			*it = *it + ('a' - 'A');
		++it;
	}
	return str;
}

template <typename E, typename C>
size_t split(std::basic_string<E> const& str,
			 C &container,
			 E const delimiter,
			 bool keepBlankFields = true)
{
	size_t n = 0;
	std::basic_string<E>::const_iterator it = str.begin(), end = str.end(), first;
	for (first = it; it != end; ++it)
	{
		// Examine each character and if it matches the delimiter
		if (delimiter == *it)
		{
			if (keepBlankFields || first != it)
			{
				// extract the current field from the string and
				// append the current field to the given container
				container.push_back(std::basic_string<E>(first, it));
				++n;

				// skip the delimiter
				first = it + 1;
			}
			else
			{
				++first;
			}
		}
	}
	if (keepBlankFields || first != it)
	{
		// extract the last field from the string and
		// append the last field to the given container
		container.push_back(std::basic_string<E>(first, it));
		++n;
	}
	return n;
}

size_t tstrsplit(const std::basic_string< TCHAR > &str, std::vector< std::basic_string< TCHAR > > &vec, const TCHAR spliter /*= TEXT(' ')*/)
{
	vec.clear();
	return split(str, vec, spliter, false);
}

size_t strsplit(const std::string &str, std::vector< std::string > &vec, const char spliter /*= ' '*/)
{
	vec.clear();
	return split(str, vec, spliter, false);
}

size_t wstrsplit(const std::wstring &wstr, std::vector< std::wstring > &vec, const wchar_t spliter /*= L' '*/)
{
	vec.clear();
	return split(wstr, vec, spliter, false);
}

std::string str_replace(const std::string& input, const std::string& find, const std::string& replace_with)
{
	std::string strout(input);
	size_t curPos = 0;

	size_t pos = 0;
	while((pos = strout.find(find, curPos)) != std::string::npos)
	{
		strout.replace(pos, find.size(), replace_with);
		curPos = pos + replace_with.size();
	}

	return strout;
}

std::string& strformat(std::string &str, const char *szformat, ...)
{
	va_list params;
	va_start(params, szformat);

	str.resize(_vscprintf(szformat, params) + 1, '\0');
	int cchdest = vsprintf_s(const_cast< char * >(str.c_str()), str.size(), szformat, params);
	str.erase(cchdest);

	va_end(params);

	return str;
}

// parse ipv4 address value
bool parse_ipaddr_V4(unsigned int * ip_val, const std::string &strip_v4)
{
	unsigned char _ipbyte[4];
	memset(_ipbyte, 0, sizeof(_ipbyte));

	bool bparse = false;

	int itv = 0;
	int sum = 0;
	for (size_t i = 0; i <= strip_v4.size(); ++i)
	{
		if ((i < strip_v4.size()) && (strip_v4[i] >= '0') && (strip_v4[i] <= '9'))
		{
			sum = 10 * sum + (strip_v4[i] - '0');
			bparse = true;
		}
		else if (bparse && ((i == strip_v4.size()) || (strip_v4[i] == '.')) && (itv < sizeof(_ipbyte)) && (sum <= 0xFF))
		{
			_ipbyte[itv++] = sum;
			sum = 0;
			bparse = false;
		}
		else
			break;
	}
#ifndef MAKEIPADDRESS
#define MAKEIPADDRESS(b1,b2,b3,b4)  ((LPARAM)(((DWORD)(b1)<<24)+((DWORD)(b2)<<16)+((DWORD)(b3)<<8)+((DWORD)(b4))))
#endif // MAKEIPADDRESS
	bparse = (itv == sizeof(_ipbyte));
	if (NULL != ip_val)
	{
		*ip_val = bparse ? MAKEIPADDRESS(_ipbyte[0], _ipbyte[1], _ipbyte[2], _ipbyte[3]) : 0xFFFFFFFF;
	}

	return bparse;
}

// current time
std::string cur_time(void)
{
	time_t tmutc;
	char szTime[20];
	std::string strtime;

	time(&tmutc);

	struct tm tmnow;
	localtime_s(&tmnow, &tmutc);

	strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", &tmnow);

	strtime = szTime;

	return strtime;
}


