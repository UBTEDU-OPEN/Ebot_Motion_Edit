#include "actiondetailwidget.h"
#include "ui_actiondetailwidget.h"
#include "commentitemdelegate.h"
#include "ubxactionlibconstants.h"
#include "UBXPublic.h"
#include <QPainter>

#include "../UBXLogin/loginlibmgr.h"
#include "datamodel/usercommentitem.h"
#include "configs.h"


RETRANSLATEUIFUNCIMPL(ActionDetailWidget)

ActionDetailWidget::ActionDetailWidget(QWidget *parent) :
    UBXWidget(parent),
    m_pActionCommentWidget(NULL),
    m_pActionYouMayLikeWidget(NULL),
    m_pVideoPlayer(NULL),
    m_bCurIsCollected(false),
    ui(new Ui::ActionDetailWidget)
{
    ui->setupUi(this);

    Init();
    setDefaultStyle();

    OnIsCollectResult(-1,  -1, -1);
}

ActionDetailWidget::~ActionDetailWidget()
{
    delete ui;
}

/************************************
* 名称: requestForYouMayLikeAction
* 功能: 请求猜你喜欢的动作
* 参数：[in] eGroup：动作组类，如最热的
* 参数：[in] eType：动作类型，如舞蹈
* 返回:   void
* 时间:   2016/06/19
* 作者:   hwx
************************************/
void ActionDetailWidget::requestForYouMayLikeAction(emActionGroup eGroup, emActionType eType)
{
    if (m_pActionYouMayLikeWidget)
    {
        m_pActionYouMayLikeWidget->requestForYouMayLikeAction(eGroup, eType);
    }
}

/************************************
* 名称: requestForDescriptionImage
* 功能: 请求动作描述图片
* 参数：[in] itemData: 动作数据
* 返回:   void
* 时间:   2016/11/18
* 作者:   hwx
************************************/
void ActionDetailWidget::requestForDescriptionImage(ActItemData &itemData)
{
    // 如果视频文件URL有效，则使用视频播放器播放，否则显示预览图片
    if (isVideoURLValid(itemData.m_sVideoURL))
    {
        return;
    }
    
    QString strDescriptionImageUrl = itemData.m_sImageURL;
    if (strDescriptionImageUrl.isEmpty())
    {
        return;
    }
    m_nDescImageRequestId = g_actupdatemgr->queryDownloadImage(strDescriptionImageUrl, CConfigs::getLocalPreviewImgDir(), Constants::HTTP_DEFAULT_REQUEST_ID);
}

/************************************
* 名称: SetItemData
* 功能: 设置显示Item详情
* 参数: [in]itemData 动作项数据
* 返回:   void
* 时间:   2016/04/15
* 作者:   hels
************************************/
void ActionDetailWidget::SetItemData(const ActItemData &itemData)
{
    m_CurItemData = itemData;

    QString strDefaultImageFile = CConfigs::getLocalResPath(UBX_DEFAULT_IMAGE);//如果加载图片不成功则加载默认图片

    QFileInfo fileInfo(itemData.m_sImageHeadUrl);
    if(!fileInfo.fileName().isEmpty())//如果预览图片不存在则加载默认图片
    {
        QPixmap pixmap(itemData.m_sImageHeadUrl);
        QPixmap fitpixmap;
#ifdef Q_OS_OSX
        if(itemData.m_sImageHeadUrl.indexOf("@2x")>0)//苹果系统下，有没有@2x文件名的图片实际大小不一样，这里需要特殊处理
        {
            fitpixmap = pixmap.scaled(ui->labelActionImage->width()*2,ui->labelActionImage->height()*2);
        }
        else
        {
            fitpixmap = pixmap.scaled(ui->labelActionImage->width(),ui->labelActionImage->height());
        }
#else
        fitpixmap = pixmap.scaled(ui->labelActionImage->width(),ui->labelActionImage->height());
#endif

        ui->labelActionImage->setPixmap(fitpixmap);
    }
    else
    {
        QPixmap pixmap(strDefaultImageFile);
        QPixmap fitpixmap = pixmap.scaled(ui->labelActionImage->width(),ui->labelActionImage->height());
        ui->labelActionImage->setPixmap(fitpixmap);
    }

    ui->labelActionTitle->setText(itemData.m_sActName);
    //描述内容如果太长，需要截断显示
    QFontMetrics metrics(ui->labelActionDes->font());
    int nLen = metrics.width(itemData.m_sActDesc) + TEXT_MORE_SPACE;
    if (nLen > MAX_ACT_DESC_LEN)
    {
        nLen = MAX_ACT_DESC_LEN;
    }
    ui->labelActionDes->setText(metrics.elidedText(itemData.m_sActDesc, Qt::ElideRight, nLen));

    ui->labelActionDate->setText(itemData.m_sPublishTime);

    // not show office user name
    ui->labelActionByWho->setText("");
    if(eActionResourceOfficial != itemData.m_nActResource)
    {
        ui->labelActionByWho->setText(itemData.m_sUserName);
    }

    ui->labelActionSize->setText(GetFormatSizeString(itemData.m_nActSize));
    ui->pbFavorNum->setText(QString::number(itemData.m_nActionCollectTime));
    ui->pbDownloadNum->setText(QString::number(itemData.m_nActionDownloads));
    ui->pbCommentNum->setText(QString::number(itemData.m_nActionComments));
    ui->labelActAllDes->setText(itemData.m_sActDesc);
    if(g_actupdatemgr && -1 != g_actupdatemgr->GetUserID())
    {
         g_actupdatemgr->QueryActFavour(g_actupdatemgr->GetUserID(), itemData.m_nItemID);
    }
    else
    {
//        ui->pbFavInDetail->hide();
//        ui->pbDownloadInDetail->hide();
    }

    if (m_pActionYouMayLikeWidget)
    {
        m_pActionYouMayLikeWidget->setCurrentItemData(itemData);
    }

    if (isVideoURLValid(itemData.m_sVideoURL))
    {
        setVideoContent();
    }
    else
    {
        updateActionDescImage();
    }
}

/************************************
* 名称: setDefaultStyle
* 功能: 设置窗体默认样式
* 返回:   void
* 时间:   2016/04/16
* 作者:   hels
************************************/
void ActionDetailWidget::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/ubxactionlib.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

/************************************
* 名称: SetUserLogIn
* 功能: 设置用户登录状态
* 参数: [in]bLogin 用户是否登录
* 返回:   void
* 时间:   2016/04/16
* 作者:   hels
************************************/
void ActionDetailWidget::SetUserLogIn(bool bLogin)
{
    if(g_actupdatemgr)
        g_actupdatemgr->QueryActFavour(g_actupdatemgr->GetUserID(), m_CurItemData.m_nItemID);
}

/************************************
* 名称: OnIsCollectResult
* 功能: 响应查询是否收藏结果
* 参数: [in]requestID 请求ID
* 参数: [in]nActionID 收藏总数
* 参数: [in]nRet 请求结果
* 返回:   void
* 时间:   2016/04/16
* 作者:   hels
************************************/
void ActionDetailWidget::OnIsCollectResult(int requestID, int nActionID, int nRet)
{
    // 注意：未登录时，由于在hover后就会请求并返回数据，此时并未设置了m_CurItemData，导致
    // 按钮状态未刷新，因此需要在初始化中设置按钮qss解决该问题
    if (nActionID == m_CurItemData.m_nItemID)
    {
        if(1 == nRet)
        {
            ui->pbFavInDetail->setStyleSheet("QPushButton{background: transparent;"
                                             "border-image: url(:/res/images/btn_collection_press.png);"
                                             "width:18px;"
                                             "height:17px;}");
            m_bCurIsCollected = true;
        }
        else
        {
            ui->pbFavInDetail->setStyleSheet("QPushButton{background: transparent;"
                                             "border-image: url(:/res/images/btn_collection.png);"
                                             "width:18px;"
                                             "height:17px;}");
            m_bCurIsCollected = false;
        }

        QMap<int, ActItemData> mapItems;
        g_actupdatemgr->GetDownloadedActList(mapItems);
        if(mapItems.contains(m_CurItemData.m_nItemID))
        {
            ui->pbDownloadInDetail->setStyleSheet("QPushButton{background: transparent;"
                                                  "border-image: url(:/res/images/btn_download_press.png);"
                                                  "width:20px;"
                                                  "height:14px;}");
        }
        else
        {
            ui->pbDownloadInDetail->setStyleSheet("QPushButton{background: transparent;"
                                                  "border-image: url(:/res/images/btn_download.png);"
                                                  "width:18px;"
                                                  "height:18px;}");
        }

        ui->pbFavInDetail->setEnabled(true);
        ui->pbFavInDetail->show();
        ui->pbDownloadInDetail->show();
    }
}

/************************************
* 名称: slotOnYouMayLikeWidgetMoreClicked
* 功能: 动作详情页猜你喜欢控件槽函数
* 返回:   void
* 时间:   2016/08/22
* 作者:   hwx
************************************/
void ActionDetailWidget::slotOnYouMayLikeWidgetMoreClicked()
{
    emActionGroup eActType = eActGroupOfficalHotest;
    if (m_CurItemData.m_nActResource == eActionResourceIShow)
    {
        eActType = eActGroupAllHotest;
    }
    emit sigOnDetailedWidgetMoreClicked(eActType, (emActionType)m_CurItemData.m_nActType);
}

/************************************
* 名称: slotOnActionPreviewClicked
* 功能: 点击动作预览的槽函数
* 返回:   void
* 时间:   2016/08/22
* 作者:   hwx
************************************/
void ActionDetailWidget::slotOnActionPreviewClicked(ActItemData itemData)
{
    resetUI();

    // 针对用户点击动作预览、返回、预览下一个动作的场景，
    // 由于QMediaPlayer在短时间内如果反复setMedia()，会出现
    // 以下问题：
    // 1、最后一次setMedia()不一定有效，播放的可能是上一个视频，可能是最后一个视频
    // 2、视频加载后时长问题，多次setMedia()后durationChanged()信号可能都会触发，顺序和加载视频缓冲有关，导致最后的时长显示可能不是最后加载的视频文件
    // 3、多次进入导致反复loadMedia()时获取第一帧画面视频的时间不定，导致最后播放器显示的第一帧画面可能和视频对不上
    // 针对上述问题，重新构建视频播放器可以规避
    resetPlayer();

    SetItemData(itemData);

    this->update();

    requestForYouMayLikeAction(eActGroupAllHotest, (emActionType)itemData.m_nActType);

    if (m_pActionCommentWidget)
    {
        m_pActionCommentWidget->requestActionComment(itemData.m_nItemID, 1); // 请求第一页评论
    }
}

/************************************
* 名称: Init
* 功能: 初始化
* 返回:   void
* 时间:   2016/04/15
* 作者:   hels
************************************/
void ActionDetailWidget::Init()
{
    ui->labelActDesTitle->setText(tr("Action description:"));//该控件多语言文本没有出来，这里手动加了tr
    ui->labelActAllDes->setWordWrap(true);
    connect(ui->pbBack, &QPushButton::clicked, this, &ActionDetailWidget::OnBackBtnclicked);
    connect(ui->pbFavInDetail, &QPushButton::clicked, this, &ActionDetailWidget::OnCollection);
    connect(ui->pbDownloadInDetail, &QPushButton::clicked, this, &ActionDetailWidget::OnDownload);
    connect(g_actupdatemgr, &CActUpdateMangr::SigIsCollectResult, this, &ActionDetailWidget::OnIsCollectResult);

    m_pActionCommentWidget = new ActionCommentWidget(this);
    ui->horizontalLayoutAllComments->addWidget(m_pActionCommentWidget);
    connect(g_actupdatemgr, &CActUpdateMangr::sigOnHttpRequestFinished, m_pActionCommentWidget, &ActionCommentWidget::slotOnAddCommentFinished);
    connect(g_actupdatemgr, &CActUpdateMangr::sigOnReceivedActionComment, m_pActionCommentWidget, &ActionCommentWidget::slotOnReceivedCommentData);
    connect(g_actupdatemgr, &CActUpdateMangr::sigOnDownloadCommentReplierIconDone, m_pActionCommentWidget, &ActionCommentWidget::slotOnDownloadReplierIconDone);
    connect(m_pActionCommentWidget, &ActionCommentWidget::sigOnReplyClicked, this, &ActionDetailWidget::slotOnReplyClicked);

    double dCommentWidgetHeight = Constants::COMMENT_WIDGET_HEIGHT_RATIO * this->height();
    m_pActionCommentWidget->setFixedHeight((int)(dCommentWidgetHeight));

    m_pActionYouMayLikeWidget = new ActionYouMayLikeWidget(this);
    m_pActionYouMayLikeWidget->setFixedWidth(this->width() * Constants::ACTION_YOU_MAY_LIKE_WIDTH_WIDTH_RATIO);
    m_pActionYouMayLikeWidget->setObjectName("m_pActionYouMayLikeWidget");
    ui->verticalLayoutYouMayLike->addWidget(m_pActionYouMayLikeWidget);
    connect(g_actupdatemgr, &CActUpdateMangr::SigItemDate, m_pActionYouMayLikeWidget, &ActionYouMayLikeWidget::slotOnItemDate);
    connect(m_pActionYouMayLikeWidget, &ActionYouMayLikeWidget::sigOnMoreClicked, this, &ActionDetailWidget::slotOnYouMayLikeWidgetMoreClicked);
    connect(m_pActionYouMayLikeWidget, &ActionYouMayLikeWidget::sigOnActionPreviewClicked, this, &ActionDetailWidget::slotOnActionPreviewClicked);

//    m_pVideoPlayer = new UBXVideoPlayer(this);
//    double dPlayerWidgetHeight = Constants::PLAYER_WIDGET_HEIGHT_RATIO * this->height();
//    m_pVideoPlayer->setFixedHeight((int)(dPlayerWidgetHeight));
//    ui->horizontalLayoutVideoPlayer->addWidget(m_pVideoPlayer);

    connect(g_actupdatemgr, &CActUpdateMangr::sigOndownLoadImageDone, this, &ActionDetailWidget::slotOndownloadDescImageDone);

    ui->scrollAreaActionPreview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // 默认为未收藏
    ui->pbFavInDetail->setStyleSheet("QPushButton{background: transparent;"
                                     "border-image: url(:/res/images/btn_collection.png);"
                                     "width:18px;"
                                     "height:17px;}");

    // 默认为未下载
    ui->pbDownloadInDetail->setStyleSheet("QPushButton{background: transparent;"
                                          "border-image: url(:/res/images/btn_download.png);"
                                          "width:18px;"
                                          "height:18px;}");
}

/************************************
* 名称: paintEvent
* 功能: 摘抄自Qt帮助文档：If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below
* 返回:   void
* 时间:   2016/04/16
* 作者:   hels
************************************/
void ActionDetailWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void ActionDetailWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_pActionYouMayLikeWidget->setFixedWidth(this->width() * Constants::ACTION_YOU_MAY_LIKE_WIDTH_WIDTH_RATIO);
}

/************************************
* 名称: OnBackBtnclicked
* 功能: 返回按钮槽函数
* 返回:   void
* 时间:   2016/04/16
* 作者:   hels
************************************/
void ActionDetailWidget::OnBackBtnclicked()
{
    //将来支持多次进入，需要添加返回链表，一层层返回
    emit SigGoBack();
}

/************************************
* 名称: OnDownload
* 功能: 响应下载操作
* 返回:   void
* 时间:   2016/04/16
* 作者:   hels
************************************/
void ActionDetailWidget::OnDownload()
{
    if(-1 == g_actupdatemgr->GetUserID())//未登录时不能下载
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);

        return;
    }

    g_actupdatemgr->DownloadAct(m_CurItemData);
    if(ui->pbDownloadInDetail)
    {
        ui->pbDownloadInDetail->setStyleSheet("QPushButton{background: transparent;"
                                              "border-image: url(:/res/images/btn_download_press.png);"
                                              "width:20px;"
                                              "height:14px;}");
    }
}

/************************************
* 名称: OnCollection
* 功能: 响应收藏操作
* 返回:   void
* 时间:   2016/04/16
* 作者:   hels
************************************/
void ActionDetailWidget::OnCollection()
{
    if(!g_userinfo.m_bLogin)
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);

        return;
    }

    if(m_bCurIsCollected)
    {
         g_actupdatemgr->UnfavourAction(g_actupdatemgr->GetUserID(), m_CurItemData.m_nItemID);
    }
    else
    {
         g_actupdatemgr->FavourAction(g_actupdatemgr->GetUserID(), m_CurItemData.m_nItemID);
    }

    ui->pbFavInDetail->setEnabled(false);
}

/************************************
* 名称: slotOndownloadDescImageDone
* 功能: 完成动作详情页图片下载
* 参数：[in] nRequestId 请求ID
* 返回:   void
* 时间:   2016/11/17
* 作者:   hwx
************************************/
void ActionDetailWidget::slotOndownloadDescImageDone(int nRequestId)
{
    if (m_nDescImageRequestId == nRequestId)
    {
        updateActionDescImage();
        m_nDescImageRequestId = Constants::HTTP_DEFAULT_REQUEST_ID;
    }
}

/************************************
* 名称: slotOnReplyClicked
* 功能: 用户点击回复评论图标的槽函数，调整滚动条到评论区
* 参数：
* 返回:   void
* 时间:   2016/11/25
* 作者:   hwx
************************************/
void ActionDetailWidget::slotOnReplyClicked()
{
    ui->scrollAreaActionPreview->ensureVisible(ui->frameCommentContainer->x(), ui->frameCommentContainer->y());
}

/************************************
* 名称: setVideoContent
* 功能: 设置媒体播放器内容
* 返回:   void
* 时间:   2016/08/27
* 作者:   hwx
************************************/
void ActionDetailWidget::setVideoContent()
{
    if (!m_pVideoPlayer)
    {
        return;
    }

//    m_pVideoPlayer->resetVideoPlayer();

    QString strVideoUrl = m_CurItemData.m_sVideoURL;
    if (!isVideoURLValid(strVideoUrl))
    {
        return;
    }

    m_pVideoPlayer->loadVideoFile(strVideoUrl);
}

/************************************
* 名称: isVideoURLValid
* 功能: 判断视频URL是否有效
* 返回:   void
* 时间:   2016/11/18
* 作者:   hwx
************************************/
bool ActionDetailWidget::isVideoURLValid(const QString &strUrl)
{
    QString strVideoUrl = strUrl;
    QUrl urlVideo(strVideoUrl);
    if (!urlVideo.isValid())
    {
        AlphaRobotLog::getInstance()->printError(QString("Invalid Url: %1").arg(strVideoUrl));
        return false;
    }

    QFileInfo fileInfo(strVideoUrl);
    if (fileInfo.suffix().compare(VIDEO_SUPPORT_FORMAT_WITHOUT_DOT) != 0)
    {
        AlphaRobotLog::getInstance()->printError(QString("Invalid video format: %1. Video format only support mp4.").arg(strVideoUrl));
        return false;
    }

    return true;
}

/************************************
* 名称: updateActionDescImage
* 功能: 刷新动作描述图片显示
* 返回:   void
* 时间:   2016/11/18
* 作者:   hwx
************************************/
void ActionDetailWidget::updateActionDescImage()
{
    if (!m_pVideoPlayer)
    {
        return;
    }

    QString strDescImageUrl = m_CurItemData.m_sImageURL;
    QFileInfo fileInfo(strDescImageUrl);
    QString strDescImageFile = CConfigs::getFilePath(CConfigs::getLocalDetailedImageDir(), fileInfo.fileName());
    QFileInfo descImageFileInof(strDescImageFile);
    if(strDescImageUrl.isEmpty() || !descImageFileInof.exists())//如果预览图片不存在则加载默认图片
    {
        strDescImageFile = CConfigs::getLocalResPath(QString("display%1%2").arg(QDir::separator()).arg(UBX_DISPLAY_IMAGE_1));
    }

    m_pVideoPlayer->setImage(strDescImageFile);
}

/************************************
* 名称: resetUI
* 功能: 复位界面
* 返回:   void
* 时间:   2016/09/26
* 作者:   hwx
************************************/
void ActionDetailWidget::resetUI()
{
    if (m_pVideoPlayer)
    {
        m_pVideoPlayer->resetVideoPlayer();
    }

    if (m_pActionCommentWidget)
    {
        m_pActionCommentWidget->resetUI();
    }

    if (m_pActionYouMayLikeWidget)
    {
        m_pActionYouMayLikeWidget->resetUI();
    }
}

void ActionDetailWidget::resetPlayer()
{
    if (m_pVideoPlayer && ui->horizontalLayoutVideoPlayer->indexOf(m_pVideoPlayer) != -1)
    {
        ui->horizontalLayoutVideoPlayer->removeWidget(m_pVideoPlayer);
    }

    SAFE_DELETE(m_pVideoPlayer);

    m_pVideoPlayer = new UBXVideoPlayer(this);
    ui->horizontalLayoutVideoPlayer->addWidget(m_pVideoPlayer);
    m_pVideoPlayer->show();
}
