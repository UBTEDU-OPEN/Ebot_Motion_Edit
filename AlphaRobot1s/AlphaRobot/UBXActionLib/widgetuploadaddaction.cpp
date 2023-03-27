/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：WidgetUploadAddAction.cpp
* 创建时间：2016/05/18 17:15
* 文件标识：
* 文件摘要：动作上传添加动作界面
*
* 当前版本：1.0.0.0
* 作    者：ZDJ
* 完成时间：2016/05/18 17:15
* 版本摘要：
*/
#include "widgetuploadaddaction.h"
#include "ui_widgetuploadaddaction.h"

#include <QFileDialog>
#include <QPainter>
#include <QTime>

#include "Util.h"
#include "UBXPublic.h"
#include "ActUpdateMangr.h"
#include "widgetupload.h"
#include "../common/filedirhandle.h"
#include "../common/htsparser.h"
#include "../zxLib/configs.h"
#include "../zxLib/widget/messagebox.h"


RETRANSLATEUIFUNCIMPL(WidgetUploadAddAction)
WidgetUploadAddAction::WidgetUploadAddAction(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::WidgetUploadAddAction),
    m_pNextWidget(NULL),
    m_dragPosition(QPoint(-1, -1)),
    m_nRequestID(-1),
    m_nUploadPercent(0),
    m_nSchemId(-1)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setDefaultStyle();

    ClearContents();

    //connect(ui->pbClose, &QPushButton::clicked, this, &WidgetUploadAddAction::hide);
    connect(ui->pushButton_actsel, &QPushButton::clicked, this, &WidgetUploadAddAction::onactsel_clicked);
    connect(ui->pushButton_audiosel, &QPushButton::clicked, this, &WidgetUploadAddAction::onaudiosel_clicked);
    connect(ui->pushButton_nextstep, &QPushButton::clicked, this, &WidgetUploadAddAction::onnextstep_clicked);


#ifndef QT_NO_DEBUG
    //测试用
#ifdef Q_OS_MAC
    ui->lineEdit_uploadact->setText("/Users/zdj/Desktop/act/Little apple.hts");
    ui->lineEdit_uploadaudio->setText("/Users/zdj/Desktop/act/Little apple.mp3");
#elif defined(Q_OS_WIN)
    ui->lineEdit_uploadact->setText("D:\\qqq\\action2(1)\\Little apple.hts");
    ui->lineEdit_uploadaudio->setText("D:\\qqq\\music\\Little apple.mp3");
#endif
#endif

    connect(g_actupdatemgr, &CActUpdateMangr::SigUploadProgress, this, &WidgetUploadAddAction::OnUploadProgress);

#ifdef Q_OS_OSX
    ui->pbClose_Win->hide();
    connect(ui->pbClose, &QPushButton::clicked, this, &WidgetUploadAddAction::onClose);
#else
    ui->pbClose->hide();
    connect(ui->pbClose_Win, &QPushButton::clicked, this, &WidgetUploadAddAction::onClose);
#endif

    ui->label_remarktext->setText(tr("Please make sure that two file names are the same"));

    //
    ui->label_remark->hide();
    ui->label_remarktext->hide();

    ui->lineEdit_uploadact->setEnabled(false);
    ui->lineEdit_uploadaudio->setEnabled(false);
}

WidgetUploadAddAction::~WidgetUploadAddAction()
{
    delete ui;
}

/************************************
* 名称: setDefaultStyle
* 功能: 设置窗体默认样式
* 返回:   void
* 时间:   2016/03/31
* 作者:   hels
************************************/
void WidgetUploadAddAction::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/ubxactionupload.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

//动作选择点击事件槽
void WidgetUploadAddAction::onactsel_clicked()
{
    m_sActPath.clear();

    QString sFmtTitle = tr("Action File");
    QString sFmt;

#ifdef Q_OS_MAC
    sFmt.sprintf("%s(*%s);;%s(*%s)", sFmtTitle.toStdString().c_str(), HTS_FILE_EXT,
                 sFmtTitle.toStdString().c_str(), HTS_FILE_EXT);
#else
    sFmt.sprintf("%s(*%s)", sFmtTitle.toStdString().c_str(), HTS_FILE_EXT);
#endif

    QString sActPath = QFileDialog::getOpenFileName(NULL, tr("Open File"), "/", sFmt);
    sActPath = sActPath.trimmed();
    if(sActPath.isEmpty())
    {
        return;
    }

    m_sActPath = QDir::toNativeSeparators(sActPath);

    QFileInfo fileActPath(m_sActPath);
    ui->lineEdit_uploadact->setText(fileActPath.fileName());
}

//音频选择点击事件槽
void WidgetUploadAddAction::onaudiosel_clicked()
{
    m_sAudioPath.clear();

    QString sFmtTitle = tr("Audio File");
    QString sFmtExt = MUSIC_FILE_EXT;
    QString sFmt;

#ifdef Q_OS_MAC
    sFmt.sprintf("%s(*%s);;%s(*%s)", sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str(), sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str());
#else
    sFmt.sprintf("%s(*%s)", sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str());
#endif

    QString sActPath = QFileDialog::getOpenFileName(NULL, tr("Open File"), "/", sFmt);
    sActPath = sActPath.trimmed();

    if(sActPath.isEmpty())
    {
        return;
    }    

    m_sAudioPath = QDir::toNativeSeparators(sActPath);

    QFileInfo fileAudio(m_sAudioPath);
    ui->lineEdit_uploadaudio->setText(fileAudio.fileName());
}

void WidgetUploadAddAction::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void WidgetUploadAddAction::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = QPoint(-1, -1);
        event->accept();
    }
}


void WidgetUploadAddAction::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}


void WidgetUploadAddAction::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() &Qt::LeftButton)
    {
        if (m_dragPosition != QPoint(-1, -1))
            move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

//下一步点击事件槽
void WidgetUploadAddAction::onnextstep_clicked()
{
    if(m_pNextWidget)
    {
        //zip打包
        const QString sActPath = m_sActPath;
        const QString sAudioPath = m_sAudioPath;

        QFileInfo tempfileinfo(sActPath);
        if(!tempfileinfo.exists())
        {
            CMessageBox::warning(this, "", tr("action file does not exist"));   //动作文件不存在
            return;
        }

        tempfileinfo.setFile(sAudioPath);
        if(!sAudioPath.isEmpty() && !tempfileinfo.exists())
        {
            CMessageBox::warning(this, "", tr("audio file does not exist"));  //音乐文件不存在
            return;
        }

        const qint64 curtimevalue = QDateTime::currentMSecsSinceEpoch();
        const QString sCurTimeValue = QString::number(curtimevalue);

        QString sNewActName = sCurTimeValue + HTS_FILE_EXT;
        QString sNewAudioName = sCurTimeValue + MUSIC_FILE_EXT;

        QString sNewActPath = QDir::toNativeSeparators(CConfigs::getLocalTempDir() + "/" + sNewActName);
        QString sNewAudioPath = QDir::toNativeSeparators(CConfigs::getLocalTempDir() + "/" + sNewAudioName);

        if(!QFile::copy(sActPath, sNewActPath))
        {
            CMessageBox::warning(this, "", tr("action file copy failed"));  //动作文件拷贝失败
            return;
        }

        if(!sAudioPath.isEmpty() && !QFile::copy(sAudioPath, sNewAudioPath))
        {
            CMessageBox::warning(this, "", tr("audio file copy failed"));  //音乐文件拷贝失败
            return;
        }

        QString sZipFilePath;
        sZipFilePath.sprintf("%lld.zip", curtimevalue);

        sZipFilePath.insert(0, QDir::toNativeSeparators(CConfigs::getLocalTempDir() + "/"));

        m_sZipPath = sZipFilePath;

        vector<QString> vecFileList;
        vecFileList.push_back(sNewActPath);

        if(!sAudioPath.isEmpty())
        {
            vecFileList.push_back(sNewAudioPath);
        }

        bool bZipRet = ZipFile(vecFileList, m_sZipPath);


        //删除临时文件
        QFile tempfile(sNewActPath);
        tempfile.remove();

        tempfile.setFileName(sNewAudioPath);
        tempfile.remove();

        if(!bZipRet)
        {
            CMessageBox::warning(this, "", tr("file package failed")); //文件打包失败
            return;
        }

        //上传动作        
        const qint64  nActionID = getActionID();

        if(g_actupdatemgr)
        {
            m_nUploadPercent = 0;
            int nUserId = g_actupdatemgr->GetUserID();
            m_nRequestID = g_actupdatemgr->UploadActionFile(m_sZipPath, nActionID, nUserId, m_nRequestID);

            this->hide();

            WidgetUpload* pNextWidget = dynamic_cast<WidgetUpload*>(m_pNextWidget);
            if(pNextWidget)
            {
                const QString uploadActName = ui->lineEdit_uploadact->text().trimmed();
                QFileInfo fileInfo(uploadActName);
                const QString fileBaseName = fileInfo.baseName();
                pNextWidget->setActionName(fileBaseName);

                // 设置活动id
                pNextWidget->setSchemId(m_nSchemId);
            }

            m_pNextWidget->setEnabled(true);
            this->hide();
            m_pNextWidget->show();
        }
    }
}

void WidgetUploadAddAction::showEvent(QShowEvent* event)
{
    if(event->type() == QEvent::Show)
    {
        if(!m_sZipPath.isEmpty())
        {
            QFile tempfile(m_sZipPath);
            tempfile.remove();
        }
    }
    QWidget::showEvent(event);
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
void WidgetUploadAddAction::OnUploadProgress(const QString& sRequestType, int nRequestID, qint64 nActionID, qint64 bytesUploaded, qint64 bytesTotal)
{
    if(nRequestID != m_nRequestID)
        return;

    QString sInfo;
    int nPercent = bytesUploaded * 1.0 / bytesTotal * 100;
    if(nPercent > m_nUploadPercent)
    {
        m_nUploadPercent = nPercent;
        sInfo.sprintf("UploadType:%s, ActionID:%lld, Percent:%d%%", sRequestType.toStdString().c_str(), nActionID, nPercent);
        qDebug() << sInfo;

        if(bytesUploaded >= bytesTotal)
        {
        }
    }
}

/************************************
* 名称: getActionID
* 功能: 获取动作ID
* 时间: 2016/06/04 13:14
* 作者: ZDJ
*/
qint64 WidgetUploadAddAction::getActionID()const
{
    QString sFileName = CFileDirHandle::getFileName(m_sZipPath);
    const qint64  nActionID = sFileName.left(sFileName.lastIndexOf('.')).toLongLong();
    return nActionID;
}

qint64 WidgetUploadAddAction::getActTotalTime() const
{
    int nHtsActTotalTime = 0;
    char *pHtsData = NULL;

    qint64 readLen = 0;
    readLen = CFileDirHandle::read(m_sActPath, &pHtsData);
    if(pHtsData)
    {
        CHtsParser::getActTotalTime(pHtsData, readLen, nHtsActTotalTime);
    }

    SAFE_DELETE(pHtsData);

    return nHtsActTotalTime/1000;//ms to s
}

/************************************
* 名称: ClearContents
* 功能: 清除界面内容
* 时间: 2016/06/15 15:48
* 作者: ZDJ
*/
void WidgetUploadAddAction::ClearContents()
{
    ui->lineEdit_uploadact->clear();
    ui->lineEdit_uploadaudio->clear();

    m_sActPath.clear();
    m_sAudioPath.clear();
    m_sZipPath.clear();

    m_nUploadPercent = 0;
    m_nSchemId = -1;
}

/************************************
* 名称: getZipPath
* 功能: 获取zip文件路径
* 时间: 2016/06/15 15:48
* 作者: ZDJ
*/
QString WidgetUploadAddAction::getZipPath()
{
    return m_sZipPath;
}

/************************************
* 名称: setSchemId
* 功能: 设置活动id
* 参数: [in] nId 活动id
* 时间: 2016/09/09 15:48
* 作者: hwx
*/
void WidgetUploadAddAction::setSchemId(int nId)
{
    m_nSchemId = nId;
}

//关闭事件槽
void WidgetUploadAddAction::onClose()
{
#ifdef QT_NO_DEBUG
    ClearContents();  //调试模式下为方便调试，不清除内容
#endif
    this->hide();
}
