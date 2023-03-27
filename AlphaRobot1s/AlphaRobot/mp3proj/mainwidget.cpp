#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QFileDialog>
#include <QResource>

#include "alpharobotlog.h"
#include "ciniconfig.h"

bool CreateMusicLayOptionHandler(ILayerOptionHandler** ppHandler)
{
    MainWidget * pProxy = new MainWidget();
    if(pProxy)
    {
        *ppHandler = pProxy;
        return true;
    }
    return false;
}

MainWidget::MainWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::MainWidget),
    m_pImportMP3Progress(NULL)
{
    ui->setupUi(this);
    m_pMp3Container = new mp3Container(this);
    ui->horizontalLayout_mp3->addWidget(m_pMp3Container);
    m_pEvent = NULL;
    m_pMP3Widget = m_pMp3Container->GetMP3Widget();
    m_bMotionPlaying = false;
    m_bMusicPlaying = false;
    m_bMusicPause = false;
    m_eMotionPlayStatus = ePlayStatusNone;
    connect(this, &MainWidget::SigMotionPlayStop, this, &MainWidget::OnMotionPlayStop);
    connect(m_pMP3Widget, &MP3Widget::SigFinishMusicPlaying, this, &MainWidget::OnFinishMusicPlaying);
    connect(this, &MainWidget::SigSetControlState, this, &MainWidget::OnSetControlState);
    connect(m_pMP3Widget, &MP3Widget::SigNotifyReleasedMusic, this, &MainWidget::OnReleaseMusic);
    connect(m_pMP3Widget, &MP3Widget::SigNotifyLoadedMusic, this, &MainWidget::OnLoadedMusic);

    ui->verticalLayout->setAlignment(ui->btnAddMusic, Qt::AlignHCenter);
    ui->verticalLayout->setAlignment(ui->btnDeleteMusic, Qt::AlignHCenter);
    ui->pauseMP3->hide();
    ui->pauseMotion->hide();
    connect(ui->pauseMP3, &QPushButton::clicked, this, &MainWidget::on_playMP3_clicked);
    connect(ui->pauseMotion, &QPushButton::clicked, this, &MainWidget::on_playMotion_clicked);

    ui->btnDeleteMusic->setEnabled(false);
    setMP3PlayBtnState(false);
    setMotionPlayBtnState(false);
    m_bHasMotionData = false;

    subscribeMsg();

    // 注册，否则信号槽连接不成功
    qRegisterMetaType<emPlayStatus>("emPlayStatus");
}

void MainWidget::subscribeMsg()
{
    SetMsgType Msg;
    Msg.clear();
    Msg.insert(WM_MSG_EXECUTE_REDO);
    Msg.insert(WM_MSG_EXECUTE_UNDO);
    Msg.insert(NOTIFY_MSG_UNITED_PLAY);
    Msg.insert(NOTIFY_MSG_PLAY_MUSIC);
    Msg.insert(NOTIFY_MSG_PLAY_MOTION);
    Msg.insert(NOTIFY_MSG_STOP_PLAY);
    this->AddSubscribe(Msg);
    this->EnableRecvNotify(TRUE);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    // 如果是停止播放消息，由于先停止播放，再播放有顺序要求，需要在同一线程中
    if (nMsgID == NOTIFY_MSG_STOP_PLAY)
    {
        doProcessStopPlayMsg(nMsgID, (emPlayType)lParam);
        return;
    }

    zxEvent* pEvent = new zxEvent(nMsgID);
    pEvent->eEventType = zxEventTypeWindow;
    pEvent->lReceiver = (long)this;
    pEvent->dwMsgId = nMsgID;
    pEvent->dwContext = dwContext;
    pEvent->wParam = wParam;
    pEvent->lParam = lParam;
    QCoreApplication::postEvent(this, pEvent);
}

bool MainWidget::Connect(QWidget *pParent, ILayerOptionHandler_Event *pEvent)
{
    setParent(pParent);
    QLayout* layout = pParent->layout();
    layout->addWidget(this);
    m_pMP3Widget->Connect(pParent, pEvent);
    connect(m_pMP3Widget, &MP3Widget::SigLButtonDoubleClick, this, &MainWidget::OnLButtonDoubleClick);
    m_pEvent = pEvent;

    QString sResDir;
#ifdef Q_OS_MAC
    sResDir = "/../res/mp3proj.rcc";
#else
    sResDir = "/res/mp3proj.rcc";
#endif
    QString rccPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()+ sResDir);
    QResource::registerResource(rccPath);
    setDefaultStyle();
    return true;
}

void MainWidget::ReleaseConnection()
{
    disconnect();
    delete this;
}

QWidget *MainWidget::GetWidget()
{
    return this;
}

void MainWidget::SetActionBlockDataManager(CActionBlockDataManager *pManager, int nCurrentGroupID, int nCurrentActionID)
{
    m_pMP3Widget->SetActionBlockDataManager(pManager, nCurrentGroupID, nCurrentActionID);
}

int MainWidget::GetCurrentSelectCol()
{
    int ret = m_pMP3Widget->GetCurrentSelectCol();
    return ret;
}

void MainWidget::SetTimeScale(int timeScale)
{
    m_pMP3Widget->SetTimeScale(timeScale);
}

void MainWidget::UpdateMP3Action(int nGroupID, float fTime)
{
    m_pMP3Widget->UpdateMP3Action(nGroupID, fTime);
}

void MainWidget::UpdateMP3Action(int nCol)
{
    m_pMP3Widget->UpdateMP3Action(nCol);
}

void MainWidget::HasMotionData(bool hasData)
{
    m_bHasMotionData = hasData;
    if(m_bHasMotionData && !m_bMotionPlaying)
    {
        setMotionPlayBtnState(true);
    }
    else if(!m_bHasMotionData)
    {
        setMotionPlayBtnState(false);
    }
}

void MainWidget::SetCellCount(int nCount)
{
    m_pMP3Widget->SetCellCount(nCount);
}

void MainWidget::MotionPlayStop(int nPlayStatus)
{
    m_eMotionPlayStatus = (emPlayStatus)nPlayStatus;
    emit SigMotionPlayStop(m_eMotionPlayStatus);
}

bool MainWidget::IsMusicLoaded()
{
    return m_pMP3Widget->IsMusicLoaded();
}

void MainWidget::SetMusicResource(QString strFilePath)
{
    if(!strFilePath.isEmpty())
    {
        m_pMP3Widget->LoadMusic(strFilePath);
        emit m_pMP3Widget->SigReDrawAll();
    }
}

void MainWidget::PlayMusic(int nStartTime, int nEndTime,emPlayStatus ePlayState)
{
    if(m_pMP3Widget->IsMusicLoaded())
    {
        if (ePlayState == ePlayStatusPause || ePlayState == ePlayStatusPlaying)
        {
            m_bMusicPlaying = true;
        }
        else // 导入音乐但是音乐播放完成
        {
            m_bMusicPlaying = false;
            if(m_pEvent)
            {
                m_pEvent->ILayerOptinHandler_FinishMusicPlaying();
            }
        }
    }

    //    if(nPlayState == 0)
    //    {
    //        m_pMP3Widget->RestoreCurrentX();
    //    }
    //    else
    //    {
    //        m_pMP3Widget->StoreCurrentX();
    //    }

    emit m_pMP3Widget->SigOnPlayMP3(nStartTime, nEndTime, ePlayState);
    if(ePlayStatusPlaying == ePlayState)//播放
    {
        ui->btnAddMusic->setEnabled(false);
        ui->btnDeleteMusic->setEnabled(false);
        ui->playMotion->setEnabled(false);
        ui->playMP3->setEnabled(false);
        ui->stopMotion->setEnabled(false);
        ui->stopMP3->setEnabled(false);
        m_pMP3Widget->SetPlaying(true);
    }
    else
    {
        ui->btnAddMusic->setEnabled(true);
        if(m_pMP3Widget->IsMusicLoaded())
        {
            ui->btnDeleteMusic->setEnabled(true);
            setMP3PlayBtnState(true);
        }
        if(m_bHasMotionData)
        {
            setMotionPlayBtnState(true);
        }
        m_pMP3Widget->SetPlaying(false);
    }
}
/* LayerManager中的定义
#define ACTION_LAYER_TIME		0
#define ACTION_LAYER_MOTION		1
#define ACTION_LAYER_EYE		2
#define ACTION_LAYER_EAR		3
#define ACTION_LAYER_MUSIC		4
#define MAX_ACTION_LAYER		5
*/
void MainWidget::SetControlState(bool controlState, int nLayerType)
{
    emit SigSetControlState(controlState, nLayerType);
}

void MainWidget::InsertGroupByMenu(int nCol)
{

}

void MainWidget::OnMotionPlayStop(emPlayStatus eStatus)
{
    if(!m_bMusicPlaying)//直接动作仿真结束和全局仿真但音乐没有加载时都会进入，此时需要通知Motion模块更新按钮状态
    {
        //ui->playMotion->setText(tr("Simulate"));
        ui->pauseMotion->hide();
        ui->playMotion->show();
        ui->btnAddMusic->setEnabled(true);

        if(m_pMP3Widget->IsMusicLoaded())
        {
            ui->btnDeleteMusic->setEnabled(true);
            setMP3PlayBtnState(true);
        }
        if(m_bHasMotionData)
        {
            setMotionPlayBtnState(true);
        }

        if (eStatus == ePlayStatusStop || eStatus == ePlayStatusFinished)
        {
            NotifyObservers(NOTIFY_MSG_PLAY_MOTION, NULL, NULL, eStatus);
        }
    }
    m_bMotionPlaying = false;
    m_pMP3Widget->SetSinglePlaying(false);
    m_pMP3Widget->SetPlaying(false);
}

void MainWidget::OnSetControlState(bool controlState, int nLayerType)
{
    if(controlState)
    {
        ui->btnAddMusic->setEnabled(true);
        if(m_pMP3Widget->IsMusicLoaded())
        {
            ui->btnDeleteMusic->setEnabled(true);
            setMP3PlayBtnState(true);
        }
        ui->pauseMotion->hide();
        ui->playMotion->show();
        if(m_bHasMotionData)
        {
            setMotionPlayBtnState(true);
        }
        m_bMotionPlaying = false;
        m_bMusicPlaying = false;
        m_pMP3Widget->SetSinglePlaying(false);
        m_pMP3Widget->SetPlaying(false);
    }
    else
    {
        switch(nLayerType)
        {
        case 1:
        {
            //ui->playMotion->setEnabled(false);
            //ui->stopMotion->setEnabled(false);
            m_bMotionPlaying = true;
            m_pMP3Widget->SetSinglePlaying(true);
            m_pMP3Widget->SetPlaying(false);
            ui->pauseMotion->show();
            ui->playMotion->hide();
        }
            break;
        case 4:
        {
            //ui->playMP3->setEnabled(false);
            //ui->stopMP3->setEnabled(false);
        }
            break;
        case 5:
        {
            ui->btnAddMusic->setEnabled(false);
            ui->btnDeleteMusic->setEnabled(false);
            setMP3PlayBtnState(false);
            setMotionPlayBtnState(false);
            m_pMP3Widget->SetSinglePlaying(false);
            m_pMP3Widget->SetPlaying(true);
            m_bMotionPlaying = true;
        }
            break;
        }
    }
}

void MainWidget::OnReleaseMusic()
{
    if(!m_pMP3Widget->IsMusicLoaded())
    {
        ui->btnDeleteMusic->setEnabled(false);
        setMP3PlayBtnState(false);
    }
}

void MainWidget::OnLoadedMusic(MP3Info stMP3Info)
{
    emit sigOnUpdateMP3Info(stMP3Info);
    if(m_pMP3Widget->IsMusicLoaded())
    {
        ui->btnDeleteMusic->setEnabled(true);
        setMP3PlayBtnState(true);
    }
}

void MainWidget::OnFinishMusicPlaying()
{
    m_bMusicPlaying = false;
    ui->btnAddMusic->setEnabled(true);
    ui->pauseMP3->hide();
    ui->playMP3->show();
    if(m_pMP3Widget->IsMusicLoaded())
    {
        ui->btnDeleteMusic->setEnabled(true);
        setMP3PlayBtnState(true);
    }

    // 如果只是单独音乐仿真，音乐播放结束后自动回到开始位置
    if (!m_bMotionPlaying)
    {
        on_stopMP3_clicked();
    }

    if(m_bHasMotionData && !m_bMotionPlaying)
    {
        setMotionPlayBtnState(true);
    }
    if(m_pEvent)
    {
        m_pEvent->ILayerOptinHandler_FinishMusicPlaying();
    }

    NotifyObservers(NOTIFY_MSG_PLAY_MUSIC, NULL, NULL, ePlayStatusStop);
}

void MainWidget::OnLButtonDoubleClick(int nCol)
{
    if(m_pEvent)
    {
        m_pEvent->ILayerOptinHandler_OnLButtonDBClick(nCol);
        //ui->playMotion->setEnabled(false);
        //ui->stopMotion->setEnabled(false);
    }
}

void MainWidget::on_btnAddMusic_clicked()
{
    QString sFmtTitle = tr("Music File"); //格式名称
    QString sFmtExt = MUSIC_FILE_EXT;  //格式后缀
    QString sFmt;

    QString strLastPath = getLastMusicPath();
#ifdef Q_OS_MAC
    sFmt.sprintf("%s(*%s);;%s(*%s)", sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str(), sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str());
#else
    sFmt.sprintf("%s(*%s)", sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str());
#endif
    QString sMusicFile = QFileDialog::getOpenFileName(NULL, tr("Choose a music file"), strLastPath, sFmt);
    if(!sMusicFile.isEmpty())
    {
        // 保存路径记录
        on_stopMP3_clicked();//导入文件前，先完全停止音乐播放
        saveLastMusicPath(sMusicFile);

        // 记录导入音乐文件前的文件
        IOperation *pHandle = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleLoadMusicFile);
        m_pMP3Widget->cloneMusicFileBeforeChanged(pHandle);

        sMusicFile = QDir::toNativeSeparators(sMusicFile);
        m_pMP3Widget->LoadMusic(sMusicFile);

//        // 弹出音乐文件加载进度
//        SAFE_DELETE(m_pImportMP3Progress);
//        m_pImportMP3Progress = new CImportMP3Progress(this);
//        connect(this, &MainWidget::sigOnUpdateMP3Info, m_pImportMP3Progress, &CImportMP3Progress::slotOnUpdateMP3Info);
//        m_pImportMP3Progress->exec();

        emit m_pMP3Widget->SigReDrawAll();

        // 记录导入音乐后的文件
        m_pMP3Widget->cloneMusicFileAfterChanged(pHandle);
    }
}

void MainWidget::on_playMP3_clicked()
{
    //m_pMP3Widget->StoreCurrentX();
    // 如果当前不是暂停播放，则先停止上次播放的内容
    emPlayType ePlayType = ePlayTypeNone;
    if (m_pEvent)
    {
        ePlayType= m_pEvent->ILayerOptionHandler_GetPlayType();
        if (ePlayType != ePlayTypeMusic)
        {
            NotifyObservers(NOTIFY_MSG_STOP_PLAY, NULL, NULL, ePlayType);
        }
    }

    if(!m_bMusicPlaying && m_pMP3Widget->IsMusicLoaded())
    {
        if(m_bMusicPause)
        {
            m_pMP3Widget->OnResumeMP3();
            m_bMusicPause = false;
        }
        else
        {
            m_pMP3Widget->OnPlayMP3FromCur();
        }
        m_bMusicPlaying = true;
        //ui->playMP3->setText(tr("Pause"));
        ui->pauseMP3->show();
        ui->playMP3->hide();
        ui->btnAddMusic->setEnabled(false);
        ui->btnDeleteMusic->setEnabled(false);

        NotifyObservers(NOTIFY_MSG_PLAY_MUSIC, NULL, NULL, ePlayStatusPlaying);
    }
    else if(m_bMusicPlaying)
    {
        m_pMP3Widget->OnPauseMP3();
        m_bMusicPause = true;
        m_bMusicPlaying = false;
        //ui->playMP3->setText(tr("Simulate"));
        ui->pauseMP3->hide();
        ui->playMP3->show();
        ui->btnAddMusic->setEnabled(true);
        if(m_pMP3Widget->IsMusicLoaded())
            ui->btnDeleteMusic->setEnabled(true);

        NotifyObservers(NOTIFY_MSG_PLAY_MUSIC, NULL, NULL, ePlayStatusPause);
    }
}

void MainWidget::on_stopMP3_clicked()
{
    m_pMP3Widget->OnStopMP3();

//    if(m_bMusicPlaying || m_bMusicPause)//stop must have to reset status, bug of 4717
    {
        //m_pMP3Widget->UpdateMP3Action(0);
        ui->btnAddMusic->setEnabled(true);
        if(m_pMP3Widget->IsMusicLoaded())
        {
            ui->btnDeleteMusic->setEnabled(true);
        }
    }

    //m_pMP3Widget->RestoreCurrentX();
    m_bMusicPlaying = false;
    m_bMusicPause = false;
    //ui->playMP3->setText(tr("Simulate"));
    ui->pauseMP3->hide();
    ui->playMP3->show();
    if(m_pEvent)
    {
        m_pEvent->ILayerOptinHandler_FinishMusicPlaying();
    }

    NotifyObservers(NOTIFY_MSG_PLAY_MUSIC, NULL, NULL, ePlayStatusStop);
}

void MainWidget::on_playMotion_clicked()
{
    if(!m_pEvent)
    {
        return;
    }

    // 如果当前不是暂停播放，则先停止上次播放的内容
    emPlayType ePlayType = m_pEvent->ILayerOptionHandler_GetPlayType();
    if (ePlayType != ePlayTypeMotion)
    {
        NotifyObservers(NOTIFY_MSG_STOP_PLAY, NULL, NULL, ePlayType);
    }

    if(m_bMotionPlaying)
    {
        m_pEvent->ILayerOptionHanlder_LayerPause();
        //ui->playMotion->setText(tr("Simulate"));
        ui->pauseMotion->hide();
        ui->playMotion->show();
        m_bMotionPlaying = false;
        NotifyObservers(NOTIFY_MSG_PLAY_MOTION, NULL, NULL, ePlayStatusPause);
    }
    else
    {
        // 如果不是暂停播放，先停止播放
        if (m_eMotionPlayStatus != ePlayStatusPause)
        {
            on_stopMotion_clicked();
        }

        m_pEvent->ILayerOptionHanlder_LayerPause();
        m_pEvent->ILayerOptionHanlder_LayerPlaying();
        //ui->playMotion->setText(tr("Pause"));
        ui->pauseMotion->show();
        ui->playMotion->hide();
        m_bMotionPlaying = true;
        NotifyObservers(NOTIFY_MSG_PLAY_MOTION, NULL, NULL, ePlayStatusPlaying);
    }
    m_pMP3Widget->SetPlaying(m_bMotionPlaying);
}

void MainWidget::on_stopMotion_clicked()
{
    if(m_pEvent)
    {
        m_pEvent->ILayerOptionHanlder_OnStopPlay();
        //ui->playMotion->setText(tr("Simulate"));
        ui->pauseMotion->hide();
        ui->playMotion->show();
        m_bMotionPlaying = false;
        m_pMP3Widget->SetPlaying(m_bMotionPlaying);
    }
    NotifyObservers(NOTIFY_MSG_PLAY_MOTION, NULL, NULL, ePlayStatusStop);
}

void MainWidget::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/mp3projdefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

void MainWidget::saveLastMusicPath(const QString &strMusicFile)
{
    QFileInfo fileInfo(strMusicFile);
    CIniConfig::getInstance().setPath(eFilePathImportMusic, fileInfo.path());
}

QString MainWidget::getLastMusicPath()
{
    QString strFilePath;
    CIniConfig::getInstance().getPath(eFilePathImportMusic, strFilePath);
    QDir dir(strFilePath);
    if (strFilePath.isEmpty() || !dir.exists())
    {
        strFilePath = QDir::currentPath();
    }

    return strFilePath;
}

void MainWidget::setMP3PlayBtnState(bool bEnable)
{
    ui->playMP3->setEnabled(bEnable);
    ui->pauseMP3->setEnabled(bEnable);
    ui->stopMP3->setEnabled(bEnable);
}

void MainWidget::setMotionPlayBtnState(bool bEnable)
{
    ui->playMotion->setEnabled(bEnable);
    ui->playMotion->setEnabled(bEnable);
    ui->stopMotion->setEnabled(bEnable);
}

void MainWidget::on_btnDeleteMusic_clicked()
{
    on_stopMP3_clicked();

    if(m_pMP3Widget)
    {
        // 历史数据保存之所以没有放到m_pMP3Widget中，是为了保证在用户操作时才触发进行数据存储
        // 避免内部程序其他地方调用删除音乐时不必要的数据保存
        IOperation *pHandle = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleLoadMusicFile);
        m_pMP3Widget->cloneMusicFileBeforeChanged(pHandle);

        m_pMP3Widget->OnDeleteSelectedMusic();

        m_pMP3Widget->cloneMusicFileAfterChanged(pHandle);
    }
}

void MainWidget::customEvent(QEvent *event)
{
    zxEvent* pEvent = (zxEvent*)event;
    DWORD nMsgID = pEvent->dwMsgId;
    DWORD nOperateCode = pEvent->dwContext;
    WPARAM wParam = pEvent->wParam;
    LPARAM lParam = pEvent->lParam;

    if (nMsgID == WM_MSG_EXECUTE_REDO || nMsgID == WM_MSG_EXECUTE_UNDO)
    {
        doProcessUndoRedoMsg((emOperateCode)nOperateCode, wParam, lParam);
    }
    else if (nMsgID == NOTIFY_MSG_PLAY_MOTION || NOTIFY_MSG_PLAY_MUSIC || nMsgID == NOTIFY_MSG_UNITED_PLAY)
    {
        doProcessSimulateMsg(nMsgID,  (emPlayStatus)lParam);
    }
}

void MainWidget::retranslateUi()
{
    ui->retranslateUi(this);
}

void MainWidget::doProcessUndoRedoMsg(emOperateCode eCode, WPARAM wParam, LPARAM lParam)
{
    if (eCode == eCodeHandleMarkLine)
    {
        CMarkLineContainer *pContainer = (CMarkLineContainer *) lParam;
        m_pMP3Widget->updateMarkLines(pContainer);
    }
    else if (eCode == eCodeHandleLoadMusicFile)
    {
        QString strMP3File = QString::fromUtf8((char*)lParam);
        if (strMP3File.isEmpty() && m_pMP3Widget)
        {
            m_pMP3Widget->OnDeleteSelectedMusic();
        }
        else
        {
            SetMusicResource(strMP3File);
        }
    }
}

void MainWidget::doProcessSimulateMsg(int nMsg, emPlayStatus eStatus)
{
    switch (eStatus) {
    case ePlayStatusPlaying:
    {
        if (nMsg == NOTIFY_MSG_PLAY_MUSIC)
        {
            setMotionPlayBtnState(false);
        }
        else if (nMsg == NOTIFY_MSG_PLAY_MOTION)
        {
            setMP3PlayBtnState(false);
        }
        else if (nMsg == NOTIFY_MSG_UNITED_PLAY)
        {
            setMP3PlayBtnState(false);
            setMotionPlayBtnState(false);
        }
        break;
    }
    case ePlayStatusPause:
    case ePlayStatusStop:
    case ePlayStatusFinished:
    {
        if (nMsg == NOTIFY_MSG_PLAY_MUSIC)
        {
            setMotionPlayBtnState(m_bHasMotionData);
        }
        else if (nMsg == NOTIFY_MSG_PLAY_MOTION)
        {
            setMP3PlayBtnState(IsMusicLoaded());
        }
        else if (nMsg == NOTIFY_MSG_UNITED_PLAY)
        {
            setMP3PlayBtnState(IsMusicLoaded());
            setMotionPlayBtnState(m_bHasMotionData);
        }
        break;
    }
    default:
        break;
    }
}

void MainWidget::doProcessStopPlayMsg(int nMsgID, emPlayType eType)
{
    if (eType == ePlayTypeMusic)
    {
        on_stopMP3_clicked();
    }
    else if (eType == ePlayTypeMotion)
    {
        on_stopMotion_clicked();
    }
}
