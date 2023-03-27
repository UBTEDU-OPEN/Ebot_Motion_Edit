/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：downloadwidget.cpp
* 创建时间：2016/03/30
* 文件标识：
* 文件摘要：下载项组件，用于显示和管理下载项
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/03/30
* 版本摘要：
*/
#include "downloadwidget.h"
#include "ui_downloadwidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QFileInfo>
#include <QFile>
#include <QScrollBar>

#include "util.h"
#include "ubxactionlibconstants.h"
#include "UBXPublic.h"
#include "../common/filedirhandle.h"
#include "../zxLib/widget/messagebox.h"
#include "../zxLib/widget/ctablewidget.h"
#include "../zxLib/widget/ctablewidgetitem.h"


const int DEFAULT_INVALID_ACTIONID = -1;


const int TABLE_COL_CHECKBOX       = 0;
const int TABLE_COL_NAME           = 1;
const int TABLE_COL_SIZE           = 2;
const int TABLE_COL_DATE_DOWNLOAD  = 3;
const int TABLE_COL_STATUS         = 4;
const int TABLE_COL_SYNC           = 5;
const int TABLE_COL_OPERATIONS     = 6;


#define CLASSNAME_DOWNLOAD_WIDGET  "DownloadWidget"

DownloadWidget::DownloadWidget(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::DownloadWidget)
{
    ui->setupUi(this);    

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    //QStringList strCols;
    //第一列给checkbox
    //strCols << "" << tr("Name") << tr("Size") << tr("Date Downloaded") << tr("Status") << tr("Syncing") << tr("Operations");

    ui->tableWidgetDownload->setColumnCount(7);
    //ui->tableWidgetDownload->setHorizontalHeaderLabels(strCols); //设置表头
    ui->tableWidgetDownload->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetDownload->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidgetDownload->setFrameShape(QFrame::NoFrame);
    ui->tableWidgetDownload->setShowGrid(false);
    ui->tableWidgetDownload->verticalHeader()->hide();
    ui->tableWidgetDownload->horizontalHeader()->setFixedHeight(34);
    ui->tableWidgetDownload->setMouseTracking(true);
    ui->tableWidgetDownload->installEventFilter(this);
    ui->tableWidgetDownload->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//去除水平滚动条
    
    QScrollBar *pScrollBar = ui->tableWidgetDownload->verticalScrollBar();
    if (pScrollBar)
    {
        connect(pScrollBar, &QScrollBar::valueChanged, this, &DownloadWidget::slotOnTableVScrollValueChanged);
    }

    QHeaderView* pHeaderView =  ui->tableWidgetDownload->horizontalHeader();
    pHeaderView->setDefaultAlignment(Qt::AlignCenter);
    //单独设置文件列左对齐
    ui->tableWidgetDownload->model()->setHeaderData(TABLE_COL_NAME, Qt::Horizontal, Qt::AlignCenter, Qt::TextAlignmentRole);

    connect(ui->tableWidgetDownload, &QTableWidget::cellEntered, this, &DownloadWidget::OnCellEntered);
    connect(ui->tableWidgetDownload, &CTableWidget::sigRowCountChanged, this, &DownloadWidget::onRowCountChanged);
    ui->tableWidgetDownload->startRowChangeTimer(1000);

    m_pCheckBoxSelAll = new QCheckBox(pHeaderView);
    m_pCheckBoxSelAll->setObjectName(QStringLiteral("checkBox_selall"));
    connect(m_pCheckBoxSelAll, &QCheckBox::clicked, this, &DownloadWidget::OnSelAllCheckBoxClicked);
    pHeaderView->setIndexWidget(pHeaderView->model()->index(0,TABLE_COL_CHECKBOX), m_pCheckBoxSelAll);

#ifdef Q_OS_OSX
    ui->pbCloseWin->hide();
    connect(ui->pbClose, &QPushButton::clicked, this, &DownloadWidget::hide);
#else
    ui->pbClose->hide();
    connect(ui->pbCloseWin, &QPushButton::clicked, this, &DownloadWidget::hide);
#endif

    m_nRowPrevHover = -1;
    m_nCurActionID = -1;
    m_nCurRow = -1;
    m_nCurCol = -1;
    ui->pbAddToSyncList->setEnabled(false);

    m_pBtnPause = new QPushButton(ui->tableWidgetDownload);
    m_pBtnPause->hide();
    m_pBtnPause->setObjectName(QStringLiteral("pbDownloadItemPause"));
    connect(m_pBtnPause, &QPushButton::clicked, this, &DownloadWidget::OnPauseBtnClicked);

    m_pBtnDelete = new QPushButton(ui->tableWidgetDownload);
    m_pBtnDelete->setObjectName(QStringLiteral("pbDownloadItemDelete"));
    m_pBtnDelete->hide();
    connect(m_pBtnDelete, &QPushButton::clicked, this, &DownloadWidget::OnDeleteBtnClicked);

    m_pBtnOpen = new QPushButton(ui->tableWidgetDownload);
    m_pBtnOpen->setObjectName(QStringLiteral("pbDownloadItemOpen"));
    m_pBtnOpen->hide();
    connect(m_pBtnOpen, &QPushButton::clicked, this, &DownloadWidget::OnOpenBtnClicked);

    connect(ui->pbPause, &QPushButton::clicked, this, &DownloadWidget::OnPauseSelBtnClicked);
    connect(ui->pbAllStart, &QPushButton::clicked, this, &DownloadWidget::OnStartAllBtnClicked);
    connect(ui->pbDelete, &QPushButton::clicked, this, &DownloadWidget::OnDeleteSelBtnClicked);
    connect(ui->pbOpenFolder, &QPushButton::clicked, this, &DownloadWidget::OnOpenFolderBtnClicked);
    connect(ui->pbAddToSyncList, &QPushButton::clicked, this, &DownloadWidget::OnAddToSyncListclicked);
    connect(ui->checkBoxNotAdded, &QCheckBox::clicked, this, &DownloadWidget::OnCheckBoxNotAddedClicked);

    m_bLeftDown = false;
    connect(g_actupdatemgr, &CActUpdateMangr::SigDownloadProgress, this, &DownloadWidget::OnDownloadProgress);
    connect(g_actupdatemgr, &CActUpdateMangr::SigStartDownload, this, &DownloadWidget::OnStartDownload);
    connect(g_actupdatemgr, &CActUpdateMangr::SigHasPausedDownload, this, &DownloadWidget::OnHasPausedDownload);
    connect(ui->tableWidgetDownload, &QTableWidget::currentCellChanged, this, &DownloadWidget::onCurrentCellChanged);
    connect(g_actupdatemgr, &CActUpdateMangr::sigOnDownloadFinish, this, &DownloadWidget::slotOnDownloadFinish);
    setDefaultStyle();

    InitItems();

    retranslateUi();
}

DownloadWidget::~DownloadWidget()
{
    delete ui;
}

void DownloadWidget::retranslateUi()
{
    ui->retranslateUi(this);
    QStringList strCols;
    //第一列给checkbox
    strCols << "" << tr("Name") << tr("Size") << tr("Date Downloaded") << tr("Status") << tr("Syncing") << tr("Operations");
    ui->tableWidgetDownload->setHorizontalHeaderLabels(strCols); //设置表头
    ui->tableWidgetDownload->retranslateUi();
}

/************************************
* 名称: setDefaultStyle
* 功能: 设置窗体默认样式
* 返回:   void
* 时间:   2016/03/31
* 作者:   hels
************************************/
void DownloadWidget::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/ubxactionlib.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}

/************************************
* 名称: resizeEvent
* 功能: 窗体大小改变事件，用于重新计算组件大小
* 参数: [in]parameter-name parameter description
* 返回:   void
* 时间:   2016/03/30
* 作者:   hels
************************************/
void DownloadWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    //根据窗体大小，重新设置表格标题每栏占据的宽度值
    if(NULL != ui->tableWidgetDownload)
    {
        int width = this->size().width() - 56 - 20;//这里除了减去连个边缘宽度外，还需要减去垂直滚动条的宽度
        ui->tableWidgetDownload->setColumnWidth(TABLE_COL_CHECKBOX, width * 0.04); //checkbox 4%
        ui->tableWidgetDownload->setColumnWidth(TABLE_COL_NAME, width * 0.28); //文件名
        ui->tableWidgetDownload->setColumnWidth(TABLE_COL_SIZE, width* 0.10); //文件大小
        ui->tableWidgetDownload->setColumnWidth(TABLE_COL_DATE_DOWNLOAD, width* 0.15); //下载日期
        ui->tableWidgetDownload->setColumnWidth(TABLE_COL_STATUS, width * 0.16);//状态
        ui->tableWidgetDownload->setColumnWidth(TABLE_COL_SYNC, width* 0.15); //同步
        ui->tableWidgetDownload->setColumnWidth(TABLE_COL_OPERATIONS, width* 0.12); //操作
    }
}

void DownloadWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_ptPre = event->globalPos();
        m_bLeftDown = true;
    }
    else
    {
        m_bLeftDown = false;
    }
}

void DownloadWidget::mouseMoveEvent(QMouseEvent *event)
{
    QRect rectTitleBar(0, 0, this->width(), 38);
    if(m_bLeftDown && rectTitleBar.contains(mapFromGlobal(m_ptPre)))
    {
        QPoint nowPt = event->globalPos();
        QPoint moveTo = mapToParent(nowPt - m_ptPre);
        move(moveTo);
        m_ptPre = nowPt;
    }
}

void DownloadWidget::mouseReleaseEvent(QMouseEvent */*event*/)
{
    m_bLeftDown = false;
}

void DownloadWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/************************************
* 名称: eventFilter
* 功能: 重写父类函数，当鼠标离开tablewidget时，将hover状态清空
* 返回:   void
* 时间:   2016/04/06
* 作者:   hels
************************************/
bool DownloadWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == ui->tableWidgetDownload)
    {
        if(ev->type() == QEvent::Leave ||
                ev->type() == QEvent::HoverLeave)
        {
            if(m_nRowPrevHover != -1)
            {
                for(int col = 0; col < ui->tableWidgetDownload->columnCount(); col++)
                {
                    QTableWidgetItem* pItem = ui->tableWidgetDownload->item(m_nRowPrevHover, col);
                    if(pItem)
                        pItem->setBackground(Constants::COLOR_ITEM_HOVER_CLEAR);
                }
                m_nRowPrevHover = -1;
                m_nCurRow = -1;
                m_nCurCol = -1;
                m_nCurActionID = -1;
            }
            if(m_pBtnPause)
            {
                m_pBtnPause->hide();
            }
            if(m_pBtnDelete)
            {
                m_pBtnDelete->hide();
            }
            if(m_pBtnOpen)
            {
                m_pBtnOpen->hide();
            }
        }
    }
    return QWidget::eventFilter(obj, ev);
}

/************************************
* 名称: OnStartDownload
* 功能: 通知外部开始下载
* 参数: [in]requestID 对应的请求ID
* 参数: [in]itemData 下载项
* 参数: [in]bytesReceived 已经接收到的字节数
* 参数: [in]bytesTotal 总共的字节数
* 返回:   void
* 时间:   2016/04/05
* 作者:   hels
************************************/
void DownloadWidget::OnStartDownload(int requestID, ActItemData itemData, qint64 bytesReceived, qint64 bytesTotal)
{
    AddDownloadItem(requestID, itemData, bytesReceived, bytesTotal);
    addItemStatus(itemData.m_nItemID, DownloadingState);
}

/************************************
* 名称: OnDownloadProgress
* 功能: 响应下载进度
* 参数: [in]requestID 对应的请求ID
* 参数: [in]nActionID 下载项的ID
* 参数: [in]bytesReceived 已经接收到的字节数
* 参数: [in]bytesTotal 总共的字节数
* 返回:   void
* 时间:   2016/04/05
* 作者:   hels
************************************/
void DownloadWidget::OnDownloadProgress(int /*requestID*/, int nActionID, qint64 bytesReceived, qint64 bytesTotal)
{
    if(m_mapProgressBar.contains(nActionID))
    {
        if(m_mapItemData.contains(nActionID))
        {
            m_mapItemData[nActionID].m_nHasDownloadedBytes = bytesReceived;
            int value = (double)m_mapItemData[nActionID].m_nHasDownloadedBytes / (double)m_mapItemData[nActionID].m_nActSize * 100.0;
            if((value >= 0) && (value < 100))
            {
                QProgressBar *pProgressBar = m_mapProgressBar[nActionID];
                if(pProgressBar)
                {
                    pProgressBar->setValue(value); 
                }

                removeItemStatus(nActionID, FinishedState);
                removeItemStatus(nActionID, PausedState);
                addItemStatus(nActionID, DownloadingState);
            }
            
            if(g_actupdatemgr)
            {
                g_actupdatemgr->UpdateDownloadingActInfo(m_mapItemData[nActionID]);
            }
        }

        if(bytesReceived == bytesTotal)
        {
            slotOnDownloadFinish(nActionID);
//            removeItemStatus(nActionID, DownloadingState);
//            removeItemStatus(nActionID, PausedState);
//            addItemStatus(nActionID, FinishedState);

//            if(m_mapProgressBar.contains(nActionID))
//            {
//                //Qt 帮助文档对于QTableWidget的说明
//                //Sets the given widget to be displayed in the cell in the given row and column, passing the ownership of the widget to the table.
//                //If cell widget A is replaced with cell widget B, cell widget A will be deleted.
//                //因此不能直接删除CellWidget，否则程序会崩溃
//                QProgressBar *pProgressBar = m_mapProgressBar[nActionID];
//                m_mapProgressBar.remove(nActionID);
////                int nRow = pProgressBar->property(Constants::PROPERTY_ITEM_ROW).toInt();
////                int nCol = pProgressBar->property(Constants::PROPERTY_ITEM_COL).toInt();
//                int nRow = getRowByActionID(nActionID);

//                QWidget* pWidget = ui->tableWidgetDownload->cellWidget(nRow, TABLE_COL_CHECKBOX);
//                if(pWidget)
//                {
//                    ui->tableWidgetDownload->removeCellWidget(nRow, TABLE_COL_STATUS);
//                }
//                pProgressBar->deleteLater();
                
//                QTableWidgetItem* pItem = ui->tableWidgetDownload->item(nRow, TABLE_COL_STATUS);
//                if(pItem)
//                {
//                    pItem->setText(tr("Downloaded"));
//                }
//            }
        }

        qApp->processEvents();
    }
}

/************************************
* 名称: OnItemCheckBoxClicked
* 功能: 响应下载项checkbox槽函数
* 参数: [in]checked 按钮check状态
* 返回:   void
* 时间:   2016/04/05
* 作者:   hels
************************************/
void DownloadWidget::OnItemCheckBoxClicked(bool /*checked*/)
{
    Qt::CheckState eCheckState = CTableWidget::getAllCheckBoxCheckedState(ui->tableWidgetDownload, TABLE_COL_CHECKBOX);
    bool bAllSelect = true;
    if (eCheckState != Qt::Checked)
    {
        bAllSelect = false;
    }
    m_pCheckBoxSelAll->setChecked(bAllSelect);
}

/************************************
* 名称: OnSelAllCheckBoxClicked
* 功能: 选中所有按钮槽函数
* 参数: [in]checked 按钮check状态
* 返回:   void
* 时间:   2016/04/08
* 作者:   hels
************************************/
void DownloadWidget::OnSelAllCheckBoxClicked(bool checked)
{
    Qt::CheckState state = Qt::Unchecked;
    if(checked)
    {
        state = Qt::Checked;
    }
    CTableWidget::setAllCheckBoxs(ui->tableWidgetDownload, TABLE_COL_CHECKBOX, state);
}

/************************************
* 名称: OnPauseBtnClicked
* 功能: 暂停按钮槽函数
* 返回:   void
* 时间:   2016/04/06
* 作者:   hels
************************************/
void DownloadWidget::OnPauseBtnClicked()
{
    if(g_actupdatemgr)
    {
        if(isItemStatus(m_nCurActionID, DownloadingState))
        {
            g_actupdatemgr->PauseDownload(m_nCurActionID);
        }
        else if(isItemStatus(m_nCurActionID, PausedState))
        {
            g_actupdatemgr->ResumeDownload(m_mapItemData[m_nCurActionID]);
            removeItemStatus(m_nCurActionID, PausedState);
            addItemStatus(m_nCurActionID, DownloadingState);
            m_pBtnPause->setStyleSheet("QPushButton{background: transparent;"
                                       "border-image: url(:/res/images/icon_download_stop.png);"
                                       "width:16px;"
                                       "height:16px;}"
                                       "QPushButton:pressed{background: transparent;"
                                       "border-image: url(:/res/images/icon_download_stop_sel.png);"
                                       "width:16px;"
                                       "height:16px;}");
        }
    }
}

/************************************
* 名称: OnDeleteBtnClicked
* 功能: 删除按钮槽函数
* 返回:   void
* 时间:   2016/04/07
* 作者:   hels
************************************/
void DownloadWidget::OnDeleteBtnClicked()
{
    ui->tableWidgetDownload->setEnabled(false);

    const int nCurRow = /*ui->tableWidgetDownload->currentRow();*/m_nCurRow;
    const int nCurCol = /*ui->tableWidgetDownload->currentColumn();*/m_nCurCol;
    qDebug()<<"nCurRow:"<<nCurRow<<endl;
    int nCurActionID = -1;

//    const int nRowCountPre = ui->tableWidgetDownload->rowCount();

    if(0 <= nCurRow )
    {
        QTableWidgetItem* pItem = ui->tableWidgetDownload->item(nCurRow, TABLE_COL_CHECKBOX);
        if(pItem)
        {
            nCurActionID = pItem->data(Constants::ActionItemID).toInt();
        }
    }

    if((0 <= nCurRow) && m_mapItemData.contains(nCurActionID) && g_actupdatemgr)
    {
        //        qDebug()<<"nCurRow:"<<nCurRow<<"g_actupdatemgr:"<<g_actupdatemgr<<endl;
        //        if(AddedState & m_mapItemStatus[nCurActionID])
        //        {
        //            CMessageBox::warning(NULL, "", tr("this Action has been Added,can't remove at this time!"));
        //            return;
        //        }//已添加的可以删除

        if(isItemStatus(nCurActionID, DownloadingState))
        {
            ui->tableWidgetDownload->setEnabled(true);
            return ;
            //            g_actupdatemgr->PauseDownload(nCurActionID); //正在下载的不能删除
        }
        else
        {
            ui->tableWidgetDownload->setEnabled(false);

            ui->tableWidgetDownload->removeRow(nCurRow);
//            if(PausedState & m_mapItemStatus[nCurActionID])
//            {
//                g_actupdatemgr->RemoveDownloadingActInfo(m_mapItemData[nCurActionID].m_sGuid);
//            }
//            else
//            {
//                g_actupdatemgr->RemoveDownloadedActInfo(m_mapItemData[nCurActionID].m_sGuid);
//            }
            g_actupdatemgr->RemoveDownloadingActInfo(m_mapItemData[nCurActionID].m_sGuid);
            g_actupdatemgr->RemoveDownloadedActInfo(m_mapItemData[nCurActionID].m_sGuid);

            QFileInfo fileInfo(m_mapItemData[nCurActionID].m_sLocalPath);
            CFileDirHandle::clearDir(fileInfo.absolutePath(), true);

            m_mapItemData.remove(nCurActionID);


            if(ui->tableWidgetDownload->rowCount() > 0)
            {
                if(nCurRow != ui->tableWidgetDownload->rowCount())
                {
                    OnCellEntered(nCurRow, nCurCol);
                }
                else
                {
                    OnCellEntered(nCurRow - 1, nCurCol);
                }
            }
        }
    }

//    //重新更新进度条特性
//    int nRowCountAft = ui->tableWidgetDownload->rowCount();
//    if((nRowCountAft > 0) && (nRowCountAft != nRowCountPre))
//    {
//        for(int i = 0; i< ui->tableWidgetDownload->rowCount(); i++)
//        {
//            QTableWidgetItem* pItem = ui->tableWidgetDownload->item(i, 0);
//            if(pItem)
//            {
//                const int nActionID = pItem->data(Constants::ActionItemID).toInt();
//                if(m_mapProgressBar.contains(nActionID))
//                {
//                    QProgressBar *pProgressBar = m_mapProgressBar[nActionID];
//                    if(pProgressBar)
//                    {
//                        pProgressBar->setProperty(Constants::PROPERTY_ITEM_ROW, i);
//                    }
//                }
//            }
//        }
//    }

    ui->tableWidgetDownload->setEnabled(true);
}

/************************************
* 名称: OnOpenBtnClicked
* 功能: 打开文件按钮槽函数
* 返回:   void
* 时间:   2016/04/07
* 作者:   hels
************************************/
void DownloadWidget::OnOpenBtnClicked()
{
    if(m_mapItemData.contains(m_nCurActionID))
    {
        QFileInfo fi=QFileInfo(m_mapItemData[m_nCurActionID].m_sLocalPath);
        QString filePath;
        filePath=fi.absolutePath();
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

/************************************
* 名称: OnCellEntered
* 功能: 响应鼠标进入item项，实现hover效果
* 参数: [in]int row 当前Item行
* 参数: [in]int column 当前Item列
* 返回:   void
* 时间:   2016/04/06
* 作者:   hels
************************************/
void DownloadWidget::OnCellEntered(int row, int column)
{
    QTableWidgetItem* pItem = NULL;

    if(m_nRowPrevHover != row)
    {
        for(int col = 0; col < ui->tableWidgetDownload->columnCount(); col++)
        {
            //清除上一次hover状态
            QTableWidgetItem* pItem = ui->tableWidgetDownload->item(m_nRowPrevHover, col);
            if(pItem)
                pItem->setBackground(Constants::COLOR_ITEM_HOVER_CLEAR);

            //hover本次状态
            pItem = ui->tableWidgetDownload->item(row, col);
            if(pItem)
                pItem->setBackground(Constants::COLOR_ITEM_HOVER);
        }
    }

    m_nRowPrevHover = row;
    m_nCurRow = row;
    m_nCurCol = column;

    //第一列保存ID
    pItem = ui->tableWidgetDownload->item(row, TABLE_COL_CHECKBOX);
    if(pItem)
    {
        m_nCurActionID = pItem->data(Constants::ActionItemID).value<int>();
    }

    //最后一列显示按钮
    pItem = ui->tableWidgetDownload->item(row, TABLE_COL_OPERATIONS);
    if(NULL == m_pBtnPause || NULL == m_pBtnDelete || NULL == m_pBtnOpen || NULL == pItem)
        return;

    int headerViewHeight = ui->tableWidgetDownload->horizontalHeader()->height();
    QRect itemRect = ui->tableWidgetDownload->visualItemRect(pItem);
    if(isItemStatus(m_nCurActionID, DownloadingState))
    {
        m_pBtnPause->setStyleSheet("QPushButton{background: transparent;"
                                   "border-image: url(:/res/images/icon_download_stop.png);"
                                   "width:16px;"
                                   "height:16px;}"
                                   "QPushButton:pressed{background: transparent;"
                                   "border-image: url(:/res/images/icon_download_stop_sel.png);"
                                   "width:16px;"
                                   "height:16px;}");
    }
    else if(isItemStatus(m_nCurActionID, PausedState))
    {
        m_pBtnPause->setStyleSheet("QPushButton{background: transparent;"
                                   "border-image: url(:/res/images/icon_download_resume.png);"
                                   "width:16px;"
                                   "height:16px;}"
                                   "QPushButton:pressed{background: transparent;"
                                   "border-image: url(:/res/images/icon_download_resume_sel.png);"
                                   "width:16px;"
                                   "height:16px;}");
    }

    if(isItemStatus(m_nCurActionID, DownloadingState) ||
       isItemStatus(m_nCurActionID, PausedState))
    {
        QPoint pausePt;
        pausePt.setX(itemRect.left() + Constants::CONTENT_SPACE);
        pausePt.setY(itemRect.top() + headerViewHeight + (itemRect.height() - m_pBtnPause->height()) / 2);
        m_pBtnPause->move(pausePt);
        m_pBtnPause->show();
    }
    else
    {
        m_pBtnPause->hide();
    }

    //
    m_pBtnDelete->setStyleSheet("QPushButton{background: transparent;"
                                "border-image: url(:/res/images/icon_download_delete.png);"
                                "width:16px;"
                                "height:16px;}"
                                "QPushButton:pressed{background: transparent;"
                                "border-image: url(:/res/images/icon_download_delete_sel.png);"
                                "width:16px;"
                                "height:16px;}");
    QPoint deletePt;
    deletePt.setX(itemRect.left() + 16 + 2 * Constants::CONTENT_SPACE);
    deletePt.setY(itemRect.top()  + headerViewHeight + (itemRect.height() - m_pBtnDelete->height()) / 2);
    m_pBtnDelete->move(deletePt);
    if(isItemStatus(m_nCurActionID, DownloadingState)
            /*|| AddedState & m_mapItemStatus[m_nCurActionID]*/) //已添加可以删除
    {
        m_pBtnDelete->hide();
    }
    else
    {
        m_pBtnDelete->show();
    }

    m_pBtnOpen->setStyleSheet("QPushButton{background: transparent;"
                              "border-image: url(:/res/images/icon_openfile.png);"
                              "width:16px;"
                              "height:16px;}"
                              "QPushButton:pressed{background: transparent;"
                              "border-image: url(:/res/images/icon_openfile_sel.png);"
                              "width:16px;"
                              "height:16px;}");
    QPoint openPt;
    openPt.setX(itemRect.left() + 16 + 16 + 3 * Constants::CONTENT_SPACE);
    openPt.setY(itemRect.top()  + headerViewHeight + (itemRect.height() - m_pBtnOpen->height()) / 2);
    m_pBtnOpen->move(openPt);
    m_pBtnOpen->show();
}

/************************************
* 名称: OnHasPausedDownload
* 功能: 下载停止后由下载线程通知
* 参数: [in]nActionID 暂停的action ID
* 返回:   void
* 时间:   2016/04/06
* 作者:   hels
************************************/
void DownloadWidget::OnHasPausedDownload(int nActionID)
{
    removeItemStatus(nActionID, DownloadingState);
    addItemStatus(nActionID, PausedState);

    m_pBtnPause->setStyleSheet("QPushButton{background: transparent;"
                               "border-image: url(:/res/images/icon_download_resume.png);"
                               "width:16px;"
                               "height:16px;}"
                               "QPushButton:pressed{background: transparent;"
                               "border-image: url(:/res/images/icon_download_resume_sel.png);"
                               "width:16px;"
                               "height:16px;}");
}

/************************************
* 名称: OnPauseSelBtnClicked
* 功能: 批量暂停按钮槽函数
* 返回:   void
* 时间:   2016/04/08
* 作者:   hels
************************************/
void DownloadWidget::OnPauseSelBtnClicked()
{
    ui->pbPause->setEnabled(false);
    for(int i = 0; i < ui->tableWidgetDownload->rowCount(); i++)
    {
        QCheckBox* pCheckBox = (QCheckBox*)ui->tableWidgetDownload->cellWidget(i, TABLE_COL_CHECKBOX);
        if(pCheckBox && pCheckBox->isChecked() && g_actupdatemgr)
        {
            QTableWidgetItem *pItem = ui->tableWidgetDownload->item(i, TABLE_COL_CHECKBOX);
            if(pItem)
            {
                int nActionID = pItem->data(Constants::ActionItemID).value<int>();
                if(isItemStatus(nActionID, DownloadingState))
                {
                    g_actupdatemgr->PauseDownload(nActionID);
                }
            }
        }
    }
    ui->pbPause->setEnabled(true);
}

/************************************
* 名称: OnStartAllBtnClicked
* 功能: 全部开始按钮槽函数
* 返回:   void
* 时间:   2016/04/08
* 作者:   hels
************************************/
void DownloadWidget::OnStartAllBtnClicked()
{
    ui->pbAllStart->setEnabled(false);
    for(int i = 0; i < ui->tableWidgetDownload->rowCount(); i++)
    {
        QCheckBox* pCheckBox = (QCheckBox*)ui->tableWidgetDownload->cellWidget(i, TABLE_COL_CHECKBOX);
        if(pCheckBox && pCheckBox->isChecked() && g_actupdatemgr)
        {
            QTableWidgetItem *pItem = ui->tableWidgetDownload->item(i, TABLE_COL_CHECKBOX);
            if(pItem)
            {
                int nActionID = pItem->data(Constants::ActionItemID).value<int>();
                if(isItemStatus(nActionID, PausedState))
                {
                    g_actupdatemgr->ResumeDownload(m_mapItemData[nActionID]);
                    removeItemStatus(nActionID, PausedState);
                    addItemStatus(nActionID, DownloadingState);
                }
            }
        }
    }

    ui->pbAllStart->setEnabled(true);
}

/************************************
* 名称: OnDeleteSelBtnClicked
* 功能: 批量删除按钮槽函数
* 返回:   void
* 时间:   2016/04/08
* 作者:   hels
************************************/
void DownloadWidget::OnDeleteSelBtnClicked()
{
    //删除需要从最后一个项开始
    ui->pbDelete->setEnabled(false);
    ui->tableWidgetDownload->setEnabled(false);
    const int nRowCount = ui->tableWidgetDownload->rowCount();
    qDebug()<<"DeleteStatusPre "<<"RowCount:"<<nRowCount<<endl;

    for(int i = ui->tableWidgetDownload->rowCount() - 1; i >= 0; i--)
    {
        QCheckBox* pCheckBox = (QCheckBox*)ui->tableWidgetDownload->cellWidget(i, TABLE_COL_CHECKBOX);
        if(pCheckBox && pCheckBox->isChecked() && g_actupdatemgr)
        {
            QTableWidgetItem *pItem = ui->tableWidgetDownload->item(i, TABLE_COL_CHECKBOX);
            if(pItem)
            {
                const int nActionID = pItem->data(Constants::ActionItemID).value<int>();
                if(isItemStatus(nActionID, DownloadingState)
                        /*|| AddedState & m_mapItemStatus[nActionID]*/)//不明白为什么这里不能删除
                {
                    //                                    g_actupdatemgr->PauseDownload(nActionID);//正在下载的动作不能删除
                    continue;
                }

                QFileInfo fileInfo(m_mapItemData[nActionID].m_sLocalPath);
//                if(PausedState & m_mapItemStatus[nActionID])
//                {
//                    g_actupdatemgr->RemoveDownloadingActInfo(m_mapItemData[nActionID].m_sGuid);
//                }
//                else
//                {
//                    g_actupdatemgr->RemoveDownloadedActInfo(m_mapItemData[nActionID].m_sGuid);
//                }
                g_actupdatemgr->RemoveDownloadingActInfo(m_mapItemData[nActionID].m_sGuid);
                g_actupdatemgr->RemoveDownloadedActInfo(m_mapItemData[nActionID].m_sGuid);

                CFileDirHandle::clearDir(fileInfo.absolutePath(), true);
                ui->tableWidgetDownload->removeRow(i);
                m_mapItemData.remove(nActionID);                
            }
        }
    }

//    int nRowCountAft = ui->tableWidgetDownload->rowCount();
//    if((nRowCountAft > 0) &&(nRowCountAft != nRowCount))
//    {
//        for(int i = 0; i< ui->tableWidgetDownload->rowCount(); i++)
//        {
//            QTableWidgetItem* pItem = ui->tableWidgetDownload->item(i, 0);
//            int nActionID = pItem->data(Constants::ActionItemID).toInt();
//            if(m_mapProgressBar.contains(nActionID))
//            {
//                QProgressBar *pProgressBar = m_mapProgressBar[nActionID];
//                if(pProgressBar)
//                {
//                    pProgressBar->setProperty(Constants::PROPERTY_ITEM_ROW, i);
//                }
//            }
//        }
//    }    
//    qDebug()<<"DeleteStatusAft "<<"RowCount:"<<nRowCountAft<<endl;

    ui->tableWidgetDownload->setEnabled(true);
    ui->pbDelete->setEnabled(true);
}

/************************************
* 名称: OnOpenFolderBtnClicked
* 功能: 打开目录按钮槽函数
* 返回:   void
* 时间:   2016/04/08
* 作者:   hels
************************************/
void DownloadWidget::OnOpenFolderBtnClicked()
{
    QFileInfo fi=QFileInfo(g_sActPath);
    QString filePath;
    filePath=fi.absoluteFilePath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

/************************************
* 名称: InitItems
* 功能: 初始化下载项
* 返回:   void
* 时间:   2016/04/07
* 作者:   hels
************************************/
void DownloadWidget::InitItems()
{
    if(g_actupdatemgr)
    {
        QMap<int, ActItemData> mapItem;
        g_actupdatemgr->GetDownloadedActList(mapItem);
        foreach (ActItemData item, mapItem)
        {
            if(QFile::exists(item.m_sLocalPath))
            {
                //if had loaddown , so it must have exist the file.
                AddDownloadItem(REQUSET_ID_DEFAULT_INVALID, item, item.m_nHasDownloadedBytes, item.m_nActSize);
            }
        }

        vector<ActItemData> vecActList;
        g_actupdatemgr->GetDownloadingActList(vecActList);
        vector<ActItemData>::iterator iter = vecActList.begin();
         for(; iter != vecActList.end(); iter++)
        {
             AddDownloadItem(REQUSET_ID_DEFAULT_INVALID, *iter, iter->m_nHasDownloadedBytes, iter->m_nActSize);
        }
    }
}

/************************************
* 名称: AddDownloadItem
* 功能: 添加下载项
* 参数: [in]requestID 对应的请求ID
* 参数: [in]itemData 下载项
* 参数: [in]bytesReceived 已经接收到的字节数
* 参数: [in]bytesTotal 总共的字节数
* 返回:   void
* 时间:   2016/04/05
* 作者:   hels
************************************/
void DownloadWidget::AddDownloadItem(int requestID, ActItemData itemData, qint64 /*bytesReceived*/, qint64 /*bytesTotal*/)
{
    for(int i = 0; i < ui->tableWidgetDownload->rowCount(); i++)
    {
        const int nActionID = getActionIDByRow(i);
        if(nActionID == itemData.m_nItemID)
        {
            if(0 != itemData.m_nActSize)
            {
                //when it is add first time(add to the UI), the itemdata is lost many info
                m_mapItemData.insert(itemData.m_nItemID, itemData);
            }
        }
    }

#if 1
    QFileInfo fileinfo(itemData.m_sLocalPath);
#else
    //显示hts名称到列表，非zip名称
    QString sHtsDir = itemData.m_sLocalPath.left(itemData.m_sLocalPath.lastIndexOf(QDir::separator()));
    QString sHtsShortName = itemData.m_sActName + HTS_FILE_EXT;
    QString sHtsFilePath;

    QDir dir(sHtsDir);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    QFileInfoList filelist = dir.entryInfoList();

    foreach(QFileInfo fileinfo, filelist)
    {
        QString sExt = '.' + fileinfo.suffix();
        if(sExt.compare(HTS_FILE_EXT, Qt::CaseInsensitive) == 0)
        {
            sHtsFilePath = fileinfo.filePath();
            sHtsShortName = fileinfo.fileName();
            break;
        }
    }

    if(sHtsFilePath.isEmpty())
    {
        CMessageBox::warning(this, "", sHtsShortName + tr("does not exist"));
        return;
    }

    QFileInfo fileinfo(sHtsFilePath);
#endif

    int nRowInsert = ui->tableWidgetDownload->rowCount();
    int nRowOfItemData = getRowByActionID(itemData.m_nItemID);
    bool bCheckBoxIsChecked = false;
    if(0 <= nRowOfItemData)
    {
        QCheckBox* pCheckBox = (QCheckBox*)ui->tableWidgetDownload->cellWidget(nRowOfItemData, TABLE_COL_CHECKBOX);
        if(pCheckBox)
        {
            bCheckBoxIsChecked = pCheckBox->isChecked();
        }

        nRowInsert = nRowOfItemData;
        ui->tableWidgetDownload->removeRow(nRowOfItemData);
    }

    ui->tableWidgetDownload->insertRow(nRowInsert);
    int nCol = 0;

    QFont font = QApplication::font();
    font.setPixelSize(FONT_PIXEL_SIZE_14);
    //复选框
    QTableWidgetItem *itemSpace = new QTableWidgetItem();//插入空对象，占位符,方便后面其他算获取pItem进行操作
    itemSpace->setData(Constants::ActionItemID, itemData.m_nItemID);//第一列保存ID
    if(!m_mapItemData.contains(itemData.m_nItemID))
    {
        m_mapItemData.insert(itemData.m_nItemID, itemData);
    }

    ui->tableWidgetDownload->setItem(nRowInsert, nCol, itemSpace);
    QCheckBox* pCheckBox = new QCheckBox(ui->tableWidgetDownload);
    pCheckBox->setChecked(bCheckBoxIsChecked);
    ui->tableWidgetDownload->setCellWidget(nRowInsert, nCol, pCheckBox);
    connect(pCheckBox, &QCheckBox::clicked, this, &DownloadWidget::OnItemCheckBoxClicked);

    nCol++;
    //文件名称

    QTableWidgetItem *pItem = new QTableWidgetItem(itemData.m_sActName);
    pItem->setFont(font);
    pItem->setForeground(QColor("#d3d3d3"));
    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidgetDownload->setItem(nRowInsert, nCol, pItem);

    nCol++;
    //文件大小
    QString strTemp = GetFormatSizeString(itemData.m_nActSize);
    if(0 >= itemData.m_nActSize)
    {
        strTemp = tr("Unknown");
    }
    pItem = new QTableWidgetItem(strTemp);
    pItem->setFont(font);
    pItem->setForeground(QColor("#d3d3d3"));
    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidgetDownload->setItem(nRowInsert, nCol, pItem);

    nCol++;
    //修改日期
    QDateTime timetemp = fileinfo.created();
    if(!QFile::exists(itemData.m_sLocalPath))
    {
        timetemp = QDateTime::currentDateTime();
    }

    if(!timetemp.isValid())
    {
        timetemp = QDateTime::currentDateTime();
    }

    strTemp = timetemp.toString("yyyy-MM-dd hh:mm");
    pItem = new QTableWidgetItem(strTemp);
    pItem->setFont(font);
    pItem->setForeground(QColor("#d3d3d3"));
    pItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidgetDownload->setItem(nRowInsert, nCol, pItem);

    nCol++;

    //状态
    CTableWidgetItem* pItemDownStatus = new CTableWidgetItem("");//插入空对象，占位符,方便后面其他算获取pItem进行操作
    pItemDownStatus->setFont(font);
    pItemDownStatus->setForeground(QColor("#17a9f4"));
    pItemDownStatus->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidgetDownload->setItem(nRowInsert, nCol, pItemDownStatus);
    ui->tableWidgetDownload->addRetranslateColumn(nCol);
    pItemDownStatus->setRetranslateContext(CLASSNAME_DOWNLOAD_WIDGET);
    if((itemData.m_nHasDownloadedBytes == itemData.m_nActSize) &&
            (0 != itemData.m_nActSize))
    {
        //pItemDownStatus->setText(tr("Downloaded"));
        pItemDownStatus->setTextRetranslate("Downloaded", tr("Downloaded"));
        addItemStatus(itemData.m_nItemID, FinishedState);
    }
    else
    {        
        QProgressBar* pProgressbar = new QProgressBar(ui->tableWidgetDownload);
//        pProgressbar->setProperty(Constants::PROPERTY_ITEM_ROW, nRow);
//        pProgressbar->setProperty(Constants::PROPERTY_ITEM_COL, nCol);
        pProgressbar->setObjectName(QString::number(itemData.m_nItemID));
        pProgressbar->setTextVisible(true);
        double dActionSize = (double)itemData.m_nActSize;
        if(0 == dActionSize)
        {
            dActionSize = 0.0001;
        }
        pProgressbar->setValue((double)itemData.m_nHasDownloadedBytes / dActionSize * 100.0);
        ui->tableWidgetDownload->setCellWidget(nRowInsert, nCol, pProgressbar);
        m_mapProgressBar.insert(itemData.m_nItemID, pProgressbar);

        if(requestID >= 0)
        {
            removeItemStatus(itemData.m_nItemID, PausedState);
            addItemStatus(itemData.m_nItemID, DownloadingState);
        }
        else
        {
            removeItemStatus(itemData.m_nItemID, DownloadingState);
            addItemStatus(itemData.m_nItemID, PausedState);
        }
    }

    nCol++;
    //同步
    CTableWidgetItem* pItemAddStatus = new CTableWidgetItem("");
    pItemAddStatus->setTextRetranslate("Not added", tr("Not added"));
    pItemAddStatus->setFont(font);
    pItemAddStatus->setForeground(QColor("#fd4a5b"));
    pItemAddStatus->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidgetDownload->setItem(nRowInsert, nCol, pItemAddStatus);
    ui->tableWidgetDownload->addRetranslateColumn(nCol);
    pItemAddStatus->setRetranslateContext(CLASSNAME_DOWNLOAD_WIDGET);
    if(m_mapItemStatus.contains(itemData.m_nItemID))
    {
        if(isItemStatus(itemData.m_nItemID, AddedState))
        {
            //pItemAddStatus->setText(tr("Added"));
            pItemAddStatus->setTextRetranslate("Added", tr("Added"));
        }
    }

    nCol++;
    //操作
    itemSpace = new QTableWidgetItem();//插入空对象，占位符,方便后面其他算获取pItem进行操作

    ui->tableWidgetDownload->setItem(nRowInsert, nCol, itemSpace);

    ui->tableWidgetDownload->setRowHeight(nRowInsert, 54);
}

/************************************
* 名称: getRowByActionID
* 功能: get row number
* 参数: [in]nActionID 下载项的ID
* 返回:   int
* 时间:   2016/05/31
* 作者:   suhaiwu
************************************/
int DownloadWidget::getRowByActionID(const int &nActionID)const
{
    int nTempRow = -1;

    const int nRowCount = ui->tableWidgetDownload->rowCount();
    for(int iRow = 0; iRow < nRowCount; ++iRow)
    {
        const int nTempActionID = getActionIDByRow(iRow);
        if(nTempActionID == nActionID)
        {
            nTempRow = iRow;

            return nTempRow;
        }
    }

    return nTempRow;
}

/************************************
* 名称: getActionIDByRow
* 功能: get ActionID
* 参数: [in]nRowTab  table row
* 返回:   int
* 时间:   2016/05/31
* 作者:   suhaiwu
************************************/
int DownloadWidget::getActionIDByRow(const int &nRowTab)const
{
    int nActionID = DEFAULT_INVALID_ACTIONID;
    QTableWidgetItem *pItem = ui->tableWidgetDownload->item(nRowTab, TABLE_COL_CHECKBOX);
    if(pItem)
    {
        nActionID = pItem->data(Constants::ActionItemID).value<int>();
    }

    return nActionID;
}

/************************************
* 名称: getActionIDByProgressBar
* 功能: get ActionID
* 参数: [in]pProgressBar  table progressBar
* 返回:   int
* 时间:   2016/05/31
* 作者:   suhaiwu
************************************/
int DownloadWidget::getActionIDByProgressBar(const QProgressBar * const pProgressBar) const
{
    int nActionID = DEFAULT_INVALID_ACTIONID;
    if(!pProgressBar)
    {
        return nActionID;
    }

    const QString objectName = pProgressBar->objectName();
    bool bOk = false;
    nActionID = objectName.toInt(&bOk);
    if(!bOk)
    {
       nActionID = DEFAULT_INVALID_ACTIONID;
    }

    return nActionID;
}

/************************************
* 名称: addItemStatus
* 功能: set item status of nActionID
* 参数: [in]nActionID     load item action id
* 参数: [in]eItemStatus   item status
* 返回:   bool
* 时间:   2016/06/01
* 作者:   suhaiwu
************************************/
bool DownloadWidget::addItemStatus(const int nActionID, const DownloadWidget::ItemStatus eItemStatus)
{
    m_mapItemStatus[nActionID] |= eItemStatus;

    return true;
}

/************************************
* 名称: removeItemStatus
* 功能: remove item status of nActionID
* 参数: [in]nActionID     load item action id
* 参数: [in]eItemStatus   item status
* 返回:   bool
* 时间:   2016/06/01
* 作者:   suhaiwu
************************************/
bool DownloadWidget::removeItemStatus(const int nActionID, const DownloadWidget::ItemStatus eItemStatus)
{
    bool bOk = false;
    if(m_mapItemStatus.contains(nActionID))
    {
        m_mapItemStatus[nActionID] &= ~eItemStatus;
        bOk = true;
    }

    return bOk;
}

/************************************
* 名称: getItemStatus
* 功能: get item status of nActionID
* 参数: [in]nActionID     load item action id
* 返回:   ItemStatus
* 时间:   2016/06/01
* 作者:   suhaiwu
************************************/
int DownloadWidget::getItemStatus(const int nActionID) const
{
     ItemStatus eItemStatus = UnknownState;
     if(m_mapItemStatus.contains(nActionID))
     {
         return m_mapItemStatus[nActionID];
     }

     return eItemStatus;
}

/************************************
* 名称: isTtemStatus
* 功能: just item status of nActionID
* 参数: [in]nActionID     load item action id
* 参数: [in]eItemStatus   item status
* 返回:   bool
* 时间:   2016/06/01
* 作者:   suhaiwu
************************************/
bool DownloadWidget::isItemStatus(const int nActionID, const DownloadWidget::ItemStatus eItemStatus) const
{
    if(eItemStatus & m_mapItemStatus[nActionID])
    {
        return true;
    }

    return false;
}


/************************************
* 名称: OnAddToSyncListclicked
* 功能: 添加到同步列表
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void DownloadWidget::OnAddToSyncListclicked()
{
    QVector<ActItemData> vecActions;
    for(int i = 0; i < ui->tableWidgetDownload->rowCount(); i++)
    {
        QCheckBox* pCheckBox = (QCheckBox*)ui->tableWidgetDownload->cellWidget(i, TABLE_COL_CHECKBOX);
        if(pCheckBox && pCheckBox->isChecked())
        {
            QTableWidgetItem *pItem = ui->tableWidgetDownload->item(i, TABLE_COL_CHECKBOX);
            if(pItem)
            {
                int nActionID = pItem->data(Constants::ActionItemID).value<int>();
                if(isItemStatus(m_nCurActionID, DownloadingState) ||
                        isItemStatus(m_nCurActionID, PausedState) ||
                        isItemStatus(m_nCurActionID, AddedState))
                {
                    continue;
                }
                vecActions.push_back(m_mapItemData[nActionID]);
            }
        }
    }
    if(vecActions.size() > 0)
    {
        emit SigAddToSyncList(vecActions);
    }
}

/************************************
* 名称: onRowCountChanged
* 功能: the slot to tabel row count
* 返回:   void
* 时间:   20161108
* 作者:   asu
************************************/
void DownloadWidget::onRowCountChanged(int nRowCount)
{
    OnItemCheckBoxClicked(false);
}

/************************************
* 名称: slotOnTableVScrollValueChanged
* 功能: 垂直滚动条值发生变化的槽函数，此时需要更新删除、
* 返回:   void
* 时间:   20161108
* 作者:   asu
************************************/
void DownloadWidget::slotOnTableVScrollValueChanged(int nValue)
{
    OnCellEntered(m_nRowPrevHover, 0);
}

/************************************
* 名称: OnPortOpenChanged
* 功能: 响应机器人连接状态改变
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void DownloadWidget::OnPortOpenChanged(BOOL bOpen)
{
    ui->pbAddToSyncList->setEnabled(bOpen);
}

#if 1
/************************************
* 名称: OnAddedAction
* 功能: 同步界面添加动作文件响应函数，更新状态用
* 参数: [in] sFilePath 动作列表文件路径
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void DownloadWidget::OnAddedAction(const QString& sFilePath)
{
    //比较md5值
    //QFileInfo addedFileInfo(filePath);
    for(int i = 0; i < ui->tableWidgetDownload->rowCount(); i++)
    {
        QTableWidgetItem *pItem = ui->tableWidgetDownload->item(i, TABLE_COL_CHECKBOX);
        if(pItem)
        {
            int nActionID = pItem->data(Constants::ActionItemID).value<int>();
            if(m_mapItemData[nActionID].m_nActType > eActTypeOther)
                return;

            QString sTempPath = m_mapItemData[nActionID].m_sLocalPath;
            sTempPath = sTempPath.left(sTempPath.lastIndexOf(QDir::separator()));

            //QString sActType = g_sActTypePrefix[m_mapItemData[nActionID].m_nActType];
            //sTempPath = sTempPath + sActType + m_mapItemData[nActionID].m_sActName + HTS_FILE_EXT;

            QString sFileDir = sFilePath.left(sFilePath.lastIndexOf(QDir::separator()));
            if(sFileDir == sTempPath)
            {
//                QFile addedFile(sFilePath);
//                addedFile.open(QIODevice::ReadOnly);
//                QByteArray baAdded = QCryptographicHash::hash(addedFile.readAll(),QCryptographicHash::Md5);
//                addedFile.close();

//                QFile fileTemp(sTempPath);
//                fileTemp.open(QIODevice::ReadOnly);
//                QByteArray baTemp = QCryptographicHash::hash(fileTemp.readAll(), QCryptographicHash::Md5);
//                fileTemp.close();

                //if(baAdded == baTemp)
                {
                    //设置同步列文本
                    pItem = ui->tableWidgetDownload->item(i, TABLE_COL_SYNC);
                    if(pItem)
                    {
                        pItem->setText(tr("Added"));
                    }

                    removeItemStatus(nActionID, NotAddedState);
                    addItemStatus(nActionID, AddedState);

                    break;
                }
            }
        }
    }
}
#endif

/************************************
* 名称: OnRemoveAction
* 功能: 同步界面移除动作文件响应函数，更新状态
* 参数: [in] sFilePath 动作列表文件路径
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void DownloadWidget::OnRemoveAction(const QString& sFilePath)
{
    //比较md5值
    //QFileInfo addedFileInfo(filePath);
    for(int i = 0; i < ui->tableWidgetDownload->rowCount(); i++)
    {
        QTableWidgetItem *pItem = ui->tableWidgetDownload->item(i, TABLE_COL_CHECKBOX);
        if(pItem)
        {
            int nActionID = pItem->data(Constants::ActionItemID).value<int>();

            QString sTempPath = m_mapItemData[nActionID].m_sLocalPath;
            sTempPath = sTempPath.left(sTempPath.lastIndexOf(QDir::separator()));

            //QString sActType = g_sActTypePrefix[m_mapItemData[nActionID].m_nActType];
            //sTempPath = sTempPath + sActType + m_mapItemData[nActionID].m_sActName + HTS_FILE_EXT;

            QString sFileDir = sFilePath.left(sFilePath.lastIndexOf(QDir::separator()));
            if(sFileDir == sTempPath)
            {
//                QFile addedFile(sFilePath);
//                addedFile.open(QIODevice::ReadOnly);
//                QByteArray baAdded = QCryptographicHash::hash(addedFile.readAll(),QCryptographicHash::Md5);
//                addedFile.close();

//                QFile fileTemp(sTempPath);
//                fileTemp.open(QIODevice::ReadOnly);
//                QByteArray baTemp = QCryptographicHash::hash(fileTemp.readAll(), QCryptographicHash::Md5);
//                fileTemp.close();

//                if(baAdded == baTemp)
                {
                    //设置同步列文本
                    CTableWidgetItem* pItemSync = (CTableWidgetItem*)ui->tableWidgetDownload->item(i, TABLE_COL_SYNC);
                    if(pItemSync)
                    {
                        //pItem->setText(tr("Not added"));
                        pItemSync->setTextRetranslate("Not added", tr("Not added"));
                    }

                    removeItemStatus(nActionID, AddedState);
                    addItemStatus(nActionID, NotAddedState);

                    break;
                }
            }
        }
    }
}

/************************************
* 名称: OnCheckBoxNotAddedClicked
* 功能: 仅显示未添加槽函数
* 参数: [in]checked 按钮的check状态
* 返回:   void
* 时间:   2016/04/11
* 作者:   hels
************************************/
void DownloadWidget::OnCheckBoxNotAddedClicked(bool checked)
{
   //如果有正在下载项，不操作
    for(int i = 0; i < ui->tableWidgetDownload->rowCount(); i++)
    {
        QTableWidgetItem *pItem = ui->tableWidgetDownload->item(i, TABLE_COL_CHECKBOX);
        if(pItem)
        {
            int nActionID = pItem->data(Constants::ActionItemID).value<int>();
            if(isItemStatus(nActionID, DownloadingState))
            {
                CMessageBox::warning(this, "", tr("Finding some actions is downing! please wait or pause it first!"));
                return;
            }
        }
    }
    //只显示未添加选项
    if(checked)
    {
        for(int i = (ui->tableWidgetDownload->rowCount()-1); i >=0; i--)
        {
            QTableWidgetItem *pItem = ui->tableWidgetDownload->item(i, TABLE_COL_CHECKBOX);
            if(pItem)
            {
                int nActionID = pItem->data(Constants::ActionItemID).value<int>();
                if(isItemStatus(nActionID, AddedState))
                {
                    ui->tableWidgetDownload->removeRow(i);
                }
            }
        }
    }
    else
    {
        if(ui->tableWidgetDownload->rowCount() < m_mapItemData.size())
        {
            ui->tableWidgetDownload->clearContents();
            ui->tableWidgetDownload->setRowCount(0);
            foreach (ActItemData itemData, m_mapItemData.values())
            {
                AddDownloadItem(-1, itemData, itemData.m_nHasDownloadedBytes, itemData.m_nActSize);
            }
        }
    }
}

/************************************
* 名称: slotOnDownloadFinish
* 功能: 动作下载完成后，更新进度条和下载状态
* 参数: nActionId：动作id
* 返回:   void
* 时间:   2016/07/14
* 作者:   hwx
************************************/
void DownloadWidget::slotOnDownloadFinish(int nActionId)
{
    removeItemStatus(nActionId, DownloadingState);
    removeItemStatus(nActionId, PausedState);
    addItemStatus(nActionId, FinishedState);

    if(m_mapProgressBar.contains(nActionId))
    {
        //Qt 帮助文档对于QTableWidget的说明
        //Sets the given widget to be displayed in the cell in the given row and column, passing the ownership of the widget to the table.
        //If cell widget A is replaced with cell widget B, cell widget A will be deleted.
        //因此不能直接删除CellWidget，否则程序会崩溃
        QProgressBar *pProgressBar = m_mapProgressBar[nActionId];
        m_mapProgressBar.remove(nActionId);
//                int nRow = pProgressBar->property(Constants::PROPERTY_ITEM_ROW).toInt();
//                int nCol = pProgressBar->property(Constants::PROPERTY_ITEM_COL).toInt();
        int nRow = getRowByActionID(nActionId);

        QWidget* pWidget = ui->tableWidgetDownload->cellWidget(nRow, TABLE_COL_CHECKBOX);
        if(pWidget)
        {
            ui->tableWidgetDownload->removeCellWidget(nRow, TABLE_COL_STATUS);
        }
        pProgressBar->deleteLater();

        QTableWidgetItem* pItem = ui->tableWidgetDownload->item(nRow, TABLE_COL_STATUS);
        if(pItem)
        {
            CTableWidgetItem* pTableItem = dynamic_cast<CTableWidgetItem*>(pItem);
            if(pTableItem)
            {
                pTableItem->setTextRetranslate("Downloaded", tr("Downloaded"));
            }
            //pItem->setText(tr("Downloaded"));
        }
    }
}

/************************************
* 名称: slotOnReceiveRobotTFCardStatus
* 功能: 机器人没有插入TF卡的响应函数，同步动作按钮设置为不可用状态
* 参数: bHasTFCard: 是否存在TF卡标识
* 返回:   void
* 时间:   2016/10/25
* 作者:   hwx
************************************/
void DownloadWidget::slotOnReceiveRobotTFCardStatus(bool bHasTFCard)
{
    ui->pbAddToSyncList->setEnabled(bHasTFCard);
}


void DownloadWidget::onCurrentCellChanged(int currentRow, int currentColumn, int /*previousRow*/, int /*previousColumn*/)
{
    m_nCurRow = currentRow;
    m_nCurCol = currentColumn;
}
