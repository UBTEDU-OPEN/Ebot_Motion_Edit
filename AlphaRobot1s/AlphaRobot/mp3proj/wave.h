/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：wave.h
* 创建时间：2015/11/03 10:15
* 文件标识：
* 文件摘要：波形数据类
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/11/03 10:15
* 版本摘要：
*/
#ifndef WAVE_H
#define WAVE_H

#include <memory>
#include <string>
#include <QVector>
#include <QString>
#include <QProgressDialog>
using namespace std;

#include "myglobal.h"
#include "mpg123.h"
#include "UBXWaveAnalyze.h"

#include <QPoint>
#include <QPointF>

//波形RIFF块结构
struct RIFF_CHUNK
{
    byte  riff_id[4];
    int   riff_rearlen;
    byte  waveid[4];

    RIFF_CHUNK()
    {
        QString sTemp = "RIFF";
        const char* ptemp = sTemp.toStdString().c_str();
        memcpy(riff_id, ptemp, 4);

        riff_rearlen = 0;

        sTemp = "WAVE";
        ptemp = sTemp.toStdString().c_str();
        memcpy(waveid, ptemp, 4);

    }
};

//波形FMT块结构
struct FMT_CHUNK
{
    byte  fmt_id[4];
    int   fmt_rearlen;

    short fmttag;
    short channels;
    int   samplerate;
    int   avgbytesrate;
    short blockalign;
    short bitpersample;
    //byte  reservedata[0];//扩展域大小，扩展域数据

    FMT_CHUNK()
    {
        QString sTemp = "fmt ";
        const char* ptemp = sTemp.toStdString().c_str();
        memcpy(fmt_id, ptemp, 4);

        fmt_rearlen = 16;

        fmttag = 1; //PCM标记
        channels = 2; //声道数
        samplerate = 44100;        
        bitpersample = 16;

        avgbytesrate = samplerate * bitpersample * channels / 8; //平均字节率
        blockalign = bitpersample * channels / 8;
    }
};

//波形数据块结构
struct DATA_CHUNK
{
    byte data_id[4];
    int  data_rearlen;
    byte data[0];
};

//波形存储结构
struct WAVE_CHUNK_16_STEREO
{
    byte byteleftlow;
    byte bytelefthigh;
    byte byterightlow;
    byte byterighthigh;
};

//波形类
class Wave : public QObject
{
        Q_OBJECT

public:
    Wave(QObject *parent = NULL);
    ~Wave();

    /**************************************************************************
    * 函数名: LoadData
    * 功能: 加载数据
    * 参数:
    *    @[in ] pszFilePath: mp3音乐文件路径
    * 返回值: 成功返回true，失败返回false
    * 时间: 2015/12/08 16:16
    * 作者: ZDJ
    */
    bool LoadData(const char* pszFilePath);

    /**************************************************************************
    * 函数名: ReleaseData
    * 功能: 清除数据
    * 时间: 2015/12/08 16:16
    * 作者: ZDJ
    */
    void ReleaseData();

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
    bool CreateWaveFileFromData(byte* pWavData, int nDataLen, byte*& pFileData, int& nFileLen);

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
    bool SaveToFile(const char* pszFilePath, byte* pFileData, int nFileLen);

    /**************************************************************************
    * 函数名: GetSimpleRate
    * 功能: 获取采样率
    * 返回值: 采样率
    * 时间: 2015/12/21
    * 作者: ZDJ
    */
    int GetSimpleRate();

    /**************************************************************************
    * 函数名: GetChannels
    * 功能: 获取声道数
    * 返回值: 声道数
    * 时间: 2015/12/21
    * 作者: ZDJ
    */
    int GetChannels();

    /**************************************************************************
    * 函数名: GetBitPerSimple
    * 功能: 获取采样位数
    * 返回值: 采样位数
    * 时间: 2015/12/21
    * 作者: ZDJ
    */
    int GetBitPerSimple();

    /**************************************************************************
    * 函数名: GetAvgByteRate
    * 功能: 获取平均字节率，单位字节/s
    * 返回值: 平均字节率
    * 时间: 2015/12/21
    * 作者: ZDJ
    */
    int GetAvgByteRate();

    /**************************************************************************
    * 函数名: GetTime
    * 功能: 获取时长，单位s
    * 返回值: 时长
    * 时间: 2015/12/21
    * 作者: ZDJ
    */
    int GetTime();

    /**************************************************************************
    * 函数名: GetSimplePoints
    * 功能: 获取采样点信息列表
    * 返回值: 采样点信息列表
    * 时间: 2015/12/21
    * 作者: ZDJ
    */
    QVector<QPointF>& GetSimplePoints();

    /************************************
    * 名称: GetSamples
    * 功能: 获取采样数
    * 返回:   void
    * 时间:   2016/01/10
    * 作者:   hels
    ************************************/
    unsigned long GetSamples();

    /************************************
    * 名称: GetBitRate
    * 功能: 获取比特率
    * 返回:   void
    * 时间:   2016/01/11
    * 作者:   hels
    ************************************/
    int GetBitRate();

    /************************************
    * 名称: GetMusicTitle
    * 功能: 获取音乐名称，若音乐文件没有写入ID3信息，则返回文件名（不带后缀）
    * 返回:   QString
    * 时间:   2016/01/27
    * 作者:   hels
    ************************************/
    QString GetMusicTitle();

    int CalculaBitRate();
    QString GetWavePath();

    int getDuration() { return m_nMP3Duration; }
    int getBPM() { return m_nBpm; }

private:
    // 提取音频文件BMP
    bool pickMP3BMP(const QString &strAudioFile);

private:
    byte* m_pBuffer;        //波形数据缓冲区
    size_t m_nDecodedLen;  //已解码的数据长度
    int m_nErr;
    long m_nSimpleRate;    //采样率
    int m_nChannels;       //声道数
    int m_nEnc;           //编码类型
    int m_nBitPerSimple;   //采样位数
    unsigned long m_nSamples;//样本采样总数
    int m_nBitRate;//比特率

    int m_nBpm;  // 音乐节拍，Beat per Minute
    int m_nMP3Duration; // 音乐时长, 单位是s

    mpg123_handle* m_hMpg123;
    QString m_strMP3Title;
    QString m_strWavPath;

    QVector<QPointF> m_vecSimplePoints;    
};

#endif // WAVE_H
