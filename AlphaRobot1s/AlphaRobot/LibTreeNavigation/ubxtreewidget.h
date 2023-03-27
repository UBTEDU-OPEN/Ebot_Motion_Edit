/*
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubxtreewidget.h
* 创建时间：2015/12/04 14:00
* 文件标识：
* 文件摘要：动作模板库导航树图界面设计类
*
* 当前版本：1.0.0.0
* 作    者：周志平
* 完成时间：2015/12/04 14:00
* 版本摘要：
*/
#ifndef UBXTREEWIDGET_H
#define UBXTREEWIDGET_H

#include <QDockWidget>
#include "ILibTreeNavigation.h"
#include <QTreeWidget>
#include <QWheelEvent>
#include <QDrag>
#include <QStyleOption>

#include <QCompleter>

#include "navigationconst.h"
#include "ubxundoredomanager.h"
#include "../zxLib/widget/cdockwidget.h"


namespace Ui {
class UBXTreeWidget;
}

class UBXTreeWidget : public CDockWidget, public ILibTreeNavigation, public zxObserverBaseEx
{
    Q_OBJECT

public:
    explicit UBXTreeWidget(QWidget *parent = 0);
    ~UBXTreeWidget();

public:
    //ITreeNavigationEvent 接口实现
    virtual bool AddNewTreeNode(char* pData, int nLen, QString strName);
    virtual bool connect(QWidget* pParent/*,ITreeNavigationEvent* pNotify*/);
    virtual void disconnect();
    virtual QWidget* GetWidget();
    virtual void SetFlowCtrl(IUBXMotionEditor* pFlowCtrl);
//    virtual void keyPressEvent(QKeyEvent * event);

public:
    // 观察者模式通知消息
    virtual void OnNotify(int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *);
    virtual void customEvent(QEvent * event) override;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    virtual void retranslateUi();

public:
    IUBXMotionEditor* m_pFlowCtrl;

private slots:
    void slotOnAddFileButtonClicked();
    void slotOnDeleteFileButtonClicked();
    void on_CustomTreeWidget_itemPressed(QTreeWidgetItem *item, int column);
    void onExpandShrinkPressed();
    void on_tabWidgetModel_currentChanged(int index);
    void slotOnSearch();
    void slotOnSearchTextEditFinished();

private:
    void setDefaultStyle();
    void saveToTemlpateDir(QString strFileName, char *pData, int nLen);

    bool createDir(QString& strPath);
    bool copyFileToPath(const QString& sourceDir ,const QString& toDir, bool coverFileIfExist);
    bool deleteFile(const QString& FileName);
    bool FindActionFromTreeWidget(QTreeWidget* treeWidget, QString tagName);

    void createPopMenuActions();

    QString getActionIconFile(QString sActionIconName);

    void addNodeItem(QTreeWidgetItem* pParentItem, QString strName, QString strIconFilePath);

    /************************************
    * 名称: getTemplateFiles
    * 功能: 获取指定路径下的动作模板文件
    * 参数: [in] strFilePath: 路径
    *           [out] lstFiles: 动作模板文件列表
    * 返回:  void
    * 时间:   2017/01/17
    * 作者:   hwx
    ************************************/
    void getTemplateFiles(const QString &strFilePath, QList<QString> &lstFiles);

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
    void updateTreeWidget(const QString &strFilePath, QTreeWidget *pTreeWidget, QList<QString> &lstFiles);

    /************************************
    * 名称: updateActionTemplate
    * 功能: 处理动作模板undo/redo操作
    * 参数:[in] pTemplateFileMgr: 模板文件列表
    * 返回:  void
    * 时间:   2017/01/17
    * 作者:   hwx
    ************************************/
    void updateActionTemplate(CStringListContainer *pTemplateFileMgr);

    // 创建默认文件夹
    void createDefaultDirs();

    // 初始化界面
    void init();

private:
    Ui::UBXTreeWidget *ui;

private:
    int m_currentTabIndex;  // 页签
    bool m_bShrink;

    QString m_strSearchText;  // 搜索文本

    CStringListContainer *m_pDefaultTemplateFileList;
    CStringListContainer *m_pCustTemplateFileList;

    QCompleter* m_pCompleterTemplateFind;
};

#endif // UBXTREEWIDGET_H
