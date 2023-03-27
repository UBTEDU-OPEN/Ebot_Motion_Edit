#include "ccomandparsernewbt.h"
#include "cdataoperate.h"

#include <QDebug>


#include "DataPool.h"



const uchar BT_CMD_HEAD_NEW = 0XFE;
const uchar BT_CMD_END_NEW = 0XED;


const int DEFAULT_MAX_LEN_CMD_ARRAY = 1024;



//命令头(2B) +长度(2B) +命令(1B) +<参数1[参数2][参数3](nB) >+ checksum(1B)  + 结束符(1B)
int CComandParserNewBT::m_nHeadSize = 2;
int CComandParserNewBT::m_nLenFlagSize = 2;
int CComandParserNewBT::m_nCmdIDFlagSize = 1;
int CComandParserNewBT::m_nChksumSize = 1;
int CComandParserNewBT::m_nTailSize = 1;

CComandParserNewBT::CComandParserNewBT()
{

}

CComandParserNewBT::~CComandParserNewBT()
{

}


////////////////////////////////////////////////////////////////////////////////////

int CComandParserNewBT::getHeadSize()
{
    return m_nHeadSize;
}

int CComandParserNewBT::getLenFlagSize()
{
    return m_nLenFlagSize;
}

int CComandParserNewBT::getCmdIDFlagSize()
{
    return m_nCmdIDFlagSize;
}

int CComandParserNewBT::getChksumSize()
{
    return m_nChksumSize;
}

int CComandParserNewBT::getTailSize()
{
    return m_nTailSize;
}


vecByteLst CComandParserNewBT::packageBTCmd(int nCmdID, const uchar *pBufCmd, int nLenCmd)
{
    vecByteLst vecSaved;
    vecSaved.clear();

    uchar head[] = {BT_CMD_HEAD_NEW, CDataOperate::swap(BT_CMD_HEAD_NEW)};
    //head
    const int nLenHead = sizeof(head);
    CDataOperate::packageByAppend(vecSaved, head, nLenHead);

    //len
    int nLenCmdAndParams = 0;
    if(pBufCmd && (nLenCmd > 0))
    {
        nLenCmdAndParams = CComandParserNewBT::m_nCmdIDFlagSize + nLenCmd;
    }
    else
    {
        nLenCmdAndParams = CComandParserNewBT::m_nCmdIDFlagSize;
    }

    const int nLenCmdTemp = CComandParserNewBT::getCmdLenByParamsLen(nLenCmdAndParams);
    int nLenCmdTempLow = CDataOperate::getValue(nLenCmdTemp, 0);
    int nLenCmdTempHight = CDataOperate::getValue(nLenCmdTemp, 1);

    vecSaved.push_back((uchar)nLenCmdTempLow);
    vecSaved.push_back((uchar)nLenCmdTempHight);

    //cmd
    vecSaved.push_back((uchar)nCmdID);

    //params
    CDataOperate::packageByAppend(vecSaved, pBufCmd, nLenCmd);


    //check
    uchar checkSum = (uchar)CDataOperate::checkSum(pBufCmd, nLenCmd);
    //qDebug()<<"1 checkSum="<<checkSum<<"\n";

    checkSum += (uchar)nLenCmdTempLow;
    checkSum += (uchar)nLenCmdTempHight;
    checkSum += (uchar)nCmdID;

    vecSaved.push_back(checkSum);
    //qDebug()<<QString("1 checkSum=%1, nCmdID=%2, nLenCmdTemp=%3\n").arg(checkSum).arg((uchar)nCmdID).arg(nLenCmdTemp)<<"\n";

    //end
    vecSaved.push_back((uchar)BT_CMD_END_NEW);

    return vecSaved;
}

int CComandParserNewBT::getCmdID(const uchar *pBufTolal, int nLenTotal)
{
    if(pBufTolal)
    {
        int nPos = CComandParserNewBT::m_nHeadSize + CComandParserNewBT::m_nLenFlagSize;
        if(nLenTotal > nPos)
        {
            int nCmdID = CDataOperate::getValueInt(pBufTolal + nPos, CComandParserNewBT::m_nCmdIDFlagSize, false);
            return nCmdID;
        }
    }

    return 0;
}

vecByteLst CComandParserNewBT::getCmd_downloadBinIng(int nFileCurrFrame, const byte *pDate, int nLenDate)
{
//    if(!pDate || nLenDate <= 0 || nFileCurrFrame <= 0)
//    {
//        return vecByteLst();
//    }

//    byte data[DEFAULT_MAX_LEN_CMD_ARRAY] = {0x00};
//    const int nLenAll = 2 + nLenDate;
//    memcpy(data, &nFileCurrFrame, 2);
//    return CComandParserNewBT::packageBTCmd(eBTCmdDownloadBinIng, data, nLenAll);

    return vecByteLst();
}


////////////////////////////////////////////////////////////////////////////////////

int CComandParserNewBT::getCmdLenByParamsLen(int nLenCmdAndParams)
{
    //长度:命令头2+长度2+<命令2+参数>+checksum1的长度.
    return (CComandParserNewBT::m_nHeadSize
            + m_nLenFlagSize
            + nLenCmdAndParams
            + CComandParserNewBT::getChksumSize());
}

int CComandParserNewBT::getCmdTotalLenByParamsLen(int nLenCmdAndParams)
{
    return CComandParserNewBT::getCmdLenByParamsLen(nLenCmdAndParams) + CComandParserNewBT::m_nTailSize;
}
