#include "cdialogcontainer.h"
#include "ui_cdialogcontainer.h"

#include "ubxwidget.h"

#include <QMessageBox>


CDialogContainer::CDialogContainer(QWidget *parent) :
    CUBXDialog(parent),
    ui(new Ui::CDialogContainer)
{
    ui->setupUi(this);

    m_pContentWdg = NULL;
    m_clickBtnStatus = (int)QMessageBox::No;

    ui->verticalLayout->insertWidget(1, (QWidget*)&m_myTitleBar, Qt::AlignCenter);
    ui->verticalLayout->setContentsMargins(15,0,15,0);
    setContentsMargins(0,0,0,0);


    m_myTitleBar.setLayoutMargins(5, 15, 35, 15);
    m_myTitleBar.setMinimumHeight(45);

    m_myTitleBar.setObjectName("m_myTitleBar");//for not using the old title qss

    connect(&m_myTitleBar, &MyTitleBar::sigClose, this, &CDialogContainer::close);

    UBXWidget::setDefaultStyle(this, DEFAULT_ZXLIB_QSS_PATH);
    UBXWidget::applyUBXWinFlags(this);
    setEnableMove(true);
    setLayout(ui->verticalLayout);
}

CDialogContainer::~CDialogContainer()
{
    if(m_pContentWdg)
    {
        ui->verticalLayout->removeWidget(m_pContentWdg);
        m_pContentWdg = NULL;
    }

    delete ui;
}

void CDialogContainer::setContentWdg(QWidget *pWdg)
{
    m_pContentWdg = pWdg;
    if(m_pContentWdg)
    {
        int nWidth = m_pContentWdg->width() + 10;
        int nHeight = m_pContentWdg->height() + m_myTitleBar.height() + 30;
        this->setFixedSize(nWidth , nHeight);

        ui->verticalLayout->insertWidget(2, (QWidget*)m_pContentWdg, Qt::AlignCenter);
        QString strTitle = m_pContentWdg->windowTitle();
        m_myTitleBar.getTitleLabel()->setText(strTitle);

        ///////////////////////////////////////////////////////////////////////
        connect(m_pContentWdg, SIGNAL(sigClicked(int)), this, SLOT(onSigClicked(int)));
        connect(m_pContentWdg, SIGNAL(sigClose()), this, SLOT(close()));
        ///////////////////////////////////////////////////////////////
    }
}

int CDialogContainer::exec(QWidget *pWdg)
{
    if(!pWdg)
    {
        return -1;
    }

    setContentWdg(pWdg);
    return exec();
}

void CDialogContainer::onSigClicked(int nClicked)
{
    m_clickBtnStatus = nClicked;
}

int CDialogContainer::exec()
{
    QDialog::exec();
    return m_clickBtnStatus;
}
