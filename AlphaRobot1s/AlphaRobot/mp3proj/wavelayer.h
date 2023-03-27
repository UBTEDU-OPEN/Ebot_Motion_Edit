/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：wavelayer.h
* 创建时间：2016/01/11
* 文件标识：
* 文件摘要：波形绘图类
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/01/11
* 版本摘要：
*/


#ifndef WAVELAYER_H
#define WAVELAYER_H
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include "wave.h"
#include "mp3projconstants.h"

using namespace Constants;

class MP3Widget;

class WaveLayer : public QObject
{
    Q_OBJECT
public:
    WaveLayer(MP3Widget* mp3Widget);

public:
    //hels 获取MP3总时间
    int GetTime();
    QString GetMusicTitle();
    //hels 获取MP3采样率
    int GetSimpleRate();
    //hels 将波形图贴到给定位图中
    void DrawWavePixmap(QPixmap* pixmap);
    //hels 音乐是否已经加载
    bool IsLoaded();
    QString GetWavPath();

    /************************************
    * 名称: DrawWaveBackGround
    * 功能: 绘制波形背景
    * 参数: [in]QPixmap* pixmap 位图对象指针
    * 返回:   void
    * 时间:   2016/01/11
    * 作者:   hels
    ************************************/
    void DrawWaveBackGround(QPixmap* pixmap);

signals:
    //加载完成时发出
    void SigMusicLoaded(bool sucessed, MP3Info stMp3Info, int bitRate);
    //卸载完成时发出
    void SigMusicRelease(bool sucessed);

public slots:
    /************************************
    * 名称: OnLoadMusic
    * 功能: 加载音乐数据
    * 参数: [in]const QString& strFilePath 音乐文件路径
    * 返回:   void
    * 时间:   2016/01/11
    * 作者:   hels
    ************************************/
    void OnLoadMusic(const QString& strFilePath);

    //响应删除波形
    void OnReleaseMusic();
protected:
    /************************************
    * 名称: DrawWave
    * 功能: 绘制波形
    * 参数: [in]QPixmap* pixmap 位图对象指针
    * 返回:   void
    * 时间:   2016/01/11
    * 作者:   hels
    ************************************/
    void DrawWave(QPixmap* pixmap);

private:
    MP3Widget* m_pMp3Widget; //波形窗体对象
    Wave  m_wavedata; //mp3解码类
    bool bMusicLoaded;//记录音乐是否已经加载
};

#endif // WAVELAYER_H
