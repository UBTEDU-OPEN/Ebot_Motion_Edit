/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cmarklineadm.h
* 创建时间：2016/12/09
* 文件标识：
* 文件摘要：标记线管理类
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/12/09
* 版本摘要：
*/

#ifndef CMARKLINEADM_H
#define CMARKLINEADM_H

#include <QObject>
#include <QList>

#include "datamodel/cmarkline.h"
#include "chandlemarkline.h"
#include "chandlestring.h"
#include "ubxundoredomanager.h"

class CMarkLineAdm : public QObject
{
    Q_OBJECT
public:
    explicit CMarkLineAdm(QObject *parent = 0);
    ~CMarkLineAdm();

    void setLineSpaceWidth(int nWidth) { m_nLineSpaceWidth = nWidth; } // 设置线宽
    void setLineSpaceHeight(int nHeight) { m_nLineSpaceHeight = nHeight; } // 设置线高

    // 设置、获取所有标记线
    CMarkLineContainer  *getMarkLineContainer() { return m_pMarkLineContainer; }
    void updateMarkLines(CMarkLineContainer *pContainer);

    // 拷贝修改前、后的动作数据
    void cloneMarkLineBeforeChanged(IOperation *pHandleMarkLine);
    void cloneMarkLineAfterChanged(IOperation *pHandleMarkLine);

public:

    /************************************
    * 名称: redrawAllMarkLine
    * 功能: 绘制所有标记线到缓冲的pixmap
    * 参数: [in] dStartPosMs: 绘制区域起始坐标位置，以ms为单位
    * 参数: [in] dEndPosMs:  绘制区域结束坐标位置，以ms为单位
    * 参数: [out]pixmap: 缓冲的pixmap
    * 返回:  bool
    * 时间:   2016/12/10
    * 作者:   hwx
    ************************************/
    void redrawAllMarkLine(double dStartPosMs, double dEndPosMs, QPixmap &pixmap);

    /************************************
    * 名称: createMarkLine
    * 功能: 新建标记线
    * 参数: [in]eLineType: 标记线类型
    * 参数: [in]dPosMs: 标记线在坐标系中的位置，以时间ms为单位
    * 返回:  bool
    * 时间:   2016/12/10
    * 作者:   hwx
    ************************************/
    bool createMarkLine(Constants::emMarkLineType eLineType, double dPosMs, QObject *pParent);

    /************************************
    * 名称: deleteSelectedMarkLines
    * 功能: 删除选中的标记线
    * 参数:
    * 返回:  void
    * 时间:   2017/1/4
    * 作者:   hwx
    ************************************/
    void deleteSelectedMarkLines();

    /************************************
    * 名称: isAnyMarkLinePressed
    * 功能: 判断是否存在任何一条线被点击
    * 返回:  bool
    * 时间:   2016/12/10
    * 作者:   hwx
    ************************************/
    bool isAnyMarkLinePressed();

    /************************************
    * 名称: moveMarkLines
    * 功能: 移动选中的标记线
    * 参数: [in] dOffset: 移动距离，正值表示右移，负值表示左移，以时间ms为单位
    * 返回:  void
    * 时间:   2016/12/10
    * 作者:   hwx
    ************************************/
    void moveMarkLines(double dOffset);

    /************************************
    * 名称: selectMarkLines
    * 功能: 矩形框选中标记线
    * 参数: [in] dStartPosMs: 绘制区域起始坐标位置，以ms为单位
    * 参数: [in] dEndPosMs:  绘制区域结束坐标位置，以ms为单位
    * 返回:  void
    * 时间:   2016/12/10
    * 作者:   hwx
    ************************************/
    void selectMarkLines(double dStartPosMs, double dEndPosMs);

    /************************************
    * 名称: resetLineState
    * 功能: 设置标记线为默认状态
    * 返回:  void
    * 时间:   2016/12/10
    * 作者:   hwx
    ************************************/
    void resetLineState();

    /************************************
    * 名称: getLineCursorHoverOn
    * 功能: 获取光标悬停在上面的标记线
    * 参数: [in] dCursorPos: 光标位置，已经转为为时间单位，ms
                [in] dPixelPerMs: 1ms占得像素值
    * 返回:  标记线
    * 时间:   2016/12/10
    * 作者:   hwx
    ************************************/
    CMarkLine *getLineCursorHoverOn(double dCursorPos, double dPixelPerMs);

private:
    /************************************
    * 名称: deleteMarkLine
    * 功能: 删除标记线
    * 参数:
    * 返回:  bool
    * 时间:   2017/1/4
    * 作者:   hwx
    ************************************/
    bool deleteMarkLine(CMarkLine *pLine);

    /************************************
    * 名称: removeAllLines
    * 功能: 删除所有标记线
    * 参数:
    * 返回:
    * 时间:   2017/01/13
    * 作者:   hwx
    ************************************/
    void removeAllLines();

signals:

public slots:

private:
    int  m_nLineSpaceWidth;  // 绘线所在区域宽度，即线条顶部图标宽度，以像素为单位
    int  m_nLineSpaceHeight; // 绘线所在区域高度，以像素为单位

    CMarkLineContainer *m_pMarkLineContainer;  // 动作组、动作帧标记线
};

#endif // CMARKLINEADM_H
