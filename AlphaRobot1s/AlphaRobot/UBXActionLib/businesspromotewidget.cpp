/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：businesspromotewidget.h
* 创建时间：2016/06/17
* 文件标识：
* 文件摘要：运营推广活动控件
*
* 当前版本：1.0.0.0
* 作    者：   hwx
* 完成时间：2016/06/17
* 版本摘要：
*/

#include <QTableWidgetItem>
#include <QCoreApplication>
#include <QDir>

#include "businesspromotewidget.h"
#include "ui_businesspromotewidget.h"
#include "ubxactionlibconstants.h"
#include "configs.h"
#include "actupdatemangr.h"

RETRANSLATEUIFUNCIMPL(BusinessPromoteWidget)
BusinessPromoteWidget::BusinessPromoteWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::BusinessPromoteWidget),
//    m_pPictureFlow(NULL),
    m_pBusinessPromoteData(NULL),
    m_pTableDelegate(NULL)
{
    ui->setupUi(this);

    setupWidget();
    initWidget();

    setDefaultStyle();
}

BusinessPromoteWidget::~BusinessPromoteWidget()
{
    delete ui;
}

/************************************
* 名称: requestRankActionList
* 功能: 请求排行榜数据
* 参数：
* 返回:   void
* 时间:   2016/10/30
* 作者:   hwx
************************************/
int BusinessPromoteWidget::requestRankActionList()
{
    if (!m_pBusinessPromoteData)
    {
        return -1;
    }

    m_pBusinessPromoteData->clearRankActionList();
    return g_actupdatemgr->queryRankActionData(REQUSET_ID_DEFAULT_INVALID, m_pBusinessPromoteData->getId());
}

/************************************
* 名称: addItem
* 功能: 单个用户工作数据
* 参数：[in] itemData:个人用户数据
* 返回:   void
* 时间:   2016/08/08
* 作者:   hwx
************************************/
void BusinessPromoteWidget::addItem(const UserRankItem &itemData)
{
    // 只显示前面RANK_LIST_MAX_NUM
    int row = ui->tableWidgetRankList->rowCount();
    if (row >= Constants::RANK_LIST_MAX_NUM)
    {
        return;
    }

    ui->tableWidgetRankList->insertRow(row);

    //动作列表信息项
    QTableWidgetItem *item = new QTableWidgetItem();
    item->setData(Constants::DownLoadItemType, Constants::CUSTOM_DOWNLOAD_ITEM_TYPE);
    QVariant varUserRankItem;
    varUserRankItem.setValue(itemData);
    item->setData(Constants::RankItemData, varUserRankItem);
    ui->tableWidgetRankList->setItem(row, 0, item);
    ui->tableWidgetRankList->resizeRowToContents(row);
    ui->tableWidgetRankList->resizeColumnToContents(0);
}

/************************************
* 名称: setCoverImage
* 功能: 设置运营活动图片
* 参数：[in] strFile: 图片文件
* 返回:   void
* 时间:   2016/08/18
* 作者:   hwx
************************************/
void BusinessPromoteWidget::setCoverImage(const QString &strFile)
{
    QPixmap pixmapIMage;
    QString strImageFile = strFile;
    if (strFile.isEmpty() || !QFile::exists(strFile))
    {
        strImageFile = CConfigs::getLocalResPath(QString("display%1%2").arg(QDir::separator()).arg(UBX_DISPLAY_IMAGE_1));//如果加载图片不成功则加载默认图片
    }

    if(!pixmapIMage.load(strImageFile))
    {
        AlphaRobotLog::getInstance()->printError(QString("Failed to load image file %1").arg(strImageFile));
        return;
    }

    ui->labelImage->setPixmap(pixmapIMage);
}

/************************************
* 名称: resetUI
* 功能: 复位界面
* 返回:   void
* 时间:   2016/08/18
* 作者:   hwx
************************************/
void BusinessPromoteWidget::resetUI()
{
    ui->tableWidgetRankList->clear();
    ui->textEditDescription->clear();
    ui->tableWidgetRankList->setRowCount(0);
}

/************************************
* 名称: slotUpdateData
* 功能: 更新数据
* 参数：[in] pData:运营活动数据
* 返回:   void
* 时间:   2016/08/08
* 作者:   hwx
************************************/
void BusinessPromoteWidget::slotUpdateData(BusinessPromoteData *pData)
{
    if (!pData)
    {
        return;
    }

    m_pBusinessPromoteData = pData;

    // 复位界面数据
    resetUI();

    // 运营活动详细图片
    setCoverImage(pData->getDetailedImageUrl());

    // 设置活动描述
    ui->textEditDescription->setText(pData->getDescriptionMsg());

    // 运营活动排名项
    QVector<UserRankItem> &vecItemList =  pData->getAllUserRankItem();
    for (int nId = 0; nId < vecItemList.size(); nId++)
    {
        UserRankItem &rankItem = vecItemList[nId];
        rankItem.setRankOrder(nId + 1);
        addItem(vecItemList[nId]);
    }
}

/************************************
* 名称: sigDetailedActionPreviewClicked
* 功能: 预览动作详细信息
* 参数: [in]itemData 对应的动作数据
* 返回:   void
* 时间:   2016/09/01
* 作者:   hwx
************************************/
void BusinessPromoteWidget::slotOnDetailedActionPreviewClicked(ActItemData item)
{
    emit sigDetailedActionPreviewClicked(BUTTON_BUSINESS_PROMOTE_PAGE, item);
}

/************************************
* 名称: slotOnJoinInClicked
* 功能: 点击参加运营活动的槽函数
* 返回:   void
* 时间:   2016/09/02
* 作者:   hwx
************************************/
void BusinessPromoteWidget::slotOnJoinInClicked()
{
    if (!m_pBusinessPromoteData)
    {
        return;
    }

    int nSchemId = m_pBusinessPromoteData->getId();
    emit sigOnActionUpload(nSchemId);
}

void BusinessPromoteWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BusinessPromoteWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 调整子控件宽度
    double dWidth = Constants::WIDTH_PERCENT_IN_ACTIVITY_WIDGET * width();
    ui->tableWidgetRankList->setFixedWidth((int)dWidth);
    ui->textEditDescription->setFixedWidth((int)dWidth);
    ui->labelImage->setFixedWidth((int)dWidth);

//    // 调整排行榜文字显示对齐
//    double dLeftRightMargin = (1 - Constants::WIDTH_PERCENT_IN_ACTIVITY_WIDGET)/2 * width();
//    ui->horizontalLayoutLabRank->setContentsMargins((int)(dLeftRightMargin), 0, 0, 0);
}

/************************************
* 名称: setDefaultStyle
* 功能: 设置样式
* 返回:  void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void BusinessPromoteWidget::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/ubxactionlib.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

/************************************
* 名称: setupWidget
* 功能: 构建界面
* 返回:   void
* 时间:   2016/08/08
* 作者:   hwx
************************************/
void BusinessPromoteWidget::setupWidget()
{
    m_pTableDelegate = new UserRankItemDelegate(ui->tableWidgetRankList);
    connect(m_pTableDelegate, &UserRankItemDelegate::sigOnActionDetailedClicked, this, &BusinessPromoteWidget::slotOnDetailedActionPreviewClicked);
    ui->tableWidgetRankList->setItemDelegate(m_pTableDelegate);
    ui->tableWidgetRankList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetRankList->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidgetRankList->setFrameShape(QFrame::NoFrame);
    ui->tableWidgetRankList->setShowGrid(false);
    ui->tableWidgetRankList->verticalHeader()->hide();
    ui->tableWidgetRankList->horizontalHeader()->hide();
    ui->tableWidgetRankList->setMouseTracking(true);

//    m_pPictureFlow = new PictureFlow(this);
//    //m_pPictureFlow->setBackgroundColor(QColor("#404040"));
//    m_pPictureFlow->setFixedHeight(Constants::WINDOW_HEIGHT);
//    m_pPictureFlow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    m_pPictureFlow->setSlideSize(QSize(Constants::COVER_FLOW_WIDGET_WIDTH, Constants::COVER_FLOW_WIDGET_HEIGHT));
//    ui->horizontalLayoutImage->addWidget(m_pPictureFlow);

    ui->textEditDescription->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->textEditDescription->setReadOnly(true);

    connect(ui->pbGoBack, &QPushButton::clicked, this, &BusinessPromoteWidget::sigOnGoBackClicked);
    connect(ui->pbJoinIn, &QPushButton::clicked, this, &BusinessPromoteWidget::slotOnJoinInClicked);
}

/************************************
* 名称: initWidget
* 功能: 初始化界面
* 返回:   void
* 时间:   2016/08/08
* 作者:   hwx
************************************/
void BusinessPromoteWidget::initWidget()
{
    ui->labelImage->setScaledContents(true);
    ui->tableWidgetRankList->setColumnCount(1);
    ui->textEditDescription->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
}
