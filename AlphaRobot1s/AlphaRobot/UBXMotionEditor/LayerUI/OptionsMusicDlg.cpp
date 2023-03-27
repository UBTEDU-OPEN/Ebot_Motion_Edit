//// OptionsMotionDlg.cpp : 实现文件
////

//#include "stdafx.h"
//#include "UBXMotionEditor.h"
//#include "OptionsMusicDlg.h"
//#include "afxdialogex.h"
//#include "ActionBlockDataManager.h"
//#include "MusicBlockData.h"
//#include "MusicBlockGroupManager.h"

//bool CreateMusicLayOptionHandler(ILayerOptionHandler** ppHandler)
//{
//	COptionsMusicDlg* pProxy = new COptionsMusicDlg();
//	if (pProxy)
//	{
//		*ppHandler = pProxy;

//		return true;
//	}

//	return false;
//}
//// COptionsMusicDlg 对话框

//IMPLEMENT_DYNAMIC(COptionsMusicDlg, CDialog)

//COptionsMusicDlg::COptionsMusicDlg(QWidget* pParent /*=NULL*/)
//	: CDialog(COptionsMusicDlg::IDD, pParent)
//	, m_pEvent(NULL)
//	, m_nCurrentGroupID(-1)
//	, m_CurSelectBlockId(-1)
//	, m_nCurrentSelectItem(-1)
//	, m_CurSelectBlockItem(-1)
//	, m_DefaultNameNum(0)
//{
	
//}

//COptionsMusicDlg::~COptionsMusicDlg()
//{
	
//}

//void COptionsMusicDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_LIST_MUSIC_GROUP, m_musicGroupList);
//	DDX_Control(pDX, IDC_LIST_MUSIC_BLOCK, m_musicBlockList);
//}


//BEGIN_MESSAGE_MAP(COptionsMusicDlg, CDialog)
//	ON_WM_SIZE()
//	ON_NOTIFY(NM_CLICK, IDC_LIST_MUSIC_GROUP, &COptionsMusicDlg::OnNMClickListGroupList)
//	ON_NOTIFY(NM_CLICK, IDC_LIST_MUSIC_BLOCK, &COptionsMusicDlg::OnNMClickListBlockList)
//	ON_NOTIFY(NM_RCLICK, IDC_LIST_MUSIC_GROUP, &COptionsMusicDlg::OnNMRClickGroupList)
//	ON_NOTIFY(NM_RCLICK, IDC_LIST_MUSIC_BLOCK, &COptionsMusicDlg::OnNMRClickBlockList)
//	ON_WM_DESTROY()
//END_MESSAGE_MAP()


//bool COptionsMusicDlg::Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent)
//{
//	if (Create(COptionsMusicDlg::IDD, pParent))
//	{
//		m_pEvent = pEvent;

//		return true;
//	}

//	return false;
//}

//void COptionsMusicDlg::ReleaseConnection()
//{
//	if (GetSafeHwnd())
//	{
//		DestroyWindow();
//	}

//	delete this;
//}

//QWidget* COptionsMusicDlg::GetQWidget()
//{
//	return this;
//}
//// COptionsMusicDlg 消息处理程序


//bool COptionsMusicDlg::OnInitDialog()
//{
//	__super::OnInitDialog();

//// 	m_optionDlg.Create(CMusicChooseDialog::IDD, this);
//// 	m_optionDlg.SetWindowPos(NULL, 0, 0, 50, 22, SWP_SHOWWINDOW);

//	DWORD dwStyle = m_musicGroupList.GetExtendedStyle();
//	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
//	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
//	dwStyle |= LVS_EX_SUBITEMIMAGES;//填充图片
//	dwStyle |= LVS_SHOWSELALWAYS;
//	m_musicGroupList.SetExtendedStyle(dwStyle);
//	m_musicBlockList.SetExtendedStyle(dwStyle);

//	m_musicGroupList.InsertColumn( 0, _T("音乐层组列表"), LVCFMT_LEFT, 150 );
//	m_musicBlockList.InsertColumn( 0, _T("音乐层块列表"), LVCFMT_LEFT, 150);

//	m_imageList.Create(16, 16, ILC_COLOR24 ,5,1);
//	m_musicGroupList.SetImageList(&m_imageList, LVSIL_SMALL);
//	m_musicBlockList.SetImageList(&m_imageList, LVSIL_SMALL);
//	m_imageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_ACCEPT));
//	//ShowWindow(SW_HIDE);

//	return true;  // return true unless you set the focus to a control
//	// 异常: OCX 属性页应返回 false
//}


//void COptionsMusicDlg::OnSize(UINT nType, int cx, int cy)
//{
//	__super::OnSize(nType, cx, cy);
///*	ShowWindow(SW_HIDE);*/
//	if (m_optionDlg.GetSafeHwnd() == NULL)
//		return;

//	CRect rc;
//	GetClientRect(rc);
//	m_musicGroupList.SetWindowPos(NULL, 0, 0, rc.Width()/4, cy, SWP_SHOWWINDOW);
//	m_musicBlockList.SetWindowPos(NULL, rc.Width()/4 + 3, 0, rc.Width()/4, cy, SWP_SHOWWINDOW);
//	m_optionDlg.MoveWindow(rc.Width()/2 + 30, 0, rc.Width()/2 - 6, cy);
	
//}

//void COptionsMusicDlg::SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID)
//{
//	if (m_nCurrentGroupID == nCurrentGroupID)
//		return;

//	m_nCurrentGroupID = nCurrentGroupID;
//	m_pGroupManager = pManager;
//	UpdateUIByData();

//	if (nCurrentGroupID == -1)
//	{
//		m_nCurrentSelectItem = -1;
//		m_CurSelectBlockItem = -1;
//	}
//	else
//	{
//		for (int i=0; i<m_musicGroupList.GetItemCount(); i++)
//		{
//			if (m_musicGroupList.GetItemData(i) == nCurrentGroupID)
//			{
//				m_nCurrentSelectItem = i;
//				break;
//			}
//		}
//	}

//	SetCurrentSelectItem(m_nCurrentSelectItem);
//}

//void COptionsMusicDlg::UpdateUIByData()
//{
//	if (m_pGroupManager == NULL)
//		return;

//	// 先清空LIST
//	m_musicGroupList.DeleteAllItems();

//	for (int i=0; i<m_pGroupManager->GetCount(); i++)
//	{
//		CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(i);
//		int nItem = m_musicGroupList.InsertItem(m_musicGroupList.GetItemCount(), pGroup->GetDescription(), -1);
//		m_musicGroupList.SetItemData(nItem, pGroup->GetID());
//	}
//}

//void COptionsMusicDlg::ModifyBlockInfo()
//{
//	if (m_CurSelectBlockItem == -1 || m_nCurrentSelectItem == -1)
//		return;

//	if (m_musicBlockList.GetItemCount() <= 0)
//		return;

//	int nTempID = m_musicBlockList.GetItemData(m_CurSelectBlockItem);
//	CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(m_nCurrentSelectItem);
//	if (pGroup)
//	{
//		CActionBlockData* pActionBlock = pGroup->FindBlockData(nTempID);
//		if (pActionBlock)
//		{
//			int timeInc = pActionBlock->GetAllTime();

//			pActionBlock->SetRunTime(m_optionDlg.m_nEndTime - m_optionDlg.m_nStartTime);
//			pActionBlock->SetAllTime(m_optionDlg.m_nEndTime - m_optionDlg.m_nStartTime);
//			pActionBlock->SetDescription(GetName(m_optionDlg.m_EditFilePath));

//			char* pData = NULL;
//			int nLen = 0;
//			nLen = m_optionDlg.GetRawData(&pData);
//			if (nLen && pData)
//			{
//				pActionBlock->SetExtBlockData(nLen, pData);
//			}

//			delete []pData;
//			FileCopyTo(m_optionDlg.m_EditFilePath, true);

//			timeInc -= (m_optionDlg.m_nStartTime + m_optionDlg.m_nEndTime);
//			m_pEvent->ILayerOptionHandler_OnGroupManagerChange(m_nCurrentGroupID, -timeInc);
//		}
//	}
//}

//void COptionsMusicDlg::DeleteLayerBlock()
//{
//	if (m_CurSelectBlockItem == -1)
//		return;

//	int nTimeInc = 0;
//	CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(m_nCurrentSelectItem);
//	if (pGroup == NULL)
//		return;

//	QString FileName = L"";
//	for(int nItem = 0; nItem < m_musicBlockList.GetItemCount(); nItem++)
//	{
//		if(m_musicBlockList.GetItemState(nItem, LVNI_ALL | LVNI_SELECTED) == LVIS_SELECTED)
//		{
			
//			int nTempID = m_musicBlockList.GetItemData(nItem);
//			CActionBlockData* pActionBlockData = pGroup->FindBlockData(nTempID);
//			nTimeInc += pActionBlockData->GetAllTime();
//			FileName = pActionBlockData->GetDescription();
//			pGroup->DeleteLayerBlock(nTempID);
//		}
//	}

//	if (m_pEvent)
//	{
//		m_pEvent->ILayerOptionHandler_OnGroupManagerChange(m_nCurrentGroupID, -nTimeInc);
//		m_pEvent->ILayerOptionHandler_OnDeleteDataBlock(m_nCurrentGroupID);
//	}
//	UpdateBlockList(pGroup);
//	DeleteFile(GetModuleDir()+L"\\Music//"+FileName);

//	if (m_musicBlockList.GetItemCount() <= 0)
//	{
//		m_CurSelectBlockItem = -1;
//	}
//}

//void COptionsMusicDlg::AddLayerBlock()
//{
//	if (m_pEvent && m_optionDlg)
//	{
//		CActionBlockData* pActionBlock = new CActionBlockData();
//		if (pActionBlock == NULL)
//			return;

//		QString tempDescriptionInfo = m_optionDlg.m_EditFilePath;
//		if (!tempDescriptionInfo.Compare(_T("DefaultName")))
//		{
//			QString tempStr;
//			tempStr.Format(_T("%d"), m_DefaultNameNum);
//			tempDescriptionInfo = _T("DefaultName") + tempStr;
//		}

//		pActionBlock->SetID(-1);
//		pActionBlock->SetRunTime(m_optionDlg.m_nEndTime - m_optionDlg.m_nStartTime);
//		pActionBlock->SetAllTime(m_optionDlg.m_nEndTime - m_optionDlg.m_nStartTime);
//		pActionBlock->SetDescription(GetName(tempDescriptionInfo));



////		pActionBlock->SetExtBlockData(0, NULL);
//		char* pData = NULL;
//		int nLen = 0;
//		nLen = m_optionDlg.GetRawData(&pData);
//		if (nLen && pData)
//		{
//			pActionBlock->SetExtBlockData(nLen, pData);
//		}
		
//		delete []pData;

//		int nCurID = GetCurrenGroupManagerID();
//		m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurID, pActionBlock);
//		FileCopyTo(tempDescriptionInfo, true);

//		if (nCurID != -1)
//		{
//			int nItem = m_musicBlockList.InsertItem(m_musicBlockList.GetItemCount(), GetName(tempDescriptionInfo), -1);
//			m_musicBlockList.SetItemData(nItem, pActionBlock->GetID());
//			SetCurSelectBlockItem(nItem);
//		}
//		m_DefaultNameNum++;
//	}
//}
//void COptionsMusicDlg::AddMusicFromGroup(QString strFilePath)
//{

////     if (m_optionDlg.GetSafeHwnd())
////     {
//// 		//需要转换到debug目录下的music音乐文件
//// 		QString strFileNameWithExt =_T("NULL");
//// 		int nPos = strFilePath.ReverseFind(_T('\\'));
//// 		if (nPos >= 0)
//// 		{
//// 			strFileNameWithExt = strFilePath.Right(strFilePath.GetLength() - nPos - 1);
//// 		}
//// 		QString strDesPath = GetModuleDir()+L"\\Music";
//// 	    strDesPath = strDesPath + L"//" + strFileNameWithExt;
//// 		m_optionDlg.AddMusicResource(strDesPath );
////
////     }
//}
//void COptionsMusicDlg::AddLayerBlock1S(void *pData , int nDataLen , int nDataID)
//{
//	if (nDataID != 1)
//	{
//		//判断是不是音乐层操作 nDataID以后可以实际定制，方便扩展
//		return;
		
//	}
//	if (m_pEvent && m_optionDlg.GetSafeHwnd())
//	{
//		CActionBlockData* pActionBlock = new CActionBlockData();
//		if (pActionBlock == NULL)
//			return;
//		UBXMusicInfo *pUBXMusicInfo = (UBXMusicInfo *)pData;
//		//比较数据包的长度是否一致
//		if (nDataLen != sizeof(UBXMusicInfo))
//		{
//			return;
//		}
//		int nScale = 10;
//		if (m_pEvent)
//		{
//			nScale = m_pEvent->ILayerOptionHandler_GetScaleValue();
//		}

//		int nTotalMiliSec = pUBXMusicInfo->StreamTime.ms;//音乐时长
//		float fEndTime = (int)nTotalMiliSec/nScale; //此处不是float???
//		if (nTotalMiliSec % nScale != 0)
//		{
//			pActionBlock->SetRunTime(fEndTime + 1);//为啥要+1 ???
//			pActionBlock->SetAllTime(fEndTime + 1);//为啥要+1 ???
//			//pActionBlock->SetActualEndTime((int)nTotalMiliSec/nScale + 1);
//		}
//		else
//		{
//			pActionBlock->SetRunTime(fEndTime);
//			pActionBlock->SetAllTime(fEndTime);
//			//pActionBlock->SetActualEndTime((int)nTotalMiliSec/nScale);
//		}
		
//		pActionBlock->SetID(-1);
//		pActionBlock->SetDescription(pUBXMusicInfo->szFilePath);
//		pActionBlock->SetExtBlockData(0, NULL);
//		int nCurID = GetCurrenGroupManagerID();
		
//		if (nCurID != -1)
//		{
//			//删除当前图层
//			if (m_pEvent)
//			{
//				//m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(m_nCurrentGroupID);
//				m_pEvent->ILayerOptionHandler_OnDeleteGroupBlock();
//			}
//			nCurID = -1;
//		}
//		if (m_pEvent )
//		{
//			/*m_optionDlg.m_EditFilePath = / *pBlockData->GetDescription();* /strBlockTitle;*/
//			m_pEvent->ILayerOptionHandler_OnAddActionBlock(nCurID, pActionBlock,pUBXMusicInfo->szFilePath);
//			m_pEvent->ILayerOptionHandler_SetBPMValue(pUBXMusicInfo->nBPM);
//			//m_pEvent->ILayerOptionHandler_OnAddWave(pUBXMusicInfo->szWavePath);
//		}
//		//需要先删除debug目录下的音乐文件，再添加
//		FileCopyTo(pUBXMusicInfo->szFilePath, true);
//// 		if (pUBXMusicInfo->szWavePath)
//// 		{
//// 			FileCopyTo(pUBXMusicInfo->szWavePath,true);
//// 		}
//		if (nCurID != -1)
//		{
//			int nItem = m_musicBlockList.InsertItem(m_musicBlockList.GetItemCount(), GetName(pUBXMusicInfo->szFilePath), -1);
//			m_musicBlockList.SetItemData(nItem, pActionBlock->GetID());
//			SetCurSelectBlockItem(nItem);
//		}
//		m_DefaultNameNum++;
//	}
//}
//void COptionsMusicDlg::InsertLayerBlock()
//{
//	if (m_nCurrentSelectItem == -1 || m_CurSelectBlockItem == -1)
//		return;

//	if (m_musicBlockList.GetItemCount() <= 0)
//		return;

//	int nTempID = m_musicBlockList.GetItemData(m_CurSelectBlockItem);
//	CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(m_nCurrentSelectItem);
//	if (pGroup)
//	{
//		CActionBlockData* pActionBlock = new CActionBlockData();
//		if (pActionBlock == NULL)
//			return;

//		QString tempDescriptionInfo = m_optionDlg.m_EditFilePath;
//		if (!tempDescriptionInfo.Compare(_T("DefaultName")))
//		{
//			QString tempStr;
//			tempStr.Format(_T("%d"), m_DefaultNameNum);
//			tempDescriptionInfo = _T("DefaultName") + tempStr;
//		}

//		pActionBlock->SetID(-1);
//		pActionBlock->SetRunTime(m_optionDlg.m_nEndTime - m_optionDlg.m_nStartTime);
//		pActionBlock->SetAllTime(m_optionDlg.m_nEndTime - m_optionDlg.m_nStartTime);
//		pActionBlock->SetDescription(GetName(tempDescriptionInfo));

////		pActionBlock->SetExtBlockData(0, NULL);

//		char* pData = NULL;
//		int nLen = 0;
//		nLen = m_optionDlg.GetRawData(&pData);
//		if (nLen && pData)
//		{
//			pActionBlock->SetExtBlockData(nLen, pData);
//		}
//		delete []pData;

//		pGroup->InsertBeforeId(nTempID, pActionBlock);
//		int nCurrentID = m_nCurrentGroupID;
//		m_nCurrentGroupID = -1;
//		m_pEvent->ILayerOptionHandler_OnGroupManagerChange(nCurrentID, -pActionBlock->GetAllTime());
//		FileCopyTo(m_optionDlg.m_EditFilePath, true);
//	}
//}

//bool COptionsMusicDlg::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//	if (wParam == ID_MENU_SWITCH_DELETE)
//	{
//		DeleteLayerBlock();
//	}

//	return __super::OnCommand(wParam, lParam);
//}
//void COptionsMusicDlg::PlayMusic(int nStartTime, int nEndTime, int nPlayState)
//{
//	if (m_optionDlg.GetSafeHwnd())
//	{
//		m_optionDlg.PlayMusic(nStartTime, nEndTime, nPlayState);
//	}
//}
//void COptionsMusicDlg::SetMusicResource(QString strFilePath)
//{
//	if (m_optionDlg.GetSafeHwnd())
//	{
//		m_optionDlg.m_nCurrentScale = GetScaleValue();
//		//m_optionDlg.m_nCurrentBPM = (int) (SecsPerMinute / m_optionDlg.m_nCurrentScale);
//		//m_optionDlg.m_MusicInfo.nBPM= (int)(SecsPerMinute / m_optionDlg.m_nOriginScale);
//		if (m_optionDlg.AddMusicResource(strFilePath))
//		{
//			m_optionDlg.OnCtrlShow();
//		}

//	}
//}
//int COptionsMusicDlg::GetCurrentPos()
//{
//	if (m_optionDlg.GetSafeHwnd())
//	{
//		return m_optionDlg.GetCurrentPos();
//	}
//	else
//		 return -1;
//}
//void COptionsMusicDlg::OnNMRClickGroupList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

//	// 	CMenu menu;
//	// 	menu.LoadMenu(IDR_MENU_SWITCH_LOGIC);
//	// 	CMenu *pContextMenu = menu.GetSubMenu(0);
//	//
//	// 	QPoint point1;
//	// 	GetCursorPos(&point1);
//	// 	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point1.x, point1.y, this);

//	*pResult = 0;
//}

//void COptionsMusicDlg::OnNMRClickBlockList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

////	m_CurSelectBlockItem = pNMItemActivate->iItem;

//	if (pNMItemActivate->iItem != -1)
//	{
//		SetCurSelectBlockItem(pNMItemActivate->iItem);
//	}

//	CMenu menu;
//	menu.LoadMenu(IDR_MENU_SWITCH_LOGIC);
//	CMenu *pContextMenu = menu.GetSubMenu(0);

//	QPoint point1;
//	GetCursorPos(&point1);
//	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point1.x, point1.y, this);

//	*pResult = 0;
//}

//void COptionsMusicDlg::OnNMClickListGroupList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

//	if (pNMItemActivate->iItem != -1)
//	{
//		SetCurrentSelectItem(pNMItemActivate->iItem);
//		if (m_pEvent)
//		{
//			m_pEvent->ILayerOptionHandler_OnSelectGroupManageChange(m_nCurrentGroupID,0);
//		}
//	}

//	*pResult = 0;
//}

//void COptionsMusicDlg::OnNMClickListBlockList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

//	if (pNMItemActivate->iItem != -1)
//	{
//		SetCurSelectBlockItem(pNMItemActivate->iItem);
//	}

//	*pResult = 0;
//}
//void COptionsMusicDlg::SetScaleValue(int nScale)
//{
//	if (m_pEvent)
//	{
//		m_pEvent->ILayerOptionHandler_SetScaleValue(nScale);
//	}
//}
//int COptionsMusicDlg::GetScaleValue()
//{
//	if (m_pEvent)
//	{
//		return m_pEvent->ILayerOptionHandler_GetScaleValue();
//	}
//	else
//		return -1;
//}
//void COptionsMusicDlg::UpdateBlockOptionUI(CActionBlockData* pBlockData)
//{
//	m_optionDlg.m_nStartTime = 0/*pBlockData->GetStartTime()*/;
//	m_optionDlg.m_nEndTime = pBlockData->GetAllTime();
////	m_optionDlg.m_EditFilePath = pBlockData->GetDescription();

//	char* pData = NULL;
//	int nLen = 0;
//	pBlockData->GetExtBlockData(&pData, nLen);
//	if (pData && nLen)
//	{
//		m_optionDlg.SetRawData(pData, nLen);
//	}
//	delete [] pData;

//	m_optionDlg.UpdateData(false);
//}

//void COptionsMusicDlg::SetCurrentSelectItem(int nItem)
//{
//	if (m_nCurrentSelectItem != -1)
//	{
//		m_musicGroupList.SetItem(m_nCurrentSelectItem,0, LVIF_IMAGE, NULL, -1, 0, 0, 0);
//	}

//	if (nItem == -1)
//	{
//		m_musicBlockList.DeleteAllItems();
//		return;
//	}

//	m_musicGroupList.SetItem(nItem,0, LVIF_IMAGE, NULL, 0, 0, 0, 0);
//	m_nCurrentSelectItem = nItem;


//	if (m_pGroupManager)
//	{
//		CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(nItem);
//		if (pGroup)
//		{
//			m_nCurrentGroupID = pGroup->GetID();
//			UpdateBlockList(pGroup);
//		}
//	}
//}

//void COptionsMusicDlg::InsertGroupByMenu(int nCol)
//{

//}

//void COptionsMusicDlg::UpdateBlockList(CActionBlockGroupManager* pGroup)
//{
//	m_musicBlockList.DeleteAllItems();
//	for (int i=0; i<pGroup->GetActionCount(); i++)
//	{
//		CActionBlockData* pBlockData = pGroup->GetAt(i);
//		QString strFilePath = pBlockData->GetDescription();
//		if (pBlockData)
//		{
//// 			if(strFilePath.IsEmpty() || strFilePath.Find(_T(".mp3")) < 0)
//// 			{
//// 				return;
//// 			}
//			int nItem = m_musicBlockList.InsertItem(m_musicBlockList.GetItemCount(), pBlockData->GetDescription(), -1);
//			m_musicBlockList.SetItemData(nItem, pBlockData->GetID());
			
//			QString str = pBlockData->GetDescription();
//			//AddMusicFromGroup(pBlockData->GetDescription());
//			SetCurSelectBlockItem(nItem);

//		}
//	}
//}

//int COptionsMusicDlg::GetCurrenGroupManagerID()
//{
//	if (m_nCurrentSelectItem == -1)
//		return -1;

//	if (m_pGroupManager == NULL)
//		return -1;

//	CActionBlockGroupManager* pGroupManager = m_pGroupManager->GetAt(m_nCurrentSelectItem);
//	if (pGroupManager)
//	{
//		return pGroupManager->GetID();
//	}

//	return -1;
//}
//void COptionsMusicDlg::SetMusicFile(UBXStreamHMSTime hmsTime , QString strMusicName)
//{
    
//}
//void COptionsMusicDlg::DeleteMusicFile()
//{
//	if (m_optionDlg.GetSafeHwnd())
//	{
//		m_optionDlg.DeleteMusic();
//	}
//}
//void COptionsMusicDlg::SetCurSelectBlockItem(int nBlockItem)
//{
//	if (m_nCurrentSelectItem == -1)
//	{
//		return;
//	}

//	if (m_CurSelectBlockItem != -1)
//	{
//		m_musicBlockList.SetItem(m_CurSelectBlockItem, 0, LVIF_IMAGE, NULL, -1, 0, 0, 0);
//	}

//	m_musicBlockList.SetItem(nBlockItem, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0);
//	m_CurSelectBlockItem = nBlockItem;

//	int nTempID = m_musicBlockList.GetItemData(nBlockItem);
//	CActionBlockGroupManager* pGroup = m_pGroupManager->GetAt(m_nCurrentSelectItem);

//	if (pGroup)
//	{
//		CActionBlockData* pBlockData = pGroup->FindBlockData(nTempID);
//		if (pBlockData)
//		{
//			UpdateBlockOptionUI(pBlockData);
//		}
//	}
//}

//QString COptionsMusicDlg::GetModuleDir()
//{
//	HMODULE module = GetModuleHandle(0);
//	wchar_t pFileName[MAX_PATH];
//	GetModuleFileName(module, pFileName, MAX_PATH);

//	QString csFullPath(pFileName);
//	int nPos = csFullPath.ReverseFind( _T('\\') );
//	if( nPos < 0 )
//		return QString("");
//	else
//		return csFullPath.Left( nPos );
//}

//void COptionsMusicDlg::FileCopyTo(QString strSearchPath, bool cover)
//{
//	QString strDesPath = GetModuleDir()+L"\\Music";

//	CFileFind filefinder;

//	QString filename;

//	bool bfind = filefinder.FindFile(strSearchPath);

//	QString SourcePath, DisPath;

//	while (bfind)

//	{
//		bfind = filefinder.FindNextFile();

//		filename = filefinder.GetFileName();

//		SourcePath = strSearchPath;

//		DisPath = strDesPath + "//" + filename;

//		CopyFile(/*(LPCSTR)*/SourcePath, DisPath, cover);

//	}

//	filefinder.Close();
//}

//QString COptionsMusicDlg::GetName(QString strName)
//{
//	int nPos = strName.Find('\\');
//	QString sSubStr = strName;
//	while (nPos)
//	{
//		sSubStr = sSubStr.Mid(nPos+1,sSubStr.GetLength()-nPos);  //取'\'右边字符串
//		nPos = sSubStr.Find('\\');   //不包含'\'，函数值返回-1

//		if (nPos==-1)
//		{
//			nPos = 0;
//		}
//	}
//	return sSubStr;
//}


//void COptionsMusicDlg::OnOK()
//{
//	// TODO: 在此添加专用代码和/或调用基类

//	//__super::OnOK();
//}

//void COptionsMusicDlg::OnCancel()
//{
//	// TODO: 在此添加专用代码和/或调用基类

//	//__super::OnCancel();
//}

//void COptionsMusicDlg::SetControlState(bool controlState)
//{
//	if (GetSafeHwnd())
//	{
//		this->EnableWindow(controlState);
//	}
////	m_optionDlg.SetButtonEnableState(controlState);
//}


//void COptionsMusicDlg::OnDestroy()
//{
//	__super::OnDestroy();
//// 	if (m_pMP3Session)
//// 	{
//// 		m_pMP3Session->Release();
//// 	}
//	// TODO: 在此处添加消息处理程序代码
//}
