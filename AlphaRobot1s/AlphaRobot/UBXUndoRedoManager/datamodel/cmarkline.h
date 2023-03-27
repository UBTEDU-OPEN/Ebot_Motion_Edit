/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cmarkline.h
* 创建时间：2016/12/09
* 文件标识：
* 文件摘要：标记线类
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/12/09
* 版本摘要：
*/

#ifndef CMARKLINE_H
#define CMARKLINE_H

#include <QObject>
#include <QPixmap>

#include "ubxundoredomanagerconst.h"

class UBXUNDOREDOMANAGER_API CMarkLine : public QPixmap
{
public:
    explicit CMarkLine(int nWidth, int nHeight, QObject *pParent = NULL);
    explicit CMarkLine(Constants::emMarkLineType eLineType, double dPosMs, int nWidth, int nHeight, QObject *pParent = NULL);
    ~CMarkLine();

public:
    // 设置、获取线条宽度、高度
    void setLineWidth(int nLineWidth) { m_nLineSpaceWidth = nLineWidth; }
    void setLineHeight(int nLineHeight) { m_nLineSpaceHeight = nLineHeight; }
    int getLineHeight() { return m_nLineSpaceHeight; }
    int getLineWidth() { return m_nLineSpaceWidth; }

    // 获取线条类型
    Constants::emMarkLineType getType() { return m_eType; }

    // 设置、获取线条位置
    double getPos();
    void setPos(double dPosMs) { m_dPosMs = dPosMs; }

    // 设置、获取标记线状态
    void setState(Constants::emMarkLineState eState) { m_eState = eState; }
    Constants::emMarkLineState getState() { return m_eState; }

public:
    /************************************
    * 名称: clone
    * 功能: 复制线条数据
    * 参数: [in] pLine: 源数据
    * 返回:  void
    * 时间:   2016/12/12
    * 作者:   hwx
    ************************************/
    void clone(CMarkLine *pLine);

    /************************************
    * 名称: move
    * 功能: 移动线条
    * 参数: [in] dOffset: 移动距离，正值表示右移，负值表示左移，以时间ms为单位
    * 返回:  void
    * 时间:   2016/12/12
    * 作者:   hwx
    ************************************/
    void move(double dOffset);

    /************************************
    * 名称: isInSelectedArea
    * 功能: 判断线条是否在选择的矩形区域内
    * 参数: [in] dStartPosMs: 绘制区域起始坐标位置，以ms为单位
                 [in] dEndPosMs:  绘制区域结束坐标位置，以ms为单位
    * 返回:  void
    * 时间:   2016/12/17
    * 作者:   hwx
    ************************************/
    bool isInSelectedArea(double dStartPosMs, double dEndPosMs);

public:
    /************************************
    * 名称: init
    * 功能: 初始化线条
    * 参数:
    * 返回:  void
    * 时间:   2016/12/17
    * 作者:   hwx
    ************************************/
    void init();

    /************************************
    * 名称: redraw
    * 功能: 重绘线条
    * 参数:
    * 返回:  void
    * 时间:   2016/12/17
    * 作者:   hwx
    ************************************/
    void redraw();

    /************************************
    * 名称: isCursorHoverOn
    * 功能: 检测光标位置是否落在标记线上
    * 参数: [in] dCursorPos: 光标位置，已经转为为时间单位，ms
                [in] dPixelPerMs: 1ms占得像素值
    * 返回:  bool
    * 时间:   2016/12/17
    * 作者:   hwx
    ************************************/
    bool isCursorHoverOn(double dCursorPos, double dPixelPerMs);

private:
    /************************************
    * 名称: drawLine
    * 功能: 绘制标记线
    * 参数: [in] color: 线条颜色
                 [in] nLineThick:  线条宽度
    * 返回:  bool
    * 时间:   2016/12/17
    * 作者:   hwx
    ************************************/
    bool drawLine(QColor color, int nLineThick);

    // 获取线条颜色
    QColor getColor();

    // 获取线条宽度
    int getThick();

private:
    double  m_dPosMs;   // 线条水平位置坐标，以ms为单位

    int  m_nLineSpaceWidth; // 线宽
    int  m_nLineSpaceHeight;  // 设置线高

    Constants::emMarkLineType m_eType;  // 标记线类型
    Constants::emMarkLineState m_eState; // 标记线状态
};

#endif // CMARKLINE_H
