#ifndef CALPHAROBOT_H
/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：cmyalpha.h
* 创建时间：2016/12/01
* 文件标识：
* 文件摘要：我的机器人控件
*
* 当前版本：1.0.0.0
* 作    者：hwx
* 完成时间：2016/12/01
* 版本摘要：
*/

#define CALPHAROBOT_H

#include <QWidget>
#include <QPushButton>
#include <QList>
#include <QCheckBox>

#include "widget/ubxwidget.h"
#include "UBXRobotController.h"
#include "czxqueue.h"
#include "UBXPublic.h"

namespace Ui {
class CMyAlpha;
}

class CMyAlpha : public UBXWidget, public  zxObserverBase
{
    Q_OBJECT

public:
    explicit CMyAlpha(QWidget *parent = 0);
    ~CMyAlpha();

    /**************************************************************************
    * 函数名: OnNotify
    * 功能: 处理通知消息
    *     * 参数:
    *    @[in ] pSubject: 主题
    *    @[in ] nMsgID: 消息ID
    *    @[in ] dwContext: 上下文
    *    @[in ] wParam: 参数1
    *    @[in ] lParam: 参数2
    *
    * 时间: 2016/12/02
    * 作者: hwx
    */
    virtual void OnNotify(const zxObserverSubjectBase* pSubject, int nMsgID, DWORD dwContext, WPARAM wParam, LPARAM lParam) override;

public:
    RETRANSLATEUIFUNC

    /************************************
    * 名称: requestRobotActionList
    * 功能: 请求机器人动作列表
    * 参数：
    * 返回:   void
    * 时间:   2016/12/01
    * 作者:   hwx
    ************************************/
    void requestRobotActionList();

    /************************************
    * 名称: checkSystemEmbedUpdate
    * 功能: 检查嵌入式版本更新
    * 参数：
    * 返回:   void
    * 时间:   2016/12/01
    * 作者:   hwx
    ************************************/
    void checkSystemEmbedUpdate();

    // 复位数据
    void resetData();

public slots:
    /************************************
    * 名称: slotOnGetSystemEmbedVersionInfo
    * 功能: 获取嵌入式系统版本信息的槽函数
    *     * 参数:
    *    @[in ] strCurrentVersion: 当前版本
    *    @[in ] strLatestVersion: 最新版本
    *    @[in ] nStatus: 是否需要更新
    * 返回:   void
    * 时间:   2016/12/01
    * 作者:   hwx
    ************************************/
    void slotOnGetSystemEmbedVersionInfo(const QString &strCurrentVersion, const QString &strLatestVersion, int nStatus);

    /**************************************************************************
    * 函数名: slotOnPortOpenChanged
    * 功能: 机器人连接状态变化的槽函数
    *     * 参数:
    *    @[in ] bConnected: 是否连接上机器人
    *
    * 时间: 2017/03/29
    * 作者: hwx
    */
    void slotOnPortOpenChanged(BOOL bConnected);

signals:

protected:
    virtual void customEvent(QEvent * event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject * watched, QEvent * event);

private slots:
    /**************************************************************************
    * 函数名: slotOnTableRowChanged
    * 功能: 表格行数发生变化槽函数
    *     * 参数:
    *    @[in ] nRowCount: 当前行数
    *
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void slotOnTableRowChanged(int nRowCount);

    /**************************************************************************
    * 函数名: slotOnRowCheckBoxStateChanged
    * 功能: 行选择框状态发生变化
    *     * 参数:
    *    @[in ] state: 选中状态
    *
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void slotOnRowCheckBoxStateChanged(int state);

    /**************************************************************************
    * 函数名: slotOnSelAllCheckBoxClicked
    * 功能: 全选按钮状态发生变化
    *     * 参数:
    *    @[in ] checked: 选中状态
    *
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void slotOnSelAllCheckBoxClicked(bool checked);

    /**************************************************************************
    * 函数名: slotOnDeleteSelectItemsButtonClicked
    * 功能: 点击表格下方的删除按钮，删除表格中选中的动作
    *  参数:
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void slotOnDeleteSelectItemsButtonClicked();

    /**************************************************************************
    * 函数名: slotOnDeleteRowItemButtonClicked
    * 功能: 点击表格某行中的删除按钮，删除指定行动作
    *  参数:
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void slotOnDeleteRowItemButtonClicked();

    /**************************************************************************
    * 函数名: slotOnTableCellEnter
    * 功能: 光标进入指定单元格的槽函数
    *     * 参数:
    *    @[in ] nRow: 行
   *    @[in ] nCol: 列
    *
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void slotOnTableCellEnter(int nRow, int nCol);

    /**************************************************************************
    * 函数名: slotOnUpdateButtonClicked
    * 功能: 更新版本槽函数
    * 参数:
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void slotOnUpdateButtonClicked();

    /**************************************************************************
    * 函数名: slotOnTableScrollValueChanged
    * 功能: 表格滚动条变化的槽函数
    * 参数:
    * 时间: 2017/04/05
    * 作者: hwx
    */
    void slotOnTableScrollValueChanged(int nValue);

private:
    /**************************************************************************
    * 函数名: setUpUI
    * 功能: 处理机器人动作列表
    *     * 参数:
    *
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void setupUI();

    void init();

    // 初始化机器人动作路径
    void initReqActionPath();

    /**************************************************************************
    * 函数名: processGetRobotActionListRsp
    * 功能: 处理机器人动作列表
    *     * 参数:
    *    @[in ] wParam: 参数1，true/false
    *    @[in ] lParam: 参数2，动作名称
    *
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void processGetRobotActionListRsp(WPARAM wParam, LPARAM lParam);

    /**************************************************************************
    * 函数名: processDeleteRobotActionRsp
    * 功能: 处理删除机器人动作响应
    *     * 参数:
    *    @[in ] eRsp: 响应类型
    *    @[in ] pActionName: 动作名称
    * 时间: 2016/12/02
    * 作者: hwx
    */
    void processDeleteRobotActionRsp(emDeleteRobotActionRsp eRsp, char * pActionName);

    /************************************
    * 名称: requestRobotAction
    * 功能: 请求指定路径下的机器人动作列表
    * 参数：[in]strPath: 路径
    * 返回:   void
    * 时间:   2016/12/01
    * 作者:   hwx
    ************************************/
    void requestRobotAction(const QString &strPath);

    /************************************
    * 名称: requestNextActionPath
    * 功能: 请求读取下一个机器人动作路径
    * 参数：
    * 返回:   void
    * 时间:   2016/12/01
    * 作者:   hwx
    ************************************/
    void requestNextActionPath();

    // 加载机器人图片
    void loadAlphaRobotImage(const QString &strImageFile = "");

    // 添加、删除动作
    void addActionToVector(const QString &strAction);
    void deleteActionFromVector(const QString &strAction);

    // 从缓存队列中删除第一个动作
    void deleteOneActionInQueue();

    // 删除指定名称的动作
    void deleteActionItem(const QString &strName);

    // 刷新表格数据
    void updateTable();
    void addActionToTable(const QString &strName);

    // 更新全选checkbox状态
    void updateSelAllCheckBoxState();

    // 设置删除按钮状态
    void setDeleteButtonState(bool bEnable);

    // 重置Hover行
    void resetHoverRow(int nRow, int nCol);

private:
    int m_nCurEnterRow;  // 当前光标滑过所在的行

    QString  m_strCurReqActionPath;  // 当前请求的机器人动作路径，由于机器人只返回动作名称，不返回路径，需要自己拼接路径
    CZXQueue<QString> m_qReqActionPaths; // 待读取的机器人动作路径

    QList<QString>   m_lstActions;   // 动作名称列表，含路径
    CZXQueue<QString> m_qDeleteActionList; // 待删除动作列表

    QPushButton      *m_pBtnDeleteItem; // 删除行按钮
    QCheckBox        *m_pCheckBoxSelAll; // 全选

private:
    Ui::CMyAlpha *ui;
};

#endif // CALPHAROBOT_H
