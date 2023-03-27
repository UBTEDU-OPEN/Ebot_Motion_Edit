#ifndef CACTMIXTUREPARSE_H
#define CACTMIXTUREPARSE_H


#include <QString>

#include <QDomElement>


#include "htsparser.h"

class CActMixtureParse
{
public:
    CActMixtureParse();

public:
    /**************************************************************************
    * 函数名: htsToMixXml
    * 功能: hts to xml
    * 参数:
    *    @[in ] pBuf: hts文件数据
    * 返回值: bool
    * 时间: 20161128
    * 作者: suhaiwu
    */
    static bool htsToMixXml(const unsigned char* const pBufHts, int nLenTotalHts, const QString& strFileXml);

    //return one xml element of one frame
    static QDomElement getXmlElementActFrame(const HTS_PER_PAGE_INFO& htsPerPageInfo);
    //return one xml element of allruntime of all action
    static QDomElement getXmlElementAllRunTime(int nAllRunTimeMillisecond);




};

#endif // CACTMIXTUREPARSE_H
