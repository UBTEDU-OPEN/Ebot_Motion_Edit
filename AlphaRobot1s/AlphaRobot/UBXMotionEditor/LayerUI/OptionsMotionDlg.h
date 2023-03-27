#pragma once
#include "afxcmn.h"

#include "../../UBXLanguage/UBXLChs/resource.h"
#include "../../UBXPublic/INC/UBXPublic.h"
#include "ILayerOptionHandler.h"
#include "GridCtrl.h"

#define GRID_COL_RUNTIME  18 //�б�����ʱ���������
#define GRID_COL_ALLTIME  GRID_COL_RUNTIME+1 //�б���ʱ���������

// COptionsMotionDlg �Ի���

class CActionBlockGroupManager;
class COptionsMotionDlg : public CDialog, public ILayerOptionHandler
{
        DECLARE_DYNAMIC(COptionsMotionDlg)

public:
    COptionsMotionDlg(QWidget* pParent = NULL);   // ��׼���캯��
        virtual ~COptionsMotionDlg();

// �Ի�������
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
        * ������: AddToGrid
        * ����: ������ݵ��б�
        * ����:
        *    @[in ] pData: ���ݻ�����
        *    @[in ] nLen: ���ݳ���
        *    @[in ] BlockType: ��������
        *    @[in ] fRunTime: �ѱ任������ʱ��
        *    @[in ] fAllTime: �ѱ任����ʱ��
        *    @[in ] ActionBlockID: ����id
        * ����ֵ: void
        * ʱ��: 2015/08/04 16:29
        * ����: ZDJ
        */
    void AddToGrid(char* pData, int nLen, QString BlockType, float fRunTime, float fAllTime, int ActionBlockID);
	
        /**************************************************************************
        * ������: UpdateGridShow
        * ����: �����б���ʾ
        * ����:
        *    @[in ] nRow: ������
        *    @[in ] pActionBlock: ��������
        * ����ֵ: void
        * ʱ��: 2015/08/07 13:12
        * ����: ZDJ
        */
        void UpdateGridShow(int nRow, CActionBlockData* pActionBlock);
        // ��ʾѡ�е�ID
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
        // ILayerOptionHandler�ӿ�ʵ��
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
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
