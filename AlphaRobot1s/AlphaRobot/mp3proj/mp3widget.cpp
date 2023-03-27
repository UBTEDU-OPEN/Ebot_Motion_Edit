/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：MP3Widget.cpp
* 创建时间：2015/11/03 10:15
* 文件标识：
* 文件摘要：波形显示控件
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/11/03 10:15
* 版本摘要：
*/
#include "mp3widget.h"
#include "ui_mp3widget.h"
#include <QPen>
#include <QBrush>
#include <QScrollBar>
#include <QMenu>
#include <QDir>
#include <QHelpEvent>
#include <QToolTip>

//#include "LayerUI/ILayerUI.h"

#include "alpharobotlog.h"
#include "../zxLib/configs.h"
#include "../zxLib/widget/messagebox.h"

MP3Widget::MP3Widget(QWidget *parent)
    : QWidget(parent)
    , m_pMP3WidgetUI(new Ui::MP3Widget)
    , m_waveLayer(this)
    , m_actionLayer(this)
{    
    m_pMP3WidgetUI->setupUi(this);

    m_pPixmapMem = NULL;
    m_pPixmapGroup = NULL;
    m_pPixmapWave = NULL;
    m_pPixmapBackGround = NULL;
    m_pActionDataManager = NULL;
    m_pHandleMarkLine = NULL;
    m_nCurrentGroupID = -1;
    m_nCurrentFrameID = -1;
    m_pEvent = NULL;

    m_nCellCount = Constants::DEFAULT_CELL_COUNT;
    m_nCellWidth = Constants::DEFAULT_CELL_WIDTH;
    m_nLineThick = Constants::DEFAULT_LINE_THICK;
    m_nCellHeight = Constants::DEFAULT_CELL_HEIGHT;
    m_nCellTotalSeconds = Constants::DEFAULT_CELL_COUNT;//初始cellcount以秒为单位

    m_nTimeCellWidth = Constants::DEFAULT_CELL_WIDTH;
    m_nTimeCellHeight = Constants::DEFAULT_CELL_TIME_HEIGHT;
    m_nMusicCellWidth = Constants::DEFAULT_CELL_WIDTH;
    m_nMusicCellHeight = Constants::DEFAULT_CELL_MUSIC_HEIGHT;

    m_nStartX = 0;
    m_nStartY = 0;
    m_nCellStartIndex = 0;

    m_nCurSelX = 100;
    m_nSelLineLength = 500;

    m_nPixelPerSec = m_nCellWidth + m_nLineThick;
    m_timeLevel = ss_1;
    m_nStartMilliSecs = 0;
    m_nEndMilliSecs = 0;
    m_pMP3Player = NULL;
    //m_pMP3Thread = NULL;

    m_timeScale = 10;

    m_bForwards = false;
    m_bIsLeftMousePressed = false;
    m_nWheelCount = 0;

    m_oMousePressedPos.setX(0);
    m_oMousePressedPos.setY(0);

    m_nMarkLineMovePrePos = 0;

    connect(this, &MP3Widget::SigLoadMusic, &m_waveLayer, &WaveLayer::OnLoadMusic, Qt::QueuedConnection);
    connect(this, &MP3Widget::SigReleaseMusic, &m_waveLayer, &WaveLayer::OnReleaseMusic, Qt::QueuedConnection);
    connect(&m_waveLayer, &WaveLayer::SigMusicLoaded, this, &MP3Widget::OnMusicLoaded, Qt::QueuedConnection);
    connect(&m_waveLayer, &WaveLayer::SigMusicRelease, this, &MP3Widget::OnMusicReleaseed, Qt::QueuedConnection);

    connect(this, &MP3Widget::SigReDrawAll, this, &MP3Widget::ReDrawAll);
    connect(this, &MP3Widget::SigRedDrawTimeAndAction, this, &MP3Widget::OnReDrawTimeAndAction, Qt::DirectConnection);
    connect(this, &MP3Widget::SigOnPlayMP3, this, &MP3Widget::OnPlayMP3);

    m_bIsPlaying = false;
    m_bIsSinglePlaying = false;
    m_eCurMenuAction = eMenuNoAction;
    m_eMouseState = Constants::eMouseStateReleased;
    m_nPasteType = NO_PASTE;

    m_strMp3Path = "";
    m_strWavPath = "";

    m_nCurSelXStore = 0;
    setAcceptDrops(true);
    m_dragMode = -1;

    m_oMarkLineAdm.setLineSpaceWidth(Constants::MARK_LINE_TOP_ICON_WIDTH);
    m_oMarkLineAdm.setLineSpaceHeight(m_nTimeCellHeight + m_nSelLineLength);

    m_oRectSelectDrawer.setPenColor(Constants::COLOR_RECT_SELECT_BORER);
    m_oRectSelectDrawer.setBrushColor(Constants::COLOR_RECT_SELECT_FILL);

    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
}

MP3Widget::~MP3Widget()
{    
    SAFE_DELETE(m_pPixmapGroup);
    SAFE_DELETE(m_pPixmapMem);
    SAFE_DELETE(m_pPixmapWave);
    SAFE_DELETE(m_pPixmapBackGround);
    SAFE_DELETE(m_pMP3WidgetUI);
    SAFE_DELETE(m_pMP3Player);
}

bool MP3Widget::Connect(QWidget *pParent, ILayerOptionHandler_Event *pEvent)
{
    m_pEvent = pEvent;
    return true;
}

void MP3Widget::ReleaseConnection()
{
    //to do
}

QWidget *MP3Widget::GetWidget()
{
    return this;
}

void MP3Widget::SetActionBlockDataManager(CActionBlockDataManager *pManager, int nCurrentGroupID, int nCurrentIndex)
{
    if (!pManager)
    {
        return;
    }

    m_pActionDataManager = pManager;

    m_actionLayer.SetActionBlockDataManager(pManager);

    m_nCurrentGroupID = nCurrentGroupID;
    m_nCurrentFrameID = nCurrentIndex;
    m_actionLayer.ClearSelectGroupBlock();
    m_actionLayer.SetSelectGroupBlockID(nCurrentGroupID);
    m_actionLayer.setSelectGroupBlockID(m_pActionDataManager->getGroupIDSelected());

    // 由于传入的nCurrentIndex是数组下标，因此需要转为ID
    int nBlockID = -1;
    CActionBlockGroupManager *pGroupMgr = m_pActionDataManager->FindBlockData(nCurrentGroupID);
    if (pGroupMgr)
    {
        CActionBlockData *pActionBlock = pGroupMgr->GetAt(nCurrentIndex);
        if (pActionBlock)
        {
            nBlockID = pActionBlock->GetID();
        }
    }

    float fActionStartTime = m_pActionDataManager->getActionFrameStartTime(nCurrentGroupID, nBlockID);
    if (fActionStartTime > INVALID_INTEGER)
    {
       m_nCurSelX = fActionStartTime * m_timeScale * GetXScale();
    }

    updateCellCount();
}

void MP3Widget::SetTimeScale(int timeScale)
{
    m_timeScale = timeScale;
}

int MP3Widget::GetTimeScale()
{
    return m_timeScale;
}

void MP3Widget::UpdateMP3Action(int nGroupID, float fTime)
{
    m_nCurrentGroupID = nGroupID;
    m_actionLayer.ClearSelectGroupBlock();
    m_actionLayer.SetSelectGroupBlockID(nGroupID);

    m_actionLayer.setSelectGroupBlockID(m_pActionDataManager->getGroupIDSelected());

    if(NULL == m_pMP3Player ||!m_pMP3Player->IsPlaying())
    {
        double startTime = fTime * GetTimeScale();
        int endIndex = GetCellEndIndex();
        int startIndex = (startTime* GetXScale()) / (m_nCellWidth + m_nLineThick);

        m_nCurSelX = startTime * GetXScale();
        //往时间增加的方向选择
        if(startIndex >= endIndex)
        {
            setStartCellIndex(startIndex);
//            m_nCellStartIndex = startIndex;
//            emit SigCellIndexChanged(m_nCellStartIndex);
//            emit SigReDrawAll();
        }
        //往时间小的方向选择
        else if(startIndex <= m_nCellStartIndex)
        {
            setStartCellIndex(startIndex);
//            m_nCellStartIndex = startIndex;
//            emit SigCellIndexChanged(m_nCellStartIndex);
//            emit SigReDrawAll();
        }
        else//其他情况只需要重绘时间线和动作组选中状态
        {
            emit SigReDrawAll();
        }
    }
}

void MP3Widget::UpdateMP3Action(int nCol)
{
    m_actionLayer.ClearSelectGroupBlock();
    if(NULL == m_pMP3Player || !m_pMP3Player->IsPlaying())
    {
        int startIndex = (nCol * m_timeScale * GetXScale()) / (m_nCellWidth + m_nLineThick);
        m_nCurSelX = nCol * m_timeScale * GetXScale();
        if(startIndex >= GetCellEndIndex())
        {
            setStartCellIndex(GetCellEndIndex());
        }
        else if(startIndex <= m_nCellStartIndex)
        {
            setStartCellIndex(startIndex);
        }
        else
        {
            // modify by huangwuxian 仿真的时候需要重绘所有所有界面
            emit SigReDrawAll();
            //emit SigRedDrawTimeAndAction();
        }
    }
}

void MP3Widget::SetCellCount(int nCount)
{
    if(nCount > m_nCellCount)
    {
        m_nCellCount = nCount;
        m_nCellTotalSeconds = nCount;
        TimeLevelChanged(m_timeLevel);
        emitRangeChanged();
    }
}

bool MP3Widget::IsMusicLoaded()
{
    return m_waveLayer.IsLoaded();
}

void MP3Widget::SetPlaying(bool play)
{
    m_bIsPlaying = play;
}

void MP3Widget::SetSinglePlaying(bool play)
{
    m_bIsSinglePlaying = play;
}

void MP3Widget::OnScrollBarValueChanged(int value)
{
    //mp3正在播放，不响应操作
//    if(m_pMP3Player && m_pMP3Player->IsPlaying())
//    {
//        return;
//    }
    m_nCellStartIndex = value / (m_nCellWidth + m_nLineThick);
    emit SigReDrawAll();
}

void MP3Widget::OnMusicLoaded(bool sucessed, MP3Info stMP3Info, int bitRate)
{
    if(sucessed)
    {
        m_nCellCount = m_waveLayer.GetTime()  + 1;
        m_strWavPath = m_waveLayer.GetWavPath();
        if (!QFile::exists(m_strWavPath))
        {
            AlphaRobotLog::getInstance()->printError(QString("Wave file %1 doesn't exist").arg(m_strWavPath));
            return;
        }
        m_nCellTotalSeconds = m_nCellCount;
        m_timeLevel = ss_1;
        emitRangeChanged();
        m_nEndMilliSecs = m_waveLayer.GetTime() * 1000;//转为毫秒
        updateCellCount();
        if(m_pEvent)
        {
            m_pEvent->ILayerOptionHandler_DeleteMusic();
            m_pEvent->ILayerOptionHandler_MusicLoaded(m_strMp3Path, m_waveLayer.GetMusicTitle(), m_waveLayer.GetTime());
            m_pEvent->ILayerOptionHandler_SetMusicModifiedState(true);
        }
        QFileInfo fileInfo(m_strMp3Path);


        QString strMusicPath = CConfigs::getLocalMusicDir();
        QString strMusicPathFile = QDir::toNativeSeparators(strMusicPath + '/' + fileInfo.fileName());
        if(strMusicPathFile.compare(m_strMp3Path) != 0)
        {
             ClearDir(strMusicPath);
             QFile::copy(m_strMp3Path, strMusicPathFile);
        }
        emit SigNotifyLoadedMusic(stMP3Info);
    }
    else if(bitRate > 128)
    {
        CMessageBox::warning(NULL,
                             QObject::tr("AlphaRobot1s"),
                             QObject::tr("The bit rate of the current music is %1kbps, the robot can not play a "
                                                                           "music whose bit rate more than 128kbps, please select another music!").arg(bitRate));
    }
}


void MP3Widget::OnMusicReleaseed(bool sucessed)
{
    if(sucessed)
    {

        QString strMusicPath = CConfigs::getLocalMusicDir();

        ClearDir(strMusicPath);
        m_nCellCount = 0;
        m_nCellTotalSeconds = 0;
        m_timeLevel = ss_1;
        emitRangeChanged();
        m_nEndMilliSecs = 0;//转为毫秒
        if(m_pEvent)
        {
            m_pEvent->ILayerOptionHandler_DeleteMusic();
            m_pEvent->ILayerOptionHandler_MusicLoaded("", "", 0);
            m_pEvent->ILayerOptionHandler_SetMusicModifiedState(true);
        }
        updateCellCount();
        emit SigNotifyReleasedMusic();
    }
}

void MP3Widget::OnPositionChanged(quint64 position)
{
    UpdateTimeLineWidthMusic(position);
    if(m_pEvent)
    {
        m_pEvent->ILayerOptionHandler_UpdateMusicPosition(position);
    }
}

void MP3Widget::OnStopMP3()
{
    if(m_pMP3Player)
    {
        m_pMP3Player->disconnect();
        m_pMP3Player->Stop();
//        SAFE_DELETE(m_pMP3Player);
    }
}

void MP3Widget::OnPauseMP3()
{
    if(m_pMP3Player)
    {
        m_pMP3Player->Pause();
    }
}

void MP3Widget::OnFinishPlaying()
{
    if(m_pMP3Player)
    {
        m_pMP3Player->disconnect();
        m_pMP3Player->Stop();
//        SAFE_DELETE(m_pMP3Player);
    }
}

void MP3Widget::OnPlayMP3(int nStartTime, int nEndTime, emPlayStatus ePlayState)
{
    PrepareMP3();
    if(NULL == m_pMP3Player ||
            !m_waveLayer.IsLoaded())
        return;

    if(ePlayStatusStop == ePlayState)
    {
        m_pMP3Player->Stop();
    }
    else if(ePlayStatusPause == ePlayState)
    {
        m_pMP3Player->Pause();
    }
    else if(ePlayStatusPlaying == ePlayState)
    {
        m_pMP3Player->Play(nStartTime * m_timeScale, m_waveLayer.GetTime() * 1000.0);//从开始到结束
    }
    return;
}

void MP3Widget::OnPlayMP3FromCur()
{
    PrepareMP3();
    if(m_pMP3Player && m_waveLayer.IsLoaded())
        m_pMP3Player->Play(GetCurrentSelectCol() * m_timeScale, m_waveLayer.GetTime() * 1000.0);
}

void MP3Widget::OnResumeMP3()
{
    if(m_pMP3Player)
    {
        m_pMP3Player->Resume();
    }
}

/**************************************************************************
* 函数名: LoadMusic
* 功能: 加载音乐数据
* 参数:
*    @[in ] const QString& pszFilePath: 音乐文件路径
* 返回值: 成功返回true，失败返回false
* 时间: 2015/12/08 16:16
* 作者: ZDJ
*/
bool MP3Widget::LoadMusic(const QString& pszFilePath)
{
    emit SigLoadMusic(pszFilePath);
    m_strMp3Path = pszFilePath;
    return true;
}

int MP3Widget::GetCellWidth()
{
    return m_nCellWidth;
}

int MP3Widget::GetLineThick()
{
    return m_nLineThick;
}

int MP3Widget::GetCurSelectX()
{
    return m_nCurSelX;
}

QString MP3Widget::getMP3File()
{
    return m_strMp3Path;
}

double MP3Widget::GetCurrentSelectCol()
{
    double endIndex = (double)m_nCurSelX / (double)(m_nCellWidth + m_nLineThick);
    double ret = (double)GetCurrentms(endIndex) / (double)m_timeScale;
    return ret;
}

double MP3Widget::GetCurrentms(double curIndex)
{
    //转换成ms然后返回
    switch(m_timeLevel)
    {
    case ddd_10:
        return curIndex * 10.0;
    case ddd_50:
        return curIndex * 50.0;
    case ddd_100:
        return curIndex * 100 ;
    case ss_1:
        return curIndex * 1000.0;
    case ss_5:
        return curIndex * 5000.0;
    case ss_10:
        return curIndex * 10000.0;
    case mm_1:
        return curIndex * 60000.0;
    case mm_5:
        return curIndex * 300000.0 ;
    case mm_10:
        return curIndex * 600000.0;
    default:
        return curIndex;
        break;
    }

}

double MP3Widget::getCellTimeMs()
{
    return GetCurrentms(1);
}

double MP3Widget::getTimeMs(const QPoint &pos)
{
    double dOffset = ((double)pos.x())/((double)GetXScale());
    return (GetDisplayStartTime() + dOffset);
}

QPoint MP3Widget::getPos(quint64 nTimeMs)
{
    quint64 nTimeOffset = nTimeMs - GetDisplayStartTime();
    double nPosX = nTimeOffset * GetXScale();
    return QPoint(nPosX, this->height()/2);
}

quint64 MP3Widget::getCurrentTimeMs()
{
    return GetCurrentSelectCol() * GetTimeScale();
}

void MP3Widget::paintEvent(QPaintEvent *event)
{
    if(NULL != m_pPixmapMem)
    {
        QPainter painter(this);
        QRect rcWnd(0, 0, this->width(), this->height());
        painter.drawPixmap(rcWnd, *m_pPixmapMem, rcWnd);
    }
}

void MP3Widget::wheelEvent(QWheelEvent *event)
{
    event->ignore();//不处理，让父窗体处理，实现滚动条功能
}

void MP3Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 按比例显示音乐层各组件高度
    int nTotalHeight = Constants::DEFAULT_MUSIC_LAYER_TOTAL_HEIGHT;
    m_nCellHeight = this->height() * Constants::DEFAULT_CELL_HEIGHT/nTotalHeight;
    m_nTimeCellHeight = this->height() * Constants::DEFAULT_CELL_TIME_HEIGHT/nTotalHeight;
    m_nMusicCellHeight = this->height() * Constants::DEFAULT_CELL_MUSIC_HEIGHT/nTotalHeight;

    AdjustCellCount();
    emitRangeChanged();
    emit SigReDrawAll();
}

void MP3Widget::mousePressEvent(QMouseEvent *event)
{
    m_eMouseState = Constants::eMouseStatePressed;

    QRect rectContain(0,0,0,0);
    rectContain.setRight(GetContentWidth());
    rectContain.setBottom(m_nTimeCellHeight + m_nMusicCellHeight + m_nCellHeight);

    //Music layer
    QRect rectMusicContain(0,0,0,0);
    rectMusicContain.setRight(GetContentWidth());
    rectMusicContain.setTop(m_nTimeCellHeight);
    rectMusicContain.setBottom(m_nTimeCellHeight + m_nMusicCellHeight);

    //Motion Layer
    QRect rectMotionContain(0,0,0,0);
    rectMotionContain.setRight(GetContentWidth());
    rectMotionContain.setTop(m_nTimeCellHeight + m_nMusicCellHeight);
    rectMotionContain.setBottom(m_nTimeCellHeight + m_nMusicCellHeight + m_nCellHeight);
    m_oMousePressedPos = event->pos();
    if(rectContain.contains(m_oMousePressedPos))
    {
        if(m_pEvent)
        {
            int nCol = 0;
            if(event->button() == Qt::LeftButton)
            {
                processMusicLayerLeftMouseClicked(event);
            }
            else if(event->button() == Qt::RightButton)
            {
                m_eCurMenuAction =  eMenuNoAction;
                if(rectMotionContain.contains(m_oMousePressedPos))
                {
                    m_nCurSelX = m_nCellStartIndex * (m_nCellWidth + m_nLineThick) + m_oMousePressedPos.x();
                    nCol = GetCurrentSelectCol();
                    m_pEvent->ILayerOptionHandler_OnRButtonDown(nCol, m_oMousePressedPos);
                    CreateMotionLayerGroupListPopMenuActions();
                }
                else if(rectMusicContain.contains(m_oMousePressedPos))
                {
                    nCol = GetCurrentSelectCol();
                    QPoint curPos = event->pos();
                    double nDisplayStartTime = GetDisplayStartTime();
                    float fStartTime = nDisplayStartTime / GetTimeScale();

                    int nGroupId = -1;
                    int nFrameId = -1;
                    int nTimeOffset = 0;
                    m_pActionDataManager->clearAllGroupIDSelect();
                    m_pActionDataManager->getSelectGroupFrameID(nCol, nGroupId, nFrameId, nTimeOffset);
                    if (nGroupId >= 0) // 选中的区域属于某个动作组
                    {
                        m_pActionDataManager->setGroupIDSelected(nGroupId, true);
                    }
                    m_pEvent->ILayerOptionHandler_OnLButtonDown(nCol, curPos, fStartTime);

                    CreateMusicLayerGroupListPopMenuActions(nCol);
                }
                m_pEvent->Ilayeroptionhandler_MotionlayerActionCommand((int)m_eCurMenuAction, nCol);
            }
        }
    }

    QWidget::mousePressEvent(event);
}

void MP3Widget::mouseReleaseEvent(QMouseEvent *event)
{
    QRect rectAvailable(0, 0, 0, 0);
    rectAvailable.setRight(GetContentWidth());
    rectAvailable.setBottom(m_nTimeCellHeight + m_nMusicCellHeight + m_nCellHeight);
    QPoint curPos = event->pos();
    if(event->button() == Qt::LeftButton && rectAvailable.contains(curPos))
    {
        m_bIsLeftMousePressed = false;

        // 当光标下有标记线时，点击左键则进入移动状态
        double dCurPos = m_nCellStartIndex * (m_nCellWidth + m_nLineThick) + curPos.x();
        double dCurPosMs = GetCurrentms(dCurPos/(m_nCellWidth + m_nLineThick));

        // 鼠标点击时的位置
        double dMousePressedPos = m_nCellStartIndex * (m_nCellWidth + m_nLineThick) + m_oMousePressedPos.x();
        double dMousePressedPosMs = GetCurrentms(dMousePressedPos/(m_nCellWidth + m_nLineThick));

        CMarkLine *pMarkLine = m_oMarkLineAdm.getLineCursorHoverOn(dCurPosMs, GetXScale());

        // 上一个状态是eMouseStatePressed，即点击后马上释放，此时操作的是选择线
        if (!pMarkLine && m_eMouseState == Constants::eMouseStatePressed)
        {
            m_nCurSelX = dCurPos;
            int nCol = GetCurrentSelectCol();
            double nDisplayStartTime = GetDisplayStartTime();
            float fStartTime = nDisplayStartTime / GetTimeScale();
            if ((QApplication::keyboardModifiers() == Qt::ControlModifier))
            {
                //ctrl or shift
                int nGroupId = -1;
                int nFrameId = -1;
                int nTimeOffset = 0;
                m_pActionDataManager->getSelectGroupFrameID(nCol, nGroupId, nFrameId, nTimeOffset);
                if (nGroupId >= 0) // 选中的区域属于某个动作组
                {
                    m_pActionDataManager->setGroupIDSelected(nGroupId, true);
                }
                m_pEvent->ILayerOptionHandler_OnLButtonDown(nCol, curPos, fStartTime);
            }
            else
            {
                m_pActionDataManager->clearAllGroupIDSelect();
                m_pEvent->ILayerOptionHandler_OnLButtonDown(nCol, curPos, fStartTime);
            }
        }

        if (m_eMouseState == Constants::eMouseStateDragMove)
        {
            // 拖动矩形框选择方式结束
            if (!m_oMarkLineAdm.isAnyMarkLinePressed())
            {
                if (dCurPosMs >= dMousePressedPosMs)
                {
                    m_oMarkLineAdm.selectMarkLines(dMousePressedPosMs, dCurPosMs);
                }
                else
                {
                    m_oMarkLineAdm.selectMarkLines(dCurPosMs, dMousePressedPosMs);
                }
            }
            else  // 拖动标记线结束
            {
                m_oMarkLineAdm.resetLineState();
                //qDebug() << "Mouse release after clone line" << endl;

                // 拖动标记线结束后记录拖动后的数据
                m_oMarkLineAdm.cloneMarkLineAfterChanged(m_pHandleMarkLine);
                m_pHandleMarkLine = NULL;
            }
        }
    }

    // 重绘实现去掉选择框的效果
    emit ReDrawAll();
    m_eMouseState = Constants::eMouseStateReleased;
    QWidget::mouseReleaseEvent(event);
}

void MP3Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit SigLButtonDoubleClick(GetCurrentSelectCol());
    }
}

void MP3Widget::mouseMoveEvent(QMouseEvent *event)
{
    //Music layer
    QRect rectMusicContain(0,0,0,0);
    rectMusicContain.setRight(GetContentWidth());
    rectMusicContain.setTop(m_nTimeCellHeight);
    rectMusicContain.setBottom(m_nTimeCellHeight + m_nMusicCellHeight);
    QPoint curPos = event->pos();
    if(rectMusicContain.contains(curPos))
    {
        double dPosX = m_nCellStartIndex * (m_nCellWidth + m_nLineThick) + curPos.x();
        double dPosMs = GetCurrentms(dPosX/(m_nCellWidth + m_nLineThick));
        CMarkLine *pLine = m_oMarkLineAdm.getLineCursorHoverOn(dPosMs, GetXScale());

        // 光标在标记线上，且不是移动绘制矩形框
        if (pLine && (!(m_eMouseState == Constants::eMouseStateDragMove && !m_oMarkLineAdm.isAnyMarkLinePressed())))
        {
            //pLine->setState(Constants::eMarkLineStateHover);
            QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
        }
        else
        {
            // 如果左键还没松开，即使光标已经不在指定线上，还是认为选中标记线
            if (!m_bIsLeftMousePressed)
            {
                QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
            }
        }

        // 如果鼠标左键是按下状态，则标记线跟随光标移动位置
        if (m_bIsLeftMousePressed)
        {
            // 如果有标记线被点击，则认为是拖动标记线
            if (m_oMarkLineAdm.isAnyMarkLinePressed())
            {
                double dCurPosMs = getTimeMs(curPos);
                double dOffsetMs =  dCurPosMs - m_nMarkLineMovePrePos;

                m_nMarkLineMovePrePos = dCurPosMs;

                // 出现跨屏切换标记线
                int nCellStartIndex = getUpdatedStartCellIndex(dCurPosMs, getMoveDirection(dOffsetMs));
                if (m_nCellStartIndex != nCellStartIndex)
                {
                    setStartCellIndex(nCellStartIndex);

                    // 由于计算时根据光标上个位置和当前位置，如果光标不重新设置到线条位置，当左右滚动跨屏幕时，会出现大幅度调变
                    QPoint posNew = getPos(dCurPosMs);
                    QCursor cursor(Qt::SizeHorCursor);
                    cursor.setPos(mapToGlobal(posNew));
                    QApplication::setOverrideCursor(cursor);
                }

                //qDebug() << "Current pos ms = " << dCurPosMs << "offset ms = " << dOffsetMs << "New Cell Start Index = " << nCellStartIndex << endl;
                m_oMarkLineAdm.moveMarkLines(dOffsetMs);
                SigReDrawAll();
            }
            else  // 拖动选择框
            {
                SigReDrawAll();  // 需要重绘清空原来的选择线条

                // 在光标移动过程中不断重绘，释放鼠标后不会绘制，矩形选择框自动消失
                drawRectSelect(m_oMousePressedPos, curPos);
            }
        }
    }

    // 如果上一个状态是eMouseStateReleased状态或者eMouseStateMove，则是eMouseStateMove状态
    // 如果上一个状态是eMouseStatePressed或者eMouseStateDragMove，则是eMouseStateDragMove状态
    if (m_eMouseState == Constants::eMouseStateReleased)
    {
        m_eMouseState = Constants::eMouseStateMove;
    }
    else if (m_eMouseState == Constants::eMouseStatePressed)
    {
        m_eMouseState = Constants::eMouseStateDragMove;

        // 如果存在标记线选中的情况
        if (m_oMarkLineAdm.isAnyMarkLinePressed())
        {
            // 记录标记线移动前的数据
            m_pHandleMarkLine = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleMarkLine);
            //qDebug() << "Mouse release before clone line" << endl;
            m_oMarkLineAdm.cloneMarkLineBeforeChanged(m_pHandleMarkLine);
        }
    }

//    m_nMarkLineMovePrePos = getPosTimeMs(curPos);
    QWidget::mouseMoveEvent(event);
}

void MP3Widget::keyPressEvent(QKeyEvent *event)
{
    // 删除所有选中的标记线
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        m_oMarkLineAdm.deleteSelectedMarkLines();
        emit SigReDrawAll();
        update();
    }

    QWidget::keyPressEvent(event);
}

bool MP3Widget::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

        // 比较时间线位置和当前光标的位置，只有当两者基本重合时，才会提示当前的时间值
        int nCurrentCursor = m_nCellStartIndex * (m_nCellWidth + m_nLineThick) +helpEvent->pos().x();
        if (qAbs(m_nCurSelX - nCurrentCursor) < SELECT_LINE_HOVER_OFFSET)
        {
            QToolTip::showText(helpEvent->globalPos(), getFormatDisplayTime(m_nCurSelX/GetXScale()));
        }
    }
    return QWidget::event(event);
}

void MP3Widget::dragEnterEvent(QDragEnterEvent *event)
{
    //qDebug("dragEnterEvent: enter drag mode ............");

    if(event->mimeData()->hasFormat("TemplateAction"))
    {
        m_dragMode = 1;
        // 设置可放下图片
//        QPixmap drag_img(20, 18);
//        QPainter painter(&drag_img);
//        painter.drawText(QRectF(0,0, 20,18), NULL , QTextOption(Qt::AlignVCenter));
//        drag->setPixmap(drag_img);

        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void MP3Widget::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasFormat("TemplateAction"))
    {

        //Motion Layer
        QRect rectMotionContain(0,0,0,0);
        rectMotionContain.setRight(GetContentWidth());
        rectMotionContain.setTop(m_nTimeCellHeight + m_nMusicCellHeight);
        rectMotionContain.setBottom(m_nTimeCellHeight + m_nMusicCellHeight + m_nCellHeight);
        if(rectMotionContain.contains(event->pos()))
        {
            event->acceptProposedAction();
            return;
        }

    }
    event->ignore();

    //       QTreeWidget::dragMoveEvent(event);
}

void MP3Widget::dropLeaveEvent(QDragLeaveEvent *event)
{


    //qDebug("exit topotree, exit drag mode ............");
    m_dragMode = 0;

    //       QTreeWidget::dragLeaveEvent(event);
}

void MP3Widget::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat("TemplateAction"))
    {

        QByteArray data = event->mimeData()->data("TemplateAction");
        QString strName = "";
        if(event->mimeData()->hasText())
        {
            strName = event->mimeData()->text();
        }
        int nLens = data.length();
        char *pData = data.data();
        m_nCurSelX = m_nCellStartIndex * (m_nCellWidth + m_nLineThick) + event->pos().x();
        if(m_pEvent)
        {
            m_pEvent->Ilayeroptionhandler_TreeToView(pData, nLens, m_nCurSelX, strName );
        }
        return;
    }

    event->ignore();
    //        QTreeWidget::dropEvent(event);
}

void MP3Widget::DrawBackGround(QPixmap* pixmap)
{
    if(NULL == pixmap)
    {
        return;
    }
    //int wTmp = (m_nCellWidth + m_nLineThick) * m_nCellCount;//所有cell总宽度
    //QRect rtAll(m_nStartX, m_nStartY, wTmp, m_nCellHeight);//cell所占区域
    QRect rectWidget = this->geometry();
    QRect rectTimeBG = rectWidget;
    rectTimeBG.setHeight(m_nTimeCellHeight);
    QBrush bgBrush(Constants::COLOR_MP3_BG);
    QPainter painter;
    painter.begin(pixmap);
    painter.fillRect(rectWidget, bgBrush);
    painter.fillRect(rectTimeBG, QBrush(Constants::COLOR_TIME_BG));
    painter.end();
    DrawTimeLayer(pixmap);
    DrawBottomCell(pixmap);
}

//绘制显示的时间
void MP3Widget::DrawTimeLayer(QPixmap* pixmap)
{
    if(NULL == pixmap)
    {
        return;
    }
    QPainter painter(pixmap);
    int hTmp3 = m_nTimeCellHeight / 3;
    int xTmp = m_nStartX;
    //绘制选择线
    int nCount = GetCellEndIndex();

    //绘制时间刻度
    for(int i = m_nCellStartIndex; i <= nCount; i++)
    {
        if( i >= m_nCellCount)
            break;
        QPen pen(Constants::COLOR_LINE, m_nLineThick);
        painter.setPen(pen);

        //绘制时间文字
        //if((i + 1) % 2 == 0)
        {
            //刻度线
            painter.drawLine(xTmp, m_nTimeCellHeight - hTmp3,
                             xTmp, m_nTimeCellHeight);
            //绘制横线
            painter.drawLine(xTmp, m_nTimeCellHeight -1,
                             xTmp + m_nTimeCellWidth + m_nLineThick, m_nTimeCellHeight -1);

            //少画一个时间值，给放大和缩放
            if(i < nCount - 1)
            {
                QFont font;
                font.setPixelSize(FONT_PIXEL_SIZE_12);
                painter.setFont(font);
                QFontMetrics fm(font);
                QString strTime = GetTimeString(i+1);
                int width = fm.width(strTime);
                int height = fm.height();
                QRect rect(xTmp + m_nTimeCellWidth +  m_nLineThick - width / 2, 0,
                           width, height);
                painter.setPen(Constants::COLOR_TIME_FONT);
                painter.drawText(rect, strTime);
            }
        }
        xTmp += m_nLineThick + m_nTimeCellWidth;
    }
    //刻度线
    QPen pen(Constants::COLOR_LINE, m_nLineThick);
    painter.setPen(pen);
    painter.drawLine(xTmp, m_nTimeCellHeight - hTmp3,
                     xTmp, m_nTimeCellHeight);
}

//m_wavedata中是以s为单位存储波形点，因此需要根据TimeLevel进行换算
double MP3Widget::GetDisplayStartTime()
{
    return GetCurrentms(m_nCellStartIndex);
}

//m_wavedata中是以s为单位存储波形点，因此需要根据TimeLevel进行换算
double MP3Widget::GetDisplayEndTime()
{
    int width = GetContentWidth();
    double nStartTime= GetDisplayStartTime();
    //当前width对应的cell个数
    double cellCount = (double)width / (double)(m_nCellWidth + m_nLineThick);
    return nStartTime + GetCurrentms(cellCount);
}

//根据TimeLevel计算波形取点间隔数，m_wavedata中是以s为单位存储波形点
int MP3Widget::GetDrawInterval()
{
    //先计算每1秒间隔
    int nDrawInterval = (int)m_waveLayer.GetSimpleRate() / m_nPixelPerSec;//由于一格中只有m_nPixelPerSec个像素
    //因此在采样点中每隔nDrawInterval取一个样本绘制
    switch(m_timeLevel)
    {
    case ddd_10:
        return nDrawInterval / 100;
    case ddd_50:
        return nDrawInterval / 20;
    case ddd_100:
        return nDrawInterval / 10;
    case ss_1:
        return nDrawInterval;
    case ss_5:
        return nDrawInterval * 5;
    case ss_10:
        return nDrawInterval * 10;
    case mm_1:
        return nDrawInterval * 60;
    case mm_5:
        return nDrawInterval * 60 * 5;
    case mm_10:
        return nDrawInterval * 60 * 10;
    default:
        return nDrawInterval;
        break;
    }
}

//根据TimeLevel计算1ms所占像素值
double MP3Widget::GetXScale()
{
    double scale = (m_nCellWidth + m_nLineThick);//单位为1个cell时的长度
    switch(m_timeLevel)
    {
    case ddd_10:
        return scale / 10.0;
    case ddd_50:
        return scale / 50.0;
    case ddd_100:
        return scale / 100;
    case ss_1:
        return scale / 1000.0;
    case ss_5:
        return scale / 5000.0;
    case ss_10:
        return scale / 10000.0;
    case mm_1:
        return scale / 60000.0;
    case mm_5:
        return scale / 300000.0;
    case mm_10:
        return scale / 600000.0;
    default:
        return scale;
        break;
    }
}

void MP3Widget::DrawTimeSelLine(QPixmap* pixmap)
{
    if(NULL != pixmap)
    {
        QPainter painter;
        painter.begin(pixmap);
        int xPosition = m_nCurSelX - m_nCellStartIndex * (m_nCellWidth + m_nLineThick);
        QPen penSelLine(Constants::COLOR_SEL_LINE, m_nLineThick);
        painter.setPen(penSelLine);
        QPixmap pixmapPointer(":/res/images/icon_pointer.png");
        painter.drawPixmap(xPosition - pixmapPointer.width() / 2, 0,
                           pixmapPointer.width(), pixmapPointer.height(),
                           pixmapPointer);
        painter.drawLine(xPosition, pixmapPointer.height(), xPosition, m_nTimeCellHeight + m_nSelLineLength);
        painter.end();
    }
}

//获取要显示到第几个cell，通过widget宽度+m_nCellStartIndex计算得出
int MP3Widget::GetCellEndIndex()
{
    QRect rectWidget = this->geometry();
    //widget一屏幕能显示的cell的个数 + 开始的cell index，其中留最后一个格子显示放大缩小
    int nCellEndIndex = rectWidget.width() / (m_nLineThick + m_nCellWidth) + m_nCellStartIndex;
//    if (nCellEndIndex >= m_nCellCount)
//    {
//        nCellEndIndex = m_nCellCount;
//    }

    return nCellEndIndex;
}

/************************************
* 名称: GetTimeString
* 功能: 获取显示的时间字符串，根据hh:mm:ss和mm:ss.ddd的精度进行计算，参考Adobe Audtion软件
* 参数: [in]int nCurIndex当前的cell索引
* 返回:   QString 时间字符串
* 时间:   2016/01/08
* 作者:   hels
************************************/
QString MP3Widget::GetTimeString(int nCurIndex)
{
    //默认处于显示秒级别 间隔一个刻度显示时间
    //毫秒级别      10  50  100
    //秒级别        1   5   10
    //分钟级别      1  5   10
    switch(m_timeLevel)
    {
    case ddd_10:
        return QString().sprintf("%02d:%02d:%02d.%03d",
                                 (int)(nCurIndex / 100) / 3600,
                                 ( (int)(nCurIndex / 100) % 3600 ) / 60,
                                 (int)(nCurIndex / 100) % 60,
                                 (int)(nCurIndex) % 100 * 10);
        break;
    case ddd_50:
        return QString().sprintf("%02d:%02d:%02d.%03d",
                                 (int)(nCurIndex / 20) / 3600,
                                 ( (int)(nCurIndex / 20) % 3600 ) / 60,
                                 (int)(nCurIndex / 20) % 60,
                                 (int)(nCurIndex) % 20 * 50);
        break;
    case ddd_100:
        return QString().sprintf("%02d:%02d:%02d.%03d",
                                 (int)(nCurIndex / 10) / 3600,
                                 ( (int)(nCurIndex / 10) % 3600 ) / 60,
                                 (int)(nCurIndex / 10) % 60,
                                 (int)(nCurIndex) % 10 * 100);
        break;
    case ss_1:
        return QString().sprintf("%02d:%02d:%02d",
                                 (int)(nCurIndex) / 3600,
                                 ( (int)(nCurIndex) % 3600 ) / 60,
                                 (int)(nCurIndex) % 60);
    case ss_5:
        return QString().sprintf("%02d:%02d:%02d",
                                 (int)(nCurIndex * 5) / 3600,
                                 ( (int)(nCurIndex * 5) % 3600 ) / 60,
                                 (int)(nCurIndex * 5) % 60);
    case ss_10:
        return QString().sprintf("%02d:%02d:%02d",
                                 (int)(nCurIndex * 10) / 3600,
                                 ( (int)(nCurIndex * 10) % 3600 ) / 60,
                                 (int)(nCurIndex * 10) % 60);
    case mm_1:
        return QString().sprintf("%02d:%02d:%02d",
                                 (int)(nCurIndex * 60) / 3600,
                                 ( (int)(nCurIndex * 60) % 3600 ) / 60,
                                 0);
    case mm_5:
        return QString().sprintf("%02d:%02d:%02d",
                                 (int)(nCurIndex * 60 * 5) / 3600,
                                 ( (int)(nCurIndex * 60 * 5) % 3600 ) / 60,
                                 0);
    case mm_10:
        return QString().sprintf("%02d:%02d:%02d",
                                 (int)(nCurIndex * 60 * 10) / 3600,
                                 ( (int)(nCurIndex * 60 * 10) % 3600 ) / 60,
                                 0);
    default:
        break;
    }
    return QString("");
}

/************************************
* 名称: getFormatDisplayTime
* 功能: 获取格式化显示的字符串
* 参数: [in]quint64 nTimeMs 时间值，单位ms
* 返回:   QString 格式化后的字符串
* 时间:   2016/12/01
* 作者:   hwx
************************************/
QString MP3Widget::getFormatDisplayTime(quint64 nTimeMs)
{
    //默认处于显示秒级别 间隔一个刻度显示时间
    //毫秒级别      10  50  100
    //秒级别        1   5   10
    //分钟级别      1  5   10

    QString strDisplay;
    int nHour = (int)(nTimeMs / 1000) / 3600;
    int nMinute = ((int)(nTimeMs / 1000) % 3600 ) / 60;
    int nSecond = (int)(nTimeMs / 1000) % 60;
    int nMSecond = 0;
    switch(m_timeLevel)
    {
    case ddd_10:
    case ddd_50:
    case ddd_100:
    case ss_1:
        nMSecond = (int)(nTimeMs) % 1000;
        //strDisplay += QString("%1 %2 %3 %4 %5 %6 %7 %8").arg(nHour).arg(tr("hour")).arg(nMinute).arg(tr("minute")).arg(nSecond).arg(tr("Second")).arg(nMSecond).arg(tr("millisecond"));
        break;

    case ss_5:
    case ss_10:
    case mm_1:
    case mm_5:
    case mm_10:
        //strDisplay += QString("%1%2%3%4%5%6").arg(nHour).arg(tr("hour ")).arg(nMinute).arg(tr("minute ")).arg(nSecond).arg(tr("Second"));
    default:
        break;

    #if 0
    case ddd_10:
    case ddd_50:
    case ddd_100:
    case ss_1:
        return QString().sprintf("%02d:%02d:%02d.%03d",
                                 (int)(nTimeMs / 1000) / 3600,
                                  ((int)(nTimeMs / 1000) % 3600 ) / 60,
                                 (int)(nTimeMs / 1000) % 60,
                                 (int)(nTimeMs) % 1000);
    case ss_5:
    case ss_10:
    case mm_1:
    case mm_5:
    case mm_10:
        return QString().sprintf("%02d:%02d:%02d",
                                 (int)(nTimeMs / 1000) / 3600,
                                 ( (int)(nTimeMs / 1000) % 3600 ) / 60,
                                 (int)(nTimeMs / 1000) % 60);
    default:
        break;
#endif
    }

    // 如果高位时间单位已经存在或者当前单位值不为0，则低位必须显示
    bool bHasHighUnit = false;
    if (nHour > 0)
    {
        bHasHighUnit = true;
        strDisplay += QString(" %1 %2 ").arg(nHour).arg(tr("hour"));
         //strDisplay += QString("%1 %2 %3 %4 %5 %6 %7 %8").arg(nHour).arg(tr("hour")).arg(nMinute).arg(tr("minute")).arg(nSecond).arg(tr("Second")).arg(nMSecond).arg(tr("millisecond"));
    }
    if (bHasHighUnit || nMinute > 0)
    {
        bHasHighUnit = true;
        strDisplay += QString(" %1 %2 ").arg(nMinute).arg(tr("minute"));
    }
    if (bHasHighUnit || nSecond > 0)
    {
        bHasHighUnit = true;
        strDisplay += QString(" %1 %2 ").arg(nSecond).arg(tr("second"));
    }

    if (nMSecond > 0)
    {
        strDisplay += QString(" %1 %2 ").arg(nMSecond).arg(tr("millisecond"));
    }

    strDisplay = strDisplay.trimmed();
    return strDisplay;
}

int MP3Widget::GetContentWidth()
{
    int nEndIndex = GetCellEndIndex();
    int width;
    if(nEndIndex >= m_nCellCount) // 最后一块空间是空白的
    {
        width = (m_nCellWidth + m_nLineThick) * (m_nCellCount - m_nCellStartIndex);
    }
    else
    {
        width = geometry().width();
    }
    return width;
}

void MP3Widget::TimeLevelChanged(TimeLevel timeLevel)
{
    //if(m_timeLevel != timeLevel)
    {
        //根据时间变化前需要先获取当前的时间点真实数据
        //double time = GetCurrentms(m_nCellCount);
        double time = m_nCellTotalSeconds * 1000;//转为ms
        double endIndexTime = GetCurrentms((double)m_nCurSelX / (double)(m_nCellWidth + m_nLineThick));

        m_timeLevel = timeLevel;
        switch(timeLevel)
        {
        case ddd_10:
            m_nCellCount = qCeil(time / 10.0);
            break;
        case ddd_50:
            m_nCellCount = qCeil(time / 50.0);
            break;
        case ddd_100:
            m_nCellCount = qCeil(time / 100.0);
            break;
        case ss_1:
            m_nCellCount = qCeil(time / 1000.0);
            break;
        case ss_5:
            m_nCellCount = qCeil(time / 5000.0);
            break;
        case ss_10:
            m_nCellCount = qCeil(time / 10000.0);
            break;
        case mm_1:
            m_nCellCount = qCeil(time / 60000.0);
            break;
        case mm_5:
            m_nCellCount = qCeil(time / 300000.0);
            break;
        case mm_10:
            m_nCellCount = qCeil(time / 600000.0);
            break;
        default:
            break;
        }

        //根据要缩放的时间做相关的变化
        m_nCurSelX = endIndexTime * GetXScale();
        //m_nCellCount = (m_nCellCount == 0 ? 1 : m_nCellCount);
        AdjustCellCount();
        emitRangeChanged();
        emit SigReDrawAll();
    }
}

/**************************************************************************
* 函数名: PrepareMP3
* 功能:
* 参数:
* 返回值:
* 时间:
* 作者:
* 改动日志
**************************************************************************/
void MP3Widget::PrepareMP3()
{
    if(m_waveLayer.IsLoaded())
    {
        if(NULL == m_pMP3Player)
        {
            m_pMP3Player = new MP3Player(this);
//            connect(m_pMP3Player, &MP3Player::SigPositionChanged, this, &MP3Widget::OnPositionChanged);
//            connect(m_pMP3Player, &MP3Player::SigFinishPlaying, this, &MP3Widget::SigFinishMusicPlaying);
        }

        // 每次重新播放都需要重新connect，否则时间线不会跟着走动
        connect(m_pMP3Player, &MP3Player::SigPositionChanged, this, &MP3Widget::OnPositionChanged);
        connect(m_pMP3Player, &MP3Player::SigFinishPlaying, this, &MP3Widget::SigFinishMusicPlaying);

        m_pMP3Player->SetFileName(m_strWavPath);
    }
}

/**************************************************************************
* 函数名: ResetMP3
* 功能:删除MP3后，重置
* 参数:
* 返回值:
* 时间:
* 作者:周志平
* 改动日志
**************************************************************************/
void MP3Widget::ResetMP3()
{

//    if(m_pMP3Player)
//    {
//        SAFE_DELETE(m_pMP3Player);
//    }

    if(m_pMP3Player)
    {
        m_pMP3Player->SetFileName("");
    }
}


/**************************************************************************
* 函数名: emitRangeChanged
* 功能:
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::emitRangeChanged()
{
    //asu, 尾部有空余的地方，需要减去
    //emit SigRangeChanged(0, ((m_nCellCount - width() / (m_nCellWidth + m_nLineThick)) + 1) * (m_nCellWidth + m_nLineThick), m_nCellWidth + m_nLineThick);
    const int nMax = (m_nCellCount - (width() / (m_nCellWidth + m_nLineThick)))* (m_nCellWidth + m_nLineThick);
    emit SigRangeChanged(0, nMax, m_nCellWidth + m_nLineThick);
}

/**************************************************************************
* 函数名: ClearDir
* 功能:
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::ClearDir(const QString &fullPath)
{
    if(fullPath.isEmpty())
    {
        return;
    }

    QDir dir(fullPath);
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
        if(fi.isFile())
        {
            fi.dir().remove(fi.fileName());
        }
        else
        {
            ClearDir(fi.absoluteFilePath());
        }
    }
    //dir.rmpath(dir.absolutePath());
}

void MP3Widget::updateCellCount()
{
    int nCellCount = Constants::DEFAULT_CELL_COUNT;
    CActionBlockDataManager* pManager = m_actionLayer.GetActionBlockDataManager();
    if(pManager)
    {
        int ncount = pManager->GetCount();
        if(ncount > 0)
        {
           CActionBlockGroupManager* pGroupManager = pManager->GetAt(ncount - 1);
           if(pGroupManager)
           {
               nCellCount = pGroupManager->GetEndTime() / 100 + Constants::ADD_OFFSET;//转换为秒
               if(nCellCount < Constants::DEFAULT_CELL_COUNT)
               {
                   nCellCount = Constants::DEFAULT_CELL_COUNT;
               }
           }
        }
    }

    int nTotalTime = m_nCellTotalSeconds;
    if(!m_strMp3Path.isEmpty())
    {

        int nTime = m_waveLayer.GetTime() + 1;
        if(nTime > nCellCount)
        {
            nTotalTime = nTime;
        }
        else
        {
            nTotalTime = nCellCount;
        }
    }
    else
    {
        nTotalTime = nCellCount;
    }

    m_nCellTotalSeconds = nTotalTime;
    TimeLevelChanged(m_timeLevel);
}

void MP3Widget::AdjustCellCount()
{
    QRect rectWidget = this->geometry();
    int cellCount = rectWidget.width() / (m_nLineThick + m_nCellWidth);
    m_nCellCount = m_nCellCount > cellCount? m_nCellCount : cellCount;
}

void MP3Widget::addMarkLine(Constants::emMarkLineType eLineType)
{
    double dCurX = m_nCellStartIndex * (m_nCellWidth + m_nLineThick) + m_oMousePressedPos.x();
    double dCurMs = GetCurrentms(dCurX/(m_nCellWidth + m_nLineThick));
     m_oMarkLineAdm.createMarkLine(eLineType, dCurMs, this);
}

void MP3Widget::redrawAllMarkLine()
{
    double dStartX = m_nCellStartIndex * (m_nCellWidth + m_nLineThick);
    double dEndX = dStartX + m_pPixmapMem->width();
    double dStartMs = GetCurrentms(dStartX/(m_nCellWidth + m_nLineThick));
    double dEndMs = GetCurrentms(dEndX/(m_nCellWidth + m_nLineThick));
    QPixmap pixmapMarkLine(m_pPixmapMem->size());
    pixmapMarkLine.fill(QColor(0, 0, 0, 0));  // 设置背景透明
     m_oMarkLineAdm.redrawAllMarkLine(dStartMs, dEndMs, pixmapMarkLine);

     QPainter painter;
     painter.begin(m_pPixmapMem);
     painter.drawPixmap(0, 0, pixmapMarkLine.width(), pixmapMarkLine.height(), pixmapMarkLine);
     painter.end();
}

void MP3Widget::drawRectSelect(const QPoint &posTopLeft, const QPoint &posBottomRight)
{
    m_oRectSelectDrawer.drawRect(QRect(posTopLeft, posBottomRight), *m_pPixmapMem);
    update();
}

void MP3Widget::processMusicLayerLeftMouseClicked(QMouseEvent *event)
{
    m_nMarkLineMovePrePos = getTimeMs(event->pos());

    // 当光标下有标记线时，点击左键则进入移动状态
    double dCurPos = m_nCellStartIndex * (m_nCellWidth + m_nLineThick) + m_oMousePressedPos.x();
    double dPosMs = GetCurrentms(dCurPos/(m_nCellWidth + m_nLineThick));
    m_bIsLeftMousePressed = true;
    CMarkLine *pMarkLine = m_oMarkLineAdm.getLineCursorHoverOn(dPosMs, GetXScale());
    if (!pMarkLine)
    {
        return;
    }

    // 从hover状态进入移动状态
    pMarkLine->setState(Constants::eMarkLineStatePressed);
}

void MP3Widget::setStartCellIndex(int nIndex)
{
    m_nCellStartIndex = nIndex;
    emit SigCellIndexChanged(m_nCellStartIndex);
    emit SigReDrawAll();
}

int MP3Widget::getUpdatedStartCellIndex(quint64 nTimeMs, Constants::emMoveDirection eDir)
{
    int nCellStartIndex = m_nCellStartIndex;

    // 计算时间值对应的cell index
    double dCellIndex = (double)nTimeMs/getCellTimeMs();

    // 视窗能显示的cell数目
    int nDisplayCellNum = GetContentWidth()/(m_nCellWidth + m_nLineThick);

    // 获取视窗最后一个显示cell index
    int nEndCellIndex = GetCellEndIndex();

    // 时间值落在第一个cell
    if ((dCellIndex <= m_nCellStartIndex + 1) && (eDir == Constants::eMoveDirLeft))
    {
        // 如果重设后还没超出范围
        if (m_nCellStartIndex - nDisplayCellNum >= 0)
        {
            nCellStartIndex = m_nCellStartIndex - nDisplayCellNum + 1;
        }
        else
        {
            nCellStartIndex = 0;
        }
    }
    else if ((dCellIndex >= nEndCellIndex - 1) && (eDir == Constants::eMoveDirRight)) // 时间值落在最后一个cell
    {
        if ((nEndCellIndex + nDisplayCellNum) <= m_nCellCount)
        {
            nCellStartIndex = nEndCellIndex - 1;
        }
        else
        {
            nCellStartIndex = m_nCellCount - nDisplayCellNum;
        }
    }

    return nCellStartIndex;
}

Constants::emMoveDirection MP3Widget::getMoveDirection(double dOffset)
{
    if (dOffset < 0)
    {
        return Constants::eMoveDirLeft;
    }
    else if (dOffset > 0)
    {
        return Constants::eMoveDirRight;
    }

    return Constants::eMoveDirNone;
}

void MP3Widget::cloneMusicFileBeforeChanged(IOperation *pHMusicFile)
{
    if (!pHMusicFile)
    {
        return;
    }

    pHMusicFile->cloneDataBeforeChanged((LPARAM)(m_strMp3Path.toUtf8().data()), NULL);
}

void MP3Widget::cloneMusicFileAfterChanged(IOperation *pHMusicFile)
{
    if (!pHMusicFile)
    {
        return;
    }

    pHMusicFile->cloneDataAfterChanged((LPARAM)(m_strMp3Path.toUtf8().data()), NULL);
}

void MP3Widget::updateMarkLines(CMarkLineContainer *pMarkLines)
{
    if (!pMarkLines)
    {
        return;
    }

    m_oMarkLineAdm.updateMarkLines(pMarkLines);
    emit SigReDrawAll();
}

/**************************************************************************
* 函数名: OnCopy
* 功能:动作组复制响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnCopy()
{
    m_nPasteType = PASTE_AFTER_COPY;
    m_eCurMenuAction = eMenuMotionBlockCopy;
}

/**************************************************************************
* 函数名: OnSelectedBlockPlay
* 功能:动作组粘贴响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnPaste()
{
    if(m_nPasteType == PASTE_AFTER_CUT)
    {
        m_nPasteType = NO_PASTE;
    }
    m_eCurMenuAction = eMenuMotionBlockPaste;
}

/**************************************************************************
* 函数名: OnSelectedBlockPlay
* 功能:动作组剪切响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnCut()
{
    m_nPasteType = PASTE_AFTER_CUT;
    m_eCurMenuAction = eMenuMotionBlockCut;
}

void MP3Widget::onMergeGroup()
{
    m_eCurMenuAction = eMenuMotionBlockMergeGroup;
}

/**************************************************************************
* 函数名: OnSelectedBlockPlay
* 功能:删除动作组响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnDeleteSelectGroup()
{
    m_eCurMenuAction = eMenuMotionBlockDelete;
}

/**************************************************************************
* 函数名: OnSelectedBlockPlay
* 功能:单个动作组仿真响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnSelectedBlockPlay()
{
    m_eCurMenuAction = eMenuMotionBlockPlay;
}

/**************************************************************************
* 函数名: OnCancelBlockPlay
* 功能:取消动作组播放响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnCancelBlockPlay()
{
    m_eCurMenuAction = eMenuMotionBlockCancelPlay;
}
/**************************************************************************
* 函数名: OnInsertBlankTime
* 功能:动作组复制响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnInsertBlankTime()
{
    m_eCurMenuAction = eMenuMotionBlockInsert;
}
/**************************************************************************
* 函数名: OnSaveToCommandBox
* 功能:保存响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnSaveToCommandBox()
{
    m_eCurMenuAction = eMenuMotionBlockSaveTemplate;
}
/**************************************************************************
* 函数名: OnModifyGroup
* 功能:修改组名响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnModifyGroup()
{
    m_eCurMenuAction = eMenuMotionBlockModifyName;
}

void MP3Widget::slotOnMarkMotionFrame()
{
    m_eCurMenuAction = eMenuMotionFrameMarkFrame;
    addMarkLine(Constants::eMarkLineTypeFrame);
    redrawAllMarkLine();
    update();
}

void MP3Widget::slotOnMarkMotionGroup()
{
    m_eCurMenuAction = eMenuMotionFrameMarkGroup;
    addMarkLine(Constants::eMarkLineTypeBlock);
    redrawAllMarkLine();
    update();
}

void MP3Widget::slotOnModifyFrame()
{
    m_eCurMenuAction = eMenuMotionFrameModify;
}

void MP3Widget::slotOnInsertFrame()
{
    m_eCurMenuAction = eMenuMotionFrameInsert;
}

void MP3Widget::slotOnSplitInsertFrame()
{
    m_eCurMenuAction = eMenuMotionFrameSplitInsert;
}

void MP3Widget::slotOnCopyFrame()
{
    m_eCurMenuAction = eMenuMotionFrameCopy;
}

void MP3Widget::slotOnPaste()
{
    m_eCurMenuAction = eMenuMotionFramePaste;
}

void MP3Widget::slotOnDeleteFrame()
{
    m_eCurMenuAction = eMenuMotionFrameDelete;
}

void MP3Widget::slotOnPasteInNewGroup()
{
    m_eCurMenuAction = eMenuMotionFramePasteInNewGroup;
}

void MP3Widget::slotOnAddNewFrameToPreBlock()
{
    m_eCurMenuAction = eMenuMotionFrameAddFrameToPreGroup;
}

void MP3Widget::slotOnCreateNewFrameInNewBlock()
{
    m_eCurMenuAction = eMenuMotionFrameCreateNewFrameInNewGroup;
}
/**************************************************************************
* 函数名:OnSelectedMusicDel
* 功能: 删除Music响应槽
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::OnDeleteSelectedMusic()
{
    if(!m_strMp3Path.isEmpty())
    {

//        if (m_pMP3Player && m_pMP3Player->IsPlaying())
//        {
//            m_pMP3Player->disconnect();
//            m_pMP3Player->Stop();
//        }

        // 拷贝历史音乐文件，以便恢复
        m_strMp3Path = "";
        emit SigReleaseMusic();
    }
}

void MP3Widget::ReDrawAll()
{
    QSize size = this->size();
    if(NULL != m_pPixmapMem)
    {
        delete m_pPixmapMem;
    }
    m_pPixmapMem = new QPixmap(size.width(), size.height());
    m_pPixmapMem->fill(Qt::transparent);

    if(NULL != m_pPixmapBackGround)
    {
        delete m_pPixmapBackGround;
    }
    m_pPixmapBackGround = new QPixmap(size.width(), size.height());
    m_pPixmapBackGround->fill(Qt::transparent);

    if(NULL != m_pPixmapGroup)
    {
        delete m_pPixmapGroup;
    }
    m_pPixmapGroup = new QPixmap(size.width(), m_nCellHeight);
    m_pPixmapGroup->fill(Qt::transparent);

    if(NULL != m_pPixmapWave)
    {
        delete m_pPixmapWave;
    }
    m_pPixmapWave = new QPixmap(size.width(), m_nMusicCellHeight);
    m_pPixmapWave->fill(Qt::transparent);

    DrawBackGround(m_pPixmapBackGround);
    m_actionLayer.OnDrawActionGroup(m_pPixmapGroup);

    //填充背景色
    m_waveLayer.DrawWaveBackGround(m_pPixmapWave);
    m_actionLayer.onDrawActionFrame(m_pPixmapWave);
    m_waveLayer.DrawWavePixmap(m_pPixmapWave);

    DrawPixmapExcludeSelLine(m_pPixmapMem);
    DrawTimeSelLine(m_pPixmapMem);
    redrawAllMarkLine();

    update();
}

void MP3Widget::OnReDrawTimeAndAction()
{
    m_actionLayer.OnDrawActionGroup(m_pPixmapGroup);
    DrawPixmapExcludeSelLine(m_pPixmapMem);
    DrawTimeSelLine(m_pPixmapMem);
    update();
}

void MP3Widget::UpdateTimeLineWidthMusic(quint64 position)
{
    //position单位是毫秒，需要做相应转换
    double nCurPos = position;
    switch(m_timeLevel)
    {
    case ddd_10:
        nCurPos = nCurPos / 10.0;
        break;
    case ddd_50:
        nCurPos = nCurPos / 50.0;
        break;
    case ddd_100:
        nCurPos = nCurPos / 100.0;
        break;
    case ss_1:
        nCurPos = nCurPos / 1000.0;
        break;
    case ss_5:
        nCurPos = nCurPos / 5000.0 ;
        break;
    case ss_10:
        nCurPos = nCurPos / 10000.0;
        break;
    case mm_1:
        nCurPos = nCurPos / 60000.0;
        break;
    case mm_5:
        nCurPos = nCurPos / 300000.0;
        break;
    case mm_10:
        nCurPos = nCurPos / 600000.0;
        break;
    default:
        break;
    }
    int nEndIndex = GetCellEndIndex();
    m_nCurSelX = nCurPos * (m_nCellWidth + m_nLineThick);
    if(nCurPos >= nEndIndex)
    {
        setStartCellIndex(nEndIndex);
//        m_nCellStartIndex = nEndIndex;
//        emit SigReDrawAll();
//        emit SigCellIndexChanged(m_nCellStartIndex);
    }
    else
    {
        //只是更新时间线，不需要重新绘制所有图形
        if(m_bIsPlaying)
        {
            ReDrawAll();
            //OnReDrawTimeAndAction();
        }
        else
        {
            DrawPixmapExcludeSelLine(m_pPixmapMem);
            DrawTimeSelLine(m_pPixmapMem);
            update();
        }
    }
}

CActionBlockData *MP3Widget::getCurrentSelectedFrameData(CActionBlockGroupManager **pActionGroupMgr)
{
    if (!m_pActionDataManager)
    {
        return NULL;
    }

    quint64 nCurrentTime = getCurrentTimeMs();
    SetActionBlockDataType::const_iterator iter = m_pActionDataManager->FirstGroupIndex(0);
    for(;iter != m_pActionDataManager->EndIndex(); iter++)
    {
        CActionBlockGroupManager* pGroupManager = *iter;
        if(pGroupManager == NULL)
        {
            continue;
        }

        // 绘制动作组中每一动作帧范围
        vector<CActionBlockData*> vecFrameData;
        pGroupManager->GetAllBlockData(vecFrameData);
        for (int i  = 0; i < vecFrameData.size(); i++)  // 第一根线和最后一根线不用画
        {
            CActionBlockData *pFrameData = vecFrameData[i];
            if (!pFrameData)
            {
                continue;
            }

            int dFrameStartTime = pGroupManager->GetActionStartTimeByIndex(i) * GetTimeScale();
            int dFrameEndTime = dFrameStartTime + pFrameData->GetAllTime() * GetTimeScale();
            if (nCurrentTime >= dFrameStartTime && nCurrentTime < dFrameEndTime)
            {
                *pActionGroupMgr = pGroupManager;
                return pFrameData;
            }
        }
    }

    return NULL;
}


//void MP3Widget::StoreCurrentX()
//{
//    m_nCurSelXStore = m_nCurSelX;
//}

//void MP3Widget::RestoreCurrentX()
//{
//    m_nCurSelX = m_nCurSelXStore;
//    int nCol = GetCurrentSelectCol();
//    emit SigReDrawAll();
//    emit SigCellIndexChanged(m_nCellStartIndex);
//}


void MP3Widget::DrawPixmapExcludeSelLine(QPixmap* pixmap)
{
    if(NULL != pixmap)
    {
        QPainter painter;
        painter.begin(pixmap);
        painter.drawPixmap(0, 0, m_pPixmapBackGround->width(), m_pPixmapBackGround->height(), *m_pPixmapBackGround);
        painter.drawPixmap(0, m_nTimeCellHeight, m_pPixmapWave->width(), m_pPixmapWave->height(), *m_pPixmapWave);
        painter.drawPixmap(0, m_nTimeCellHeight + m_nMusicCellHeight, m_pPixmapGroup->width(), m_pPixmapGroup->height(), *m_pPixmapGroup);
        painter.end();
    }
}

/**************************************************************************
* 函数名:CreateMotionLayerGroupListPopMenuActions
* 功能:创建动作组右键动作组菜单
* 参数:
* 返回值:
* 时间:2016/01/28 17:19
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::CreateMotionLayerGroupListPopMenuActions()
{
    //创建菜单、菜单项
    QMenu* pMenu = new QMenu();

    QAction* pActionCopy = new QAction(QObject::tr("Copy"), this);
    QAction* pActionCut = new QAction(QObject::tr("Cut"), this);
    QAction* pActionPaste = new QAction(QObject::tr("Paste"), this);
    QAction* pActionInsert = new QAction(QObject::tr("Insert"), this);

    QAction *pActionMarkFrame = new QAction(QObject::tr("Mark Frame"), this);
    QAction *pActionMarkGroup = new QAction(QObject::tr("Mark Group"), this);

    QAction* pActionSaveToTemplate = new QAction(QObject::tr("Save to Template"), this);

    QAction* pActionSimulation = new QAction(QObject::tr("Simulate") ,this);

    QAction* pActionDelete = new QAction(QObject::tr("Delete"), this);
    QAction* pActionMergeGroup = new QAction(QObject::tr("Merge Group"), this);

    pMenu->addAction(pActionCopy);
    pMenu->addAction(pActionCut);
    pMenu->addAction(pActionInsert);
    pMenu->addAction(pActionPaste);
    pMenu->addSeparator();

    pMenu->addAction(pActionMarkFrame);
    pMenu->addAction(pActionMarkGroup);
    pMenu->addSeparator();

    pMenu->addAction(pActionSaveToTemplate);
    pMenu->addSeparator();

    pMenu->addAction(pActionSimulation);
    pMenu->addSeparator();

    pMenu->addAction(pActionDelete);

    pMenu->addAction(pActionMergeGroup);

    //连接信号与槽
    connect(pActionCopy, &QAction::triggered, this, &MP3Widget::OnCopy);
    connect(pActionCut, &QAction::triggered, this, &MP3Widget::OnCut);
    connect(pActionPaste, &QAction::triggered ,this, &MP3Widget::OnPaste);
    connect(pActionInsert, &QAction::triggered, this, &MP3Widget::OnInsertBlankTime);

    connect(pActionMarkFrame, &QAction::triggered, this, &MP3Widget::slotOnMarkMotionFrame);
    connect(pActionMarkGroup, &QAction::triggered, this, &MP3Widget::slotOnMarkMotionGroup);
    //connect(CancelPlayAction,&QAction::triggered,this,&MP3Widget::OnCancelBlockPlay);
   // connect(ModifyGroupAction,&QAction::triggered,this,&MP3Widget::OnModifyGroup);

    connect(pActionSaveToTemplate, &QAction::triggered, this, &MP3Widget::OnSaveToCommandBox);
    connect(pActionSimulation, &QAction::triggered, this, &MP3Widget::OnSelectedBlockPlay);
    connect(pActionDelete, &QAction::triggered, this, &MP3Widget::OnDeleteSelectGroup);
    connect(pActionMergeGroup, &QAction::triggered, this, &MP3Widget::onMergeGroup);


    //菜单出现的位置为当前鼠标的位置
    if(m_pEvent)
    {

        if( m_nCurrentGroupID == -1 || m_bIsPlaying || m_bIsSinglePlaying)
        {
            pActionCopy->setEnabled(false);
            pActionCut->setEnabled(false);
//            pActionPaste->setEnabled(false);
            pActionInsert->setEnabled(false);

            pActionMarkFrame->setEnabled(false);
            pActionMarkGroup->setEnabled(false);

            pActionSaveToTemplate->setEnabled(false);
            pActionSimulation->setEnabled(false);
            pActionDelete->setEnabled(false);
            pActionMergeGroup->setEnabled(false);
        }

        if(m_bIsPlaying)
        {
            pActionPaste->setEnabled(false);
        }
//        if(m_bIsSinglePlaying)
//        {
//            CancelPlayAction->setEnabled(true);
//        }
        if(m_nPasteType == NO_PASTE)
        {
            pActionPaste->setEnabled(false);
        }

        if(m_pActionDataManager->getGroupIDSelected().size() < 2)
        {
            pActionMergeGroup->setEnabled(false);
        }else {
            pActionCopy->setEnabled(false);
            pActionCut->setEnabled(false);
            pActionPaste->setEnabled(false);
            pActionInsert->setEnabled(false);

            pActionMarkFrame->setEnabled(false);
            pActionMarkGroup->setEnabled(false);

            pActionSaveToTemplate->setEnabled(false);
            pActionSimulation->setEnabled(false);
            pActionDelete->setEnabled(false);
        }
    }

    QFile styleSheet(":/res/qss/mp3projmenudefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        pMenu->setStyleSheet(styleSheet.readAll());
    }

    pMenu->exec(QCursor::pos());
    pMenu->deleteLater();
}
/**************************************************************************
* 函数名:CreateMusicLayerGroupListPopMenuActions
* 功能:创建音乐右键动作组菜单
* 参数:
* 返回值:
* 时间:2016/01/28 17:19
* 作者: 周志平
* 改动日志
**************************************************************************/
void MP3Widget::CreateMusicLayerGroupListPopMenuActions(int nSelectCol)
{
    // 判断用户右键点击的位置是否在空白区域
    if (!m_pActionDataManager)
    {
        return;
    }

    int nGroupId = -1;
    int nFrameId = -1;
    int nTimeOffset = 0;
    m_pActionDataManager->getSelectGroupFrameID(nSelectCol, nGroupId, nFrameId, nTimeOffset);
    if (nGroupId < 0) // 选中的区域属于某个动作组
    {
        createMusicLayerMenuRightClickedOnBlankMotionFrame();
    }
    else  // 选中空白区域
    {
        createMusicLayerMenuRightClickedOnNoneBlankMotionFrame();
    }
}

void MP3Widget::createMusicLayerMenuRightClickedOnNoneBlankMotionFrame()
{
    //创建菜单、菜单项
    QMenu* pMenu = new QMenu(this);

    QAction* pActionModifyFrame = new QAction(tr("Modify"), pMenu);
    QAction *pActionInsertFrame = new QAction(tr("Insert"), pMenu);
    QAction *pActionSplitInsertFrame = new QAction(tr("Split Insert"), pMenu);

    QAction *pActionCopy = new QAction(tr("Copy"), pMenu);
    QAction *pActionPaste = new QAction(tr("Paste"), pMenu);

    QAction *pActionDelete = new QAction(tr("Delete"), pMenu);

    QAction *pActionMarkFrame = new QAction(QObject::tr("Mark Frame"), pMenu);
    QAction *pActionMarkGroup = new QAction(QObject::tr("Mark Group"), pMenu);

   pMenu->addAction(pActionModifyFrame);
   pMenu->addAction(pActionInsertFrame);
   pMenu->addAction(pActionSplitInsertFrame);
   pMenu->addSeparator();

  pMenu->addAction(pActionCopy);
  pMenu->addAction(pActionPaste);
  pMenu->addSeparator();

   pMenu->addAction(pActionMarkFrame);
   pMenu->addAction(pActionMarkGroup);
   pMenu->addSeparator();

   pMenu->addAction(pActionDelete);

    //连接信号与槽
    //connect(DeleteAction,&QAction::triggered,this,&MP3Widget::OnDeleteSelectedMusic);
   connect(pActionModifyFrame, &QAction::triggered, this, &MP3Widget::slotOnModifyFrame);
   connect(pActionInsertFrame, &QAction::triggered, this, &MP3Widget::slotOnInsertFrame);
   connect(pActionSplitInsertFrame, &QAction::triggered, this, &MP3Widget::slotOnSplitInsertFrame);

  connect(pActionCopy, &QAction::triggered, this, &MP3Widget::slotOnCopyFrame);
  connect(pActionPaste, &QAction::triggered, this, &MP3Widget::slotOnPaste);

   connect(pActionMarkFrame, &QAction::triggered, this, &MP3Widget::slotOnMarkMotionFrame);
   connect(pActionMarkGroup, &QAction::triggered, this, &MP3Widget::slotOnMarkMotionGroup);

   connect(pActionDelete, &QAction::triggered, this, &MP3Widget::slotOnDeleteFrame);

    //菜单出现的位置为当前鼠标的位置
    int nSelGroupID = -1;
    int nSelFrameID = -1;
    int nOffset = 0;
    if (m_pActionDataManager)
    {
        m_pActionDataManager->getSelectGroupFrameID(GetCurrentSelectCol(), nSelGroupID, nSelFrameID, nOffset);
    }

    if( nSelGroupID == -1 || m_bIsPlaying || m_bIsSinglePlaying)
    {
        pActionModifyFrame->setEnabled(false);
        pActionInsertFrame->setEnabled(false);
        pActionSplitInsertFrame->setEnabled(false);
        pActionCopy->setEnabled(false);
        pActionPaste->setEnabled(false);
        pActionDelete->setEnabled(false);
    }
    else
    {
        pActionModifyFrame->setEnabled(true);
        pActionCopy->setEnabled(true);
        pActionInsertFrame->setEnabled(true);
        pActionSplitInsertFrame->setEnabled(true);
        pActionDelete->setEnabled(true);
    }

    // 判断copy后才能paste
    if (m_pEvent && m_pEvent->ILayerOptionHandler_hasCopyFrame())
    {
        pActionPaste->setEnabled(true);
    }
    else
    {
        pActionPaste->setEnabled(false);
    }

    QFile styleSheet(":/res/qss/mp3projmenudefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        pMenu->setStyleSheet(styleSheet.readAll());
    }

    pMenu->exec(QCursor::pos());
    pMenu->deleteLater();


    // 由于调用exec右键菜单弹出后，事件由pMenu，MP3Widget捕获不到release事件，
    // 这里需要手动设置成release状态
    m_eMouseState = Constants::eMouseStateReleased;
}

void MP3Widget::createMusicLayerMenuRightClickedOnBlankMotionFrame()
{
    //创建菜单、菜单项
    QMenu* pMenu = new QMenu(this);

    QAction* pActionAddNewFrameToPreBlock = new QAction(tr("Add New Frame to Previous Block"), pMenu);
    QAction *pActionCreateNewFrameInNewBlock = new QAction(tr("Create New Frame in New Block"), pMenu);

    QAction *pActionPaste = new QAction(tr("Paste"), pMenu);

    QAction *pActionMarkFrame = new QAction(QObject::tr("Mark Frame"), pMenu);
    QAction *pActionMarkGroup = new QAction(QObject::tr("Mark Group"), pMenu);

   pMenu->addAction(pActionAddNewFrameToPreBlock);
   pMenu->addAction(pActionCreateNewFrameInNewBlock);
   pMenu->addSeparator();

   pMenu->addAction(pActionPaste);
   pMenu->addSeparator();

   pMenu->addAction(pActionMarkFrame);
   pMenu->addAction(pActionMarkGroup);

    //连接信号与槽
    //connect(DeleteAction,&QAction::triggered,this,&MP3Widget::OnDeleteSelectedMusic);
   connect(pActionPaste, &QAction::triggered, this, &MP3Widget::slotOnPasteInNewGroup);
   connect(pActionAddNewFrameToPreBlock, &QAction::triggered, this, &MP3Widget::slotOnAddNewFrameToPreBlock);
   connect(pActionCreateNewFrameInNewBlock, &QAction::triggered, this, &MP3Widget::slotOnCreateNewFrameInNewBlock);

   connect(pActionMarkFrame, &QAction::triggered, this, &MP3Widget::slotOnMarkMotionFrame);
   connect(pActionMarkGroup, &QAction::triggered, this, &MP3Widget::slotOnMarkMotionGroup);

   // 如果当前没有选中任何动作帧（动作组）
   // 则添加到上一个动作组禁用
   if (m_pActionDataManager && (m_pActionDataManager->GetCount() <= 0))
   {
       pActionAddNewFrameToPreBlock->setEnabled(false);
   }

   // 判断copy后才能paste
   if (m_pEvent && m_pEvent->ILayerOptionHandler_hasCopyFrame())
   {
       pActionPaste->setEnabled(true);
   }
   else
   {
       pActionPaste->setEnabled(false);
   }

    QFile styleSheet(":/res/qss/mp3projmenudefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        pMenu->setStyleSheet(styleSheet.readAll());
    }

    pMenu->exec(QCursor::pos());
    pMenu->deleteLater();

    // 由于调用exec右键菜单弹出后，事件由pMenu，MP3Widget捕获不到release事件，
    // 这里需要手动设置成release状态
    m_eMouseState = Constants::eMouseStateReleased;
}

void MP3Widget::SetActStartTime(float fActStartTime)
{

}

void MP3Widget::SetActEndTime(float fActEndTime)
{

}

/************************************
* 名称: DrawBottomCell
* 功能: 绘制底部cell，参考原来windows下的界面
* 参数: [in]QPixmap* pixmap绘图对象指针
* 返回:   void
* 时间:   2016/01/07
* 作者:   hels
************************************/
void MP3Widget::DrawBottomCell(QPixmap* pixmap)
{
    if(NULL == pixmap)
    {
        return;
    }
    QPainter painter(pixmap);
    int nCount = GetCellEndIndex();
    int xTmp = m_nStartX;
    //绘制波形底部的方框
    int bottomStartY = m_nTimeCellHeight + m_nMusicCellHeight;//底部方框起始Y坐标
    for(int i = m_nCellStartIndex; i <= nCount; i++)
    {
        if(i >= m_nCellCount) //判断音谱太短，rectWidget宽度都没占满时返回
            break;
        QPen penLine(Constants::COLOR_LINE, m_nLineThick, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);
        painter.setPen(penLine);

        //竖线
        painter.drawLine(xTmp, bottomStartY, xTmp, bottomStartY + m_nCellHeight);
        //横线
        painter.drawLine(xTmp, bottomStartY + m_nCellHeight,
                         xTmp + m_nCellWidth + m_nLineThick, bottomStartY + m_nCellHeight);
        //每五个，填充一次稍深得颜色，跟windows下vs版本保持一致
        if( (i + 1) % 5 == 0)
        {
            QRect rect(xTmp + 1, bottomStartY, m_nCellWidth, m_nCellHeight);
            QBrush brush(Constants::COLOR_BRUSH_CELL);
            painter.fillRect(rect, brush);
        }
        else
        {
            QRect rect(xTmp + 1, bottomStartY, m_nCellWidth, m_nCellHeight);
            QBrush brush(Constants::COLOR_DEF_BRUSH_CELL);
            painter.fillRect(rect, brush);
        }
        xTmp += m_nCellWidth;
        xTmp += m_nLineThick;
    }
    //竖线
    painter.drawLine(xTmp, bottomStartY, xTmp, bottomStartY + m_nCellHeight);
}

void MP3Widget::on_btnZoomOut_clicked()
{
    //mp3正在播放，不响应缩放操作
    if(m_pMP3Player && m_pMP3Player->IsPlaying())
    {
        return;
    }

    if(m_timeLevel < TimeLevel::mm_10)
    {
        TimeLevelChanged((TimeLevel)((int)m_timeLevel + 1));
    }
}

void MP3Widget::on_btnZoomIn_clicked()
{
    //mp3正在播放，不响应缩放操作
    if(m_pMP3Player && m_pMP3Player->IsPlaying())
    {
        return;
    }

    if(m_timeLevel > TimeLevel::ddd_10)
    {
        TimeLevelChanged((TimeLevel)((int)m_timeLevel - 1));
    }
}
