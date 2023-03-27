#pragma once
#include "afxcmn.h"

#include "../../UBXLanguage/UBXLChs/resource.h"
#include "../../UBXPublic/INC/UBXPublic.h"
#include "ILayerOptionHandler.h"
#include "GridCtrl.h"

#define GRID_COL_RUNTIME  18 //列表运行时间的列索引
#define GRID_COL_ALLTIME  GRID_COL_RUNTIME+1 //列表总时间的列索引

// COptionsMotionDlg 对话框

class CActionBlockGroupManager;
class COptionsMotionDlg : public CDialog, public ILayerOptionHandler
{
        DECLARE_DYNAMIC(COptionsMotionDlg)

public:
    COptionsMotionDlg(QWidget* pParent = NULL);   // 标准构造函数
        virtual ~COptionsMotionDlg();

// 对话框数据
        enum { IDD = IDD_DIALOG_MOTION_OPTION };

private:
        ILayerOptionHandler_Event* m_pEvent;
        bool IsCompensatory;

private:
        void InitGrid();
        void OnVirtualMode();
        void UpdateUIByData();

        void SetCurrentSelectItem(int nItem);
        void UpdateGridByGroupManager(CActionBlockGroupManager* pGroup);
        int GetCurrenGroupManagerID();

        MOTOR_DATA* GetMotoData(char* pData, int nLen, int nID);
        void SetMotoAngle(char* pData, int nLen, int nID, int nAngle);

        /**************************************************************************
        * 函数名: AddToGrid
        * 功能: 添加数据到列表
        * 参数:
        *    @[in ] pData: 数据缓冲区
        *    @[in ] nLen: 数据长度
        *    @[in ] BlockType: 动作类型
        *    @[in ] fRunTime: 已变换的运行时间
        *    @[in ] fAllTime: 已变换的总时间
        *    @[in ] ActionBlockID: 动作id
        * 返回值: void
        * 时间: 2015/08/04 16:29
        * 作者: ZDJ
        */
    void AddToGrid(char* pData, int nLen, QString BlockType, float fRunTime, float fAllTime, int ActionBlockID);
	
        /**************************************************************************
        * 函数名: UpdateGridShow
        * 功能: 更新列表显示
        * 参数:
        *    @[in ] nRow: 行索引
        *    @[in ] pActionBlock: 动作数据
        * 返回值: void
        * 时间: 2015/08/07 13:12
        * 作者: ZDJ
        */
        void UpdateGridShow(int nRow, CActionBlockData* pActionBlock);
        // 显示选中的ID
        void ShowSelectActionBlock();
        void SetShowID(int *pID, int nCount);
        void PlayBlock(int nActionBlockID);
        void ModleView(int nActionBlockID);
        bool IsFullRowSelect(int nRow);
        bool IsGridHasSelectRow();
        int GetSelectRowCount();
        int GetFirstSelectRowID();
    void DoMenuCommand(uint nCommand, QPoint point, int nRow);
        void OnCopy();
        void OnPast();
        void OnInsert();

        CListCtrl m_lstGroupManager;
        CGridCtrl m_Grid;
        CXTPButton m_btnAdd;
        CEdit m_editBeatValue;
        CEdit m_editRecoupValue;
        CXTPButton m_btnDelete;
        CXTPButton m_btnInsert;
        CXTPButton m_btnModify;
        CXTPButton m_btnInsertCompensatory;
        CXTPButton m_btnDisaddembInsert;
        CImageList m_imageList;
        int m_nCurrentSelectItem;
        int m_nCurrentGroupID;
        vector<int> m_ShowId;
        bool m_bLbtnDownDebug;
    uint m_nCol;
        CActionBlockDataManager* m_pGroupManager;
    IUBXRobot3DView* m_pRobotView;

public:
        // ILayerOptionHandler接口实现
    virtual bool Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent);
        virtual void ReleaseConnection();
    virtual QWidget* GetQWidget();
        virtual void SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID);
    virtual void Set3DRobotViewInterface(IUBXRobot3DView* pRobotView);
        virtual void SetControlState(bool controlState);
        virtual void InsertGroupByMenu(int nCol);
        virtual void SetMusicBeatsTime(int nBeatTime) ;
        virtual void AddGroupByMenu() ;
	
protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

        DECLARE_MESSAGE_MAP()
public:
        virtual bool OnInitDialog();
    afx_msg void OnSize(uint nType, int cx, int cy);
        afx_msg void OnBnClickAdd();
        afx_msg void OnBnClickDelete();
        afx_msg void OnBnClickDisassemblyInsert();
        afx_msg void OnBnClickModify();
        afx_msg void OnBnClickInsertCompensatory();
        afx_msg void OnBnClickInsert();
        afx_msg void OnNMClickListMotionList(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnNMRClickGroupList(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
        afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
        afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
        afx_msg void OnRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
        afx_msg void OnChangeEditBeatValue();
        afx_msg void OnEditBeatValueKillFocus();
//	afx_msg void OnCustDrawList(NMHDR *pNMHDR,LRESULT *pResult);
        virtual void OnOK();
        virtual void OnCancel();
        virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(uint_PTR nIDEvent);
};
