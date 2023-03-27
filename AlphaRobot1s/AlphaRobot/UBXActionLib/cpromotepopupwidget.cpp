/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cpromotepopupwidget.cpp
* 创建时间：2016/03/10
* 文件标识：
* 文件摘要：推广活动自动弹出界面
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/08/31
* 版本摘要：
*/

#include <QFileInfo>
#include <QDir>
#include <QApplication>
#include <QStyleOption>
#include <QPainter>
#include <QTimer>

#include "cpromotepopupwidget.h"
#include "ui_cpromotepopupwidget.h"
#include "UBXPublic.h"
#include "ubxactionlibconstants.h"
#include "configs.h"

RETRANSLATEUIFUNCIMPL(CPromotePopUpWidget)
CPromotePopUpWidget::CPromotePopUpWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::CPromotePopUpWidget)
{
    ui->setupUi(this);

    setFixedSize(parent->width()/2, parent->height()/2);

    init();

    setDefaultStyle(this, ":/res/qss/ubxactionlib.qss");
}

CPromotePopUpWidget::~CPromotePopUpWidget()
{
    delete ui;
}

bool CPromotePopUpWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->labImage)
    {
        if(QEvent::MouseButtonPress == event->type()) // 鼠标点击事件
        {
            QMouseEvent *pMouseEvent = dynamic_cast<QMouseEvent*>(event);
            if (!pMouseEvent)
            {
                return false;
            }

            // 只响应鼠标左键
            if (Qt::LeftButton == pMouseEvent->button())
            {
                sigSwitchTab((int)eTitleBarTabActionLib);
                sigOnImageClicked();
                hide();
            }  // 鼠标左键
        } // MouseButtonPress
    }
    return QWidget::eventFilter(watched, event);
}

void CPromotePopUpWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

void CPromotePopUpWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

/************************************
* 名称: updateImage
* 功能: 更新图片文件的槽函数
* 参数: [in]strImageFIle 图片文件
* 返回:   void
* 时间:   2016/08/31
* 作者:   hwx
************************************/
void CPromotePopUpWidget::updateImage(const QString &strImageFIle)
{
    QString sImageFile = strImageFIle;
    if (sImageFile.isEmpty())
    {
        // 使用默认图片
        sImageFile = CConfigs::getLocalResPath(UBX_DEFAULT_IMAGE);//如果加载图片不成功则加载默认图片
    }

    if (!m_pixMapImage.load(sImageFile))
    {
        m_pixMapImage.load(CConfigs::getLocalResPath(QString("display%1%2").arg(QDir::separator()).arg(UBX_DISPLAY_IMAGE_1)));
    }

    ui->labImage->setPixmap(m_pixMapImage);
    showCloseButton();
}

void CPromotePopUpWidget::init()
{
#ifdef Q_OS_WIN
    m_pPushButtonCloseWin = new QPushButton(this);
    m_pPushButtonCloseWin->setObjectName("pbCloseWin");
    m_pPushButtonCloseWin->setFixedSize(QSize(Constants::CLOSE_BUTTON_WIDTH, Constants::CLOSE_BUTTON_HEIGHT));
    connect(m_pPushButtonCloseWin, &QPushButton::clicked, this, &CPromotePopUpWidget::close);
#else
    m_pPushButtonCloseMac = new QPushButton(this);
    m_pPushButtonCloseMac->setObjectName("pbClose");
    m_pPushButtonCloseMac->setFixedSize(QSize(Constants::CLOSE_BUTTON_WIDTH, Constants::CLOSE_BUTTON_HEIGHT));
    connect(m_pPushButtonCloseMac, &QPushButton::clicked, this, &CPromotePopUpWidget::close);
#endif

    setEnableMove(true);

    ui->labImage->setScaledContents(true);
    ui->labImage->setMouseTracking(true);
    ui->labImage->installEventFilter(this);
}

/************************************
* 名称: showCloseButton
* 功能: 显示关闭按钮
* 返回:   void
* 时间:   2016/09/06
* 作者:   hwx
************************************/
void CPromotePopUpWidget::showCloseButton()
{
#ifdef Q_OS_WIN
    m_pPushButtonCloseWin->move(this->rect().topRight() + QPoint(-(5 *  Constants::CLOSE_BUTTON_BORDER_OFFSET), Constants::CLOSE_BUTTON_BORDER_OFFSET));
    m_pPushButtonCloseWin->show();
#else
    m_pPushButtonCloseMac->move(this->rect().topLeft() + QPoint(Constants::CLOSE_BUTTON_BORDER_OFFSET, Constants::CLOSE_BUTTON_BORDER_OFFSET));
    m_pPushButtonCloseMac->show();
#endif
}
