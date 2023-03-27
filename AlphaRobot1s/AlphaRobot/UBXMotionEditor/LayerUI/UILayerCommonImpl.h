#ifndef _UILAYERCOMMONIMPL_H_
#define _UILAYERCOMMONIMPL_H_

#include "ILayerUI.h"
#include "ILayerOptionHandler.h"
#include "IGroupDataPlay.h"
#include "ubxundoredomanager.h"

#define ADD_OFFSET  5
typedef ulong COLORREF;

class QMimeData;

class CUILayerCommonImpl: public ILayerUI, public ILayerOptionHandler_Event, public IGroupDataPlayEvent
{
public:
    CUILayerCommonImpl(void);
    ~CUILayerCommonImpl(void);

protected:
    //int m_lineThick;
    COLORREF m_lineColor;
    int x, y;						//起始坐标
    int h;
    int m_divisor;
    int m_HScrollOffset;

    //int m_scaleValue;
    uint m_CellStartIndex;
    //int m_CellCount;

    QString m_fontFamily;	//字体名称
    int m_fontSize;			//字体大小
    COLORREF m_fontColor;	//字体颜色

    // 块默认颜色
    COLORREF m_GroupBlockDefColor;
    // 块选中颜色
    COLORREF m_GroupBlockSelColor;

    QList<int>	m_GroupSelectID;

    // 用于保存数据用
    CActionBlockDataManager* m_pBlockDataManager;
    ILayerOptionHandler*	m_pOptionHandler;

    IUILayerManager*	m_pLayerManager;
    IGroupDataPlay*	m_pGroupDataPlay;

    QWidget* m_ManagerHandle;
    bool m_IsPlaying;
    bool m_IsSinglePlaying;
    bool m_bMouseDown;
    ILibTreeNavigation* m_treeView;
    bool m_IsDragMouse;
    int m_positionDragMouse;
    bool m_IsHaveCopyData;
//    uint m_nPasteType;   // 0-no paste  1-Paste after copy 2-paste after cut
    int GetNewActionBalockDataID();
    bool IsSelectGroupBlock(int nID);
    void ClearSelectGroupBlock();
    void SetSelectGroupBlock(int nCol);
    int GetSelectGroupAction(int nCol);
    int GetSelectGroupBlock();
    // 动态调整时间
    void AdjustAllGroupTime(CActionBlockGroupManager* pManager, float nAdd);
    void MoveAllGourpTime(CActionBlockGroupManager* pManager, float fOffset);
    void onInsertMotionBlock(int nCol);
    void AjustTimeForInsert(CActionBlockGroupManager* pManager, float fAdd);

    void deleteSelectMotionBlock(bool bCopyMotion = true);
    void onCopyMotionBlock();
    void onPasteMotionBlock(int nCol);
    void onPlaySelectMotionBlock();
    void InsertBlankTime(int nCol);
    void onSaveMotionBlockToTemplate();
    QString GetModuleDir();

    void DrawBase( /*CDC* pDC*/ );
    void DrawgBlockGroup(/*CDC* pDC*/);
    virtual void SetLayerManager(IUILayerManager* pManager);
    virtual IUILayerManager* GetLayerManager();
    virtual CActionBlockDataManager* GetBlockDataManager(){return m_pBlockDataManager;}
    //hels更新MP3选中组,参数为组ID和Action对应的时间，由MP3模块算出时间选择线位置并更新
    void UpdateMP3Action(int nGroupID, float fStartTime);

    // 粘贴动作帧到新建动作组
    void pasteActionFrameToNewGroup();

public:
    // ILayerOptionHandler_Event接口实现
    virtual void ILayerOptionHandler_OnAddActionBlock(int nGroupManagerID, CActionBlockData* pActionBlockData, QString strBlockName = QObject::tr(""), bool bIsUpdateMP3Widget = false);
    virtual void ILayerOptionHandler_OnSelectGroupManageChange(int nGroupID, int nActionIndex);
    virtual void ILayerOptionHandler_OnSelectActionManageChange(int nGroupID,int nActionIndex);
    //设置
    //virtual void ILayerOptionHandler_SetActionBlockState(bool bState){};
    virtual void ILayerOptionHandler_OnActionBlockDataChange(int nGroupID, int nActionBlockID, float fTimeInc);
    virtual void ILayerOptionHandler_OnGroupManagerChange(int nGroupID, float fTimeInc, bool bIsUpdateMp3Widget = false);
    virtual int  ILayerOptionHandler_GetScaleValue();
    // 设置时间粒度值
    virtual void ILayerOptionHandler_SetScaleValue(int nScale) ;
    virtual void ILayerOptionHandler_OnDeleteGroupBlock(bool bCopyMotion = true) Q_DECL_OVERRIDE;
    //双击块仿真
    virtual void ILayerOptionHandler_OnDataBlockPlaying(CActionBlockData *pActionData) ;
    virtual void ILayerOptionHandler_ModifyGroupName(int nCol);
    virtual void ILayerOptionHandler_CopyGroup();
    virtual void ILayerOptionHandler_PasteGroup(int nCol);

    virtual void ILayerOptionHandler_SetSelectGroupBlock(int nCol);
    //设置当前文件的修改状态（通过opTions实现）
    virtual void ILayerOptionHandler_SetModifiedState(bool bState) ;
    virtual void ILayerOptionHandler_InsertGroupByMenu(CActionBlockData* pActionBlockData, int nCol);

    //hels 供Motion模块调用,并通知MP3模块响应操作
    virtual void ILayerOptionHanlder_UnitedSimulation();//开始联合仿真
    virtual void ILayerOptionHanlder_PauseUnitedSimulation();//暂停联合仿真
    virtual void ILayerOptionHanlder_StopUnitedSimulation();//停止联合仿真
    virtual void ILayerOptionHandler_HasMotionData(bool hasData);//通知MP3是否有动作帧，以便刷新按钮状态
    virtual bool ILayerOptionHandler_IsPlaying();

    virtual void ILayerOptionHandler_SetMusicModifiedState(bool bState);
    virtual bool ILayerOptionHandler_GetMusicModifiedState();

    virtual IUILayerManager *ILayerOptionHandler_GetLayerManager() override;
    virtual emPlayType ILayerOptionHandler_GetPlayType() Q_DECL_OVERRIDE;

public:
    virtual bool Connect(QWidget* managerHandle);
    virtual bool ReleaseConnection();
    virtual void ClearSelectState();
    virtual void OnLButtonDBClick(int nCol);
    virtual void OnLButtonDown(int nCol, QPoint point, float fStartTime);
    virtual void OnLButtonDownAndShift(int nCol);
    virtual void OnLButtonUpAndShift();
    virtual void OnLButtonUp(QPoint point);
    virtual void OnRButtonDdown(int nCol);
    virtual void OnMouseMove(QPoint point);
    virtual void AddBlankBlockInTail();
    virtual void GetSelectGroupAction(int nCol,int &nGroupIndex , int &nActionIndex , int &nTimeOffset){};
    virtual void AddGroupFromMenu() {};
    virtual void SetSelectGroupByOthers(int nCol){};
    virtual void OnLButtonDownDebug(int nCol) {};
    virtual void OnLButtonDbClickDebug(int nCol) {};
    virtual void DoMenuCommand(uint nCommand, int nCol);

    //绘自己
    virtual void OnDraw(/*CDC* pDC*/);

    virtual bool GetData(char** ppData, int& nLen);
    virtual bool SetData(char* pData, int nLen);
    virtual void AdjustData() ;
    // 取得类型，具体见宏定义
    virtual int GetType();
    // 设置起始X,Y的位置
    virtual	void SetPosition(int PositionX, int PositionY);
    virtual void GetPosition(int& x, int& y);
    virtual void SetHScrollOffset(int offset);
    virtual void SetScalevalue(int scaleValue);
    virtual int  GetScaleValue();
    virtual void SetCellStartIndex(uint CellIndex);
    virtual uint GetCellStartIndex();
    virtual int  GetScreenCellCount();
    virtual void GetLayerWidth(int &LayerWidth);
    virtual int  GetCellCount();
    virtual void SetCellWidth(int width);
    virtual int  GetCellWidth();
    virtual void SetCellHeight(int height);
    virtual int  GetCellHeight();
    virtual void SetLineThick(int nThick);
    virtual int  GetLineThick();
    virtual void CreateOptionHandler();
    virtual ILayerOptionHandler* GetOptionHandler();
    virtual int  GetMaxTimeCellCount();
    virtual void PlayEvent_OnDebugStop(int nLayerType, int nPlayStatus) Q_DECL_OVERRIDE;
    virtual void SetLayerMenuState(bool menuState);
    virtual void onCancelPlayMotionBlock();
    virtual void SetTreeView(ILibTreeNavigation* pView);
    virtual void TreeToView(char *pData, int nLen, int nCol, QString FileName);
private:
    void createLayoutMenuPopMenuActions();
    bool CreateActionDir(QString &strPath);

    // 获取经过限制后的运行时间
    float getAllTime(float fStartTime, float fEndTime);

    // 拷贝修改前、后的动作数据
    void  cloneBeforeChangedMotionData(IOperation *pHandle);
    void  cloneAfterChangedMotionData(IOperation *pHandle);

    // 获取包含指定时间粒度的动作组
    CActionBlockGroupManager *getActionBlockGroupMgr(quint64 nTimeCol);

    // 获取拷贝动作组数据
    CActionBlockGroupManager *getCopyBlockGroupData(float fStartTime);

    // 获取拷贝动作帧数据
    void getActionFrameData(QMimeData *pMimeData, QList<CActionBlockData *> &lstActionFrames);

    // 粘贴动作组数据到当前动作组前
    void pasteMotionGroupToPreBlock(CActionBlockGroupManager *pCurActionGroupMgr, CActionBlockGroupManager *pNewActionGroupMgr);

    // 粘贴动作组到末尾
    void pasteMotionGroupToEnd(CActionBlockGroupManager *pNewActionGroupMgr);

    // 获取当前选中的时间，以粒度为单位
    quint64 getCurrentSelTimeCol();

private:
    int m_nCurCol;

    QMimeData* m_pMimeData;

};
#endif  //_UILAYERCOMMONIMPL_H_
