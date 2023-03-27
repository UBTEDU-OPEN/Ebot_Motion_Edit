#include "mainwindowwidget.h"
#include "ui_mainwindowwidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QLibrary>
#include <QStandardPaths>
#include <QDesktopServices>


#include "ubxrobot3dcontainer.h"
#include "LocalActSyncWidget.h"
#include "dialogabout.h"
#include "loginlibmgr.h"
#include "ActUpdateMangr.h"
#include "UpdateProgressDlg.h"
#include "util.h"
#include "alpharobotlog.h"
#include "../zxLib/widget/ubxwidget.h"
#include "../zxLib/configs.h"
#include "../common/filedirhandle.h"
#include "filessaveasui.h"
#include "../zxLib/configs.h"
#include "../zxLib/widget/menu.h"
#include "../zxLib/widget/messagebox.h"
#include "UBXPublic.h"
#include "ubxundoredomanager.h"
#include "ciniconfig.h"
#include "../Common/cactmixtureparse.h"
#include "cjsonutil.h"

#include "csystemupdatehttphandle.h"
#include "../zxLib/widget/cdialogcontainer.h"
#include "ubtlib.h"


//9宫格相关数据
//const int BORDER_LEFT   = 5;
//const int BORDER_TOP    = 5;
//const int BORDER_RIGHT  = 5;
//const int BORDER_BOTTOM = 5;
//const int BORDER_WIDTH  = 5;
//const int BORDER_HEIGHT = 5;

//四个角落触发resize的区域
const int BORDER_PADDING = 2;

const int BORDER_RADIUS = 6;

const int DEFAULT_UPDATE_DOWN_LOAD_TIMER_ID = -1;

MainWindowWidget * MainWindowWidget::m_pMainWindow = NULL;

MainWindowWidget* MainWindowWidget::instance()
{
    if (!m_pMainWindow)
    {
        m_pMainWindow = new MainWindowWidget();
    }

    return m_pMainWindow;

    // huangwuxian修改 2016.09.12
    // 如果使用静态局部变量方式，在工程中添加QMediaplayer/QVideoWidget后
    // 关闭app时会出现程序崩溃现象
//    static MainWindowWidget mainWindow;
//    return &mainWindow;
}

MainWindowWidget::MainWindowWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::MainWindowWidget),
    m_pITreeNavigation(NULL),
    m_pTabWidget(NULL),
    m_pIUBXmEditor(NULL),
    m_pIUBX3DView(NULL),
    m_pTitleBar(NULL),
    m_pSettingDialog(NULL),
    m_pActionCheckFirmwareVersion(NULL),
    m_pActionRobotPCSoftVersion(NULL),
    m_pPromotePopUpWidget(NULL),
    m_pRobotControlWidget(NULL),
    m_bProjectTitleTabTextHadSet(false)
{
    ui->setupUi(this);

    LoadConfig();

    //初始化主窗体
    setMouseTracking(true);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

#ifdef Q_OS_MAC
    setAttribute(Qt::WA_TranslucentBackground, true);
#endif

    QRect rect = QApplication::desktop()->availableGeometry();
    this->setGeometry(rect);
    m_bytearrGeometry = saveGeometry();

    setWindowState(Qt::WindowMaximized);

    m_pMenuBar = NULL;
    m_pTitleBar = NULL;
    m_resizeType = UnSet;
    m_bLeftDown = false;
    m_bRobotCtrlWidgetHasFirstShow = false;

    m_pActionLibWidget = NULL;
    m_pActionListPage = NULL;
    m_pActionDownload = NULL;
    m_pActionListCurWidget = NULL;
    m_pFavoritesPage = NULL;
    m_pMyUploadPage = NULL;
    m_pActionDetail = NULL;
    m_pActionRecommendPage = NULL;
    m_pBusinessPromoteWidget = NULL;
    m_nIdForActionWidget = 0;

    m_FileMenu = NULL;
    m_newAct = NULL;
    m_aboutAct = NULL;
    m_openAct = NULL;
    m_saveAct = NULL;
    m_saveAsAct = NULL;
    m_exportHts = NULL;

    m_pSettingAction = NULL;

    m_pHelpMenu = NULL;
    m_pOpenHelpDocAction = NULL;
    m_pOpenVideoTutorialAction = NULL;

    m_pILocalActSyncWidget = NULL;
    m_pLoginDlg = NULL;
    m_pActionUploadAddAct = NULL;
    m_pActionUploadMain = NULL;

    //初始化菜单栏
    InitMenuBar();

    //初始化标题栏
    InitTitleBar();

    // 初始化机器人控制按钮
    InitRobotControlWidget();

    //初始化编辑界面
    InitEditPage();

    //初始化动作库界面
    InitActionLibrayPage();

    //初始化同步界面
    InitSyncPage();

    //设置默认style sheet样式
    SetDefaultStyle();    
    m_strBntDownLoadsOldStyle = ui->pbDownLoads->styleSheet();

    ui->tabWidget->setCurrentIndex(0);//默认显示编辑界面
    ui->tabWidget->tabBar()->hide();

    ui->pbHomePage->setChecked(true);//动作库界面默认显示首页

    this->setFocusPolicy(Qt::ClickFocus);

    //下载列表和同步界面之间的信号槽连接
    connect(m_pActionDownload, &DownloadWidget::SigAddToSyncList, (LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(), &LocalActSyncWidget::OnSyncingAction);
    connect((LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(), &LocalActSyncWidget::SigAddedAction, m_pActionDownload, &DownloadWidget::OnAddedAction);
    connect((LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(), &LocalActSyncWidget::SigRemoveAction, m_pActionDownload, &DownloadWidget::OnRemoveAction);

    QObject::connect((LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(),
                     &LocalActSyncWidget::PortOpenChanged,
                     this,
                     &MainWindowWidget::sigOnPortOpenChanged);
    QObject::connect(((UBXRobot3DContainer*)m_pIUBX3DView->GetWidget())->GetCtrlWidget(),
                     &UBXRobot3DControlWidget::PortOpenChanged,
                     this,
                     &MainWindowWidget::sigOnPortOpenChanged);

    QObject::connect(this,
                     &MainWindowWidget::sigOnPortOpenChanged,
                     m_pActionDownload,
                     &DownloadWidget::OnPortOpenChanged);

    connect(this, &MainWindowWidget::sigOnPortOpenChanged, this, &MainWindowWidget::slotOnPortStatusChanged);

    connect(this, &MainWindowWidget::SigFullScreen, this, &MainWindowWidget::showFullScreen);
    connect(this, &MainWindowWidget::SigClose, this, &MainWindowWidget::close);
    connect(this, &MainWindowWidget::SigNormal, this, &MainWindowWidget::showNormal);

    //g_actupdatemgr->ActUserReg("20160308", "123456", 1);

    //获取版本号
    FILEVERINFO fileinfo;
    CUtil::GetFileVersion(QApplication::applicationFilePath(), fileinfo);

    g_exefileinfo = fileinfo;
//    m_strSaveFileName = "";

    subscribeMsg();
    
    connect(&m_timer, &QTimer::timeout, this, &MainWindowWidget::onRestoreDownLoadBtn);
//    QTimer::singleShot(1000, this, &MainWindowWidget::onUserLogin);  // 不用自动登录

   //
    UBXUpdateAdministrator::getInstance()->setUpdateBasicUrl(g_sUpdateURL);

    // 暂时不显示版本更新前的图标，后续所有图标一起添加
    //QTimer::singleShot(1000, this, &MainWindowWidget::onRefreshVersionIcon);
   // connect(UBXUpdateAdministrator::getInstance(),
//            &UBXUpdateAdministrator::sigSystemEmbedVersion,
//            this, &MainWindowWidget::onRefreshSystemEmbedVersionIcon);


    //m_saveAsDatas.strSaveFilePath = CConfigs::getLocalDownloadDir();
    m_saveAsDatas.strSaveFilePath = getOpenProjectPath();

    onRefreshVideoTutorialAlert();

    QTimer::singleShot(0, this, &MainWindowWidget::slotOnMainWinShow);
    QTimer::singleShot(DELAY_LOADED_TIME, this, &MainWindowWidget::slotOnDelayLoaded);

    // 针对全局控制按钮，采用alwaysStackOnTop方式，mac需要特殊处理
#ifdef Q_OS_OSX
    connect((QApplication *)(QApplication::instance()), &QApplication::applicationStateChanged, this, &MainWindowWidget::slotOnAppStateChanged);
#endif

    connect(UBXBase::getInstance(), &UBXBase::sigRobotProductTypeChange, this, &MainWindowWidget::onRobotProductTypeChange);
}

MainWindowWidget::~MainWindowWidget()
{    
    QString strTempPath;

    //清理Music目录
    strTempPath = CConfigs::getLocalMusicDir();
    CFileDirHandle::clearDir(strTempPath, false);

    //清理Wave目录
    strTempPath =  CConfigs::getLocalWaveDir();
    CFileDirHandle::clearDir(strTempPath, false);

    //清理Temp目录
    strTempPath = CConfigs::getLocalTempDir();
    CFileDirHandle::clearDir(strTempPath, false);

    QString sPreviewImagePath = CConfigs::getLocalPreviewImgDir();
    CFileDirHandle::clearDir(sPreviewImagePath, false);

    QString sRecommendImagePath = CConfigs::getLocalRecommendImgDir();
    CFileDirHandle::clearDir(sRecommendImagePath, false);

    if(m_pIUBXmEditor)
    {
        m_pIUBXmEditor->Disconnect();
        m_pIUBXmEditor = NULL;
    }

    SAFE_DELETE(m_pActionLibWidget);
    SAFE_DELETE(m_pActionListPage);
    SAFE_DELETE(m_pActionRecommendPage);
    SAFE_DELETE(m_pFavoritesPage);
    SAFE_DELETE(m_pMyUploadPage);

    delete ui;
}

void MainWindowWidget::IUBXRobot3DViewEvent_On3DModelLoaded()
{

}

bool MainWindowWidget::IUBXRobot3DViewEvent_IsRobotDebuging()
{
    return false;
}

/**************************************************************************
* 函数名: CheckFile
* 功能: 检查文件
* 参数:
*    @[in out] sFilePath: 检查的文件路径
* 返回值: 成功返回TRUE，失败返回FALSE
* 时间: 2015/08/25 16:12
* 作者: ZDJ
*/
BOOL MainWindowWidget::CheckFile(QString& sFilePath)
{
    BOOL bRet = FALSE;
    char* pDataBuf = NULL;//文件数据
    int nDataLen = 0;//数据长度

    do
    {
        QString sExt = sFilePath.mid(sFilePath.lastIndexOf('.'));//后缀名
        if(sExt.compare(UBX_FILE_EXT) == 0 || sExt.compare(UBX_FILE_EXT_OLD) == 0)
        {
            QFile file(sFilePath);
            if(!file.open(QIODevice::ReadOnly))
            {
                //AfxMessageBox(language_loadstring(IDS_SURE_FILEOPEN_FAILED));
                CMessageBox::warning(this, "", tr("open file failed"));
                break;
            }

            nDataLen = file.size();
            pDataBuf = new char[nDataLen];
            file.read(pDataBuf, nDataLen);
            file.close();
        }
        bRet = TRUE;
    } while (0);

    SAFE_DELETE_ARRAY(pDataBuf);
    return bRet;
}

void MainWindowWidget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    drawBackGround(painter);
    drawBorder(painter);
}

void MainWindowWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
#ifdef Q_OS_MAC
    if (m_pRobotControlWidget && m_bRobotCtrlWidgetHasFirstShow)
    {
        m_pRobotControlWidget->show();
    }
#endif
}

void MainWindowWidget::hideEvent(QHideEvent *event)
{
#if 0  // 由于隐藏比较慢，移到onMin中
#ifdef Q_OS_MAC
    if (m_pRobotControlWidget)
    {
        m_pRobotControlWidget->hide();
    }
#endif
#endif
    QWidget::hideEvent(event);
}

void MainWindowWidget::closeEvent(QCloseEvent *event)
{
    bool isProjectModify = false;
    bool bIsMusicModify = false;
    if(m_pIUBXmEditor)
    {
        m_pIUBXmEditor->UBXMotionEditor_StopDebug();
        isProjectModify = m_pIUBXmEditor->UBXMotionEditor_IsProjectModity();
        bIsMusicModify = m_pIUBXmEditor->UBXMotionEditor_IsMusicModity();
    }

    if(isProjectModify || bIsMusicModify)
    {
        QMessageBox::StandardButton retValue = CMessageBox::question(this, "", tr("The project has been modified, would you like to save it?"),
                                             QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
                                             QMessageBox::Yes);
        if (QMessageBox::Cancel == retValue)
        {
            event->ignore();
            return;
        }
        else if(QMessageBox::Yes == retValue)
        {
            slotSave();
        }
    }

    if(m_pIUBXmEditor)
    {
        m_pIUBXmEditor->UBXMotionEditor_SetProjectModity(false);
        m_pIUBXmEditor->UBXMotionEditor_SetMusicModity(false);
    }

    // 退出时，需要关闭下载线程
    UBXUpdateAdministrator::getInstance()->onExit();

    event->accept();
    QWidget::closeEvent(event);
}

void MainWindowWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_ptPre = e->globalPos();
        m_bLeftDown = true;
    }
    else
    {
        m_bLeftDown = false;
    }
    return QWidget::mousePressEvent(e);
}

void MainWindowWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(!(windowState() & Qt::WindowMaximized))
    {
        if(m_bLeftDown && m_resizeType != UnSet)
        {
            changeSize(e->globalPos());
            e->accept();
        }
        else if(m_bLeftDown && m_resizeType == UnSet && m_pTitleBar->rect().contains(mapFromGlobal(m_ptPre)))
        {
            QPoint nowPt = e->globalPos();
            QPoint moveTo = mapToParent(nowPt - m_ptPre);
            move(moveTo);
            m_ptPre = nowPt;
            m_bytearrGeometry = saveGeometry();
        }
        //button()Note that the returned value is always Qt::NoButton for mouse move events.
        else if(!(e->buttons() & Qt::LeftButton))
        {
            changeCursor(e->pos());
        }
    }
    QWidget::mouseMoveEvent(e);
}

void MainWindowWidget::mouseReleaseEvent(QMouseEvent *e)
{
    m_resizeType = UnSet;
    //setCursor(QCursor(Qt::ArrowCursor));
    m_bLeftDown = false;
    QWidget::mouseReleaseEvent(e);
}

void MainWindowWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
}

void MainWindowWidget::keyPressEvent(QKeyEvent *event)
{
    if (event)
    {
#ifdef Q_OS_WIN
        if ((event->key() == Qt::Key_Z) && (event->modifiers() & Qt::ControlModifier))
        {
            slotOnUndo();
            return;
        }
        if ((event->key() == Qt::Key_Y) && (event->modifiers() & Qt::ControlModifier))
        {
            slotOnRedo();
            return;
        }
#elif defined(Q_OS_MAC)
        if ((event->key() == Qt::Key_Z) && (event->modifiers() & Qt::ControlModifier & Qt::ShiftModifier))
        {
            slotOnUndo();
            return;
        }
        if ((event->key() == Qt::Key_Z) && (event->modifiers() & Qt::ControlModifier & Qt::ShiftModifier))
        {
            slotOnRedo();
            return;
        }
#endif
    }
    QWidget::keyPressEvent(event);
}

void MainWindowWidget::InitMenuBar()
{
#ifdef Q_OS_OSX
    m_pMenuBar = new QMenuBar(this);
    m_pMenuBar->setObjectName(QStringLiteral("menuBar"));
    m_pMenuBar->setGeometry(0,0,200,20);
    m_pMenuBar->show();
    m_pMenuBar->setNativeMenuBar(true);
    QMenu* menuAbout = new QMenu(m_pMenuBar);
    menuAbout->setObjectName(QStringLiteral("menuAbout"));
    ui->verticalLayout_3->setMenuBar(m_pMenuBar);

    m_pMenuBar->addAction(menuAbout->menuAction());
    createFileMenu();
    createHelpMenu();
#endif
}

void MainWindowWidget::InitTitleBar()
{
    m_pTitleBar = new TitleBar(this);
    connect(m_pTitleBar, &TitleBar::clickTab, this, &MainWindowWidget::onClickTab);
    connect(m_pTitleBar, &TitleBar::SigClose, this, &MainWindowWidget::onClose);
    connect(m_pTitleBar, &TitleBar::SigMin, this, &MainWindowWidget::onMin);
    connect(m_pTitleBar, &TitleBar::SigMax, this, &MainWindowWidget::onMax);
    connect(m_pTitleBar, &TitleBar::SigLogin, this, &MainWindowWidget::onUserLogin);
    connect(m_pTitleBar, &TitleBar::SigRestore, this, &MainWindowWidget::onRestore);
    connect(m_pTitleBar, &TitleBar::sigAbout, this, &MainWindowWidget::slotAbout);
    connect(m_pTitleBar, &TitleBar::sigOnDoubleClicked, this, &MainWindowWidget::slotOnDoubleClicked);
    ui->verticalLayout_3->insertWidget(0, m_pTitleBar);

    connect(this, &MainWindowWidget::SigMax, m_pTitleBar, &TitleBar::onMax);
    connect(this, &MainWindowWidget::SigRestore, m_pTitleBar, &TitleBar::onRestore);
    connect(this, &MainWindowWidget::SigTextChange, m_pTitleBar, &TitleBar::onTextChange);

#ifndef Q_OS_OSX
    connect(m_pTitleBar, &TitleBar::sigProjectNew, this, &MainWindowWidget::slotNew);
    connect(m_pTitleBar, &TitleBar::sigProjectOpen, this, &MainWindowWidget::slotOpen);
    connect(m_pTitleBar, &TitleBar::sigProjectSave, this, &MainWindowWidget::slotSave);
    connect(m_pTitleBar, &TitleBar::sigProjectSaveAs, this, &MainWindowWidget::slotSaveAs);
    connect(m_pTitleBar, &TitleBar::sigExportHts, this, &MainWindowWidget::slotExportToHts);
    connect(m_pTitleBar, &TitleBar::sigExportActMixXml, this, &MainWindowWidget::slotExportToXml);
    connect(m_pTitleBar, &TitleBar::sigOpenHelpDoc, this, &MainWindowWidget::slotOnOpenHelpDoc);
    connect(m_pTitleBar, &TitleBar::sigOpenVideoTutorial, this, &MainWindowWidget::slotOnVideoTutorial);
    connect(m_pTitleBar, &TitleBar::sigCheckUpdateSysEmbed, this, &MainWindowWidget::slotOnCheckUpdateSysEmbedVersion);
    connect(m_pTitleBar, &TitleBar::sigPCSoftVersion, this, &MainWindowWidget::slotOnShowSystemPCSoftVersion);
    connect(m_pTitleBar, &TitleBar::sigOnSetting, this, &MainWindowWidget::slotOnSetting);
#endif
}

void MainWindowWidget::InitEditPage()
{
    if(CreateUBXRobot3DView(&m_pIUBX3DView))
    {
        m_pIUBX3DView->Connect(this, this);

        connect(m_pRobotControlWidget,
                &CRobotControlWidget::sigResetRobot,
                ((UBXRobot3DContainer*)m_pIUBX3DView->GetWidget())->GetCtrlWidget(),
                &UBXRobot3DControlWidget::onResetGesture);

        connect(m_pRobotControlWidget,
                &CRobotControlWidget::sigRobotPowerOff,
                ((UBXRobot3DContainer*)m_pIUBX3DView->GetWidget())->GetCtrlWidget(),
                &UBXRobot3DControlWidget::slotOnRobotPowerOff);
    }

    if(CreateUBXMotionEditor(&m_pIUBXmEditor))
    {
        m_pIUBXmEditor->Connect(this);
        m_pIUBXmEditor->UBXMotionEditor_Set3DRobotViewInterface(m_pIUBX3DView);
    }
    if(m_pIUBXmEditor)
    {
        m_pIUBXmEditor->SetData(NULL,0);
    }
    if(CreateTreeNavigation(&m_pITreeNavigation))
    {
        m_pITreeNavigation->connect(this/*, this*/);
        if(m_pIUBXmEditor)
        {
            m_pIUBXmEditor->UBXMotionEditor_SetTreeView(m_pITreeNavigation);
        }
    }

    //hels 布局界面
    if(m_pIUBX3DView && m_pIUBXmEditor)
    {        
        //m_pITreeNavigation->GetWidget()->setMinimumWidth(WIDGET_LAYOUT_UBX_TREEWIDGET_MIN_WIDTH);
        //m_pIUBX3DView->GetWidget()->setMinimumWidth(WIDGET_LAYOUT_UBX_ROBOT_3D_CONTAINER_MIN_WIDTH);

        ui->mainHL->insertWidget(eEditPageIndexTreeNavi, m_pITreeNavigation->GetWidget(), STRETCH_FATOR_TREE_NAV);
        ui->mainHL->insertWidget(eEditPageIndexActionEdit, m_pIUBXmEditor->GetWidget(), STRETCH_FATOR_ACTION_EDIT);
        ui->mainHL->insertWidget(eEditPageIndex3DView, m_pIUBX3DView->GetWidget(), STRETCH_FATOR_3D_VIEW);
    }

    connect(ui->pbUndo, &QPushButton::clicked, this, &MainWindowWidget::slotOnUndo);
    connect(ui->pbRedo, &QPushButton::clicked, this, &MainWindowWidget::slotOnRedo);
    connect(UBXUndoRedoManager::getInstance(), &UBXUndoRedoManager::sigUndoListSizeChanged, this, &MainWindowWidget::slotOnUndoListSizeChanged);
    connect(UBXUndoRedoManager::getInstance(), &UBXUndoRedoManager::sigRedoListSizeChanged, this, &MainWindowWidget::slotOnRedoListSizeChanged);

    setUndoBtnEnable(false);
    setRedoBtnEnable(false);
}

void MainWindowWidget::InitActionLibrayPage()
{
    m_btnGroupForActionLib.addButton(ui->pbHomePage, BUTTON_HOME_PAGE);//首页
    m_btnGroupForActionLib.addButton(ui->pbActions, BUTTON_ACTIONS_LIBRARY);//动作页
    m_btnGroupForActionLib.addButton(ui->pbFavorites, BUTTON_FAVORITES);//我的收藏页
    m_btnGroupForActionLib.addButton(ui->pbMyUpload, BUTTON_MYUPLOADS);

    ui->pbFavorites->hide();//未登录之前不可用
    ui->pbMyUpload->hide();
//    ui->pbDownLoads->hide();
//    ui->pushButton_actupload->hide();

    //m_btnGroupForActionLib.addButton(ui->pbDownLoads, 3);//下载列表
    connect(ui->pbDownLoads, &QPushButton::clicked, this, &MainWindowWidget::OnDownloadBtnClicked);

    connect(&m_btnGroupForActionLib, static_cast<void (QButtonGroup:: *)(int, bool)>(&QButtonGroup::buttonToggled),
            this, &MainWindowWidget::OnActionLibBGToggled);
    //初始化
    m_pActionLibWidget = new ActionLibWidget(); // 传入this会导致homePage进入动作详情页后返回时hover无效
    connect(m_pActionLibWidget, &ActionLibWidget::SigMoreBtnClicked, this, &MainWindowWidget::OnMoreBtnClicked);
    connect(m_pActionLibWidget, &ActionLibWidget::SigActionPreviewClicked, this, &MainWindowWidget::OnActionPreviewClicked);
    connect(m_pActionLibWidget, &ActionLibWidget::sigRecommendImageClicked, this, &MainWindowWidget::slotOnRecommendFlowImageClicked);
    connect(m_pActionLibWidget, &ActionLibWidget::sigBusinessPromoteImageClicked, this, &MainWindowWidget::slotOnBusinessPromoteImageClicked);
    connect(m_pActionLibWidget, &ActionLibWidget::sigOnUpdatePopUpImage, this, &MainWindowWidget::slotOnUpdatePopUp);
    connect(g_actupdatemgr, &CActUpdateMangr::sigAddRecommendAction, m_pActionLibWidget, &ActionLibWidget::slotOnAddRecommendAction);
    connect(g_actupdatemgr, &CActUpdateMangr::sigOnAddPromoteItem, m_pActionLibWidget, &ActionLibWidget::slotOnAddBusinessPromoteData);
    ui->tabActionsVL->addWidget(m_pActionLibWidget);
    m_pActionListCurWidget = m_pActionLibWidget;
    m_pActionDownload = new DownloadWidget(this);
    m_pActionDownload->setWindowModality(Qt::ApplicationModal);

    // 请求推广活动
    m_pActionLibWidget->requestBusinessPromoteList();

    m_pActionUploadAddAct = new WidgetUploadAddAction(this);
    m_pActionUploadAddAct->setWindowModality(Qt::ApplicationModal);
    m_pActionUploadAddAct->hide();

    m_pActionUploadMain = new WidgetUpload(this);
    m_pActionUploadMain->setWindowModality(Qt::ApplicationModal);
    m_pActionUploadMain->hide();

    m_pActionUploadAddAct->m_pNextWidget = m_pActionUploadMain;
    m_pActionUploadMain->m_pPreWidget = m_pActionUploadAddAct;

    connect(ui->pushButton_actupload, &QPushButton::clicked, this, &MainWindowWidget::OnActionUploadBtnClicked);
    connect(g_actupdatemgr, &CActUpdateMangr::SigDownloadProgress, this, &MainWindowWidget::onUpdateDownLoadBtnStatus);
}

void MainWindowWidget::InitSyncPage()
{
    if(CreateLocalActSyncWidget(&m_pILocalActSyncWidget))
    {
        m_pILocalActSyncWidget->Connect(this, this);
        ui->actSyncHL->insertWidget(1, m_pILocalActSyncWidget->GetWidget());
    }

    UBXRobot3DControlWidget* p3DCtrlWidget = ((UBXRobot3DContainer*)m_pIUBX3DView->GetWidget())->GetCtrlWidget();
    QObject::connect(this,
                     &MainWindowWidget::sigOnPortOpenChanged,
                     p3DCtrlWidget,
                     &UBXRobot3DControlWidget::OnPortOpenChanged);
    QObject::connect(this,
                     &MainWindowWidget::sigOnPortOpenChanged,
                     (LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(),
                     &LocalActSyncWidget::OnPortOpenChanged);
    QObject::connect(this,
                     &MainWindowWidget::sigOnPortOpenChanged,
                     m_pTitleBar,
                     &TitleBar::OnPortOpenChanged);
    QObject::connect(m_pTitleBar,
                     &TitleBar::SigOpenPort,
                     (LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(),
                     &LocalActSyncWidget::onPortOpenClick);

    QObject::connect(m_pRobotControlWidget,
                     &CRobotControlWidget::sigConnectRobot,
                     (LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(),
                     &LocalActSyncWidget::onPortOpenClick);
    QObject::connect(m_pRobotControlWidget,
                     &CRobotControlWidget::sigDisconnectRobot,
                     (LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(),
                     &LocalActSyncWidget::onPortOpenClick);

    QObject::connect(this, &MainWindowWidget::sigOnRcvRobotTFCardStatus, (LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(),
                     &LocalActSyncWidget::slotOnReceiveRobotTFCardStatus);
    QObject::connect(this, &MainWindowWidget::sigOnRcvRobotTFCardStatus, m_pActionDownload, &DownloadWidget::slotOnReceiveRobotTFCardStatus);
    QObject::connect(UBXUpdateAdministrator::getInstance(), &UBXUpdateAdministrator::sigIsSystemEmbedVersionNeedCopyUpdate,
                     (LocalActSyncWidget*)m_pILocalActSyncWidget->GetWidget(), &LocalActSyncWidget::slotOnIsSystemEmbedVersionNeedCopyUpdate);
}

void MainWindowWidget::InitRobotControlWidget()
{
    m_pRobotControlWidget = new CRobotControlWidget(this);
    // mac环境下需要设置为WindowStaysOnTopHint
#ifdef Q_OS_MAC
    m_pRobotControlWidget->setWindowFlags(m_pRobotControlWidget->windowFlags()| Qt::WindowStaysOnTopHint);
#endif
    connect(this, &MainWindowWidget::sigOnPortOpenChanged, m_pRobotControlWidget, &CRobotControlWidget::slotOnPortOpenChanged);
}

void MainWindowWidget::updateRobotControlWidgetPos()
{
    if (!m_pRobotControlWidget)
    {
        return;
    }
    m_bRobotCtrlWidgetHasFirstShow = true;
    m_pRobotControlWidget->move(ui->tabWidget->frameGeometry().topRight().x() - m_pRobotControlWidget->width(), ui->tabWidget->frameGeometry().topRight().y());
    m_pRobotControlWidget->show();
}


void MainWindowWidget::changeCursor(const QPoint &clientPoint)
{
    //窗体rect
    QRect rtWin = rect();
    //不包含border的窗体rect
    QRect resizeInnerRect(BORDER_PADDING, BORDER_PADDING,
                          rtWin.width() - 2*BORDER_PADDING,
                          rtWin.height() - 2*BORDER_PADDING);
    //先判断鼠标是否移动到border范围
    if(rtWin.contains(clientPoint) && !resizeInnerRect.contains(clientPoint))
    {
        if(clientPoint.y() <= BORDER_PADDING)
        {
            if(clientPoint.x() <= BORDER_PADDING)
            {
                //左上角
                m_resizeType = ResizeLT;
                QApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
            }
            else if(clientPoint.x() >= rtWin.width() - BORDER_PADDING)
            {
                //右上角
                m_resizeType = ResizeRT;
                QApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
            }
            else
            {
                //上边界
                m_resizeType = ResizeT;
                QApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
            }
        }
        else if(clientPoint.y() >= rtWin.height() - BORDER_PADDING)
        {
            if(clientPoint.x() <= BORDER_PADDING)
            {
                //左下角
                m_resizeType = ResizeLB;
                QApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
            }
            else if(clientPoint.x() >= rtWin.width() - BORDER_PADDING)
            {
                //右下角
                m_resizeType = ResizeRB;
                QApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
            }
            else
            {
                //下边界
                m_resizeType = ResizeB;
                QApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
            }
        }
        else
        {
            if(clientPoint.x() <= BORDER_PADDING)
            {
                //左边界
                m_resizeType = ResizeL;
                QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
            }
            else
            {
                //右边界
                m_resizeType = ResizeR;
                QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
            }
        }
    }
    //    else
    //    {
    //        m_resizeType = UnSet;
    //        setCursor(QCursor(Qt::ArrowCursor));
    //    }
}

void MainWindowWidget::changeSize(const QPoint &nowPt)
{
    QRect rectWnd = rect();
    QPoint ltPt = mapToGlobal(rectWnd.topLeft());
    QPoint rbPt = mapToGlobal(rectWnd.bottomRight());

    QRect rectResize(ltPt, rbPt);
    switch(m_resizeType)
    {
    case ResizeL://左边
    {
        rectResize.setX(nowPt.x() - BORDER_PADDING / 2);
        if(minimumSize().width() >= rectResize.width())
        {
            return;
        }
    }
        break;
    case ResizeLT:
    {
        rectResize.setX(nowPt.x() - BORDER_PADDING / 2);
        rectResize.setY(nowPt.y() - BORDER_PADDING / 2);
    }
        break;
    case ResizeT:
    {
        rectResize.setY(nowPt.y() - BORDER_PADDING / 2);
        if(minimumSize().height() >= rectResize.height())
        {
            return;
        }
    }
        break;
    case ResizeRT://右下角
    {
        rectResize.setWidth(nowPt.x() - ltPt.x() + BORDER_PADDING / 2);
        rectResize.setY(nowPt.y() + BORDER_PADDING / 2);
    }
        break;
    case ResizeR:
    {
        rectResize.setWidth(nowPt.x() - ltPt.x() + BORDER_PADDING / 2);
    }
        break;
    case ResizeRB:
    {
        rectResize.setWidth(nowPt.x() - ltPt.x() + BORDER_PADDING / 2);
        rectResize.setHeight(nowPt.y() - ltPt.y() + BORDER_PADDING / 2);
    }
        break;
    case ResizeB:
    {
        rectResize.setHeight(nowPt.y() - ltPt.y() + BORDER_PADDING / 2);
    }
        break;
    case ResizeLB:
    {
        rectResize.setX(nowPt.x() - BORDER_PADDING / 2);
        rectResize.setHeight(nowPt.y() - ltPt.y() + BORDER_PADDING / 2);
    }
        break;
    }
    if(minimumSize().width() >= rectResize.width() && minimumSize().height() >= rectResize.height())
    {
        return;
    }
    setGeometry(rectResize);
    m_bytearrGeometry = saveGeometry();
}

void MainWindowWidget::slotNew()
{
    // 清空撤销重做列表
    clearUndoRedoList();

    bool isProjectModify = false;
    bool bIsMusicModify = false;
    if(m_pIUBXmEditor)
    {
        m_pIUBXmEditor->UBXMotionEditor_StopDebug();
        isProjectModify = m_pIUBXmEditor->UBXMotionEditor_IsProjectModity();
        bIsMusicModify = m_pIUBXmEditor->UBXMotionEditor_IsMusicModity();
    }

    if(isProjectModify || bIsMusicModify)
    {
        QMessageBox::StandardButton retValue = CMessageBox::question(this, "", tr("The project has been modified, would you like to save it?"),
                                             QMessageBox::No|QMessageBox::Cancel|QMessageBox::Yes,
                                             QMessageBox::Yes );
        if(QMessageBox::Cancel == retValue)
        {
            return;
        }

        if(QMessageBox::Yes == retValue)
        {
            if(m_pIUBXmEditor)
            {
                m_pIUBXmEditor->UBXMotionEditor_SetProjectModity(false);
                m_pIUBXmEditor->UBXMotionEditor_SetMusicModity(false);

            }
            slotSave();
        }

    }
//    ClearTempDir();
    clearWaveDir();
    clearMusicDir();

    if(m_pIUBXmEditor)
    {
        m_pIUBXmEditor->GetWidget()->hide();
        m_pIUBXmEditor->Disconnect();
        m_pIUBXmEditor = NULL;
        if(CreateUBXMotionEditor(&m_pIUBXmEditor))
        {
            m_pIUBXmEditor->Connect(this);
            m_pIUBXmEditor->UBXMotionEditor_Set3DRobotViewInterface(m_pIUBX3DView);
        }
        
        if(m_pIUBXmEditor)
        {
            // 添加到UI后再显示，避免闪烁
            m_pIUBXmEditor->GetWidget()->hide();

            m_pIUBXmEditor->SetData(NULL,0);
            ui->mainHL->insertWidget(eEditPageIndexActionEdit, m_pIUBXmEditor->GetWidget(), STRETCH_FATOR_ACTION_EDIT);
            m_pIUBXmEditor->UBXMotionEditor_SetTreeView(m_pITreeNavigation);
            m_pIUBXmEditor->GetWidget()->show();
        }
    }

    // 由于在更新tab页显示的时候需要文件名，因此先设置文件名，
    // 后续调用UBXMotionEditor_SetProjectModity()/UBXMotionEditor_SetMusicModity才会刷新
    setUIMainLayerLayoutTabText(tr("untitled"), false);

    m_saveAsDatas.clearDatas();
    m_saveAsDatas.strSaveFilePath = getOpenProjectPath();
}

void MainWindowWidget::slotAbout()
{
    DialogAbout* dlg = new DialogAbout(this);
    dlg->show();
}

void MainWindowWidget::slotOpen()
{
    // 清空撤销重做列表
    clearUndoRedoList();

    bool isProjectModify = false;
    bool bIsMusicModify = false;
    if(m_pIUBXmEditor)
    {
        m_pIUBXmEditor->UBXMotionEditor_StopDebug();
        isProjectModify = m_pIUBXmEditor->UBXMotionEditor_IsProjectModity();
        bIsMusicModify = m_pIUBXmEditor->UBXMotionEditor_IsMusicModity();
    }

    if(isProjectModify || bIsMusicModify)
    {
        QMessageBox::StandardButton retValue = CMessageBox::question(this, "", tr("The project has been modified, would you like to save it?"),
                                             QMessageBox::No|QMessageBox::Cancel|QMessageBox::Yes,
                                             QMessageBox::Yes);
        if(QMessageBox::Cancel == retValue)
        {
            return;
        }
        else if(QMessageBox::Yes == retValue)
        {
            if(m_pIUBXmEditor)
            {
                m_pIUBXmEditor->UBXMotionEditor_SetProjectModity(false);
                m_pIUBXmEditor->UBXMotionEditor_SetMusicModity(false);
            }
            slotSave();
        }
    }

//    ClearTempDir();
    clearWaveDir();
    clearMusicDir(); // 打开文件时，会自动到music路径加载音乐文件，因此需要清除音乐文件

    if(m_pIUBXmEditor)
    {
        QString sFmtTitle = tr("Action File");
        QString sFmt;
#ifdef Q_OS_MAC
        sFmt.sprintf("%s(*%s *%s *%s);;%s(*%s *%s *%s)", sFmtTitle.toStdString().c_str(), UBX_FILE_EXT, UBX_FILE_EXT_OLD, TAB_FILE_EXT, sFmtTitle.toStdString().c_str(), UBX_FILE_EXT, UBX_FILE_EXT_OLD, TAB_FILE_EXT);
#else
        sFmt.sprintf("%s(*%s *%s *%s)", sFmtTitle.toStdString().c_str(), UBX_FILE_EXT, UBX_FILE_EXT_OLD, TAB_FILE_EXT);
#endif
        QString sAppLocalDownLoadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        QString strFileName = QFileDialog::getOpenFileName(NULL, tr("Open Project"), getOpenProjectPath(), sFmt); //选择路径
        if(strFileName.isEmpty())
        {
            //CMessageBox::warning(this, "", "打开文件路径为空");
            return;
        }

        strFileName = QDir::toNativeSeparators(strFileName);

        if(!CheckFile(strFileName))
        {
            //CMessageBox::warning(this, "", "分析文件格式失败");
            return;
        }

        // 重新打开工程需要清空历史数据
        m_saveAsDatas.clearDatas();
        ReadFromFile(strFileName);

        // 记录历史路径
        QFileInfo fileInfo(strFileName);
        saveOpenProjectPath(fileInfo.path());
        m_saveAsDatas.strSaveFilePath = getOpenProjectPath();

        setUIMainLayerLayoutTabText(strFileName, true);

        //        QFile file(strFileName);
        //        if(file.open(QIODevice::ReadOnly))
        //        {
        //            int nLens = file.size();
        //            char *pDataBuf = new char[nLens];
        //            file.read(pDataBuf,nLens);
        //            file.close();

        //            if(pDataBuf)
        //            {
        //                m_pIUBXmEditor->SetData(pDataBuf,nLens);
        //                m_strSaveFileName = strFileName;
        //            }
        //            else
        //            {
        //                return;
        //            }
        //        }
    }
}

void MainWindowWidget::slotSave()
{
    if(m_pIUBXmEditor)
    {
//        if(m_strSaveFileName.isEmpty())
//        {
//            slotSaveAs();
//        }
//        else
//        {
//            char *pData = NULL;
//            int nLens = 0;
//            m_pIUBXmEditor->GetData(&pData,nLens);
//            CFileDirHandle::saveFile(m_strSaveFileName, pData, nLens);
//            SAFE_DELETE_ARRAY(pData);

////            QChar sSeparator = QDir::separator();
////            int nPos = m_strSaveFileName.lastIndexOf(sSeparator);
////            QString strPath = m_strSaveFileName.left(nPos);
////            QString StrTmpName = m_strSaveFileName.right(m_strSaveFileName.length() - nPos -1);
////            int nTmpPos = StrTmpName.lastIndexOf(".");
////            QString strNewName = StrTmpName.left(nTmpPos);

//            QString strPath = CFileDirHandle::getFileDirPath(m_strSaveFileName);
//            QString strNewName = CFileDirHandle::getFileBaseName(m_strSaveFileName);

//            QString strMusicPath = CConfigs::getLocalMusicDir();
//            if(m_pIUBXmEditor)
//            {
//                if(m_pIUBXmEditor->UBXMotionEditor_IsMusicModity())
//                {
//                    myCopyDirectoryAndRename(strMusicPath, strPath, strNewName);
//                }
//            }
//        }


       if(m_saveAsDatas.strSaveFilePath.isEmpty() || m_saveAsDatas.strSaveFileName.isEmpty())
       {
           slotSaveAs();
       }
       else
       {
           //save
           if(m_pIUBXmEditor)
           {
               CFilesSaveAsUI::saveFile(m_pIUBXmEditor, m_saveAsDatas);
           }
       }
    }

    if(m_pIUBXmEditor)
    {
        m_pIUBXmEditor->UBXMotionEditor_SetProjectModity(false);
        m_pIUBXmEditor->UBXMotionEditor_SetMusicModity(false);
    }
}

void MainWindowWidget::slotSaveAs()
{
//    QString sFmtTitle = tr("Action File");
//    QString sFmt = CFileDirHandle::getFileSelectFormat(sFmtTitle, UBX_FILE_EXT);

//    const QString strFileName = QFileDialog::getSaveFileName(this,
//                                                       tr("Save Project"),
//                                                       "Project",
//                                                       sFmt); //选择路径

//    if(strFileName.isEmpty())
//    {
//        return;
//    }
//    else
//    {
////        m_strSaveFileName = strFileName;

//        m_saveAsDatas.strSaveFilePath = CFileDirHandle::getFileDirPath(strFileName);
//        m_saveAsDatas.strSaveFileName = CFileDirHandle::getFileBaseName(strFileName);

//        char *pData = NULL;
//        int nLens = 0;
//        m_pIUBXmEditor->GetData(&pData,nLens);
//        bool bOk = CFileDirHandle::saveFile(strFileName, pData, nLens);
//        SAFE_DELETE_ARRAY(pData);

//        if(bOk)
//        {
//            //这里处理树图表
////            QChar sSeparator = QDir::separator();
////            int nPos = m_strSaveFileName.lastIndexOf(sSeparator);
////            QString strPath = m_strSaveFileName.left(nPos);
////            QString StrTmpName = m_strSaveFileName.right(m_strSaveFileName.length() - nPos -1);

////            int nTmpPos = StrTmpName.lastIndexOf(".");
////            QString strNewName = StrTmpName.left(nTmpPos);

//            QString strPath = CFileDirHandle::getFileDirPath(strFileName);
//            QString strNewName = CFileDirHandle::getFileBaseName(strFileName);
//            QString strMusicPath = CConfigs::getLocalMusicDir();

//            myCopyDirectoryAndRename(strMusicPath, strPath, strNewName);
//        }
//    }

    // 另存为可能会上传到后台服务器，因此需要复位nFileActionID，否则可能出现
    // 第一次保存到后台后，再选择另存为保存到后台时，配置的nFileActionID相同；
    // 手机app端下载只能下载其中一个上传
    m_saveAsDatas.nFileActionID = DEFAULT_INVALID_NOINIT_INT_NUMBER;

    CFilesSaveAsUI* pFileSaveAsUIWidget = new CFilesSaveAsUI(&m_saveAsDatas);
    pFileSaveAsUIWidget->setIUBXMotionEditor(m_pIUBXmEditor);
    pFileSaveAsUIWidget->exec();

    if(!m_saveAsDatas.strSaveFilePath.isEmpty())
    {
        saveOpenProjectPath(m_saveAsDatas.strSaveFilePath);
    }

    if(m_pIUBXmEditor)
    {
        m_pIUBXmEditor->UBXMotionEditor_SetProjectModity(false);
        m_pIUBXmEditor->UBXMotionEditor_SetMusicModity(false);
    }

//    setUIMainLayerLayoutTabText(strFileName);
    if (!m_saveAsDatas.strSaveFileName.isEmpty())  // 如果用户没有填写文件名，不会设置，否则文件名会变成.aesx
    {
        const QString strSaveFileDir = m_saveAsDatas.strSaveFilePath;
        const QString strSaveFileAesx = CConfigs::getFilePath(strSaveFileDir,
                                                             m_saveAsDatas.strSaveFileName + UBX_FILE_EXT);
        setUIMainLayerLayoutTabText(strSaveFileAesx, true);
    }
}

void MainWindowWidget::slotExportToHts()
{

    if(m_pIUBXmEditor)
    {
        QString sFmtTitle = tr("Action File");
        QString sFmt;
#ifdef Q_OS_MAC
        sFmt.sprintf("%s(*%s);;%s(*%s)",
                     sFmtTitle.toStdString().c_str(),
                     HTS_FILE_EXT,
                     sFmtTitle.toStdString().c_str(),
                     HTS_FILE_EXT);
#else
        sFmt.sprintf("%s(*%s)", sFmtTitle.toStdString().c_str(), HTS_FILE_EXT);
#endif

        QString strDefaultHtsFileName = m_saveAsDatas.strSaveFileName;
        if (strDefaultHtsFileName.isEmpty())
        {
            strDefaultHtsFileName = DEFAULT_FILE_NAME;
        }
        const QString strHtsFileName = QFileDialog::getSaveFileName(this,
                                                                    tr("Export Hts"),
                                                                    strDefaultHtsFileName,
                                                                    sFmt);

        if(strHtsFileName.isEmpty())
        {
            return;
        }
        else
        {
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
                    //                        CMessageBox::warning(this, "", tr("create hts data failed."));
                }

                if(pHtsData)
                {
//                    QFile fileobj(strHtsFileName);
//                    if(fileobj.open(QIODevice::WriteOnly))
//                    {
//                        fileobj.write(pHtsData, nHtsLen);
//                        fileobj.close();

//                        bExportOK = true;
//                    }
                    bExportOK = CFileDirHandle::saveFile(strHtsFileName, pHtsData, nHtsLen);
                }

                SAFE_DELETE_ARRAY(pHtsData);
            }

            SAFE_DELETE_ARRAY(pData);

            if(!bExportOK)
            {
                CMessageBox::warning(this, "", tr("create hts file failed."));
            }
        }
    }
}

void MainWindowWidget::slotExportToXml()
{
    if(m_pIUBXmEditor)
    {
        QString sFmtTitle = tr("Action File");
        QString sFmt;
#ifdef Q_OS_MAC
        sFmt.sprintf("%s(*%s);;%s(*%s)",
                     sFmtTitle.toStdString().c_str(),
                     XML_FILE_EXT,
                     sFmtTitle.toStdString().c_str(),
                     XML_FILE_EXT);
#else
        sFmt.sprintf("%s(*%s)", sFmtTitle.toStdString().c_str(), XML_FILE_EXT);
#endif

        QString strDefaultHtsFileName = m_saveAsDatas.strSaveFileName;
        if (strDefaultHtsFileName.isEmpty())
        {
            strDefaultHtsFileName = DEFAULT_FILE_NAME;
        }
        const QString strFileName = QFileDialog::getSaveFileName(this,
                                                                    tr("Export"),
                                                                    strDefaultHtsFileName,
                                                                    sFmt);

        if(strFileName.isEmpty())
        {
            return;
        }
        else
        {
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
                    //                        CMessageBox::warning(this, "", tr("create hts data failed."));
                }

                if(pHtsData)
                {
                    bExportOK = CActMixtureParse::htsToMixXml((const unsigned char * const)pHtsData, nHtsLen, strFileName);
                }

                SAFE_DELETE_ARRAY(pHtsData);
            }

            SAFE_DELETE_ARRAY(pData);

            if(!bExportOK)
            {
                CMessageBox::warning(this, "", tr("create file failed."));
            }
        }
    }
}

void MainWindowWidget::slotQuit()
{
    this->close();
}

void MainWindowWidget::onClickTab(int nID)
{
    if (ui->tabWidget)
    {
        ui->tabWidget->setCurrentIndex(nID);
    }
}

void MainWindowWidget::onClose()
{
    // 关闭的时候先关闭控制按钮
    if (m_pRobotControlWidget)
    {
        m_pRobotControlWidget->close();
    }
    this->close();
}

void MainWindowWidget::onMin()
{
#ifdef Q_OS_MAC
    if (m_pRobotControlWidget)
    {
        m_pRobotControlWidget->hide();
    }
#endif
    this->showMinimized();
}

void MainWindowWidget::onMax()
{
    setWindowState(Qt::WindowMaximized);
}

void MainWindowWidget::onRestore()
{
    setWindowState(Qt::WindowNoState);
    restoreGeometry(m_bytearrGeometry);
}

void MainWindowWidget::on_pbNew_clicked()
{
    this->slotNew();
}

void MainWindowWidget::on_pbOpen_clicked()
{
    this->slotOpen();
}

void MainWindowWidget::on_pbSave_clicked()
{
    this->slotSave();
}

void MainWindowWidget::on_pbSaveAs_clicked()
{
    this->slotSaveAs();
}

void MainWindowWidget::slotOnDoubleClicked()
{
    if(windowState() & Qt::WindowMaximized)
        emit SigRestore();
    else
        emit SigMax();
}

void MainWindowWidget::slotOnUndo()
{
    UBXUndoRedoManager *pUndoRedoManager =  UBXUndoRedoManager::getInstance();
    if (!pUndoRedoManager)
    {
        return;
    }
    pUndoRedoManager->executeUndoOperation();
}

void MainWindowWidget::slotOnRedo()
{
    UBXUndoRedoManager *pUndoRedoManager =  UBXUndoRedoManager::getInstance();
    if (!pUndoRedoManager)
    {
        return;
    }
    pUndoRedoManager->executeRedoOperation();
}

void MainWindowWidget::slotOnUndoListSizeChanged(int nSize)
{
    if (nSize > 0)
    {
        setUndoBtnEnable(true);
    }
    else
    {
        setUndoBtnEnable(false);
    }
}

void MainWindowWidget::slotOnRedoListSizeChanged(int nSize)
{
    if (nSize > 0)
    {
        setRedoBtnEnable(true);
    }
    else
    {
        setRedoBtnEnable(false);
    }
}

void MainWindowWidget::clearWaveDir()
{
    QString strWaveDir = CConfigs::getLocalWaveDir();
    CFileDirHandle::clearDir(strWaveDir, false);
}

void MainWindowWidget::clearMusicDir()
{
    QString strMusicDir = CConfigs::getLocalMusicDir();
    CFileDirHandle::clearDir(strMusicDir, false);
}

void MainWindowWidget::saveOpenProjectPath(const QString &strPath)
{
    if (strPath.isEmpty())
    {
        return;
    }
    CIniConfig::getInstance().setPath(eFilePathOpenProject, strPath);
}

QString MainWindowWidget::getOpenProjectPath()
{
    QString strPath;
    CIniConfig::getInstance().getPath(eFilePathOpenProject, strPath);
    QDir dir(strPath);

    if (strPath.isEmpty() || !dir.exists())
    {
        strPath = QDir::currentPath();//CConfigs::getLocalDownloadDir();
    }

    return strPath;
}

void MainWindowWidget::drawBorder(QPainter &painter)
{
    painter.save();
    QRect rect = this->rect();
    painter.setPen(QPen(QBrush(QColor(182, 182, 182)), BORDER_PADDING));
    painter.drawRoundedRect(rect, BORDER_RADIUS, BORDER_RADIUS);
    painter.restore();
}

void MainWindowWidget::drawBackGround(QPainter &painter)
{
    painter.save();
    QBrush brush(QColor("#0e0e0e"));
    QRect rect = this->rect();
    QPainterPath path;
    path.addRoundedRect(rect, BORDER_RADIUS, BORDER_RADIUS);
    painter.fillPath(path, brush);
    painter.restore();
}

void MainWindowWidget::setUIMainLayerLayoutTabText(const QString &sFile, bool bIsTitleToSetToTabText)
{
    if (m_pIUBXmEditor)
    {
        m_pIUBXmEditor->UBXMotionEditor_SetTabWidgetTabText(sFile);
        m_bProjectTitleTabTextHadSet = bIsTitleToSetToTabText;
    }
}

void MainWindowWidget::showActionDetailedPage(const ActItemData &itemData)
{
    if(NULL != m_pActionListCurWidget)
    {
        m_pActionListCurWidget->hide();
        ui->tabActionsVL->removeWidget(m_pActionListCurWidget);
    }

    if(NULL == m_pActionDetail)
    {
        m_pActionDetail = new ActionDetailWidget();
        connect(m_pActionDetail, &ActionDetailWidget::SigGoBack, this, &MainWindowWidget::OnGoBack);
        connect(m_pActionDetail, &ActionDetailWidget::sigOnDetailedWidgetMoreClicked, this, &MainWindowWidget::slotOnDetailedWidgetMoreClicked);
    }

    ui->tabActionsVL->addWidget(m_pActionDetail);
    m_pActionListCurWidget = m_pActionDetail;
    m_pActionDetail->show();
    m_pActionDetail->slotOnActionPreviewClicked(itemData);
}

void MainWindowWidget::clearUndoRedoList()
{
    UBXUndoRedoManager::getInstance()->clearAllList();
}

void MainWindowWidget::setUndoBtnEnable(bool bEnable)
{
    ui->pbUndo->setEnabled(bEnable);
}

void MainWindowWidget::setRedoBtnEnable(bool bEnable)
{
    ui->pbRedo->setEnabled(bEnable);
}

void MainWindowWidget::SetDefaultStyle()
{
    QFile styleSheet(ALPHA_DEFAULT_QSS_PATH);
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

void MainWindowWidget::subscribeMsg()
{
    SetMsgType mainWidgetMsg;
    mainWidgetMsg.clear();
    mainWidgetMsg.insert(NOTIFY_MSG_LOGIN_REQUEST);
    mainWidgetMsg.insert(WM_MSG_ROBOT_TF_CARD_STATUS);
    mainWidgetMsg.insert(NOTIFY_MSG_PLAY_MOTION);
    mainWidgetMsg.insert(NOTIFY_MSG_PLAY_MUSIC);
    mainWidgetMsg.insert(NOTIFY_MSG_UNITED_PLAY);
    this->AddSubscribe(mainWidgetMsg);
    this->EnableRecvNotify(TRUE);

}

void MainWindowWidget::createFileMenu()
{
    m_aboutAct = new QAction("About AlphaRobot", this);
    m_Quit =  new QAction("Quit",this);
    m_pSettingAction = new QAction("Setting", this);

    m_newAct = new QAction(tr("New Project(&N)"),this);
    m_openAct = new QAction(tr("Open Project(&O)"),this);
    m_saveAct = new QAction(tr("Save Project(&S)"),this);
    m_saveAsAct =new QAction(tr("Project SaveAs(&A)"),this);
    m_exportHts = new QAction(tr("Export to Hts"), this);

    m_FileMenu = m_pMenuBar->addMenu(("File(&F)"));

    retranslateUi();

    m_FileMenu->addAction(m_aboutAct);   //About 菜单
    m_FileMenu->addAction(m_Quit);
    m_FileMenu->addAction(m_pSettingAction);
    m_FileMenu->addAction(m_newAct);
    m_FileMenu->addAction(m_openAct);
    m_FileMenu->addAction(m_saveAct);
    m_FileMenu->addAction(m_saveAsAct);
    m_FileMenu->addAction(m_exportHts);

    connect(m_newAct, SIGNAL(triggered()), this ,SLOT(slotNew()));
    connect(m_aboutAct, SIGNAL(triggered()), this ,SLOT(slotAbout()));
    connect(m_openAct, SIGNAL(triggered()), this ,SLOT(slotOpen()));
    connect(m_saveAct, SIGNAL(triggered()),this,SLOT(slotSave()));
    connect(m_saveAsAct, SIGNAL(triggered()),this,SLOT(slotSaveAs()));
    connect(m_exportHts, SIGNAL(triggered()), this, SLOT(slotExportToHts()));
    connect(m_Quit, SIGNAL(triggered()),this,SLOT(slotQuit()));
    connect(m_pSettingAction, &QAction::triggered, this, &MainWindowWidget::slotOnSetting);
}

void MainWindowWidget::createHelpMenu()
{
//    m_pHelpMenu = m_pMenuBar->addMenu(tr("Help(&H)"));
    m_pHelpMenu = new CMenu(tr("Help(&H)"), m_pMenuBar);
    m_pHelpMenu->setEnableAlert(true);
    m_pHelpMenu->setTextAlert(QString("*"));
    m_pHelpMenu->setProperty(MENU_DATA_ALERT_KEY.toStdString().c_str(), true);

    m_pMenuBar->addMenu(m_pHelpMenu);

    m_pOpenHelpDocAction = new QAction(tr("Document(&D)"), m_pHelpMenu);
    m_pHelpMenu->addAction(m_pOpenHelpDocAction);
    connect(m_pOpenHelpDocAction, &QAction::triggered, this, &MainWindowWidget::slotOnOpenHelpDoc);

    m_pOpenVideoTutorialAction = new QAction(tr("Video Tutorial"), m_pHelpMenu);
//    m_pHelpMenu->addAction(m_pOpenVideoTutorialAction);
    connect(m_pOpenVideoTutorialAction, &QAction::triggered, this, &MainWindowWidget::slotOnVideoTutorial);

    m_pActionCheckFirmwareVersion = new QAction(tr("Robot Fireware Version"), m_pHelpMenu);
    m_pHelpMenu->addAction(m_pActionCheckFirmwareVersion);
    m_pActionCheckFirmwareVersion->setEnabled(false);
    connect(m_pActionCheckFirmwareVersion, &QAction::triggered, this, &MainWindowWidget::slotOnCheckUpdateSysEmbedVersion);

// Mac版本更新功能暂时不需要
//    m_pActionRobotPCSoftVersion = new QAction(tr("Robot Soft Version"), m_pHelpMenu);
//    m_pHelpMenu->addAction(m_pActionRobotPCSoftVersion);
//    connect(m_pActionRobotPCSoftVersion, &QAction::triggered, this, &MainWindowWidget::slotOnShowSystemPCSoftVersion);

    retranslateUi();
}

bool MainWindowWidget::ReadFromFile(QString strPathName)
{
    bool bRet = false;
    char* pDataBuf = NULL; //文件数据
    int nDataLen = 0;  //数据长度
    do
    {
        if(strPathName.isEmpty())
        {
            break;
        }
        if(m_pIUBXmEditor)
        {
            QString strFullPath = strPathName;
            QString sMusicDir ="";
            int nPos =strFullPath.lastIndexOf('.');
            if(nPos < 0)
            {
                sMusicDir = "";
            }
            else
            {
                sMusicDir = strFullPath.left(nPos);
            }

            QString sMusicFilePath = sMusicDir + MUSIC_FILE_EXT;

            QString strMusicPath = CConfigs::getLocalMusicDir();
            QFile file(sMusicFilePath);
            if(file.exists())
            {
                QString strCurDir = strFullPath.left(strFullPath.lastIndexOf(QDir::separator()));
                if(QMessageBox::Yes == CMessageBox::question(this, "", tr("Found music file exist, Open it?"),
                                                             QMessageBox::No|QMessageBox::Yes,
                                                             QMessageBox::Yes))//QMessageBox::Yes)
                {

                    QString sFmtTitle = tr("Music File");
                    QString sFmtExt = MUSIC_FILE_EXT;
                    QString sFmt;
#ifdef Q_OS_MAC
                    sFmt.sprintf("%s(*%s);;%s(*%s)", sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str(), sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str());
#else
                    sFmt.sprintf("%s(*%s)", sFmtTitle.toStdString().c_str(), sFmtExt.toStdString().c_str());
#endif
                    QString sSrcMusicFile = QFileDialog::getOpenFileName(NULL, tr("Choose a music file"), strCurDir, sFmt);
                    if(!sSrcMusicFile.isEmpty())
                    {
                        sSrcMusicFile = QDir::toNativeSeparators(sSrcMusicFile);

                        QFile fileMusic(sSrcMusicFile);
                        QChar sSeparator = QDir::separator();
                        int nPosMusic = sMusicFilePath.lastIndexOf(sSeparator);
                        QString sMusicName = sMusicFilePath.mid(nPosMusic);
                        if(fileMusic.exists())
                        {
                            fileMusic.copy(strMusicPath + sMusicName);
                        }
                    }

                }
            }
            QString strExt =strFullPath.mid(nPos);
            if(strExt.compare(UBX_FILE_EXT) == 0 || strExt.compare(UBX_FILE_EXT_OLD) == 0 )
            {
                QFile file(strFullPath);
                if(!file.open(QFile::ReadOnly))
                {
                    qDebug()<<tr("open file failed")<<endl;
                    break;
                }
                nDataLen = file.size();
                pDataBuf = new char[nDataLen];
                file.read(pDataBuf, nDataLen);
                file.close();
//                m_strSaveFileName = strFullPath;

                m_saveAsDatas.strSaveFilePath = CFileDirHandle::getFileDirPath(strFullPath);
                m_saveAsDatas.strSaveFileName = CFileDirHandle::getFileBaseName(strFullPath);
            }
            if(pDataBuf)
            {
                // 由于加载数据时会删除所有层，如果不先隐藏再显示会出现闪烁
                m_pIUBXmEditor->GetWidget()->hide();

                m_pIUBXmEditor->SetData(pDataBuf, nDataLen);
                m_pIUBXmEditor->AdjustData();
                m_pIUBXmEditor->GetWidget()->show();
                SAFE_DELETE_ARRAY(pDataBuf);
            }
            else
            {
                break;
            }
            if(m_pIUBXmEditor)
            {
                m_pIUBXmEditor->UBXMotionEditor_SetTreeView(m_pITreeNavigation);
                m_pIUBXmEditor->UBXMotionEditor_SetProjectModity(false);
                m_pIUBXmEditor->UBXMotionEditor_SetMusicModity(false);
            }
            bRet = true;

        }
    }while(0);
    return bRet;
}

/**************************************************************************
* 函数名:myCopyDirectory
* 功能:拷贝文件到目标目录
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MainWindowWidget::myCopyDirectory(QString strSource, QString strTarget)
{
    QDir TmpDir;
    if(!TmpDir.exists(strTarget))
    {
        TmpDir.mkdir(strTarget);//创建目标文件夹
    }
    QDir dir(strSource);
    dir.setFilter(QDir::Files);
    QFileInfoList info_list = dir.entryInfoList();
    foreach(QFileInfo file_info, info_list)
    {
        if (file_info.isFile())
        {
            QFile::copy(file_info.absoluteFilePath(),strTarget + "/" +file_info.fileName());
        }
    }

}
/**************************************************************************
* 函数名:myCopyDirectoryAndRename
* 功能:拷贝文件到目标目录并修改文件名
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/
void MainWindowWidget::myCopyDirectoryAndRename(QString strSource, QString strTarget ,QString sNewName)
{
    QDir TmpDir;
    if(!TmpDir.exists(strTarget))
    {
        return ; //目标路径不存在
    }
    QDir dir(strSource);
    dir.setFilter(QDir::Files);
    QFileInfoList info_list = dir.entryInfoList();
    foreach(QFileInfo file_info, info_list)
    {
        if (file_info.isFile())
        {
            QString sFmtTitle = tr("Music File");
            QString sFmtExt = MUSIC_FILE_EXT;
            QString sFmt = CFileDirHandle::getFileSelectFormat(sFmtTitle, sFmtExt);
;
            QString strDefaultFileName = CConfigs::getFilePath(strTarget, sNewName + sFmtExt);

            QString strNewFilePath = QFileDialog::getSaveFileName(this,
                                                                  tr("Save Music file"),
                                                                  strDefaultFileName,
                                                                  sFmt); //选择路径
            if(strNewFilePath.isEmpty())
            {
                return;
            }

            CFileDirHandle::copyFile(file_info.absoluteFilePath(), strNewFilePath, true);

            return;
        }
    }

}

void MainWindowWidget::UnsetCursor()
{
    if(!m_bLeftDown)
    {
        m_resizeType = UnSet;
        QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
    }
}

/**************************************************************************
* 函数名: SetUserLoginStatus
* 功能: 设置用户登录状态
* 参数:
*       @[in]userName:用户名
*       @[in]ActionType:动作类型
*       @[in]isLogin:是否已登录
* 返回值:void
* 时间:2016/02/24/15:58
* 作者: 周志平
**************************************************************************/
void MainWindowWidget::SetUserLoginStatus(QString text, QString ActionType, bool bIsLogin)
{
    //此处修改登录状态
    //connect(this, &MainWindowWidget::SigTextChange, m_pTitleBar, &TitleBar::onTextChange);
    emit SigTextChange(text);
    g_userinfo.m_bLogin = bIsLogin;

    m_pTitleBar->showWelcomeLable(bIsLogin);
}

/**************************************************************************
* 函数名: setUserNickName
* 功能: 设置用户昵称
* 参数:
*       @[in]strNickName:用户昵称
* 返回值:void
* 时间:2016/06/24
* 作者: hwx
**************************************************************************/
void MainWindowWidget::setUserNickName(const QString &strNickName)
{
    emit SigTextChange(strNickName);
}

/**************************************************************************
* 函数名:onUserLogin
* 功能:用户登录注册
* 参数:
* 返回值:
* 时间:
* 作者: 周志平
* 改动日志
**************************************************************************/

void MainWindowWidget::onUserLogin()
{
    if(!g_userinfo.m_bLogin)
    {
        if(m_pLoginDlg)
        {
            m_pLoginDlg->Disconnect();
            m_pLoginDlg = NULL;
        }
        CreateLoginUI(&m_pLoginDlg,this);

        bool bRet = m_pLoginDlg->Connect(this,this);
        if(bRet)
        {
            int nRes = m_pLoginDlg->GetDialog()->exec();
            if(nRes == QDialog::Accepted)
            {
                QString strUserName = "";
                if(m_pLoginDlg)
                {
                    strUserName = m_pLoginDlg->GetUserName();
                }
                SetUserLoginStatus(strUserName,"",true);
                ui->pbFavorites->show();
                ui->pbMyUpload->show();
                ui->pbDownLoads->show();
                ui->pushButton_actupload->show(); //暂时注释

                g_actupdatemgr->SetUserID(m_pLoginDlg->GetUserID());
                if(m_pActionListPage)
                    m_pActionListPage->SetUserLogIn(true);
                if(m_pActionDetail)
                    m_pActionDetail->SetUserLogIn(true);
                if(m_pActionLibWidget)
                {
                    m_pActionLibWidget->UpdatePreviewItemList();
                }
            }
        }
    }
    else
    {
        CreateLoginPopMenuActions();
    }

}
void MainWindowWidget::onUserLogout()
{
    if(m_pLoginDlg)
    {
        if(m_pLoginDlg->UserLoginOut())
        {
            SetUserLoginStatus(tr("login or reg"),"",false);
            g_actupdatemgr->SetUserID(-1);
            ui->pbFavorites->hide();
            ui->pbMyUpload->hide();
//            ui->pbDownLoads->hide();
//            ui->pushButton_actupload->hide();

            if(m_pActionLibWidget)
            {
                m_pActionLibWidget->UpdatePreviewItemList();
            }

            if(m_pActionListPage)
            {
                m_pActionListPage->SetUserLogIn(false);
            }
            if(m_pActionDetail)
            {
                m_pActionDetail->SetUserLogIn(false);
            }
        }
    }
}

//动作库界面导航栏按钮组槽函数
void MainWindowWidget::OnActionLibBGToggled(int id, bool checked)
{
    //鉴于收藏页目前的特殊性，因此每次切换收藏页都要求新建，主要是为了保证在首页或者动作列表页
    //收藏或取消收藏动作时，能确实刷新到我的收藏页面。
    if(checked)
    {
        if(BUTTON_HOME_PAGE == id)
        {
            ui->tabActionsVL->removeWidget(m_pActionListCurWidget);
            m_pActionListCurWidget->hide();
            ui->tabActionsVL->addWidget(m_pActionLibWidget);
            m_pActionLibWidget->show();

            m_pActionListCurWidget = m_pActionLibWidget;
            m_nIdForActionWidget = id;

//            SAFE_DELETE(m_pFavoritesPage);
//            SAFE_DELETE(m_pMyUploadPage);
        }
        else if(BUTTON_ACTIONS_LIBRARY == id)
        {
            if(m_pActionListPage == NULL)
            {
                m_pActionListPage = new ActionListsPage(); // 传入this会导致homePage进入动作详情页后返回时hover无效
                if(-1 != g_actupdatemgr->GetUserID())
                {
                    m_pActionListPage->SetUserLogIn(true);
                }
                else
                {
                    m_pActionListPage->SetUserLogIn(false);
                }
                connect(m_pActionListPage, &ActionListsPage::SigActionPreviewClicked, this, &MainWindowWidget::OnActionPreviewClicked);
            }
            ui->tabActionsVL->removeWidget(m_pActionListCurWidget);
            m_pActionListCurWidget->hide();
            ui->tabActionsVL->addWidget(m_pActionListPage);
            m_pActionListPage->show();
            m_pActionListCurWidget = m_pActionListPage;
            m_nIdForActionWidget = id;

//            SAFE_DELETE(m_pFavoritesPage);
//            SAFE_DELETE(m_pMyUploadPage);
        }
        else if(BUTTON_FAVORITES == id)
        {
            if(NULL == m_pFavoritesPage)
            {
                m_pFavoritesPage = new FavoritesPage();
                connect(m_pFavoritesPage, &FavoritesPage::SigActionPreviewClicked, this, &MainWindowWidget::OnActionPreviewClicked);
            }

            ui->tabActionsVL->removeWidget(m_pActionListCurWidget);
            m_pActionListCurWidget->hide();
            ui->tabActionsVL->addWidget(m_pFavoritesPage);
            m_pFavoritesPage->show();
            m_pFavoritesPage->requestFavouiteActions(m_pFavoritesPage->getActionType());
            m_pActionListCurWidget = m_pFavoritesPage;
            m_nIdForActionWidget = id;

//            SAFE_DELETE(m_pMyUploadPage);
        }
        else if(BUTTON_MYUPLOADS == id)
        {
            if(NULL == m_pMyUploadPage)
            {
                m_pMyUploadPage = new MyUploadPage();
                connect(m_pMyUploadPage, &MyUploadPage::SigActionPreviewClicked, this, &MainWindowWidget::OnActionPreviewClicked);
            }
            ui->tabActionsVL->removeWidget(m_pActionListCurWidget);
            m_pActionListCurWidget->hide();
            ui->tabActionsVL->addWidget(m_pMyUploadPage);
            m_pMyUploadPage->show();
            m_pMyUploadPage->requestUploadActions(m_pMyUploadPage->getActionType());
            m_pActionListCurWidget = m_pMyUploadPage;
            m_nIdForActionWidget = id;

//            SAFE_DELETE(m_pFavoritesPage);
        }
        // 当从主题推荐动作页或运营活动推广页点击进入动作详细信息，然后在动作详细信息返回时，应该退回到上一级界面
        else if (BUTTON_RECOMMEND_SUBJECT_PAGE == id || BUTTON_BUSINESS_PROMOTE_PAGE == id)
        {
            ui->tabActionsVL->removeWidget(m_pActionListCurWidget);
            m_pActionListCurWidget->hide();

            if (BUTTON_RECOMMEND_SUBJECT_PAGE == id)
            {
                  ui->tabActionsVL->addWidget(m_pActionRecommendPage);
                  m_pActionListCurWidget = m_pActionRecommendPage;
            }
            else if (BUTTON_BUSINESS_PROMOTE_PAGE == id)
            {
                ui->tabActionsVL->addWidget(m_pBusinessPromoteWidget);
                m_pBusinessPromoteWidget->requestRankActionList(); // 从动作详情页评论返回后，需要重新刷新排行榜
                m_pActionListCurWidget = m_pBusinessPromoteWidget;
            }

            m_pActionListCurWidget->show();

            // 上一级界面应该是HomePage
            m_nIdForActionWidget = BUTTON_HOME_PAGE;
        }
    }
}

//下载按钮槽函数
void MainWindowWidget::OnDownloadBtnClicked()
{
    if(!g_userinfo.m_bLogin)
    {
        this->onUserLogin();

//        return;
    }

    // 如果没有登录
    if (!g_userinfo.m_bLogin)
    {
        return;
    }

    if(m_pActionDownload)
    {
        m_pActionDownload->show();
    }

    ui->pbDownLoads->setChecked(true);
}

void MainWindowWidget::onRefreshVersionIcon()
{
    const int nPCSoftVersionStatus = UBXUpdateAdministrator::getInstance()->getSystemPCSoftVersionUI()->getUpdateCheckStatus();

#ifndef Q_OS_MAC
    m_pTitleBar->setSoftVersionStatus(nPCSoftVersionStatus);
#else
    m_pActionRobotPCSoftVersion->setIcon(m_pTitleBar->getVersionStatusIcon(nPCSoftVersionStatus));

    bool bAlert = TitleBar::getAlertMarkFlag(nPCSoftVersionStatus);
    m_pActionRobotPCSoftVersion->setProperty(MENU_DATA_ALERT_KEY.toStdString().c_str(), bAlert);
#endif

    UBXUpdateAdministrator::getInstance()->checkUpdateSystemsEmbedded(false);

    QTimer::singleShot(4*60*60*1000, this, &MainWindowWidget::onRefreshVersionIcon);
}

void MainWindowWidget::onRefreshSystemEmbedVersionIcon(int nStatus)
{
#ifndef Q_OS_MAC
    m_pTitleBar->setFirmwareVersionStatus(nStatus);
#else
    m_pActionCheckFirmwareVersion->setIcon(m_pTitleBar->getVersionStatusIcon(nStatus));

    bool bAlert = TitleBar::getAlertMarkFlag(nStatus);
    m_pActionCheckFirmwareVersion->setProperty(MENU_DATA_ALERT_KEY.toStdString().c_str(), bAlert);
#endif
}

void MainWindowWidget::onRefreshVideoTutorialAlert()
{
    const int nVideoTutorialVersionStatus = UBXUpdateAdministrator::getInstance()->getVideoTutorialHandle()->getUpdateCheckStatus();

#ifndef Q_OS_MAC
    m_pTitleBar->setVideoTutorialStatus(nVideoTutorialVersionStatus);
#else    
    bool bAlert = TitleBar::getAlertMarkFlag(nVideoTutorialVersionStatus);
    m_pOpenVideoTutorialAction->setProperty(MENU_DATA_ALERT_KEY.toStdString().c_str(), bAlert);
#endif

    QTimer::singleShot(8*60*60*1000, this, &MainWindowWidget::onRefreshVideoTutorialAlert);
}

/************************************
* 名称: slotOnPortStatusChanged
* 功能: 机器人连接状态改变的槽函数
* 返回:   void
* 时间:   2016/07/27
* 作者:   hwx
* */
void MainWindowWidget::slotOnPortStatusChanged(bool bConnect)
{
    #ifdef Q_OS_MAC
    m_pActionCheckFirmwareVersion->setEnabled(bConnect);
    #endif
}

/************************************
* 名称: slotOnSettingLanguage
* 功能: 切换语言
* 返回:   void
* 时间:   2016/06/08
* 作者:   hwx
* */
void MainWindowWidget::slotOnSetting()
{
     QString sCfgFilePath = CConfigs::getLocalUserConfigPath("settings.ini");//now is not using the file path, is save to cofnig.ini

     SAFE_DELETE(m_pSettingDialog);

     if (NULL == m_pSettingDialog)
     {
          m_pSettingDialog = new SettingDialog(sCfgFilePath, this);
          m_pSettingDialog->setObjectName(QStringLiteral("m_pSettingDialog"));
     }

    m_pSettingDialog->loadAllSettingFromFile(sCfgFilePath);
    m_pSettingDialog->show();
}

/************************************
* 名称: slotOnOpenHelpDoc
* 功能: 打开帮助文档
* 返回:   void
* 时间:   2016/06/08
* 作者:   hwx
* */
void MainWindowWidget::slotOnOpenHelpDoc()
{
//    // 指定文件在服务器上的位置
//    QString strURL = "services.ubtrobot.com:8080";

    QString strLanguageKeyword = CIniConfig::getInstance().getValueLanguage();
    QString strURL;
    if (strLanguageKeyword.compare(LANGUAGE_CHINESE_POSTFIX) == 0)
    {
        strURL = ChineseHelpURL;
    }
    else if (strLanguageKeyword.compare(LANGUAGE_ENGLISH_POSTFIX) == 0)
    {
        strURL = EnglishHelpURL;
    }
    else
    {
        strURL = EnglishHelpURL;//其他国家语言使用英文帮助文档
    }

    char *pUrl = NULL;
   int nSize = Utf8ToAnsi(strURL.toStdString().c_str(), pUrl);  // URL已经编码，不需要再次编码
    QUrl urlDoc = QUrl::fromEncoded(pUrl);
//    urlDoc.setScheme("http");
    if (!urlDoc.isValid())
    {
        qDebug() << "Error: " << urlDoc.errorString() << endl;
        qCritical() << "Invalid url: " << urlDoc.toString() << endl;
        return;
    }
    QDesktopServices::openUrl(urlDoc);
}

/************************************
* 名称: slotOnVideoTutorial
* 功能: 打开视频教程帮助
* 返回:   void
* 时间:   20160822
* 作者:   asu
* */
void MainWindowWidget::slotOnVideoTutorial()
{
    QString strLanguageKeyword = CIniConfig::getInstance().getValueLanguage();
    QString strURL;
    if (strLanguageKeyword.compare(LANGUAGE_CHINESE_POSTFIX) == 0)
    {
        strURL = ChineseVideoTutorialURL;
    }
    else if (strLanguageKeyword.compare(LANGUAGE_ENGLISH_POSTFIX) == 0)
    {
        strURL = EnglishVideoTutorialURL;
    }
    else
    {
        strURL = EnglishVideoTutorialURL;//其他国家才有英语视频教程
    }

    char *pUrl = NULL;
    int nSize = Utf8ToAnsi(strURL.toStdString().c_str(), pUrl);  // URL已经编码，不需要再次编码
    QUrl urlDoc = QUrl::fromEncoded(pUrl);
    urlDoc.setScheme("http");
    if (!urlDoc.isValid())
    {
        qDebug() << "Error: " << urlDoc.errorString() << endl;
        qCritical() << "Invalid url: " << urlDoc.toString() << endl;
        return;
    }

    QDesktopServices::openUrl(urlDoc);
}

/************************************
* 名称: slotOnCheckUpdateSysEmbedVersion
* 功能: 检查更新嵌入式软件版本
* 返回:   void
* 时间:   2016/07/22
* 作者:   hwx
* */
void MainWindowWidget::slotOnCheckUpdateSysEmbedVersion()
{
    UBXUpdateAdministrator::getInstance()->checkUpdateSystemsEmbedded(true);
    UBXWidget::execByAppModal(UBXUpdateAdministrator::getInstance()->getSysEmbedUpdateWidget());
}

void MainWindowWidget::slotOnShowSystemPCSoftVersion()
{    
    UBXUpdateAdministrator::getInstance()->showSytemPCSoftVersionUI();
}

#ifdef Q_OS_MAC
void MainWindowWidget::slotOnAppStateChanged(Qt::ApplicationState state)
{
    //qDebug() << "App state changed: " << state << endl;
    switch (state) {
    case Qt::ApplicationActive:
    {
        if (m_pRobotControlWidget && m_bRobotCtrlWidgetHasFirstShow)
        {
            m_pRobotControlWidget->show();
        }
        break;
    }
    case Qt::ApplicationHidden:
    case Qt::ApplicationInactive:
    case Qt::ApplicationSuspended:
    {
        if (m_pRobotControlWidget)
        {
            m_pRobotControlWidget->hide();
        }
        break;
    }
    default:
        break;
    }

}
#endif

/************************************
* 名称: OnMoreBtnClicked
* 功能: 更多按钮槽函数
* 参数: [in]actGroup 动作查询组类型
* 参数: [in]actType 动作类型
* 返回:   void
* 时间:   2016/04/12
* 作者:   hels
************************************/
void MainWindowWidget::OnMoreBtnClicked(emActionGroup actGroup, emActionType actType)
{
    ui->pbActions->setChecked(true);
    if(m_pActionListPage)
    {
        m_pActionListPage->SetQueryAction(actGroup, actType);
    }
}

/************************************
* 名称: OnActionPreviewClicked
* 功能: 点击预览图时响应
* 参数: [in]itemData 对应的动作数据
* 返回:   void
* 时间:   2016/04/15
* 作者:   hels
************************************/
void MainWindowWidget::OnActionPreviewClicked(ActItemData itemData)
{
    showActionDetailedPage(itemData);
//    if(NULL != m_pActionListCurWidget)
//    {
//        ui->tabActionsVL->removeWidget(m_pActionListCurWidget);
//        m_pActionListCurWidget->hide();
//    }

//    if(NULL == m_pActionDetail)
//    {
//        m_pActionDetail = new ActionDetailWidget();
//        connect(m_pActionDetail, &ActionDetailWidget::SigGoBack, this, &MainWindowWidget::OnGoBack);
//    }

//    ui->tabActionsVL->addWidget(m_pActionDetail);
//    m_pActionListCurWidget = m_pActionDetail;
//    m_pActionDetail->show();
//    m_pActionDetail->SetItemData(itemData);
}

/************************************
* 名称: onFlowCoverActionPreviewClicked
* 功能: 点击flow流图弹出的子窗口中的动作时的槽函数
* 参数: [in] nPageId 页标识
* 参数: [in]itemData 对应的动作数据
* 返回:   void
* 时间:   2016/07/18
* 作者:   hwx
************************************/
void MainWindowWidget::onFlowCoverActionPreviewClicked(int nPageId, ActItemData itemData)
{
    // 记录当前页，以便在详细信息预览返回时退回到当前页面
    m_nIdForActionWidget = nPageId;

    showActionDetailedPage(itemData);
}

/************************************
* 名称: slotOnRecommendFlowImageClicked
* 功能: 点击flowcover推荐主题图片时响应
* 参数: [in]pRecommendItem 对应的动作数据
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void MainWindowWidget::slotOnRecommendFlowImageClicked(RecommendActItemData *pRecommendItem)
{
    if(NULL != m_pActionListCurWidget)
    {
        ui->tabActionsVL->removeWidget(m_pActionListCurWidget);
        m_pActionListCurWidget->hide();
    }

    if(NULL == m_pActionRecommendPage)
    {
        m_pActionRecommendPage = new ActionRecommendPage();
        connect(m_pActionRecommendPage, &ActionRecommendPage::sigGoBack, this, &MainWindowWidget::OnGoBack);
        connect(m_pActionRecommendPage, &ActionRecommendPage::sigActionDetailedClicked, this, &MainWindowWidget::onFlowCoverActionPreviewClicked, Qt::DirectConnection);
    }

    ui->tabActionsVL->addWidget(m_pActionRecommendPage);
    m_pActionListCurWidget = m_pActionRecommendPage;
    m_pActionRecommendPage->updateData(pRecommendItem);
    m_pActionRecommendPage->show();
}

/************************************
* 名称: slotOnBusinessPromoteImageClicked
* 功能: 点击flowcover活动推广图片时响应
* 参数: [in]pPromoteItem 活动推广数据
* 返回:   void
* 时间:   2016/06/17
* 作者:   hwx
************************************/
void MainWindowWidget::slotOnBusinessPromoteImageClicked(BusinessPromoteData *pPromoteItem)
{
    if(NULL != m_pActionListCurWidget)
    {
        ui->tabActionsVL->removeWidget(m_pActionListCurWidget);
        m_pActionListCurWidget->hide();
    }

    if(NULL == m_pBusinessPromoteWidget)
    {
        m_pBusinessPromoteWidget = new BusinessPromoteWidget(this);
        connect(m_pBusinessPromoteWidget, &BusinessPromoteWidget::sigOnGoBackClicked, this, &MainWindowWidget::OnGoBack);
        connect(m_pBusinessPromoteWidget, &BusinessPromoteWidget::sigOnActionUpload, this, &MainWindowWidget::slotOnActionUpload);
        connect(g_actupdatemgr, &CActUpdateMangr::sigOnUpdatePromoteData, m_pBusinessPromoteWidget, &BusinessPromoteWidget::slotUpdateData);
        connect(m_pBusinessPromoteWidget, &BusinessPromoteWidget::sigDetailedActionPreviewClicked, this, &MainWindowWidget::onFlowCoverActionPreviewClicked);
    }

    ui->tabActionsVL->addWidget(m_pBusinessPromoteWidget);
    m_pActionListCurWidget = m_pBusinessPromoteWidget;
    m_pBusinessPromoteWidget->slotUpdateData(pPromoteItem);
    m_pBusinessPromoteWidget->requestRankActionList();
    m_pBusinessPromoteWidget->show();
}

/************************************
* 名称: slotOnUpdatePopUp
* 功能: 显示活动推广弹窗
* 参数: [in]strImageFile 图片文件
* 返回:   void
* 时间:   2016/08/31
* 作者:   hwx
************************************/
void MainWindowWidget::slotOnUpdatePopUp(const QString &strImageFile)
{
    if (!m_pPromotePopUpWidget)
    {
        m_pPromotePopUpWidget = new CPromotePopUpWidget(this);
        connect(m_pPromotePopUpWidget, &CPromotePopUpWidget::sigOnImageClicked, m_pActionLibWidget, &ActionLibWidget::slotOnClickPopUpPage);
        connect(m_pPromotePopUpWidget, &CPromotePopUpWidget::sigSwitchTab, m_pTitleBar, &TitleBar::slotOnSwitchTab);
    }
    m_pPromotePopUpWidget->updateImage(strImageFile);
    CPromotePopUpWidget::execByAppModal(m_pPromotePopUpWidget);
}

/************************************
* 名称: slotOnActionUpload
* 功能: 上传动作槽函数
* 参数: [in]nSchemId 运营活动id
* 返回:   void
* 时间:   2016/09/09
* 作者:   hwx
************************************/
void MainWindowWidget::slotOnActionUpload(int nSchemId)
{
    if(-1 == g_actupdatemgr->GetUserID())//未登录时不能上传
    {
        NotifyObservers(NOTIFY_MSG_LOGIN_REQUEST, NULL, NULL, NULL);
    }

    // 如果用户没有选择登录，则返回，否则进入上传界面
    if (-1 == g_actupdatemgr->GetUserID())
    {
        return;
    }

    if(m_pActionUploadAddAct)
    {
        m_pActionUploadAddAct->ClearContents();//调试模式下为方便调试，不清除内容
        m_pActionUploadAddAct->setSchemId(nSchemId);
        m_pActionUploadAddAct->show();
    }
}

/************************************
* 名称: OnGoBack
* 功能: 响应详情页返回操作
* 返回:   void
* 时间:   2016/04/16
* 作者:   hels
************************************/
void MainWindowWidget::OnGoBack()
{
    OnActionLibBGToggled(m_nIdForActionWidget, true);
}

void MainWindowWidget::onUserMessageEdit()
{
    if(m_pLoginDlg)
    {
        m_pLoginDlg->UserMessageEdit();
    }
}

/**************************************************************************
* 函数名:CreateLoginPopMenuActions
* 功能:点击用户名弹出菜单
* 参数:
* 返回值:
* 时间:2016/03/01
* 作者: 周志平
* 改动日志
**************************************************************************/
void MainWindowWidget::CreateLoginPopMenuActions()
{
    //创建菜单、菜单项
    QMenu* pop_menu = new QMenu();
    QAction* UserInfoAction = new QAction(tr("userinfo"),this);
    QAction* QuitLoginAction = new QAction(tr("logout"),this);

    pop_menu->addAction(UserInfoAction);
    pop_menu->addAction(QuitLoginAction);

    //连接信号与槽
    QAbstractItemDelegate::connect(UserInfoAction,&QAction::triggered,this,&MainWindowWidget::onUserMessageEdit);
    QAbstractItemDelegate::connect(QuitLoginAction,&QAction::triggered,this,&MainWindowWidget::onUserLogout);

    //菜单出现的位置为当前鼠标的位置
    QFile styleSheet(":/res/qss/alphaMenuDefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        pop_menu->setStyleSheet(styleSheet.readAll());
    }
    //菜单出现的位置为当前鼠标的位置
    pop_menu->exec(QCursor::pos());
    pop_menu->deleteLater();
}

#ifdef Q_OS_WIN
bool MainWindowWidget::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    if(eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* pMsg = reinterpret_cast<MSG*>(message);
        if(pMsg->message == WM_DEVICECHANGE)
        {
            qDebug()<<"MainWindow windows system message WM_DEVICECHANGE";

            if(g_pRobotController)
                g_pRobotController->OnDeviceChange(pMsg, result);
        }
    }
    return false;
}
#endif

//动作上传按钮槽函数
void MainWindowWidget::OnActionUploadBtnClicked()
{
    slotOnActionUpload(-1);
}

/************************************
* 名称: onUpdateDownLoadBtnStatus
* 功能: update down load button status
* 参数: void
* 返回:   void
* 时间:   20160602
* 作者:   suhaiwu
************************************/
void MainWindowWidget::onUpdateDownLoadBtnStatus()
{
    if( !m_timer.isActive() )
    {
        ui->pbDownLoads->setFixedSize(20, 20);
        //        ui->pbDownLoads->setStyleSheet("border-image: url(:/res/images/btn_nav_hl.png);"
        //                                            "qproperty-icon: url(:/res/images/btn_download.png);"
        //                                            "qproperty-iconSize: 18px 18px;"
        //                                            "font:12pt;"
        //                                            "color: #cfcfcf;");

        ui->pbDownLoads->setStyleSheet("border-image: url(:/res/images/btn_nav_hl.png);"
                                       "qproperty-iconSize: 20px 20px;"
                                       "font:12pt;"
                                       "color: #cfcfcf;");
    }

    ui->pbDownLoads->setMovie(QString(":/res/images/btn_download.gif"));

    m_timer.start(1500);
}

/************************************
* 名称: onRestoreDownLoadBtn
* 功能: restore down load button status
* 参数: void
* 返回:   void
* 时间:   20160607
* 作者:   suhaiwu
************************************/
void MainWindowWidget::onRestoreDownLoadBtn()
{
    //        ui->pbDownLoads->setFixedSize(25, 25);
    ui->pbDownLoads->setMovie(false);
    ui->pbDownLoads->setStyleSheet(m_strBntDownLoadsOldStyle);
}

/************************************
* 名称: slotOnDetailedWidgetMoreClicked
* 功能: 动作详情页点击更多按钮的槽函数，跳转到动作页
* 参数：[in] eGroup：动作组类，如最热的
* 参数：[in] eType：动作类型，如舞蹈
* 返回:   void
* 时间:   20160607
* 作者:   hwx
************************************/
void MainWindowWidget::slotOnDetailedWidgetMoreClicked(emActionGroup eGroup, emActionType eType)
{
    QAbstractButton *pBtnActionLib= m_btnGroupForActionLib.button(BUTTON_ACTIONS_LIBRARY);
    if (!pBtnActionLib)
    {
        return;
    }

    pBtnActionLib->setChecked(true);

    // 需要手动设置，因为如果原来就是动作库，setChecked()后信号槽不会触发
    OnActionLibBGToggled(BUTTON_ACTIONS_LIBRARY, true);
    if (m_pActionListPage)
    {
        m_pActionListPage->setButtonCurrentChecked(eGroup, eType);
    }
}

/************************************
* 名称: slotOnDelayLoaded
* 功能: 延时加载的槽函数
* 返回:   void
* 时间:   20160901
* 作者:   hwx
************************************/
void MainWindowWidget::slotOnDelayLoaded()
{
    if (m_pActionLibWidget)
    {
        // 请求推荐主题
        m_pActionLibWidget->requestRecommendActionList();
    }
}

/************************************
* 名称: slotOnMainWinShow
* 功能: 主界面显示后的槽函数
* 返回:   void
* 时间:   20170216
* 作者:   hwx
************************************/
void MainWindowWidget::slotOnMainWinShow()
{
    // 主界面显示后再更新控制按钮位置
    updateRobotControlWidgetPos();

//    robotTypeSettingWdg robotTypeSetWdg;
//    robotTypeSetWdg.show();
}

void MainWindowWidget::timerEvent(QTimerEvent */*event*/)
{

}

void MainWindowWidget::retranslateUi()
{
    ui->retranslateUi(this);

    ////////////file
    if(m_newAct)
        m_newAct->setText(tr("New Project(&N)"));
    if(m_openAct)
        m_openAct->setText(tr("Open Project(&O)"));
    if(m_saveAct)
    m_saveAct->setText(tr("Save Project(&S)"));
    if(m_saveAsAct)
        m_saveAsAct->setText(tr("Project SaveAs(&A)"));
    if(m_exportHts)
        m_exportHts->setText(tr("Export to Hts"));

    if(m_FileMenu)
        m_FileMenu->setTitle(tr("File(&F)"));


    ////////////help
    if(m_pHelpMenu)
        m_pHelpMenu->setTitle(tr("Help(&H)"));

    if(m_pOpenHelpDocAction)
        m_pOpenHelpDocAction->setText(tr("Document(&D)"));
    if(m_pOpenVideoTutorialAction)
        m_pOpenVideoTutorialAction->setText(tr("Video Tutorial"));
    if(m_pActionCheckFirmwareVersion)
        m_pActionCheckFirmwareVersion->setText(tr("Robot Fireware Version"));
    if(m_pActionRobotPCSoftVersion)
        m_pActionRobotPCSoftVersion->setText(tr("Robot Soft Version"));

    //
    if(!m_bProjectTitleTabTextHadSet)
    {
        setUIMainLayerLayoutTabText(tr("untitled"), false);
    }
}

/**************************************************************************
* 函数名: OnNotify
* 功能: 通知消息
* 参数:
*    @[in ] nMsgID: 消息ID
*    @[in ] dwContext: 消息上下文
*    @[in ] wParam: 参数1
*    @[in ] lParam: 参数2
* 返回值: void
* 时间: 2016/06/06
* 作者: suhaiwu
*/
void MainWindowWidget::OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM Param)
{
    if(NOTIFY_MSG_LOGIN_REQUEST == nMsgID)
    {
        this->onUserLogin();
        return;
    }
    else if (WM_MSG_ROBOT_TF_CARD_STATUS == nMsgID)
    {
        if (wParam)
        {
            // 机器人存在内存卡
            AlphaRobotLog::getInstance()->printDebug(QString("Find robot TF card"));
        }
        else
        {
            CMessageBox::critical(this, tr("Warning"), tr("Failed to detected the TF card of robot"));
        }
//        emit sigOnRcvRobotTFCardStatus(wParam);
        doProcessUTFCardMsg(wParam, Param);
        return;
    }
    else if (NOTIFY_MSG_PLAY_MOTION == nMsgID || NOTIFY_MSG_PLAY_MUSIC == nMsgID || NOTIFY_MSG_UNITED_PLAY == nMsgID)
    {
        doProcessSimulateMsg(nMsgID, (emPlayStatus)Param);
        return;
    }
}

/**************************************************************************
* 函数名: doProcessUTFCardMsg
* 功能: 处理内存卡消息
* 参数:
*    @[in ] wParam: 是否插上内存卡标志
*    @[in ] lParam:
* 返回值: void
* 时间: 2017/03/17
* 作者: hwx
*/
void MainWindowWidget::doProcessUTFCardMsg(WPARAM wParam, LPARAM lParam)
{
    if (wParam)
    {
        // 机器人存在内存卡
        AlphaRobotLog::getInstance()->printDebug(QString("Find robot TF card"));
    }
    else
    {
        CMessageBox::critical(this, tr("Warning"), tr("Failed to detected the TF card of robot"));
    }
    emit sigOnRcvRobotTFCardStatus(wParam);
}

/**************************************************************************
* 函数名: doProcessSimulateMsg
* 功能: 处理播放音乐、动作消息
* 参数:
*    @[in ] nMsgID: 消息ID
*    @[in ] eStatus: 播放状态
* 返回值: void
* 时间: 2017/03/17
* 作者: hwx
*/
void MainWindowWidget::doProcessSimulateMsg(int nMsgID, emPlayStatus eStatus)
{
    switch (eStatus) {
    case ePlayStatusPlaying:
    {
        setButtonStatus(false);
        break;
    }
    case ePlayStatusPause:
    case ePlayStatusStop:
    case ePlayStatusFinished:
    {
        setButtonStatus(true);
        break;
    }
    default:
        break;
    }
}

void MainWindowWidget::setButtonStatus(bool bEnable)
{
    ui->pbNew->setEnabled(bEnable);
    ui->pbOpen->setEnabled(bEnable);
    ui->pbUndo->setEnabled(bEnable);
    ui->pbRedo->setEnabled(bEnable);
}

void MainWindowWidget::onRobotProductTypeChange(int nOldType, int nNewType)
{
    emUbtProductType eProductTypeNew = (emUbtProductType)nNewType;
    QString strProductTypeNew = UBXBase::productTypeToStr(eProductTypeNew);
    if(!strProductTypeNew.isEmpty())
    {
        CIniConfig::getInstance().setValueRobotType(strProductTypeNew);
    }

    QMessageBox::StandardButton status = CMessageBox::warning(this,
                                                              tr("Warning"),
                                                              tr("Robot type has changed, do you want to manual modify the invalid action angles ?"),
                                                              QMessageBox::Yes | QMessageBox::No,
                                                              QMessageBox::No);

    emAngleChectWayType eCheckWay = eAngleCheckWayAuto;
    if(QMessageBox::Yes == status)
    {
        eCheckWay = eAngleCheckWayManual;
    } else {
        eCheckWay = eAngleCheckWayAuto;
    }

    UBXBase::getInstance()->sigAngleValueCheckWay(eCheckWay);
}
