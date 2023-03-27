/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ActionLibWidget.cpp
* 创建时间：2016/03/03 18:20
* 文件标识：
* 文件摘要：动作库界面
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/03/03 18:20
* 版本摘要：
*/
#include "actionlibwidget.h"
#include "ui_actionlibwidget.h"
#include "QtHttpOperSync.h"
#include "ActUpdateMangr.h"
#include <QDir>
#include <QFileInfo>
#include "ubxactionlibconstants.h"
#include <QPainter>
#include "../Common/UBXPublic.h"
#include "loginlibmgr.h"
#include "alpharobotlog.h"
#include "cactionlibdatapool.h"
#include "../zxLib/widget/messagebox.h"
#include "../zxLib/ciniconfig.h"

ActionLibWidget::ActionLibWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActionLibWidget)
{
    ui->setupUi(this);

    AlphaRobotLog::getInstance()->printInfo("action lib constructor");

    m_pLatestPreview = NULL;
    m_pHotestPreview = NULL;
    m_pCommentsPreview = NULL;
    m_pCoverFlow = NULL;
    m_pChangedFlowImageTimer = NULL;
    m_nNewHotestPreviewTimeID = 0;
    m_nNewCommentsPreviewTimeID = 0;
    m_nUpdateHotestPreviewTimeID = 0;
    m_nUpdateCommentsPreviewTimeID = 0;
    m_nTime = 1000;
    m_bHasClearDefaultRecommendImage = false;

    //以下为示例代码，实际使用时删除
    //创建首页CoverFLow展示界面
    m_pCoverFlow = new PictureFlow(this);
    //m_pCoverFlow->setReflectionEffect(m_pCoverFlow->BlurredReflection);
    // 自适应屏幕分辨率
    m_pCoverFlow->setFixedHeight(Constants::WINDOW_HEIGHT);
    m_pCoverFlow->setSlideSize(QSize(Constants::COVER_FLOW_WIDGET_WIDTH, Constants::COVER_FLOW_WIDGET_HEIGHT));
    m_pCoverFlow->setBackgroundColor(QColor("#404040"));
    m_pCoverFlow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->coverflowVL->addWidget(m_pCoverFlow);
    connect(m_pCoverFlow, &PictureFlow::sigCenterImageClicked, this, &ActionLibWidget::slotOnCenterImageClicked);

    setDefaultStyle();

    // 默认显示本地图片，如果能从服务器上获取到数据，则会清除默认本地图片
   setDefaultCoverFlowImage();

    InitPreviewItemList();
}

void ActionLibWidget::OnDownLoadClicked(const QString &itemPath)
{
    CMessageBox::information(this, "DownLoad", itemPath);
}

//If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
void ActionLibWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

/************************************
* 名称: showEvent
* 功能: 显示事件
* 返回:   void
* 时间:   2016/06/23
* 作者:   hwx
************************************/
void ActionLibWidget::showEvent(QShowEvent *event)
{
    startChangedFlowImageTimer();
    QWidget::showEvent(event);
}

/************************************
* 名称: hideEvent
* 功能: 隐藏事件
* 返回:   void
* 时间:   2016/06/23
* 作者:   hwx
************************************/
void ActionLibWidget::hideEvent(QHideEvent *event)
{
    stopChangedFlowImageTimer();
    QWidget::hideEvent(event);
}

ActionLibWidget::~ActionLibWidget()
{
    stopChangedFlowImageTimer();
    for (int i = 0; i < m_lstRecommendItemData.size(); i++)
    {
        SAFE_DELETE(m_lstRecommendItemData[i]);
    }
    m_lstRecommendItemData.clear();
    SAFE_DELETE(m_pChangedFlowImageTimer);

    delete ui;
}

/************************************
* 名称: setDefaultStyle
* 功能: 设置窗体默认样式
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void ActionLibWidget::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/ubxactionlib.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

void ActionLibWidget::timerEvent(QTimerEvent *event)
{
    int nTimeID = event->timerId();
    if(nTimeID == m_nNewHotestPreviewTimeID)
    {
        killTimer(m_nNewHotestPreviewTimeID);
        m_nNewHotestPreviewTimeID = 0;
        m_pHotestPreview = new HomePageItemPreview(eActGroupOfficalHotest);
        connect(m_pHotestPreview, &HomePageItemPreview::SigMoreBtnClicked, this, &ActionLibWidget::SigMoreBtnClicked);
        connect(m_pHotestPreview, &HomePageItemPreview::SigActionPreviewClicked, this, &ActionLibWidget::SigActionPreviewClicked);
        m_pHotestPreview->SetTitle(tr("Hottest"));
        //m_pHotestPreview->setFixedWidth(this->width() * Constants::HOME_PREVIEW_WIDGET_RATIO);
        m_pHotestPreview->setFixedHeight(this->height() * Constants::HOME_PREVIEW_HEIGHT_RATIO);
        m_nNewCommentsPreviewTimeID = startTimer(m_nTime);
        ui->verticalLayoutHotest->insertWidget(Constants::eHomePageHotest, m_pHotestPreview);

    }
    else if(nTimeID == m_nNewCommentsPreviewTimeID)
    {
        killTimer(m_nNewCommentsPreviewTimeID);
        m_nNewCommentsPreviewTimeID = 0;
        m_pCommentsPreview = new HomePageItemPreview(eActGroupOfficalMostComment);
        connect(m_pCommentsPreview, &HomePageItemPreview::SigMoreBtnClicked, this, &ActionLibWidget::SigMoreBtnClicked);
        connect(m_pCommentsPreview, &HomePageItemPreview::SigActionPreviewClicked, this, &ActionLibWidget::SigActionPreviewClicked);
        m_pCommentsPreview->SetTitle(tr("Comments"));
        m_pCommentsPreview->setFixedHeight(this->height() * Constants::HOME_PREVIEW_HEIGHT_RATIO);
        ui->verticalLayoutComments->insertWidget(Constants::eHomePageComment, m_pCommentsPreview);
    }
    else if(nTimeID == m_nUpdateHotestPreviewTimeID)
    {
        m_nUpdateCommentsPreviewTimeID = startTimer(m_nTime);
        killTimer(m_nUpdateHotestPreviewTimeID);
        m_nUpdateHotestPreviewTimeID = 0;
        if(m_pHotestPreview)
        {
            m_pHotestPreview->UpdateActionListItems(m_pHotestPreview->getCurrentActionType());
        }
    }
    else if(nTimeID == m_nUpdateCommentsPreviewTimeID)
    {
        killTimer(m_nUpdateCommentsPreviewTimeID);
        m_nUpdateCommentsPreviewTimeID = 0;
        if(m_pCommentsPreview)
        {
            m_pCommentsPreview->UpdateActionListItems(m_pCommentsPreview->getCurrentActionType());
        }
    }

}

/************************************
* 名称: slotOnAddRecommendAction
* 功能: 添加推荐主题
* 参数：pRecommendItem，推荐动作数据
* 返回:   void
* 时间:   2016/04/11
* 作者:   hwx
************************************/
void ActionLibWidget::slotOnAddRecommendAction(RecommendActItemData *pRecommendItem)
{
    if(NULL == m_pCoverFlow || NULL == pRecommendItem)
    {
        return;
    }

    // 根据recommendOrder字段排序，添加到list
    // recommendOrder越大，排在越靠前
    addRecommendData(pRecommendItem);

    // 由于图片下载完成的时间是随机的，不可控，导致banner每次启动顺序不同
    // 因此这里每次添加新的数据都要重新刷新banner页
    updateFlowCover();
}

/************************************
* 名称: slotOnAddBusinessPromoteData
* 功能: 添加运营推广活动数据
* 参数：pPromoteItem，运营推广活动数据
* 返回:   void
* 时间:   2016/08/17
* 作者:   hwx
************************************/
void ActionLibWidget::slotOnAddBusinessPromoteData(BusinessPromoteData *pPromoteItem)
{
    if(NULL == m_pCoverFlow || NULL == pPromoteItem)
    {
        return;
    }

    if (CActionLibDataPool::GetInstance()->isBusinessPromoteItemExist(pPromoteItem))
    {
        return;
    }

    CActionLibDataPool::GetInstance()->addBusinessPromoteData(pPromoteItem);

    QString strImageFile = pPromoteItem->getCoverImageUrl();
    if (strImageFile.isEmpty() || !QFile::exists(strImageFile))
    {
        // 使用默认图片
#ifdef Q_OS_MAC
    strImageFile = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/../res/display/display1.png");
#else
    strImageFile =  QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/res/display/display1.png");
#endif
    }


// 不需要添加到banner，banner页已经有对应图片，只需要关联就可以了
#if 0
    QMutexLocker locker(&m_Mutex);

    // 如果第一次从服务器收取到数据，则清除原来默认显示的图片数据
    // 从而实现服务器上没有图片时，显示默认本地图片，否则显示服务器
    // 图片的效果
    if (!m_bHasClearDefaultRecommendImage)
    {
        m_pCoverFlow->clear();
        m_bHasClearDefaultRecommendImage = true;
    }

    QImage imgCover;
    if(!imgCover.load(strImageFile))
    {
        AlphaRobotLog::getInstance()->printError(QString("Failed to load image file %1").arg(strImageFile));
        return;
    }

    int nCount = m_pCoverFlow->slideCount();

    AlphaRobotLog::getInstance()->printDebug(QString("Add slide %1").arg(nCount));

    m_pCoverFlow->addSlide(imgCover);
    m_pCoverFlow->setCenterIndex(m_pCoverFlow->slideCount() / 2);
//    m_pCoverFlow->setBackgroundColor(QColor("#404040"));
#endif
    emit sigOnUpdatePopUpImage(strImageFile);
}

/************************************
* 名称: slotOnCurrentFlowPictureChanged
* 功能: 当前推荐动作图片选中的槽函数
* 参数：int：当前选中的图片
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionLibWidget::slotOnCenterImageClicked()
{
    int nIndex = m_pCoverFlow->getLatestCenterPictureIndex();
    slotOnFlowCoverClicked(nIndex);
}

/************************************
* 名称: slotOnFlowCoverClicked
* 功能: 点击flow流图指定Id页
* 参数：[in]：nFlowCoverId 当前点击的图片id
* 返回:   void
* 时间:   2016/09/01
* 作者:   hwx
************************************/
void ActionLibWidget::slotOnFlowCoverClicked(int nFlowCoverId)
{
    if (m_lstRecommendItemData.size() <= nFlowCoverId)
    {
        AlphaRobotLog::getInstance()->printError(QString("Flow image index %1 doesn't exist").arg(nFlowCoverId));
        return;
    }

    RecommendActItemData *pRecommendItem = m_lstRecommendItemData.at(nFlowCoverId);
    if (NULL == pRecommendItem)
    {
        AlphaRobotLog::getInstance()->printError(QString("NULL pointer when get recommend item"));
        return;
    }

    // 如果推送页
    if (pRecommendItem->m_nRecommendForwardType == Constants::ePageTypePush)
    {
        slotOnClickPopUpPage();
    }
    else  // 普通banner页
    {
        emit sigRecommendImageClicked(pRecommendItem);
    }
}

/************************************
* 名称: slotOnClickPopUpPage
* 功能: 点击运营活动退送页的槽函数
* 参数：
* 返回:   void
* 时间:   2016/03/01
* 作者:   hwx
************************************/
void ActionLibWidget::slotOnClickPopUpPage()
{
    ///////////////////////////////////////////////////////
    /// 由于目前后台的banner页和推送页没有绑定关系
    /// 目前默认弹出的都是第一个推送
    ///// //////////////////////////////////////////////////
   CActionLibDataPool *pDataPool = CActionLibDataPool::GetInstance();
   QList<BusinessPromoteData *> lstData = pDataPool->getAllBusinessPromoteData();
    if (lstData.size() <= 0)
    {
        return;
    }

   emit sigBusinessPromoteImageClicked(lstData.at(0));
}

/************************************
* 名称: slotOnChangedFlowCoverImageTimerTimeOut
* 功能: flowcover定时器槽函数
* 返回:   void
* 时间:   2016/06/21
* 作者:   hwx
************************************/
void ActionLibWidget::slotOnChangedFlowCoverImageTimerTimeOut()
{
    QMutexLocker locker(&m_Mutex);

    int nTotalImage = m_pCoverFlow->slideCount();
    if (nTotalImage <= 0)
    {
        return;
    }

    emPictureFlowDirection eFlowDir = m_pCoverFlow->getPictureFlowDirection();
    if (eFlowDir == eDirectionLeftToRight)
    {
        m_pCoverFlow->showPrevious();
    }
    else if (eFlowDir == eDirectionRightToLeft)
    {
        m_pCoverFlow->showNext();
    }

//    int nCenterImage = m_pCoverFlow->centerIndex();
//    if (nTotalImage - 1 > nCenterImage)
//    {
//        m_pCoverFlow->showNext();
//    }
//    else if (nTotalImage - 1 <= nCenterImage)
//    {
//        m_pCoverFlow->showSlide(0);
//    }
}

/************************************
* 名称: setDefaultCoverFlowImage
* 功能: 设置默认flow图片
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionLibWidget::setDefaultCoverFlowImage()
{
#ifdef Q_OS_MAC
    QString sTempDir = "/../res/display";
#else
    QString sTempDir = "/res/display";
#endif

    QString strPath = QDir::toNativeSeparators(QApplication::applicationDirPath() + sTempDir);
    QStringList pictures = FindPictures(strPath);
    QImage img;
    foreach (QString file, pictures)
    {
        if(img.load(file))
        {
            m_pCoverFlow->addSlide(img);
        }
    }

    m_pCoverFlow->setCenterIndex(m_pCoverFlow->slideCount() / 2);
    //m_pCoverFlow->setBackgroundColor(QColor("#404040"));
}

void ActionLibWidget::UpdatePreviewItemList()
{
    if(m_pLatestPreview)
    {
        m_pLatestPreview->UpdateActionListItems(m_pLatestPreview->getCurrentActionType());
        m_nUpdateHotestPreviewTimeID = startTimer(m_nTime);
    }
}

void ActionLibWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 自适应屏幕分辨率
    if (m_pCoverFlow)
    {
        m_pCoverFlow->setFixedHeight(Constants::FLOW_WIDGET_HEIGHT_RATIO * this->height());
        m_pCoverFlow->setSlideSize(QSize(m_pCoverFlow->width() * Constants::FLOW_IMAGE_WIDTH_RATIO, Constants::FLOW_IMAGE_HEIGHT_RATIO * m_pCoverFlow->height()));
    }

    int nTableWidgetHeight = this->height() * Constants::HOME_PREVIEW_HEIGHT_RATIO;
    if (nTableWidgetHeight > Constants::HOME_PREVIEW_MAX_HEIGHT)
    {
        nTableWidgetHeight = Constants::HOME_PREVIEW_MAX_HEIGHT;
    }

    if (m_pLatestPreview)
    {
        m_pLatestPreview->setFixedHeight(nTableWidgetHeight);
    }
    if (m_pHotestPreview)
    {
        m_pHotestPreview->setFixedHeight(nTableWidgetHeight);
    }
    if (m_pCommentsPreview)
    {
        m_pCommentsPreview->setFixedHeight(nTableWidgetHeight);
    }
}

/************************************
* 名称: startChangedFlowImageTimer
* 功能: 启动flowcover中切换图片的定时器
* 返回:   void
* 时间:   2016/06/21
* 作者:   hwx
************************************/
void ActionLibWidget::startChangedFlowImageTimer()
{
    if (NULL == m_pChangedFlowImageTimer)
    {
        m_pChangedFlowImageTimer = new QTimer();
        m_pChangedFlowImageTimer->setInterval(Constants::TIME_INTERVAL_CHANGED_FLOW_COVER_IMAGE);
        connect(m_pChangedFlowImageTimer, &QTimer::timeout, this, &ActionLibWidget::slotOnChangedFlowCoverImageTimerTimeOut);
    }

    if (!m_pChangedFlowImageTimer->isActive())
    {
        m_pChangedFlowImageTimer->start();
    }
}

/************************************
* 名称: stopChangedFlowImageTimer
* 功能: 停止flowcover中切换图片的定时器
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void ActionLibWidget::stopChangedFlowImageTimer()
{
    if (m_pChangedFlowImageTimer)
    {
        if (m_pChangedFlowImageTimer->isActive())
        {
            m_pChangedFlowImageTimer->stop();
        }
    }
}

/************************************
* 名称: requestRecommendActionList
* 功能: 请求推荐动作数据
* 返回:   void
* 时间:   2016/08/30
* 作者:   hwx
************************************/
void ActionLibWidget::requestRecommendActionList()
{
    if (g_actupdatemgr)
    {
          g_actupdatemgr->QueryRecommendList(RECOMMEND_TYPE, m_ZoneCode.toInt(), Constants::HTTP_DEFAULT_REQUEST_ID, getRecommendActionRequestLanguage());
    }
}

/************************************
* 名称: requestBusinessPromoteList
* 功能: 请求运营活动推广数据
* 返回:   void
* 时间:   2016/08/30
* 作者:   hwx
************************************/
void ActionLibWidget::requestBusinessPromoteList()
{
    if (g_actupdatemgr)
    {
        g_actupdatemgr->queryBusinessPromoteData(Constants::HTTP_DEFAULT_REQUEST_ID);
    }
}

/************************************
* 名称: InitPreviewItemList
* 功能: 初始化预览动作项列表
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void ActionLibWidget::InitPreviewItemList()
{
    //由于初始化动作项列表是根据最新、最热、最多评论从后台获取的，获取到的动作项有重复，这里改成串行处理，
    //即完全获取到一个列表之后，再请求下一个列表，使用定时器完成，不然数据出现重复存取会有错误
    m_pLatestPreview = new HomePageItemPreview(eActGroupOfficalLastUpdate);
    connect(m_pLatestPreview, &HomePageItemPreview::SigMoreBtnClicked, this, &ActionLibWidget::SigMoreBtnClicked);
    connect(m_pLatestPreview, &HomePageItemPreview::SigActionPreviewClicked, this, &ActionLibWidget::SigActionPreviewClicked);
    m_pLatestPreview->SetTitle(tr("Latest"));
    m_pLatestPreview->setFixedHeight(this->height() * Constants::HOME_PREVIEW_HEIGHT_RATIO);
    ui->verticalLayoutLatest->insertWidget(Constants::eHomePageLatest, m_pLatestPreview);
    m_nNewHotestPreviewTimeID = startTimer(m_nTime);
}

/************************************
* 名称: getRecommendActionRequestLanguage
* 功能: 获取后台请求数据语言类型，CN或EN
* 返回:   void
* 时间:   2016/04/11
* 作者:   hwx
************************************/
QString ActionLibWidget::getRecommendActionRequestLanguage()
{
    QString strLanguageKeyword = CIniConfig::getInstance().getValueLanguage();
    QString strLanguage;
    if (strLanguageKeyword.compare(LANGUAGE_CHINESE_POSTFIX) == 0)
    {
        strLanguage = "CN";
    }
    else if (strLanguageKeyword.compare(LANGUAGE_ENGLISH_POSTFIX) == 0)
    {
        strLanguage = "EN";
    }
    else
    {
        strLanguage = "EN";//其他国家使用英文动作库
    }

    return strLanguage;
}

/************************************
* 名称: addRecommendData
* 功能: 添加主题数据，自动根据order排序
* 返回:   void
* 时间:   2016/04/11
* 作者:   hwx
************************************/
void ActionLibWidget::addRecommendData(RecommendActItemData *pData)
{
    if (!pData)
    {
        return;
    }

    // 降序排列，优先级高排在List前面
    int nIndex = -1;
    for (int i = 0; i < m_lstRecommendItemData.size(); i++)
    {
        RecommendActItemData *pCompareData = m_lstRecommendItemData[i];
        if (!pCompareData)
        {
            continue;
        }

        // 如果已经存在，则不要再次添加了
        if (pData == pCompareData)
        {
            return;
        }

        if (pData->m_nRecomendOrder > pCompareData->m_nRecomendOrder)
        {
            nIndex = i;
            break;
        }
    }

    // 如果优先级最低，则放到最后
    if (nIndex >= 0)
    {
        m_lstRecommendItemData.insert(nIndex, pData);
    }
    else
    {
        m_lstRecommendItemData.push_back(pData);
    }
}

/************************************
* 名称: updateFlowCover
* 功能: 根据新的顺序重新刷新banner页
* 返回:   void
* 时间:   2016/04/11
* 作者:   hwx
************************************/
void ActionLibWidget::updateFlowCover()
{
    if (!hasBannerImagesAllDownloaded())
    {
        return;
    }

    QMutexLocker locker(&m_Mutex);

    // 清空原来的banner页
    m_pCoverFlow->clear();

    // 如果第一次从服务器收取到数据，则清除原来默认显示的图片数据
    // 从而实现服务器上没有图片时，显示默认本地图片，否则显示服务器
    // 图片的效果
    if (!m_bHasClearDefaultRecommendImage)
    {
        m_pCoverFlow->clear();
        m_bHasClearDefaultRecommendImage = true;
    }

    for (int i = 0; i < m_lstRecommendItemData.size(); i++)
    {
        RecommendActItemData *pData = m_lstRecommendItemData.at(i);
        if (!pData)
        {
            continue;
        }

        QString strFile = pData->m_strRecommendImage;
        if (!QFile::exists(strFile))
        {
            continue;
        }

        QImage imgAction;
        if(!imgAction.load(strFile))
        {
            AlphaRobotLog::getInstance()->printError(QString("Failed to load image file %1").arg(strFile));
            return;
        }

        int nCount = m_pCoverFlow->slideCount();
        AlphaRobotLog::getInstance()->printDebug(QString("Add slide %1").arg(nCount));

        m_pCoverFlow->addSlide(imgAction);
    }

    m_pCoverFlow->setCenterIndex(m_pCoverFlow->slideCount() / 2);
}

bool ActionLibWidget::hasBannerImagesAllDownloaded()
{
    for (int i = 0; i < m_lstRecommendItemData.size(); i++)
    {
        RecommendActItemData *pData = m_lstRecommendItemData.at(i);
        if (!pData)
        {
            continue;
        }

        QString strFile = pData->m_strRecommendImage;
        if (!QFile::exists(strFile))
        {
            return false;
        }
    }

    return true;
}

QStringList ActionLibWidget::FindPictures(const QString &path)
{
    if(path.isEmpty())
        return QStringList();

    QStringList pictures;
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    foreach (QFileInfo fileInfo, list)
    {
        pictures.append(dir.absoluteFilePath(fileInfo.fileName()));
    }
    return pictures;
}
