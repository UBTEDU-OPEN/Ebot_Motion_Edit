/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：rankitemdeletegate.h
* 创建时间：2016/03/30
* 文件标识：
* 文件摘要：排行榜项委托，显示推广活动排行榜
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/08
* 版本摘要：
*/

#ifndef RANKITEMDELETEGATE_H
#define RANKITEMDELETEGAT_H

#include <QPainter>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QEvent>
#include <QPoint>
#include <QSize>

#include "datamodel/userrankitem.h"
#include "UBXPublic.h"
#include "ubxactionlibconstants.h"

class UserRankItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit UserRankItemDelegate(QObject *parent = 0);
    ~UserRankItemDelegate();

signals:
    /************************************
    * 名称: sigOnActionDetailedClicked
    * 功能: 点击用户名称
    * 返回:   void
    * 时间:   2016/08/08
    * 作者:   hwx
    ************************************/
    void sigOnActionDetailedClicked(ActItemData item);

public:

    //重写父类函数，绘制item项时调用
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //重写父类函数，获取item项大小时调用
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:

    //重写父类事件处理函数
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    /************************************
    * 名称: getActionIcon
    * 功能: 根据动作类型获取对应图标资源
    * 参数: [in] eType: 动作类型
    * 返回:   void
    * 时间:   2016/09/01
    * 作者:   hwx
    ************************************/
    QString getActionIcon(emActionType eType) const;

    /************************************
    * 名称: getHotIconByRankOrder
    * 功能: 根据排名获取热度图标
    * 参数: [in] nOrder: 动作排名
    * 返回:   void
    * 时间:   2016/09/08
    * 作者:   hwx
    ************************************/
    QString getHotIconByRankOrder(int nOrder) const;

    /************************************
    * 名称: getMedalIconByRankOrder
    * 功能: 根据排名获取奖牌图标
    * 参数: [in] nOrder: 动作排名
    * 返回:   void
    * 时间:   2016/09/10
    * 作者:   hwx
    ************************************/
    QString getMedalIconByRankOrder(int nOrder) const;

    /************************************
    * 名称: drawMedal
    * 功能: 绘制奖牌
    * 参数: [in] nRankOrder: 排名
    * 参数: [in] painter 画笔
    *参数: [in] rectWhere 绘制区域
    * 返回:   void
    * 时间:   2016/09/01
    * 作者:   hwx
    ************************************/
    void drawMedal(int nRankOrder, QPainter *painter, const QRect &rectWhere) const;

    /************************************
    * 名称: getRoundImage
    * 功能: 获取圆形头像
    * 参数: [in] pixMapSource: 源头像
    * 参数: [in] size 大小
    * 返回:   void
    * 时间:   2016/09/08
    * 作者:   hwx
    ************************************/
    void getRoundImage(QPixmap &pixMapSource, QSize size) const;

    /************************************
    * 名称: drawPixMap
    * 功能: 绘制图片
    * 参数: [in] painter: 画笔
    * 参数: [in] strImage 图标资源
    * 参数: [in] rectWhere: 绘制区域
    * 参数: [in] size 绘制图片大小
    * 返回:   void
    * 时间:   2016/09/08
    * 作者:   hwx
    ************************************/
    void drawPixMap(QPainter *painter, const QString &strImage, const QRect &rectWhere, QSize size) const;

    /************************************
    * 名称: getPaddingValue
    * 功能: 获取指定位置空白区域的宽度
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 参数: [in] emDir: 位置信息
    * 返回:   int
    * 时间:   2016/09/14
    * 作者:   hwx
    ************************************/
    int getPaddingValue(const QStyleOptionViewItem &option, const QModelIndex &index, Constants::emDirection emDir) const;

    /************************************
    * 名称: getPointTopRight
    * 功能: 获取widget右上角点
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 返回:   int
    * 时间:   2016/09/14
    * 作者:   hwx
    ************************************/
    QPoint getPointTopRight(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectRankIcon
    * 功能: 获取动作排行图标区域
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 返回:   动作排行图标区域
    * 时间:   2016/09/28
    * 作者:   hwx
    ************************************/
    QRect getRectRankIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectUserIcon
    * 功能: 获取用户头像图标区域
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 返回:   用户头像图标区域
    * 时间:   2016/09/28
    * 作者:   hwx
    ************************************/
    QRect getRectUserIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectUserName
    * 功能: 获取用户名称区域
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 返回:   用户名称区域
    * 时间:   2016/09/28
    * 作者:   hwx
    ************************************/
    QRect getRectUserName(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectActionIcon
    * 功能: 获取动作图标区域
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 返回:   动作图标区域
    * 时间:   2016/09/28
    * 作者:   hwx
    ************************************/
    QRect getRectActionIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectActionName
    * 功能: 获取动作名称区域
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 返回:   动作名称区域
    * 时间:   2016/09/28
    * 作者:   hwx
    ************************************/
    QRect getRectActionName(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectHotIcon
    * 功能: 获取热度图标区域
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 返回:   热度图标区域
    * 时间:   2016/09/28
    * 作者:   hwx
    ************************************/
    QRect getRectHotIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectHotValue
    * 功能: 获取热度值区域
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 返回:   热度值区域
    * 时间:   2016/09/28
    * 作者:   hwx
    ************************************/
    QRect getRectHotValue(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:

};

#endif // RANKITEMDELETEGATE_H

