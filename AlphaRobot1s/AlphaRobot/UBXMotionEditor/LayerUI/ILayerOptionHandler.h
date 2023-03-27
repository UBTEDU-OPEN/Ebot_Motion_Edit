﻿#ifndef _ILAYEROPTIONHANDLER_H_
#define _ILAYEROPTIONHANDLER_H_

#include <QMimeData>

#include "stable.h"
#include "datamodel/ActionBlockDataManager.h"

class CActionBlockData;
class CActionBlockDataManager;
class IUILayerManager;
class ILayerOptionHandler_Event
{
public:
	// 添加ACTION BLOCK数据
	// nGroupManagerID = -1时，表示通知上层创建一个GROUP DATA MANAGER块，并把数据添加进去
	// nActionBlockID = -1时， 表示由GROUP MANAGER去分配ID
    virtual void ILayerOptionHandler_OnAddActionBlock(int nGroupManagerID, CActionBlockData* pActionBlockData,QString strBlockName = QObject::tr(""), bool bIsUpdateMP3Widget = false) = 0;
	// 用户点击，改变当前选择的组
    virtual void ILayerOptionHandler_OnAddWave(QString strWavePath ){}
	virtual void ILayerOptionHandler_OnSelectGroupManageChange(int nGroupID, int nActionId) = 0;
	virtual void ILayerOptionHandler_OnSelectActionManageChange(int nGroupID, int nActionIndex) = 0;
    virtual void ILayerOptionHandler_SetActionBlockState(bool bState) {}
    //virtual void ILayerOptionHandler_OnSelectGroupManageChange(int nGroupID) = 0;
    virtual void ILayerOptionHandler_SetBPMValue(int nBPMValue) {}
	// 数据改变
	virtual void ILayerOptionHandler_OnActionBlockDataChange(int nGroupID, int nActionBlockID, float fTimeInc) = 0;
	// 组数据改变
    virtual void ILayerOptionHandler_OnGroupManagerChange(int nGroupID, float fTimeInc, bool bIsUpdateMp3Widget = false) = 0;
	// 从OptionsDlg中删除一个音乐Data（在1S中即为删除一个组）
    virtual void ILayerOptionHandler_OnDeleteDataBlock(int nGroupID) {}
	// 获取时间粒度值
	virtual int ILayerOptionHandler_GetScaleValue() = 0;
	// 设置时间粒度值
	virtual void ILayerOptionHandler_SetScaleValue(int nScale) = 0;
	// 删除组块
    virtual void ILayerOptionHandler_OnDeleteGroupBlock(bool bCopyMotion = true) = 0;
	// 双击块仿真
	virtual void ILayerOptionHandler_OnDataBlockPlaying(CActionBlockData *pActionData) = 0;
	virtual void ILayerOptionHandler_ModifyGroupName(int nCol) = 0;
	virtual void ILayerOptionHandler_CopyGroup() = 0;
	virtual void ILayerOptionHandler_PasteGroup(int nCol) = 0;

	virtual void ILayerOptionHandler_SetSelectGroupBlock(int nCol) = 0;
	// 设置当前文件的修改状态
    virtual void ILayerOptionHandler_SetModifiedState(bool bState) = 0;
	virtual void ILayerOptionHandler_InsertGroupByMenu(CActionBlockData* pActionBlockData, int nCol) = 0;

    //hels MusicLayer实现，此接口会由MP3模块调用，通知Motion模块做相应的操作
    virtual void ILayerOptionHandler_OnLButtonDown(int nCol, QPoint point, float fStartTime){}//切换选中组
    virtual void ILayerOptinHandler_OnLButtonDBClick(int nCol){}//双击播放动作
    virtual void ILayerOptinHandler_OnLButtonDownAndShift(int nCol, QPoint point, float fStartTime){}//select more
    virtual void ILayerOptionHanlder_OnStopPlay(){}//停止动作播放，时间线会归位到0
    virtual void ILayerOptionHanlder_LayerPlaying(){}//点击Simulate按钮时触发，通知Motion模块仿真
    virtual void ILayerOptionHanlder_LayerPause(){}//暂停仿真,跟停止略有不同，暂停不会重置时间线的位置
    virtual void ILayerOptinHandler_FinishMusicPlaying(){}//音乐播放完成时通知
    virtual void ILayerOptionHandler_MusicLoaded(const QString& path, const QString& title, int time){}//音乐加载完成时通知，Motion模块更新名称和时长,时间单位为s
    virtual void ILayerOptionHandler_UpdateMusicPosition(quint64 position){}//音乐播放的位置时间单位为ms
    virtual void ILayerOptionHandler_DeleteMusic(){}//删除音乐时调用

    //hels UILayerCommonImpl实现，供Motion模块调用,并通知MP3模块响应操作
    virtual void ILayerOptionHanlder_UnitedSimulation(){}//开始联合仿真
    virtual void ILayerOptionHanlder_PauseUnitedSimulation(){}//暂停联合仿真
    virtual void ILayerOptionHanlder_StopUnitedSimulation(){}//停止联合仿真
    virtual void ILayerOptionHandler_OnRButtonDown(int nCol, QPoint point){}
    virtual void ILayerOptionHandler_HasMotionData(bool hasData){}//通知MP3是否有动作帧，以便刷新按钮状态

    virtual void Ilayeroptionhandler_MotionlayerActionCommand(uint nCommand, int nCol){}
    //CUILayerCommonImpl实现，此接口会由MP3模块，调用过来，获取Group状态
    virtual bool ILayerOptionHandler_IsPlaying(){ return true;}

    virtual void Ilayeroptionhandler_TreeToView(char *pData, int nLen, int nCol, QString FileName){}

    virtual void ILayerOptionHandler_SetMusicModifiedState(bool bState){}
    virtual bool ILayerOptionHandler_GetMusicModifiedState(){return true;}

    virtual IUILayerManager *ILayerOptionHandler_GetLayerManager() { return NULL; }

    virtual emPlayType ILayerOptionHandler_GetPlayType() =0; // 获取当前播放类型

    // 判断是否复制动作帧，音乐层的paste操作需要从动作层获取该信息
    virtual bool ILayerOptionHandler_hasCopyFrame() { return false; }
};

class ILayerOptionHandler
{
public:
    virtual ~ILayerOptionHandler(){}
    virtual bool Connect(QWidget* pParent, ILayerOptionHandler_Event* pEvent) = 0;
    virtual void ReleaseConnection() = 0;
    virtual QWidget* GetWidget() = 0;
	virtual void SetActionBlockDataManager(CActionBlockDataManager* pManager, int nCurrentGroupID,int nCurrentActionID) = 0;
    //hels MP3模块实现，提供给Motion模块获取当前选中列，两边的接口通信均以以ms为单位
    virtual int GetCurrentSelectCol() {return 0;}
    //hels MP3模块实现，因为ActionBlock中存储的时间需要乘以时间粒度才能得到正确的时间，因此需要将时间粒度设置给MP3模块，以便绘制动作组
    virtual void SetTimeScale(int timeScale){}
    virtual void UpdateMP3Action(int nGroupID, float fTime){}
    virtual void UpdateMP3Action(int nCol){}
    virtual void SetCellCount(int nCount){}
    virtual void SetMusicResource(QString strFilePath){}
    virtual void HasMotionData(bool hasData){}

    //hels 通知MP3,Motion模块，动作播放停止或者结束
    virtual void MotionPlayStop(int nPlayStatus){}
    virtual bool IsMusicLoaded(){return true;}//判断音乐是否加载，供Motion模块调用

    //hels Motion模块实现，MP3播放结束时，通知Motion模块更新按钮状态
    virtual void FinishPlayingMusic(){}
    virtual void MusicLoaded(const QString& title, int time){}//Motion模块更新名称和时长,时间单位为s
    virtual void UpdateMusicPosition(quint64 position){}//音乐播放的位置时间单位为ms

    virtual void Set3DRobotViewInterface(IUBXRobot3DView* pRobotView) {}

    virtual void PlayMusic(int nStartTime , int nEndTime,emPlayStatus ePlayState) {}
    virtual void SetMusicBeatsTime(int nBeatTime) {}
    virtual int  GetCurrentPos() {return 0;}
    virtual void SetControlState(bool controlState, int nLayerType = 1) = 0;//新增nLayerType,ACTION_LAYER_MOTION		1
    //由于层的框架已经修改，mp3模块独立负责时间波形和动作层的绘制，因此通知时需要通知层类型
	virtual void InsertGroupByMenu(int nCol) = 0;
    virtual void AddGroupByMenu() {}
//	virtual void LayerDebugStop() = 0;

    // hwx 动作编辑模块操作
    virtual void copyMotionFrame() {} // 复制动作帧
    virtual void pasteMotionFrame() {} // 粘贴动作帧
    virtual void deleteMotionFrame() {} // 删除动作帧
    virtual void modifyMotionFrame() {}  // 修改动作帧
    virtual void insertMotionFrame() {}  // 插入动作帧
    virtual void splitInsertMotionFrame() {} // 插入拆分动作帧
    virtual void addFrameToPreMotionBlock() {} // 向前一个动作组添加动作帧
    virtual void createFrameInNewBlock() {}  // 在新的动作组中新建动作帧
    virtual void mergeGroup(){}//合并选中动作组

    // 判断是否复制动作帧，音乐层的paste操作需要从动作层获取该信息
    virtual bool hasCopyFrame() { return false; }

    virtual QMimeData *getCopyFrameData() { return NULL; } // 获取拷贝的动作帧数据
};
bool CreateMotionLayOptionHandler(ILayerOptionHandler** ppHandler);
bool CreateEyesLayOptionHandler(ILayerOptionHandler** ppHandler);
bool CreateEarsLayOptionHandler(ILayerOptionHandler** ppHandler);
bool CreateMusicLayOptionHandler(ILayerOptionHandler** ppHandler);
#endif  //_ILAYEROPTIONHANDLER_H_
