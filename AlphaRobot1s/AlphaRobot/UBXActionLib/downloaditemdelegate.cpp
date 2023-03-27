/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：downloaditemdelegate.cpp
* 创建时间：2016/03/10
* 文件标识：
* 文件摘要：动作库界面下载列表项代理定义，用于显示item和提供交互
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/10
* 版本摘要：
*/
#include "downloaditemdelegate.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QDate>

#include "UBXPublic.h"
#include "ubxactionlibconstants.h"
#include "cactionlibdatapool.h"
#include "configs.h"

DownLoadItemDelegate::DownLoadItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_bEnableShowApproveStatus = false;
}

/************************************
* 名称: getItemMaxWidth
* 功能: get item mac width
* 返回: int
* 时间: 20160929
* 作者: asu
************************************/
int DownLoadItemDelegate::getItemMaxWidth()
{
    const int nMaxWidth = 3*Constants::CONTENT_SPACE +
            Constants::BTN_CHECOBOX_CONTAINER_WIDTH +
            Constants::PREVIEW_ICON_WIDTH +
            Constants::DESCRIPTION_DEFAULT_WIDTH +
            Constants::BTN_FAV_DOWN_CONTAINER_WIDTH;

    return nMaxWidth;
}

/************************************
* 名称: setShowAppoveStatus
* 功能: set to show the action status or not
* 返回:   void
* 时间:   2016/06/14
* 作者:   asu
************************************/
void DownLoadItemDelegate::setShowAppoveStatus(bool bEnableShow)
{
    m_bEnableShowApproveStatus = bEnableShow;
}

/************************************
* 名称: paint
* 功能: 重写父类函数，绘制item项时调用
* 返回:   void
* 时间:   2016/03/10
* 作者:   hels
************************************/
void DownLoadItemDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    //自定义描述项类型
    if(index.data(Constants::DownLoadItemType).toString() == Constants::CUSTOM_DOWNLOAD_ITEM_TYPE)
    {
        painter->save();

        int nDescTextWidth = option.rect.width() * Constants::DESCRIPTION_MAX_WIDTH_RATION;
        ActItemData itemData = index.data(Constants::ActionItemData).value<ActItemData>();

        // 记录上次使用的画笔，画刷
        QPen penPreviouse;

        //先绘制背景
        QBrush brush = index.data(Qt::BackgroundRole).value<QBrush>();
        if(Qt::NoBrush != brush.style())
        {
            painter->fillRect(option.rect, brush);
        }

        const int space = Constants::CONTENT_SPACE;//margin
        const int nOptionRectRight = option.rect.right();

        //绘制每项数据
        //绘制预览图
        QPixmap pixmapPeview;
        pixmapPeview.fill(Qt::transparent);
        QRect previewRect = getRectActionIcon(option, index);
        if(pixmapPeview.load(itemData.m_sImageHeadUrl))
        {
            pixmapPeview.scaled(Constants::PREVIEW_ICON_WIDTH, Constants::PREVIEW_ICON_HEIGHT,
                                Qt::KeepAspectRatio, Qt::SmoothTransformation);

            painter->drawPixmap(previewRect, pixmapPeview);
        }
        else
        {            
            QString strFielName = CConfigs::instance()->getLocalResPath("DefaultImage.jpg");//如果加载图片不成功则加载默认图片
            if(pixmapPeview.load(strFielName))
            {
                pixmapPeview.scaled(Constants::PREVIEW_ICON_WIDTH, Constants::PREVIEW_ICON_HEIGHT,
                                    Qt::KeepAspectRatio, Qt::SmoothTransformation);

                painter->drawPixmap(previewRect, pixmapPeview);
            }
        }


        //绘制status
        if(m_bEnableShowApproveStatus &&
                (eActStatusApproving == itemData.m_nActionStatus) || (eActStatusNoApproved == itemData.m_nActionStatus))
        {
            QString strActionStatus;

            if(eActStatusApproving == itemData.m_nActionStatus)
            {
                strActionStatus = QObject::tr("Pending");
            }
            else if (eActStatusNoApproved == itemData.m_nActionStatus)
            {
                strActionStatus = QObject::tr("NoPass");
            }

            // 记录上次使用的画笔，以便恢复
            penPreviouse = painter->pen();

            QFont font = QApplication::font();
            font.setPixelSize(FONT_PIXEL_SIZE_16);
            painter->setPen(QColor(Qt::white));
            painter->setFont(font);
            QFontMetrics metrics(font);

            QRect rectStatus = getRectExamineStatus(option, index);
            QString elidedTextStatus = metrics.elidedText(strActionStatus, Qt::ElideRight, rectStatus.width());

            painter->drawText(rectStatus.left(),
                              rectStatus.top(),
                              rectStatus.width(),
                              rectStatus.height(),
                              Qt::AlignCenter,
                              elidedTextStatus,
                              &rectStatus);
            painter->setPen(penPreviouse);
        }

        //绘制Title, Description
        const QString &title = itemData.m_sActName;
        QString description = itemData.m_sActDesc;
        if(description.isEmpty())
        {
            description = tr("No description yet");
        }

        // 记录上次使用的画笔，以便恢复
        penPreviouse = painter->pen();

        QFont font = QApplication::font();
        font.setPixelSize(FONT_PIXEL_SIZE_18);
        painter->setPen(QPen(UBX_COLOR_GREY));
        painter->setFont(font);
        QFontMetrics metrics(font);

        //Title
        QRect rectTitle = getRectActionTitle(option, index);
        rectTitle.setRight(qMin(rectTitle.right(), nOptionRectRight));

        QString elidedText = metrics.elidedText(title, Qt::ElideRight, rectTitle.width());
        painter->drawText(rectTitle.left(),
                          rectTitle.top(),
                          rectTitle.width(),
                          rectTitle.height(),
                          Qt::AlignLeft,
                          elidedText,
                          &rectTitle);

        painter->setPen(penPreviouse);

        // 如果存在活动标签，则绘制活动标签
        QString strSchemeTag = itemData.m_strSchemeName;
        if (!strSchemeTag.isEmpty())
        {
            int nPadding = 4;  // 文本距离背景框边距
            font.setPixelSize(FONT_PIXEL_SIZE_14);
            QFontMetrics metricsActivityTag(font);

            // 绘制活动标签背景，最大宽度Constants::DESCRIPTION_MAX_WIDTH/2 - space，高度与动作名称相同
            QRect rectActivityTagBg = getRectSchemeTag(option, index);
            rectActivityTagBg.setRight(qMin(rectActivityTagBg.right(), nOptionRectRight));

            painter->setRenderHints(QPainter::Antialiasing);
            painter->setPen(QPen(UBX_COLOR_DARK_RED));
            painter->setBrush(QBrush(UBX_COLOR_DARK_RED, Qt::SolidPattern));
            painter->drawRoundRect(rectActivityTagBg);

            // 绘制活动标签文本
            painter->setPen(UBX_COLOR_WHITE);
            painter->setFont(font);
            int nTextWidth = metricsActivityTag.width(strSchemeTag); // 文本宽度
            QRect rectActivityTagText = QRect(rectActivityTagBg.topLeft() + QPoint(nPadding, nPadding/2), QSize(nTextWidth + Constants::TEXT_MORE_SPACE, metricsActivityTag.height())); // 标题
            rectActivityTagText.setRight(qMin(rectActivityTagText.right(), nOptionRectRight));

            QString strElidedActivityTag = metricsActivityTag.elidedText(strSchemeTag, Qt::ElideRight, rectActivityTagText.width());
            painter->drawText(rectActivityTagText.left(),
                              rectActivityTagText.top(),
                              rectActivityTagText.width(),
                              rectActivityTagText.height(),
                              Qt::AlignLeft | Qt::AlignVCenter,
                              strElidedActivityTag,
                              &rectActivityTagText);
        }

        //Description
        font.setPixelSize(FONT_PIXEL_SIZE_14);
        painter->setFont(font);
        painter->setPen(QPen(UBX_COLOR_GREY));
        QFontMetrics metricsDes(font);
        QRect rectDes = getRectActionDescription(option, index);
        rectDes.setRight(qMin(rectDes.right(), nOptionRectRight));

        elidedText = metricsDes.elidedText(description, Qt::ElideRight, rectDes.width());
        painter->drawText(rectDes.left(),
                          rectDes.top(),
                          rectDes.width(),
                          rectDes.height(),
                          Qt::AlignLeft,//Qt::AlignBottom|Qt::AlignLeft,
                          elidedText,
                          &rectDes);

        //绘制downloads Icon， starts Icon，Comments Icon直接从rcc中获取图标
        //设置文本颜色
        painter->setPen(QColor(Qt::white));
        //获取文本信息
        const QString &downLoads = QString::number(itemData.m_nActionDownloads);
        const QString &stars = QString::number(itemData.m_nActionCollectTime);
        const QString &comments = QString::number(itemData.m_nActionComments);
        QDateTime createDatetTime = QDateTime::fromString(itemData.m_sPublishTime, NET_ACTION_DATE_FORMAT);
        const QString creationDate = createDatetTime.date().toString("yyyy-MM-dd");
        const QString &creator = itemData.m_sUserName;


        bool bElidedForText = false;
        //ActionCollectTime
        QRect rectStarsIcon = getRectPraiseIcon(option, index);
        rectStarsIcon.setRight(qMin(rectStarsIcon.right(), nOptionRectRight));
        font.setPixelSize(FONT_PIXEL_SIZE_12);
        painter->setFont(font);
        QFontMetrics metricsStarts(font);
        QRect rectStars = getRectPraiseNumber(option, index);
        rectStars.setRight(qMin(rectStars.right(), nOptionRectRight));
        if(!stars.isEmpty())
        {
            QPixmap pixmapStars;
            pixmapStars.load(":/res/images/icon_collection.png");
            painter->drawPixmap(rectStarsIcon, pixmapStars.scaled(Constants::ICON_STARTS_WIDTH, Constants::ICON_STARTS_HEGIHT,
                                                                  Qt::KeepAspectRatio, Qt::SmoothTransformation));

            const QString strStartsTemp  = metricsStarts.elidedText(stars, Qt::ElideRight,
                                                                 rectStars.width());
            if(strStartsTemp != stars)
            {
                bElidedForText = true;
            }
            painter->drawText(rectStars.left(),
                              rectStars.top(),
                              rectStars.width(),
                              rectStars.height(),
                              Qt::AlignTop|Qt::AlignLeft,
                              strStartsTemp,
                              &rectStars);
        }

        //ActionDownloads
        QRect rectDownLoadIcon = getRectDownloadIcon(option, index);
        rectDownLoadIcon.setRight(qMin(rectDownLoadIcon.right(), nOptionRectRight));
        QRect rectDownLoads = getRectDownloadCount(option, index);
        rectDownLoads.setRight(qMin(rectDownLoads.right(), nOptionRectRight));
        if(!downLoads.isEmpty() && !bElidedForText)
        {
            QPixmap pixmapDowLoads;
            pixmapDowLoads.load(":/res/images/icon_download.png");        
            painter->drawPixmap(rectDownLoadIcon, pixmapDowLoads.scaled(Constants::ICON_DOWNLOADS_WIDTH, Constants::ICON_DOWNLOADS_HEGIHT,
                                                                        Qt::KeepAspectRatio, Qt::SmoothTransformation));

            const QString strDownLoadsTemp  = metricsStarts.elidedText(downLoads, Qt::ElideRight,
                                                                 rectDownLoads.width());
            if(strDownLoadsTemp != downLoads)
            {
                bElidedForText = true;
            }
            painter->drawText(rectDownLoads.left(),
                              rectDownLoads.top(),
                              rectDownLoads.width(),
                              rectDownLoads.height(),
                              Qt::AlignTop|Qt::AlignLeft,
                              strDownLoadsTemp,
                              &rectDownLoads);

        }


        //ActionComments
        QRect rectCommentsIcon = getRectCommentIcon(option, index);
        rectCommentsIcon.setRight(qMin(rectCommentsIcon.right(), nOptionRectRight));
        QRect rectComments = getRectCommentCount(option, index);
        rectComments.setRight(qMin(rectComments.right(), nOptionRectRight));
        if(!comments.isEmpty() && !bElidedForText)
        {
            QPixmap pixmapComments;
            pixmapComments.load(":/res/images/icon_comment.png");
            painter->drawPixmap(rectCommentsIcon, pixmapComments.scaled(Constants::ICON_COMMENTS_WIDTH, Constants::ICON_COMMENTS_HEGIHT,
                                                                        Qt::KeepAspectRatio, Qt::SmoothTransformation));

            const QString strCommentsTemp  = metricsStarts.elidedText(comments, Qt::ElideRight,
                                                                 rectComments.width());
            if(strCommentsTemp != comments)
            {
                bElidedForText = true;
            }
            painter->drawText(rectComments.left(),
                              rectComments.top(),
                              rectComments.width(),
                              rectComments.height(),
                              Qt::AlignTop|Qt::AlignLeft,
                              strCommentsTemp,
                              &rectComments);
        }

        //CreationData
        QRect rectCreationData = getRectCreateTime(option, index);
        rectCreationData.setRight(qMin(rectCreationData.right(), nOptionRectRight));
        if(!creationDate.isEmpty() && createDatetTime.isValid() && !bElidedForText)
        {
            const QString strCreationDateTemp  = metricsStarts.elidedText(creationDate, Qt::ElideRight,
                                                                 rectCreationData.width());

            if(strCreationDateTemp != creationDate)
            {
                bElidedForText = true;
            }
            painter->drawText(rectCreationData.left(),
                              rectCreationData.top(),
                              rectCreationData.width(),
                              rectCreationData.height(),
                              Qt::AlignTop|Qt::AlignLeft,
                              strCreationDateTemp,
                              &rectCreationData);
        }

// 显示不下，干脆不要显示了
#if 0
        //Creator
        QRect rectCreator = getRectCreator(option, index);
        rectCreator.setRight(qMin(rectCreator.right(), nOptionRectRight));
        if(!creator.isEmpty() && !bElidedForText)
        {
            const QString strCreatorTemp  = metricsStarts.elidedText("by " + creator, Qt::ElideRight, rectCreator.width());
            painter->drawText(rectCreator.left(),
                              rectCreator.top(),
                              rectCreator.width(),
                              rectCreator.height(),
                              Qt::AlignTop|Qt::AlignLeft,
                              strCreatorTemp,
                              &rectCreator);
        }
#endif
        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

/************************************
* 名称: sizeHint
* 功能: 重写父类函数，获取item项大小时调用
* 返回:   void
* 时间:   2016/03/10
* 作者:   hels
************************************/
QSize DownLoadItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.data(Constants::DownLoadItemType).toString() == Constants::CUSTOM_DOWNLOAD_ITEM_TYPE)
    {        
        int nWidthParent = -1;
        int nDescTextWidth = 0;

        const QAbstractItemModel *pModel = index.model();
        if (pModel)
        {
            QWidget *pWidgetTable = dynamic_cast<QWidget *>(pModel->parent());
            if (pWidgetTable)
            {
                QWidget *pWidget = dynamic_cast<QWidget *>(pWidgetTable->parent());
                if(pWidget)
                {
                    //table parent
                    nWidthParent = pWidget->width();
                }
            }
        }

        // 实现界面自适应分辨率问题，描述文本占总宽度一半（如果固定宽度，在不同分辨率下显示会存在问题）
        if (nWidthParent != -1)
        {
            nDescTextWidth = nWidthParent * Constants::DESCRIPION_COL_WIDTH_RATION;
        }

        const int nWidthMaxDes = Constants::PREVIEW_ICON_WIDTH + nDescTextWidth + 2*Constants::CONTENT_SPACE;
        const int nWidthMinDes = Constants::PREVIEW_ICON_WIDTH + nDescTextWidth - Constants::CONTENT_SPACE;

        const int nItemHeightNew = Constants::PREVIEW_ICON_HEIGHT + 2*Constants::CONTENT_SPACE;
        int nItemWidthNew = nWidthMaxDes;
        if(Constants::ACTION_TABLE_PREVIEW_MIN_WIDTH > nWidthParent)
        {
            nItemWidthNew = nWidthMinDes;
        }

        return QSize(nItemWidthNew, nItemHeightNew);
    }
    else
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

/************************************
* 名称: editorEvent
* 功能: 重写父类函数，判断是否点击预览图
* 返回:   void
* 时间:   2016/04/15
* 作者:   hels
************************************/
bool DownLoadItemDelegate::editorEvent(QEvent *event,
                                       QAbstractItemModel *model,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index)
{
    //自定义描述项类型
    if(index.data(Constants::DownLoadItemType).toString() == Constants::CUSTOM_DOWNLOAD_ITEM_TYPE)
    {
        ActItemData actItem = index.data(Constants::ActionItemData).value<ActItemData>();

        //预览图位置计算
        QRect previewRect = getRectActionIcon(option, index);

        QRect rectPraiseIcon = getRectPraiseIcon(option, index);

        // 评论图标位置
        QRect rectCommentIcon = getRectCommentIcon(option, index);

        QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (mouseEvent && event->type() == QEvent::MouseButtonPress)
        {
            if (previewRect.contains(mouseEvent->pos()))
            {
                emit SigActionPreviewClicked(actItem);
            }

            if (rectCommentIcon.contains(mouseEvent->pos()))
            {
                emit sigOnActionCommentIconClicked(actItem);
            }

//////////////////////暂时只支持在动作详情页点赞，其他地方只负责展示//////////////////////////////
//            if (rectPraiseIcon.contains(mouseEvent->pos()))
//            {
//                emit sigOnActionPraiseIconClicked(actItem);
//            }
        }

        if (mouseEvent && event->type() == QEvent::MouseMove &&
           (previewRect.contains(mouseEvent->pos())) || (rectCommentIcon.contains(mouseEvent->pos()))/* || (rectPraiseIcon.contains(mouseEvent->pos()))*/)
        {
            QCursor cursor(Qt::PointingHandCursor);
            QApplication::setOverrideCursor(cursor);
        }
        else
        {
            QCursor cursor(Qt::ArrowCursor);
            QApplication::setOverrideCursor(cursor);
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

/************************************
* 名称: getRectActionIcon
* 功能: 获取动作图标绘图区域
* 返回:   动作图标绘图区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectActionIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //自定义描述项类型
    if(index.data(Constants::DownLoadItemType).toString() == Constants::CUSTOM_DOWNLOAD_ITEM_TYPE)
    {
        int space = Constants::CONTENT_SPACE;//margin
        return  QRect(option.rect.topLeft() + QPoint(space, space),
                                  QSize(Constants::PREVIEW_ICON_WIDTH, Constants::PREVIEW_ICON_HEIGHT));
    }

    return QRect();
}

/************************************
* 名称: getRectActionTitle
* 功能: 获取动作名称绘图区域
* 返回:   动作名称绘图区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectActionTitle(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectActionIcon = getRectActionIcon(option, index);
    if (rectActionIcon.isValid())
    {
        int space = Constants::CONTENT_SPACE;//margin
        int nDescTextWidth = option.rect.width() * Constants::DESCRIPTION_MAX_WIDTH_RATION;
        ActItemData itemData = index.data(Constants::ActionItemData).value<ActItemData>();
        QString strActionTitle = itemData.m_sActName;

        QFont font = QApplication::font();
        font.setPixelSize(FONT_PIXEL_SIZE_18);
        QFontMetrics metrics(font);
        int nTextWidth = metrics.width(strActionTitle);
        if (nTextWidth > nDescTextWidth)
        {
            nTextWidth = nDescTextWidth;
        }
        QRect rectTitle = QRect(rectActionIcon.topRight() + QPoint(space, 0),
                                QSize(nTextWidth + space, metrics.height()));  // 如果宽度不加上space，英文状态可能显示不全

        // 如果存在活动标签，则活动标签和文本内容各占DESCRIPTION_MAX_WIDTH width一半
        QString strSchemTag = itemData.m_strSchemeName;
        if (!strSchemTag.isEmpty())
        {
            if (nTextWidth > nDescTextWidth/2)
            {
                nTextWidth = nDescTextWidth/2;
            }
            rectTitle = QRect(rectActionIcon.topRight() + QPoint(space, 0),
                                    QSize(nTextWidth + space, metrics.height()));
        }

        return rectTitle;
    }

    return QRect();
}

/************************************
* 名称: getRectSchemeTag
* 功能: 获取活动标签区域
* 返回:   活动标签区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectSchemeTag(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectActionTitle = getRectActionTitle(option, index);
    if (rectActionTitle.isValid())
    {
        int space = Constants::CONTENT_SPACE;//margin
        ActItemData itemData = index.data(Constants::ActionItemData).value<ActItemData>();
        QString strSchemeTag = itemData.m_strSchemeName;
        if (strSchemeTag.isEmpty())
        {
            return QRect();
        }

        QFont font = QApplication::font();
        font.setPixelSize(FONT_PIXEL_SIZE_18);
        QFontMetrics metrics(font);

        int nPadding = 4;  // 文本距离背景框边距
        font.setPixelSize(FONT_PIXEL_SIZE_14);
        QFontMetrics metricsActivityTag(font);
        int nTextWidth = metricsActivityTag.width(strSchemeTag); // 文本宽度
       return QRect(rectActionTitle.topRight() + QPoint(space, 1), QSize(nTextWidth + 2 * nPadding, metrics.height()));
    }
    return QRect();
}

/************************************
* 名称: getRectExamineStatus
* 功能: 获取审核状态区域
* 返回:   审核状态区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectExamineStatus(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QRect rectActionIcon = getRectActionIcon(option, index);
    if (rectActionIcon.isValid())
    {
        ActItemData itemData = index.data(Constants::ActionItemData).value<ActItemData>();

        //绘制status
        if(m_bEnableShowApproveStatus && (eActStatusApproving == itemData.m_nActionStatus) || (eActStatusNoApproved == itemData.m_nActionStatus))
        {
//            QString strActionStatus;

//            if(eActStatusApproving == itemData.m_nActionStatus)
//            {
//                strActionStatus = QObject::tr("Pending");
//            }
//            else if (eActStatusNoApproved == itemData.m_nActionStatus)
//            {
//                strActionStatus = QObject::tr("NoPass");
//            }

//            QFont font = QApplication::font();
//            font.setPixelSize(FONT_PIXEL_SIZE_16);
//            QFontMetrics metrics(font);
//            int nTextWidth = metrics.width(strActionStatus); // 文本宽度
//            return QRect(rectActionIcon.topLeft() + QPoint(0, (rectActionIcon.height() - metrics.height())/2),
//                                     QSize(nTextWidth + Constants::TEXT_MORE_SPACE, metrics.height()));
              return rectActionIcon;
        }

    }

    return QRect();
}

/************************************
* 名称: getRectActionDescription
* 功能: 获取动作描述区域
* 返回:   动作描述绘图区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectActionDescription(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectActionTitle = getRectActionTitle(option, index);
    if (rectActionTitle.isValid())
    {
        int space = Constants::CONTENT_SPACE;//margin
        int nDescTextWidth = option.rect.width() * Constants::DESCRIPTION_MAX_WIDTH_RATION;
        QFont font = QApplication::font();
        font.setPixelSize(FONT_PIXEL_SIZE_14);

        QFontMetrics metricsDes(font);
        return QRect(rectActionTitle.bottomLeft() + QPoint(0, space),
                              QSize(nDescTextWidth, metricsDes.height()));
    }

    return QRect();
}

/************************************
* 名称: getRectPraiseIcon
* 功能: 获取点赞图标区域
* 返回:   点赞图标区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectPraiseIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectActionIcon = getRectActionIcon(option, index);
    if (rectActionIcon.isValid())
    {
        int space = Constants::CONTENT_SPACE;//margin
        return QRect(rectActionIcon.bottomRight() + QPoint(space, -Constants::ICON_STARTS_HEGIHT),
                                     QSize(Constants::ICON_STARTS_WIDTH, Constants::ICON_STARTS_HEGIHT));
    }
    return QRect();
}

/************************************
* 名称: getRectPraiseNumber
* 功能: 获取点赞数目
* 返回:  动作点赞数目
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectPraiseNumber(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectStartIcon = getRectPraiseIcon(option, index);
    if (rectStartIcon.isValid())
    {
        ActItemData itemData = index.data(Constants::ActionItemData).value<ActItemData>();
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;
        const QString &starts = QString::number(itemData.m_nActionCollectTime);
        QFont font = QApplication::font();
        font.setPixelSize(FONT_PIXEL_SIZE_12);
        QFontMetrics metricsStarts(font);
        // 为了对齐，文本需要向上偏移
        return QRect(rectStartIcon.topRight() + QPoint(icon_text_margin, -icon_text_margin),
                                 QSize(metricsStarts.width(starts) + Constants::TEXT_MORE_SPACE, metricsStarts.height()));
    }
    return QRect();
}

/************************************
* 名称: getRectDownloadIcon
* 功能: 获取下载次数图标区域
* 返回:  下载次数图标区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectDownloadIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectStartIcon = getRectPraiseIcon(option, index);
    QRect rectStartNumber = getRectPraiseNumber(option, index);
    if (rectStartNumber.isValid() && rectStartIcon.isValid())
    {
        int space = Constants::CONTENT_SPACE;//margin
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;
        // 以rectActionIcon为起点，显示更加水平
        return QRect(rectStartIcon.topRight()+ QPoint(icon_text_margin + rectStartNumber.width(), 0) + QPoint(space, 0),
                     QSize(Constants::ICON_DOWNLOADS_WIDTH, Constants::ICON_DOWNLOADS_HEGIHT));
//        return QRect(rectStartNumber.topRight() + QPoint(space, 0),
//                                       QSize(Constants::ICON_DOWNLOADS_WIDTH, Constants::ICON_DOWNLOADS_HEGIHT));
    }
    return QRect();
}

/************************************
* 名称: getRectDownloadCount
* 功能: 获取下载次数区域
* 返回:  下载次数区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectDownloadCount(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectDownloadIcon = getRectDownloadIcon(option, index);
    if (rectDownloadIcon.isValid())
    {
        ActItemData itemData = index.data(Constants::ActionItemData).value<ActItemData>();
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;
        const QString &downLoads = QString::number(itemData.m_nActionDownloads);
        QFont font = QApplication::font();
        font.setPixelSize(FONT_PIXEL_SIZE_12);
        QFontMetrics metricsStarts(font);
        return  QRect(rectDownloadIcon.topRight() + QPoint(icon_text_margin, -icon_text_margin), QSize(metricsStarts.width(downLoads) + Constants::TEXT_MORE_SPACE, metricsStarts.height()));
    }
    return QRect();
}

/************************************
* 名称: getRectCommentIcon
* 功能: 获取评论图标区域
* 返回:  评论图标区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectCommentIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectDownloadIcon = getRectDownloadIcon(option, index);
    QRect rectDownCount = getRectDownloadCount(option, index);
    if (rectDownCount.isValid() && rectDownloadIcon.isValid())
    {
        int space = Constants::CONTENT_SPACE;//margin
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;
        return QRect(rectDownloadIcon.topRight()+ QPoint(icon_text_margin + rectDownCount.width(), 0) + QPoint(space, 0),
                     QSize(Constants::ICON_COMMENTS_WIDTH, Constants::ICON_COMMENTS_HEGIHT));
        //return QRect(rectDownCount.topRight() + QPoint(space, 0), QSize(Constants::ICON_COMMENTS_WIDTH, Constants::ICON_COMMENTS_HEGIHT));
    }
    return QRect();
}

/************************************
* 名称: getRectCommentCount
* 功能: 获取动作评论次数区域
* 返回:  动作评论次数区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectCommentCount(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectCommentIcon = getRectCommentIcon(option, index);
    if (rectCommentIcon.isValid())
    {
        ActItemData itemData = index.data(Constants::ActionItemData).value<ActItemData>();
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;
        const QString &comments = QString::number(itemData.m_nActionComments);
        QFont font = QApplication::font();
        font.setPixelSize(FONT_PIXEL_SIZE_12);
        QFontMetrics metricsStarts(font);
        // 为了对齐，文本要向下偏移
        return  QRect(rectCommentIcon.topRight() + QPoint(icon_text_margin, -icon_text_margin), QSize(metricsStarts.width(comments) + Constants::TEXT_MORE_SPACE, metricsStarts.height()));
    }
    return QRect();
}

/************************************
* 名称: getRectCreateTime
* 功能: 获取动作创建时间区域
* 返回:  动作创建时间区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectCreateTime(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectCommentCount = getRectCommentCount(option, index);
    if (rectCommentCount.isValid())
    {
        int space = Constants::CONTENT_SPACE;//margin
        ActItemData itemData = index.data(Constants::ActionItemData).value<ActItemData>();
        QDateTime createDatetTime = QDateTime::fromString(itemData.m_sPublishTime, NET_ACTION_DATE_FORMAT);
        const QString creationDate = createDatetTime.date().toString("yyyy-MM-dd");
        QFont font = QApplication::font();
        font.setPixelSize(FONT_PIXEL_SIZE_12);
        QFontMetrics metricsStarts(font);
        return  QRect(rectCommentCount.topRight() + QPoint(space, 0), QSize(metricsStarts.width(creationDate) + Constants::TEXT_MORE_SPACE, metricsStarts.height()));
    }
    return QRect();
}

/************************************
* 名称: getRectCreator
* 功能: 获取动作创建者区域
* 返回:  动作创建者区域
* 时间:   2016/09/27
* 作者:   hwx
************************************/
QRect DownLoadItemDelegate::getRectCreator(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rectCreationData = getRectCreateTime(option, index);
    if (rectCreationData.isValid())
    {
        int space = Constants::CONTENT_SPACE;//margin
        ActItemData itemData = index.data(Constants::ActionItemData).value<ActItemData>();
        const QString &creator = itemData.m_sUserName;
        QFont font = QApplication::font();
        font.setPixelSize(FONT_PIXEL_SIZE_12);
        QFontMetrics metricsStarts(font);
        return  QRect(rectCreationData.topRight() + QPoint(space, 0), QSize(metricsStarts.width(creator) + Constants::TEXT_MORE_SPACE, metricsStarts.height()));
    }
    return QRect();
}
