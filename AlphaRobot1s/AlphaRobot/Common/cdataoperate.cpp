#include "cdataoperate.h"


#include <string.h>


CDataOperate::CDataOperate()
{

}

CDataOperate::~CDataOperate()
{

}

/**************************************************************************
* 函数名: swap
* 功能: swap the value of hight and low
* 参数:
*    @[in ] valueSwap:
* 返回值: 0XAB to 0XBA
* 时间: 20161202
* 作者: suhaiwu
*/
uchar CDataOperate::swap(uchar valueSwap)
{
    uchar charHight = (uchar)((valueSwap & 0xF0) >> 4);
    uchar charLow = valueSwap & 0x0F;

    return ((uchar)(charLow << 4) | charHight);
}

/**************************************************************************
* 函数名: CheckSum
* 功能: get the check sum
* 参数:
*    @[in ] pBuf: hts文件数据
*    @[int] nLenChecked:
* 返回值: sum
* 时间: 20161128
* 作者: suhaiwu
*/
unsigned int CDataOperate::checkSum(const unsigned char * const pBuf, int nLenChecked)
{
    unsigned int sum = 0;
    if(pBuf)
    {
       for(int i = 0; i < nLenChecked; i++)
       {
           sum += pBuf[i];
       }
    }

    return(sum);
}


/**************************************************************************
* 函数名: isAllEqual
* 功能: check the buf is the value
* 参数:
*    @[in ] pBuf: 数据
* 返回值: bool
* 时间: 20161128
* 作者: suhaiwu
*/
bool CDataOperate::isAllEqual(const unsigned char * const pBuf, int nLen, const unsigned char valueEqual)
{
    bool bIsEqual = false;
    if(!pBuf || (nLen <= 0))
    {
        bIsEqual = false;

        return bIsEqual;
    }

    for(int i = 0; i < nLen; i++)
    {
        if( valueEqual != *(pBuf + i))
        {
            bIsEqual = false;

            return bIsEqual;
        }
    }

    bIsEqual = true;

    return bIsEqual;
}

/**************************************************************************
* 函数名: getValue
* 功能: get the value saved in pbuf
* 参数:
*    @[in ] pBuf: 数据
*    @[in ] nLen: the len of data
*    @[in ] isBigendian:
* 返回值: int
* 时间: 20161128
* 作者: suhaiwu
*/
int CDataOperate::getValueInt(const unsigned char * const pBuf, int nLen, bool isBigendian)
{
    int nValue = 0;
    if(pBuf && (nLen > 0))
    {
        unsigned char byteTemp = 0;
        int nValueTemp = 0;

        if(!isBigendian)
        {
            for(int i = 0; i < nLen; i++)
            {
                byteTemp = *(pBuf +i);

                nValueTemp = ((int)byteTemp) << (8*i);

                nValue += nValueTemp;
            }
        }

        else
        {
            for(int i = (nLen - 1); i >= 0; i--)
            {
                byteTemp = *(pBuf +i);

                nValueTemp = ((int)byteTemp) << (8*(nLen - 1 - i));

                nValue += nValueTemp;
            }
        }
    }

    return nValue;
}

/**************************************************************************
* 函数名: getValue
* 功能: get the value of the ndata on position
* 参数:
*    @[in ] nData: 数据
*    @[in ] nPositionFromLow:
* 返回值: int
* 时间: 20161128
* 作者: suhaiwu
*/
byte CDataOperate::getValue(int nData, int nPositionFromLow)
{
    byte value = 0;
    if(nPositionFromLow >= sizeof(int))
    {
        return value;
    }

    int nTemp = nData >> (nPositionFromLow*8);
    nTemp = nTemp & 0x00FF;

    value = (byte)nTemp;
    return value;
}

/**************************************************************************
* 函数名: getValue
* 功能: get the value saved in pbuf
* 参数:
*    @[in ] pBuf: 数据
*    @[in ] nLen: the len of data
*    @[in ] nlenParam1: param1 data len
*    @[in ] isBigendian:
*    @[out ] nparam1: return the param1 data
* 返回值: if get the data return true, or return false
* 时间: 20170112
* 作者: suhaiwu
*/
bool CDataOperate::getValue(const unsigned char * const pBuf, int nLen,
                            int &nparam1, int nlenParam1, bool isBigendian)
{
    if(!pBuf ||
            (nlenParam1 > nLen)||
            (nLen <= 0) || (nlenParam1 <= 0))
    {
        return false;
    }

    nparam1 = CDataOperate::getValueInt(pBuf, nlenParam1, isBigendian);
    return true;
}

bool CDataOperate::getValue(const unsigned char * const pBuf, int nLen,
                            int &nparam1, int nlenParam1,
                            int &nparam2, int nlenParam2, bool isBigendian)
{

    bool bOk = CDataOperate::getValue(pBuf, nLen, nparam1, nlenParam1, isBigendian);
    bOk &= CDataOperate::getValue(pBuf + nlenParam1, nLen - nlenParam1, nparam2, nlenParam2, isBigendian);
    return bOk;
}

bool CDataOperate::getValue(const unsigned char * const pBuf, int nLen,
                            int &nparam1, int nlenParam1, int &nparam2, int nlenParam2,
                            int &nparam3, int nlenParam3, bool isBigendian)
{
    bool bOk = CDataOperate::getValue(pBuf, nLen, nparam1, nlenParam1, isBigendian);
    bOk &= CDataOperate::getValue(pBuf + nlenParam1, nLen - nlenParam1,
                                  nparam2, nlenParam2,
                                  nparam3, nlenParam3,
                                  isBigendian);
    return bOk;
}

bool CDataOperate::getValue(const unsigned char * const pBuf, int nLen,
                            int &nparam1, int nlenParam1, int &nparam2, int nlenParam2,
                            int &nparam3, int nlenParam3, int &nparam4, int nlenParam4, bool isBigendian)
{
    bool bOk = CDataOperate::getValue(pBuf, nLen, nparam1, nlenParam1, isBigendian);
    bOk &= CDataOperate::getValue(pBuf + nlenParam1, nLen - nlenParam1,
                                  nparam2, nlenParam2,
                                  nparam3, nlenParam3,
                                  nparam4, nlenParam4,
                                  isBigendian);
    return bOk;
}

bool CDataOperate::getValue(const unsigned char * const pBuf, int nLen,
                            int &nparam1, int nlenParam1, int &nparam2, int nlenParam2,
                            int &nparam3, int nlenParam3, int &nparam4, int nlenParam4,
                            int &nparam5, int nlenParam5, bool isBigendian)
{
    bool bOk = CDataOperate::getValue(pBuf, nLen, nparam1, nlenParam1, isBigendian);
    bOk &= CDataOperate::getValue(pBuf + nlenParam1, nLen - nlenParam1,
                                  nparam2, nlenParam2,
                                  nparam3, nlenParam3,
                                  nparam4, nlenParam4,
                                  nparam5, nlenParam5,
                                  isBigendian);
    return bOk;
}

bool CDataOperate::getValue(const unsigned char * const pBuf, int nLen,
                            int &nparam1, int nlenParam1, int &nparam2, int nlenParam2,
                            int &nparam3, int nlenParam3, int &nparam4, int nlenParam4,
                            int &nparam5, int nlenParam5, int &nparam6, int nlenParam6, bool isBigendian)
{
    bool bOk = CDataOperate::getValue(pBuf, nLen, nparam1, nlenParam1, isBigendian);
    bOk &= CDataOperate::getValue(pBuf + nlenParam1, nLen - nlenParam1,
                                  nparam2, nlenParam2,
                                  nparam3, nlenParam3,
                                  nparam4, nlenParam4,
                                  nparam5, nlenParam5,
                                  nparam6, nlenParam6,
                                  isBigendian);
    return bOk;
}

bool CDataOperate::getValue(const unsigned char * const pBuf, int nLen,
                            int &nparam1, int nlenParam1, int &nparam2, int nlenParam2,
                            int &nparam3, int nlenParam3, int &nparam4, int nlenParam4,
                            int &nparam5, int nlenParam5, int &nparam6, int nlenParam6,
                            int &nparam7, int nlenParam7, bool isBigendian)
{
    bool bOk = CDataOperate::getValue(pBuf, nLen, nparam1, nlenParam1, isBigendian);
    bOk &= CDataOperate::getValue(pBuf + nlenParam1, nLen - nlenParam1,
                                  nparam2, nlenParam2,
                                  nparam3, nlenParam3,
                                  nparam4, nlenParam4,
                                  nparam5, nlenParam5,
                                  nparam6, nlenParam6,
                                  nparam7, nlenParam7,
                                  isBigendian);
    return bOk;
}

/**************************************************************************
* 函数名: package
* 功能: package the datas
* 参数:
*    @[in ] pBuf: 数据
*    @[in ] nLen: the len of data
* 返回值: nLenSaved  the len of data had be saved
* 时间: 20161207
* 作者: asu
*/
int CDataOperate::package(unsigned char *pBufSaved,
                          const unsigned char * const pBufHead, int nLenHead,
                          const unsigned char * const pBufEnd, int nLenEnd)
{
    int nLenSaved = 0;
    if(!pBufSaved || !pBufHead || !pBufEnd
            ||(nLenHead <= 0) || (nLenEnd <= 0))
    {
        return nLenSaved;
    }

    memcpy(pBufSaved, pBufHead, nLenHead);
    pBufSaved += nLenHead;

    memcpy(pBufSaved, pBufEnd, nLenEnd);

    nLenSaved = nLenHead + nLenEnd;

    return nLenSaved;
}

/**************************************************************************
* 函数名: package
* 功能: package the datas to vect
* 参数:
*    @[in ] pBufToPackaged: 数据
*    @[in ] nLenToPackaged: the len of data
* 返回值:the len of data had be packaged
* 时间: 20161207
* 作者: asu
*/
int CDataOperate::package(vecByteLst &vectSaved, const uchar * const pBufToPackaged, int nLenToPackaged)
{
    vectSaved.clear();

    int nDataHadPackaged = 0;    
    nDataHadPackaged = CDataOperate::packageByAppend(vectSaved, pBufToPackaged, nLenToPackaged);

    return nDataHadPackaged;
}

int CDataOperate::packageByAppend(vecByteLst &vectSaved, const uchar * const pBufToPackaged, int nLenToPackaged)
{
    int nDataHadPackaged = 0;

    if(pBufToPackaged)
    {
        for(int i = 0; i < nLenToPackaged; ++i)
        {
            vectSaved.push_back(*(pBufToPackaged + i));

            nDataHadPackaged++;
        }
    }

    return nDataHadPackaged;
}

/**************************************************************************
* 函数名: vectToArr
* 功能: vect list datas to byte datas
* 参数:
*    @[in ] vecLst: 数据
*    @[in ] pByte: save the transformed data
*    @[in ] nLenToTransform: the len of data to be transform
* 返回值:the len of data had be transform
* 时间: 20170109
* 作者: asu
*/
int CDataOperate::vectToArr(const vecByteLst& vecLst, byte*& pByte, int nLenToTransform)
{
    if((vecLst.size() <= 0) || (nLenToTransform < -1))
    {
        return 0;
    }

    const int nLenVeclst = vecLst.size();
    int nTempLen = nLenVeclst;
    if(nLenToTransform != -1)
    {
        nTempLen = MIN(nTempLen, nLenToTransform);
    }

    if(nTempLen <= 0)
    {
        return 0;
    }

    pByte = new byte[nTempLen];
    int nIndex = 0;
    for(; (nIndex < nTempLen) && (nIndex < nLenVeclst) ; nIndex++)
    {
        *(pByte + nIndex) = vecLst.at(nIndex);
    }

    return nIndex;
}

