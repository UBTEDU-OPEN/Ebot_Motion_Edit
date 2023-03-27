#ifndef _UITIMELAYER_H_
#define _UITIMELAYER_H_


#include "ILayerUI.h"
#include "UILayerCommonImpl.h"

/************************************************************************/
/* 时间层实现类                                                                     */
/************************************************************************/
class CUILayerCommonImpl;
class CUITimeLayer: public CUILayerCommonImpl
{
public:
    CUITimeLayer(void);
    ~CUITimeLayer(void);

    int GetCurrentSelTime(){return m_currentSel;};

public:
    virtual bool Connect(QWidget* managerHandle);
    virtual bool ReleaseConnection();

    //绘自己
    virtual void OnDraw(/*CDC* pDC*/);
    virtual bool GetData(char** ppData, int& nLen);
    virtual bool SetData(char* pData, int nLen);
    //取得类型，具体见宏定义
    virtual int GetType();
    virtual QString GetLayerName();
    virtual	void SetPosition(int PositionX, int PositionY);
    virtual void SetHScrollOffset(int offset);

    //设置当前选中的列
    void SetCurrentSelectCol(int nCol);
    int GetCurrentSelectCol();

private:
    QWidget* m_ManagerHandle;

public:
    int m_lineThick;				//线条宽度
    int m_lineLength_selected;		//选中线条长度
    COLORREF m_lineColor_default;	//默认线条颜色
    COLORREF m_bgColor_default;		//默认背景颜色
    COLORREF m_lineColor_selected;	//选中线条颜色
    COLORREF m_bgColor_selected;	//选中背景颜色
    COLORREF m_textColor;			//刻度颜色
    int m_divisor;					//是它的倍数则显示刻度
    int x, y;						//起始坐标
    int w, h;						//刻度高度
    int m_start, m_end;				//从开始到结束
    int m_currentSel;				//当前选中的列

    int m_HScrollOffset;
};
#endif  //_UITIMELAYER_H_
