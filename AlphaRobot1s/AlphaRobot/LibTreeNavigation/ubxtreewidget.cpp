/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubxtreewidget.cpp
* 创建时间：2015/12/04 14:00
* 文件标识：
* 文件摘要：动作模板库导航树图界面设计类实现
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2015/12/04 14:00
* 版本摘要：
*/
#include "ubxtreewidget.h"
#include "ui_ubxtreewidget.h"
#include <QDir>
#include <QFileInfoList>
#include <QFileDialog>
#include <QDebug>
#include <QApplication>
#include <QMenu>
#include <QLineEdit>
#include <QResource>
#include <QStandardPaths>

#include "../zxLib/configs.h"
#include "../zxLib/widget/messagebox.h"
#include "widget/messagebox.h"
#include "filedirhandle.h"

/***********************************************************
 * 函数名： CreateTreeNavigation
 * 功能：  创建动作模板库树图导航窗口
 * 参数：  ILibTreeNavigation** ppUBXInavigationMotify（树图导向抽象类指针）
 * 返回值：bool 如果创建成功，返回真，否则，返回假
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
bool CreateTreeNavigation(ILibTreeNavigation** ppUBXInavigationMotify)
{
    UBXTreeWidget * pProxy = new UBXTreeWidget();
    if(pProxy)
    {
        *ppUBXInavigationMotify =(ILibTreeNavigation*)pProxy;
        return true;
    }
    return false;
}

UBXTreeWidget::UBXTreeWidget(QWidget *parent) :
    CDockWidget(parent),
    ui(new Ui::UBXTreeWidget),
    m_pFlowCtrl(NULL),
    m_pDefaultTemplateFileList(NULL),
    m_pCustTemplateFileList(NULL),
    m_pCompleterTemplateFind(NULL)
{
    ui->setupUi(this);

    QString sResDir;
#ifdef Q_OS_MAC
    sResDir = "/../res/treenavigation.rcc";
#else
    sResDir = "/res/treenavigation.rcc";
#endif
    QString rccPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath()+ sResDir);
    QResource::registerResource(rccPath);
    setDefaultStyle();

    createDefaultDirs();
    init();

    SetMsgType Msg;
    Msg.clear();
    Msg.insert(WM_MSG_EXECUTE_REDO);
    Msg.insert(WM_MSG_EXECUTE_UNDO);
    this->AddSubscribe(Msg);
    this->EnableRecvNotify(TRUE);

    on_tabWidgetModel_currentChanged(eTabDefault);
}


UBXTreeWidget::~UBXTreeWidget()
{
    SAFE_DELETE(m_pDefaultTemplateFileList);
    SAFE_DELETE(m_pCustTemplateFileList);

    delete ui;
}

/**************************************************************************
* 函数名:eventFilter
* 功能:事件筛选器
* 参数:QObject *obj  对象
* 参数:QEvent *event 事件
* 返回值:bool
* 时间:2015/01/24 20:58
* 作者: 周志平
* 改动日志
**************************************************************************/
bool UBXTreeWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        qDebug()<<"MouseButtonPress"<<endl;
    }
    else if(event ->type() == QEvent::KeyPress)
    {
        if(obj == ui->listActionName)
        {
            QKeyEvent* kEvent = static_cast<QKeyEvent* >(event);
            int uKey = kEvent->key();
            Qt::Key key = static_cast<Qt::Key>(uKey);
            if(key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)
            {
                slotOnSearch();
            }
        }
    }

    return QWidget::eventFilter(obj, event);

}

void UBXTreeWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void UBXTreeWidget::customEvent(QEvent *event)
{
    zxEvent* pEvent = (zxEvent*)event;
    DWORD nMsgID = pEvent->dwMsgId;
    DWORD eOperateCode = pEvent->dwContext;
    WPARAM wParam = pEvent->wParam;
    LPARAM lParam = pEvent->lParam;

    if (nMsgID == WM_MSG_EXECUTE_REDO || nMsgID == WM_MSG_EXECUTE_UNDO)
    {
        if (eOperateCode == eCodeHandleTemplateFile)
        {
            CStringListContainer *pFileMgr = (CStringListContainer *) lParam;
            updateActionTemplate(pFileMgr);
        }
  #if 0
        else if (eOperateCode == eCodeHandleSearchText)
        {
            QString strSearchText = QString::fromUtf8((char*)lParam);
            ui->listActionName->setEditText(strSearchText);
        }
#endif
    }
}

void UBXTreeWidget::resizeEvent(QResizeEvent *event)
{
    QDockWidget::resizeEvent(event);

    // Tab页大小自适应屏幕分辨率
    int nTabWidth = (ui->tabWidgetModel->width() - TAB_INVALID_SPACE)/2;
    ui->tabWidgetModel->setStyleSheet(QString("QTabBar::tab{width: %1px; height: 25px}").arg(nTabWidth));
}

void UBXTreeWidget::retranslateUi()
{
    ui->retranslateUi(this);
}

/***********************************************************
 * 函数名： connect
 * 功能：  将动作模板库树图导航窗口与父窗口相关联
 * 参数：  QWidget* pParent（父窗口）
 * 参数:  ILibTreeNavigation* pNotify  动作模板库树图导航窗口指针
 * 返回值：bool 如果关联成功，返回真，否则，返回假
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/

bool UBXTreeWidget::connect(QWidget* pParent/*,ITreeNavigationEvent* pNotify*/)
{
    //   m_pEvent = pNotify;
    //   ui->setupUi(this);

    return true;
}

void UBXTreeWidget::disconnect()
{
    disconnect();
    delete this;
}

QWidget *UBXTreeWidget::GetWidget()
{
    return this;
}

void UBXTreeWidget::SetFlowCtrl(IUBXMotionEditor *pFlowCtrl)
{
    m_pFlowCtrl = pFlowCtrl;
}

void UBXTreeWidget::OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam)
{
    zxEvent* pEvent = new zxEvent(nMsgID);
    pEvent->eEventType = zxEventTypeWindow;
    pEvent->lReceiver = (long)this;
    pEvent->dwMsgId = nMsgID;
    pEvent->dwContext = dwContext;
    pEvent->wParam = wParam;
    pEvent->lParam = lParam;
    QCoreApplication::postEvent(this, pEvent);
}

/***********************************************************
 * 函数名： addNodeItem
 * 功能：  向树图增加节点
 * 参数：  QTreeWidgetItem *pParentItem（树图节点指针）
 * 参数:   QString strName（节点名）
 * 参数：  QString strIconFilePath（图标文件全路径名）
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/

void UBXTreeWidget::addNodeItem(QTreeWidgetItem* pParentItem, QString strName, QString strIconFilePath)
{
    QTreeWidgetItem *pItem = new QTreeWidgetItem(pParentItem, QStringList(strName)); //添加子节点
    pParentItem->addChild(pItem);
    pItem->setIcon(0, QIcon(strIconFilePath));
}

/***********************************************************
 * 函数名： createDir
 * 功能：  创建动作模板库目录
 * 参数：  QString& strPath（路径名）
 * 返回值： bool 当创建成功时，返回真，否则，返回假
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
bool UBXTreeWidget::createDir(QString& strPath)
{
    bool ret = false;
    QDir dir;
    bool exist = dir.exists(strPath);
    if(!exist)
        ret = dir.mkdir(strPath);
    return ret;
}

/***********************************************************
 * 函数名： getActionIconFile
 * 功能：  将图标文件名转化为输出目录路径全路径图标文件名
 * 参数：  QString sActionIconName（图标全路径名）
 * 返回值： QString  当创建成功时，返回真，否则，返回假
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
QString UBXTreeWidget::getActionIconFile(QString /*sActionIconName*/)
{
    QString strIconFilePathName = "";
    strIconFilePathName = CConfigs::getLocalResPath("loginhead.png");  //暂时采用默认图标，后续从3D模型截图
    return strIconFilePathName;
}

/************************************
* 名称: getTemplateFiles
* 功能: 获取指定路径下的动作模板文件
* 参数: [in] strFilePath: 路径
*           [out] lstFiles: 动作模板文件列表
* 返回:  void
* 时间:   2017/01/17
* 作者:   hwx
************************************/
void UBXTreeWidget::getTemplateFiles(const QString &strFilePath, QList<QString> &lstFiles)
{
    lstFiles.clear();

    QDir dir(strFilePath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList listEntries = dir.entryInfoList();
    for (int i = 0; i < listEntries.size(); ++i)
    {
        QFileInfo fileInfo = listEntries.at(i);
        QString strExtName = "." + fileInfo.suffix();
        if(strExtName.compare(UBX_TEMPLATE_EXT, Qt::CaseInsensitive) != 0)
        {
            continue;
        }

        QString strFilePath = fileInfo.filePath();
        lstFiles.append(QDir::toNativeSeparators(strFilePath));
    }
    return;
}

/************************************
* 名称: updateTreeWidget
* 功能: 刷新treewidget
* 参数:[in] strFilePath: 模板文件路径
*           [in] pTreeWidget: 指定的treeWidget
*           [out] lstFiles: 动作模板文件列表
* 返回:  void
* 时间:   2017/01/17
* 作者:   hwx
************************************/
void UBXTreeWidget::updateTreeWidget(const QString &strFilePath, QTreeWidget *pTreeWidget, QList<QString> &lstFiles)
{
    if (!pTreeWidget)
    {
        return;
    }

    QFileInfo oFileInfo(strFilePath);
    QString strFileName =oFileInfo.baseName();

    pTreeWidget->clear();

    QString strIconFilePath = getActionIconFile("");
    QTreeWidgetItem *pRootItem = NULL;
    if(lstFiles.size() > 0)
    {
        pRootItem = new QTreeWidgetItem();   //添加第一个父节点
        pRootItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        pRootItem->setText(0, strFileName);   //文件夹1
        pRootItem->setIcon(0, QIcon(":/res/images/icon_dir.png"));
        pTreeWidget->addTopLevelItem(pRootItem);
    }

    for (int i = 0; i < lstFiles.size(); ++i)
    {
        QString strFilePath = lstFiles.at(i);
        QFileInfo fileInfo(strFilePath);
        QString strActionName = fileInfo.baseName();
        addNodeItem(pRootItem, strActionName, strIconFilePath);
    }

    pTreeWidget->expandAll();
}

/************************************
* 名称: updateActionTemplate
* 功能: 处理动作模板undo/redo操作
* 参数:[in] pTemplateFileMgr: 模板文件列表
* 返回:  void
* 时间:   2017/01/17
* 作者:   hwx
************************************/
void UBXTreeWidget::updateActionTemplate(CStringListContainer *pTemplateFileMgr)
{
    m_pCustTemplateFileList->setFiles(pTemplateFileMgr->getFiles());

    QList<QString> lstCustomDirFiles;
    getTemplateFiles(CConfigs::getLocalCusTemplateDir(), lstCustomDirFiles);

    // 执行删除操作后，再执行undo操作时，文件列表中会比custom目录下多出已经删除过的文件
    // 此时需要到临时目录下找回
    QList<QString> lstNeedDelete;
    for (int i = 0; i < m_pCustTemplateFileList->getFiles().size(); i++)
    {
        QString strFile = m_pCustTemplateFileList->getFiles().at(i);
        QFileInfo fileInfo(strFile);
        QString strFileName = fileInfo.fileName();
        if (fileInfo.exists())
        {
            continue;
        }

        QString strSrcFile = CConfigs::getFilePath(CConfigs::getLocalTempDir(), strFileName);
        if (!copyFileToPath(strSrcFile, strFile, true))
        {
            lstNeedDelete.append(strFile);
        }
    }

    m_pCustTemplateFileList->removeFiles(lstNeedDelete);

    // 从临时目录下找回了删除过的文件，再执行redo操作时，此时custom目录比文件列表多了删除过的文件
    // 需要删除这些文件，否则下次启动程序会读取到列表
    for (int i = 0; i < lstCustomDirFiles.size(); i++)
    {
        QString strFile = lstCustomDirFiles.at(i);
        if (m_pCustTemplateFileList->getFiles().contains(strFile))
        {
            continue;
        }

        deleteFile(strFile);
    }

    updateTreeWidget(CConfigs::getLocalCusTemplateDir(), ui->CustomTreeWidget, m_pCustTemplateFileList->getFiles());
}

void UBXTreeWidget::createDefaultDirs()
{
    QString strDefActionPath = CConfigs::getLocalDefaultTemplateDir();
#ifdef Q_OS_MAC
    QDir dir;
    bool exist = dir.exists(strDefActionPath);
    if(!exist)
    {
        CMessageBox::warning(NULL,"", tr("DefaultTemplate isn't exist!Please check your app!"));
    }
#else
    createDir(strDefActionPath); //判断系统动作模板库目录是否存在
#endif

    QString strCustActionPath = CConfigs::getLocalCusTemplateDir();
    createDir(strCustActionPath); //创建自定义动作模板库目录
}

void UBXTreeWidget::init()
{
    ui->DefaultTreeWidget->clear();
    //ui->DefaultTreeWidget->setSortingEnabled(true);
    ui->CustomTreeWidget->clear();
    //ui->CustomTreeWidget->setSortingEnabled(true);

    // add by huangwuxian 2016.06.29 禅道1017
    // 选中时在文本周围会出现虚线框，去掉QTreeWidgetItem的虚线框
    ui->DefaultTreeWidget->setFocusPolicy(Qt::NoFocus);
    ui->CustomTreeWidget->setFocusPolicy(Qt::NoFocus);

    m_pDefaultTemplateFileList = new CStringListContainer;
    m_pCustTemplateFileList = new CStringListContainer;

    m_currentTabIndex = eTabDefault;

    ui->tabWidgetModel->setCurrentIndex(m_currentTabIndex);
    ui->addFileButton->setEnabled(true);
    ui->addFileButton->setFlat(true);

    // 手动更新样式
    ui->addFileButton->setStyle(QApplication::style());

    //    showCustomTreeWedget();
    //    showDefaultTreeWedget();
    //先去除标题栏
    QWidget* widTitle = new QWidget();
    //    QHBoxLayout *layout = new QHBoxLayout;
    //    QPushButton * btn = new QPushButton("<<");
    //    QObject::connect(btn, &QPushButton::pressed, this, &UBXTreeWidget::onExpandShrinkPressed);
    //    btn->setFixedSize(25, 22);
    //    layout->addWidget(btn);
    //    layout->setMargin(0);
    //    widTitle->setLayout(layout);
    setTitleBarWidget(widTitle);
    ui->DefaultTreeWidget->installEventFilter(this);
    ui->CustomTreeWidget->installEventFilter(this);
    ui ->listActionName ->installEventFilter(this);

    m_bShrink = false;

    ui->DefaultTreeWidget->header()->hide();
    ui->DefaultTreeWidget->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->CustomTreeWidget->header()->hide();
    ui->CustomTreeWidget->setAttribute(Qt::WA_MacShowFocusRect, false);

//    delete ui->deleteFileButton;
//    delete ui->addMotionButton;
    ui->deleteFileButton->hide();
    ui->addMotionButton->hide();

    QLineEdit *pLineEdit = ui->listActionName->lineEdit();
    if (pLineEdit)
    {
        QObject::connect(pLineEdit, &QLineEdit::editingFinished, this, &UBXTreeWidget::slotOnSearchTextEditFinished);
    }
    QObject::connect(ui->addFileButton, &QPushButton::clicked, this, &UBXTreeWidget::slotOnAddFileButtonClicked);
    QObject::connect(ui->findButton, &QPushButton::clicked, this,&UBXTreeWidget::slotOnSearch);

    // 解析default路径下默认的动作模板
    QString strDefaultFile = CConfigs::getLocalDefaultTemplateDir();
    QString strCustomFile = CConfigs::getLocalCusTemplateDir();
    getTemplateFiles(strDefaultFile, m_pDefaultTemplateFileList->getFiles());
    getTemplateFiles(strCustomFile, m_pCustTemplateFileList->getFiles());
    updateTreeWidget(strDefaultFile, ui->DefaultTreeWidget, m_pDefaultTemplateFileList->getFiles());
    updateTreeWidget(strCustomFile, ui->CustomTreeWidget, m_pCustTemplateFileList->getFiles());
}

/***********************************************************
 * 函数名： on_addFileButton_clicked
 * 功能：  添加文件按钮响应事件（缩写版）
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
void UBXTreeWidget::slotOnAddFileButtonClicked()
{
    QTreeWidget * treeWidget = NULL;
    if(m_currentTabIndex == eTabDefault)
    {
        return;
    }
    else if(m_currentTabIndex == eTabCustom)
    {
        treeWidget = ui->CustomTreeWidget;
    }
    if(!treeWidget)
        return;

    QString sFmtTitle = tr("Template File");
    QString sFmt;
#ifdef Q_OS_MAC
    sFmt.sprintf("%s(*%s);;%s(*%s)", sFmtTitle.toStdString().c_str(), UBX_TEMPLATE_EXT, sFmtTitle.toStdString().c_str(), UBX_TEMPLATE_EXT);
#else
    sFmt.sprintf("%s(*%s)", sFmtTitle.toStdString().c_str(), UBX_TEMPLATE_EXT);
#endif
//    QString sAppLocalDownLoadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"), QDir::currentPath(), sFmt);
    if(fileName.isEmpty())
    {
        return;
    }
    fileName = QDir::toNativeSeparators(fileName);

//    copyFileToPath(fileName,m_cusActionDir,true);
    QFileInfo fileInfo(fileName);
    QString strCustTemplateDir = CConfigs::getLocalCusTemplateDir();
    QString strActionFilePathName = CConfigs::getFilePath(strCustTemplateDir, fileInfo.fileName());
    copyFileToPath(fileName, strActionFilePathName, true);

    ui->CustomTreeWidget->clear();
    //getTemplateFiles(strCustTemplateDir, m_pCustTemplateFileList->getFiles());
    m_pCustTemplateFileList->addFile(strActionFilePathName);
    updateTreeWidget(strCustTemplateDir, ui->CustomTreeWidget, m_pCustTemplateFileList->getFiles());
}

/***********************************************************
 * 函数名： copyFileToPath
 * 功能：  将文件拷贝到动作模板库目录
 * 参数：  const QString& sourceDir  源文件全路径名
 * 参数:   const QString& toDir      目标文件全路径名
 * 参数：  bool coverFileIfExist     是否覆盖已存在的文件
 * 参数：  bool 如果拷贝成功返回真，否则，返回假
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
bool UBXTreeWidget::copyFileToPath(const QString& sourceDir ,const QString& toDir, bool coverFileIfExist)
{
    //    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}

/***********************************************************
 * 函数名： on_deleteFileButton_clicked
 * 功能：  删除文件按钮响应事件（缩写版）
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
void UBXTreeWidget::slotOnDeleteFileButtonClicked()
{
    QTreeWidgetItem * currentItem = ui->CustomTreeWidget->currentItem();

    if(currentItem==Q_NULLPTR)
    {
        return;
    }
    //如果没有父节点不能删除
    if(currentItem->parent()==Q_NULLPTR)
    {
        //        delete ui->CustomTreeWidget->takeTopLevelItem(ui->CustomTreeWidget->currentIndex().row());
        return;
    }
    else
    {
        // 记录删除前的数据
        IOperation *pHandle = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleTemplateFile);
        pHandle->cloneDataBeforeChanged((LPARAM)m_pCustTemplateFileList, NULL);

        //如果有父节点就要用父节点的takeChild删除节点
        QString itemName = currentItem->text(0);
        itemName +=UBX_TEMPLATE_EXT;
        QString curItemFilePathName = CConfigs::getFilePath(CConfigs::getLocalCusTemplateDir(), itemName);
        qDebug()<<curItemFilePathName<<endl;

        // 为了实现撤销重做功能，删除前先将文件拷贝到临时文件夹，以便恢复
        QString strTempFile = CConfigs::getFilePath(CConfigs::getLocalTempDir(), itemName);
        copyFileToPath(curItemFilePathName, strTempFile, true);

        if(!deleteFile(curItemFilePathName))
        {
            qDebug()<<"Can't delete"<<curItemFilePathName<<"Please check if file exist"<<endl;
        }
        m_pCustTemplateFileList->removeFile(curItemFilePathName);
        updateTreeWidget(CConfigs::getLocalCusTemplateDir(), ui->CustomTreeWidget, m_pCustTemplateFileList->getFiles());

        // 记录删除后的数据
        pHandle->cloneDataAfterChanged((LPARAM)m_pCustTemplateFileList, NULL);
    }
}

/***********************************************************
 * 函数名： on_deleteFileButton_clicked
 * 功能：  删除文件按钮响应事件（缩写版）
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
bool UBXTreeWidget::deleteFile(const QString& FileName)
{
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(FileName);
    if (exist)
    {
        createfile->remove(FileName);
        return true;
    }
    else
    {
        return false;
    }
}

/***********************************************************
 * 函数名： onSearch
 * 功能：  查找文件
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
void UBXTreeWidget::slotOnSearch()
{
    QString tagName = ui->listActionName->currentText();
    QTreeWidget * curTreeWidget = NULL;
    if(m_currentTabIndex == eTabDefault)
    {
        curTreeWidget = ui->DefaultTreeWidget;
    }
    else if(m_currentTabIndex == eTabCustom)
    {
        curTreeWidget = ui->CustomTreeWidget;
    }
    FindActionFromTreeWidget(curTreeWidget, tagName);
}

void UBXTreeWidget::slotOnSearchTextEditFinished()
{
    const QString strText = ui->listActionName->currentText();
    m_strSearchText = strText;

// 搜索功能不做撤销重做功能
#if 0
    IOperation *pHandle = UBXUndoRedoManager::getInstance()->createOperation(eCodeHandleSearchText);
    pHandle->cloneDataBeforeChanged((LPARAM)(m_strSearchText.toUtf8().data()), NULL);
    m_strSearchText = strText;
    pHandle->cloneDataAfterChanged((LPARAM)(m_strSearchText.toUtf8().data()), NULL);
#endif
}

/***********************************************************a
 * 函数名： FindActionFromTreeWidget
 * 功能：  查找动作名
 * 参数：  QTreeWidget* treeWidget (树图指针)
 * 参数：  QString tagName (要查找的动作名)
 * 返回值： bool 如果查找成功则返回真，否则返回假
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
bool UBXTreeWidget::FindActionFromTreeWidget(QTreeWidget* treeWidget, QString tagName)
{
    QTreeWidgetItemIterator it(treeWidget);
    QTreeWidgetItem* pFindItem = NULL;
    int nMaxMatchCount = 0;
    QString strMostMatched; // 记录最匹配
    while (*it)
    {
        if((*it)->childCount() != 0)
        {
            ++it;
            continue;
        }

        QString strItemName = (*it)->text(0);
        if (strItemName.contains(tagName, Qt::CaseSensitive) && strItemName.length() > strMostMatched.length())
        {
            strMostMatched = strItemName;
            pFindItem = *it;
        }

// add by huangwuxian 下列比较算法有问题，
//  tagName=abc, itemName=ab竟然也会匹配上
#if 0
        //比较算法
        QString itemName =(*it)->text(0);
        int nItemLen = itemName.length();
        int ntagLen = tagName.length();
        int nLen = (nItemLen>ntagLen)?nItemLen:ntagLen;
        int nMatchCount = 0;
        for(int i = 0; i < nLen; ++i)
        {
            if(QString::compare(itemName.left(i+1), tagName.left(i+1), Qt::CaseInsensitive) == 0)
            {
                nMatchCount++;
            }
        }
        if(nMatchCount > nMaxMatchCount)
        {
            nMaxMatchCount = nMatchCount;
            pFindItem = *it;
        }
#endif
        ++it;
    }

    if(pFindItem)
    {
        treeWidget->setCurrentItem(pFindItem);
        return true;
    }

    treeWidget->setCurrentItem(NULL);
    CMessageBox::information(NULL, tr(""), tr("Failed to find any matched template file."));
    return false;

}
/***********************************************************************
 * 函数名：AddNewTreeNode
 * 功能：  从动作编辑区获取动作帧添加到树图并保存文件
 * 参数：  char * pData   动作帧数据内容
 * 参数：  int nLen       动作帧数据长度
 * 参数：  QString strName 动作名
 * 返回值： bool
 * 作者：  周志平
 ***********************************************************************/
bool UBXTreeWidget::AddNewTreeNode(char *pData, int nLen, QString strName)
{
//    QString strActionFilePathName =m_cusActionDir +"/" + strName + UBX_TEMPLATE_EXT;
    QString strActionFilePathName = CConfigs::getFilePath(CConfigs::getLocalCusTemplateDir(), strName + UBX_TEMPLATE_EXT);
    qDebug()<< strActionFilePathName <<endl;
    saveToTemlpateDir(strActionFilePathName, pData, nLen);
    return true;
}


//void UBXTreeWidget::on_DefaultTreeWidget_itemPressed(QTreeWidgetItem *item, int column)
//{

//}

/***********************************************************************
 * 函数名： createPopMenuActions
 * 功能：  创建弹出菜单
 * 返回值： void
 * 作者：  周志平
 ***********************************************************************/
void UBXTreeWidget::createPopMenuActions()
{
    //创建菜单、菜单项
    QMenu* pop_menu = new QMenu();
    QAction* AddActionFile = new QAction(tr("Add"),this);
    QAction* DeleteAction = new QAction(tr("Delete"),this);

    pop_menu->addAction(AddActionFile);
    pop_menu->addAction(DeleteAction);
    QAbstractItemDelegate::connect(AddActionFile,&QAction::triggered,this,&UBXTreeWidget::slotOnAddFileButtonClicked);
    QAbstractItemDelegate::connect(DeleteAction,&QAction::triggered,this,&UBXTreeWidget::slotOnDeleteFileButtonClicked);

    //菜单出现的位置为当前鼠标的位置
    QFile styleSheet(":/res/qss/TreeNavigationMenuDefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        pop_menu->setStyleSheet(styleSheet.readAll());
    }
    pop_menu->exec(QCursor::pos());
    pop_menu->deleteLater();
    //连接信号与槽

}

/***********************************************************************
 * 函数名： on_CustomTreeWidget_itemPressed
 * 功能：  单击树图节点消息响应事件
 * 参数:   QTreeWidgetItem *item(选中的节点)
 * 参数：  int column （列）
 * 返回值： void
 * 作者：  周志平
 ***********************************************************************/
void UBXTreeWidget::on_CustomTreeWidget_itemPressed(QTreeWidgetItem *item, int column)
{
    if(qApp->mouseButtons() == Qt::RightButton) //右键
    {
        if(item==Q_NULLPTR)
        {
            return;
        }
        //如果没有父节点不能删除
        if(item->parent()==Q_NULLPTR)
        {
            //        delete ui->CustomTreeWidget->takeTopLevelItem(ui->CustomTreeWidget->currentIndex().row());
            return;
        }
        else
        {
            createPopMenuActions();
        }

    }
}

void UBXTreeWidget::onExpandShrinkPressed()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if(!m_bShrink)
    {
        ui->listActionName->hide();
        ui->findButton->hide();
        ui->addFileButton->hide();
        ui->tabWidgetModel->hide();
        btn->setText(">>");
        m_bShrink = true;
    }
    else
    {
        ui->listActionName->show();
        ui->findButton->show();
        ui->addFileButton->show();
        ui->tabWidgetModel->show();
        btn->setText("<<");
        m_bShrink = false;
    }
}

void UBXTreeWidget::setDefaultStyle()
{
    QFile styleSheet(":/res/qss/TreeNavigationDefault.qss");
    if(styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }
}
/**************************************************************************
* 函数名:saveToTemlpateDir
* 功能: 保存到模板库
* 参数:QString strFileName   文件名
* 参数:char * pDate 动作组数据
* 参数:int nLen  动作组数据长度
* 返回值:
* 时间:2016/03/12 12:00
* 作者: 周志平
* 改动日志
**************************************************************************/
void UBXTreeWidget::saveToTemlpateDir(QString strFileName, char *pData, int nLen)
{
    char* pSaveData = NULL;
    int nTotalLen = 0;
    vector <char> DataSave;

    //保存总的数据长度，先占位，后门再改为实际长度
    AddVectorData(&DataSave, (char*)&nTotalLen, sizeof(nTotalLen));


    //保存文件头长度
    int nTagLen = strlen(UBX_TEMPLATE_TAG);
    AddVectorData(&DataSave, (char*)&nTagLen, sizeof(nTagLen));
    // 保存文件头
    AddVectorData(&DataSave, UBX_TEMPLATE_TAG, strlen(UBX_TEMPLATE_TAG));

    //长度信息
    AddVectorData(&DataSave, (char*)&nLen, sizeof(nLen));

    //保存文件信息
    AddVectorData(&DataSave, pData,nLen);

    nTotalLen  = DataSave.size();
    ModifyVertorData(DataSave, 0, (char*)&nTotalLen, sizeof(nTotalLen));

    if(nTotalLen)
    {
        pSaveData = new char [nTotalLen];
        CopyVertorToPointer(DataSave, pSaveData);
        //写入文件
        QFile file(strFileName);
        file.open(QFile::ReadWrite);
        file.write(pSaveData,nTotalLen);
        file.close();
        ui->CustomTreeWidget->clear();

        //添加到模板库树图
        //getTemplateFiles(CConfigs::getLocalDefaultTemplateDir(), m_pCustTemplateFileList->getFiles());
        m_pCustTemplateFileList->addFile(strFileName);
        updateTreeWidget(CConfigs::getLocalCusTemplateDir(), ui->CustomTreeWidget, m_pCustTemplateFileList->getFiles());
    }

    if(pData)
    {
        delete[]pData;
        pData = NULL;
    }
}

//void UBXTreeWidget::showValue(int Value)
//{
////    ui->label->setText(tr("获取的值是：%1").arg(Value));
////    int  a =inherits("sss");

//}

/***********************************************************
 * 函数名： on_tabWidget_currentChanged
 * 功能：  当前表格选项切换响应事件（缩写）
 * 参数：  int index(序列号)
 * 作者：  周志平
 * 时间：  2015/12/04 14:00:00
 ************************************************************/
void UBXTreeWidget::on_tabWidgetModel_currentChanged(int index)
{
    QStringList lstTempleAct;

    if(index == eTabDefault)
    {
        ui->addFileButton->setEnabled(false);
        ui->addFileButton->setFlat(false);
        ui->addFileButton->setStyle(QApplication::style());

        //ui->addMotionButton->setEnabled(false);
        //        ui->newDirButton ->setEnabled(false);
        //ui->deleteFileButton->setEnabled(false);

        QList<QString>  strList = m_pDefaultTemplateFileList->getFiles();
        for(int i = 0; i < strList.size(); i++)
        {
            lstTempleAct.append(CFileDirHandle::getFileBaseName(strList.at(i)));
        }
    }
    else if(index == eTabCustom)
    {
        ui->addFileButton->setEnabled(true);
        ui->addFileButton->setFlat(true);
        ui->addFileButton->setStyle(QApplication::style());
        //ui->addMotionButton->setEnabled(true);
        //        ui->newDirButton ->setEnabled(true);
        //ui->deleteFileButton->setEnabled(true);

        QList<QString>  strList = m_pCustTemplateFileList->getFiles();
        for(int i = 0; i < strList.size(); i++)
        {
            lstTempleAct.append(CFileDirHandle::getFileBaseName(strList.at(i)));
        }
    }
    m_currentTabIndex =index;


    SAFE_DELETE(m_pCompleterTemplateFind);
    m_pCompleterTemplateFind = new QCompleter(lstTempleAct, this);
    m_pCompleterTemplateFind->setCaseSensitivity(Qt::CaseInsensitive);

    m_pCompleterTemplateFind->setCompletionMode(QCompleter::PopupCompletion);
    ui->listActionName->lineEdit()->setCompleter(m_pCompleterTemplateFind);
}
