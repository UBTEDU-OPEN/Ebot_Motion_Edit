/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：rankitemdeletegate.cpp
* 创建时间：2016/03/30
* 文件标识：
* 文件摘要：排行榜项委托，显示推广活动排行榜
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/08
* 版本摘要：
*/

#include <QDir>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>

#include "userrankitemdeletegate.h"
#include "ubxactionlibconstants.h"
#include "configs.h"

UserRankItemDelegate::UserRankItemDelegate(QObject *parent)
{

}

UserRankItemDelegate::~UserRankItemDelegate()
{

}

void UserRankItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 默认表格右上角位置，根据QTableWidget大小自动调整绘图，如果表格控件大小大于
    // 默认值，则根据表格大小调整绘图
    QPoint pointTopRigth = getPointTopRight(option, index);
    int nPaddingLeft = getPaddingValue(option, index, Constants::eLeft);
    int nPaddingCenter = getPaddingValue(option, index, Constants::eCenter);
    int nPaddingRight = getPaddingValue(option, index, Constants::eRight);

    //自定义描述项类型
    if(index.data(Constants::DownLoadItemType).toString() == Constants::CUSTOM_DOWNLOAD_ITEM_TYPE)
    {
        painter->save();

        UserRankItem rankItemData = index.data(Constants::RankItemData).value<UserRankItem>();
        ActItemData &actionItem = rankItemData.getActionItemData();

        //先绘制背景
        QBrush brush = index.data(Qt::BackgroundRole).value<QBrush>();
        if(Qt::NoBrush != brush.style())
        {
            painter->fillRect(option.rect, brush);
        }

        int space = Constants::CONTENT_SPACE;
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;

        // 绘制奖牌图标
        QRect rankIconRect = getRectRankIcon(option, index);
        drawMedal(rankItemData.getRankOrder(), painter, rankIconRect);

        // 绘制用户头像
        QPixmap pixmapUserPicture;
        QRect rectUserPic = getRectUserIcon(option, index);
        if(pixmapUserPicture.load(actionItem.m_sUserImageUrl))
        {
              getRoundImage(pixmapUserPicture, QSize(Constants::RANK_USER_PICTURE_WIDTH, Constants::RANK_USER_PICTURE_HEIGHT));
//            pixmapUserPicture.scaled(Constants::RANK_USER_PICTURE_WIDTH, Constants::RANK_USER_PICTURE_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            painter->drawPixmap(rectUserPic, pixmapUserPicture);
        }
        else
        {
            QString strFielName = CConfigs::getLocalResPath(UBX_LOGIN_HEAD_IMAGE);//如果加载图片不成功则加载默认图片
            if(pixmapUserPicture.load(strFielName))
            {
                getRoundImage(pixmapUserPicture, QSize(Constants::RANK_USER_PICTURE_WIDTH, Constants::RANK_USER_PICTURE_HEIGHT));
                painter->drawPixmap(rectUserPic, pixmapUserPicture);
            }
        }

        // 绘制用户名
        const QString sUserName = actionItem.m_sUserName;
        QFont fontUserName = QApplication::font();
        fontUserName.setPixelSize(FONT_PIXEL_SIZE_20);
        painter->setPen(QColor("#d3d3d3"));
        painter->setFont(fontUserName);
        QFontMetrics metricsUserName(fontUserName);

        QRect rectUserName = getRectUserName(option, index);
        QString sEditedUserName = metricsUserName.elidedText(sUserName, Qt::ElideRight, rectUserName.width());
        painter->drawText(rectUserName.left(),
                          rectUserName.top(),
                          rectUserName.width(),
                          rectUserName.height(),
                          Qt::AlignLeft,
                          sEditedUserName,
                          &rectUserName);

        // 绘制动作图标
        QSize sizeActionIcon(Constants::RANK_ICON_ACTION_TYPE_WIDTH, Constants::RANK_ICON_ACTION_TYPE_HEIGHT);
        QRect rectActionIcon = getRectActionIcon(option, index);
        int nActType = actionItem.m_nActType;
        QString strActIcon = getActionIcon((emActionType)nActType);
        drawPixMap(painter, strActIcon, rectActionIcon,sizeActionIcon );

        // 绘制动作名称
        const QString sActionName = actionItem.m_sActName;
        QFont fontActionName = QApplication::font();
        fontActionName.setPixelSize(FONT_PIXEL_SIZE_16);
        painter->setPen(QColor(UBX_COLOR_GREY));
        painter->setFont(fontActionName);
        QFontMetrics metricsActionName(fontActionName);

        QRect rectActionName = getRectActionName(option, index);
        QString sEditedActionName = metricsActionName.elidedText(sActionName, Qt::ElideRight, rectActionName.width());
        painter->drawText(rectActionName.left(),
                          rectActionName.top(),
                          rectActionName.width(),
                          rectActionName.height(),
                          Qt::AlignLeft,
                          sEditedActionName,
                          &rectActionName);

        // 绘制动作热度图标
        QString strIconHot = getHotIconByRankOrder(rankItemData.getRankOrder());
        QSize sizeHotIcon(Constants::RANK_ICON_HOT_WIDTH, Constants::RANK_ICON_HOT_HEIGHT);
        //QRect rectHotIcon = QRect(rectUserName.topRight() + QPoint(Constants::RANK_LEFT_RIGHT_INTERNAL_SPACE, space), sizeHotIcon);
//        QRect rectHotIcon = QRect(option.rect.topRight() - QPoint(nHotIconTopLeftXPos, -nHotIconTopLeftHeightOffset), sizeHotIcon);
        QRect rectHotIcon = getRectHotIcon(option, index);
        drawPixMap(painter, strIconHot, rectHotIcon, sizeHotIcon);

        // 绘制动作热度值
        const QString sHotValue = QString("%1").arg(actionItem.m_nActionHotValue);
        QFont fontHotValue = QApplication::font();
        fontHotValue.setPixelSize(FONT_PIXEL_SIZE_18);
        painter->setPen(QColor(UBX_COLOR_GREY));
        painter->setFont(fontHotValue);
        QFontMetrics metricsHotValue(fontHotValue);
        QRect rectHotValue = getRectHotValue(option, index);
        QString sEditedHotValue = metricsHotValue.elidedText(sHotValue, Qt::ElideRight, rectHotValue.width());
        painter->drawText(rectHotValue.left(),
                          rectHotValue.top(),
                          rectHotValue.width(),
                          rectHotValue.height(),
                          Qt::AlignLeft,
                          sEditedHotValue,
                          &rectHotValue);

        // 绘制行分割线
        QPen penGridLine;
        penGridLine.setColor("#343434");
        penGridLine.setWidth(1);
        painter->setPen(penGridLine);
        QPoint pointLineStart = option.rect.topLeft() + QPoint(nPaddingLeft, Constants::RANK_TABLE_COL_HEIGHT - Constants::RANK_TABLE_GRID_LINE_WIDTH);
//        QPoint pointLineEnd = option.rect.topRight() +  QPoint(-Constants::RANK_TABLE_ROW_PADDING_RIGHT, Constants::RANK_TABLE_COL_HEIGHT - Constants::RANK_TABLE_GRID_LINE_WIDTH);
        QPoint pointLineEnd = pointTopRigth +  QPoint(-nPaddingRight, Constants::RANK_TABLE_COL_HEIGHT - Constants::RANK_TABLE_GRID_LINE_WIDTH);
        painter->drawLine(pointLineStart, pointLineEnd);

        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize UserRankItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.data(Constants::DownLoadItemType).toString() == Constants::CUSTOM_DOWNLOAD_ITEM_TYPE)
    {
        return QSize(Constants::RANK_TABLE_ROW_WIDTH, Constants::RANK_TABLE_COL_HEIGHT);
    }
    else
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

bool UserRankItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    //自定义描述项类型
    if(index.data(Constants::DownLoadItemType).toString() == Constants::CUSTOM_DOWNLOAD_ITEM_TYPE)
    {
        int nSpace = Constants::CONTENT_SPACE;//margin
        int nPaddintLeft = getPaddingValue(option, index, Constants::eLeft);

        // 用户头像区
        QRect rectUserPhoto = getRectUserIcon(option, index);

        // 用户名区
        QRect rectUserName = getRectUserName(option, index);

        // 用户排名数据
        UserRankItem userRankItem = index.data(Constants::RankItemData).value<UserRankItem>();

        QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (mouseEvent && event->type() == QEvent::MouseButtonPress)
        {
            if (rectUserName.contains(mouseEvent->pos()))
            {
                emit sigOnActionDetailedClicked(userRankItem.getActionItemData());
            }
            else if (rectUserPhoto.contains(mouseEvent->pos()))
            {
                emit sigOnActionDetailedClicked(userRankItem.getActionItemData());
            }
        }

        // 光标移动到上面变成手形
        if (mouseEvent && event->type() == QEvent::MouseMove && (rectUserName.contains(mouseEvent->pos()) || rectUserPhoto.contains(mouseEvent->pos())))
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
* 名称: getActionIcon
* 功能: 根据动作类型获取对应图标资源
* 参数: [in] eType: 动作类型
* 返回:   void
* 时间:   2016/09/01
* 作者:   hwx
************************************/
QString UserRankItemDelegate::getActionIcon(emActionType eType) const
{
    QString strActionIcon = "";
    switch (eType) {
        case eActTypeDance:
        {
            strActionIcon = ICON_ACTION_TYPE_DANCE;
            break;
        }
        case eActTypeStory:
        {
            strActionIcon = ICON_ACTION_TYPE_STORY;
            break;
        }
        case eActTypeMusic:
        {
            strActionIcon = ICON_ACTION_TYPE_MUSIC;
            break;
        }
        case eActTypeSport:
        {
            strActionIcon = ICON_ACTION_TYPE_SPORTS;
            break;
        }
        case eActTypeEducation:
        {
            strActionIcon = ICON_ACTION_TYPE_EDUCATE;
            break;
        }
        default:
        {
            strActionIcon = ICON_ACTION_TYPE_DEFAULT;
            break;
        }
    }
    return strActionIcon;
}

/************************************
* 名称: getHotIconByRankOrder
* 功能: 根据排名获取热度图标
* 参数: [in] nOrder: 动作排名
* 返回:   void
* 时间:   2016/09/08
* 作者:   hwx
************************************/
QString UserRankItemDelegate::getHotIconByRankOrder(int nOrder) const
{
    QString strHotIcon = Constants::ICON_HOT_OTHER;
    switch (nOrder) {
    case 1:
    {
        strHotIcon = Constants::ICON_HOT_1;
        break;
    }
    case 2:
    {
        strHotIcon = Constants::ICON_HOT_2;
        break;
    }
    case 3:
    {
        strHotIcon = Constants::ICON_HOT_3;
        break;
    }
    default:
        break;
    }

    return strHotIcon;
}

/************************************
* 名称: getMedalIconByRankOrder
* 功能: 根据排名获取奖牌图标
* 参数: [in] nOrder: 动作排名
* 返回:   void
* 时间:   2016/09/10
* 作者:   hwx
************************************/
QString UserRankItemDelegate::getMedalIconByRankOrder(int nOrder) const
{
    QString strIconMedal ="";
    switch (nOrder) {
    case 1:
    {
        strIconMedal = Constants::ICON_MEDAL_1;
        break;
    }
    case 2:
    {
        strIconMedal = Constants::ICON_MEDAL_2;
        break;
    }
    case 3:
    {
        strIconMedal = Constants::ICON_MEDAL_3;
        break;
    }
    default:
        break;
    }

    return strIconMedal;
}

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
void UserRankItemDelegate::drawMedal(int nRankOrder, QPainter *painter, const QRect &rectWhere) const
{
    QRect rectBounding = rectWhere;
    QPen penPrevious = painter->pen(); // 记录原来的画笔，绘制完成后需要恢复
    QBrush brushPrevious = painter->brush();

    // 奖牌图标
    QString strIconMedal = getMedalIconByRankOrder(nRankOrder);
    if (!strIconMedal.isEmpty())
    {
        drawPixMap(painter, strIconMedal, rectBounding, QSize(Constants::RANK_ICON_WIDTH, Constants::RANK_ICON_HEIGHT));
    }
    else
    {
        const QString strRank = QString("%1").arg(nRankOrder);
        QFont fontRank = QApplication::font();
        fontRank.setBold(true);
        fontRank.setPixelSize(FONT_PIXEL_SIZE_18);
        painter->setPen(QColor(UBX_COLOR_DARK_GREY));
        painter->setFont(fontRank);
        QFontMetrics metricsRank(fontRank);
        QString strEditedRank = metricsRank.elidedText(strRank, Qt::ElideRight, rectWhere.width());
        painter->drawText(rectWhere.left(),
                          rectWhere.top(),
                          rectWhere.width(),
                          rectWhere.height(),
                          Qt::AlignCenter,
                          strEditedRank,
                          &rectBounding);
    }

    painter->setPen(penPrevious);
    painter->setBrush(brushPrevious);
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
void UserRankItemDelegate::getRoundImage(QPixmap &pixMapSource, QSize size) const
{
    QImage resultImage(size,QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&resultImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(resultImage.rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QPixmap pixmapMask(":/res/images/head_mask.png");
    painter.drawPixmap(0, 0, pixmapMask.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOut);
    painter.drawPixmap(0, 0, pixMapSource.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);

    painter.end();
    pixMapSource = QPixmap::fromImage(resultImage);
}

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
void UserRankItemDelegate::drawPixMap(QPainter *painter, const QString &strImage, const QRect &rectWhere, QSize size) const
{
    if (!painter)
    {
        return;
    }

    QPixmap pixmapImage;
    pixmapImage.fill(Qt::transparent);
    QRect rectPixmap = rectWhere;

    if (pixmapImage.load(strImage))
    {
        pixmapImage.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        painter->drawPixmap(rectPixmap, pixmapImage);
    }
    else
    {
         QString strFielName = CConfigs::getLocalResPath(UBX_DEFAULT_IMAGE);//如果加载图片不成功则加载默认图片
         if(pixmapImage.load(strFielName))
         {
             pixmapImage.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
             painter->drawPixmap(rectPixmap, pixmapImage);
         }
    }
}

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
int UserRankItemDelegate::getPaddingValue(const QStyleOptionViewItem &option, const QModelIndex &index, Constants::emDirection emDir) const
{
    int nResult = 0;
    const QAbstractItemModel *pModel = index.model();
    if (!pModel)
    {
        return nResult;
    }

    QWidget *pWidget = dynamic_cast<QWidget *> (pModel->parent());
    if (!pWidget)
    {
        return nResult;
    }

    // 默认表格右上角位置，根据QTableWidget大小自动调整绘图，如果表格控件大小大于
    // 默认值，则根据表格大小调整绘图
    int nPaddingLeft = Constants::RANK_TABLE_ROW_PADDING_LEFT;
    int nPaddingCenter = Constants::RANK_TABLE_ROW_PADDING_CENTER;
    int nPaddingRight = Constants::RANK_TABLE_ROW_PADDING_RIGHT;
    if (pWidget->width() > Constants::RANK_TABLE_ROW_WIDTH)
    {
        // 左边、中间、右边空白区域总大小
        int nTotalSpace = pWidget->width() - Constants::RANK_TABLE_ROW_WIDTH_NOPADDINT;
        nPaddingLeft = Constants::RANK_TABLE_PADDIN_LEFT_RATIO * nTotalSpace;
        nPaddingCenter = Constants::RANK_TABLE_PADDING_CENTER_RATIO * nTotalSpace;
        nPaddingRight = Constants::RANK_TABLE_PADDING_RIGHT_RATIO * nTotalSpace;
    }

    switch (emDir) {
        case Constants::eLeft:
        {
            nResult = nPaddingLeft;
            break;
        }
        case Constants::eRight:
        {
            nResult = nPaddingRight;
            break;
        }
        case Constants::eCenter:
        {
            nResult = nPaddingCenter;
            break;
        }
        default:
        {
            nResult = nPaddingLeft;
            break;
        }
    }

    return nResult;
}

/************************************
* 名称: getPointTopRight
* 功能: 获取右上角点
* 参数: [in] option 绘图信息
* 参数: [in] index: 模型index
* 返回:   int
* 时间:   2016/09/14
* 作者:   hwx
************************************/
QPoint UserRankItemDelegate::getPointTopRight(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPoint pointTopRigth = option.rect.topLeft() + QPoint(Constants::RANK_TABLE_ROW_WIDTH, 0);
    const QAbstractItemModel *pModel = index.model();
    if (!pModel)
    {
        return pointTopRigth;
    }

    QWidget *pWidget = dynamic_cast<QWidget *> (pModel->parent());
    if (!pWidget)
    {
        return pointTopRigth;
    }

    // 默认表格右上角位置，根据QTableWidget大小自动调整绘图
    if (pWidget->width() > Constants::RANK_TABLE_ROW_WIDTH)
    {
        pointTopRigth = option.rect.topLeft() + QPoint(pWidget->width(), 0);
    }

    return pointTopRigth;
}

/************************************
* 名称: getRectRankIcon
* 功能: 获取动作排行图标区域
* 参数: [in] option 绘图信息
* 参数: [in] index: 模型index
* 返回:   动作排行图标区域
* 时间:   2016/09/14
* 作者:   hwx
************************************/
QRect UserRankItemDelegate::getRectRankIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 默认表格右上角位置，根据QTableWidget大小自动调整绘图，如果表格控件大小大于
    // 默认值，则根据表格大小调整绘图
    int nPaddingLeft = getPaddingValue(option, index, Constants::eLeft);

    //自定义描述项类型
    if(index.data(Constants::DownLoadItemType).toString() == Constants::CUSTOM_DOWNLOAD_ITEM_TYPE)
    {
        UserRankItem rankItemData = index.data(Constants::RankItemData).value<UserRankItem>();
        ActItemData &actionItem = rankItemData.getActionItemData();

        int space = Constants::CONTENT_SPACE;
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;

        return QRect(option.rect.topLeft() + QPoint(nPaddingLeft, 2 * space), QSize(Constants::RANK_ICON_WIDTH, Constants::RANK_ICON_HEIGHT));
    }
    return QRect();
}

/************************************
* 名称: getRectUserIcon
* 功能: 获取用户头像图标区域
* 参数: [in] option 绘图信息
* 参数: [in] index: 模型index
* 返回:   用户头像图标区域
* 时间:   2016/09/28
* 作者:   hwx
************************************/
QRect UserRankItemDelegate::getRectUserIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 排行榜图标区域
    QRect rectRankIcon = getRectRankIcon(option, index);
    if(rectRankIcon.isValid())
    {
        UserRankItem rankItemData = index.data(Constants::RankItemData).value<UserRankItem>();
        ActItemData &actionItem = rankItemData.getActionItemData();

        int space = Constants::CONTENT_SPACE;
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;

        return QRect(rectRankIcon.topRight() + QPoint(space, -space), QSize(Constants::RANK_USER_PICTURE_WIDTH, Constants::RANK_USER_PICTURE_HEIGHT));
    }

    return QRect();
}

/************************************
* 名称: getRectUserName
* 功能: 获取用户名称区域
* 参数: [in] option 绘图信息
* 参数: [in] index: 模型index
* 返回:   用户名称区域
* 时间:   2016/09/28
* 作者:   hwx
************************************/
QRect UserRankItemDelegate::getRectUserName(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 排行榜图标区域
    QRect rectUserIcon = getRectUserIcon(option, index);
    if(rectUserIcon.isValid())
    {
        UserRankItem rankItemData = index.data(Constants::RankItemData).value<UserRankItem>();
        ActItemData &actionItem = rankItemData.getActionItemData();
        int space = Constants::CONTENT_SPACE;

        // 绘制用户名
        const QString sUserName = actionItem.m_sUserName;
        QFont fontUserName = QApplication::font();
        fontUserName.setPixelSize(FONT_PIXEL_SIZE_20);
        QFontMetrics metricsUserName(fontUserName);
        int nUserNameWidth = metricsUserName.width(sUserName);
        return QRect(rectUserIcon.topRight() + QPoint(space, 0), QSize(nUserNameWidth + Constants::TEXT_MORE_SPACE, metricsUserName.height()));
    }

    return QRect();
}

/************************************
* 名称: getRectActionIcon
* 功能: 获取动作图标区域
* 参数: [in] option 绘图信息
* 参数: [in] index: 模型index
* 返回:   动作图标区域
* 时间:   2016/09/28
* 作者:   hwx
************************************/
QRect UserRankItemDelegate::getRectActionIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 排行榜图标区域
    QRect rectUserName = getRectUserName(option, index);
    if(rectUserName.isValid())
    {
        UserRankItem rankItemData = index.data(Constants::RankItemData).value<UserRankItem>();
        ActItemData &actionItem = rankItemData.getActionItemData();

        int space = Constants::CONTENT_SPACE;
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;

        return QRect(rectUserName.bottomLeft() + QPoint(0, space), QSize(Constants::RANK_ICON_ACTION_TYPE_WIDTH, Constants::RANK_ICON_ACTION_TYPE_HEIGHT));
    }

    return QRect();
}

/************************************
* 名称: getRectActionName
* 功能: 获取动作名称区域
* 参数: [in] option 绘图信息
* 参数: [in] index: 模型index
* 返回:   动作名称区域
* 时间:   2016/09/28
* 作者:   hwx
************************************/
QRect UserRankItemDelegate::getRectActionName(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 排行榜图标区域
    QRect rectActionIcon = getRectActionIcon(option, index);
    if(rectActionIcon.isValid())
    {
        UserRankItem rankItemData = index.data(Constants::RankItemData).value<UserRankItem>();
        ActItemData &actionItem = rankItemData.getActionItemData();

        int space = Constants::CONTENT_SPACE;
        int icon_text_margin = Constants::ICON_TEXT_MARGIN;

        const QString sActionName = actionItem.m_sActName;
        QFont fontActionName = QApplication::font();
        fontActionName.setPixelSize(FONT_PIXEL_SIZE_16);
        QFontMetrics metricsActionName(fontActionName);
        int nActionNameWidth = metricsActionName.width(sActionName);
        return QRect(rectActionIcon.topRight() + QPoint(icon_text_margin, icon_text_margin),
                     QSize(nActionNameWidth + Constants::TEXT_MORE_SPACE, metricsActionName.height()));
    }

    return QRect();
}

/************************************
* 名称: getRectHotIcon
* 功能: 获取热度图标区域
* 参数: [in] option 绘图信息
* 参数: [in] index: 模型index
* 返回:   热度图标区域
* 时间:   2016/09/28
* 作者:   hwx
************************************/
QRect UserRankItemDelegate::getRectHotIcon(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 默认表格右上角位置，根据QTableWidget大小自动调整绘图，如果表格控件大小大于
    // 默认值，则根据表格大小调整绘图
    QPoint pointTopRigth = getPointTopRight(option, index);
    int nPaddingRight = getPaddingValue(option, index, Constants::eRight);

    //自定义描述项类型
    if(index.data(Constants::DownLoadItemType).toString() == Constants::CUSTOM_DOWNLOAD_ITEM_TYPE)
    {
        UserRankItem rankItemData = index.data(Constants::RankItemData).value<UserRankItem>();
        ActItemData &actionItem = rankItemData.getActionItemData();
        int space = Constants::CONTENT_SPACE;

        int nHotIconTopLeftHeightOffset = Constants::RANK_TABLE_COL_HEIGHT/2 - Constants::RANK_ICON_HOT_HEIGHT/2;
        int nHotIconTopLeftXPos = nPaddingRight + Constants::RANK_HOT_VALUE_WIDTH + Constants::RANK_ICON_HOT_WIDTH + space;
        return QRect(pointTopRigth - QPoint(nHotIconTopLeftXPos, -nHotIconTopLeftHeightOffset), QSize(Constants::RANK_ICON_HOT_WIDTH, Constants::RANK_ICON_HOT_HEIGHT));
    }

    return QRect();
}

/************************************
* 名称: getRectHotValue
* 功能: 获取热度值区域
* 参数: [in] option 绘图信息
* 参数: [in] index: 模型index
* 返回:   热度值区域
* 时间:   2016/09/28
* 作者:   hwx
************************************/
QRect UserRankItemDelegate::getRectHotValue(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //自定义描述项类型
    QRect rectHotIcon = getRectHotIcon(option, index);
    if(rectHotIcon.isValid())
    {
        UserRankItem rankItemData = index.data(Constants::RankItemData).value<UserRankItem>();
        ActItemData &actionItem = rankItemData.getActionItemData();
        int space = Constants::CONTENT_SPACE;

        // 动作热度值
        const QString sHotValue = QString("%1").arg(actionItem.m_nActionBrownTime);
        QFont fontHotValue = QApplication::font();
        fontHotValue.setPixelSize(FONT_PIXEL_SIZE_18);
        QFontMetrics metricsHotValue(fontHotValue);
        return QRect(rectHotIcon.topRight() + QPoint(space, 0), QSize(Constants::RANK_HOT_VALUE_WIDTH, metricsHotValue.height()));
    }

    return QRect();
}


