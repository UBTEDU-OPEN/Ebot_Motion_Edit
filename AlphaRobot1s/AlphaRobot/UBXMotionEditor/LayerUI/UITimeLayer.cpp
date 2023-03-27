#include "stable.h"
#include "UITimeLayer.h"
#include "IUILayerManager.h"
#include <QRgb>
/*#include "vld.h"*/

CUITimeLayer::CUITimeLayer(void)
{
//	m_pUILayerConfigure = NULL;

    x = 0;
    y = 0;
    w = 99;
    h = 20;

    m_start = 0;
    m_currentSel = 100;
    m_lineThick = 1;
    m_lineLength_selected = 500;
    m_lineColor_default = qRgb( 0,0,0);
    m_bgColor_default = qRgb( 221, 221, 221 );
    m_lineColor_selected = qRgb( 255, 0, 0 );
    m_bgColor_selected = qRgb( 255, 153, 153 );
    m_textColor = qRgb( 0, 0, 0 );

    m_HScrollOffset = 0;
}


CUITimeLayer::~CUITimeLayer(void)
{
}

bool CUITimeLayer::Connect(QWidget* managerHandle)
{
    m_ManagerHandle = managerHandle;
    return true;
}

bool CUITimeLayer::ReleaseConnection()
{
    return true;
}

void CUITimeLayer::OnDraw(/*CDC* pDC*/)
{
//	int hTmp = h / 4;
//	int m_CellCount = m_pLayerManager->GetCellCount();
//	int m_nCellWidth = m_pLayerManager->GetCellWidth();
//	int xStart, yStart;
//	GetPosition(xStart, yStart);
//	int xTmp = xStart;

//	//画选择线
//	CRect rc;
//	m_ManagerHandle->GetClientRect(rc);
//	int nCount = rc.Width()/(m_lineThick + w) + GetCellStartIndex();

//	int xPosition = m_currentSel - GetCellStartIndex()*100;
//	CPen pen( PS_SOLID, m_lineThick, m_lineColor_default );
//	pDC->SelectObject(&pen);
//	pDC->MoveTo( xPosition, 0 );
//	pDC->LineTo( xPosition, y + h + m_lineLength_selected );

//	for( UINT i = GetCellStartIndex(); i <= nCount; i++ )
//	{
//		if (i >= m_CellCount)
//			break;

//		//画刻度线
//		CPen pen( PS_SOLID, m_lineThick, m_lineColor_default );
//		CBrush brush( m_bgColor_default );
//		pDC->SelectObject( &pen );
//		pDC->SelectObject( &brush );

// 		pDC->MoveTo( xTmp, y + h );
// 		pDC->LineTo( xTmp, y + h - hTmp );

//		pDC->MoveTo(xTmp, y + h-1);
//		pDC->LineTo(xTmp+m_nCellWidth+m_lineThick, y+h-1);

//		//画文字
//		CFont font;
//		VERIFY(font.CreatePointFont( 100, _T("Arial"), pDC ));
//		CFont* def_font = pDC->SelectObject( &font );
// 		int nHour = (i + 1)  / 3600;
// 		int nMinute = ((i + 1)  % 3600) / 60 ;
//		int nSec = (i + 1)  % 60;
//        CString strTime = TimeStringCustomise(nHour,nMinute,nSec);

//		CSize m_size = pDC->GetTextExtent(strTime);
//		pDC->SetTextColor( this->m_textColor );
//		pDC->SetBkMode( TRANSPARENT );
//		CRect rt(m_lineThick + w + xTmp - m_size.cx / 2, y, m_lineThick + w + xTmp + w + m_size.cx / 2, y + h );
//		pDC->DrawText( strTime, &rt, DT_LEFT | DT_VCENTER| DT_SINGLELINE );
//		pDC->SelectObject( def_font );
//		font.DeleteObject();

//		xTmp += this->m_lineThick + w;
//	}
}

bool CUITimeLayer::GetData(char** ppData, int& nLen)
{
    if (ppData == NULL)
        return false;

    *ppData = new char[8];
    nLen = 8;

    int m_scaleValue = m_pLayerManager->GetTimeScaleValues();
    int m_CellCount = m_pLayerManager->GetCellCount();

    char* p = *ppData;
    memcpy(p, &m_scaleValue, sizeof(int));
    p += sizeof(int);
    memcpy(p, &m_CellCount, sizeof(int));

    return true;
}

bool CUITimeLayer::SetData(char* pData, int nLen)
{
    int m_scaleValue;
    int m_CellCount;

    if (nLen == 4)
    {
        m_scaleValue = 20;

        int nTimes = 0;
        memcpy(&nTimes, pData, 4);
        m_CellCount = nTimes/20;
    }
    else
    {
        memcpy(&m_scaleValue, pData, sizeof(int));
        memcpy(&m_CellCount, pData+sizeof(int), sizeof(int));

    }

    m_pLayerManager->SetTimeScaleValues(m_scaleValue);
    m_pLayerManager->SetCellCount(m_CellCount);

    return true;
}

// 取得类型，具体见宏定义
int CUITimeLayer::GetType()
{
    return ACTION_LAYER_TIME;
}

QString CUITimeLayer::GetLayerName()
{
    return QObject::tr("");
}

void CUITimeLayer::SetHScrollOffset(int offset)
{
    m_HScrollOffset = offset*(w + m_lineThick);
}

void CUITimeLayer::SetPosition(int PositionX, int PositionY)
{
    x = PositionX;
    y = PositionY;
}

void CUITimeLayer::SetCurrentSelectCol(int nCol)
{
    m_currentSel = nCol;
}

int CUITimeLayer::GetCurrentSelectCol()
{
    return m_currentSel;
}
