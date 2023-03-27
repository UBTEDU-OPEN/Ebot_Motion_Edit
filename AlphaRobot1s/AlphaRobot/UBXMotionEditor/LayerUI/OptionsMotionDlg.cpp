// OptionsMotionDlg.cpp : 实现文件
//

#include "stable.h"
#include "UBXMotionEditor.h"
#include "OptionsMotionDlg.h"
#include "afxdialogex.h"
#include "ActionBlockDataManager.h"
#include "../../UBXPublic/INC/UBXPublic.h"

#define GRID_CTRL_ID	100
#define GRID_COLUMN_COUNT 20 //列表列数

bool CreateMotionLayOptionHandler(ILayerOptionHandler** ppHandler)
{
        /*CAutolockRes res;*/
        COptionsMotionDlg* pProxy = new COptionsMotionDlg();
        if (pProxy)
        {
                *ppHandler = pProxy;

                return true;
        }

        return false;
}
// COptionsMotionDlg 对话框

IMPLEMENT_DYNAMIC(COptionsMotionDlg, CDialog)

COptionsMotionDlg::COptionsMotionDlg(QWidget* pParent /*=NULL*/)
        : CDialog(COptionsMotionDlg::IDD, pParent)
        , m_pEvent(NULL)
        , m_pGroupManager(NULL)
        , m_pRobotView(NULL)
        , m_nCurrentSelectItem(-1)
        , m_nCurrentGroupID(-1)
        , IsCompensatory(false)
{
         m_bLbtnDownDebug = false;
        m_nCol = 0;
}

COptionsMotionDlg::~COptionsMotionDlg()
{
}

void COptionsMotionDlg::DoDataExchange(CDataExchange* pDX)
{
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_LIST_MOTION_LIST, m_lstGroupManager);
        DDX_Control(pDX, IDC_BTN_MOTION_OPTION_ADD, m_btnAdd);
        DDX_Control(pDX, IDC_EDIT_BEAT_VALUE, m_editBeatValue);
	
        DDX_Control(pDX, IDC_BTN_MOTION_OPTION_DELETE, m_btnDelete);
        DDX_Control(pDX, IDC_BTN_MOTION_OPTION_INSERT, m_btnInsert);
        DDX_Control(pDX, IDC_BTN_MOTION_OPTION_MODIFY, m_btnModify);
        DDX_Control(pDX, IDC_BUTTON_INSERT_COM, m_btnInsertCompensatory);
        DDX_Control(pDX, IDC_EDIT_INSERT_RECOUP, m_editRecoupValue);
        DDX_Control (pDX, IDC_BUTTON_INSERT_DISASSEMB, m_btnDisaddembInsert);
}


BEGIN_MESSAGE_MAP(COptionsMotionDlg, CDialog)
        ON_WM_SIZE()
        ON_BN_CLICKED(IDC_BTN_MOTION_OPTION_ADD, OnBnClickAdd)
        ON_BN_CLICKED(IDC_BTN_MOTION_OPTION_DELETE, OnBnClickDelete)
        ON_BN_CLICKED(IDC_BTN_MOTION_OPTION_INSERT, OnBnClickInsert)
        ON_BN_CLICKED(IDC_BTN_MOTION_OPTION_MODIFY, OnBnClickModify)
        ON_BN_CLICKED(IDC_BUTTON_INSERT_COM, OnBnClickInsertCompensatory)
        ON_BN_CLICKED(IDC_BUTTON_INSERT_DISASSEMB, OnBnClickDisassemblyInsert)
        ON_NOTIFY(NM_CLICK, IDC_LIST_MOTION_LIST, &COptionsMotionDlg::OnNMClickListMotionList)
        ON_NOTIFY(NM_RCLICK, IDC_LIST_MOTION_LIST, &COptionsMotionDlg::OnNMRClickGroupList)
        ON_NOTIFY(GVN_ENDLABELEDIT, GRID_CTRL_ID, OnGridEndEdit)
        ON_NOTIFY(NM_DBLCLK, GRID_CTRL_ID, OnGridDblClick)
        ON_NOTIFY(NM_CLICK, GRID_CTRL_ID, OnGridClick)
        ON_NOTIFY(NM_RCLICK, GRID_CTRL_ID, OnRClick)
        ON_EN_CHANGE(IDC_EDIT_BEAT_VALUE, &COptionsMotionDlg::OnChangeEditBeatValue)
        ON_EN_KILLFOCUS(IDC_EDIT_BEAT_VALUE, &COptionsMotionDlg::OnEditBeatValueKillFocus)
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_MOTION_LIST, OnCustDrawList)
        ON_WM_TIMER()
END_MESSAGE_MAP()



void COptionsMotionDlg::OnEditBeatValueKillFocus()
{
        if (m_pEvent == NULL)
        {
                return;
        }
    QString strValue;
        //m_editBeatValue.GetDlgItemText()
        m_editBeatValue.GetWindowTextW(strValue);
        float fValue = _wtof(strValue.GetBuffer());
        int nScale = m_pEvent->ILayerOptionHandler_GetScaleValue();
        if (fValue > GRID_MAX_TIMEVALUE)
        {
                fValue = GRID_MAX_TIMEVALUE;
        QString str;
        str.Format(QObject::tr("%d"),(int)fValue);
                m_editBeatValue.SetWindowText(str);
        }
        else if (fValue < GRID_MIN_TIMEVALUE)
        {
                fValue = GRID_MIN_TIMEVALUE;
        QString str;
        str.Format(QObject::tr("%d"),(int)fValue);
                m_editBeatValue.SetWindowText(str);
        }
}
void COptionsMotionDlg::OnChangeEditBeatValue()
{
// 	if (m_pEvent == NULL)
// 	{
// 		return;
// 	}
// 	QString strValue;
// 	//m_editBeatValue.GetDlgItemText()
// 	m_editBeatValue.GetWindowTextW(strValue);
// 	float fValue = _wtof(strValue.GetBuffer());
// 	int nScale = m_pEvent->ILayerOptionHandler_GetScaleValue();
// 	if (fValue > GRID_MAX_TIMEVALUE)
// 	{
// 		fValue = GRID_MAX_TIMEVALUE;
// 		QString str;
// 		str.Format(QObject::tr("%d"),fValue);
// 		m_editBeatValue.SetWindowText(str);
// 	}

}
bool COptionsMotionDlg::Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent)
{
        CAutolockRes res;
        if (Create(COptionsMotionDlg::IDD, pParent))
        {
                m_pEvent = pEvent;
                return true;
        }

        return false;
}

void COptionsMotionDlg::ReleaseConnection()
{
        m_ShowId.clear();
        if (GetSafeHwnd())
        {
                DestroyWindow();
        }

        delete this;
}

QWidget* COptionsMotionDlg::GetQWidget()
{
        return this;
}

void COptionsMotionDlg::SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID)
{
        bool IsUpdata = true;
        if (m_nCurrentGroupID == nCurrentGroupID)
        {
                IsUpdata = false;
        }
        if (nCurrentGroupID == 0)
        {
                m_nCurrentGroupID = nCurrentGroupID;
                m_pGroupManager = pManager;
        }
        if (IsUpdata || g_IsModifyGroup)
        {
                UpdateUIByData();
        }
	
        m_nCurrentGroupID = nCurrentGroupID;
        m_pGroupManager = pManager;

        if (nCurrentGroupID == -1)
        {
                m_editBeatValue.EnableWindow(false);

                m_nCurrentSelectItem = -1;
                m_ShowId.clear();
        }
        else
        {
                for (int i=0; i<m_lstGroupManager.GetItemCount(); i++)
                {
                        if (m_lstGroupManager.GetItemData(i) == nCurrentGroupID)
                        {
                                m_nCurrentSelectItem = i;
                                break;
                        }
                }
        }

        if (IsUpdata || g_IsModifyGroup)
        {
                SetCurrentSelectItem(m_nCurrentSelectItem);
                g_IsModifyGroup = false;
        }
	 
        if (nCurrentActionID != -1 && m_nCurrentSelectItem != -1)
        {
                int nActionID = 0;
// 		if (pManager->GetAt(m_nCurrentSelectItem)->GetCount() <= nCurrentActionID)
// 		{
// 			return;
// 		}
                if (pManager)
                {
                        CActionBlockData *pData = pManager->GetAt(m_nCurrentSelectItem)->GetAt(nCurrentActionID);
                        if (pData)
                        {
                                nActionID = pData->GetID();
                                SetShowID(&nActionID,1);
                                ShowSelectActionBlock();
                        }
                }
        }
 	
}

void COptionsMotionDlg::OnVirtualMode()
{
        UpdateData();
        m_Grid.SetVirtualMode(false);
        m_Grid.SetEditable(true);

        //m_bHeaderSort = m_Grid.GetHeaderSort();
        int m_nFixCols = 1;
        int m_nFixRows = 1;
        //m_nCols = 6;
        //m_nRows = 16;

        m_Grid.SetAutoSizeStyle();

        TRY {
                m_Grid.SetRowCount(1);
                m_Grid.SetColumnCount(GRID_COLUMN_COUNT);
                m_Grid.SetFixedRowCount(m_nFixRows);
                m_Grid.SetFixedColumnCount(m_nFixCols);
        }
        CATCH (CMemoryException, e)
        {
                e->ReportError();
                return;
        }
        END_CATCH

        // fill rows/cols with text
        for (int row = 0; row < m_Grid.GetRowCount(); row++)
        {
                for (int col = 0; col < m_Grid.GetColumnCount(); col++)
                {
            QString str;
                        GV_ITEM Item;
                        Item.mask = GVIF_TEXT;
                        Item.row = row;
                        Item.col = col;
                        //Item.nFormat = DT_CENTER|DT_WORDBREAK|DT_VCENTER;
                        if (row < m_nFixRows)
                        {
                str.Format(QObject::tr("ID %d"),col-1);
                                if (col == 1)
                                        str = L"Type";
                                else if (col == 0)
                                        str = L"";
                                else if (col == 18)
                                        str = LANGUAGE_LOAD(ID_ACTLIST_HEADER_RUNTIME) + L"(ms)"; //运行时间
                                else if(col == 19)
                                        str = LANGUAGE_LOAD(ID_ACTLIST_HEADER_ALLTIME) + L"(ms)"; //总时间

                        }
                        else if (col == m_nFixCols)
                        {
                str.Format(QObject::tr("[%d]"), row);
                        }
                        else
                        {
                str.Format(QObject::tr("%d"),row*col);
                        }
                        Item.strText = str;
                        m_Grid.SetItem(&Item);
                        m_Grid.SetItemFormat(Item.row,Item.col,DT_CENTER);
                        //m_Grid.set
                }
        }

        // Having weird Autosize problems with CE 2.11 - possibly just an emulation problem
#if !defined(_WIN32_WCE) || (_WIN32_WCE < 211)
        m_Grid.AutoSize();
#endif

        //UpdateMenuUI();
        UpdateData(false);
}

void COptionsMotionDlg::InitGrid()
{
        m_Grid.SetEditable(true);
        m_Grid.SetTextBkColor(RGB(250, 250, 220)/*RGB(152, 185, 158)*//*(250, 250, 175)*/);//黄色背景
// 	m_Grid.SetGridLineColor(RGB(0, 0, 0));
        m_Grid.SetGridBkColor(RGB(250, 250, 220));
        m_Grid.SetFixedBkColor(RGB(179, 120, 116));
//	m_Grid.SetFixedHeight(0, 15);
//	m_Grid.SetDefaultCellType(RUNTIME_CLASS(LVCFMT_CENTER));
        OnVirtualMode();
}

bool COptionsMotionDlg::OnInitDialog()
{
        __super::OnInitDialog();

        DWORD dwStyle = m_lstGroupManager.GetExtendedStyle();
        dwStyle |= LVS_EX_FULLROWSELECT;// 选中某行使整行高亮（只适用与report风格的listctrl）
        dwStyle |= LVS_EX_GRIDLINES;    // 网格线（只适用与report风格的listctrl）
        dwStyle |= LVS_EX_SUBITEMIMAGES;// 填充图片
        dwStyle |= LVS_SHOWSELALWAYS;
        //dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
        m_lstGroupManager.SetExtendedStyle(dwStyle); //设置扩展风格

        m_lstGroupManager.SetTextBkColor(RGB(250, 250, 220));
        m_lstGroupManager.SetBkColor(RGB(250, 250, 220));
        CRect rectThisWindow;
        GetClientRect(rectThisWindow);
        m_lstGroupManager.InsertColumn( 0, language_loadstring(IDS_OPTIONSDLG_GROUPLIST_ACTIONLIST), LVCFMT_LEFT, rectThisWindow.Width() );//插入列
        m_imageList.Create(16, 16, ILC_COLOR24 ,5,1);
        m_lstGroupManager.SetImageList(&m_imageList, LVSIL_SMALL);
        m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_ACCEPT));
        m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_ACCEPT));

        CRect rect;
        rect.left = 100;
        rect.right = 200;
        rect.top = 0;
        rect.bottom = 200;

        m_Grid.Create(rect, this, GRID_CTRL_ID);
        InitGrid();

        m_editBeatValue.SetWindowTextW(L"200");
        m_editBeatValue.EnableWindow(false);
        m_editRecoupValue.SetWindowTextW(L"50");

        m_btnInsertCompensatory.ShowWindow(false);
        m_editRecoupValue.ShowWindow(false);

        return true;  // return true unless you set the focus to a control
        // 异常: OCX 属性页应返回 false
}


void COptionsMotionDlg::OnSize(uint nType, int cx, int cy)
{
        __super::OnSize(nType, cx, cy);

        if (m_lstGroupManager.GetSafeHwnd() == NULL || m_Grid.GetSafeHwnd() == NULL)
                return;

#define DIFF_SIZE 5

        CRect rcBtn;
        CRect rcStatic;
        CRect rcEdit;

        m_btnAdd.GetClientRect(rcBtn);

        m_lstGroupManager.SetWindowPos(NULL, 0, 0, cx/DIFF_SIZE, cy, SWP_SHOWWINDOW);
        m_Grid.SetWindowPos(NULL, cx/DIFF_SIZE+2, rcBtn.Height(), cx-(cx/DIFF_SIZE - 2), cy-rcBtn.Height(), SWP_SHOWWINDOW);

        m_btnAdd.SetWindowPos(NULL, cx/DIFF_SIZE+2, /*cy-rcBtn.Height()-2*/0, rcBtn.Width(), rcBtn.Height(), SWP_SHOWWINDOW);
	
        GetDlgItem(IDC_STATIC_BEAT_VALUE)->GetClientRect(rcStatic);
        GetDlgItem(IDC_STATIC_BEAT_VALUE)->SetWindowPos(NULL, cx/DIFF_SIZE+5+rcBtn.Width(), 4, 0, 0, SWP_NOSIZE);
        m_editBeatValue.GetClientRect(rcEdit);
        m_editBeatValue.SetWindowPos(NULL, cx/DIFF_SIZE+2+rcBtn.Width()+rcStatic.Width()+6, 0, 0, 0, SWP_NOSIZE);

        m_btnDelete.SetWindowPos(NULL, cx/DIFF_SIZE+2+rcBtn.Width()+2+rcEdit.Width()+rcStatic.Width()+35, 0, rcBtn.Width(), rcBtn.Height(), SWP_SHOWWINDOW);
        m_btnInsert.SetWindowPos(NULL, cx/DIFF_SIZE+2+(rcBtn.Width()+2)*2+rcEdit.Width()+rcStatic.Width()+35, 0, rcBtn.Width(), rcBtn.Height(), SWP_SHOWWINDOW);
        m_btnDisaddembInsert.SetWindowPos(NULL, cx/DIFF_SIZE+2+(rcBtn.Width()+2)*3+rcEdit.Width()+rcStatic.Width()+35, 0, rcBtn.Width() + 20, rcBtn.Height(), SWP_SHOWWINDOW);
        m_btnModify.SetWindowPos(NULL, cx/DIFF_SIZE+2+(rcBtn.Width()+2)*4+rcEdit.Width()+rcStatic.Width()+35 + 20, 0, rcBtn.Width(), rcBtn.Height(), SWP_SHOWWINDOW);
//	m_btnInsertCompensatory.SetWindowPos(NULL, cx/DIFF_SIZE+2+(rcBtn.Width()+2)*4+rcEdit.Width()+rcStatic.Width()+10, cy-rcBtn.Height()-2, rcBtn.Width(), rcBtn.Height(), SWP_SHOWWINDOW);
//	m_editRecoupValue.SetWindowPos(NULL, cx/DIFF_SIZE+2+(rcBtn.Width()+2)*5+rcEdit.Width()+rcStatic.Width()+10, cy-rcBtn.Height()-2, 0, 0, SWP_NOSIZE);
}

void COptionsMotionDlg::UpdateUIByData()
{
        if (m_pGroupManager == NULL)
                return;

        if ((m_lstGroupManager.GetItemCount() == m_pGroupManager->GetCount()) && !g_IsModifyGroup)
        {
                m_lstGroupManager.SetItem(m_nCurrentSelectItem, 0, LVIF_IMAGE, NULL, -1, 0, 0, 0);
                m_lstGroupManager.Update(m_nCurrentSelectItem);
                return;
        }

        m_lstGroupManager.LockWindowUpdate();
        m_lstGroupManager.DeleteAllItems();

        for (int i=0; i<m_pGroupManager->GetCount(); i++)
        {
                CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(i);
                int nItem = m_lstGroupManager.InsertItem(m_lstGroupManager.GetItemCount(), pGroup->GetDescription(), -1);
                m_lstGroupManager.SetItemData(nItem, pGroup->GetID());
        }

        m_lstGroupManager.EnsureVisible(m_nCurrentSelectItem, false);
        m_lstGroupManager.UnlockWindowUpdate();
}

void COptionsMotionDlg::SetCurrentSelectItem(int nItem)
{
        if (m_nCurrentSelectItem != -1)
        {
                m_lstGroupManager.SetItem(m_nCurrentSelectItem, 0, LVIF_IMAGE, NULL, -1, 0, 0, 0);
        }

        if (nItem == -1)
        {
                if (m_Grid.GetSafeHwnd())
                {
                        m_Grid.LockWindowUpdate();
                        m_Grid.DeleteNonFixedRows();
                        m_Grid.UnlockWindowUpdate();
	
                        m_Grid.Refresh();
                }
                return;
        }

        m_lstGroupManager.SetItem(nItem,0, LVIF_IMAGE, NULL, 0, 0, 0, 0);
        m_nCurrentSelectItem = nItem;

        if (m_pGroupManager)
        {
                CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(nItem);
                if (pGroup)
                {
                        char* pdata = NULL;
                        int nLen = 0;
                        pGroup->GetExtBlockData(&pdata, nLen);
                        delete[] pdata;
                        if (nLen == 8 || nLen == 14)
                        {
                                IsCompensatory = true;
                        } else {
                                IsCompensatory = false;
                        }

                        m_nCurrentGroupID = pGroup->GetID();
                        UpdateGridByGroupManager(pGroup);
                }
        }
}

void COptionsMotionDlg::UpdateGridByGroupManager(CActionBlockGroupManager* pGroup)
{
        m_Grid.LockWindowUpdate();
        m_Grid.DeleteNonFixedRows();

        for (int i=0; i<pGroup->GetActionCount(); i++)
        {
                CActionBlockData* pBlockData = pGroup->GetAt(i);
                if (pBlockData)
                {
                        char* pData = NULL;
                        int nLen = 0;

                        pBlockData->GetExtBlockData(&pData, nLen);
                        if (pData && nLen)
                        {
                                AddToGrid(pData, nLen, pBlockData->GetDescription(), pBlockData->GetRunTime(), pBlockData->GetAllTime(), pBlockData->GetID());

                                delete [] pData;
                        }
                }
        }
        m_Grid.UnlockWindowUpdate();
        m_editBeatValue.EnableWindow(true);
}

void COptionsMotionDlg::SetShowID(int *pID, int nCount)
{
        m_ShowId.clear();

        for(int i=0; i<nCount; i++)
        {
                m_ShowId.push_back(pID[i]);
        }
}

void COptionsMotionDlg::ShowSelectActionBlock()
{
        int nShowCount = 0;
        for (int i=0; i<m_Grid.GetRowCount(); i++)
        {
                int nID = m_Grid.GetItemData(i, 0);
                for (int j=0; j<m_ShowId.size(); j++)
                {
                        if (nID == m_ShowId[j])
                        {
                                nShowCount++;
                                m_Grid.SetSelectedRange(i, 0, i, GRID_COLUMN_COUNT - 1);

                                m_Grid.EnsureVisible(i, 0);

                                if (nShowCount>=m_ShowId.size())
                                        return;
                        }
                }
        }
}

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
void COptionsMotionDlg::AddToGrid(char* pData, int nLen, QString BlockType, float fRunTime, float fAllTime, int ActionBlockID)
{
    QString strHeader;

        int nLens = 0;
        memcpy(&nLens, pData, sizeof(int));

        int nCount = nLens/sizeof(MOTOR_DATA);

        MOTOR_DATA* pMotor = (MOTOR_DATA*)(pData+4);

        strHeader.Format(L"[%d]", m_Grid.GetRowCount());
        int nRow = m_Grid.InsertRow(strHeader);
        m_Grid.SetItemFormat(nRow,0,DT_CENTER | DT_VCENTER);
        for (int i=0; i<m_Grid.GetColumnCount()-3; i++)
        {
                if (i >= nCount)
                        break;

                GV_ITEM Item;

                Item.mask = GVIF_TEXT;
                Item.row = nRow;
                Item.col = i+2;
                Item.strText.Format(L"%d", pMotor[i].nAngle);
                //Item.nFormat = DT_CENTER;
		
                m_Grid.SetItem(&Item);
                m_Grid.SetItemFormat(Item.row,Item.col,DT_CENTER);
                m_Grid.SetItemData(nRow, 0, ActionBlockID);
        }

        GV_ITEM Item;

        Item.mask = GVIF_TEXT;
        Item.row = nRow;
        Item.col = 1;
        Item.strText = BlockType;
        m_Grid.SetItem(&Item);
        //m_Grid.SetItemFormat(Item.row,Item.col,DT_CENTER);

// 	Item.col = 18;
// 	Item.strText.Format(L"%0.2f", nStopTime);
// 	m_Grid.SetItem(&Item);

// 	if (IsCompensatory)
// 	{
// 		m_Grid.SetItemState(nRow,18, m_Grid.GetItemState(nRow,18) | GVIS_READONLY);
// 	}

        // 显示当前节拍的时间值
        Item.mask = GVIF_TEXT;
        Item.row = nRow;
        Item.col = 1;

        Item.strText = BlockType;
        m_Grid.SetItem(&Item);

        Item.col = 18;
    QString strTime;
//	float fTime = nStopTime*m_pEvent->ILayerOptionHandler_GetScaleValue();

        int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();//时间单位
	
        strTime.Format(L"%d", (ULONG)(ceil(fRunTime*nScaleTime)));//真实运行时间
        Item.strText = strTime;
	 
        m_Grid.SetItem(&Item);
        m_Grid.SetItemFormat(Item.row,Item.col,DT_CENTER);
        m_Grid.SetItemState(nRow,1, m_Grid.GetItemState(nRow,18) | GVIS_READONLY);


        //设置列表的总时间数据=======================
        Item.col = 19;
        strTime.Format(L"%d", (ULONG)(ceil(fAllTime*nScaleTime)));//真实总时间
        Item.strText = strTime;

        m_Grid.SetItem(&Item);
        m_Grid.SetItemFormat(Item.row, Item.col, DT_CENTER);
        //===============================================
	
#if !defined(_WIN32_WCE) || (_WIN32_WCE < 211)
        m_Grid.AutoSize();
#endif
}

int COptionsMotionDlg::GetCurrenGroupManagerID()
{
        if (m_nCurrentSelectItem == -1)
                return -1;

        if (m_pGroupManager == NULL)
                return -1;

        CActionBlockGroupManager* pGroupManager = m_pGroupManager->GetAt(m_nCurrentSelectItem);
        if (pGroupManager)
        {
                return pGroupManager->GetID();
        }

        return -1;
}

MOTOR_DATA* COptionsMotionDlg::GetMotoData(char* pData, int nLen, int nID)
{
        int nCount = (nLen-4)/sizeof(MOTOR_DATA);
        MOTOR_DATA* p = (MOTOR_DATA*)(pData+4);

        return &p[nID];
}

void COptionsMotionDlg::SetMotoAngle(char* pData, int nLen, int nID, int nAngle)
{
        int nCount = (nLen-4)/sizeof(MOTOR_DATA);
        MOTOR_DATA* p = (MOTOR_DATA*)(pData+4);

        p[nID].nAngle = nAngle;
}
void COptionsMotionDlg::AddGroupByMenu()
{
        OnBnClickAdd();
}
void COptionsMotionDlg::InsertGroupByMenu(int nCol)
{
        if (m_pEvent && m_pRobotView)
        {
                char* pData = NULL;
                int nLen = 0;
                m_pRobotView->GetMotorDatas(&pData, nLen);
                if (pData && nLen)
                {
            QString tempValue;
                        m_editBeatValue.GetWindowText(tempValue);
                        float fRunTime = _wtof(tempValue);//运行时间

                        int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();

                        CActionBlockData* pActionBlockData = new CActionBlockData();
                        pActionBlockData->SetID(-1);
                        pActionBlockData->SetDescription(L"Action");
                        pActionBlockData->SetRunTime(fRunTime / nScaleTime);
                        pActionBlockData->SetAllTime(fRunTime / nScaleTime);
                        pActionBlockData->SetExtBlockData(nLen, pData);

                        m_pEvent->ILayerOptionHandler_InsertGroupByMenu(pActionBlockData, nCol);
                }
        }
}

void COptionsMotionDlg::OnBnClickAdd()
{
        //不能直接使用数字10作为时间刻度
        if (!m_pEvent)
        {
                return;
        }
        int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();//时间单位
        if (m_pEvent && m_pRobotView)
        {
                char* pData = NULL;
                int nLen = 0;
                m_pRobotView->GetMotorDatas(&pData, nLen);
                if (pData && nLen)
                {
                        float fRunTime = 0;

                        if (m_editBeatValue.IsWindowEnabled())
                        {
                QString tempValue;
                                m_editBeatValue.GetWindowTextW(tempValue);
                                fRunTime = _wtof(tempValue.GetBuffer());
                                if (fRunTime < 10)
                                {
                                //	AfxMessageBox(L"节拍值不能小于零");
                                        fRunTime = 10;
                                }
                        }
			
                        CActionBlockData* pActionBlockData = new CActionBlockData();
                        pActionBlockData->SetID(-1);
                        pActionBlockData->SetDescription(L"Action");
                        pActionBlockData->SetRunTime(fRunTime / nScaleTime);
                        pActionBlockData->SetAllTime(fRunTime / nScaleTime);//设置总时间
                        pActionBlockData->SetExtBlockData(nLen, pData);

                        int nCurrentID = GetCurrenGroupManagerID();
                        m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData);

                        if (nCurrentID != -1)
                        {
                                int nID = pActionBlockData->GetID();
                                SetShowID(&nID, 1);
                                AddToGrid(pData, nLen, pActionBlockData->GetDescription(), pActionBlockData->GetRunTime(), pActionBlockData->GetAllTime(), pActionBlockData->GetID());
                                ShowSelectActionBlock();
                        } else {
                                int nID = pActionBlockData->GetID();
                                SetShowID(&nID, 1);
                                ShowSelectActionBlock();
                        }
//			delete [] pData;
                }
        }
}

void COptionsMotionDlg::OnBnClickDelete()
{
        if (m_pGroupManager == NULL)
                return;

        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if(pGroupManager == NULL)
                return;

        bool bUpdate = false;
        // 最后一次删除的ID
        int nLastDeleteID = -1;
        float nTimeInc = 0;
        for (int i=1; i<m_Grid.GetRowCount(); i++)
        {
                bool bSelectFullLine = true;

                bSelectFullLine = IsFullRowSelect(i);

                if (bSelectFullLine)
                {
                        bUpdate = true;
                        int nActionBlockID = m_Grid.GetItemData(i, 0);
                        CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nActionBlockID);

            QString tempDescription = pActionBlockData->GetDescription();
                        int nLens = 0;
                        char* pdata = NULL;
                        pGroupManager->GetExtBlockData(&pdata, nLens);
                        delete[] pdata;
                        if (nLens == 8 || nLens == 14)
                        {
                                if (tempDescription.Compare(L"Recoup"))
                                {
                                        int nRunTime = 0;
                                        int ActionCount = 0;
                                        vector<CActionBlockData*> vecDataBlock;
                                        pGroupManager->GetAllBlockData(vecDataBlock);
                                        for (int i = 0; i < vecDataBlock.size(); i++)
                                        {
                                                CActionBlockData* pTempBlockData = vecDataBlock[i];
                                                tempDescription = pTempBlockData->GetDescription();
                                                if (tempDescription.Compare(L"Action") == 0)
                                                {
                                                        ActionCount++;
                                                }
                                        }
					
                                        float fAverageAllTime = pGroupManager->GetActionAllTimeSum() / (ActionCount-1);
                                        float fAverageRunTime = pGroupManager->GetActionRunTimeSum() / (ActionCount-1);

                                        vector<CActionBlockData*> vecModifyBlock;
                                        pGroupManager->GetAllBlockData(vecModifyBlock);
                                        for (int i = 0; i < vecModifyBlock.size(); i++)
                                        {
                                                CActionBlockData* pTempBlockData = vecDataBlock[i];
                                                tempDescription = pTempBlockData->GetDescription();
                                                if (tempDescription.Compare(L"Action") == 0)
                                                {
                                                        pTempBlockData->SetRunTime(fAverageRunTime);
                                                        pTempBlockData->SetAllTime(fAverageAllTime);
                                                }
                                        }
                                        pGroupManager->DeleteLayerBlock(nActionBlockID);
                                } else {
                                        pGroupManager->DeleteLayerBlock(nActionBlockID);
                                }
                        }
                        else
                        {
                                nTimeInc += pActionBlockData->GetAllTime();
                                pGroupManager->DeleteLayerBlock(nActionBlockID);
                        }

                        //检查删除后，是否还存在补偿帧
                        bool IsHaveCompensatory = false;
                        vector<CActionBlockData*> vecDataBlock;
                        pGroupManager->GetAllBlockData(vecDataBlock);
                        for (int i = 0; i < vecDataBlock.size(); i++)
                        {
                                CActionBlockData* pTempBlockData = vecDataBlock[i];
                QString strDescription = pTempBlockData->GetDescription();
                                if (!strDescription.Compare(L"Recoup"))
                                {
                                        IsHaveCompensatory = true;
                                        break;
                                }
                        }
                        if (!IsHaveCompensatory)
                        {
                                char *pdatas = NULL;
                                int nLenss = 0;
                                pGroupManager->GetExtBlockData(&pdatas, nLenss);
                                delete []  pdatas;

                                if (nLenss == 6 || nLenss == 12)
                                {
                                        pdatas = "motion";
                                } else {
                                        pdatas = "analysis";
                                }
                                nLenss = strlen(pdatas);
                                pGroupManager->SetExtBlockData(nLenss, pdatas);
                        }
			
                        if (pGroupManager->GetActionCount() < 1)
                        {
                                m_pEvent->ILayerOptionHandler_OnDeleteGroupBlock();
                        }
                        nLastDeleteID = i;
                } else {
                        continue;
                }

                //i--;
                //m_Grid.DeleteRow(i);
        }

        // 滚动到最后一条删除的记录
        if (nLastDeleteID != -1)
        {
                if (nLastDeleteID < m_Grid.GetRowCount()-1)
                {
                        int nShowID = m_Grid.GetItemData(nLastDeleteID+1, 0);
                        CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nShowID);
                        if (pActionBlockData)
                        {
                                nShowID = pActionBlockData->GetID();
                        }
                        else
                        {
                                nShowID = -1;
                        }

                        nLastDeleteID = nShowID;
                }
                else
                {
                        nLastDeleteID = -1;
                }
        }

        if (bUpdate)
        {
                //m_nCurrentGroupID = -1;
                int nCurrentID = m_nCurrentGroupID;
                m_nCurrentGroupID = -1;
                m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, -nTimeInc);
                m_Grid.Refresh();

                if (nLastDeleteID != -1)
                {
                        SetShowID(&nLastDeleteID, 1);
                        ShowSelectActionBlock();
                }
                else
                {
                        // 滚动到最后一行
                        if (m_Grid.GetRowCount())
                                m_Grid.EnsureVisible(m_Grid.GetRowCount()-1, 0);
                }
        }
}

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
void COptionsMotionDlg::UpdateGridShow(int nRow, CActionBlockData* pActionBlock)
{
        char* pData = NULL;
        int nLen = 0;
        int nServoCnt = SERVO_COUNT;
        if (m_pRobotView)
        {
                nServoCnt = m_pRobotView->GetMotorCount();
        }
        pActionBlock->GetExtBlockData(&pData, nLen);
        for (int i=0; /*i<m_Grid.GetColumnCount()-4*/i<nServoCnt; i++)
        {
// 		if (i==15)
// 		{
// 			int c = 1;
// 		}
                MOTOR_DATA* pMotor = GetMotoData(pData, nLen, i);
                GV_ITEM Item;
                Item.mask = GVIF_TEXT;
                Item.row = nRow;
                Item.col = i+2;
                Item.strText.Format(L"%d", pMotor->nAngle);
                m_Grid.SetItem(&Item);
        }

// 	GV_ITEM Item;
//
// 	Item.mask = GVIF_TEXT;
// 	Item.row = nRow;
// 	Item.col = 21;
// 	Item.strText.Format(L"%d", nRunTime);
// 	m_Grid.SetItem(&Item);

// 	Item.mask = GVIF_TEXT;
// 	Item.row = nRow;
// 	Item.col = 22;
// 	Item.strText.Format(L"%d", nStopTime);
// 	m_Grid.SetItem(&Item);

        delete [] pData;
        m_Grid.Refresh();
}

void COptionsMotionDlg::OnBnClickModify()
{
        if (m_pGroupManager == NULL)
                return;

        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if(pGroupManager == NULL)
                return;

        int nTimeInc = 0;
        bool bUpdate = false;
        for (int i=1; i<m_Grid.GetRowCount(); i++)
        {
                bool bSelectFullLine = true;

                bSelectFullLine = IsFullRowSelect(i);

                if (bSelectFullLine)
                {
                        int nSelectBlockID = m_Grid.GetItemData(i, 0);

                        if (m_pEvent && m_pRobotView)
                        {
                                char* pData = NULL;
                                int nLen = 0;
                                m_pRobotView->GetMotorDatas(&pData, nLen);
                                if (pData && nLen)
                                {
                                        CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nSelectBlockID);
                                        int nTimeInc = 0;
                                        pActionBlockData->SetExtBlockData(nLen, pData);

                                        int nCurrentShowID = pActionBlockData->GetID();
                                        SetShowID(&nCurrentShowID, 1);

                                        if (m_pEvent)
                                        {
                                                m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(m_nCurrentGroupID, nCurrentShowID, nTimeInc);
                                        }

                                        UpdateGridShow(i, pActionBlockData);

                                }
                        }
                }
        }
}

bool COptionsMotionDlg::IsFullRowSelect(int nRow)
{
        for (int j=1; j<m_Grid.GetColumnCount(); j++)
        {
                if (m_Grid.IsCellSelected(nRow, j) == false)
                {
                        return false;
                }
        }

        return true;
}

bool COptionsMotionDlg::IsGridHasSelectRow()
{
        for(int i=0; i<m_Grid.GetRowCount(); i++)
        {
                if (IsFullRowSelect(i) == true)
                        return true;
        }

        return false;
}

void COptionsMotionDlg::OnBnClickInsertCompensatory()
{
        if (m_pGroupManager == NULL)
                return;

        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if(pGroupManager == NULL)
                return;

        bool IsRowSelected = IsGridHasSelectRow();
        if (!IsRowSelected)
        {
                int nSelectBlockID = m_Grid.GetItemData(m_Grid.GetRowCount()-1, 0);
                if (m_pEvent && m_pRobotView)
                {
                        char* pData = NULL;
                        int nLen = 0;
                        m_pRobotView->GetMotorDatas(&pData, nLen);
                        if (pData && nLen)
                        {
                QString tempValue;
                                m_editRecoupValue.GetWindowTextW(tempValue);
                                float fRunTime = _wtof(tempValue.GetBuffer());//运行时间

                                int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();

                                CActionBlockData* pActionBlockData = new CActionBlockData();
                                pActionBlockData->SetID(pGroupManager->GetUnUserID());
                                pActionBlockData->SetDescription(L"Recoup");
                                pActionBlockData->SetRunTime(fRunTime/nScaleTime);
                                pActionBlockData->SetAllTime(fRunTime/nScaleTime);
                                pActionBlockData->SetExtBlockData(nLen, pData);
                                pGroupManager->InsertAfterId(nSelectBlockID, pActionBlockData);

                                char *pdatas = NULL;
                                int nLenss = 0;
                                pGroupManager->GetExtBlockData(&pdatas, nLenss);
                                delete [] pdatas;

                                if (nLenss == 6 || nLenss == 12)
                                {
                                        pdatas = "motionRecoup";
                                }
                                else
                                {
                                        pdatas = "analysisRecoup";
                                }
                                nLenss = strlen(pdatas);
                                pGroupManager->SetExtBlockData(nLenss, pdatas);

                                int nCurrentShowID = pActionBlockData->GetID();
                                SetShowID(&nCurrentShowID, 1);
                                int nCurrentID = m_nCurrentGroupID;
                                m_nCurrentGroupID = -1;
                                m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, 0);
                        }
                }
                return;
        }

        for (int i=1; i<m_Grid.GetRowCount(); i++)
        {
                bool bSelectFullLine = true;
                bSelectFullLine = IsFullRowSelect(i);

                if (bSelectFullLine)
                {
                        int nSelectBlockID = m_Grid.GetItemData(i, 0);
                        if (m_pEvent && m_pRobotView)
                        {
                                char* pData = NULL;
                                int nLen = 0;
                                m_pRobotView->GetMotorDatas(&pData, nLen);
                                if (pData && nLen)
                                {
                    QString tempValue;
                                        m_editRecoupValue.GetWindowTextW(tempValue);
                                        float fRunTime = _wtof(tempValue);
					
                                        int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();

                                        CActionBlockData* pActionBlockData = new CActionBlockData();
                                        pActionBlockData->SetID(pGroupManager->GetUnUserID());
                                        pActionBlockData->SetDescription(L"Recoup");
                                        pActionBlockData->SetRunTime(fRunTime / nScaleTime);
                                        pActionBlockData->SetAllTime(fRunTime / nScaleTime);
                                        pActionBlockData->SetExtBlockData(nLen, pData);
                                        pGroupManager->InsertBeforeId(nSelectBlockID, pActionBlockData);

                                        char *pdatas = NULL;
                                        int nLenss = 0;
                                        pGroupManager->GetExtBlockData(&pdatas, nLenss);
                                        delete [] pdatas;

                                        if (nLenss == 6 || nLenss == 12)
                                        {
                                                pdatas = "motionRecoup";
                                        } else {
                                                pdatas = "analysisRecoup";
                                        }
                                        nLenss = strlen(pdatas);
                                        pGroupManager->SetExtBlockData(nLenss, pdatas);

                                        int nCurrentShowID = pActionBlockData->GetID();
                                        SetShowID(&nCurrentShowID, 1);
                                        int nCurrentID = m_nCurrentGroupID;
                                        m_nCurrentGroupID = -1;
                                        m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, 0);
//					UpdateUIByData();
                                }
                        }
                }
        }
}

void COptionsMotionDlg::OnBnClickDisassemblyInsert()
{
        if (m_pGroupManager == NULL)
                return;
 
        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if(pGroupManager == NULL)
                return;
        float fOldEndTime = pGroupManager->GetEndTime();
 
        bool IsRowSelected = IsGridHasSelectRow();
        if (!IsRowSelected)
        {
                if (m_pEvent && m_pRobotView)
                {
                        int nSelectBlockID = m_Grid.GetItemData(m_Grid.GetRowCount()-1, 0);
                        char* pData = NULL;
                        int nLen = 0;
                        m_pRobotView->GetMotorDatas(&pData, nLen);
                        if (pData && nLen)
                        {
                                int ActionCount = 0;

                QString tempDescription;
                                vector<CActionBlockData*> vecDataBlock;
                                pGroupManager->GetAllBlockData(vecDataBlock);
                                for (int i = 0; i < vecDataBlock.size(); i++)
                                {
                                        CActionBlockData* pTempBlockData = vecDataBlock[i];
                                        tempDescription = pTempBlockData->GetDescription();
                                        if (tempDescription.Compare(L"Action") == 0)
                                        {
                                                ActionCount++;
                                        }
                                }
 				
                                float fAverageAllTime = pGroupManager->GetActionAllTimeSum() / (ActionCount + 1);  //平均总时间
                                //四舍五入保留一位小数
                                if (fAverageAllTime  < GRID_MIN_TIMEVALUE)
                                {
                                        return;
                                }
                                //nBlockAverageTime  = Round(nBlockAverageTime  , 1);
				
                                float fAverageRunTime = pGroupManager->GetActionRunTimeSum() / (ActionCount + 1);  //平均运动时间
                                if (fAverageRunTime  < GRID_MIN_TIMEVALUE)
                                {
                                        return;
                                }

                                vector<CActionBlockData*> vecModifyBlock;
                                pGroupManager->GetAllBlockData(vecModifyBlock);//
                                for (int i = 0; i < vecModifyBlock.size(); i++)
                                {
                                        CActionBlockData* pTempBlockData = vecDataBlock[i];
                                        tempDescription = pTempBlockData->GetDescription();
                                        if (tempDescription.Compare(L"Action") == 0)
                                        {
                                                pTempBlockData->SetRunTime(fAverageRunTime);
                                                pTempBlockData->SetAllTime(fAverageAllTime);
                                        }
                                }
 
                                CActionBlockData* pActionBlockData = new CActionBlockData();
                                pActionBlockData->SetID(pGroupManager->GetUnUserID());
                                pActionBlockData->SetDescription(L"Action");
                                pActionBlockData->SetRunTime(fAverageRunTime);
                                pActionBlockData->SetAllTime(fAverageAllTime);
                                pActionBlockData->SetExtBlockData(nLen, pData);
                                pGroupManager->InsertAfterId(nSelectBlockID, pActionBlockData);
 
                                char* pdatas = NULL;
                                int nLenss = 0;
                                pGroupManager->GetExtBlockData(&pdatas, nLenss);
                                delete [] pdatas;
 
                                if (nLenss == 12 || nLenss == 14)
                                {
                                        pdatas = "analysisRecoup";
                                } else {
                                        pdatas = "analysis";
                                }
                                nLenss = strlen(pdatas);
                                pGroupManager->SetExtBlockData(nLenss, pdatas);
 
                                int nCurrentShowID = pActionBlockData->GetID();
                                SetShowID(&nCurrentShowID, 1);
                                int nCurrentID = m_nCurrentGroupID;
                                m_nCurrentGroupID = -1;
                                pGroupManager->RecalculateTime();
                                m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pGroupManager->GetEndTime() - fOldEndTime);
                        }
                }
                return;
        }
 
        for (int i=1; i<m_Grid.GetRowCount(); i++)
        {
		
                bool bSelectFullLine = true;
                bSelectFullLine = IsFullRowSelect(i);
 
                if (bSelectFullLine)
                {
                        int nSelectBlockID = m_Grid.GetItemData(i, 0);
                        if (m_pEvent && m_pRobotView)
                        {
                                char* pData = NULL;
                                int nLen = 0;
                                m_pRobotView->GetMotorDatas(&pData, nLen);
                                if (pData && nLen)
                                {
                                        int ActionCount = 0;
                    QString tempDescription;
                                        vector<CActionBlockData*> vecDataBlock;
                                        pGroupManager->GetAllBlockData(vecDataBlock);
                                        for (int i = 0; i < vecDataBlock.size(); i++)
                                        {
                                                CActionBlockData* pTempBlockData = vecDataBlock[i];
                                                tempDescription = pTempBlockData->GetDescription();
                                                if (tempDescription.Compare(L"Action") == 0)
                                                {
                                                        ActionCount++;
                                                }
                                        }
                                        float nGroupTotalTime = pGroupManager->GetEndTime()-pGroupManager->GetStartTime();
                                        float fAverageAllTime = nGroupTotalTime/(ActionCount+1);
                                        //四舍五入保留一位小数
                                        if (fAverageAllTime  < GRID_MIN_TIMEVALUE)
                                        {
                                                return;
                                        }

                                        float fTotalRunTime = pGroupManager->GetActionRunTimeSum();
                                        float fAverageRunTime = fTotalRunTime/(ActionCount + 1);  //平均运动时间
                                        if (fAverageRunTime  < GRID_MIN_TIMEVALUE)
                                        {
                                                return;
                                        }

                                        //nBlockAverageTime  = Round(nBlockAverageTime , 1);
                                        vector<CActionBlockData*> vecModifyBlock;
                                        pGroupManager->GetAllBlockData(vecModifyBlock);
                                        for (int i = 0; i < vecModifyBlock.size(); i++)
                                        {
                                                CActionBlockData* pTempBlockData = vecDataBlock[i];
                                                tempDescription = pTempBlockData->GetDescription();
                                                if (tempDescription.Compare(L"Action") == 0)
                                                {
                                                        pTempBlockData->SetRunTime(fAverageRunTime);
                                                        pTempBlockData->SetAllTime(fAverageAllTime);
                                                }
                                        }
 
                                        CActionBlockData* pActionBlockData = new CActionBlockData();
                                        pActionBlockData->SetID(pGroupManager->GetUnUserID());
                                        pActionBlockData->SetDescription(L"Action");
                                        pActionBlockData->SetRunTime(fAverageRunTime);
                                        pActionBlockData->SetAllTime(fAverageAllTime);
                                        pActionBlockData->SetExtBlockData(nLen, pData);
                                        pGroupManager->InsertAfterId(nSelectBlockID, pActionBlockData);
 
                                        char* pdatas = NULL;
                                        int nLenss = 0;
                                        pGroupManager->GetExtBlockData(&pdatas, nLenss);
                                        delete [] pdatas;
 
                                        if (nLenss == 12 || nLenss == 14)
                                        {
                                                pdatas = "analysisRecoup";
                                        } else {
                                                pdatas = "analysis";
                                        }
                                        nLenss = strlen(pdatas);
                                        pGroupManager->SetExtBlockData(nLenss, pdatas);
 
                                        int nCurrentShowID = pActionBlockData->GetID();
                                        SetShowID(&nCurrentShowID, 1);
 					
                                        int nCurrentID = m_nCurrentGroupID;
                                        m_nCurrentGroupID = -1;
 					
                                        pGroupManager->RecalculateTime();
                                        m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pGroupManager->GetEndTime() - fOldEndTime);
                                        return;
                                }
                        }
                }
        }
}

void COptionsMotionDlg::OnBnClickInsert()
{
        if (m_pGroupManager == NULL)
                return;

        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if(pGroupManager == NULL)
                return;

        for (int i=1; i<m_Grid.GetRowCount(); i++)
        {
                bool bSelectFullLine = true;
                bSelectFullLine = IsFullRowSelect(i);

                if (bSelectFullLine)
                {
                        int nSelectBlockID = m_Grid.GetItemData(i, 0);
                        if (m_pEvent && m_pRobotView)
                        {
                                char* pData = NULL;
                                int nLen = 0;
                                m_pRobotView->GetMotorDatas(&pData, nLen);
                                if (pData && nLen)
                                {
                                        float fRunTime = 0;
                    QString tempDescription;
                                        if (m_editBeatValue.IsWindowEnabled())
                                        {
                        QString tempValue;
                                                m_editBeatValue.GetWindowTextW(tempValue);
                                                fRunTime = _wtof(tempValue.GetBuffer());
                                                if (fRunTime < 10)
                                                {
                                                        fRunTime = 10;
                                                }
                                        }
                                        int nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();//时间单位

                                        CActionBlockData* pActionBlockData = new CActionBlockData();
                                        pActionBlockData->SetID(pGroupManager->GetUnUserID());
                                        pActionBlockData->SetDescription(L"Action");
                                        pActionBlockData->SetRunTime(fRunTime / nScaleTime);
                                        pActionBlockData->SetAllTime(fRunTime / nScaleTime);
                                        pActionBlockData->SetExtBlockData(nLen, pData);
                                        pGroupManager->InsertBeforeId(nSelectBlockID, pActionBlockData);

                                        int nCurrentShowID = pActionBlockData->GetID();
                                        SetShowID(&nCurrentShowID, 1);
                                        int nCurrentID = m_nCurrentGroupID;
                                        m_nCurrentGroupID = -1;
                                        m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pActionBlockData->GetAllTime());
                                }
                        }
                }
        }
}

void COptionsMotionDlg::Set3DRobotViewInterface(IUBXRobot3DView* pRobotView)
{
        m_pRobotView = pRobotView;
}


void COptionsMotionDlg::OnNMClickListMotionList(NMHDR *pNMHDR, LRESULT *pResult)
{
        LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
        if ((pNMItemActivate->iItem != -1) /*&& (pNMItemActivate->iItem != m_nCurrentSelectItem)*/)
        {
                SetCurrentSelectItem(pNMItemActivate->iItem);
                if (m_pEvent)
                {
                        m_pEvent->ILayerOptionHandler_SetActionBlockState(false);
                        m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(m_nCurrentGroupID,0);
                }
        }
	
        *pResult = 0;
}

void COptionsMotionDlg::OnNMRClickGroupList(NMHDR *pNMHDR, LRESULT *pResult)
{
        LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

        if (pNMItemActivate->iItem != -1)
        {
                SetCurrentSelectItem(pNMItemActivate->iItem);
                if (m_pEvent)
                {
                        m_pEvent->ILayerOptionHandler_SetActionBlockState(false);
                        m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(m_nCurrentGroupID,0);
                }
        }

        CMenu menu;
        CAutolockRes res;
        menu.LoadMenu(IDR_MENU_GROUP);
        CMenu *pContextMenu = menu.GetSubMenu(0);

        if (GetCurrenGroupManagerID() == -1)
        {
                pContextMenu->EnableMenuItem(ID_MENU_GROUP_DELETE, MF_BYCOMMAND|MF_GRAYED);
                pContextMenu->EnableMenuItem(ID_LIST_PASTE, MF_BYCOMMAND|MF_GRAYED);
                pContextMenu->EnableMenuItem(ID_LIST_COPY, MF_BYCOMMAND|MF_GRAYED);
                pContextMenu->EnableMenuItem(ID_LIST_MODIFY_NAME, MF_BYCOMMAND|MF_GRAYED);
        }

    QPoint point1;
        GetCursorPos(&point1);
        pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point1.x, point1.y, this);
        *pResult = 0;
}

bool COptionsMotionDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
        if (wParam == ID_MENU_GROUP_DELETE)
        {
                POSITION pos = m_lstGroupManager.GetFirstSelectedItemPosition();
                if (pos != NULL)
                {
                        while (pos)
                        {
                                int nItem = m_lstGroupManager.GetNextSelectedItem(pos);
                                CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(nItem);
                                int startTime = pGroup->GetStartTime();
                                m_pEvent->ILayerOptionHandler_SetSelectGroupBlock(startTime+2);
                        }
                }
                m_pEvent->ILayerOptionHandler_OnDeleteGroupBlock();
        }

        if (wParam == ID_LIST_PASTE)
        {
                CActionBlockGroupManager* pGroup = m_pGroupManager->FindBlockData(GetCurrenGroupManagerID());
                if (pGroup)
                {
                        int startTime = pGroup->GetStartTime();
                        m_pEvent->ILayerOptionHandler_PasteGroup(startTime+1);
                }
        }

        if (wParam == ID_LIST_COPY)
        {
                m_pEvent->ILayerOptionHandler_CopyGroup();
        }

        if (wParam == ID_LIST_MODIFY_NAME)
        {
                CActionBlockGroupManager* pGroup = m_pGroupManager->FindBlockData(GetCurrenGroupManagerID());
                if (pGroup)
                {
                        float fStartTime = pGroup->GetStartTime();
                        m_pEvent->ILayerOptionHandler_ModifyGroupName(fStartTime + 1);
                }
        }

        return __super::OnCommand(wParam, lParam);
}

bool IsNum(QString &str)
{
        int n=str.GetLength();
        for(int i=0;i<n;i++)
                if (str[i]<'0'||str[i]>'9')
                        return false;
        return true;
}


void COptionsMotionDlg::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
        int nScaleTime = 10;//时间单位
        if (m_pEvent)
        {
                nScaleTime = m_pEvent->ILayerOptionHandler_GetScaleValue();
        }
	
        NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

        if (pItem->iRow > m_Grid.GetRowCount())
                return;

        int nActionBlockID = m_Grid.GetItemData(pItem->iRow, 0);
        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if (pGroupManager)
        {
                CActionBlockData* pBlockData = pGroupManager->FindBlockData(nActionBlockID);
                if (pBlockData)
                {
                        // 舵机数据
                        if (pItem->iColumn < GRID_COL_RUNTIME)
                        {
                QString strAngle = m_Grid.GetItemText(pItem->iRow, pItem->iColumn);
                                if (strAngle.IsEmpty())
                                {
                    QString strMinAngleValue;
                    strMinAngleValue.Format(QObject::tr("%d"),GRID_MIN_ANGLEVALUE);
                                        m_Grid.SetItemText(pItem->iRow, pItem->iColumn,strMinAngleValue);
                                        strAngle = m_Grid.GetItemText(pItem->iRow, pItem->iColumn);
                                }
                                int nAngle = _wtoi(strAngle);
                                bool isNum = IsNum(strAngle);
                                if (nAngle > GRID_MAX_ANGLEVALUE || isNum==false)
                                {
                                        // 数据不合格，恢复数据
                                        char* pData = NULL;
                                        int nLen = 0;
                                        pBlockData->GetExtBlockData(&pData, nLen);
                                        if (pData && nLen)
                                        {
                                                MOTOR_DATA* pMoto = GetMotoData(pData, nLen, pItem->iColumn-2);
                                                if (pMoto)
                                                {
                                                        strAngle.Format(L"%d", pMoto->nAngle);
                                                        m_Grid.SetItemText(pItem->iRow, pItem->iColumn, strAngle);
                                                }

                                                delete [] pData;
                                        }
                                }
                                else
                                {
                                        // 更新最新的数据
                                        char* pData = NULL;
                                        int nLen = 0;
                                        pBlockData->GetExtBlockData(&pData, nLen);
                                        if (pData && nLen)
                                        {
                                                // Modified Flag
                                                SetMotoAngle(pData, nLen, pItem->iColumn-2, nAngle);
                                                if (m_pEvent)
                                                {
                                                        m_pEvent->ILayerOptionHandler_SetModifiedState(true);
                                                }
                                                pBlockData->SetExtBlockData(nLen, pData);
                                                delete [] pData;
                                        }
                                }
                        }
                        else if (pItem->iColumn == GRID_COL_RUNTIME
                                || pItem->iColumn == GRID_COL_ALLTIME)//修改了运动时间或总时间
                        {
                                bool bIsRunTimeCol = pItem->iColumn == GRID_COL_RUNTIME;

                QString sTempTime = m_Grid.GetItemText(pItem->iRow, pItem->iColumn);
                                if (sTempTime.IsEmpty())
                                {
                    sTempTime.Format(QObject::tr("%d"), GRID_MIN_TIMEVALUE);
                                        m_Grid.SetItemText(pItem->iRow, pItem->iColumn, sTempTime);
                                }

                                sTempTime = m_Grid.GetItemText(pItem->iRow, pItem->iColumn);

                                bool isNumber = IsNum(sTempTime);
                                float fRealTime = _wtof(sTempTime);
                                if (!isNumber || fRealTime > GRID_MAX_TIMEVALUE || fRealTime < GRID_MIN_TIMEVALUE)
                                {
                                        //四舍五入
                                        int nTempTime = (bIsRunTimeCol?pBlockData->GetRunTime():pBlockData->GetAllTime())*nScaleTime + 0.5;
                                        sTempTime.Format(L"%d", nTempTime);
                                        m_Grid.SetItemText(pItem->iRow, pItem->iColumn, sTempTime);
                                }

                QString sRunTime = m_Grid.GetItemText(pItem->iRow, GRID_COL_RUNTIME);
                QString sAllTime = m_Grid.GetItemText(pItem->iRow, GRID_COL_ALLTIME);

                                if(_tstoi(sRunTime) > _tstoi(sAllTime))
                                {
                                        if(bIsRunTimeCol)
                                        {
                                                if(_tstoi(sRunTime) <= GRID_MAX_TIMEVALUE)
                                                        m_Grid.SetItemText(pItem->iRow, GRID_COL_ALLTIME, sRunTime);//设置总时间为运行时间
                                                else
                                                {
                                                        //四舍五入
                                                        int nTempTime = pBlockData->GetRunTime()*nScaleTime + 0.5;
                                                        sTempTime.Format(L"%d", nTempTime);
                                                        m_Grid.SetItemText(pItem->iRow, GRID_COL_RUNTIME, sTempTime);
                                                }
                                        }
                                        else
                                        {
                                                if(_tstoi(sAllTime) <= GRID_MAX_TIMEVALUE)
                                                        m_Grid.SetItemText(pItem->iRow, GRID_COL_RUNTIME, sAllTime);//设置运行时间为总时间
                                                else
                                                {
                                                        //四舍五入
                                                        int nTempTime = pBlockData->GetAllTime()*nScaleTime + 0.5;
                                                        sTempTime.Format(L"%d", nTempTime);
                                                        m_Grid.SetItemText(pItem->iRow, GRID_COL_ALLTIME, sTempTime);
                                                }
                                        }
                                }

                                sRunTime = m_Grid.GetItemText(pItem->iRow, GRID_COL_RUNTIME);
                                sAllTime = m_Grid.GetItemText(pItem->iRow, GRID_COL_ALLTIME);

                                float fRunTime = _wtof(sRunTime);
                                float fAllTime = _wtof(sAllTime);
                                float fAllTimeInc = fAllTime / nScaleTime - pBlockData->GetAllTime();//改变的时间

                                pBlockData->SetRunTime(fRunTime / nScaleTime);
                                pBlockData->SetAllTime(fAllTime / nScaleTime);

                                if(m_pEvent)
                                {
                                        m_pEvent->ILayerOptionHandler_OnActionBlockDataChange(m_nCurrentGroupID, nActionBlockID, fAllTimeInc);//分组总时间改变通知
                                }

                                m_Grid.RedrawRow(pItem->iRow);
                        }
                }
        }

        *pResult = 0;
}
void COptionsMotionDlg::ModleView(int nActionBlockID)
{
        if (m_pGroupManager == NULL)
                return;
        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if (pGroupManager == NULL)
                return;

        CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nActionBlockID);
        if (pActionBlockData)
        {
                //int starTime = pActionBlockData->GetStartTime();
                //int endTime = pActionBlockData->GetEndTime();

                char *pData = NULL;
                int nLen = 0;
                pActionBlockData->GetExtBlockData(&pData, nLen);
                if (pData && nLen)
                {
                        int nScaleValue = m_pEvent->ILayerOptionHandler_GetScaleValue();
                        //				m_pRobotView->SetRunTimeAndStopTime(starTime, endTime);
                        m_pRobotView->SetMotorDatasManual(pData, nLen, 0);
                        delete [] pData;
                }
        }
}
void COptionsMotionDlg::PlayBlock(int nActionBlockID)
{
        if (m_pGroupManager == NULL)
                return;
        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if (pGroupManager == NULL)
                return;

        CActionBlockData* pActionBlockData = pGroupManager->FindBlockData(nActionBlockID);
        if (pActionBlockData)
        {
                //int starTime = pActionBlockData->GetStartTime();
                //int endTime = pActionBlockData->GetEndTime();

                char *pData = NULL;
                int nLen = 0;
                pActionBlockData->GetExtBlockData(&pData, nLen);
                if (pData && nLen)
                {
// 			if (m_pRobotView)
// 			{
// 				int nScaleValue = m_pEvent->ILayerOptionHandler_GetScaleValue();
// //				m_pRobotView->SetRunTimeAndStopTime(starTime, endTime);
// 				m_pRobotView->SetMotorDatasManual(pData, nLen, 0);
// 			}
            if (m_pEvent)
            {
                m_pEvent->ILayerOptionHandler_OnDataBlockPlaying(pActionBlockData);
            }
                        delete [] pData;
                }
        }
}

void COptionsMotionDlg::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
        KillTimer(0);
        m_bLbtnDownDebug = false;
        NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

        int nActionBlockID = m_Grid.GetItemData(pItem->iRow, 0);
        if (pItem->iColumn == 0 && nActionBlockID >= 0)
                {
                        m_nCol = nActionBlockID;
                        PlayBlock(nActionBlockID);
                }
}

void COptionsMotionDlg::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
        m_bLbtnDownDebug = true;
        NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
        int nActionBlockID = m_Grid.GetItemData(pItem->iRow, 0);
        if (pItem->iColumn == 0 && nActionBlockID >= 0)
        {
                if (m_pEvent)
                {
                        //m_nCurrentGroupID为组ID号，非Index，而pItem->iRow -1为该ACTION在group中的index
                        m_pEvent->ILayerOptionHandler_OnSelectActionManageChange(m_nCurrentGroupID , pItem->iRow - 1);
                }
                m_nCol = nActionBlockID;
                KillTimer(0);
                SetTimer(0,500,NULL);
        }
        m_ShowId.clear();
}

void COptionsMotionDlg::OnRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
        NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
        CMenu menuContext;
        CMenu *pSubMenu;
        menuContext.m_hMenu  = ::language_loadmenu(IDR_MENU_MOTIONLAYER);
        pSubMenu = menuContext.GetSubMenu(0);

        bool bIsSelect = IsGridHasSelectRow();
        if (bIsSelect == false)
        {
                // 没有块选中,复制，删除，剪切，不可用
                //pSubMenu->EnableMenuItem(ID_MENU_COPY_BLOCK, MF_BYCOMMAND|MF_GRAYED);
                //pSubMenu->EnableMenuItem(ID_MENU_CUT_BLOCK, MF_BYCOMMAND|MF_GRAYED);
                //pSubMenu->EnableMenuItem(ID_MENU_DELETE_BLOCK, MF_BYCOMMAND|MF_GRAYED);

                pSubMenu->EnableMenuItem(ID_MENU_MOTIONLAYER_COPY, MF_BYCOMMAND|MF_GRAYED);
                pSubMenu->EnableMenuItem(ID_MENU_MOTIONLAYER_DELETE, MF_BYCOMMAND|MF_GRAYED);
                pSubMenu->EnableMenuItem(ID_MENU_MOTIONLAYER_INSERT, MF_BYCOMMAND|MF_GRAYED);
        }

        // 显示MENU
    QPoint pos;
        GetCursorPos(&pos);
    uint nCommand = CXTPCommandBars::TrackPopupMenu(pSubMenu, TPM_RETURNCMD, pos.x, pos.y, this);
        DoMenuCommand(nCommand, pos, pItem->iRow);
        menuContext.DestroyMenu();
}

int COptionsMotionDlg::GetSelectRowCount()
{
        int nSelectCount = 0;
        for (int i=0; i<m_Grid.GetRowCount(); i++)
        {
                if (IsFullRowSelect(i))
                        nSelectCount++;
        }

        return nSelectCount;
}

int COptionsMotionDlg::GetFirstSelectRowID()
{
        int nSelectID = -1;
        for (int i=0; i<m_Grid.GetRowCount(); i++)
        {
                if (IsFullRowSelect(i))
                {
                        nSelectID = m_Grid.GetItemData(i, 0);

                        break;
                }
        }

        return nSelectID;
}

void COptionsMotionDlg::OnCopy()
{
        if (m_nCurrentGroupID == -1)
                return;

        if (IsGridHasSelectRow() == false)
                return;

        if (m_pGroupManager == NULL)
                return;

        CActionBlockGroupManager* pManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);
        if (pManager == NULL)
                return;

        int nSelectRowCount = GetSelectRowCount();
        vector<char> RawData;

        // 保存选择的行数
        AddVectorData(&RawData, (char*)&nSelectRowCount, sizeof(nSelectRowCount));
        for (int i=0; i<m_Grid.GetRowCount(); i++)
        {
                if (IsFullRowSelect(i) == false)
                        continue;

                int nBlockDataID = m_Grid.GetItemData(i, 0);
                CActionBlockData* pBlockData = pManager->FindBlockData(nBlockDataID);
                if (pBlockData == NULL)
                        continue;

                char* pData = NULL;
                int nLen = 0;
                pBlockData->GetData(&pData, nLen);
                if (pData)
                {
                        // 先保存长度
                        AddVectorData(&RawData, (char*)&nLen, sizeof(nLen));
                        // 保存数据
                        AddVectorData(&RawData, pData, nLen);

                        delete [] pData;
                }
        }

        int nTotalLen = RawData.size();
        char* pData = new char[nTotalLen];
        CopyVertorToPointer(RawData, pData);


    uint format = ::RegisterClipboardFormat(L"OptionsMotionDlg_CLIPBOARD");
        if(!this->OpenClipboard())
        {
                delete [] pData;

                return;
        }

        EmptyClipboard();

        HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, nTotalLen);
        char * buffer = (char*)GlobalLock(clipbuffer);

        memcpy(buffer, pData, nTotalLen);

        GlobalUnlock(clipbuffer);

        SetClipboardData(format,clipbuffer);

        CloseClipboard();

        if (pData && nTotalLen)
        {
                delete [] pData;
        }
}

void COptionsMotionDlg::OnPast()
{
        //if (m_nCurrentGroupID == -1)
        //	return;

    uint format = ::RegisterClipboardFormat(L"OptionsMotionDlg_CLIPBOARD");
        if(!this->OpenClipboard())
        {
                return;
        }

        HANDLE hData =GetClipboardData(format);
        if (hData)
        {
                char * buffer = (char*)GlobalLock(hData);
                int nLen = GlobalSize(hData);

                int nTotalRow = 0;
                UBX_MEMCPY_INC(nTotalRow, buffer);

                for (int i=0; i<nTotalRow; i++)
                {
                        int nLen =0;
                        UBX_MEMCPY_INC(nLen, buffer);

                        char* pData = new char[nLen];
                        memcpy(pData, buffer, nLen);
                        buffer += nLen;

                        CActionBlockData* pActionBlockData = new CActionBlockData();
                        pActionBlockData->SetData(pData, nLen);
                        pActionBlockData->SetID(-1);
			
                        if (pData)
                        {
                                delete [] pData;
                        }

                        int nCurrentID = GetCurrenGroupManagerID();
                        m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData);

                        if (nCurrentID != -1)
                        {
                                int nID = pActionBlockData->GetID();
                                SetShowID(&nID, 1);

                                char* pExtDate = NULL;
                                int nExtLen = 0;
                                pActionBlockData->GetExtBlockData(&pExtDate, nExtLen);
                                AddToGrid(pExtDate, nExtLen, pActionBlockData->GetDescription(), pActionBlockData->GetRunTime(), pActionBlockData->GetAllTime(), pActionBlockData->GetID());
                                ShowSelectActionBlock();

                                if (pExtDate)
                                {
                                        delete [] pExtDate;
                                }
                        }
                }

                GlobalUnlock(hData);
        }

        CloseClipboard();
}

void COptionsMotionDlg::OnInsert()
{
        if (m_nCurrentGroupID == -1)
                return;

    uint format = ::RegisterClipboardFormat(L"OptionsMotionDlg_CLIPBOARD");
        if(!this->OpenClipboard())
        {
                return;
        }

        if (m_pGroupManager == NULL)
                return;

        int nSelectBlockID = GetFirstSelectRowID();
        CActionBlockGroupManager* pGroupManager = m_pGroupManager->FindBlockData(m_nCurrentGroupID);

        HANDLE hData =GetClipboardData(format);
        if (hData)
        {
                char * buffer = (char*)GlobalLock(hData);
                int nLen = GlobalSize(hData);

                int nTotalRow = 0;
                UBX_MEMCPY_INC(nTotalRow, buffer);

                for (int i=0; i<nTotalRow; i++)
                {
                        int nLen =0;
                        UBX_MEMCPY_INC(nLen, buffer);

                        char* pData = new char[nLen];
                        memcpy(pData, buffer, nLen);
                        buffer += nLen;

                        CActionBlockData* pActionBlockData = new CActionBlockData();
                        pActionBlockData->SetData(pData, nLen);
                        pActionBlockData->SetID(-1);

                        if (pData)
                        {
                                delete [] pData;
                        }
			
                        pGroupManager->InsertBeforeId(nSelectBlockID, pActionBlockData);
                        int nCurrentShowID = pActionBlockData->GetID();
                        SetShowID(&nCurrentShowID, 1);

                        int nCurrentID = m_nCurrentGroupID;
                        m_nCurrentGroupID = -1;
                        m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, pActionBlockData->GetAllTime());
                        //UpdateUIByData();

                        /*int nCurrentID = GetCurrenGroupManagerID();
                        m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurrentID, pActionBlockData);

                        if (nCurrentID != -1)
                        {
                        int nID = pActionBlockData->GetID();
                        SetShowID(&nID, 1);

                        char* pDate = NULL;
                        int nLen = 0;
                        pActionBlockData->GetExtBlockData(&pDate, nLen);
                        AddToGrid(pData, nLen, pActionBlockData->GetStartTime(), pActionBlockData->GetEndTime(), pActionBlockData->GetID());
                        ShowSelectActionBlock();

                        if (pData)
                        {
                        delete [] pData;
                        }
                        }*/
                }
                UpdateUIByData();
                GlobalUnlock(hData);
        }
        CloseClipboard();
}

void COptionsMotionDlg::DoMenuCommand(uint nCommand, QPoint point, int nRow)
{
        switch (nCommand)
        {
        case ID_MENU_MOTIONLAYER_COPY:
                OnCopy();
                break;

        case ID_MENU_MOTIONLAYER_PASTE:
                OnPast();
                break;

        case ID_MENU_MOTIONLAYER_INSERT:
                OnInsert();
                break;

        case ID_MENU_MOTIONLAYER_DELETE:
                OnBnClickDelete();
                break;
        }
}

void COptionsMotionDlg::OnOK()
{
        // TODO: 在此添加专用代码和/或调用基类

        //__super::OnOK();
}


void COptionsMotionDlg::OnCancel()
{
        // TODO: 在此添加专用代码和/或调用基类

        //__super::OnCancel();
}

void COptionsMotionDlg::SetControlState(bool controlState)
{
        this->EnableWindow(controlState);
}
void COptionsMotionDlg::SetMusicBeatsTime(int nBeatTime)
{
        if (nBeatTime > GRID_MAX_TIMEVALUE)
        {
                nBeatTime = GRID_MAX_TIMEVALUE;
        }
        else if (nBeatTime < GRID_MIN_TIMEVALUE)
        {
                nBeatTime = GRID_MIN_TIMEVALUE;
        }
    QString str;
    str.Format(QObject::tr("%d"),nBeatTime);
        m_editBeatValue.SetWindowText(str);
}

void COptionsMotionDlg::OnTimer(uint_PTR nIDEvent)
{
        // TODO: 在此添加消息处理程序代码和/或调用默认值
        if (m_nCol >= 0)
        {
                ModleView(m_nCol);
        }
        KillTimer(nIDEvent);
        __super::OnTimer(nIDEvent);
}
