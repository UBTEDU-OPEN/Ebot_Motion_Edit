#include "filessaveasui.h"
#include "ui_filessaveasui.h"


#include <QFileDialog>


#include "../common/filedirhandle.h"
#include "../zxLib/configs.h"
#include "../UBXLogin/loginlibmgr.h"
#include "../UBXMotionEditor/UBXMotionEditor.h"
#include "../UBXActionLib/actupdatemangr.h"
#include "timermanager.h"
#include "../common/htsparser.h"
#include "../UBXActionLib/ubxactionlibconstants.h"
#include "../zxLib/widget/ubxwidget.h"
#include "../zxLib/widget/messagebox.h"
#include "ubtlib.h"

RETRANSLATEUIFUNCIMPL(CFilesSaveAsUI)
CFilesSaveAsUI::CFilesSaveAsUI(SFileSaveAsDatas* pSaveAsDatas, QWidget *parent) :
    CUBXDialog(parent),
    ui(new Ui::CFilesSaveAsUI),
    m_pFileSaveAsDatas(pSaveAsDatas),
    m_pIUBXmEditor(NULL),
    m_nRequestID(-1),
    m_nUploadActZipPercent(0)
{
    ui->setupUi(this);

    const QString strTitle = tr("Save File");
    setWindowTitle(strTitle);

    connect(ui->pushButtonFilePath, &QPushButton::clicked,
            this, &CFilesSaveAsUI::onFilePathClicked);

    connect(ui->pushButtonSaveAndUpLoad, &QPushButton::clicked,
            this, &CFilesSaveAsUI::onSaveAndUpLoadClicked);

    connect(ui->pushButtonSaveLocal, &QPushButton::clicked,
            this, &CFilesSaveAsUI::onSaveLocalClicked);

    connect(ui->pushButtonCancel, &QPushButton::clicked,
            this, &CFilesSaveAsUI::onCancelClicked);

    connect(ui->myTitleBar, &MyTitleBar::sigClose, this, &CFilesSaveAsUI::onCancelClicked);

    if(m_pFileSaveAsDatas)
    {
        m_FileSaveAsDatasOld = *m_pFileSaveAsDatas;
        initUI(*m_pFileSaveAsDatas);
    }

    setEnableMove(true);
    ui->myTitleBar->setLayoutMargins(15, 20, 20, 10);
    ui->myTitleBar->setMinimumHeight(45);
    ui->myTitleBar->getTitleLabel()->setText(strTitle);

    UBXWidget::setDefaultStyle(this, ":res/qss/alphadefault.qss");
    UBXWidget::applyUBXWinFlags(this);

    ui->lineEditFileName->setForbidChars("\\/:*?\"<>|");
    ui->lineEditFileName->setMaxLength(80);
}

CFilesSaveAsUI::~CFilesSaveAsUI()
{
    delete ui;
}

void CFilesSaveAsUI::setIUBXMotionEditor(IUBXMotionEditor *pIUBXmEditor)
{
    m_pIUBXmEditor = pIUBXmEditor;
}

void CFilesSaveAsUI::saveData()
{
    if(m_pFileSaveAsDatas)
    {
        m_pFileSaveAsDatas->strSaveFileName = ui->lineEditFileName->text().trimmed();
        m_pFileSaveAsDatas->strSaveFilePath = ui->lineEditFilePath->text().trimmed();
    }
}

void CFilesSaveAsUI::initUI(SFileSaveAsDatas datas)
{
    ui->lineEditFileName->setText(datas.strSaveFileName);

    QString strFilePath = datas.strSaveFilePath;
    if (strFilePath.isEmpty())
    {
        strFilePath = CConfigs::getLocalDownloadDir(); // 默认路径
    }
    ui->lineEditFilePath->setText(strFilePath);
}


void CFilesSaveAsUI::resizeEvent(QResizeEvent *event)
{
    ui->myTitleBar->setFixedWidth(this->width());

    QWidget::resizeEvent(event);
}

void CFilesSaveAsUI::onTimerout(int nTimerID)
{
    if((nTimerID == m_nRequestID) && (0 < m_nRequestID))
    {
        showMessageUploadResult(false);

        return;
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
* 时间: 20160817
* 作者: asu
*/
void CFilesSaveAsUI::OnUploadProgress(const QString &sRequestType,
                                      int nRequestID,
                                      qint64 nActionID,
                                      qint64 bytesUploaded,
                                      qint64 bytesTotal)
{

    if(nRequestID != m_nRequestID)
    {
        return;
    }

    QString sInfo;
    const int nPercent = bytesUploaded * 1.0 / bytesTotal * 100;

    if(sRequestType.compare(Constants::HTTP_REQUEST_UPLOADACTIONFILE) == 0 )
    {
        if(nPercent > m_nUploadActZipPercent)
        {
            m_nUploadActZipPercent = nPercent;
            sInfo.sprintf("UploadType:%s, ActionID:%lld, Percent:%d%%", sRequestType.toStdString().c_str(), nActionID, nPercent);
            qDebug() << sInfo;

            if(bytesUploaded >= bytesTotal)
            {
                CTimerManager::getInstance().removeTimer(m_nRequestID);

                showMessageUploadResult(true);

                close();
            }
        }
    }
}

void CFilesSaveAsUI::onFilePathClicked()
{
    if(m_pFileSaveAsDatas)
    {
        const QString strFileDir = QFileDialog::getExistingDirectory(this,
                                                                     tr("Save Project"),
                                                                     m_pFileSaveAsDatas->strSaveFilePath,
                                                                     QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks); //选择路径

        if(strFileDir.isEmpty())
        {
            return;
        }

        m_pFileSaveAsDatas->strSaveFileName = ui->lineEditFileName->text().trimmed();
        m_pFileSaveAsDatas->strSaveFilePath = strFileDir;

        initUI(*m_pFileSaveAsDatas);
    }
}

void CFilesSaveAsUI::onSaveAndUpLoadClicked()
{
    if(!g_userinfo.m_bLogin) // 没有登录不能上传
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);
        return;
    }

    m_filePathPackedReturn.clear();
    m_fileListUpload.clear();

    saveData();

    bool bOk = showMessageIfEmpty();

    if(!bOk)
    {
        return;
    }

    //
    if(!saveFileWhenExist())
    {
        return;
    }

    if(m_pFileSaveAsDatas)
    {
        m_pFileSaveAsDatas->nSaveType = eSaveTypeSaveToLocalAndUpload;
    }

    //save
    bool bSaveFileOk = false;
    if(m_pFileSaveAsDatas && m_pIUBXmEditor)
    {
        bSaveFileOk = CFilesSaveAsUI::saveFile(m_pIUBXmEditor, *m_pFileSaveAsDatas);

        if(!m_pFileSaveAsDatas->strFilePathMusic.isEmpty())
        {
            m_fileListUpload.append(m_pFileSaveAsDatas->strFilePathMusic);
        }
    }

    if(!bSaveFileOk)
    {
        showMessageUploadResult(false);
        return;
    }

    //save aesx to hts
    const QString strFileDir = m_pFileSaveAsDatas->strSaveFilePath;
    const QString strHtsFileName = CFileDirHandle::getFilePath(strFileDir,
                                                               m_pFileSaveAsDatas->strSaveFileName + HTS_FILE_EXT);

    bool bExportOK = false;

    char *pData = NULL;
    int nLens = 0;
    bool bGetOk = m_pIUBXmEditor->GetData((char**)&pData,nLens);
    if(bGetOk && (0 < nLens))
    {
        char* pHtsData = NULL;
        int nHtsLen = 0;

        if(!CreateHtsDataFromAes(pData, nLens, pHtsData, nHtsLen))
        {

        }

        if(pHtsData)
        {
            bExportOK = CFileDirHandle::saveFile(strHtsFileName, pHtsData, nHtsLen);
        }

        SAFE_DELETE_ARRAY(pHtsData);
    }

    SAFE_DELETE_ARRAY(pData);
    m_fileListUpload.append(strHtsFileName);

    if(!bExportOK)
    {
        showMessageUploadResult(false);
        return;
    }

    //upload file, upload the info
    if(DEFAULT_INVALID_NOINIT_INT_NUMBER >= m_pFileSaveAsDatas->nFileActionID)
    {
        m_pFileSaveAsDatas->nFileActionID = QDateTime::currentMSecsSinceEpoch();
    }

    ActItemData actData;
    actData.m_sActName = m_pFileSaveAsDatas->strSaveFileName;
    actData.m_nActType = eActTypeMusic;
    actData.m_sActDesc = tr("Action info is empty");

    actData.m_nActionOriginalId = m_pFileSaveAsDatas->nFileActionID;//!!!for upload, not m_nItemID
    actData.m_nActSize = CUtil::GetFileSize(m_filePathPackedReturn);

    //hts time
    int nHtsActTotalTime = 0;
    char *pHtsData = NULL;
    qint64 readLen = 0;
    readLen = CFileDirHandle::read(strHtsFileName, &pHtsData);
    if(pHtsData)
    {
        CHtsParser::getActTotalTime(pHtsData, readLen, nHtsActTotalTime);
    }
    SAFE_DELETE(pHtsData);

    actData.m_dwActTimeLen = nHtsActTotalTime/1000;//ms to s

    //copy for package
    bool bCopyOk = true;
    const QString strActionOriginalId = QString::number(actData.m_nActionOriginalId);
    const QString strToDir =  CFileDirHandle::getFilePath(CConfigs::getLocalTempDir(), strActionOriginalId);

    vector<QString> vecFileListToUpload;
    vecFileListToUpload.clear();

    for(int index = 0; (index < m_fileListUpload.count()) && bCopyOk; ++index)
    {
        const QString filePathOld = m_fileListUpload.at(index);
        const QString fileNameNew = strActionOriginalId + "." + CFileDirHandle::getFileSuffix(filePathOld);
        const QString filePathNew = CFileDirHandle::getFilePath(strToDir, fileNameNew);
        bCopyOk &= CFileDirHandle::copyFile(filePathOld, filePathNew, true);

        if(bCopyOk)
        {
            vecFileListToUpload.push_back(filePathNew);
        }
    }

    if(!bCopyOk || (vecFileListToUpload.size() <= 0))
    {
        showMessageUploadResult(false);
        return;
    }

    //package
    QString sZipFilePath;
    sZipFilePath.sprintf("%lld.zip", actData.m_nActionOriginalId);

    sZipFilePath.prepend(CFileDirHandle::getFilePath(strToDir, ""));
    bool bZipRet = CActUpdateMangr::zipFiles(vecFileListToUpload, sZipFilePath);

    if(!bZipRet)
    {
        showMessageUploadResult(false);
        return;
    }

    //
    connect(g_actupdatemgr, &CActUpdateMangr::SigUploadProgress, this, &CFilesSaveAsUI::OnUploadProgress);
    const int nUserId = g_actupdatemgr->GetUserID();
    m_nUploadActZipPercent = 0;

    //上传动作
    if(g_actupdatemgr)
    {
        m_nRequestID = g_actupdatemgr->saveFileActInfoToServer(sZipFilePath, actData, nUserId, m_nRequestID);
    }

    connect(&CTimerManager::getInstance(), &CTimerManager::sigTimeout, this, &CFilesSaveAsUI::onTimerout);
    CTimerManager::getInstance().addTimer(m_nRequestID, 15000);

    m_fileListUpload.clear();
}

void CFilesSaveAsUI::onSaveLocalClicked()
{
    saveData();

    bool bOk = showMessageIfEmpty();

    if(!bOk || !m_pFileSaveAsDatas)
    {
        return;
    }    

    //
    if(!saveFileWhenExist())
    {
        return;
    }

    if(m_pFileSaveAsDatas)
    {
        m_pFileSaveAsDatas->nSaveType = eSaveTypeSaveToLocal;
    }

    //save
    if(m_pFileSaveAsDatas && m_pIUBXmEditor)
    {
        CFilesSaveAsUI::saveFile(m_pIUBXmEditor, *m_pFileSaveAsDatas);
    }

    close();
}

bool CFilesSaveAsUI::saveFile(IUBXMotionEditor *pIUBXmEditor, SFileSaveAsDatas& datas)
{
    if(pIUBXmEditor)
    {
        char *pData = NULL;
        int nLens = 0;
        pIUBXmEditor->GetData(&pData,nLens);

        const QString strSaveFileDir = datas.strSaveFilePath;
        const QString strSaveFileAesx = CConfigs::getFilePath(strSaveFileDir,
                                                             datas.strSaveFileName + UBX_FILE_EXT);

        CFileDirHandle::saveFile(strSaveFileAesx, pData, nLens);
        SAFE_DELETE_ARRAY(pData);

//        if(pIUBXmEditor && pIUBXmEditor->UBXMotionEditor_IsMusicModity())
        if(pIUBXmEditor)
        {
            //copy music to aesx dir
            QString strMusicPath = CConfigs::getLocalMusicDir();

            //copy fiel and rename
            QStringList fileMusicList = CFileDirHandle::getRootFiles(strMusicPath, MUSIC_FILE_EXT);

            if(fileMusicList.count() == 1)
            {
                QString strDir = CFileDirHandle::getFileDirPath(strSaveFileAesx);
                QString strNewName = CFileDirHandle::getFileBaseName(strSaveFileAesx);

                const QString fileMusicOld = fileMusicList.at(0);
                const QString fileMusicNew = CConfigs::getFilePath(strDir,
                                                                   strNewName + MUSIC_FILE_EXT);

                CFileDirHandle::copyFile(fileMusicOld, fileMusicNew, true);

                datas.strFilePathMusic = fileMusicNew;
            }
        }

        return true;
    }

    return false;
}


void CFilesSaveAsUI::onCancelClicked()
{
    if(m_pFileSaveAsDatas)
    {
        *m_pFileSaveAsDatas = m_FileSaveAsDatasOld;
    }

    close();
}

bool CFilesSaveAsUI::showMessageIfEmpty()
{
    const QString strFileName = m_pFileSaveAsDatas->strSaveFileName;
    if(strFileName.isEmpty())
    {
        CMessageBox::warning(this,
                             /*tr("Warning")*/"",
                             tr("File name is empty"));

        return false;
    }

    const QString strFilePath = m_pFileSaveAsDatas->strSaveFilePath;
    if(strFilePath.isEmpty())
    {
        CMessageBox::warning(this,
                             /*tr("Warning")*/"",
                             tr("File path is empty"));

        return false;
    }

    return true;
}

void CFilesSaveAsUI::showMessageUploadResult(bool bSucceed)
{
    if(bSucceed)
    {
        CMessageBox::information(this, "", tr("Upload completed"));
    }

    else if (!bSucceed)
    {
        CMessageBox::warning(this,
                             /*tr("Warning")*/"",
                             tr("File upload failed"));
    }
}

bool CFilesSaveAsUI::saveFileWhenExist()
{
    //
    const QString strSaveFileDir = m_pFileSaveAsDatas->strSaveFilePath;
    const QString strSaveFileAesx = CConfigs::getFilePath(strSaveFileDir,
                                                         m_pFileSaveAsDatas->strSaveFileName + UBX_FILE_EXT);

    if(QFile::exists(strSaveFileAesx))
    {
        QMessageBox::StandardButton standarPress = CMessageBox::warning(this,
                                                                        /*tr("Warning")*/"",
                                                                        tr("File exists"),
                                                                        QMessageBox::Save|QMessageBox::Cancel,
                                                                        QMessageBox::Save);
        if(QMessageBox::Cancel == standarPress)
        {
            return false;//not save
        }
    }

    return true;//too save
}
