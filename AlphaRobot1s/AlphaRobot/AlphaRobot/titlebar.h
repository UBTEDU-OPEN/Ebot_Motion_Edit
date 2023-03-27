/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：titlebar.h
* 创建时间：2016/02/29
* 文件标识：
* 文件摘要：主窗体标题栏（自绘窗体标题栏）
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/02/29
* 版本摘要：
*/


#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QStyleOption>
#include <QButtonGroup>
#include <QMenu>


#include "../zxLib/widget/ubxwidget.h"


class CMenu;

namespace Ui {
class TitleBar;
}

class TitleBar : public UBXWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = 0);
    ~TitleBar();

public:
    QIcon getVersionStatusIcon(const int nStatus)const;

    //创建菜单
    void createMenu();

    // 是否展示欢迎标签
    void showWelcomeLable(bool bShow = true);

    void setVideoTutorialStatus(const int nStatus);
    void setSoftVersionStatus(const int nStatus);
    void setFirmwareVersionStatus(const int nStatus);
    void clearVideoTutorialStatus();
    void clearSoftVersionStatus();
    void clearFirmwareVersionStatus();

    //for update menu alert sation
    void updateMenuAlertSation();    

    static bool getAlertMarkFlag(const int nStation);

protected:
    //If you subclass from QWidget, you need to provide a paintEvent for your custom QWidget as below:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *obj, QEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

    RETRANSLATEUIFUNC

signals:
    void clickTab(int nID);
    void SigClose();
    void SigMin();
    void SigMax();
    void SigLogin();
    void SigRestore();
    void SigOpenPort();

    void sigProjectNew();
    void sigProjectOpen();
    void sigProjectSave();
    void sigProjectSaveAs();
    void sigExportHts();
    void sigExportActMixXml();

    void sigOpenHelpDoc();
    void sigOpenVideoTutorial();
    void sigCheckUpdateSysEmbed();
    void sigPCSoftVersion();
    void sigOnSetting();

    void sigOnDoubleClicked();

    //点击关于菜单信号
    void sigAbout();    

public slots:
    void OnPortOpenChanged(BOOL bOpen);
    void onTextChange(QString text);
    void onMax();
    void onRestore();
    void onMenuAlertMark(bool bAertMark);

    // 切换菜单栏编辑、动作库、同步页
    void slotOnSwitchTab(int nTabId);



private slots:
    void onButtonToggled(int id, bool checked);
    void OnPbCloseClicked();
    void OnPbMinClicked();
    void OnPbMaxClicked();

    void on_pbConnectRobot_clicked();
    void on_pBUserLogin_clicked();

    //菜单"视图"响应
    void on_menuview_clicked();
    //菜单"帮助"响应
    void on_menuhelp_clicked();
    //菜单"关于"响应
    void on_menuabout_clicked();
    //菜单点击响应
    void on_menu_clicked();

private:
    Ui::TitleBar *ui;
    QButtonGroup m_btnGroup;//管理radiobutton

    CMenu*       m_pWinMenu;
    QAction       *m_pActionVideoTutorial;  //
    QAction       *m_pActionOpenHelpDoc;  //
    QAction       *m_pActionRobotFirmwareVersion;  // 更新机器人软件
    QAction       *m_pActionRobotPCSoftVersion;  //

    bool         m_bIsWindowMax;  //窗口是否是最大化
};

#endif // TITLEBAR_H
