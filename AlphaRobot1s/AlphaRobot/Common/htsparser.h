
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


//头部33B
//0-32全部填充0x00

//中部N*33B
//0-1 协议头2B：0xFB 0xBF
//2   保留位 1B：默认是0x01
//3   帧状态位1B：0x01 第一帧 0x02 中间帧  0x03：结束帧（如果只有一帧为0x01）
//4-5 总帧 2B：低位在前
//6-7 当前帧 2B ：低位在前
//8-27 各舵机角度值 20B
//28运行时间 1B ：速度单位：20ms
//29-30运行总时间 2B ：可以接受下一帧的时间单位 20ms，高位在前
//31校验1B：求和从保留位到运行总时间的和的低八位
//32协议尾 1B: 0xED

//尾部 33B
//0-28全部填充0x00
//29-32  总时间4B: 所有动作的总时间之和，单位ms，低位在前


#ifndef CHTSPARSER_H
#define CHTSPARSER_H


#include "cdataoperate.h"
#include "UBXPublic.h"


#define DEFAULT_RUNTIME_UNIT   20


const int HTS_PER_PAGE_SIZE = 33;
const int HTS_HEAD_PART_LENGTH = 33;
const int HTS_TAIL_PART_LENGTH = 33;


const int HTS_PROTOCOL_HEAD = 0XFB;
const int HTS_PROTOCOL_END = 0XED;


enum emHtsFlagPos
{
    ePosStart = 0,

    ePosFrameStat = 3,
    ePosFrameTotalCount = 4,
    ePosFrameCur = 6,
    ePosSteerAngle = 8,
    ePosActTime = 28,
    ePosActTotalTime = 29,
    ePosCheck = 31,

    ePosEnd = 32
};


enum emHtsFrameStat
{
    eFrameInvalid = 0,

    eFrameFisrt = 0x01,//（如果只有一帧为0x01）
    eFrameMid = 0x02,
    eFrameEnd = 0x03,

    eFramePlayActFail, //
};


//进程消息
typedef struct tagHTSPERPAGEINFO
{
    emHtsFrameStat eHtsFrameStat;
    int nAllFrame;//总帧
    int nCurrentFrame;//当前帧
    int nRunTime; //运行时间, unit 20ms
    int nAllRunTime; //运行总时间, 单位 20ms
    MOTOR_DATA motorData[ROBOT_TOTAL_MOTOR_NUMBER];//各舵机info, such as角度值
}HTS_PER_PAGE_INFO;


class CHtsParser
{
public:
    CHtsParser();
    ~CHtsParser();

public:
    /**************************************************************************
    * 函数名: findHtsPerPageHeadPoint
    * 功能: find Hts Per Page Head Point by hts data
    * 参数:
    *    @[in ] pBuf: hts文件数据
    * 返回值: if find retrun true
    * 时间: 20170120
    * 作者: suhaiwu
    */
    static bool findHtsPerPageHeadPoint(const unsigned char* const pBuf, int64 nHtsDataLen,
                                        const unsigned char*& pBufPerPageHead, int nPerPageSize = HTS_PER_PAGE_SIZE);


    /**************************************************************************
    * 函数名: parserHtsPerPageInfo
    * 功能: parser and get Hts PerPage Info
    * 参数:
    *    @[in ] pBufPerPageHead: point to the page head of hts data
    * 返回值: if find retrun true
    * 时间: 20170120
    * 作者: suhaiwu
    */
    static bool parserHtsPerPageInfo(const unsigned char* const pBufPerPageHead, int nLenData, HTS_PER_PAGE_INFO& htsPerPageInfo);


    /**************************************************************************
    * 函数名: parserActTotalTime
    * 功能: get hts action time
    * 参数:
    *    @[in ] pInDataHts: hts文件数据(the hts file all datas)
    *    @[out] nTotalTimeSecond: action total time， union is msecond
    * 返回值: 成功返回TRUE，失败返回FALSE
    * 时间: 2016/07/25
    * 作者: suhaiwu
    *
    */
    static bool getActTotalTime(const char *const pInDataHts, qint64 inHtsDataLen, int& nTotalTimeMiliSecond);


    /**************************************************************************
    * 函数名: isHtsFormat
    * 功能: check the buf is the hts file format
    * 参数:
    *    @[in ] pBuf: hts文件数据
    * 返回值: bool
    * 时间: 20161128
    * 作者: suhaiwu
    */
    static bool isHtsFormat(const unsigned char* const pBuf, int nLenTotal);


    /**************************************************************************
    * 函数名: isHtsStartFormat
    * 功能: check the buf is the hts file start format
    * 参数:
    *    @[in ] pBuf: hts文件数据
    * 返回值: bool
    * 时间: 20161128
    * 作者: suhaiwu
    */
    static bool isHtsStartFormat(const unsigned char* const pBuf, int nLen);


    /**************************************************************************
    * 函数名: isHtsPerPageFormat
    * 功能: check the buf is the hts file per page format
    * 参数:
    *    @[in ] pBufPageHead: hts page head 文件数据
    * 返回值: bool
    * 时间: 20170120
    * 作者: suhaiwu
    */
    static bool isHtsPerPageFormat(const unsigned char* const pBufPageHead, int nLen, int nLenPerPageSize);


    /**************************************************************************
    * 函数名: isHtsEndFormat
    * 功能: check the buf is the hts file end format
    * 参数:
    *    @[in ] pBuf: hts文件数据
    * 返回值: bool
    * 时间: 20161128
    * 作者: suhaiwu
    */
    static bool isHtsEndFormat(const unsigned char* const pBuf, int nLen);
};

#endif // CHTSPARSER_H
