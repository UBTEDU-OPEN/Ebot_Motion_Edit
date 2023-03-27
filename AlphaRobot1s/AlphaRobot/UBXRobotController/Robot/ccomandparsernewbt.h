#ifndef CCOMANDPARSERNEWBT_H
#define CCOMANDPARSERNEWBT_H


#include "cbluetoothcomandparser.h"



class CComandParserNewBT
{
public:
    CComandParserNewBT();
    ~CComandParserNewBT();


public:
    //获取头部字节长度
    static int getHeadSize();
    static int getLenFlagSize();
    static int getCmdIDFlagSize();
    //获取校验码字节字节长度
    static int getChksumSize();
    //获取尾部字节长度
    static int getTailSize();


    /**************************************************************************
     * 函数名: packageBTCmd
     * 功能:
     * 参数:
     *    @[int] pBufCmd: cmd and the params datas(命令(2B) +<参数1[参数2][参数3](nB) >)
     *    @[out]
     * 返回值: 命令数据vecByteLst
     * 时间: 20161207
     * 作者: asu
     */
    static vecByteLst packageBTCmd(int nCmdID, const uchar *pBufCmd = NULL, int nLenCmd = 0);


    static int getCmdID(const uchar *pBufTolal, int nLenTotal);


public:
    /**************************************************************************
     * 函数名: getCmd_downloadBinIng
     * 功能:
     * 参数:
     * 返回值: 命令数据
     * 时间: 20161209
     * 作者: asu
     */
    static vecByteLst getCmd_downloadBinIng(int nFileCurrFrame, const byte* pDate, int nLenDate);



private:
    //长度:命令头+长度+命令+参数+checksum的长度.
    static int getCmdLenByParamsLen(int nLenCmdAndParams);
    static int getCmdTotalLenByParamsLen(int nLenCmdAndParams);


private:
    static int m_nHeadSize;
    static int m_nLenFlagSize;
    static int m_nCmdIDFlagSize;
    static int m_nChksumSize;
    static int m_nTailSize;
};

#endif // CCOMANDPARSERNEWBT_H
