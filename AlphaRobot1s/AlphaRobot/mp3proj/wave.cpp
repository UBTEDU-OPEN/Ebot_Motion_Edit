/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：wave.cpp
* 创建时间：2015/11/03 10:15
* 文件标识：
* 文件摘要：波形数据类
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/11/03 10:15
* 版本摘要：
*/
#include "wave.h"
#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QDebug>
#include <sndfile.h>
#include <UBXPublic.h>

#include "alpharobotlog.h"
#include "../zxLib/configs.h"
#include "mp3projconstants.h"
#include "MiniBpm.h"

Wave::Wave(QObject *parent):QObject(parent)
{
    m_pBuffer = NULL;
    m_nDecodedLen = 0;
    m_nErr = MPG123_OK;
    m_nSimpleRate = 0;
    m_nChannels = 0;
    m_nEnc = 0;
    m_hMpg123 = NULL;
    m_nBitPerSimple = 0;
    m_nBitRate = 0;
    m_nBpm = 0;
    m_nMP3Duration = 0;

    mpg123_init();
    m_hMpg123 = mpg123_new(NULL, &m_nErr);
    m_strMP3Title.clear();
}

Wave::~Wave()
{
    mpg123_delete(m_hMpg123);
    mpg123_exit();

    SAFE_FREE(m_pBuffer);

    m_vecSimplePoints.clear();
}

/**************************************************************************
* 函数名: LoadData
* 功能: 加载数据
* 参数:
*    @[in ] pszFilePath: mp3音乐文件路径
* 返回值: 成功返回true，失败返回false
* 时间: 2015/12/08 16:16
* 作者: ZDJ
*/
bool Wave::LoadData(const char* pszFilePath)
{
    bool bCheck = false;
    m_strWavPath.clear();
    do
    {
        SAFE_FREE(m_pBuffer);
        m_nDecodedLen = 0; //实际解码的长度

        int nErr = mpg123_open(m_hMpg123, pszFilePath);
        if(nErr != MPG123_OK)
            break;
        nErr = mpg123_getformat(m_hMpg123, &m_nSimpleRate, &m_nChannels, &m_nEnc);
        if(nErr != MPG123_OK)
            break;

        if(m_nEnc == MPG123_ENC_SIGNED_16)
            m_nBitPerSimple = 16;
        else if(m_nEnc == MPG123_ENC_FLOAT_32)
            m_nBitPerSimple = 32;
        else
            break;

        //save wave
        QFileInfo fileInfo(pszFilePath);

        m_strWavPath = CConfigs::getLocalWaveDir() + QDir::separator();

        m_strWavPath += fileInfo.baseName() + ".wav";
        m_strWavPath = QDir::toNativeSeparators(m_strWavPath);
        SNDFILE* sndfile = NULL;
        SF_INFO sfinfo;
        memset(&sfinfo, 0, sizeof(sfinfo));
        sfinfo.samplerate = m_nSimpleRate;
        sfinfo.channels = m_nChannels;
        sfinfo.format = SF_FORMAT_WAV|(m_nEnc == MPG123_ENC_SIGNED_16 ? SF_FORMAT_PCM_16 : SF_FORMAT_FLOAT);

       //sndfile = sf_open(m_strWavPath.toUtf8().data(), SFM_WRITE, &sfinfo);
#ifdef Q_OS_MAC
       sndfile = sf_open(m_strWavPath.toUtf8().data(), SFM_WRITE, &sfinfo);
#elif defined(Q_OS_WIN)
        char *pWaveFile = NULL;
        Utf8ToAnsi(m_strWavPath.toStdString().c_str(), pWaveFile);
        sndfile = sf_open(pWaveFile, SFM_WRITE, &sfinfo);
#endif

        // add by huangwuxian  2016.06.29 禅道955
        off_t totalSample = mpg123_length(m_hMpg123);
        int nMaxValue = totalSample * m_nBitPerSimple * m_nChannels/8;  // 算法参考：http://blog.csdn.net/u012141553/article/details/13016593

        // 反复申请内存时，进度很慢，需要进度条
        // 目前已经修改为一次申请内存，暂时不需要进度条
//        QProgressDialog progressDialog(tr("Loading music file ") + fileInfo.absoluteFilePath(), tr("Abort"), 0, nMaxValue, (QWidget*)parent());
//        progressDialog.setMinimumDuration(1000);  // 加载后就会显示
//        progressDialog.setModal(true);
//        progressDialog.setAutoClose(true);  // 加载完成后自动关闭

        // modify by huangwuxian 2016.7.1 禅道959
        // 解决导入音乐文件慢的问题，由反复申请内存调整为一次性申请内存
        //        size_t nBlockLen = mpg123_outblock(m_hMpg123);
        //        byte* pBlockBuf = (byte*)malloc(nBlockLen);//块缓冲区
        // 使用该方式会比原来方式在.wav末尾多一些无效数据
        size_t nBlockLen = nMaxValue;
        byte* pBlockBuf = (byte*)malloc(nBlockLen);//块缓冲区
        while(1)
        {
            memset(pBlockBuf, 0, nBlockLen);

//            if (progressDialog.wasCanceled())
//            {
//                SAFE_FREE(pBlockBuf);
//                sf_close(sndfile);
//                mpg123_close(m_hMpg123);
//                return false;
//            }

//            // 设置进度条的值
//            progressDialog.setValue(m_nDecodedLen);

            if(!m_pBuffer)
            {
                m_pBuffer = (byte*)malloc(nBlockLen);
            }
            else
            {
                m_pBuffer = (byte*)realloc(m_pBuffer, m_nDecodedLen + nBlockLen);
            }

            memset(m_pBuffer + m_nDecodedLen, 0, nBlockLen);

            size_t nDecTemp = 0;
            nErr = mpg123_read(m_hMpg123, pBlockBuf, nBlockLen, &nDecTemp);
            if((nErr != MPG123_OK && nErr != MPG123_DONE && nErr != MPG123_NEED_MORE) || nDecTemp == 0) // 由于音乐文件可能会被剪辑过，因此MPG123_NEED_MORE也认为是ok的
                break;


            m_nEnc == MPG123_ENC_SIGNED_16
                        ? sf_write_short(sndfile, (short*)pBlockBuf, nDecTemp/sizeof(short))
                        : sf_write_float(sndfile, (float*)pBlockBuf, nDecTemp/sizeof(float));

            memcpy(m_pBuffer + m_nDecodedLen, pBlockBuf, nDecTemp);

            m_nDecodedLen += nDecTemp;
        }

        SAFE_FREE(pBlockBuf);
        sf_close(sndfile);

#ifdef Q_OS_WIN
        SAFE_FREE(pWaveFile);
#endif

        if(m_nChannels != 0)
        {
            int bytes = (m_nBitPerSimple + 7) /8;
            m_nSamples = m_nDecodedLen / (m_nChannels * bytes);
        }

        //获取比特率
        mpg123_frameinfo mpgInfo;
        if(mpg123_info(m_hMpg123, &mpgInfo) == MPG123_OK)
        {
            if(mpgInfo.layer == 3)//The MPEG Audio Layer (MP1/MP2/MP3)
            {
                if(mpgInfo.vbr == MPG123_CBR)
                {
                    m_nBitRate = mpgInfo.bitrate;
                }
                else if(mpgInfo.vbr == MPG123_VBR)
                {
                    mpg123_seek(m_hMpg123, 0, SEEK_SET);
                    m_nBitRate = CalculaBitRate();
                    mpg123_seek(m_hMpg123, 0, SEEK_SET);
                }
            }
        }
        if(m_nBitRate > 128)
        {
            mpg123_close(m_hMpg123);
            return false;
        }

#if 0
        Wave waveobj;
        byte* pOutData = NULL;
        int nOutLen = 0;
        waveobj.CreateWaveFileFromData(m_pBuffer, m_nDecodedLen, pOutData, nOutLen);

        FILE* fp = fopen("/users/zdj/downloads/testout.wav", "wb");
        if(fp)
        {
            fwrite(pOutData, nOutLen, 1, fp);
            fclose(fp);
        }

        SAFE_FREE(pOutData);
#endif
        bCheck = true;
    }while(0);

#if 1
    m_vecSimplePoints.clear();

    byte* pTempData = m_pBuffer;
    const int nSimpleStep = Constants::MP3_SAMPLE_STEP;//!!!must have to bigger than 2 , or m_vecSimplePoints cannot load so much data.
    const int nPointerStep = sizeof(WAVE_CHUNK_16_STEREO)*nSimpleStep;
    for(size_t i = 0; i < m_nDecodedLen; i += nPointerStep)
    {
        WAVE_CHUNK_16_STEREO* pWaveData = (WAVE_CHUNK_16_STEREO*)pTempData;
        short nTemp1 = (pWaveData->bytelefthigh << 8) | pWaveData->byteleftlow;
        short nTemp2 = (pWaveData->byterighthigh << 8) | pWaveData->byterightlow;
        double fTemp = (nTemp1 + nTemp2) * 0.5;
        double fTimePos = 1.0 / m_nSimpleRate * (i / sizeof(WAVE_CHUNK_16_STEREO)); //时间点，单位秒       

        QPointF posSimple(fTimePos, fTemp);
        m_vecSimplePoints.push_back(posSimple);

        pTempData += nPointerStep;
    }
#endif

#if 0

    mpg123_id3v1* id3v1;
    mpg123_id3v2* id3v2;
    mpg123_scan(m_hMpg123);
    int meta = mpg123_meta_check(m_hMpg123);
    if(meta & MPG123_ID3)
    {
        if(MPG123_OK == mpg123_id3(m_hMpg123, &id3v1, &id3v2))
        {
            if(id3v1 != NULL)
            {
                m_strMP3Title = AnsiToUnicode(id3v1->title);
            }
            else if(id3v2 != NULL)
            {
                if(id3v2->title != NULL)
                {
                    m_strMP3Title = AnsiToUnicode(id3v2->title->p);
                }
            }
        }
    }
#endif

    //if(m_strMP3Title.isEmpty())
    {
        QFileInfo fileInfo(pszFilePath);
        m_strMP3Title = fileInfo.fileName();
    }

    mpg123_close(m_hMpg123);

    pickMP3BMP(m_strWavPath);

    return bCheck;
}

/**************************************************************************
* 函数名: ReleaseData
* 功能: 清除数据
* 时间: 2015/12/08 16:16
* 作者: ZDJ
*/
void Wave::ReleaseData()
{
    SAFE_FREE(m_pBuffer);
    m_vecSimplePoints.clear();
}

/**************************************************************************
* 函数名: CreateWaveFileFromData
* 功能: 从波形数据生成波形文件数据
* 参数:
*    @[in ] pWavData: 波形数据
*    @[in ] nDataLen: 波形数据长度
*    @[out] pFileData: 波形文件数据
*    @[out] nFileLen: 波形文件长度
* 返回值: 成功返回true，失败返回false
* 时间: 2015/12/08 16:16
* 作者: ZDJ
*/
bool Wave::CreateWaveFileFromData(byte* pWavData, int nDataLen, byte*& pFileData, int& nFileLen)
{
    RIFF_CHUNK riffchunk;
    FMT_CHUNK fmtchunk;

    int nDataChunkLen = nDataLen + 8;
    DATA_CHUNK* pDataChunk = (DATA_CHUNK*)malloc(nDataChunkLen);

    QString sTemp = "data";
    const char* ptemp = sTemp.toStdString().c_str();

    memcpy(pDataChunk->data_id, ptemp, 4);
    pDataChunk->data_rearlen = nDataLen;
    memcpy(pDataChunk->data, pWavData, nDataLen);

    int nriffsize = sizeof(riffchunk);
    int nfmtsize = sizeof(fmtchunk);

    riffchunk.riff_rearlen = nfmtsize + nDataChunkLen + sizeof(riffchunk.waveid);

    nFileLen = riffchunk.riff_rearlen + 8;
    pFileData = (byte*)malloc(nFileLen);
    byte* pDataCursor = pFileData;

    memcpy(pDataCursor, &riffchunk, nriffsize);
    pDataCursor += nriffsize;

    memcpy(pDataCursor, &fmtchunk, nfmtsize);
    pDataCursor += nfmtsize;

    memcpy(pDataCursor, pDataChunk, nDataChunkLen);

    free(pDataChunk);
    pDataChunk = NULL;

    return true;
}

/**************************************************************************
* 函数名: SaveToFile
* 功能: 保存波形文件数据
* 参数:
*    @[in ] pszFilePath: 文件路径
*    @[in ] pFileData: 波形文件数据
*    @[in ] nFileLen: 波形文件长度
* 返回值: 成功返回true，失败返回false
* 时间: 2015/12/08 16:16
* 作者: ZDJ
*/
bool Wave::SaveToFile(const char* pszFilePath, byte* pFileData, int nFileLen)
{
    FILE* fp = fopen(pszFilePath, "wb");
    if(fp)
    {
        fwrite(pFileData, nFileLen, 1, fp);
        fclose(fp);
        return true;
    }
    return false;
}

/**************************************************************************
* 函数名: GetSimpleRate
* 功能: 获取采样率
* 返回值: 采样率
* 时间: 2015/12/21
* 作者: ZDJ
*/
int Wave::GetSimpleRate()
{
    return m_nSimpleRate;
}

/**************************************************************************
* 函数名: GetChannels
* 功能: 获取声道数
* 返回值: 声道数
* 时间: 2015/12/21
* 作者: ZDJ
*/
int Wave::GetChannels()
{
    return m_nChannels;
}

/**************************************************************************
* 函数名: GetBitPerSimple
* 功能: 获取采样位数
* 返回值: 采样位数
* 时间: 2015/12/21
* 作者: ZDJ
*/
int Wave::GetBitPerSimple()
{
    return m_nBitPerSimple;
}

/**************************************************************************
* 函数名: GetAvgByteRate
* 功能: 获取平均字节率，单位字节/s
* 返回值: 平均字节率
* 时间: 2015/12/21
* 作者: ZDJ
*/
int Wave::GetAvgByteRate()
{
    return m_nSimpleRate * m_nBitPerSimple * m_nChannels / 8;
}

/**************************************************************************
* 函数名: GetTime
* 功能: 获取时长，单位s
* 返回值: 时长
* 时间: 2015/12/21
* 作者: ZDJ
*/
int Wave::GetTime()
{
    int nAvgByteRate = GetAvgByteRate();
    if(nAvgByteRate > 0)
        return m_nDecodedLen / nAvgByteRate;
    else
        return 0;
}

/**************************************************************************
* 函数名: GetSimplePoints
* 功能: 获取采样点信息列表
* 返回值: 采样点信息列表
* 时间: 2015/12/21
* 作者: ZDJ
*/
QVector<QPointF>& Wave::GetSimplePoints()
{
    return m_vecSimplePoints;
}

/************************************
* 名称: GetSamples
* 功能: 获取采样数
* 返回:   void
* 时间:   2016/01/10
* 作者:   hels
************************************/
unsigned long Wave::GetSamples()
{
    return m_nSamples;
}

int Wave::GetBitRate()
{
    return m_nBitRate;
}

QString Wave::GetMusicTitle()
{
    return m_strMP3Title;
}

int Wave::CalculaBitRate()
{
    int ret;
    quint64 count = 0;
    quint64 rateSum = 0;
    while( (ret = mpg123_framebyframe_next(m_hMpg123)) == MPG123_OK || ret == MPG123_NEW_FORMAT )
    {
        mpg123_frameinfo mpgInfo;
        if(mpg123_info(m_hMpg123, &mpgInfo) == MPG123_OK)
        {
            rateSum += mpgInfo.bitrate;
            count++;
        }
    }

    if(count != 0)
        return rateSum = rateSum/count;
    else
        return 0;
}

QString Wave::GetWavePath()
{
    return m_strWavPath;
}

bool Wave::pickMP3BMP(const QString &strAudioFile)
{
    float *pWaveDataSamples = new float[m_vecSimplePoints.size()];
    for (int i = 0; i < m_vecSimplePoints.size(); i++)
    {
        pWaveDataSamples[i] = m_vecSimplePoints[i].y();
    }

    m_nMP3Duration = m_vecSimplePoints[m_vecSimplePoints.size() - 1].x();
//    qDebug() << "duration = " << m_nMP3Duration << endl;

    breakfastquay::MiniBPM bpmDetector(m_nSimpleRate/(Constants::MP3_SAMPLE_STEP));
    m_nBpm = qRound64(bpmDetector.estimateTempoOfSamples(pWaveDataSamples, m_vecSimplePoints.size()));

    SAFE_DELETE_ARRAY(pWaveDataSamples);
    //qDebug() << "Pick up MP3 bpm = " << m_dBpm << endl;

#if 0
    SNDFILE* sndfile = NULL;
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));

    //QString strFile = strAudioFile;
    QString strFile = "C:/Users/ubt/Desktop/music/7674.wav";

   //sndfile = sf_open(m_strWavPath.toUtf8().data(), SFM_WRITE, &sfinfo);
#ifdef Q_OS_MAC
   sndfile = sf_open(strFile.toUtf8().data(), SFM_READ, &sfinfo);
#elif defined(Q_OS_WIN)
    char *pAudioFile = NULL;
    Utf8ToAnsi(strFile.toStdString().c_str(), pAudioFile);
    sndfile = sf_open(pAudioFile, SFM_READ, &sfinfo);
#endif

    if (!sndfile)
    {
        AlphaRobotLog::getInstance()->printError("pickBMP: Failed to open audio file to read");
        return false;
    }

    SF_LOOP_INFO sLoopInfo;
    memset(&sLoopInfo, 0, sizeof(sLoopInfo));

    ///////////////////////// sndf目前返回的sLoopInfo无效//////////////////////
    if (sf_command(sndfile, SFC_GET_LOOP_INFO, &sLoopInfo, sizeof(sLoopInfo)) == SF_FALSE)
    {
        const char *pErrMsg = sf_strerror(sndfile);
        char *pErrMsgUTF8 = NULL;
        AnsiToUtf8(pErrMsg, pErrMsgUTF8);
        QString strErrMsg(pErrMsgUTF8);

        AlphaRobotLog::getInstance()->printError("pickBMP: failed to run command SFC_GET_LOOP_INFO");
        return false;
    }

    float nBPM = sLoopInfo.bpm;
#endif
    return  true;
}
