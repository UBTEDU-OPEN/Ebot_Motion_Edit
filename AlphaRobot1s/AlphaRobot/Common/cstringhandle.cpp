#include "cstringhandle.h"


#include <QRegExp>

CStringHandle::CStringHandle()
{

}

QString CStringHandle::getQString(const char * const pData, int nLen)
{
    if(!pData && (nLen <= 0))
    {
        return "";
    }

    QLatin1String latin1Str((char*)pData, nLen);
    QString str(latin1Str);
    return str;
}

bool CStringHandle::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
    const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;
    if (*s)
    { //不是纯数字
        return false;
    }

    //纯数字
    return true;
}

bool CStringHandle::isContainsChineseChar(const QString &strText)
{    
    return strText.contains(QRegExp("[\u4e00-\u9fa5]+"));
}

bool CStringHandle::isContainsChar(const QString& strTest, const QString& strChars, bool bCaseSensitive)
{
    bool bContainChar = false;
    const int nLenChar = strChars.length();
    for(int i = 0; i < nLenChar; i++)
    {
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;
        if(bCaseSensitive)
        {
            caseSensitivity = Qt::CaseSensitive;
        }
        bool bContain = strTest.contains(strChars.at(i), caseSensitivity);
        if(bContain)
        {
            bContainChar = true;
            return true;
        }
    }

    return bContainChar;
}

