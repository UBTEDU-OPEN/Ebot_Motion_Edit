/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ActionLayer.h
* 创建时间：2016/01/12
* 文件标识：
* 文件摘要：动作层类，负责动作层绘制，同步等
*
* 当前版本：1.0.0.0
* 作    者：hels
* 完成时间：2016/01/12
* 版本摘要：
*/


#ifndef ACTIONLAYER_H
#define ACTIONLAYER_H
#include <QObject>
#include <QList>
#include "datamodel/ActionBlockDataManager.h"

class MP3Widget;

class ActionLayer : public QObject
{
    Q_OBJECT
public:
    ActionLayer(MP3Widget* mp3Widget);
    void SetActionBlockDataManager(CActionBlockDataManager* pActionBlockDataManger);
    CActionBlockDataManager* GetActionBlockDataManager();
    //void SetSelectGroupBlockCol(int nCol);
    void SetSelectGroupBlockID(int nID);
    void setSelectGroupBlockID(QList<int> lstGroupID);
    void ClearSelectGroupBlock();

public slots:
    //hels 绘制动作层
    void OnDrawActionGroup(QPixmap* pixmap);

    // 在波形上绘制动作帧
    void onDrawActionFrame(QPixmap *pixmap);

private:
    //hels 绘制动作组，包含组名等
    void DrawBlockGroup(QPixmap* pixmap);

    // 在波形上绘制动作帧
    void drawActionFrames(QPixmap *pixmap);

    //hels 绘制选中组中Action
    void DrawBlockAction(QPixmap* pixmap);

    // 获取当前动作组的上一个动作组
    CActionBlockGroupManager *getPreviousGroupMgr(CActionBlockGroupManager *pGroupMgr);

    //hels 获取选中的组中动作
    void GetSelectGroupAction(int nCol, int& nGroupIndex, int& nActionIndex, int& nTimeOffset);
    bool IsSelectGroupBlock(int nID);

private:
    CActionBlockDataManager* m_pActionBlockDataManager;
    MP3Widget* m_pMP3Widget;
    QList<int> m_listGroupSelectID;

    float m_fCurStartTime;
    float m_fCurEndTime;
};

#endif // ACTIONLAYER_H
