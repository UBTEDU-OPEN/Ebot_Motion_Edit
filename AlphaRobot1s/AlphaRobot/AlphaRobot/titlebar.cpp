#include "titlebar.h"
#include "ui_titlebar.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFont>



#include "../UBXUpdateModule/global.h"
#include "../zxLib/widget/menu.h"



const QString MENU_DEFAULT_ALERT_IMAGE = ":/res/images/btn_close.png";


//RETRANSLATEUIFUNCIMPL(TitleBar)


TitleBar::TitleBar(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::TitleBar),
    m_pWinMenu(NULL),
    m_pActionOpenHelpDoc(NULL),
    m_pActionVideoTutorial(NULL),
    m_pActionRobotFirmwareVersion(NULL),
    m_pActionRobotPCSoftVersion(NULL),
    m_bIsWindowMax(true)
{
    ui->setupUi(this);
    ui->pBUserLogin->setAttribute(Qt::WA_TranslucentBackground, true);
    m_btnGroup.addButton(ui->tbEdit, (int)eTitleBarTabEdit);
    m_btnGroup.addButton(ui->tbActions, (int)eTitleBarTabActionLib);
    m_btnGroup.addButton(ui->tbSync, (int)eTitleBarTabActionSync);
    connect(&m_btnGroup, static_cast<void (QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled),
            this, &TitleBar::onButtonToggled);
    ui->tbEdit->setChecked(true);
    //以下代码是因为在stylesheet中设置有问题
    ui->tbEdit->setIcon(QIcon(":/res/images/icon_edit_sel.png"));
    ui->tbSync->setIcon(QIcon(":/res/images/icon_tb.png"));
    ui->labelWelcome->hide(); // 未登陆时不展示欢迎标签

#ifdef Q_OS_OSX
    ui->pbWin_Close->hide();
    ui->pbWin_Min->hide();
    ui->pbWin_Max->hide();
    ui->pbWin_Menu->hide();

    ui->pbClose->installEventFilter(this);
    ui->pbMin->installEventFilter(this);
    ui->pbMax->installEventFilter(this);

    ui->pbClose->setStyleSheet("QPushButton#pbClose{border-image: url(:/res/images/btn_close.png);}");
    ui->pbMin->setStyleSheet("QPushButton#pbMin{border-image: url(:/res/images/btn_min.png);}");
    ui->pbMax->setStyleSheet("QPushButton#pbMax{border-image: url(:/res/images/btn_max.png);}");

    connect(ui->pbClose, &QPushButton::clicked, this, &TitleBar::OnPbCloseClicked);
    connect(ui->pbMin, &QPushButton::clicked, this, &TitleBar::OnPbMinClicked);
    connect(ui->pbMax, &QPushButton::clicked, this, &TitleBar::OnPbMaxClicked);
#else
    ui->pbClose->hide();
    ui->pbMin->hide();
    ui->pbMax->hide();

    ui->pbWin_Close->installEventFilter(this);
    ui->pbWin_Min->installEventFilter(this);
    ui->pbWin_Max->installEventFilter(this);

    ui->pbWin_Close->setStyleSheet("QPushButton#pbWin_Close{border-image: url(:/res/images/icon_win_close_nor.png);}"
                                   "QPushButton#pbWin_Close:hover{border-image: url(:/res/images/icon_win_close_sel.png);}"
                                   "QPushButton#pbWin_Close:pressed{border-image: url(:/res/images/icon_win_close_press.png);}");
    ui->pbWin_Min->setStyleSheet("QPushButton#pbWin_Min{border-image: url(:/res/images/icon_win_min_nor.png);}"
                                 "QPushButton#pbWin_Min:hover{border-image: url(:/res/images/icon_win_min_sel.png);}"
                                 "QPushButton#pbWin_Min:pressed{border-image: url(:/res/images/icon_win_min_press.png);}");
    ui->pbWin_Max->setStyleSheet("QPushButton#pbWin_Max{border-image: url(:/res/images/icon_win_restore_nor.png);}"
                                 "QPushButton#pbWin_Max:hover{border-image: url(:/res/images/icon_win_restore_sel.png);}"
                                 "QPushButton#pbWin_Max:pressed{border-image: url(:/res/images/icon_win_restore_press.png);}");

    connect(ui->pbWin_Close, &QPushButton::clicked, this, &TitleBar::OnPbCloseClicked);
    connect(ui->pbWin_Min, &QPushButton::clicked, this, &TitleBar::OnPbMinClicked);
    connect(ui->pbWin_Max, &QPushButton::clicked, this, &TitleBar::OnPbMaxClicked);

    onMenuAlertMark(false);

    connect(ui->pbWin_Menu, &QPushButton::clicked, this, &TitleBar::on_menu_clicked);

    createMenu();

    QMargins marginTemp = ui->horizontalLayout->contentsMargins();
    marginTemp.setTop(12);
    ui->horizontalLayout->setContentsMargins(marginTemp);
#endif
//    ui->pbConnectRobot->setStyleSheet("QPushButton#pbConnectRobot{background-image: url(:/res/images/icon_disconnec_nor.png);border-image: url(:/res/images/btn_head_nor.png);}"
//                                      "QPushButton#pbConnectRobot:hover{background-image: url(:/res/images/icon_disconnec_nor.png);border-image: url(:/res/images/btn_head_hl.png);}"
//                                      "QPushButton#pbConnectRobot:pressed{background-image: url(:/res/images/icon_disconnec_nor.png);border-image: url(:/res/images/btn_head_press.png);}");

    OnPortOpenChanged(false);
}



TitleBar::~TitleBar()
{
    delete ui;
}

void TitleBar::retranslateUi()
{
    ui->retranslateUi(this);
    SAFE_DELETE(m_pWinMenu);
    createMenu();
}

//创建菜单
void TitleBar::createMenu()
{
    //添加菜单
    m_pWinMenu = new CMenu("", this);

    QMenu *pMenuFile = new QMenu(tr("File"), m_pWinMenu);
    QAction *pActionNew = new QAction(tr("New"), pMenuFile);
    QAction *pActionOpen = new QAction(tr("Open"), pMenuFile);
    QAction *pActionSave = new QAction(tr("Save"), pMenuFile);
    QAction *pActionSaveAs = new QAction(tr("Save As"), pMenuFile);
    QAction *pActionExportHTS = new QAction(tr("Export HTS"), pMenuFile);
    QAction *pActionExportMixXml = new QAction(tr("Export XML"), pMenuFile);
    pMenuFile->addAction(pActionNew);
    pMenuFile->addAction(pActionOpen);
    pMenuFile->addAction(pActionSave);
    pMenuFile->addAction(pActionSaveAs);
    pMenuFile->addAction(pActionExportHTS);
    pMenuFile->addAction(pActionExportMixXml);
    m_pWinMenu->addMenu(pMenuFile);

    connect(pActionNew, &QAction::triggered, this, &TitleBar::sigProjectNew);
    connect(pActionOpen, &QAction::triggered, this, &TitleBar::sigProjectOpen);
    connect(pActionSave, &QAction::triggered, this, &TitleBar::sigProjectSave);
    connect(pActionSaveAs, &QAction::triggered, this, &TitleBar::sigProjectSaveAs);
    connect(pActionExportHTS, &QAction::triggered, this, &TitleBar::sigExportHts);
    connect(pActionExportMixXml, &QAction::triggered, this, &TitleBar::sigExportActMixXml);


    ///////////////////////////////////////////////////////////////////////////////////////
    // 视图菜单暂时不需要 huangwuxian 2016-07-20
    //////////////////////////////////////////////////////////////////////////////////////
//    QAction* pMenuView = new QAction(tr("View"), m_pWinMenu);
//    connect(pMenuView, &QAction::triggered, this, &TitleBar::on_menuview_clicked);
//    m_pWinMenu->addAction(pMenuView);

    QAction* pMenuSetup = new QAction(tr("Setup"), m_pWinMenu);
    connect(pMenuSetup, &QAction::triggered, this, &TitleBar::sigOnSetting);
    m_pWinMenu->addAction(pMenuSetup);

    CMenu* pMenuHelp = new CMenu(tr("Help"), m_pWinMenu);
    connect(pMenuHelp, &QMenu::triggered, this, &TitleBar::on_menuhelp_clicked);
    m_pWinMenu->addMenu(pMenuHelp);
    m_pWinMenu->setEnableAlert(true);
    m_pWinMenu->setAlertImage(MENU_DEFAULT_ALERT_IMAGE);
    m_pWinMenu->setAlertRightMargin(20);
    m_pWinMenu->setProperty(MENU_DATA_ALERT_KEY.toStdString().c_str(), true);

    pMenuHelp->setEnableAlert(true);
    pMenuHelp->setAlertImage(MENU_DEFAULT_ALERT_IMAGE);
    pMenuHelp->setProperty(MENU_DATA_ALERT_KEY.toStdString().c_str(), true);

    m_pActionOpenHelpDoc = new QAction(tr("Document(&D)"), pMenuHelp);
    pMenuHelp->addAction(m_pActionOpenHelpDoc);
    connect(m_pActionOpenHelpDoc, &QAction::triggered, this, &TitleBar::sigOpenHelpDoc);

    m_pActionVideoTutorial = new QAction(tr("Video Tutorial"), pMenuHelp);
    connect(m_pActionVideoTutorial, &QAction::triggered, this, &TitleBar::sigOpenVideoTutorial);
//    pMenuHelp->addAction(m_pActionVideoTutorial);

    m_pActionRobotFirmwareVersion = new QAction(tr("Robot Fireware Version"), pMenuHelp);
    pMenuHelp->addAction(m_pActionRobotFirmwareVersion);
    m_pActionRobotFirmwareVersion->setEnabled(false);
    connect(m_pActionRobotFirmwareVersion, &QAction::triggered, this, &TitleBar::sigCheckUpdateSysEmbed);

    m_pActionRobotPCSoftVersion = new QAction(tr("Robot Soft Version"), pMenuHelp);
    pMenuHelp->addAction(m_pActionRobotPCSoftVersion);
    connect(m_pActionRobotPCSoftVersion, &QAction::triggered, this, &TitleBar::sigPCSoftVersion);

    QAction* pMenuAbout = new QAction(tr("About"), pMenuHelp);
    connect(pMenuAbout, &QAction::triggered, this, &TitleBar::on_menuabout_clicked);
    pMenuHelp->addAction(pMenuAbout);

    connect(pMenuHelp, &CMenu::sigAlertMark, this, &TitleBar::onMenuAlertMark);

    m_pWinMenu->hide();

    m_pWinMenu->setStyleSheet("QMenu{background: transparent; border: 1px solid black; border-radius: 5px;}"
                              "QMenu::item{background-color: #484848;color: #d3d3d3; font: 14pt;}"
                              "QMenu::item:selected{background-color: #375270;}");
}

void TitleBar::showWelcomeLable(bool bShow)
{
    if (bShow)
    {
        ui->labelWelcome->show();
    }
    else
    {
        ui->labelWelcome->hide();
    }
}

void TitleBar::setVideoTutorialStatus(const int nStatus)
{
    bool bAlert = TitleBar::getAlertMarkFlag(nStatus);
    m_pActionVideoTutorial->setProperty(MENU_DATA_ALERT_KEY.toStdString().c_str(), bAlert);

    bool bAlertMenu = m_pWinMenu->isAlertMark();
    onMenuAlertMark(bAlertMenu);
}

void TitleBar::setSoftVersionStatus(const int nStatus)
{
    m_pActionRobotPCSoftVersion->setIcon(getVersionStatusIcon(nStatus));

    bool bAlert = TitleBar::getAlertMarkFlag(nStatus);
    m_pActionRobotPCSoftVersion->setProperty(MENU_DATA_ALERT_KEY.toStdString().c_str(), bAlert);

    bool bAlertMenu = m_pWinMenu->isAlertMark();
    onMenuAlertMark(bAlertMenu);
}

void TitleBar::setFirmwareVersionStatus(const int nStatus)
{
    m_pActionRobotFirmwareVersion->setIcon(getVersionStatusIcon(nStatus));

    bool bAlert = TitleBar::getAlertMarkFlag(nStatus);
    m_pActionRobotFirmwareVersion->setProperty(MENU_DATA_ALERT_KEY.toStdString().c_str(), bAlert);

    bool bAlertMenu = m_pWinMenu->isAlertMark();
    onMenuAlertMark(bAlertMenu);
}

QIcon TitleBar::getVersionStatusIcon(const int nStatus) const
{
    if(CHECKRESULT_NEEDUPDATE == nStatus)
    {
        return QIcon(":/res/images/btn_download.png");
    }

    else
    {
        return QIcon();
    }
}

void TitleBar::clearVideoTutorialStatus()
{
    setVideoTutorialStatus(CHECKRESULT_NOUPDATE);
}

void TitleBar::clearSoftVersionStatus()
{
    setSoftVersionStatus(CHECKRESULT_NOUPDATE);
}

void TitleBar::clearFirmwareVersionStatus()
{
    setFirmwareVersionStatus(CHECKRESULT_NOUPDATE);
}

bool TitleBar::getAlertMarkFlag(const int nStation)
{
    bool bAlertMark = false;
    switch (nStation)
    {
    case CHECKRESULT_NEEDUPDATE:
        bAlertMark = true;
        break;

    default:
        bAlertMark = false;
        break;
    }

    return bAlertMark;
}

void TitleBar::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(e);
}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
#ifdef Q_OS_OSX
    if (obj == ui->pbClose || obj == ui->pbMin || obj == ui->pbMax)
    {
        if(event->type() == QEvent::Enter ||
           event->type() == QEvent::HoverEnter)
        {
            ui->pbClose->setStyleSheet("QPushButton#pbClose{background-image: url(:/res/images/btn_close.png);border-image: url(:/res/images/icon_close.png);}");
            ui->pbMin->setStyleSheet("QPushButton#pbMin{background-image: url(:/res/images/btn_min.png);border-image: url(:/res/images/icon_min.png);}");
            ui->pbMax->setStyleSheet("QPushButton#pbMax{background-image: url(:/res/images/btn_max.png);border-image: url(:/res/images/icon_max.png);}");
        }
        else if(event->type() == QEvent::Leave ||
           event->type() == QEvent::HoverLeave)
        {
            ui->pbClose->setStyleSheet("QPushButton#pbClose{background-image: url(:/res/images/icon_close.png);border-image: url(:/res/images/btn_close.png);}");
            ui->pbMin->setStyleSheet("QPushButton#pbMin{background-image: url(:/res/images/icon_min.png);border-image: url(:/res/images/btn_min.png);}");
            ui->pbMax->setStyleSheet("QPushButton#pbMax{background-image: url(:/res/images/icon_max.png);border-image: url(:/res/images/btn_max.png);}");
        }
    }
#else
//    if(event->type() == QEvent::Enter ||
//       event->type() == QEvent::HoverEnter)
//    {
//        if(obj == ui->pbWin_Close)
//            ui->pbWin_Close->setStyleSheet("QPushButton#pbClose{border-image: url(:/res/images/icon_win_close_sel.png);}");
//        else if(obj == ui->pbWin_Min)
//            ui->pbWin_Min->setStyleSheet("QPushButton#pbWin_Min{border-image: url(:/res/images/icon_win_min_sel.png);}");
//        else if(obj == ui->pbWin_Max)
//            ui->pbWin_Max->setStyleSheet("QPushButton#pbWin_Max{border-image: url(:/res/images/icon_win_max_sel.png);}");
//    }
//    else if(event->type() == QEvent::Leave ||
//            event->type() == QEvent::HoverLeave)
//    {
//        if(obj == ui->pbWin_Close)
//            ui->pbWin_Close->setStyleSheet("QPushButton#pbClose{border-image: url(:/res/images/icon_win_close_nor.png);}");
//        if(obj == ui->pbWin_Min)
//            ui->pbWin_Min->setStyleSheet("QPushButton#pbWin_Min{border-image: url(:/res/images/icon_win_min_nor.png);}");
//        if(obj == ui->pbWin_Max)
//            ui->pbWin_Max->setStyleSheet("QPushButton#pbWin_Max{border-image: url(:/res/images/icon_win_max_nor.png);}");
//    }

//    if(event->type() == QEvent::MouseButtonPress)
//    {
//        if(m_pWinMenu && m_pWinMenu->isVisible())
//            m_pWinMenu->hide();
//    }

#endif
    return QWidget::eventFilter(obj, event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit sigOnDoubleClicked();
    QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::onButtonToggled(int id, bool checked)
{
    if(checked)
    {
        emit clickTab(id);
        //在style sheet中设置checked状态是，字体和颜色都可以改变，单独icon无法切换
        if(id == eTitleBarTabEdit)
        {
            ui->tbActions->setIcon(QIcon(":/res/images/icon_dzk.png"));
            ui->tbEdit->setIcon(QIcon(":/res/images/icon_edit_sel.png"));
            ui->tbSync->setIcon(QIcon(":/res/images/icon_tb.png"));
        }
        else if(id == eTitleBarTabActionLib)
        {
            ui->tbActions->setIcon(QIcon(":/res/images/icon_dzk_sel.png"));
            ui->tbEdit->setIcon(QIcon(":/res/images/icon_edit.png"));
            ui->tbSync->setIcon(QIcon(":/res/images/icon_tb.png"));
        }
        else if(id == eTitleBarTabActionSync)
        {
            ui->tbActions->setIcon(QIcon(":/res/images/icon_dzk.png"));
            ui->tbEdit->setIcon(QIcon(":/res/images/icon_edit.png"));
            ui->tbSync->setIcon(QIcon(":/res/images/icon_tb_sel.png"));
        }
    }
}

void TitleBar::OnPortOpenChanged(BOOL bOpen)
{
    if(bOpen)
    {
        #ifndef Q_OS_OSX
        // 连接机器人时，更新固件菜单才能使能
        m_pActionRobotFirmwareVersion->setEnabled(true);
       #endif
    }
    else
    {
        #ifndef Q_OS_OSX
        m_pActionRobotFirmwareVersion->setEnabled(false);
        #endif
    }
}

void TitleBar::OnPbCloseClicked()
{
    emit SigClose();
}

void TitleBar::OnPbMinClicked()
{
    emit SigMin();
}

void TitleBar::OnPbMaxClicked()
{
    if(!m_bIsWindowMax)
    {
        emit SigMax();

        m_bIsWindowMax = true;
        ui->pbWin_Max->setStyleSheet("QPushButton#pbWin_Max{border-image: url(:/res/images/icon_win_restore_nor.png);}"
                                     "QPushButton#pbWin_Max:hover{border-image: url(:/res/images/icon_win_restore_sel.png);}"
                                     "QPushButton#pbWin_Max:pressed{border-image: url(:/res/images/icon_win_restore_press.png);}");
    }
    else
    {
        emit SigRestore();

        m_bIsWindowMax = false;
        ui->pbWin_Max->setStyleSheet("QPushButton#pbWin_Max{border-image: url(:/res/images/icon_win_max_nor.png);}"
                                     "QPushButton#pbWin_Max:hover{border-image: url(:/res/images/icon_win_max_sel.png);}"
                                     "QPushButton#pbWin_Max:pressed{border-image: url(:/res/images/icon_win_max_press.png);}");
    }
}

void TitleBar::on_pbConnectRobot_clicked()
{
    emit SigOpenPort();
}

void TitleBar::on_pBUserLogin_clicked()
{
    emit SigLogin();
}

void TitleBar::onTextChange(QString text)
{
    ui->pBUserLogin->setText(text);
}

//菜单"视图"响应
void TitleBar::on_menuview_clicked()
{

}

//菜单"帮助"响应
void TitleBar::on_menuhelp_clicked()
{

}

//菜单点击响应
void TitleBar::on_menu_clicked()
{
    if(m_pWinMenu)
    {
        QPoint point = QCursor::pos();
        m_pWinMenu->move(point);

        m_pWinMenu->show();
    }
}

void TitleBar::onMax()
{
    OnPbMaxClicked();
}

void TitleBar::onRestore()
{
    OnPbMaxClicked();
}

void TitleBar::onMenuAlertMark(bool bAertMark)
{
    if(bAertMark)
    {
//        ui->pbWin_Menu->setStyleSheet("QPushButton#pbWin_Menu{border-image: url(:/res/images/icon_win_menu_alert.png);}"
//                                          "QPushButton#pbWin_Menu:hover{border-image: url(:/res/images/icon_win_menu_alert_sel.png);}"
//                                          "QPushButton#pbWin_Menu:pressed{border-image: url(:/res/images/icon_win_menu_alert_press.png);}");

        ui->pbWin_Menu->setStyleSheet("QPushButton#pbWin_Menu{border-image: url(:/res/images/icon_win_menu_nor.png);}"
                                          "QPushButton#pbWin_Menu:hover{border-image: url(:/res/images/icon_win_menu_sel.png);}"
                                          "QPushButton#pbWin_Menu:pressed{border-image: url(:/res/images/icon_win_menu_press.png);}");
    }
    if(!bAertMark)
    {
        ui->pbWin_Menu->setStyleSheet("QPushButton#pbWin_Menu{border-image: url(:/res/images/icon_win_menu_nor.png);}"
                                          "QPushButton#pbWin_Menu:hover{border-image: url(:/res/images/icon_win_menu_sel.png);}"
                                          "QPushButton#pbWin_Menu:pressed{border-image: url(:/res/images/icon_win_menu_press.png);}");
    }
}

void TitleBar::slotOnSwitchTab(int nTabId)
{
    QAbstractButton *pButton = m_btnGroup.button(nTabId);
    if (!pButton)
    {
        return;
    }

    pButton->setChecked(true);
}

//菜单"关于"响应
void TitleBar::on_menuabout_clicked()
{
    emit sigAbout();
}
