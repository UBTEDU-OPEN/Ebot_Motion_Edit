#include "stdafx.h"
#include "UBXPublic.h"
#include <QTextCodec>
#include <QtMath>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>
#include <QSettings>
#include <QLocale>


#include "../zxLib/configs.h"
#include "../common/filedirhandle.h"
#include "../zxLib/ciniconfig.h"
#include "robotDefine.h"

#ifdef QUAZIP_ENABLE
#include "quazip.h"
#include "quazipfile.h"
#endif

/*****************************/
//全局变量定义BEGIN
/****************************/

//emUbtProductType UBXBase::getInstance()->getCurrentRobotProductType() = eUbtProductUnknown;//eUbtProduct1E;//eUbtProductUnknown;

QString       g_sActPath;                    //动作库路径
QString       g_sDownloadedActInfoXMLPath;   //已下载动作列表配置文件路径
QString       g_sDownloadingActInfoXMLPath;  //正在下载动作列表配置文件路径

QString       g_sServer = "";  //服务器地址
QString       g_sUpdateURL = "";  //更新地址

////动作类型前缀
//const QString g_sActTypePrefix[] =
//{
//    "",   //全部动作
//    "@",  //基本动作
//    "#",  //音乐舞蹈
//    "%",  //寓言故事
//    ""   //其他
//};

bool          g_bIsRecordLogValue = false;              //是否记录日志键值

#if 1
int           g_nBigFrameDelay = 0;     //大帧延时，单位毫秒
int           g_nSmallFrameDelay = 0;   //小帧延时，单位毫秒
#endif

/*****************************/
//全局变量定义END
/*****************************/
// 添加数据到VECTOR里，同时返回VECTOR的长度
int AddVectorData(vector<char>* pVerData, char* pData, int nLen)
{
    if (!(pVerData && pData))
        return 0;

    for (int i=0; i<nLen; i++)
    {
        pVerData->push_back(pData[i]);
    }

    return pVerData->size();
}

void ModifyVertorData(vector<char>& pVerData, int nPos, char* pData, int nLen)
{
    if (! pData )
        return ;

    for (int i = nPos, k = 0; k < nLen; i++, k++)
    {
        pVerData[i] = pData[k];
    }
}

int CopyVertorToPointer(vector<char>& pVerData, char* pData)
{
    if (pData == NULL)
        return 0;

    for (int i=0; i<pVerData.size(); i++)
    {
        pData[i] = pVerData[i];
    }

    return pVerData.size();
}

/**************************************************************************
* 函数名: UnicodeToAnsi
* 功能: Unicode字符串转换为Ansi字符串
* 参数:
*    @[in ] sUnicodeStr: Unicode字符串
*    @[out] pOutStr: Ansi字符串，需在外释放
* 返回值: Ansi字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int UnicodeToAnsi(const QString& sUnicodeStr, char*& pOutStr)
{
    QTextCodec* pCodec = QTextCodec::codecForName("GB2312");
    QByteArray sArr = pCodec->fromUnicode(sUnicodeStr);
    pOutStr = new char[sArr.length() + 1];
    memset(pOutStr, 0, sArr.length() + 1);
    memcpy(pOutStr, sArr.data(), sArr.length());
    return sArr.length();
}

/**************************************************************************
* 函数名: UnicodeToUtf8
* 功能: Unicode字符串转换为Utf8字符串
* 参数:
*    @[in ] sUnicodeStr: Unicode字符串
*    @[out] pOutStr: Utf8字符串，需在外释放
* 返回值: Utf8字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int UnicodeToUtf8(const QString& sUnicodeStr, char*& pOutStr)
{
    QTextCodec* pCodec = QTextCodec::codecForName("UTF-8");
    QByteArray sArr = pCodec->fromUnicode(sUnicodeStr);
    pOutStr = new char[sArr.length() + 1];
    memset(pOutStr, 0, sArr.length() + 1);
    memcpy(pOutStr, sArr.data(), sArr.length());
    return sArr.length();
}

/**************************************************************************
* 函数名: AnsiToUnicode
* 功能: Ansi字符串转换为Unicode字符串
* 参数:
*    @[in ] pszAnsiStr: Ansi字符串
* 返回值: Unicode字符串
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
QString AnsiToUnicode(const char* pszAnsiStr)
{
    QTextCodec* pCodec = QTextCodec::codecForName("GB2312");
    return pCodec->toUnicode(pszAnsiStr);
}

/**************************************************************************
* 函数名: AnsiToUtf8
* 功能: Ansi字符串转换为Utf8字符串
* 参数:
*    @[in ] pszAnsiStr: Ansi字符串
*    @[out] pOutStr: Utf8字符串，需在外释放
* 返回值: Utf8字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int AnsiToUtf8(const char* pszAnsiStr, char*& pOutStr)
{
    QString sUnicode = AnsiToUnicode(pszAnsiStr);
    return UnicodeToUtf8(sUnicode, pOutStr);
}

/**************************************************************************
* 函数名: Utf8ToAnsi
* 功能: Utf8字符串转换为Ansi字符串
* 参数:
*    @[in ] pszUtf8Str: Utf8字符串
*    @[out] pOutStr: Ansi字符串，需在外释放
* 返回值: Ansi字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int Utf8ToAnsi(const char* pszUtf8Str, char*& pOutStr)
{
    QTextCodec* pCodec = QTextCodec::codecForName("UTF-8");
    QString sUnicode = pCodec->toUnicode(pszUtf8Str);
    return UnicodeToAnsi(sUnicode, pOutStr);
}

/**************************************************************************
* 函数名: Utf8ToAnsiEx
* 功能: Utf8字符串转换为Ansi字符串
* 参数:
*    @[in ] pszUtf8Str: Utf8字符串
*    @[in ] nUtf8Len: Utf8数据长度
* 返回值: Ansi字符串
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
string Utf8ToAnsiEx(const byte *pszUtf8Str, int nUtf8Len)
{
    string sRet = "";

    QTextCodec* pCodec = QTextCodec::codecForName("UTF-8");
    QString sUnicode = pCodec->toUnicode((char*)pszUtf8Str);

    char* pOutStr = NULL;
    if(UnicodeToAnsi(sUnicode, pOutStr) > 0)
    {
        sRet = pOutStr;
    }

    SAFE_DELETE_ARRAY(pOutStr);
    return sRet;
}

/**************************************************************************
* 函数名: Utf8ToUnicode
* 功能: Utf8字符串转换为Unicode字符串
* 参数:
*    @[in ] pszUtf8Str: Utf8字符串
* 返回值: Unicode字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
QString Utf8ToUnicode(const char* pszUtf8Str)
{
    QTextCodec* pCodec = QTextCodec::codecForName("UTF-8");
    return pCodec->toUnicode(pszUtf8Str);
}

/**************************************************************************
* 函数名: AnsiToWChar
* 功能: Ansi字符串转换为WChar字符串
* 参数:
*    @[in ] pszAnsiStr: Ansi字符串
*    @[in ] nAnsiLen: Ansi字符串字节长度
*    @[out] pWCharStr: WChar字符串
* 返回值: WChar字符串字节长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int AnsiToWChar(const char* pszAnsiStr, int nAnsiLen, WCHAR* pWCharStr)
{
    for(int i = 0; i < nAnsiLen; i++)
    {
        memcpy(pWCharStr + i, pszAnsiStr + i, 1);
    }
    return nAnsiLen * sizeof(WCHAR);
}

/**************************************************************************
* 函数名: WCharToAnsi
* 功能: WChar字符串转换为Ansi字符串
* 参数:
*    @[in ] pWCharStr: WChar字符串
*    @[in ] nWCharLen: WChar字符串字节长度
*    @[out] pszAnsiStr: Ansi字符串
* 返回值: Ansi字符串字节长度
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
int WCharToAnsi(const WCHAR* pWCharStr, int nWCharLen, char* pszAnsiStr)
{
    int nAnsiLen = nWCharLen / sizeof(WCHAR);
    for(int i = 0; i < nAnsiLen; i++)
    {
        memcpy(pszAnsiStr + i, pWCharStr + i, 1);
    }
    return nAnsiLen;
}

/**************************************************************************
* 函数名: PathFileExists
* 功能: 判断路径是否存在
* 参数:
*    @[in ] sPath: 路径
* 返回值: 存在返回true，失败返回false
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
bool PathFileExists(const QString& sPath)
{
    QFileInfo fileinfo(sPath);
    return fileinfo.exists();
}

/**************************************************************************
* 函数名: GetFormatSizeString
* 功能: 获取格式化后的大小字符串
* 参数:
*    @[in ] nSize: 大小
* 返回值: 格式化后的大小字符串
* 时间: 2015/12/25 17:38
* 作者: ZDJ
*/
QString GetFormatSizeString(qint64 nSize)
{
    QString sRet;
    const qint64 nUnitSize = 1024L;
    if(nSize < nUnitSize)
        sRet.sprintf("%lldB", nSize);
    else if(nSize >= nUnitSize && nSize < nUnitSize * nUnitSize)
        sRet.sprintf("%.1fKB", nSize * 1.0 / nUnitSize);
    else if(nSize >= nUnitSize * nUnitSize && nSize < nUnitSize * nUnitSize * nUnitSize)
        sRet.sprintf("%.1fMB", nSize * 1.0 / (nUnitSize * nUnitSize));
    else if(nSize >= nUnitSize * nUnitSize * nUnitSize && nSize < nUnitSize * nUnitSize * nUnitSize * nUnitSize)
        sRet.sprintf("%.1fGB", nSize * 1.0 / (nUnitSize * nUnitSize * nUnitSize));
    else
        sRet.sprintf("%lldB", nSize);

    return sRet;
}

/**************************************************************************
* 函数名: WCSLen
* 功能: 获取WCHAR字符串元素长度
* 参数:
*    @[in ] pWchar: WCHAR字符串
* 返回值: 字符串元素长度
* 时间: 2015/12/25 17:38
* 作者: ZZP
*/
int WCSLen(WCHAR * pWchar)
{
    int nLen = 0;
    while(*pWchar++ != '\0')
        nLen++;
    return nLen;
}

/**************************************************************************
* 函数名: QStringToWCHAR
* 功能: QString字符串转换为WCHAR字符串
* 参数:
*    @[out] pWchar: WCHAR字符串
*    @[in ] str: QString字符串
* 返回值: WCHAR字符串长度
* 时间: 2015/12/25 17:38
* 作者: ZZP
*/
int  QStringToWCHAR(OUT WCHAR* pWchar, IN const QString& str)
{
    QTextCodec* pCodec = QTextCodec::codecForName("UTF-16LE");
    QByteArray sArr = pCodec->fromUnicode(str);
    memcpy(pWchar, sArr.data() + 2, sArr.length() - 2);
    return sArr.length() - 2;
}

/**************************************************************************
* 函数名: WCHARToQString
* 功能: WCHAR字符串转换为QString字符串
* 参数:
*    @[in ] pwchar: WCHAR字符串
*    @[in ] nbyteLen: WCHAR字符串长度
* 返回值: QString字符串
* 时间: 2015/12/25 17:38
* 作者: ZZP
*/
QString  WCHARToQString(IN WCHAR* pwchar, IN int nbyteLen)
{
     int nLen = nbyteLen/sizeof(WCHAR) + 1;
    WCHAR* pBuf = new WCHAR[nLen];
    memset(pBuf, 0, nLen*sizeof(WCHAR));
    memcpy(pBuf, pwchar, nbyteLen);

    QString sUnicode = QString::fromUtf16((const ushort*)pBuf);
    SAFE_DELETE_ARRAY(pBuf);
    return sUnicode;
}

float Round(float fValue , int nDigits)
{
    if (nDigits < 0 || fValue < 0)
    {
        return -1;
    }
    int nTemp = 1;
    for ( int i = 0;i<nDigits;i++)
    {
        nTemp *= 10;
    }
    fValue *= nTemp;
    float fTemp =  qFloor(fValue + 0.5);
    return (float)fTemp / nTemp;
}

QString TimeStringCustomise(int nHour , int nMinute ,int nSec)
{
    QString str = QObject::tr("");
    QString strHour;
    QString strMinute;
    QString strSec;
    //hour
    //int nHour = (i + 1)  / 3600;
    if (nHour < 10)
    {
        strHour =QString(QObject::tr("0%d")).arg(nHour);
    }
    else
        strHour =QString(QObject::tr("%d")).arg(nHour);
    //minute
    //int nMinute = ((i + 1)  % 3600) / 60 ;
    if (nMinute < 10)
    {
        strMinute =QString(QObject::tr("0%d")).arg(nMinute);
    }
    else
        strMinute =QString(QObject::tr("%d")).arg(nMinute);
    // sec
    //int nSec = (i + 1)  % 60;
    if (nSec < 10)
    {
        strSec =QString(QObject::tr("0%d")).arg(nSec);
    }
    else
        strSec =QString(QObject::tr("%d")).arg(nSec);

    str =QString(QObject::tr("%s:%s:%s")).arg(strHour , strMinute ,strSec);
    return str;
}

/**************************************************************************
* 函数名: QtPostMessage
* 功能: 发送消息
* 参数:
*    @[in ] pDest: 目标窗口
*    @[in ] dwMsg: 消息ID
*    @[in ] dwContext: 消息上下文
*    @[in ] wParam: 消息数据长度
*    @[in ] lParam: 消息数据
* 返回值: void
* 时间: 2016/02/01
* 作者: ZDJ
*/
void QtPostMessage(QObject* pDest, DWORD dwMsg, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    zxEvent* pNewEvent = new zxEvent(dwMsg);
    pNewEvent->eEventType = zxEventTypeWindow;
    pNewEvent->lReceiver = (long)pDest;    
    pNewEvent->dwMsgId = dwMsg;
    pNewEvent->dwContext = dwContext;
    pNewEvent->wParam = wParam;
    pNewEvent->lParam = lParam;
    QCoreApplication::postEvent(pDest, pNewEvent);
}


#ifdef QUAZIP_ENABLE

/**************************************************************************
* 函数名: UnZipFile
* 功能: 解压zip文件
* 参数:
*    @[in ] sZipPath: zip文件路径
*    @[in ] pszDestDir: 解压文件夹路径，为NULL时解压到当前文件夹
* 返回值: 成功返回true，失败返回false
* 时间: 2016/03/10 16:59
* 作者: ZDJ
*/
bool UnZipFile(const QString& sZipPath, const char* pszDestDir)
{
    bool bRet = false;
    QuaZip zip(sZipPath);
    if(zip.open(QuaZip::mdUnzip))
    {
        QString sUnzipDir = sZipPath.left(sZipPath.lastIndexOf(QDir::separator()));
        if(pszDestDir)
            sUnzipDir = QString::fromStdString(pszDestDir);

        QDir dir;
        dir.mkpath(sUnzipDir);

        for(bool bTemp = zip.goToFirstFile(); bTemp; bTemp = zip.goToNextFile())
        {
            QuaZipFile zipfile(&zip);
            if(zipfile.open(QIODevice::ReadOnly))
            {
                QByteArray dataarr = zipfile.readAll();
                zipfile.close();

                QString sTempFilePath = sUnzipDir + QDir::separator();
                //为解决中文乱码（直接getCurrentFileName中文文件名乱码），需要根据zip的格式获取文件名
                //https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT
                /*4.6.9 -Info-ZIP Unicode Path Extra Field (0x7075):

                       Stores the UTF-8 version of the file name field as stored in the
                       local header and central directory header. (Last Revision 20070912)

                         Value         Size        Description
                         -----         ----        -----------
                 (UPath) 0x7075        Short       tag for this extra block type ("up")
                         TSize         Short       total data size for this block
                         Version       1 byte      version of this extra field, currently 1
                         NameCRC32     4 bytes     File Name Field CRC32 Checksum
                         UnicodeName   Variable    UTF-8 version of the entry File Name
                 */
                QuaZipFileInfo info;
                zipfile.getFileInfo(&info);
                QByteArray extra = info.extra;
                int index = extra.lastIndexOf("up");//从附加数据获取up标签
                if(-1 != index)
                {
                    QString strex = QString::fromUtf8(extra.mid(index + 9));//9根据格式计算得出
                    sTempFilePath += strex;
                }
                else
                {
                    sTempFilePath += zip.getCurrentFileName();
                }
                QFile tempfile(sTempFilePath);
                if(tempfile.open(QIODevice::WriteOnly))
                {
                    tempfile.write(dataarr);
                    tempfile.close();
                }
            }
        }

        zip.close();
        bRet = true;
    }

    return bRet;
}

/**************************************************************************
* 函数名: ZipFile
* 功能: 压缩为zip文件
* 参数:
*    @[in ] vecFilePathList: 待压缩文件路径列表
*    @[in ] sZipPath: zip文件路径
* 返回值: 成功返回true，失败返回false
* 时间: 2016/03/10 16:59
* 作者: ZDJ
*/
bool ZipFile(const vector<QString>& vecFilePathList, const QString& sZipPath)
{
    bool bRet = false;

    QuaZip zip(sZipPath);
    if(zip.open(QuaZip::mdCreate))
    {
        for(int i = 0; i < vecFilePathList.size(); i++)
        {
            QString sTempFilePath = vecFilePathList[i];

            QString sFileName = CFileDirHandle::getFileName(sTempFilePath);
            QuaZipNewInfo newinfo(sFileName);

            QuaZipFile zipfile(&zip);
            if(zipfile.open(QIODevice::WriteOnly, newinfo))
            {
                QFile tempfile(sTempFilePath);
                if(tempfile.open(QIODevice::ReadOnly))
                {
                    zipfile.write(tempfile.readAll());
                    tempfile.close();
                }

                zipfile.close();
            }
        }

        zip.close();
        bRet = true;
    }

    return bRet;
}

#endif


/**************************************************************************
* 函数名: LoadConfig
* 功能: 加载配置数据
* 时间: 2016/04/26 17:52
* 作者: ZDJ
*/
void LoadConfig()
{
#if 1    
    CIniConfig::getInstance().getValueActPath(g_sActPath);

    if(g_sActPath.isEmpty())
    {
        g_sActPath = CConfigs::getLocalActLibDir();
        CIniConfig::getInstance().setValueActPath(g_sActPath);
    }

    CIniConfig::getInstance().getValueServer(g_sServer);

    if(g_sServer.isEmpty())
    {
        //"http://services.ubtrobot.com:8080"
        //"http://10.10.1.12:8080"
        g_sServer = "http://services.ubtrobot.com";

        CIniConfig::getInstance().setValueServer(g_sServer);
    }

    g_sUpdateURL = g_sServer + SERVER_DOMAIN_SUFFIX + "/version/verify/";

    g_sDownloadedActInfoXMLPath = g_sActPath + QDir::separator() + "downloadedinfo.xml";
    g_sDownloadingActInfoXMLPath = g_sActPath + QDir::separator() + "downloadinginfo.xml";    

    CIniConfig::getInstance().getValueIsLogEnable(g_bIsRecordLogValue);

#endif

#if 1
    CIniConfig::getInstance().getValueBigFrameDelay(g_nBigFrameDelay);
    CIniConfig::getInstance().getValueSmallFrameDelay(g_nSmallFrameDelay);
#endif
}

#ifdef Q_OS_WIN
/**************************************************************************
* 函数名: delayUsec
* 功能: 微秒级延时
* 参数:
*    @[in ] uDelay: 延时时长，单位微秒
* 返回值: void
* 时间: 2016/06/06
* 作者: ZDJ
*/
void delayUsec(int uDelay)
{
    LARGE_INTEGER litmp;
    LONGLONG QPart1,QPart2;

    double dfMinus,dfFreq,dfTim;

    /*
        Pointer to a variable that the function sets, in counts per second, to the current performance-counter frequency.
        If the installed hardware does not support a high-resolution performance counter,
        the value passed back through this pointer can be zero.

    */
    QueryPerformanceFrequency(&litmp);

    dfFreq = (double)litmp.QuadPart;

    /*
        Pointer to a variable that the function sets, in counts, to the current performance-counter value.
    */
    QueryPerformanceCounter(&litmp);

    QPart1 = litmp.QuadPart;
    do
    {
        QueryPerformanceCounter(&litmp);
        QPart2 = litmp.QuadPart;
        dfMinus = (double)(QPart2-QPart1);
        if(dfMinus < 0)
            break;
        dfTim = dfMinus/dfFreq;
    }while(dfTim<0.000001 * uDelay);
}
#endif


#ifdef USEXML
/**************************************************************************
* 函数名: ReadModelXml
* 功能: 读取3D模型的xml配置文件
* 参数:
*    @[in ] element: dom元素
*    @[out] motorlist: 舵机数据列表
* 返回值: void
* 时间: 2016/01/26 10:15
* 作者: ZDJ
*/
void ReadModelXml(const QDomElement& element, vector<MOTOR_DATA*>& motorlist)
{
    QDomNode node = element.firstChild();
    static MOTOR_DATA* pMotorData = NULL;

    while(!node.isNull())
    {
        QDomElement nodeelem = node.toElement();
        if(nodeelem.tagName() == "Motor")
        {
            pMotorData = new MOTOR_DATA;
            motorlist.push_back(pMotorData);
            ReadModelXml(nodeelem, motorlist);
        }
        else if(nodeelem.tagName() == "ID")
        {
            QDomNode childnode = node.firstChild();
            if(childnode.nodeType() == QDomNode::TextNode)
            {
                if(pMotorData)
                    pMotorData->nID = childnode.toText().data().toInt();
            }
        }
        else if(nodeelem.tagName() == "InitAngle")
        {
            QDomNode childnode = node.firstChild();
            if(childnode.nodeType() == QDomNode::TextNode)
            {
                if(pMotorData)
                    pMotorData->nAngle = childnode.toText().data().toInt();
            }
        }

        node = node.nextSibling();
    }
}

/**************************************************************************
* 函数名: ReadModelXml
* 功能: 读取3D模型的xml配置文件
* 参数:
*    @[in ] sXmlFilePath: xml文件路径
*    @[out] motorlist: 舵机数据列表
* 返回值: void
* 时间: 2016/01/26 10:15
* 作者: ZDJ
*/
void ReadModelXml(const QString& sXmlFilePath, vector<MOTOR_DATA*>& motorlist)
{
    QFile file(sXmlFilePath);
    if(file.open(QIODevice::ReadOnly))
    {
        QDomDocument doc;
        doc.setContent(&file);
        ReadModelXml(doc.documentElement(), motorlist);
        file.close();
    }
}

/**************************************************************************
* 函数名: getMotorCount
* 功能: 获取舵机数量
* 时间: 2016/06/23
* 作者: ZDJ
*/
int getMotorCount()
{
    vector<MOTOR_DATA*> motorlist;
    QString sXmlPath = CConfigs::getLocalRobotPath(MODEL_XML_NAME);
    ReadModelXml(sXmlPath, motorlist);
    int nMotorCnt = motorlist.size();
    CUtil::ReleaseVector(motorlist);
    return nMotorCnt;
}
#endif

/**************************************************************************
* 函数名: containItem
* 功能: 判断是否包含指定动作Item
* 时间: 2016/06/23
* 作者: hwx
*/
bool containItem(ActItemData &item, vector<ActItemData> &vecItems)
{
    bool bContains = false;
    for (int i = 0; i < vecItems.size(); i++)
    {
        if (vecItems.at(i).m_nItemID == item.m_nItemID)
        {
            bContains = true;
            break;
        }
    }

    return bContains;
}
