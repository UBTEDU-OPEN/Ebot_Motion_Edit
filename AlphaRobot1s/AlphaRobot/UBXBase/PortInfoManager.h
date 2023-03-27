#ifndef _PORTINFOMANAGER_H_
#define _PORTINFOMANAGER_H_

//#include <afxtempl.h>
#include "PortInfo.h"
#include <stable.h>
#include <QList>

class CPortInfoManager:public CRawData
{
public:
    CPortInfoManager(void);
     ~CPortInfoManager(void);

protected:
//    QList<CPortInfo*, CPortInfo*> m_InputList;
    QList<CPortInfo *> m_InputList;

public:
    int GetUnUsePortID();

public:
    // 添加PortInfo到列表
    bool AddPortInfoToList(CPortInfo* pInfo, bool bChangeIndex = false);
    bool DeletePortInfoFromList(QString caption);
    bool ModifyPortInfoFromList(QString caption, QString waitModifyStr);
    bool DeletePortInfoByTyep(int nPortType);
    bool FindPortByCaption(QString caption);
    // 清除列表
    void ClearPortInotList(void);

    // 获取PORT的个数
    int GetCount();
    // 获取CPortInfo
    CPortInfo* GetAt(int i);

    // CRawData 接口实现
    virtual bool GetRawData(char** ppData, int &nLen);
    virtual bool SetRawData(char* pData, int nLen);
};
#endif  //_PORTINFOMANAGER_H_
