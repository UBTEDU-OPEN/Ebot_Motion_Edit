#ifndef _UIMOTIONLAYER_H_
#define _UIMOTIONLAYER_H_

#include "UILayerCommonImpl.h"
#include "QRgb"
/************************************************************************/
/* 动作层UI                                                                     */
/************************************************************************/
class CUIMotionLayer:public CUILayerCommonImpl
{
public:
    CUIMotionLayer(void);
    ~CUIMotionLayer(void);

private:
//	CActionBlockDataManager* m_pBlockDataManager;

public:
    virtual bool Connect(QWidget* managerHandle);
    virtual bool ReleaseConnection();

    virtual bool GetData(char** ppData, int& nLen);
    virtual bool SetData(char* pData, int nLen);
    // 取得类型，具体见宏定义
    virtual int GetType();
    virtual QString GetLayerName();
    virtual void CreateOptionHandler();
// 	virtual	void SetPosition(int PositionX, int PositionY);
// 	virtual void GetLayerWidth(int &LayerWidth);
    virtual void OnLButtonDown(int nCol, QPoint point, float fStartTime);
    virtual void OnRButtonDdown(int nCol);
    virtual void GetSelectGroupAction(int nCol,int &nGroupIndex , int &nActionIndex , int &nTimeOffset);
    virtual void GetSelectGroupAction(int nCol,int &nGroupIndex , int &nActionIndex , int &nTimeOffset, float fStartTime);
    virtual void AddGroupFromMenu();
    virtual void DrawBlockAction(/*CDC *pDC*/);
    virtual void OnLButtonDownDebug(int nCol) ;
    virtual void OnLButtonDbClickDebug(int nCol) ;
    virtual void OnLButtonDBClick(int nCol);
    virtual void SetSelectGroupByOthers(int nCol);
    virtual void OnDraw(/*CDC* pDC*/);
    virtual void ILayerOptionHandler_OnSelectActionManageChange(int nGroupID ,int nActionIndex);
    virtual void ILayerOptionHandler_SetActionBlockState(bool bState);
    virtual void PlayEvent_OnSetPosChangeToOptions(int nCol);
    virtual void DoMenuCommand(uint nCommand, int nCol);
//	static VOID CALLBACK TimerProc(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime);
public:
    bool m_bLtnDownDebug;
    COLORREF m_ActionBlockSelColor;
private:
    //是否绘制单帧底色
    bool m_bDrawBlockActiion;
    QWidget* m_managerHandle;
};
#endif  //_UIMOTIONLAYER_H_
