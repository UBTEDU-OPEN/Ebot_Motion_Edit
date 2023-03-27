#ifndef CSTRINGHANDLE_H
#define CSTRINGHANDLE_H


#include <QString>

class CStringHandle
{
public:
    CStringHandle();

public:
    static QString getQString(const char* const pData, int nLen);

    /***
      *判断一个字符串是否为纯数字
      */
    static bool isDigitStr(QString src);    

    //
    static bool isContainsChineseChar(const QString &strText);
    static bool isContainsChar(const QString& strTest, const QString& strChars, bool bCaseSensitive = true);

};

#endif // CSTRINGHANDLE_H
