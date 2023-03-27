#include <QTextStream>
#include <QSettings>

#include "dialogabout.h"
#include "ui_dialogabout.h"
#include "../common/UBXPublic.h"
#include "../common/Util.h"

#include "dialogsetting.h"
#include "ui_dialogsetting.h"
#include "../zxLib/widget/messagebox.h"


SettingDialog::SettingDialog(const QString &strIniFile, QWidget *parent) :
    CUBXDialog(parent), ui(new Ui::dialogsetting), m_strIniFile(strIniFile),
    m_pLanguageSettingWidget(NULL),
    m_pRobotTypeSettingWdg(NULL),
    m_plistWidget(NULL),
    m_pStackWidget(NULL)
{
    ui->setupUi(this);

    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    initUI();

    setDefaultStyle();

    m_dragPosition = QPoint(-1, -1);

    ui->myTitleBar->setMinimumHeight(45);
    ui->myTitleBar->setLayoutMargins(10, 0, 10, 10);    

    resize(350, 450);

    retranslateUi();

    //m_plistWidget->hide();
}

SettingDialog::~SettingDialog()
{    
    delete ui;

//    SAFE_DELETE(m_pLanguageSettingWidget);
}

/*************
* 函数名:mouseMoveEvent
* 功能:鼠标移动事件
* 参数:
* 返回值:
* 时间: 2016/06/07
* 作者: hwx
*****************/
void SettingDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() &Qt::LeftButton)
    {
        if (m_dragPosition != QPoint(-1, -1))
            move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

/*************
* 函数名:mousePressEvent
* 功能:鼠标按下事件
* 参数:
* 返回值:
* 时间: 2016/06/07
* 作者: hwx
*****************/
void SettingDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

/*************
* 函数名:mouseReleaseEvent
* 功能:鼠标离开事件
* 参数:
* 返回值:
* 时间: 2016/06/07
* 作者: hwx
*****************/
void SettingDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = QPoint(-1, -1);
        event->accept();
    }
}

/************************************
* 名称: slotCurItemChanged
* 功能: 当前项发生改变
* 参数：item，当前项
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void SettingDialog::slotItemClicked(QListWidgetItem *item)
{
    if (NULL == item)
    {
        return;
    }

    int nIndex = item->data(Qt::UserRole).toInt();
    m_pStackWidget->setCurrentIndex(nIndex);
}

/************************************
* 名称: initUI
* 功能: 初始化界面
* 参数：
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void SettingDialog::initUI()
{
    m_plistWidget = new QListWidget(this);

    m_plistWidget->setFrameShape(QFrame::NoFrame);
    m_plistWidget->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_plistWidget->setObjectName(QStringLiteral("m_plistWidget"));

    m_pStackWidget = new QStackedWidget(this);
    m_pStackWidget->setObjectName(QStringLiteral("m_pStackWidget"));

//    connect(ui->pbSettingClose, &QPushButton::clicked, this, &SettingDialog::close);
    connect(ui->myTitleBar, &MyTitleBar::sigClose, this, &SettingDialog::close);

    connect(ui->pbSettingConform, &QPushButton::clicked, this, &SettingDialog::slotOnClickedSave);
    connect(ui->pbSettingCancel, &QPushButton::clicked, this, &SettingDialog::slotOnClickedCancel);
    connect(m_plistWidget, &QListWidget::itemClicked, this, &SettingDialog::slotItemClicked);

    initStackedWidget();

    ui->horizontalLayoutCenter->addWidget(m_plistWidget);
    ui->horizontalLayoutCenter->addWidget(m_pStackWidget, 1);    
}

/************************************
* 名称:
* 功能: 显示提示信息，软件重新打开后生效
* 参数：
* 返回:   void
* 时间:   2016/06/12
* 作者:   hwx
************************************/
void SettingDialog::displayNotice()
{
    //CMessageBox::information(NULL, tr("Notice"), tr("The current preference settings will come into effect  in the next start-up."));
}

/************************************
* 名称: initStackedWidget
* 功能: 初始化stackwidget
* 参数：
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void SettingDialog::initStackedWidget()
{
    m_pLanguageSettingWidget = new LanguageSettingWidget(m_pStackWidget);
    int nIndex = m_pStackWidget->addWidget(m_pLanguageSettingWidget);
    QListWidgetItem *pListWidgetItem = new QListWidgetItem(tr("Language"));
    pListWidgetItem->setData(Qt::UserRole, nIndex);
    m_plistWidget->addItem(pListWidgetItem);

    //
    m_pRobotTypeSettingWdg = new robotTypeSettingWdg(m_pStackWidget);
    nIndex = m_pStackWidget->addWidget(m_pRobotTypeSettingWdg);
    pListWidgetItem = new QListWidgetItem(tr("Robot type"));
    pListWidgetItem->setData(Qt::UserRole, nIndex);
    m_plistWidget->addItem(pListWidgetItem);
}

/************************************
* 名称: writeAllSettingToFile
* 功能: 保存所有配置信息
* 参数：strIniFile，文件名称
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void SettingDialog::writeAllSettingToFile(const QString &strIniFile)
{
    m_pLanguageSettingWidget->saveToFile(strIniFile);

    m_pRobotTypeSettingWdg->saveToFile();
}

/************************************
* 名称: loadAllSettingFromFile
* 功能: 导入所有配置信息
* 参数：strIniFile，文件名称
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void SettingDialog::loadAllSettingFromFile(const QString &strIniFile)
{
    m_pLanguageSettingWidget->loadFromFile(strIniFile);
    m_pRobotTypeSettingWdg->initUI();
}

void SettingDialog::retranslateUi()
{
   ui->retranslateUi(this);

   const QString strSettingTitle = tr("Preferences");
   ui->myTitleBar->getTitleLabel()->setText(strSettingTitle);
}

/************************************
* 名称: setDefaultStyle
* 功能: 设置样式
* 参数：
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void SettingDialog::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/alphaSettingDialogDefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

/************************************
* 名称: slotOnClickedConform
* 功能: 点击确认按钮槽函数
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void SettingDialog::slotOnClickedSave()
{
    writeAllSettingToFile(m_strIniFile);
    //displayNotice();
    close();
}

/************************************
* 名称: slotOnClickedCancel
* 功能: 点击取消按钮槽函数
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void SettingDialog::slotOnClickedCancel()
{
    close();
}

/************************************
* 名称: slotOnClickedClosed
* 功能: 点击取消按钮槽函数
* 返回:   void
* 时间:   2016/06/07
* 作者:   hwx
************************************/
void SettingDialog::slotOnClickedClosed()
{
    close();
}
