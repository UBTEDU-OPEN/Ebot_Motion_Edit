/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：MP3Widget.h
* 创建时间：2015/11/03 10:15
* 文件标识：
* 文件摘要：波形显示控件
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2015/01/12
* 版本摘要：
*/
#ifndef MP3WIDGET_H
#define MP3WIDGET_H

#include "stdafx.h"
#include "wave.h"
#include <QPainter>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QThread>
#include "mp3player.h"
#include "mp3projconstants.h"
#include "wavelayer.h"
#include "actionlayer.h"
#include "LayerUI/IActionLayerManager.h"
#include "LayerUI/ILayerOptionHandler.h"
#include <QDrag>
#include <QMimeData>

#include "cmarklineadm.h"
#include "crectselectdrawer.h"

namespace Ui
{
    class MP3Widget;
}

enum PASTE_TYPE
{
    NO_PASTE = 0,
    PASTE_AFTER_COPY,
    PASTE_AFTER_CUT
};

class MP3Widget : public QWidget
{
    Q_OBJECT
public:
    explicit MP3Widget(QWidget *parent = 0);
    ~MP3Widget();

public:
    //ILayerOptionHandler对应的接口调用
    bool Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent);
    void ReleaseConnection();
    QWidget* GetWidget();
    void SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID);
    void SetTimeScale(int timeScale);
    int GetTimeScale();
    void UpdateMP3Action(int nGroupID, float fTime);
    void UpdateMP3Action(int nCol);
    void SetCellCount(int nCount);
    bool IsMusicLoaded();
    void SetPlaying(bool play);
    void SetSinglePlaying(bool play);

signals:
    /************************************
    * 名称: SigRangeChanged
    * 功能: 窗体范围改变时发送此信号，由滚动条监听，并做相应改变
    * 参数: [in]int min滚动条最小范围
    * 参数: [in]int max滚动条最大范围
    * 参数: [in]int nStep滚动条步进大小
    * 返回:   void
    * 时间:   2016/01/12
    * 作者:   hels
    ************************************/
    void SigRangeChanged(int min, int max, int nStep);

    /************************************
    * 名称: SigLoadMusic
    * 功能: 添加mp3时发送此信号，由解析模块监听
    * 参数: [in]const QString& pszFilePath mp3文件路径
    * 返回:   void
    * 时间:   2016/01/12
    * 作者:   hels
    ************************************/
     void SigLoadMusic(const QString& pszFilePath);

     /************************************
     * 名称: SigReleaseMusic
     * 功能: 删除mp3时发送此信号，由解析模块监听
     * 参数:
     * 返回:   void
     * 时间:   2016/01/28
     * 作者:   周志平
     ************************************/
    void SigReleaseMusic();

    //加载成功时发送，通知主窗体设置按钮状态
    void SigNotifyLoadedMusic(MP3Info stMP3Info);
    void SigNotifyReleasedMusic();

    //hels 音乐同步时，cell索引改变时发送，通知滚动条改变位置
    void SigCellIndexChanged(int index);

    void SigLButtonDoubleClick(int nCol);

    void SigReDrawAll();
    void SigRedDrawTimeAndAction();
    void SigOnPlayMP3(int nStartTime, int nEndTime, emPlayStatus ePlayState);
    void SigFinishMusicPlaying();

public slots:
    //hels 滚动条位置改变时调用，重绘所有波形相关绘图（时间、波形、底部cell）
    void OnScrollBarValueChanged(int value);

    //hels 音乐加载完成时调用，完成相关数据初始化，准备播放mp3
    void OnMusicLoaded(bool sucessed, MP3Info stMP3Info, int bitRate);

    //hels 音乐卸载完成时调用，完成相关数据删除
    void OnMusicReleaseed(bool sucessed);

    //hels 音乐播放位置更新
    void OnPositionChanged(quint64 position);
    //hels mp3播放相关
    void OnPlayMP3(int nStartTime, int nEndTime, emPlayStatus ePlayState);
    void OnPlayMP3FromCur();//从当前位置开始播放
    void OnResumeMP3();
    void OnStopMP3();
    void OnPauseMP3();
    void OnFinishPlaying();

    //hels 重绘所有图形，一般发生在大小改变，或者仿真时
    //改为信号和槽方式排队触发，Motion 线程模块更新时也会调用到此函数，之前的直接调用方式会导致程序崩溃
    void ReDrawAll();
    void OnReDrawTimeAndAction();

    void OnDeleteSelectedMusic();

public:

    /**************************************************************************
    * 函数名: LoadMusic
    * 功能: 加载音乐数据
    * 参数:
    *    @[in ] const QString& pszFilePath: 音乐文件路径
    * 返回值: 成功返回true，失败返回false
    * 时间: 2015/12/08 16:16
    * 作者: ZDJ
    */
    bool LoadMusic(const QString& pszFilePath);

    //hels
    int GetCellWidth();
    int GetLineThick();
    //当前选中的位置，单位为像素（取值范围为窗体可见范围0-width）
    int GetCurSelectX();

    // 获取MP3文件
    QString getMP3File();

    //当前选中的列，以时间ms为单位，表示显示范围内的当前列，即需要减去m_nCellStartIndex进行计算返回，
    //提供给Motion模块获取
    //（add by hwx : 获取的列与CActionBlockData/CActionBlockDataManager中m_fRunTime/m_fStartTime等单位对应,
    // 都是除以过时间粒度m_timeScale，以与m_fRunTime/m_fStartTime进行比较）
    double GetCurrentSelectCol();

    //获取当前索引位置对应的毫秒值,curIndex = curX / (m_cellWidth + m_lineThick) + m_nCellStartIndex;
    double GetCurrentms(double curIndex);

    // 获取一单元格表示的时间，单位ms
    double getCellTimeMs();

    // 获取指定位置对应的时间值，pos为相对于控件位置
    double getTimeMs(const QPoint &pos);

    // 获取指定时间值对应界面中的位置
    QPoint getPos(quint64 nTimeMs);

    // 获取当前选择线所在时间值，单位ms
    quint64 getCurrentTimeMs();

    //hels 根据TimeLevel计算curIndex对应的ms
    //quint64 GetTimeIndex(double curIndex);
    //ms为单位
    double GetDisplayStartTime();
    double GetDisplayEndTime();

    //hels 根据TimeLevel计算波形取点间隔数,提供给WaveLayer使用
    int GetDrawInterval();
    //hels 根据TimeLevel计算1ms所占像素值
    double GetXScale();

    //hels 获取显示内容的宽度
    int GetContentWidth();

    //hels 音乐播放时，同步时间线和波形位置等
    void UpdateTimeLineWidthMusic(quint64 position);

public:
    // 获取当前选中的帧数据
    CActionBlockData *getCurrentSelectedFrameData(CActionBlockGroupManager **pActionGroupMgr);

    // 更新标记线
    void updateMarkLines(CMarkLineContainer *pMarkLines);

    // 拷贝音乐文件，以便进行撤销重做功能
    void cloneMusicFileBeforeChanged(IOperation *pHMusicFile);
    void cloneMusicFileAfterChanged(IOperation *pHMusicFile);

    //提供给仿真结束时，恢复时间选择的位置
    //void StoreCurrentX();
    //void RestoreCurrentX();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent * event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event) override;
    virtual void keyPressEvent(QKeyEvent * event) override;

    virtual bool event(QEvent *event); // 显示tooltip，当前时间指针所在的值

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent * event);

protected:
    //hels 绘制背景部分
    void DrawBackGround(QPixmap* pixmap);

    /************************************
    * 名称: DrawBottomCell
    * 功能: 绘制底部cell，参考原来windows下的界面
    * 参数: [in]QPixmap* pixmap绘图对象指针
    * 返回:   void
    * 时间:   2016/01/07
    * 作者:   hels
    ************************************/
    void DrawBottomCell(QPixmap* pixmap);

    /************************************
    * 名称: DrawTimeLayer
    * 功能: 绘制显示时间
    * 参数: [in]QPixmap* pixmap绘图对象指针
    * 返回:   void
    * 时间:   2016/01/08
    * 作者:   hels
    ************************************/
    void DrawTimeLayer(QPixmap* pixmap);

    //绘制时间选择线,由于鼠标点击切换时间点时，不需要重绘全部内容，为提高效率，将时间线专门绘制
    void DrawTimeSelLine(QPixmap* pixmap);
    //将除时间线外的其他pixmap绘制到m_pPixmapMem中,贴图的操作，很快,主要给鼠标点击时间线时使用，其他时候用不到
    void DrawPixmapExcludeSelLine(QPixmap* pixmap);
    //创建动作组右键动作组菜单
    void CreateMotionLayerGroupListPopMenuActions();
    //创建音乐右键动作组菜单
    void CreateMusicLayerGroupListPopMenuActions(int nSelectCol);

    // 创建在已经存在的动作帧上的点击右键菜单
    void createMusicLayerMenuRightClickedOnNoneBlankMotionFrame();

    // 创建在空白动作帧上点击的右键菜单
    void createMusicLayerMenuRightClickedOnBlankMotionFrame();

    void SetActStartTime(float fActStartTime);
    void SetActEndTime(float fActEndTime);
private:
    enum TimeLevel
    {
        //毫秒级别
        ddd_10,
        ddd_50,
        ddd_100,
        //秒级别
        ss_1,
        ss_5,
        ss_10,
        //分钟级别
        mm_1,
        mm_5,
        mm_10
    };


    /************************************
    * 名称: GetCellEndIndex
    * 功能: 获取widget能显示到的cell的索引，会根据widget宽度和startindex计算
    * 返回:   int 返回索引值
    * 时间:   2016/01/08
    * 作者:   hels
    ************************************/
    int GetCellEndIndex();

    /************************************
    * 名称: GetTimeString
    * 功能: 获取显示的时间字符串，根据hh:mm:ss和mm:ss.ddd的精度进行计算，参考Adobe Audtion软件
    * 参数: [in]int nCurIndex当前的cell索引
    * 返回:   QString 时间字符串
    * 时间:   2016/01/08
    * 作者:   hels
    ************************************/
    QString GetTimeString(int nCurIndex);

    /************************************
    * 名称: getFormatDisplayTime
    * 功能: 获取格式化显示的字符串
    * 参数: [in]quint64 nTimeMs 时间值，单位ms
    * 返回:   QString 格式化后的字符串
    * 时间:   2016/12/01
    * 作者:   hwx
    ************************************/
    QString getFormatDisplayTime(quint64 nTimeMs);

    //hels 获取鼠标点击时，能切换时间线的有效区域
//    QRect GetContainRect();

    //hels 鼠标中键改变enum TimeLevel，此值会影响波形绘制的精度
    void TimeLevelChanged(TimeLevel timeLevel);

    //hels 初始化MP3播放
    void PrepareMP3();

    //hels 重置MP3播放
    void ResetMP3();

    void emitRangeChanged();

    void ClearDir(const QString& fullPath);

    //删除动作，删除音乐时需要重新计算
    void updateCellCount();
    //新UI如果cellcount太小，背景纯黑不好看，因此调整cellcount保证大于等于widget width
    void AdjustCellCount();

private:
    // 添加标记线
    void addMarkLine(Constants::emMarkLineType eLineType);

    // 重绘所有标记线
    void redrawAllMarkLine();

    // 绘制矩形选择框
    void drawRectSelect(const QPoint &posTopLeft, const QPoint &posBottomRight);

    // 处理音乐层鼠标左键事件
    void processMusicLayerLeftMouseClicked(QMouseEvent *event);

    // 设置起始单元
    void setStartCellIndex(int nIndex);

    // 根据传入的时间值，获取最新的起始位置index
    // 当传入的时间值落在界面显示第一格时间区域时，自动将左边时间区域向右滚动显示
    // 当传入的时间值落在界面显示最后一格时间区域时，自动将右边时间区域向左滚动显示
    // 当没有落在第一格和最后一个时间区域时，保持当前位置不滚动
    // 函数返回显示窗口新的开始位置起始index
    int getUpdatedStartCellIndex(quint64 nTimeMs, Constants::emMoveDirection eDir);

    // 获取移动方向
    Constants::emMoveDirection getMoveDirection(double dOffset);

private slots:

    // 动作组右键槽函数
    void OnCopy();
    void OnPaste();
    void OnCut();
    void onMergeGroup();
    void OnDeleteSelectGroup();
    void OnSelectedBlockPlay();
    void OnCancelBlockPlay();
    void OnInsertBlankTime();
    void OnSaveToCommandBox();
    void OnModifyGroup();

    // 标记动作帧、动作组
    void slotOnMarkMotionFrame();
    void slotOnMarkMotionGroup();

    // 动作帧右键操作槽函数
    void slotOnModifyFrame();
    void slotOnInsertFrame();
    void slotOnSplitInsertFrame();
    void slotOnCopyFrame();
    void slotOnPaste();
    void slotOnDeleteFrame();
    void slotOnPasteInNewGroup();
    void slotOnAddNewFrameToPreBlock();
    void slotOnCreateNewFrameInNewBlock();

    void on_btnZoomOut_clicked();

    void on_btnZoomIn_clicked();

private:
    Ui::MP3Widget*  m_pMP3WidgetUI;

    CActionBlockDataManager* m_pActionDataManager;
    int m_nCurrentGroupID;
    int m_nCurrentFrameID;
    ILayerOptionHandler_Event* m_pEvent;

    WaveLayer m_waveLayer;//hels 波形绘制对象
    MP3Player* m_pMP3Player;//hels MP3播放类对象
    //QThread* m_pMP3Thread;//hels MP3线程对象
    QString m_strMp3Path;//hels mp3路径
    QString m_strWavPath;
    ActionLayer m_actionLayer;

    QPixmap* m_pPixmapMem;//hels 用于最终显示的内存图，其他的绘制最终都会贴到这张图上
    QPixmap* m_pPixmapGroup;//动作组
    QPixmap* m_pPixmapWave;//波形
    QPixmap* m_pPixmapBackGround;//hels 背景部分位图，保存刻度线等

    //Time 层的宽高
    int m_nTimeCellWidth;
    int m_nTimeCellHeight;

    //Music
    //宽高
    int m_nMusicCellWidth;
    int m_nMusicCellHeight;
    int m_nPixelPerSec;//每1个cell像素大小(默认100像素)

    //底部默认cell
    int m_nCellWidth;//cell宽度
    int m_nCellHeight;//cell高度
    int m_nCellCount;//cell总数
    int m_nLineThick;//cell线宽

    quint64 m_nCellTotalSeconds;//记录原始Cell个数对应的时间s为单位,当进行波形缩放时，需要用到此时间复原cell的正确个数

    //波形控件显示的初始位置
    int m_nStartX;
    int m_nStartY;

    int m_nSelLineLength;//hels 时间选择线的长度
    double m_nCurSelX;//hels 当前选中的时间线位置，以像素为单位，总长度为（m_nCellCount * (m_nCellWidth + m_nLineThick)）

    //hels 一个m_cellWidth + m_nLineThick即为一个cell,与TimeLevel无关
    //hels TimeLevel影响的是波形时间精度，即会影响m_nCount的大小
    int m_nCellStartIndex; //当前显示的第一个cell索引（此值与滚动条的位置有关，用于计算开始绘制的点）

    TimeLevel m_timeLevel;//显示时间精度，表示每个cell代表的时间值

    //MP3起止播放时间，ms为单位
    quint64 m_nStartMilliSecs;
    quint64 m_nEndMilliSecs;

    //
    int m_timeScale;//时间粒度，由Motion模块提供(hwx UI中显示的时间除以时间粒度才是保存的动作帧数据时间)
    int m_nWheelCount;//累计中键滚动2次缩放一次波形比例

    bool m_bForwards;//记录是向前滚动
    bool m_bIsLeftMousePressed; // 记录鼠标左键是否按下

    int m_bIsPlaying;
    int m_bIsSinglePlaying;
    int m_nPasteType;

    double m_nCurSelXStore;
    int m_dragMode;

    emMenuAction m_eCurMenuAction; // 当前选中的右键菜单操作
    Constants::emMouseState m_eMouseState; // 记录鼠标状态

    QPoint    m_oMousePressedPos; // 记录鼠标点击的位置，相对当前widget的坐标
    quint64    m_nMarkLineMovePrePos;  // 标记线移动的上一个时间位置，单位ms

    CMarkLineAdm  m_oMarkLineAdm;  // 动作帧、动作组标记线管理
    CRectSelectDrawer m_oRectSelectDrawer; // 矩形选择框绘制

    // 移动标记线时，只在起始位置和结束位置记录信息
    IOperation *m_pHandleMarkLine;
};

#endif // MP3WIDGET_H
