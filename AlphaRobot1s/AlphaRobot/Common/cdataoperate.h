#ifndef CDATAOPERATE_H
#define CDATAOPERATE_H


class CDataOperate
{
public:
    CDataOperate();
    virtual ~CDataOperate();

public:
    /**************************************************************************
    * 函数名: swap
    * 功能: swap the value of hight and low
    * 参数:
    *    @[in ] valueSwap:
    * 返回值: 0XAB to 0XBA
    * 时间: 20161202
    * 作者: suhaiwu
    */
    static uchar swap(uchar valueSwap);

    /**************************************************************************
    * 函数名: CheckSum
    * 功能: get the check sum
    * 参数:
    *    @[in ] pBuf:
    *    @[int] nLenChecked:
    * 返回值: sum
    * 时间: 20161128
    * 作者: suhaiwu
    */
    static unsigned int checkSum(const unsigned char* const pBuf, int nLenChecked);


    /**************************************************************************
    * 函数名: isAllEqual
    * 功能: check the buf is the value
    * 参数:
    *    @[in ] pBuf: hts文件数据
    * 返回值: bool
    * 时间: 20161128
    * 作者: suhaiwu
    */
    static bool isAllEqual(const unsigned char* const pBuf, int nLen, const unsigned char valueEqual);

    /**************************************************************************
    * 函数名: getValueInt
    * 功能: get the value saved in pbuf
    * 参数:
    *    @[in ] pBuf: 数据
    *    @[in ] nLen: the len of data
    *    @[in ] isBigendian:
    * 返回值: int
    * 时间: 20161128
    * 作者: suhaiwu
    */
    static int getValueInt(const unsigned char* const pBuf, int nLen, bool isBigendian);

    /**************************************************************************
    * 函数名: getValue
    * 功能: get the value of the ndata on position
    * 参数:
    *    @[in ] nData: 数据
    *    @[in ] nPositionFromLow: start from 0
    * 返回值: int
    * 时间: 20161128
    * 作者: suhaiwu
    */
    static byte getValue(int nData, int nPositionFromLow);

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
    static bool getValue(const unsigned char* const pBuf, int nLen,
                         int& nparam1, int nlenParam1,
                         bool isBigendian = false);
    static bool getValue(const unsigned char* const pBuf, int nLen,
                         int& nparam1, int nlenParam1,
                         int& nparam2, int nlenParam2,
                         bool isBigendian = false);
    static bool getValue(const unsigned char* const pBuf, int nLen,
                         int& nparam1, int nlenParam1,
                         int& nparam2, int nlenParam2,
                         int& nparam3, int nlenParam3,
                         bool isBigendian = false);
    static bool getValue(const unsigned char* const pBuf, int nLen,
                         int& nparam1, int nlenParam1,
                         int& nparam2, int nlenParam2,
                         int& nparam3, int nlenParam3,
                         int& nparam4, int nlenParam4,
                         bool isBigendian = false);
    static bool getValue(const unsigned char* const pBuf, int nLen,
                         int& nparam1, int nlenParam1,
                         int& nparam2, int nlenParam2,
                         int& nparam3, int nlenParam3,
                         int& nparam4, int nlenParam4,
                         int& nparam5, int nlenParam5,
                         bool isBigendian = false);
    static bool getValue(const unsigned char* const pBuf, int nLen,
                         int& nparam1, int nlenParam1,
                         int& nparam2, int nlenParam2,
                         int& nparam3, int nlenParam3,
                         int& nparam4, int nlenParam4,
                         int& nparam5, int nlenParam5,
                         int& nparam6, int nlenParam6,
                         bool isBigendian = false);
    static bool getValue(const unsigned char* const pBuf, int nLen,
                         int& nparam1, int nlenParam1,
                         int& nparam2, int nlenParam2,
                         int& nparam3, int nlenParam3,
                         int& nparam4, int nlenParam4,
                         int& nparam5, int nlenParam5,
                         int& nparam6, int nlenParam6,
                         int& nparam7, int nlenParam7,
                         bool isBigendian = false);


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
    static int package(unsigned char* pBufSaved,
                       const unsigned char* const pBufHead, int nLenHead,
                       const unsigned char* const pBufEnd, int nLenEnd);


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
    static int package(vecByteLst& vectSaved, const uchar* const pBufToPackaged, int nLenToPackaged);


    /**************************************************************************
    * 函数名: packageByAppend
    * 功能: Append the datas to vect
    * 参数:
    *    @[in ] pBufToPackaged: 数据
    *    @[in ] nLenToPackaged: the len of data
    * 返回值:the len of data had be packaged
    * 时间: 20161207
    * 作者: asu
    */
    static int packageByAppend(vecByteLst& vectSaved, const uchar* const pBufToPackaged, int nLenToPackaged);


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
    static int vectToArr(const vecByteLst& vecLst, byte*& pByte, int nLenToTransform = -1);

};

#endif // CDATAOPERATE_H


