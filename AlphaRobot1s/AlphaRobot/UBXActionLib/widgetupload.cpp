/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：WidgetUpload.cpp
* 创建时间：2016/05/17 17:38
* 文件标识：
* 文件摘要：动作上传界面
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/05/17 17:38
* 版本摘要：
*/
#include "widgetupload.h"
#include "ui_widgetupload.h"

#include <QPainter>
#include <QFileDialog>
#include <QPicture>

#include "UBXPublic.h"
#include "ActUpdateMangr.h"
#include "WidgetUploadAddAction.h"
#include "Util.h"
#include "ubxactionlibconstants.h"
#include "Json/json.h"
#include "ubxactionlibconstants.h"
#include "../common/regexp.h"
#include "../common/textedit.h"
#include "widget/videocapture.h"
#include "../zxLib/configs.h"
#include "filedirhandle.h"
#include "cqiniuupoadhandle.h"
#include "cactionlibdatapool.h"
#include "../zxLib/widget/messagebox.h"
#include "../common/cfont.h"

const int DEFAULT_UPLOAD_FILE_SIZE_MB_MAX_IMAGE  = 2;//MB
const int DEFAULT_UPLOAD_FILE_SIZE_MB_MAX_VEDIO  = 15;//MB

#include "cactionlibdatapool.h"

RETRANSLATEUIFUNCIMPL(WidgetUpload)

WidgetUpload::WidgetUpload(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::WidgetUpload),
    m_pPreWidget(NULL),
    m_dragPosition(QPoint(-1, -1)),
    m_nRequestID(-1),
    m_nUploadPercentInfo(0),
    m_nUploadPercentDesc(0),
    m_nUploadPercentHead(0),
    m_nTryTimeForUpLoad(0),
    m_nSchemId(-1),
    m_bSetedDescpicToBtn(false),
    m_bEnableReceptQiniuUploadNotify(false)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setDefaultStyle();

    ui->label_coverpic->setEnableElidedText(true);
    ui->label_descpic->setEnableElidedText(true);

    //connect(ui->pbClose, &QPushButton::clicked, this, &WidgetUpload::hide);
    connect(ui->pushButton_prestep, &QPushButton::clicked, this, &WidgetUpload::onprestep_clicked);
    connect(ui->pushButton_upload, &QPushButton::clicked, this, &WidgetUpload::onupload_clicked);
#ifdef Q_OS_OSX
    ui->pbClose_Win->hide();
    connect(ui->pbClose, &QPushButton::clicked, this, &WidgetUpload::onClose);
#else
    ui->pbClose->hide();
    connect(ui->pbClose_Win, &QPushButton::clicked, this, &WidgetUpload::onClose);
#endif

    ui->radioButton_dance->setChecked(true);

    connect(g_actupdatemgr, &CActUpdateMangr::SigUploadProgress, this, &WidgetUpload::OnUploadProgress);
    connect(g_actupdatemgr, &CActUpdateMangr::sigUploadActionResponse, this, &WidgetUpload::onUploadActionResponse);

    connect(&CQiniuUpoadHandle::getInstance(), &CQiniuUpoadHandle::sigUploadFinish,
            this, &WidgetUpload::onQiniuUploadFinish);

    initUI();
}

WidgetUpload::~WidgetUpload()
{
    delete ui;
}

void WidgetUpload::initUI()
{
    //
    ClearContents();

    const QString iconAddPath = ":/res/images/icon_addPic.png";
    const QString changeRow = "\n\n";
    const QFont font = this->font();

    ui->label_coverpic->setFont(font);
    ui->label_coverpic->setImage(iconAddPath);
    ui->label_coverpic->setText(changeRow + tr("add cover"));
    ui->label_coverpic->setAlignment(Qt::AlignCenter);


    ui->label_descpic->setFont(font);
    ui->label_descpic->setImage(iconAddPath);
//    ui->label_descpic->setText(changeRow + tr("add desc pic"));
    ui->label_descpic->setText(changeRow + tr("picture/video"));
    ui->label_descpic->setAlignment(Qt::AlignCenter);

    ui->lineEdit_actname->setMaxTextMetric(200);
    ui->lineEdit_actname->setEnableLimitMetrics(true);

    ui->textEdit_actintro->setChartsMaxLength(400);

    ui->labeHint->clear();
    ui->labeHint->hide();
}

//上一步点击事件槽
void WidgetUpload::onprestep_clicked()
{
    if(m_pPreWidget)
    {
        this->hide();
        m_pPreWidget->show();
    }
}

//上传点击事件槽
void WidgetUpload::onupload_clicked()
{

    const int nUserId = g_actupdatemgr->GetUserID();

    //delete the unused video file of qiniu
    m_listActionDescFilePaths.removeAll(m_sActionDescFilePath);
    m_listActionDescFilePaths.removeDuplicates();
    ////not delete video, if delete maybe a dug of deleting the used video of other actioninfo
//    CQiniuUpoadHandle::getInstance().batchDelete(m_listActionDescFilePaths, nUserId);
    m_listActionDescFilePaths.clear();

   ///
    if(m_sActionCoverImagePath.isEmpty())
    {
        CMessageBox::warning(this, "", tr("action cover image does not exist")); //动作封面不存在
        return;
    }

    //
    ActItemData actData;
    actData.m_sActName = ui->lineEdit_actname->text().trimmed();
    if(actData.m_sActName.isEmpty())
    {
        CMessageBox::warning(this, "", tr("action name does not exist")); //动作名称不存在
        return;
    }

    //type
    int nTempType = eActTypeAll;
    if(ui->radioButton_dance->isChecked())
    {
        nTempType = eActTypeDance;
    }
    else if(ui->radioButton_sport->isChecked())
    {
        nTempType = eActTypeSport;
    }
    else if(ui->radioButton_story->isChecked())
    {
        nTempType = eActTypeStory;
    }
    else if(ui->radioButton_childsong->isChecked())
    {
        nTempType = eActTypeMusic;
    }
    else if(ui->radioButton_science->isChecked())
    {
        nTempType = eActTypeEducation;
    }

    if(eActTypeAll == nTempType)
    {
        CMessageBox::warning(this, "", tr("action type does not exist")); //动作类型不存在
        return;
    }

    actData.m_nActType = nTempType;

    // 活动id
    actData.m_nSchemId = m_nSchemId;

    // schemeName
    QString strSchemeName = CActionLibDataPool::GetInstance()->getBusinessPromoteName(m_nSchemId);
    actData.m_strSchemeName = strSchemeName;

    // 视频文件URL
    actData.m_sVideoURL = m_strActionVideoUrl;

    //
    if(m_sActionDescFilePath.isEmpty())
    {
        CMessageBox::warning(this, "", tr("action desc file does not exist")); //动作描述文件不存在
        return;
    }

    //
    actData.m_sActDesc = ui->textEdit_actintro->toPlainText().trimmed();
    if(actData.m_sActDesc.isEmpty())
    {
        CMessageBox::warning(this, "", tr("Action info is empty"));
        return;
    }

    if(actData.m_sActDesc.size() > Constants::UPLOAD_ACTION_DESP_MAX_CHAR)
    {
        CMessageBox::warning(this, "", QString("%1 %2 %3!").arg(tr("Action Introduction is larger than")).arg(Constants::UPLOAD_ACTION_DESP_MAX_CHAR).arg(tr("character")));
        return;
    }

    /////start to upload
    this->setEnabled(false);

    const QString strUploading = tr("Uploading");
    const int nCountDot = m_nTryTimeForUpLoad%4;
    QString strDot;
    strDot.clear();
    for(int i = 0; i < nCountDot; i++)
    {
        strDot.append(".");
    }

    ui->labeHint->setText(strUploading + strDot);
    ui->labeHint->show();

//    if(m_nUploadPercentDesc < 100)
//    {
//        CMessageBox::warning(this, "", tr("action desc image or video file upload unfinished")); //动作描述图片或视频上传未完成
//        return;
//    }


//    if(((WidgetUploadAddAction*)m_pPreWidget)->m_nUploadPercent < 100)
//    {
//        CMessageBox::warning(this, "", tr("action file upload unfinished")); //动作文件上传未完成
//        return;
//    }


    if( (Constants::UPLOAD_DONE <= ((WidgetUploadAddAction*)m_pPreWidget)->m_nUploadPercent) &&
            (Constants::UPLOAD_DONE <= m_nUploadPercentDesc) &&
            (Constants::UPLOAD_DONE <= m_nUploadPercentHead))
    {
        m_nTryTimeForUpLoad = 0;

        const int nUserId = g_actupdatemgr->GetUserID();

        //delete the unused video file of qiniu
        m_listActionDescFilePaths.removeAll(m_sActionDescFilePath);
//        CQiniuUpoadHandle::getInstance().batchDelete(m_listActionDescFilePaths, nUserId);
        m_listActionDescFilePaths.clear();

        //upload the info
        actData.m_sImageHeadUrl = m_sActionCoverImagePath;
        qint64 nActionID = ((WidgetUploadAddAction*)m_pPreWidget)->getActionID();
        actData.m_nItemID = nActionID;
        actData.m_nActSize = CUtil::GetFileSize(((WidgetUploadAddAction*)m_pPreWidget)->getZipPath());
        actData.m_dwActTimeLen = ((WidgetUploadAddAction*)m_pPreWidget)->getActTotalTime();

        if(g_actupdatemgr)
        {
            m_nUploadPercentInfo = 0;
            m_nRequestID = g_actupdatemgr->UploadActionInfo(actData, m_sActionCoverImageUrl, m_sActionDescImageUrl, nActionID, nUserId, m_nRequestID);
        }
    }

    else
    {
        if(m_nUploadPercentDesc >= Constants::UPLOAD_DONE)
        {
            m_nTryTimeForUpLoad++;
        }

        if(10 < m_nTryTimeForUpLoad)
        {
            CMessageBox::warning(this, "", tr("action file upload unfinished")); //动作文件上传未完成

            onClose();

            return;
        }
        else
        {
            QTimer::singleShot(1200, this, &WidgetUpload::onupload_clicked);
        }
    }
}

/************************************
* 名称: setDefaultStyle
* 功能: 设置窗体默认样式
* 返回:   void
* 时间:   2016/03/31
* 作者:   hels
************************************/
void WidgetUpload::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/ubxactionupload.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

void WidgetUpload::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void WidgetUpload::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = QPoint(-1, -1);
        event->accept();
    }
}

/************************************
* 名称: onQiniuNotify
* 功能: qiniu upload file info to notify
* 参数:
*    @[in ] recvr: which to rec the info
*    @[in ] nBlkCount:
*    @[in ] nBlkIdx: current blk index
*    @[in ] nBlkSize:
*    @[in ] nBlkOffset: have upload size
* 返回值:
* 时间: 20160912
* 作者: asu
*/
void WidgetUpload::onQiniuNotify(INotify *recvr, int nBlkCount, int nBlkIdx, int nBlkSize, int nBlkOffset)
{
    if((recvr == (INotify*)this) && m_bEnableReceptQiniuUploadNotify)
    {
        const int nPercent = INotify::getPercentage(nBlkCount, nBlkIdx, nBlkSize, nBlkOffset);
        AlphaRobotLog::getInstance()->printDebug(QString("qiniu upload percent: %1").arg(QString::number(nPercent)));

        repainUploadPercentDesc(nPercent);
    }
}

void WidgetUpload::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();

        QRect coverpicrect = ui->label_coverpic->geometry();
        QRect descpicrect = ui->label_descpic->geometry();
        QPoint curpos = event->pos();

        if(coverpicrect.contains(curpos))
            oncoverpic_clicked();
        else if(descpicrect.contains(curpos))
            ondescpic_clicked();

        event->accept();
    }
}


void WidgetUpload::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() &Qt::LeftButton)
    {
        if (m_dragPosition != QPoint(-1, -1))
            move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

//封面点击事件槽
void WidgetUpload::oncoverpic_clicked()
{    
    QString sFmtTitle = tr("Image File");
    QString sFmtExt = IMAGE_SUPPORT_FORMAT;
    QString sFmt;
#ifdef Q_OS_MAC
    sFmt.sprintf("%s(%s);;%s(%s)",
                 sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str(),
                 sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str());
#else
    sFmt.sprintf("%s(%s)", sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str());
#endif
    QString sImgPath = QFileDialog::getOpenFileName(NULL, tr("Open File"), "/", sFmt);
    if(sImgPath.isEmpty())
    {
        return;
    }
    sImgPath = QDir::toNativeSeparators(sImgPath);    

    qint64 nFileSize = CFileDirHandle::getFileSize(sImgPath);
    qint64 nFileSizeMB = CFileDirHandle::dBToMB(nFileSize);
    if(nFileSizeMB > DEFAULT_UPLOAD_FILE_SIZE_MB_MAX_IMAGE)
    {

        QString strTip = tr("File size must be smaller than");
        strTip.append(QString(" %1MB").arg(QString::number(DEFAULT_UPLOAD_FILE_SIZE_MB_MAX_IMAGE)));
        CMessageBox::warning(this, "", strTip); //

        return;
    }

    //QPixmap newPixmap(sImgPath);
    //newPixmap.scaled(Constants::ICON_ACTION_UPLOAD_COVERPIC_WIDTH, Constants::ICON_ACTION_UPLOAD_COVERPIC_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //ui->label_coverpic->setPixmap(newPixmap);

    if(!sImgPath.isEmpty() && g_actupdatemgr)
    {
        m_sActionCoverImagePath = sImgPath;
        m_nUploadPercentHead = 0;

        int nUserId = g_actupdatemgr->GetUserID();
        qint64 nActionID = ((WidgetUploadAddAction*)m_pPreWidget)->getActionID();

        ui->label_coverpic->setImage(sImgPath);
        repainUploadPercentCover(1);

        m_nRequestID = g_actupdatemgr->UploadActionImage(eUploadFileHeadImg, sImgPath, nActionID, nUserId, m_nRequestID);
    }
}

//描述点击事件槽
void WidgetUpload::ondescpic_clicked()
{
    // 上传文件过程中禁用
    ui->pushButton_upload->setEnabled(false);

    m_bSetedDescpicToBtn = false;

    QString sImgFmtTitle = tr("Image File");
    QString sImgFmtExt = IMAGE_SUPPORT_FORMAT;
    QString sVideoFmtTitle = tr("Video File");
    QString sVideoFmtExt = VIDEO_SUPPORT_FORMAT;
    QString strFmt;

#ifdef USEQINIU
//    strFmt.sprintf("%s(%s);;%s(*%s)",
//                 sImgFmtTitle.toStdString().c_str(), sImgFmtExt.toStdString().c_str(),
//                 sVideoFmtTitle.toStdString().c_str(), sVideoFmtExt.toStdString().c_str());
    strFmt.sprintf("%s/%s(*%s %s)",
                   sVideoFmtTitle.toStdString().c_str(),
                   sImgFmtTitle.toStdString().c_str(),
                   sVideoFmtExt.toStdString().c_str(),
                   sImgFmtExt.toStdString().c_str());
#else
    strFmt.sprintf("%s(%s)",
                 sImgFmtTitle.toStdString().c_str(), sImgFmtExt.toStdString().c_str());
#endif

    QString sFilePath = QFileDialog::getOpenFileName(NULL, tr("Open File"), "/", strFmt);
    if(sFilePath.isEmpty())
    {
        return;
    }
    sFilePath = QDir::toNativeSeparators(sFilePath);

    bool bIsImage = true;  //是否是图片，否则是视频

    QString sUploadFilePath;
    QString sTempExt = sFilePath.mid(sFilePath.lastIndexOf('.'));
    if(sImgFmtExt.indexOf(sTempExt, 0, Qt::CaseInsensitive) != -1)
    {
        //是图片
        sUploadFilePath = sFilePath;
        bIsImage = true;
    }
    else if(sVideoFmtExt.indexOf(sTempExt, 0, Qt::CaseInsensitive) != -1)
    {
        //是视频
        sUploadFilePath = sFilePath;
        bIsImage = false;
    }

    int nFileMaxSizeMB = DEFAULT_UPLOAD_FILE_SIZE_MB_MAX_VEDIO;
    if(bIsImage)
    {
        nFileMaxSizeMB = DEFAULT_UPLOAD_FILE_SIZE_MB_MAX_IMAGE;
    }

    qint64 nFileSize = CFileDirHandle::getFileSize(sFilePath);
    qint64 nFileSizeMB = CFileDirHandle::dBToMB(nFileSize);
    if(nFileSizeMB > nFileMaxSizeMB)
    {

        QString strTip = tr("File size must be smaller than");
        strTip.append(QString(" %1MB").arg(QString::number(nFileMaxSizeMB)));
        CMessageBox::warning(this, "", strTip); //

        return;
    }

    if(!sUploadFilePath.isEmpty() && !CQiniuUpoadHandle::getInstance().isEnableUpload())
    {
        QString strTip = tr("Upload file is busy. please try again later.");
        CMessageBox::warning(this, "", strTip); //

        return;
    }

    if(!sUploadFilePath.isEmpty() && g_actupdatemgr)
    {
        m_sActionDescFilePath = sUploadFilePath;
        if(!bIsImage)
        {
            m_listActionDescFilePaths.append(m_sActionDescFilePath);
        }

        m_nUploadPercentDesc = 0;

        int nUserId = g_actupdatemgr->GetUserID();
        qint64 nActionID = ((WidgetUploadAddAction*)m_pPreWidget)->getActionID();

        m_bEnableReceptQiniuUploadNotify = !bIsImage;
        if(bIsImage)
        {
            ui->label_descpic->setImage(sFilePath);
            repainUploadPercentDesc(1);

            m_nRequestID = g_actupdatemgr->UploadActionImage(eUploadFileDescImg, sUploadFilePath, nActionID, nUserId, m_nRequestID);
        }
#ifdef USEQINIU
        else
        {            
            //显示描述图片或视频
            const QString sImgFmtExt = IMAGE_SUPPORT_FORMAT;
            const QString sVideoFmtExt = VIDEO_SUPPORT_FORMAT;

            QString strImgPath;
            strImgPath.clear();

            QString sTempExt = m_sActionDescFilePath.mid(m_sActionDescFilePath.lastIndexOf('.'));
            if(!m_bSetedDescpicToBtn && sImgFmtExt.indexOf(sTempExt, 0, Qt::CaseInsensitive) != -1)
            {
                //HTTP_REQUEST_UPLOADACTIONDESCIMG
                strImgPath = m_sActionDescFilePath;
            }

            else if(!m_bSetedDescpicToBtn && (sVideoFmtExt.indexOf(sTempExt, 0, Qt::CaseInsensitive) != -1) )
            {
                //HTTP_REQUEST_UPLOADACTIONDESCVIDEO
                CVideoCapture videoCapture;
                const QString strCaptureImgPath = CConfigs::getLocalTempPath("captureImg.png");
                bool bGetImg = videoCapture.captureFrame(m_sActionDescFilePath, strCaptureImgPath, 1, 5000);

                if(bGetImg)
                {
                    strImgPath = strCaptureImgPath;
                }
            }

            if(!strImgPath.isEmpty() && QFile::exists(strImgPath))
            {

                ui->label_descpic->setPixmap(strImgPath);
                repainUploadPercentDesc(1);

                m_bSetedDescpicToBtn = true;
            }

            //
            m_nRequestID = g_actupdatemgr->UploadActionDescVideo(sUploadFilePath, nActionID, nUserId, m_nRequestID, m_strActionVideoUrl, (INotify*)this);
        }
#endif
    }
}


/************************************
* 名称: OnUploadProgress
* 功能: 上传文件进度通知
* 参数:
*    @[in ] sRequestType: 请求类型字符串
*    @[in ] nRequestID:  请求ID
*    @[in ] nActionID:  动作ID
*    @[in ] bytesUploaded: 已上传的数据长度
*    @[in ] bytesTotal: 计划上传的数据总长度
* 返回: void
* 时间: 2016/05/27 19:27
* 作者: ZDJ
*/
void WidgetUpload::OnUploadProgress(const QString& sRequestType, int nRequestID, qint64 nActionID, qint64 bytesUploaded, qint64 bytesTotal)
{
    if(nRequestID != m_nRequestID)
    {
        return;
    }

    QString sInfo;
    const int nPercent = bytesUploaded * 1.0 / bytesTotal * 100;

    sInfo.sprintf("UploadType:%s, ActionID:%lld, Percent:%d%%", sRequestType.toStdString().c_str(), nActionID, nPercent);
    qDebug() << sInfo;

    if((sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONDESCIMG) == 0) ||
            (sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONDESCVIDEO) == 0))
    {
        if (sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONDESCIMG) == 0)
        {
            repainUploadPercentDesc(nPercent);
        }
        else if (sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONDESCVIDEO) == 0)
        {            
            repainUploadPercentDesc(nPercent);
        }
    }

    else if(sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONHEADIMG) == 0)
    {
        repainUploadPercentCover(nPercent);
    }

    else if(sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONINFO) == 0)
    {
        if(nPercent > m_nUploadPercentInfo)
        {
            m_nUploadPercentInfo = nPercent;

            if(bytesUploaded >= bytesTotal)
            {
                if(sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONINFO) == 0)
                {
                    ui->labeHint->clear();
                    CMessageBox::information(this, "", tr("Upload Success"));
                    onClose();
                }
            }
        }
    }
}

/************************************
* 名称: ClearContents
* 功能: 清除界面内容
* 时间: 2016/06/15 15:48
* 作者: ZDJ
*/
void WidgetUpload::ClearContents()
{
    ui->label_coverpic->clear();
    ui->label_coverpic->setPixmap(QPixmap());

    ui->lineEdit_actname->clear();
    ui->radioButton_dance->setChecked(true);

    ui->label_descpic->clear();
    ui->label_descpic->setPixmap(QPixmap());

    ui->textEdit_actintro->clear();
    m_nUploadPercentInfo = 0;
    m_nUploadPercentDesc = 0;
    m_nUploadPercentHead = 0;

    m_nTryTimeForUpLoad = 0;

    m_sActionCoverImagePath.clear();
    m_sActionDescFilePath.clear();
    m_listActionDescFilePaths.clear();
    m_sActionCoverImageUrl.clear();
    m_sActionDescImageUrl.clear();
    m_strActionVideoUrl.clear();

    m_bEnableReceptQiniuUploadNotify = false;

    ui->labeHint->clear();
    ui->labeHint->hide();

    this->setEnabled(true);
}


void WidgetUpload::setActionName(const QString &actName)
{
    ui->lineEdit_actname->setText(actName);
}

/************************************
* 名称: onUploadActionResponse
* 功能: 上传动作应答槽
* 参数:
*    @[in ] sRequestType: 请求类型字符串
*    @[in ] nRequestID: 请求ID
*    @[in ] nActionID: 动作ID
*    @[in ] bytesData: 应答数据
* 返回: void
* 时间: 2016/06/15
* 作者: ZDJ
*/
void WidgetUpload::onUploadActionResponse(const QString& sRequestType, int nRequestID, qint64 nActionID, const QByteArray& bytesData)
{
    if(nRequestID != m_nRequestID)
        return;

    if(sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONHEADIMG) == 0
            || sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONDESCIMG) == 0)
    {
        QString sInfo = QString("UploadType:%1, ActionID:%2, bytesData:%3").arg(sRequestType).arg(nActionID).arg(QString(bytesData));
        qDebug() << sInfo;

        Json::Reader jsonreader;
        Json::Value  jvalue_root;
        if(jsonreader.parse(bytesData.toStdString().c_str(), jvalue_root))
        {
            if(!jvalue_root["models"].isNull())
            {
                if(!jvalue_root["models"]["imageType"].isNull())
                {
                    int nImgType = atoi(jvalue_root["models"]["imageType"].asCString());
                    QString sImgUrl = QString::fromLatin1(jvalue_root["models"]["imageUrl"].asCString());
                    if(nImgType == eUploadFileHeadImg)
                    {
                        //上传头像图片的应答
                        m_sActionCoverImageUrl = sImgUrl;
                    }
                    else if(nImgType == eUploadFileDescImg)
                    {
                        //上传描述图片的应答
                        m_sActionDescImageUrl = sImgUrl;

                        // 图片上传完后上传按钮才能使用
                        ui->pushButton_upload->setEnabled(true);
                    }
                }
            }

        }
    }
}


/************************************
* 名称: onQiniuUploadFinish
* 功能: qiniu上传动作应答槽
* 参数:
*    @[in ] nRequestID: 请求ID
*    @[in ] bOk: 动作ID
* 返回: void
* 时间: 20160927
* 作者: asu
*/
void WidgetUpload::onQiniuUploadFinish(const int nRequestID, bool bOk)
{
    // 视频上传完后上传按钮才能使用
    ui->pushButton_upload->setEnabled(true);

    if((nRequestID == m_nRequestID) && !bOk)
    {
        AlphaRobotLog::getInstance()->printError(QString("onQiniuUploadFinish: nRequestID=%1  false").arg(nRequestID));
        CMessageBox::warning(this, "", tr("video file upload err")); //video上传未完成

        onClose();
    }
}

//关闭事件槽
void WidgetUpload::onClose()
{
    //调试模式下为方便调试，不清除内容
    ClearContents();    
    initUI();

    ((WidgetUploadAddAction*)m_pPreWidget)->ClearContents();

    this->setEnabled(true);
    this->hide();
}

//repain the label of desc video
void WidgetUpload::repainUploadPercentDesc(const int nUploadPercentDesc)
{
    const int nPercent = nUploadPercentDesc;
    if(nPercent > m_nUploadPercentDesc)
    {
        m_nUploadPercentDesc = nPercent;

        if(m_nUploadPercentDesc > Constants::UPLOAD_DONE)
        {
            m_nUploadPercentDesc = Constants::UPLOAD_DONE;
        }

        QFont font = this->font();
        font = CFont::getFont(font, 6, true);
        ui->label_descpic->setFont(font);

        //更新描述图片或视频的上传进度
        if (m_nUploadPercentDesc < Constants::UPLOAD_DONE)
        {
            QString sTemp;
            sTemp.sprintf("%d%%",m_nUploadPercentDesc);
            ui->label_descpic->setText(sTemp);
        }
        else
        {
            ui->label_descpic->setText("");
        }

//        ui->label_descpic->repaint();
    }
}

void WidgetUpload::repainUploadPercentCover(const int nUploadPercentCover)
{
    const int nPercent = nUploadPercentCover;
    if(nPercent > m_nUploadPercentHead)
    {
        m_nUploadPercentHead = nPercent;

        QFont font = this->font();
        font = CFont::getFont(font, 6, true);

        ui->label_coverpic->setFont(font);

        //更新头像的上传进度
        if (m_nUploadPercentHead < Constants::UPLOAD_DONE)
        {
            QString sTemp;
            sTemp.sprintf("%d%%",nPercent);
            ui->label_coverpic->setText(sTemp);
        }
        else // 上传完成后不显示100%
        {
            ui->label_coverpic->setText("");
        }
    }
}
