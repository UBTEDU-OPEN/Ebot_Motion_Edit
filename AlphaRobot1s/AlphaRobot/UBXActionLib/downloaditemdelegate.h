/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：downloaditemdelegate.h
* 创建时间：2016/03/10
* 文件标识：
* 文件摘要：动作库界面下载列表项代理定义，用于显示item和提供交互
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/10
* 版本摘要：
*/


#ifndef DOWNLOADITEMDELEGATE_H
#define DOWNLOADITEMDELEGATE_H
#include <QStyledItemDelegate>
#include "actupdatemangr.h"


class DownLoadItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DownLoadItemDelegate(QObject *parent = 0);

public:

    /************************************
    * 名称: getItemMaxWidth
    * 功能: get item mac width
    * 返回: int
    * 时间: 20160929
    * 作者: asu
    ************************************/
    static int getItemMaxWidth();

    /************************************
    * 名称: setShowAppoveStatus
    * 功能: set to show the action status or not
    * 返回:   void
    * 时间:   2016/06/14
    * 作者:   asu
    ************************************/
    void setShowAppoveStatus(bool bEnableShow);

    /************************************
    * 名称: paint
    * 功能: 重写父类函数，绘制item项时调用
    * 返回:   void
    * 时间:   2016/03/10
    * 作者:   hels
    ************************************/
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: sizeHint
    * 功能: 重写父类函数，获取item项大小时调用
    * 返回:   void
    * 时间:   2016/03/10
    * 作者:   hels
    ************************************/
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
    /************************************
    * 名称: SigActionPreviewClicked
    * 功能: 点击预览图时发送，通知外部处理
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    void SigActionPreviewClicked(ActItemData itemData);

    /************************************
    * 名称: sigOnActionCommentIconClicked
    * 功能: 评论信号
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/09/28
    * 作者:   hwx
    ************************************/
    void sigOnActionCommentIconClicked(ActItemData itemData);

    /************************************
    * 名称: sigOnActionPraiseIconClicked
    * 功能: 点赞信号
    * 参数: [in]itemData 对应的动作数据
    * 返回:   void
    * 时间:   2016/10/20
    * 作者:   hwx
    ************************************/
    void sigOnActionPraiseIconClicked(ActItemData itemData);

protected:
    /************************************
    * 名称: editorEvent
    * 功能: 重写父类函数，判断是否点击预览图
    * 返回:   void
    * 时间:   2016/04/15
    * 作者:   hels
    ************************************/
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    /************************************
    * 名称: getRectActionIcon
    * 功能: 获取动作图标绘图区域
    * 返回:   动作图标绘图区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectActionIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectActionTitle
    * 功能: 获取动作名称绘图区域
    * 返回:   动作名称绘图区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectActionTitle(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectSchemeTag
    * 功能: 获取活动标签区域
    * 返回:   活动标签区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectSchemeTag(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectExamineStatus
    * 功能: 获取审核状态区域
    * 返回:   审核状态区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectExamineStatus(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectActionDescription
    * 功能: 获取动作描述区域
    * 返回:   动作描述绘图区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectActionDescription(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectPraiseIcon
    * 功能: 获取点赞图标区域
    * 返回:   点赞图标区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectPraiseIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectPraiseNumber
    * 功能: 获取点赞数目
    * 返回:  动作点赞数目
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectPraiseNumber(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectDownloadIcon
    * 功能: 获取下载次数图标区域
    * 返回:  下载次数图标区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectDownloadIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectDownloadCount
    * 功能: 获取下载次数区域
    * 返回:  下载次数区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectDownloadCount(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectCommentIcon
    * 功能: 获取评论图标区域
    * 返回:  评论图标区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectCommentIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectCommentCount
    * 功能: 获取动作评论次数区域
    * 返回:  动作评论次数区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectCommentCount(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectCreateTime
    * 功能: 获取动作创建时间区域
    * 返回:  动作创建时间区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectCreateTime(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectCreator
    * 功能: 获取动作创建者区域
    * 返回:  动作创建者区域
    * 时间:   2016/09/27
    * 作者:   hwx
    ************************************/
    QRect getRectCreator(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    bool m_bEnableShowApproveStatus;
};

#endif // DOWNLOADITEMDELEGATE_H
