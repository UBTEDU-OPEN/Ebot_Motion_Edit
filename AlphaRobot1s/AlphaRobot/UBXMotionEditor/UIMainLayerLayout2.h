#ifndef CUIMainLayerLayout_H
#define CUIMainLayerLayout_H

#include "stable.h"
#include "LayerUI/ILayerManager.h"
#include "LayerUI/IUILayerManager.h"


namespace Ui {
class CUIMainLayerLayout;
}

class CUIMainLayerLayout : public QWidget,public ILayerManager, /*public ILayoutTools_Event,*/ public IUILayerManager_Event
{
    Q_OBJECT

public:
    explicit CUIMainLayerLayout(QWidget *parent = 0);
    ~CUIMainLayerLayout();
private:
    Ui::CUIMainLayerLayout *ui;

    //IActionLayerManager 接口实现
        virtual bool Connect(QWidget* widget, ILayerManager_Event* pEvent);
        virtual void ReleaseConnection();
        virtual QWidget* GetQWidget();
        virtual bool GetData(char** ppData, int &nLen);
        virtual bool SetData(char* pData, int nLen);
        virtual void AdjustData() ;
        virtual void LayerManager_GetUnUseLayers(int **pLayerID, int& nLayerCount);
        virtual CString LayerManager_GetLayerName(int nLayerID);
        virtual void LayerManager_AddLayer(int nLayerID);
        virtual void LayerManager_AddMusic() ;
        virtual ILayerUI* LayerManager_GetLayer(int LayerID);
        virtual void LayerManager_ReleaseMusic() ;
        virtual void LayerUIEvent_AddLayerControl(int nLayerType);
        virtual void LayerManager_DeleteLayer(int nLayerID);
        virtual void LayerUIEvent_SetLayOptionHandler(ILayerOptionHandler* pHandler);
        virtual void LayerUIEvent_SetMusicLayerSourceHandler(ILayerOptionHandler* pHandler, int nLayerType
                                                           , void *pData , int nDataLen , int nDataID);
        virtual void LayerUIEvent_DeleteMusciFile(ILayerOptionHandler* pHandler) ;
        virtual void LayerUIEvent_SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID );
        virtual void LayerManager_Set3DRobotViewInterface(IUBXRobot3dView* pRobotView);
        virtual void LayerUIEvent_UpdateScrollBar();
        virtual void LayerUIEvent_SetScrollBarPos(int nPos);
        virtual void LayerUIEvent_OnDeletOptionHandler(ILayerOptionHandler* pHandler);
        virtual void LayerUIEvent_SetScaleValue(int nScale);
        virtual void LayerDebugStop(int layerType);
        virtual void LayerManager_SetTreeView(ITreeNavigation* pView);
        virtual void LayerManager_TreeToView(char *pData, int nLen, CPoint* nPoint, CString FileName);

        virtual void LayerManager_SetTimeScale(int timeScale);
        virtual int LayerManager_GetTimeScaleValue();

        virtual void LayerManager_LayerPlaying(int layerType, ILayerOptionHandler* optionHandle);
        virtual void LayerManager_SetMusicResource(CString strFilePath);
        virtual void LayerManager_StopLayerDebug();
        virtual void LayerManager_LayerStop();
        virtual ILayerOptionHandler* LayerUIEvent_GetOptionHandler();
        virtual void LayerManager_SetSocketSession(CIUBXSocketSession* pSession);

        virtual void LayerManager_UpdateScrollBar();

    public:
        // ILayoutTools_Event
        virtual void OnSrcollChange(int nPos);
        virtual void EnablePlay(int nLayerType ,bool bState) ;

    public:
        // IUILayerManager_Event 接口实现
        virtual void LayerUIEvent_SelectChanger(int nCurrentCol, int nScale);
        //virtual void LayerUIEvent_SelectChanger(int nCurrentCol, int nScale);
        // LayerControlToolsEvent接口实现

//    private:
//        IUILayerManager*	m_pUILayerManager;
//        CUIMainLayerLayoutToolsDlg	m_dlgTools;
//        ILayerManager_Event* m_pEvent;
//        CIUBXSocketSession* m_pSocketSession;
//        int m_StopDebugCount;

};

#endif // CUIMainLayerLayout_H
