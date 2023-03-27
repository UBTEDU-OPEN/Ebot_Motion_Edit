#include "mp3player.h"

MP3Player::MP3Player(QObject *parent):QObject(parent)
{
    m_pMediaPlayer = NULL;
    //m_pMediaPlayer = new QMediaPlayer(this);
    m_nStartMilliSecs = 0;
    m_nEndMilliSecs = 0;
    m_bStop = true;
    m_bPause = false;
    m_pMediaPlayer = new QMediaPlayer(this);
    connect(m_pMediaPlayer, &QMediaPlayer::durationChanged, this, &MP3Player::OnDurationChanged);
    connect(m_pMediaPlayer, &QMediaPlayer::positionChanged, this, &MP3Player::OnPositionChanged);
    //hels 设置position等改变时，发送通知的时间间隔，经测试不能设置为1，
    //否则会按默认的1000即1s来通知，具体支持到多小精度，没有一一试过,10ms已经够用
    m_pMediaPlayer->setNotifyInterval(1);
    m_bIsSchedulePlay = false;
}

MP3Player::~MP3Player()
{
    m_bStop = true;
}

void MP3Player::SetFileName(const QString &mp3FileName)
{
    // 解决同一个音乐文件反复加载问题，如果一个音乐文件反复加载，
    // 会导致setPosition无效
    if (0 == m_strFileName.compare(mp3FileName))
    {
        return;
    }
    m_strFileName = mp3FileName;
    m_pMediaPlayer->setMedia(QUrl::fromLocalFile(m_strFileName));
}

void MP3Player::Play(quint64 startMilliSecs, quint64 endMilliSecs)
{
    //for the bug of 4717 【音乐】添加一首音乐-播放音乐，音乐播放完成后点击停止按钮，红色指针没有回到初始位置
    //set restart when at end position
    if(startMilliSecs == endMilliSecs)
    {
        startMilliSecs = 0;
    }

    m_nStartMilliSecs = startMilliSecs;
    m_nEndMilliSecs = endMilliSecs;
//    m_pMediaPlayer->play();
//    m_pMediaPlayer->setPosition(startMilliSecs);

    m_pMediaPlayer->setPosition(startMilliSecs);
    m_pMediaPlayer->play();

    m_bIsSchedulePlay = true;
    m_bPause = false;
    m_bStop = false;
}

bool MP3Player::IsPlaying()
{
    if(m_bStop || m_bPause)
    {
        return false;
    }
    return true;
}

void MP3Player::Pause()
{
    if(m_pMediaPlayer)
    {
        m_pMediaPlayer->pause();
    }
    m_bPause = true;
}

void MP3Player::Resume()
{
    if(m_pMediaPlayer)
    {
        m_pMediaPlayer->play();
    }
    m_bPause = false;
    m_bStop = false;
}

void MP3Player::Stop()
{
    if(!m_bStop && m_pMediaPlayer)
    {
        m_pMediaPlayer->stop();
    }
    m_bStop = true;
    m_bPause = false;
}

void MP3Player::OnDurationChanged(qint64 duration)
{
    m_nTotalDuration = duration;
}

void MP3Player::OnPositionChanged(qint64 position)
{

    if(m_nStartMilliSecs > m_nEndMilliSecs)
    {
        m_bStop = true;
        m_pMediaPlayer->stop();
        emit SigFinishPlaying();
        return;
    }

    if(NULL == m_pMediaPlayer)
        return;
    if(m_pMediaPlayer->state() != QMediaPlayer::PlayingState)
        return;
    //已经试过多种方法，QMediaplay的异步播放方式，导致setPosition后还是从0开始，
    //因此这里先使用这种方式，监测到这种情况时，重新设置一次
    if(m_bIsSchedulePlay && position < m_nStartMilliSecs)
    {
        m_pMediaPlayer->pause();
        m_pMediaPlayer->setPosition(m_nStartMilliSecs);
        m_pMediaPlayer->play();
        return;
    }
    m_bIsSchedulePlay = false;
    emit SigPositionChanged(position);
    if(position >= m_nEndMilliSecs)
    {
        if(!m_bStop && m_pMediaPlayer)
        {
            m_bStop = true;
            m_pMediaPlayer->stop();
            emit SigFinishPlaying();
        }
    }
}

//void MP3Player::OnSeekableChanged(bool seekable)
//{
//    if(m_bIsSchedulePlay)
//    {
//        m_pMediaPlayer->setPosition(m_nStartMilliSecs);
//        m_bIsSchedulePlay = false;
//    }
//}

//void MP3Player::OnStateChanged(QMediaPlayer::State state)
//{
//    qDebug()<<state;
//    if(QMediaPlayer::StoppedState == state)
//    {
//        if(!m_bStop && m_pMediaPlayer)
//        {
//            m_bStop = true;
//            m_pMediaPlayer->disconnect();
//            delete m_pMediaPlayer;
//            m_pMediaPlayer = NULL;

//            emit SigFinishPlaying();
//        }
//    }
//}

