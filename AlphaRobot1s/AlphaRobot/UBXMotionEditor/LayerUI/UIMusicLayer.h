#ifndef _UIMUSICLAYER_H_
#define _UIMUSICLAYER_H_

#include "UILayerCommonImpl.h"
#include "UBXWaveAnalyze.h"
#include "MusicChooseDialog.h"
#include "UILayerManager.h"
class CUIMusicLayer: public CUILayerCommonImpl /*, public IMusicChooseEvent*/
{
public:
    CUIMusicLayer(void);
    ~CUIMusicLayer(void);

private:
    int nSelectStartID;
    int nEndId;
    // 块默认设置选择颜色
    COLORREF m_GroupBlockDefSetColor;
    // 块选中设置选择颜色
    COLORREF m_GroupBlockSelSetColor;
protected:
    float  m_fActStartTime;
    float  m_fActEndTime;
    //是否选择播放范围
    bool  m_bSetRange;
    int   h;
    int   nLayerWidth;
    QPen pRedPen, pGreenPen, pBluePen, pBlackPen;
//	CUBXWaveAnalyze      m_waveAnalyze;
//	CMusicChooseDialog   *m_pMusicChooseDlg;
    double               *m_pfWaveArray;
    int                  m_nPixelPerSec;
    double               m_fValueMax;
    double               m_fValueMin;
    int                  m_nBPMValue;
    bool                 m_bDrawBPMLine;

public:
    virtual bool Connect(QWidget* managerHandle);
    virtual bool ReleaseConnection();

    virtual bool GetData(char** ppData, int& nLen);
    virtual bool SetData(char* pData, int nLen);
    // 取得类型，具体见宏定义
    virtual int GetType();
    virtual QString GetLayerName();
    virtual void CreateOptionHandler();
//    virtual void OnDraw(/*CDC* pDC*/);
//    virtual void OnRButtonDdown(int nCol);
//    virtual void OnLButtonDown(int nCol);
//    virtual void OnLButtonDown(int nCol, QPoint point);
// 	virtual void OnLButtonDownDebug() ;
// 	virtual void OnLButtonDbClickDebug() ;
    virtual void DoMenuCommand(uint nCommand, QPoint point, int nCol);
    virtual void ILayerOptionHandler_OnAddActionBlock(int nGroupManagerID, CActionBlockData* pActionBlockData,QString strBlockName = QObject::tr(""), bool bIsUpdateMP3Widget = false);
    virtual void ILayerOptionHandler_OnDeleteDataBlock(int nGroupID);
    virtual void ILayerOptionHandler_OnAddWave(QString strWavePath );
    virtual void ILayerOptionHandler_OnDeleteGroupBlock();
    virtual void ILayerOptionHandler_SetBPMValue(int nBPMValue);
    virtual int  GetCellHeight();
    virtual void deleteSelectMotionBlock();
    virtual void ILayerOptionHandler_OnGroupManagerChange(int nGroupID, float fTimeInc, bool bIsUpdateMp3Widget = false);
    virtual void IMusicChooseEvent_AddMusicLayerBlock(int nGroupManagerID, const QString& musicPath, QString strBlockName = QObject::tr("")) ;
    virtual void IMusicChooseEvent_DeleteMusicLayerBlock() ;
    virtual void IMusicChooseEvent_SetFileModified(bool bState) ;
    virtual void DrawBase( /*CDC* pDC*/ );
//    virtual void OnLButtonDBClick(int nCol);
    void  DrawMusicGroup(/*CDC* pDC*/);
    void  DrawWave(/*CDC* pDC*/);
    void  RemoveWave();
    void  DrawBPMLine(/*CDC *pDC*/);
    void  SetActStartTime(float fActStartTime);
    void  SetActEndTime(float fActEndTime);
    void  PlayMusic(int nActStartTime , int nActEndTime , int nPlayState);
    void  AddMusic(QString strMusicFile , bool bFromAes = true);
    void  PauseMusic();
    void  StopMusic();
    int   GetCurrentPos();
    float GetActStartTime();
    float GetActEndTime();
    float GetStartTime();
    float GetEndTime();
    bool  AddWave(QString strWavePath);
    void  WaveSmooth(double *fArray , int nArrayCount , double fCmpValue , double fWeight);
    void  OnAddActionBlock(int nGroupManagerID, CActionBlockData* pActionBlockData,QString strBlockName );
    void  wcharToChar (const WCHAR * tchar, char **_char);
    void  SetMusicActionExtData(CActionBlockData *pActionData , const QString &path);


    virtual void  GetMusicDlgPtr(/*CMusicChooseDialog *pDlg*/);
    virtual WCHAR* GetActionDescription(int nGroupIndex , int nActionIndex);
    virtual void GetActionSongPath(int nGroupIndex , int nActionIndex, QString& strSongPath);

    //从ActionData的扩展数据中获取音乐文件名并添加
    virtual bool AddMusicFromExtData(int nGroupIndex , int nActionIndex);
    //从Music目录下获取第一个音乐文件并添加
    virtual bool AddMusicFromMusicDir();

    //hels MusicLayer实现，此接口会由MP3模块调用，通知Motion模块做相应的操作
    virtual void ILayerOptionHandler_OnLButtonDown(int nCol, QPoint point, float fStartTime);//切换选中组
    virtual void ILayerOptinHandler_OnLButtonDBClick(int nCol);//双击播放动作    
    virtual void ILayerOptionHanlder_OnStopPlay();//停止动作播放，时间线会归位到0
    virtual void ILayerOptionHanlder_LayerPlaying();//点击Simulate按钮时触发，通知Motion模块仿真
    virtual void ILayerOptionHanlder_LayerPause();//暂停仿真,跟停止略有不同，暂停不会重置时间线的位置
    virtual void ILayerOptinHandler_FinishMusicPlaying();//音乐播放完成时通知
    virtual void ILayerOptionHandler_MusicLoaded(const QString& path, const QString& title, int time);//音乐加载完成时通知，Motion模块更新名称和时长,时间单位为s
    virtual void ILayerOptionHandler_UpdateMusicPosition(quint64 position);//音乐播放的位置时间单位为ms
    virtual void ILayerOptionHandler_DeleteMusic();//删除音乐时调用

    //hels MusicLayer实现，此接口会由MP3模块点击界面时，调用过来,以便Motion模块更新
    virtual void ILayerOptionHandler_OnRButtonDown(int nCol, QPoint point);

     //hels MusicLayer实现，此接口会由MP3模块右键选择菜单后，调用过来,以便Motion模块更新
    virtual void Ilayeroptionhandler_MotionlayerActionCommand(uint nCommand, int nCol);

    virtual void Ilayeroptionhandler_TreeToView(char *pData, int nLen, int nCol, QString FileName);

    virtual void ILayerOptionHandler_SetMusicModifiedState(bool bState);
    virtual bool ILayerOptionHandler_GetMusicModifiedState();

    virtual bool ILayerOptionHandler_hasCopyFrame() Q_DECL_OVERRIDE;

};
#endif  //_UIMUSICLAYER_H_
