//// AddLayerDlg.cpp : ʵ���ļ�
////

//#include "stable.h"
//#include "UBXMotionEditor.h"
//#include "AddLayerDlg.h"
//#include "afxdialogex.h"
//#include "uilayercontroltoolsdlg.h"


//// CAddLayerDlg �Ի���

//IMPLEMENT_DYNAMIC(CAddLayerDlg, CDialog)

//CAddLayerDlg::CAddLayerDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CAddLayerDlg::IDD, pParent)
//	, m_pEvnet(NULL)
//	, m_pIDs(NULL)
//{

//}

//CAddLayerDlg::~CAddLayerDlg()
//{
//	if (m_pIDs)
//	{
//		delete [] m_pIDs;
//		m_pIDs = NULL;
//	}
//}

//void CAddLayerDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_LIST_ADDLAYER, m_lstLayer);
//}


//BEGIN_MESSAGE_MAP(CAddLayerDlg, CDialog)
//END_MESSAGE_MAP()


//// CAddLayerDlg ��Ϣ�������


//BOOL CAddLayerDlg::OnInitDialog()
//{
//	CDialog::OnInitDialog();

//	DWORD dwStyle = m_lstLayer.GetExtendedStyle();
//	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
//	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
//	dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
//	m_lstLayer.SetExtendedStyle(dwStyle); //������չ���

//	m_lstLayer.InsertColumn( 1, L"", LVCFMT_LEFT, 400 );//������

//	InitListCtrl();

//	return TRUE;  // return TRUE unless you set the focus to a control
//	// �쳣: OCX ����ҳӦ���� FALSE
//}

//void CAddLayerDlg::InitListCtrl()
//{
//	int* pIds = NULL;
//	int nCount;

//	if (m_pEvnet)
//	{
//		m_pEvnet->LayerControlToolsEvent_GetUnUseLayers(&pIds, nCount);
//	}

//	for (int i=0; i<nCount; i++)
//	{
//		CString strLayerName;
		
//		strLayerName = m_pEvnet->LayerControlToolsEvent_GetLayerName(pIds[i]);
//		int nItem = m_lstLayer.InsertItem(m_lstLayer.GetItemCount(), strLayerName.GetBuffer());

//		m_lstLayer.SetItemData(nItem, pIds[i]);
//	}

//	if (pIds)
//	{
//		delete[] pIds;
//	}
//}

//void CAddLayerDlg::GetSelectLayer(int** ppIDS, int &nCount)
//{
//	vector<int> selectLayer;

//	for (int i=0; i<m_lstLayer.GetItemCount(); i++)
//	{
//		if (m_lstLayer.GetCheck(i) == FALSE)
//			continue;

//		int nLayerID = m_lstLayer.GetItemData(i);
//		selectLayer.push_back(nLayerID);
//	}

//	nCount = selectLayer.size();
//	if (nCount)
//	{
//		*ppIDS = new int[nCount];
//	}
//	else
//	{
//		*ppIDS = NULL;
//	}

//	for (int i=0; i<selectLayer.size(); i++)
//	{
//		(*ppIDS)[i] = selectLayer[i];
//	}
//}

//void CAddLayerDlg::OnOK()
//{
//	if (m_pIDs)
//	{
//		delete [] m_pIDs;
//		m_pIDs = NULL;
//	}

//	GetSelectLayer(&m_pIDs, m_nSelectCount);

//	CDialog::OnOK();
//}
