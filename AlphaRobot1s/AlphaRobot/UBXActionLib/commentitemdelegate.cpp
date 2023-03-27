/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：commentitemdelegate.cpp
* 创建时间：2016/03/14
* 文件标识：
* 文件摘要：动作详情页面评论项代理，用于显示item和提供交互
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/14
* 版本摘要：
*/

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>

#include "configs.h"
#include "commentitemdelegate.h"
#include "ubxactionlibconstants.h"
#include "datamodel/usercommentitem.h"
#include "widget/ccommentwidget.h"

CommentItemDelegate::CommentItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

CommentItemDelegate::~CommentItemDelegate()
{

}

void CommentItemDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    //自定义类型
    if(index.data(Constants::CommentItemType).toString() == Constants::COMMENT_ITEM_TYPE)
    {
        painter->save();

        int space = Constants::COMMENT_ITEM_SPACE;//margin

        // 获取评论数据
        UserCommentItem commentItem = index.data(Constants::CommentItemData).value<UserCommentItem>();

        //绘制头像
        QString sHeadFile = commentItem.getReplierIcon();
        if (sHeadFile.isEmpty() || !QFile::exists(sHeadFile)) // 如果文件不存在或者用户没有上传头像，则使用默认图片
        {
             sHeadFile = CConfigs::getLocalResPath(UBX_LOGIN_HEAD_IMAGE);
        }
        QRect rectHeadIcon = getRectReplierIcon(option, index);
        QPixmap pixmapHead;
        if(pixmapHead.load(sHeadFile))
        {
//            pixmapHead.scaled(Constants::COMMENT_HEAD_ICON_WIDTH, Constants::COMMENT_HEAD_ICON_HEIGHT,
//                              Qt::KeepAspectRatio, Qt::SmoothTransformation);
            getRoundImage(pixmapHead, QSize(Constants::COMMENT_HEAD_ICON_WIDTH, Constants::COMMENT_HEAD_ICON_HEIGHT));
            painter->drawPixmap(rectHeadIcon, pixmapHead);
        }

        //绘制Replier
        const QString strReplierName = commentItem.getReplierName() + ": ";
        QFont drawFont = QApplication::font();
        drawFont.setPixelSize(FONT_PIXEL_SIZE_18);
        painter->setPen(QColor(UBX_COLOR_DARK_GREY));
        painter->setFont(drawFont);
        QFontMetrics metricsReplier(drawFont);
        QRect rectReplier = getRectReplier(option, index);
        QString sEditedReplier = metricsReplier.elidedText(strReplierName, Qt::ElideRight, rectReplier.width());
        painter->drawText(rectReplier, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, sEditedReplier, &rectReplier);

        // 绘制评论内容
        const QString strReplyMessage = commentItem.getReplyMessage();
        drawFont.setPixelSize(FONT_PIXEL_SIZE_16);
        painter->setPen(QColor(UBX_COLOR_GREY));
        painter->setFont(drawFont);
        QRect rectComment = getRectCommentContent(option, index);
        painter->drawText(rectComment, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, strReplyMessage);


        // 评论时间
        const QString& strReplyTime =commentItem.getReplyTime();
        drawFont.setPixelSize(FONT_PIXEL_SIZE_14);
        painter->setFont(drawFont);
        painter->setPen(QColor(UBX_COLOR_DARK_GREY));
        QFontMetrics metricsReplyTime(drawFont);
        QRect rectRepyTime = getRectReplyTime(option, index);
        QString strEditedReplyTime = metricsReplyTime.elidedText(strReplyTime, Qt::ElideRight, rectRepyTime.width());
        painter->drawText(rectRepyTime, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, strEditedReplyTime, &rectRepyTime);

        // 回复图标
        QSize sizeReplyIcon(Constants::COMMENT_REPLY_ICON_WIDTH, Constants::COMMENT_REPLY_ICON_HEIGHT);
        QRect rectReplyIcon = getRectReplyIcon(option, index);
        QPixmap pixmapReplyIcon;
        if(pixmapReplyIcon.load(":/res/images/icon_reply.png"))
        {
            pixmapReplyIcon.scaled(sizeReplyIcon, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter->drawPixmap(rectReplyIcon, pixmapReplyIcon);
        }




//        const QString &replyToWho = commentItem.getReplyToWho();


//        QString strContent = "";
//        if(replyToWho.isEmpty())
//        {
//            strContent += tr(":    ");
//        }
//        else
//        {
//            strContent += tr("@") + replyToWho + tr(": ");
//        }
//        strContent += replyMessage;

//        QFont font = QApplication::font();
//        font.setPixelSize(12);
//        painter->setFont(font);
//        QFontMetrics metrics(font);

//        //reply content
//        QRect rectContent = metrics.boundingRect(rectHeadIcon.right() + space, rectHeadIcon.top(), Constants::COMMENT_REPLY_MESSAGE_MAX_WIDTH,0,
//                                                  Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,strContent);
//        painter->drawText(rectContent, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, strContent);

//        //Date
//        const QString& date =commentItem.getReplyTime();
//        QRect rectDate = metrics.boundingRect(rectContent.right() + space, rectContent.top(),Constants::COMMENT_REPLY_DATE_MAX_WIDTH, 0,
//                                              Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, date);
//        painter->drawText(rectDate,
//                          Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, date);

//        //reply count
//        const QString &count = QString("%1").arg(commentItem.getReplyCount());
//        QString replyCount(tr("Reply"));
//        replyCount += "(" + count + ")";
//        font.setPixelSize(10);
//        font.setUnderline(true);
//        painter->setFont(font);
//        QFontMetrics metricsCount(font);
//        QRect rectReplyCount = metricsCount.boundingRect(rectContent.left(), rectContent.bottom() + space,
//                                                         Constants::COMMENT_REPLY_MESSAGE_MAX_WIDTH, 0,
//                                                         Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, replyCount);
//        painter->setPen(Qt::blue);
//        painter->drawText(rectReplyCount, Qt::AlignLeft, replyCount);

        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize CommentItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.data(Constants::CommentItemType).toString() == Constants::COMMENT_ITEM_TYPE)
    {
        int space = Constants::COMMENT_ITEM_SPACE;//margin

        //头像
        QRect rectHeadIcon = getRectReplierIcon(option, index);

        // Replier
        QRect rectReplier = getRectReplier(option, index);

        // 评论内容
        QRect rectComment = getRectCommentContent(option, index);

        // 评论时间
        QRect rectRepyTime = getRectReplyTime(option, index);

        // 回复图标
        QRect rectReplyIcon = getRectReplyIcon(option, index);

        // 总高度 = [评论内容高度 + 评论内如与评论时间间隙 +评论时间高度](或头像高度)较大者
        int nTotalHeight = rectComment.height() + space + rectRepyTime.height() + 2 * space; // 2 * space作为上下item距离
        if (nTotalHeight < (Constants::COMMENT_HEAD_ICON_HEIGHT + 2 * space))
        {
            nTotalHeight = Constants::COMMENT_HEAD_ICON_HEIGHT + 2 * space;
        }

        // 总宽度 = 用户头像宽度  + 评论者宽度 + 评论内容或(评论时间+ 评论时间与回复图标间隙 +回复图标）的较大者
        int nReplyRowWidth = rectRepyTime.width() + space + rectReplyIcon.width();
        if (nReplyRowWidth < rectComment.width())
        {
            nReplyRowWidth = rectComment.width();
        }
        int nTotalWidth = rectHeadIcon.width() + space + rectReplier.width() + nReplyRowWidth;

        return QSize(nTotalWidth, nTotalHeight);
    }
    else
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

QWidget *CommentItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

bool CommentItemDelegate::editorEvent(QEvent *event,
                                      QAbstractItemModel *model,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index)
{
//    int space = Constants::COMMENT_ITEM_SPACE;//margin

    UserCommentItem commentItem = index.data(Constants::CommentItemData).value<UserCommentItem>();
    const QString replyToWho = commentItem.getReplierName();

//    const QString &replierName = commentItem.getReplierName();
//    const QString &replyToWho = commentItem.getReplyToWho();
//    const QString &replyMessage = commentItem.getReplyMessage();

//    QString strContent = replierName;
//    if(replyToWho.isEmpty())
//    {
//        strContent += tr(":    ");
//    }
//    else
//    {
//        strContent += tr("@") + replyToWho + tr(": ");
//    }
//    strContent += replyMessage;

//    QFont font = QApplication::font();
//    font.setPixelSize(12);
//    QFontMetrics metrics(font);
//    //reply content
//    QRect rectContent = metrics.boundingRect(option.rect.left() + Constants::COMMENT_HEAD_ICON_WIDTH + space, option.rect.top() + space, Constants::COMMENT_REPLY_MESSAGE_MAX_WIDTH,0,
//                                              Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,strContent);
//    //reply count
//    const QString &count =QString("%1").arg(commentItem.getReplyCount());
//    QString replyCount(tr("Reply"));
//    replyCount += "(" + count + ")";
//    font.setPixelSize(10);
//    font.setUnderline(true);
//    QFontMetrics metricsCount(font);
//    QRect rectReplyCount = metricsCount.boundingRect(rectContent.left(), rectContent.bottom() + space,
//                                                     Constants::COMMENT_REPLY_MESSAGE_MAX_WIDTH, 0,
//                                                     Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, replyCount);

    QRect rectReply = getRectReplyIcon(option, index);

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (mouseEvent && event->type() == QEvent::MouseButtonPress && rectReply.contains(mouseEvent->pos()))
    {
        emit sigOnReplyClicked(commentItem.getReplyId(), replyToWho);
    }

    if (mouseEvent && event->type() == QEvent::MouseMove && rectReply.contains(mouseEvent->pos()))
    {
        QCursor cursor(Qt::PointingHandCursor);
        QApplication::setOverrideCursor(cursor);
    }
    else
    {
        QCursor cursor(Qt::ArrowCursor);
        QApplication::setOverrideCursor(cursor);
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

/************************************
* 名称: getRoundImage
* 功能: 获取圆形头像
* 参数: [in] pixMapSource: 源头像
* 参数: [in] size 大小
* 返回:   void
* 时间:   2016/09/08
* 作者:   hwx
************************************/
void CommentItemDelegate::getRoundImage(QPixmap &pixMapSource, QSize size) const
{
    QImage resultImage(size,QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&resultImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(resultImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QPixmap pixmapMask(":/res/images/head_mask.png");
    painter.drawPixmap(0, 0, pixmapMask.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOut);
    painter.drawPixmap(0, 0, pixMapSource.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);

    painter.end();
    pixMapSource = QPixmap::fromImage(resultImage);
}

/************************************
* 名称: getRectReplierIcon
* 功能: 获取回复者头像区域
* 参数: [in] option: 绘图样式
* 参数: [in] index: 模型下标
* 返回:   void
* 时间:   2016/09/24
* 作者:   hwx
************************************/
QRect CommentItemDelegate::getRectReplierIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //自定义类型
    if(index.data(Constants::CommentItemType).toString() == Constants::COMMENT_ITEM_TYPE)
    {
        return QRect(option.rect.topLeft(), QSize(Constants::COMMENT_HEAD_ICON_WIDTH, Constants::COMMENT_HEAD_ICON_HEIGHT));
    }

    return QRect();
}

/************************************
* 名称: getRectReplier
* 功能: 获取回复者区域
* 参数: [in] option: 绘图样式
* 参数: [in] index: 模型下标
* 返回:   void
* 时间:   2016/09/24
* 作者:   hwx
************************************/
QRect CommentItemDelegate::getRectReplier(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 用户头像区域
    QRect rectReplierIcon = getRectReplierIcon(option, index);
    if (rectReplierIcon.isValid())
    {
        int space = Constants::COMMENT_ITEM_SPACE;//margin

        // 获取评论数据
        UserCommentItem commentItem = index.data(Constants::CommentItemData).value<UserCommentItem>();

        //绘制Replier
        const QString &strReplierName = commentItem.getReplierName() + ": ";
        QFont drawFont = QApplication::font();
        drawFont.setPixelSize(FONT_PIXEL_SIZE_18);
        QFontMetrics metricsReplier(drawFont);
        return QRect(rectReplierIcon.topRight() + QPoint(space, 0), QSize(metricsReplier.width(strReplierName) + Constants::COMMENT_TEXT_MARGIN, metricsReplier.height()));
    }

    return QRect();
}

/************************************
* 名称: getRectCommentContent
* 功能: 获取评论内容区域
* 参数: [in] option: 绘图样式
* 参数: [in] index: 模型下标
* 返回:   void
* 时间:   2016/09/24
* 作者:   hwx
************************************/
QRect CommentItemDelegate::getRectCommentContent(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize sizeWidget = getWidgetSize(option, index);

    // 评论者区域
    QRect rectReplier = getRectReplier(option, index);
    if (rectReplier.isValid())
    {
        int space = Constants::COMMENT_ITEM_SPACE;//margin
         QFont drawFont = QApplication::font();
        drawFont.setPixelSize(FONT_PIXEL_SIZE_16);

        // 获取评论数据
        UserCommentItem commentItem = index.data(Constants::CommentItemData).value<UserCommentItem>();

        // 绘制评论内容
        const QString strReplyMessage = commentItem.getReplyMessage();
        QFontMetrics metricsComment(drawFont);
        int nCommentContentWidth = metricsComment.width(strReplyMessage) + space;  // 评论内容的宽度, 由于mac显示不全，多一个space宽度

        // 当前评论支持容纳的最大宽度
        int nMaxWidthAllowed = sizeWidget.width() - (rectReplier.topRight().x() - option.rect.topLeft().x()) - Constants::COMMENT_TREEVIEW_MARGIN;

        // 如果评论内容比较多，则需要换行
        if (nCommentContentWidth > nMaxWidthAllowed)
        {
            nCommentContentWidth = nMaxWidthAllowed;
        }

        return metricsComment.boundingRect(QRect(rectReplier.topRight(), QSize(nCommentContentWidth, metricsComment.height())),
                                                  Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, strReplyMessage);

//        return QRect(rectReplier.topRight(), QSize(nCommentContentWidth, metricsComment.height()));
    }

    return QRect();
}

/************************************
* 名称: getRectReplyTime
* 功能: 获取评论时间区域
* 参数: [in] option: 绘图样式
* 参数: [in] index: 模型下标
* 返回:   void
* 时间:   2016/09/24
* 作者:   hwx
************************************/
QRect CommentItemDelegate::getRectReplyTime(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 用户名区域
    QRect rectReplier = getRectReplier(option, index);

    // 评论内容区域
    QRect rectCommentContent = getRectCommentContent(option, index);
    if (rectCommentContent.isValid())
    {
        int space = Constants::COMMENT_ITEM_SPACE;//margin
       QFont drawFont = QApplication::font();

        // 获取评论数据
        UserCommentItem commentItem = index.data(Constants::CommentItemData).value<UserCommentItem>();

        // 评论时间
        const QString& strReplyTime =commentItem.getReplyTime();
        drawFont.setPixelSize(FONT_PIXEL_SIZE_14);
        QFontMetrics metricsReplyTime(drawFont);
        return QRect(rectCommentContent.bottomLeft()  - QPoint(rectReplier.width(), 0) + QPoint(0, space), QSize(metricsReplyTime.width(strReplyTime) + space, metricsReplyTime.height()));  // 由于mac显示不全，多一个space宽度
    }

    return QRect();
}

/************************************
* 名称: getRectReplyIcon
* 功能: 获取回复图标的区域
* 参数: [in] option: 绘图样式
* 参数: [in] index: 模型下标
* 返回:   void
* 时间:   2016/09/24
* 作者:   hwx
************************************/
QRect CommentItemDelegate::getRectReplyIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 评论时间区域
    QRect rectReplyTime = getRectReplyTime(option, index);
    if (rectReplyTime.isValid())
    {
        int space = Constants::COMMENT_ITEM_SPACE;//margin

        // 回复图标
        QSize sizeReplyIcon(Constants::COMMENT_REPLY_ICON_WIDTH, Constants::COMMENT_REPLY_ICON_HEIGHT);
        return QRect(rectReplyTime.topRight() + QPoint(space, 0), sizeReplyIcon);
    }

    return QRect();
}

/************************************
* 名称: getWidgetSize
* 功能: 获取widget大小
* 参数: [in] option 绘图信息
* 参数: [in] index: 模型index
* 返回:   控件大小
* 时间:   2016/09/14
* 作者:   hwx
************************************/
QSize CommentItemDelegate::getWidgetSize(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QAbstractItemModel *pModel = index.model();
    if (!pModel)
    {
        return QSize(0, 0);
    }

    QWidget *pWidget = dynamic_cast<QWidget *> (pModel->parent());
    if (!pWidget)
    {
        return QSize(0, 0);
    }

    return pWidget->size();
}

