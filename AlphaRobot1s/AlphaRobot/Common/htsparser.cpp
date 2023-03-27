/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CHtsParser.h
* 创建时间：2016/12/13
* 文件标识：
* 文件摘要：action file(hts) parser
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：2016/12/13
* 版本摘要：
*/
#include "htsparser.h"


#include <string.h>

#include "../common/Util.h"



CHtsParser::CHtsParser()
{

}

CHtsParser::~CHtsParser()
{

}

/**************************************************************************
* 函数名: findHtsPerPageHeadPoint
* 功能: find Hts Per Page Head Point by hts data
* 参数:
*    @[in ] pBuf: hts文件数据
* 返回值: if find retrun true
* 时间: 20170120
* 作者: suhaiwu
*/
bool CHtsParser::findHtsPerPageHeadPoint(const unsigned char * const pBuf, int64 nHtsDataLen,
                                         const unsigned char *&pBufPerPageHead, int nPerPageSize)
{
    pBufPerPageHead = NULL;
    if(!pBuf || (nHtsDataLen < HTS_PER_PAGE_SIZE))
    {
        return false;
    }

    bool bFind = false;
    for(int64 i = 0; (i < nHtsDataLen) && !bFind; i++)
    {
        bFind = CHtsParser::isHtsPerPageFormat(pBuf + i, nPerPageSize, nPerPageSize);
        if(bFind)
        {
            pBufPerPageHead = pBuf + i;
        }
    }

    return bFind;
}

/**************************************************************************
* 函数名: parserHtsPerPageInfo
* 功能: parser and get Hts PerPage Info
* 参数:
*    @[in ] pBufPerPageHead: point to the page head of hts data
* 返回值: if find retrun true
* 时间: 20170120
* 作者: suhaiwu
*/
bool CHtsParser::parserHtsPerPageInfo(const unsigned char * const pBufPerPageHead, int nLenData, HTS_PER_PAGE_INFO &htsPerPageInfo)
{
    if(!pBufPerPageHead || nLenData != HTS_PER_PAGE_SIZE)
    {
        return false;
    }

    bool bPageFormat = CHtsParser::isHtsPerPageFormat(pBufPerPageHead, nLenData, HTS_PER_PAGE_SIZE);
    if(!bPageFormat)
    {
        return false;
    }

    int nHtsFrameStat = eFrameInvalid;
    CDataOperate::getValue(pBufPerPageHead + ePosFrameStat, 1, nHtsFrameStat, 1);
    htsPerPageInfo.eHtsFrameStat = (emHtsFrameStat)nHtsFrameStat;
    CDataOperate::getValue(pBufPerPageHead + ePosFrameTotalCount, 2, htsPerPageInfo.nAllFrame, 2);
    CDataOperate::getValue(pBufPerPageHead + ePosFrameCur, 2, htsPerPageInfo.nCurrentFrame, 2);

    for(int i = 0; i < ROBOT_TOTAL_MOTOR_NUMBER; ++i)
    {
        int nAngle = 0;
        CDataOperate::getValue(pBufPerPageHead + ePosSteerAngle + i, 1, nAngle, 1);
        htsPerPageInfo.motorData[i].nID = ROBOT_ID_DEFAULT_START_NUMBER + i;
        htsPerPageInfo.motorData[i].nAngle = nAngle;
    }

    int nRunTime = 0;
    CDataOperate::getValue(pBufPerPageHead + ePosActTime, 1, nRunTime, 1);
//    htsPerPageInfo.nRunTime = nRunTime*DEFAULT_RUNTIME_UNIT;
    htsPerPageInfo.nRunTime = nRunTime;

    int nAllRunTime = 0;
    //!!!运行总时间 2B ：可以接受下一帧的时间 单位 20ms，高位在前
    CDataOperate::getValue(pBufPerPageHead + ePosActTotalTime, 2, nAllRunTime, 2, true);
//    htsPerPageInfo.nAllRunTime = nAllRunTime*DEFAULT_RUNTIME_UNIT;
    htsPerPageInfo.nAllRunTime = nAllRunTime;

///////////////////////////////////////////////////////////////////////////////////
    if (htsPerPageInfo.nAllRunTime > 1)
    {
        htsPerPageInfo.nAllRunTime = htsPerPageInfo.nAllRunTime + 2;//时间补偿
    }
///////////////////////////////////////////////////////////////////////////////////

    return true;
}

bool CHtsParser::getActTotalTime(const char *const pInDataHts, qint64 inHtsDataLen, int &nTotalTimeMiliSecond)
{
    nTotalTimeMiliSecond = -1;

    if(!pInDataHts)
    {
        return false;
    }

    if( HTS_TAIL_PART_LENGTH > inHtsDataLen)
    {
        return false;
    }


    const char * pHtsData = pInDataHts;
    //to the last point
    pHtsData = pHtsData + inHtsDataLen - 1;

    //to the tail part first point
    pHtsData = pHtsData - HTS_TAIL_PART_LENGTH + 1;

    uchar time0 = pHtsData[ePosActTotalTime];
    uchar time1 = pHtsData[ePosActTotalTime + 1];
    uchar time2 = pHtsData[ePosActTotalTime + 2];
    uchar time3 = pHtsData[ePosActTotalTime + 3];

    int ntime0 = (int)time0;
    int ntime1 = ((int)time1)<<8;
    int ntime2 = ((int)time2)<<16;
    int ntime3 = ((int)time3)<<24;

    nTotalTimeMiliSecond = ntime0 + ntime1 + ntime2 + ntime3;//ntime0 union is ms
   // nTotalTimeSecond = nTotalTimeSecond/1000;//ms to s

    return true;
}

/**************************************************************************
* 函数名: isHtsFormat
* 功能: check the buf is the hts file format
* 参数:
*    @[in ] pBuf: hts文件数据
* 返回值: bool
* 时间: 20161128
* 作者: suhaiwu
*/
bool CHtsParser::isHtsFormat(const unsigned char* const pBuf, int nLenTotal)
{
    bool bIsHtsFormatData = false;
    if(!pBuf || (nLenTotal <= 0) || (nLenTotal%HTS_PER_PAGE_SIZE != 0))
    {
        bIsHtsFormatData = false;
        return bIsHtsFormatData;
    }

    const unsigned char * const pBufStart = pBuf;
    bool bStartFormat = CHtsParser::isHtsStartFormat(pBufStart, HTS_HEAD_PART_LENGTH);
    if(!bStartFormat)
    {
        bIsHtsFormatData = false;
        return bIsHtsFormatData;
    }

    int nCOunt = nLenTotal/HTS_PER_PAGE_SIZE;
    const unsigned char * pBufTemp = NULL;
    for(int i = 1; i < nCOunt - 1; i++)
    {
        pBufTemp = pBuf + i*HTS_PER_PAGE_SIZE;

        bool bIsPerPageFormat = CHtsParser::isHtsPerPageFormat(pBufTemp, HTS_PER_PAGE_SIZE, HTS_PER_PAGE_SIZE);
        if(!bIsPerPageFormat)
        {
            return false;
        }
    }

    const unsigned char * const pBufEnd = pBuf + nLenTotal - HTS_TAIL_PART_LENGTH;
    bool bEndFormat = CHtsParser::isHtsEndFormat(pBufEnd, HTS_TAIL_PART_LENGTH);
    if(!bEndFormat)
    {
        bIsHtsFormatData = false;
        return bIsHtsFormatData;
    }

    bIsHtsFormatData = true;

    return bIsHtsFormatData;
}

/**************************************************************************
* 函数名: isHtsStartFormat
* 功能: check the buf is the hts file start format
* 参数:
*    @[in ] pBuf: hts文件数据
* 返回值: bool
* 时间: 20161128
* 作者: suhaiwu
*/
bool CHtsParser::isHtsStartFormat(const unsigned char * const pBuf, int nLen)
{
    bool bIsHtsFormatData = false;
    if(!pBuf || (nLen != HTS_HEAD_PART_LENGTH))
    {
        bIsHtsFormatData = false;

        return bIsHtsFormatData;
    }

    return CDataOperate::isAllEqual(pBuf, HTS_HEAD_PART_LENGTH, 0x00);
}

/**************************************************************************
* 函数名: isHtsPerPageFormat
* 功能: check the buf is the hts file per page format
* 参数:
*    @[in ] pBufPageHead: hts page head 文件数据
* 返回值: bool
* 时间: 20170120
* 作者: suhaiwu
*/
bool CHtsParser::isHtsPerPageFormat(const unsigned char * const pBufPageHead, int nLen, int nLenPerPageSize)
{
    if(!pBufPageHead || (nLen != nLenPerPageSize))
    {
        return false;
    }

    bool bIsHtsFormatData = false;

    const unsigned char * pBufTemp = pBufPageHead;
    if((HTS_PROTOCOL_HEAD != *(pBufTemp + ePosStart))
            && ( CDataOperate::swap(HTS_PROTOCOL_HEAD) == *(pBufTemp + ePosStart + 1)))
    {
        bIsHtsFormatData = false;
        return bIsHtsFormatData;
    }

    if(HTS_PROTOCOL_END != *(pBufTemp + ePosEnd))
    {
        bIsHtsFormatData = false;
        return bIsHtsFormatData;
    }

    const unsigned char nCheckSum = (unsigned char)(CDataOperate::checkSum(pBufTemp + 2, 29)&0xff);
    if(nCheckSum != *(pBufTemp + ePosCheck))
    {
        bIsHtsFormatData = false;
        return bIsHtsFormatData;
    }

    bIsHtsFormatData = true;
    return bIsHtsFormatData;
}

/**************************************************************************
* 函数名: isHtsEndFormat
* 功能: check the buf is the hts file end format
* 参数:
*    @[in ] pBuf: hts文件数据
* 返回值: bool
* 时间: 20161128
* 作者: suhaiwu
*/
bool CHtsParser::isHtsEndFormat(const unsigned char * const pBuf, int nLen)
{
    bool bIsHtsFormatData = false;
    if(!pBuf || (nLen != HTS_TAIL_PART_LENGTH))
    {
        bIsHtsFormatData = false;

        return bIsHtsFormatData;
    }

    return CDataOperate::isAllEqual(pBuf, 29, 0x00);
}
