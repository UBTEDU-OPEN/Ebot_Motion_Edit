#include "wavelayer.h"
#include "mp3widget.h"

#include "alpharobotlog.h"

WaveLayer::WaveLayer(MP3Widget* mp3Widget)
    : m_pMp3Widget(mp3Widget)
{
    bMusicLoaded = false;

    // Add by huangwuxian 2016.03.30
    // 传递mp3Widget用于设置Wave类弹出的QProgressDialog qss设置
    m_wavedata.setParent(mp3Widget);
}

int WaveLayer::GetTime()
{
    return m_wavedata.GetTime();
}

QString WaveLayer::GetMusicTitle()
{
    return m_wavedata.GetMusicTitle();
}

int WaveLayer::GetSimpleRate()
{
    return m_wavedata.GetSimpleRate();
}

void WaveLayer::DrawWavePixmap(QPixmap *pixmap)
{
    if(NULL == pixmap)
    {
        return;
    }
    DrawWave(pixmap);
}

bool WaveLayer::IsLoaded()
{
    return bMusicLoaded;
}

QString WaveLayer::GetWavPath()
{
    return m_wavedata.GetWavePath();
}

/************************************
* 名称: OnLoadMusic
* 功能: 加载音乐数据
* 参数: [in]const QString& strFilePath音乐文件路径
* 返回:   void
* 时间:   2016/01/11
* 作者:   hels
************************************/
void WaveLayer::OnLoadMusic(const QString& strFilePath)
{
    bool ret = m_wavedata.LoadData(strFilePath.toStdString().c_str());
    bMusicLoaded = ret;

    MP3Info stMP3Info;
    stMP3Info.m_strMP3FilePath = m_pMp3Widget->getMP3File();
    stMP3Info.m_nDuration = m_wavedata.getDuration();
    stMP3Info.m_nBPM = m_wavedata.getBPM();

    emit SigMusicLoaded(ret, stMP3Info, m_wavedata.GetBitRate());
}

/************************************
* 名称: OnReleaseMusic
* 功能: 卸载音乐数据
* 参数: [in]const QString& strFilePath音乐文件路径
* 返回:   void
* 时间:   2016/01/11
* 作者:   hels
************************************/
void WaveLayer::OnReleaseMusic()
{
    m_wavedata.ReleaseData();
    bMusicLoaded = false;
    emit SigMusicRelease(true);
}

/************************************
* 名称: DrawWave
* 功能: 绘制波形
* 参数: [in]QPixmap* pixmap 位图对象指针
* 返回:   void
* 时间:   2016/01/11
* 作者:   hels
************************************/
void WaveLayer::DrawWave(QPixmap *pixmap)
{
    if(NULL == pixmap)
    {
        return;
    }
//    //填充背景色
//    DrawWaveBackGround(pixmap);

    QVector<QPointF>& points = m_wavedata.GetSimplePoints();
    int size = points.size();
    if(points.size() <= 0)
        return;

    int width = m_pMp3Widget->GetContentWidth();
    QPainter painter(pixmap);
    QPen penLine(Constants::COLOR_WAVE_LINE, Constants::DEFULAT_PLOT_THICK);
    QPen penBaseLine(Constants::COLOR_WAVE_BASE_LINE, 0);
    painter.setPen(penBaseLine);
    //绘制基线
    painter.drawLine(0, pixmap->height() / 2, width, pixmap->height() / 2);

    //绘制波形 1格=100pixel
    //m_wavedata中是以s为单位存储波形点，因此需要根据TimeLevel进行换算
    painter.setPen(penLine);
    quint64 dStartIndex = m_pMp3Widget->GetDisplayStartTime();
    quint64 dEndIndex = m_pMp3Widget->GetDisplayEndTime();
    int nDrawInterval = m_pMp3Widget->GetDrawInterval();
    double xScale = m_pMp3Widget->GetXScale();
    QPointF ptPrev;
    bool bIsFirstPt = true;
    double nPixVerScale = pixmap->height() * 1.0 / 65536;

    for(int i = 0; i < size;)
    {
        //m_wavedata中是以s为单位存储波形点,转换为ms
        double x = points.at(i).x() * 1000.0;

        //范围在dStartIndex ~ dEndIndex之间则绘制
        if(x - dStartIndex > 0.0000000001)
        {
            if(x - dEndIndex > 0.0000000001)
                break;
            QPointF ptCur = points.at(i);
            ptCur.setY(nPixVerScale * ptCur.y() + pixmap->height() / 2);
            ptCur.setX((x - dStartIndex) * xScale);
            if(bIsFirstPt)
            {
                bIsFirstPt = false;
                ptPrev =ptCur;
            }
            else
            {
                if(qAbs(ptCur.x() - ptPrev.x()) >= 1 ||
                   qAbs(ptCur.y() - ptPrev.y()) >= 1)
                {
                    painter.drawLine(ptCur, ptPrev);
                    ptPrev = ptCur;
                }
            } 
        }

        i += nDrawInterval;
    }
}

/************************************
* 名称: DrawWaveBackGround
* 功能: 绘制波形背景
* 参数: [in]QPixmap* pixmap 位图对象指针
* 返回:   void
* 时间:   2016/01/11
* 作者:   hels
************************************/
void WaveLayer::DrawWaveBackGround(QPixmap *pixmap)
{
    if(NULL == pixmap)
    {
        return;
    }

    QBrush brBack(Constants::COLOR_WAVE_BACK);
    QPainter painter(pixmap);
    painter.fillRect(0, 0, pixmap->width(), pixmap->height(), brBack);
}
