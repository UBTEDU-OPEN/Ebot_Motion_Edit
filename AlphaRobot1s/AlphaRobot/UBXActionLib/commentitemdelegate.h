/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：commentitemdelegate.h
* 创建时间：2016/03/14
* 文件标识：
* 文件摘要：动作详情页面评论项代理，用于显示item和提供交互
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/14
* 版本摘要：
*/
#ifndef COMMENTITEMDELEGATE_H
#define COMMENTITEMDELEGATE_H
#include <QStyledItemDelegate>

class CommentItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CommentItemDelegate(QObject *parent = 0);
    ~CommentItemDelegate();

    //重写父类函数，绘制item项时调用
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //重写父类函数，获取item项大小时调用
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // 重写父类函数，创建编辑控件
    virtual QWidget *	createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;

signals:
    /************************************
    * 名称: sigOnReplyClicked
    * 功能: 点击回复的信号
    * 参数: [in] nCommentId: 被回复的评论id
    * 参数: [in] strReplyToWho: 被回复的对象
    * 返回:   void
    * 时间:   2016/09/26
    * 作者:   hwx
    ************************************/
    void sigOnReplyClicked(quint64 nCommentId, const QString &strReplyToWho);

protected:
    //重写父类事件处理函数
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
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
    * 名称: getRectReplierIcon
    * 功能: 获取回复者头像区域
    * 参数: [in] option: 绘图样式
    * 参数: [in] index: 模型下标
    * 返回:   void
    * 时间:   2016/09/24
    * 作者:   hwx
    ************************************/
    QRect getRectReplierIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectReplier
    * 功能: 获取回复者区域
    * 参数: [in] option: 绘图样式
    * 参数: [in] index: 模型下标
    * 返回:   void
    * 时间:   2016/09/24
    * 作者:   hwx
    ************************************/
    QRect getRectReplier(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectCommentContent
    * 功能: 获取评论内容区域
    * 参数: [in] option: 绘图样式
    * 参数: [in] index: 模型下标
    * 返回:   void
    * 时间:   2016/09/24
    * 作者:   hwx
    ************************************/
    QRect getRectCommentContent(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectReplyTime
    * 功能: 获取评论时间区域
    * 参数: [in] option: 绘图样式
    * 参数: [in] index: 模型下标
    * 返回:   void
    * 时间:   2016/09/24
    * 作者:   hwx
    ************************************/
    QRect getRectReplyTime(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getRectReplyIcon
    * 功能: 获取回复图标的区域
    * 参数: [in] option: 绘图样式
    * 参数: [in] index: 模型下标
    * 返回:   void
    * 时间:   2016/09/24
    * 作者:   hwx
    ************************************/
    QRect getRectReplyIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /************************************
    * 名称: getWidgetSize
    * 功能: 获取widget大小
    * 参数: [in] option 绘图信息
    * 参数: [in] index: 模型index
    * 返回:   控件大小
    * 时间:   2016/09/14
    * 作者:   hwx
    ************************************/
    QSize getWidgetSize(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // COMMENTITEMDELEGATE_H
