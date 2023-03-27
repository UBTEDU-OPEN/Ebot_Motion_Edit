#ifndef _ILAYERUI_H_
#define _ILAYERUI_H_

#include "ILayerManager.h"
#include "MusicChooseDialog.h"
#include "stable.h"
// 每一格的宽度
#define PER_FRAME_WIDTH			5


class IUIConfigure;
class LayerUI_Event;
class IUILayerManager;
/************************************************************************/
/* layer 层的接口类                                                                     */
/************************************************************************/
class ILayerUI
{
public:
    virtual ~ILayerUI(){}
    virtual bool Connect(QWidget* managerHandle) = 0;
    virtual bool ReleaseConnection() = 0;

    virtual void SetLayerManager(IUILayerManager* pManager) = 0;
    virtual IUILayerManager* GetLayerManager() = 0;
    virtual CActionBlockDataManager* GetBlockDataManager() = 0;

    // 清除选中状态d
    virtual void ClearSelectState() = 0;
    // 鼠标双击事件处理
    virtual void OnLButtonDBClick(int nCol) = 0;
    // 鼠标单击事件处理
    virtual void OnLButtonDown(int nCol, QPoint point, float fStartTime) = 0;
    virtual void OnLButtonDownAndShift(int nCol) = 0;
    virtual void OnLButtonUpAndShift() = 0;
    virtual void OnLButtonUp(QPoint point) = 0;
    virtual void OnMouseMove(QPoint point) = 0;

    virtual void OnRButtonDdown(int nCol) = 0;
    virtual void AddGroupFromMenu() = 0;
    // motion层继承，用于其它地方获取motion层接口设置光标位置为nCol的动作块选中
    virtual void SetSelectGroupByOthers(int nCol){};
    virtual void OnLButtonDownDebug(int nCol) = 0;
    virtual void OnLButtonDbClickDebug(int nCol) = 0;
    virtual void AddBlankBlockInTail() = 0;

    //绘自己
    virtual void OnDraw(/*CDC* pDC*/) = 0;
    virtual bool GetData(char** ppData, int& nLen) = 0;
    virtual bool SetData(char* pData, int nLen)  = 0;
    virtual void AdjustData() =0;
    // 取得类型，具体见宏定义
    virtual int GetType() = 0;
    virtual QString GetLayerName() = 0;
    virtual	void SetPosition(int PositionX, int PositionY) = 0;
    virtual void GetPosition(int& x, int& y) = 0;
    virtual void SetHScrollOffset(int offset) = 0;
    // 设置每个单元格的大小
    virtual void SetScalevalue(int scaleValue) = 0;
    virtual int GetScaleValue() = 0;
    virtual void GetLayerWidth(int &LayerWidth) = 0;
    // 设置可见的起始块序号
    virtual void SetCellStartIndex(uint CellIndex) =0;
    virtual uint GetCellStartIndex() = 0;
    virtual int GetCellCount() = 0;
    // 设置单元格的宽度
    virtual void SetCellWidth(int width) = 0;
    virtual int GetCellWidth() = 0;
    // 设置单元格的高度
    virtual void SetCellHeight(int height) = 0;
    virtual int GetCellHeight() = 0;
    // 设置线条大小
    virtual void SetLineThick(int nThick) = 0;
    virtual int GetLineThick() = 0;

    // 创建OPTION处理接口
    virtual void CreateOptionHandler() = 0;
    virtual ILayerOptionHandler* GetOptionHandler() = 0;

    virtual int GetMaxTimeCellCount() = 0;

    // 播放该层的数据
//	virtual void OnPlay() = 0;
//	virtual void OnDebugStop() =0;
    virtual void SetLayerMenuState(bool menuState) = 0;
    virtual void onCancelPlayMotionBlock() = 0;

    virtual void SetTreeView(ILibTreeNavigation* pView) = 0;
    virtual void TreeToView(char *pData, int nLen, int nCol, QString FileName) = 0;
    virtual void  GetMusicDlgPtr(/*CMusicChooseDialog *pDlg*/){};
    virtual WCHAR* GetActionDescription(int nGroupIndex , int nActionIndex){return NULL;};
    virtual void GetActionSongPath(int nGroupIndex , int nActionIndex, QString& strSongPath){};
    //从ActionData的扩展数据中获取音乐文件名并添加
    virtual bool AddMusicFromExtData(int nGroupIndex , int nActionIndex){return false;};
    //从Music目录下获取第一个音乐文件并添加
    virtual bool AddMusicFromMusicDir(){return false;};

    virtual void DoMenuCommand(uint nCommand, int nCol){};

    //判断PT是否在这个层上，如果是，则返回块的ID号。如果不是，则返回－1；
//	virtual int HitTest(QPoint pt) = 0;

#if 0
    // 总的单元块数
    static int m_CellCount;
    // 线宽
    static int m_lineThick;
    // 单元格宽度
    static int m_nCellWidth;
    // 时间粒度
    static int m_scaleValue;
#else
    // 总的单元块数
    //int m_CellCount;
    //// 线宽
    //int m_lineThick;
    //// 单元格宽度
    //int m_nCellWidth;
    //// 时间粒度
    //int m_scaleValue;
#endif

protected:
    // 当前层的配置对象
    IUIConfigure*			m_pUILayerConfigure;
    LayerUI_Event*			m_pEvent;

    // 时间粒度
    //int m_scaleValue;
};
#endif  //_ILAYERUI_H_
