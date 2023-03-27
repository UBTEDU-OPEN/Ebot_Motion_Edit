#ifndef _ACTIONBLOCKDATAMANAGER_H_
#define _ACTIONBLOCKDATAMANAGER_H_

#include "ubxundoredomanager_global.h"
#include "ActionBlockGroupManager.h"
#include "stable.h"

class CompareClass
{
public:
    //分组按开始时刻升序排列
    bool operator () (CActionBlockGroupManager* pBlock1, CActionBlockGroupManager* pBlock2)
    {
        float fTimeSpan = pBlock1->GetStartTime() - pBlock2->GetStartTime();
        if(fTimeSpan == 0)
            return pBlock1->GetID() < pBlock2->GetID();
        else
            return fTimeSpan < 0 ? true : false;
    }
};
typedef set<CActionBlockGroupManager*, CompareClass> SetActionBlockDataType;


class UBXUNDOREDOMANAGER_API CActionBlockDataManager
{
public:
    CActionBlockDataManager(void);
     ~CActionBlockDataManager(void);

public:
    virtual bool Connect(void);
    virtual void ReleaseConnection();
    virtual bool GetData(char** ppData, int &nLen);
    virtual bool SetData(char* pData, int nLen);

    // 拷贝动作数据
    virtual void cloneData(CActionBlockDataManager *pBlockDataManager);

    // 删除指定 块 信息
    virtual bool DeleteLayerBlock(int nID);

    // 扩展指定块
    //virtual bool ExtrudeLayerBlock(int nID, int startTime, int endTime);
    // 增加块
    virtual bool AddLayerBlock(CActionBlockGroupManager* pLayer);
    // 修改块
    //virtual bool ModifyLayerBlockDescription(int nID, QString DescriptionInfo);

    virtual void ClearBlockDataList();

    //virtual bool OnSaveData(int nID);

    // 是否存在
    //bool IsExistBlockData(int nID);
    CActionBlockGroupManager* FindBlockData(int nID);
    CActionBlockGroupManager* GetFinalBlockData();

    bool GetAllBlockData(vector<CActionBlockGroupManager*> &blockData);

    int GetCount();
    CActionBlockGroupManager* GetAt(int nIndex);    
    int getIndex(const CActionBlockGroupManager *pGroupMgr);
    //find pActionBlockGroupManager next ActionBlockGroupManager
    CActionBlockGroupManager* getNextActionBlockGroupManager(const CActionBlockGroupManager* pActionBlockGroupManager);

    // 获取没有用过的ID
    int GetUnUserID();

    /**************************************************************************
    * 函数名: SetUnUseBaseID
    * 功能:设置动作帧未使用ID基数
    * 时间: 2016/03/16 20:52
    * 作者: 周志平
    */
    void SetUnUseBaseID();

    // 获取没有用过的名
    QString GetUnUseDescription();    

    //bool AddLayerBlockNoSort(CActionBlockGroupManager* pLayer);
    int GetIndexByID(int nID);

    /**************************************************************************
    * 函数名: FirstGroupIndex
    * 功能:查找第一个包含给定fStartTime的索引
    * 时间: 2016/03/18
    * 作者: hels
    */
    SetActionBlockDataType::const_iterator FirstGroupIndex(float fStartTime);
    SetActionBlockDataType::const_iterator EndIndex();

    /**************************************************************************
    * 函数名: MoveGroup
    * 功能: 移动分组
    * 参数:
    *    @[in ] pBaseGroup: 起始分组
    *    @[in ] fTimeSpan: 移动的时间间隔，正值向右，负值向左
    * 返回值: 成功返回true，失败返回false
    * 时间: 2016/03/19 12:13
    * 作者: ZDJ
    */
    bool MoveGroup(CActionBlockGroupManager* pBaseGroup, float fTimeSpan);

    /**************************************************************************
    * 函数名: getSelectGroupFrameArrayIndex
    * 功能: 获取指定时间对应的动作组在数组中下标，动作帧在数组中下标，时间偏移
    * 参数:
    *    @[in ] nSelectTime: 选中的时间，CActionBlockGroupManager中对应的m_fStartTime等对应，都除以了时间粒度
    *    @[in ] nGroupIndex: 返回的选中的时间所在的动作组在数据结构中的index
    *    @[in ] nFrameIndex: 返回的选中的时间所在的动作组的动作帧在数据结构中的index
    *    @[in ] nTimeOffset: 返回选中的时间在动作帧中的时间偏移
    * 返回值:
    * 时间: 2016/12/08
    * 作者:hwx
    */
    void getSelectGroupFrameArrayIndex(int nSelectTime,int &nGroupIndex , int &nFrameIndex , int &nTimeOffset);

    /**************************************************************************
    * 函数名: getSelectGroupFrameID
    * 功能: 获取指定时间对应的动作组ID，动作帧ID，时间偏移
    * 参数:
    *    @[in ] nSelectTime: 选中的时间，CActionBlockGroupManager中对应的m_fStartTime等对应，都除以了时间粒度
    *    @[in ] nGroupID: 返回的选中的时间所在的动作组id
    *    @[in ] nFrameID: 返回的选中的时间所在的动作帧id
    *    @[in ] nTimeOffset: 返回选中的时间在动作帧中的时间偏移
    * 返回值:
    * 时间: 2016/12/08
    * 作者:hwx
    */
    void getSelectGroupFrameID(int nSelectTime,int &nGroupID , int &nFrameID , int &nTimeOffset);

    /**************************************************************************
    * 函数名: getSelectGroupFrame
    * 功能: 获取指定时间对应的动作组ID，动作帧ID，时间偏移
    * 参数:
    *    @[in ] nSelectTime: 选中的时间，CActionBlockGroupManager中对应的m_fStartTime等对应，都除以了时间粒度
    *    @[in ] fStartTime: 从指定的时间开始搜索（已经除以时间粒度）
    *    @[in ] nGroupIndex: 返回的选中的时间所在的动作组在数据结构中的index
    *    @[in ] nFrameIndex: 返回的选中的时间所在的动作组的动作帧在数据结构中的index
    *    @[in ] nTimeOffset: 返回选中的时间在动作帧中的时间偏移
    * 返回值:
    * 时间: 2016/12/08
    * 作者:hwx
    */
    void getSelectGroupFrameArrayIndex(int nSelectTime, float fStartTime, int &nGroupIndex , int &nFrameIndex , int &nTimeOffset);

    /**************************************************************************
    * 函数名: getActionFrameStartTime
    * 功能:根据动作组ID和动作帧ID获取动作帧起始时间（粒度，非真实时间）
    * 参数:）
    *    @[in ] nGroupID: 动作组ID
    *    @[in ] nFrameID: 动作帧ID
    * 返回值:
    * 时间: 2017/03/25
    * 作者:hwx
    */
    float getActionFrameStartTime(int nGroupID, int nFrameID);

    /**************************************************************************
    * 函数名: createDefaultGroupMgr
    * 功能: 创建获取使用默认配置的动作组
    * 参数:
    * 返回值:
    * 时间: 2017/03/23
    * 作者:hwx
    */
    CActionBlockGroupManager *createDefaultGroupMgr();

    QList<int> getGroupIDSelected();
    int getGroupIDSelectedFirst();//选择中的第一个组ID
    void setGroupIDSelected(int nGroupIDSelect, bool bSelect);
    void clearAllGroupIDSelect();

private:
    // 保存所有创建的层的容器
    //QList<CActionBlockGroupManager*> m_ListActionBlockData;



    SetActionBlockDataType   m_setActionBlockData;  //分组集合

    int m_nUnUseBaseID;     //分组未使用ID基数；
};
#endif  //_ACTIONBLOCKDATAMANAGER_H_
