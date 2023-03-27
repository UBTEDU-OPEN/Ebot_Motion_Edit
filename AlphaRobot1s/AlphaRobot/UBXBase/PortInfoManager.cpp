#include "stable.h"
#include "PortInfoManager.h"
#include <QObject>

//**************!!!!! ci mu kuai jing guo da gai ***********//
CPortInfoManager::CPortInfoManager(void)
{
//    m_InputList.removeAll();
    for (int i=0; i<m_InputList.count(); i++)
    {
        CPortInfo* pInfo = m_InputList.at(i);
        if (pInfo == NULL)
            continue;
        m_InputList.removeAll(pInfo);

    }
}


CPortInfoManager::~CPortInfoManager(void)
{
    ClearPortInotList();
}

/************************************************************************/
/* [功	能] 获取没有用过的ID                                                                     */
/************************************************************************/
int CPortInfoManager::GetUnUsePortID()
{
    int nUnUseID = 0;

    if (m_InputList.count() == 0)
        return nUnUseID;

    while (1)
    {
        bool bExit = true;
        for (int i=0; i<m_InputList.count(); i++)
        {
            CPortInfo* pInfo = m_InputList.at(i);
            if (pInfo == NULL)
                continue;

            if (pInfo->GetIndex() == nUnUseID)
            {
                nUnUseID++;
                bExit = false;
                break;
            }
        }

        if (bExit)
            break;
    }

    return nUnUseID;
}

/************************************************************************
* [功	能]	添加PortInfo到列表
* [输	入]	pInfo : PortInfo
            bChangeIndex - true 重置PortInfo的Index
* [输	出]	无
************************************************************************/
bool CPortInfoManager::AddPortInfoToList(CPortInfo* pInfo, bool bChangeIndex /* = false */)
{
    if (pInfo == NULL)
        return false;

    if (bChangeIndex)
    {
        int nIndex = GetUnUsePortID();
        pInfo->SetIndex(nIndex);
    }

    m_InputList.append(pInfo);

    return true;
}

bool CPortInfoManager::DeletePortInfoFromList(QString caption)
{
    for (int i=0; i<m_InputList.count(); i++)
    {
        CPortInfo * info = m_InputList.at(i);
        QString strCaption = WCHARToQString(info->GetCaption(), MAX_PORTINFO_SIZE*sizeof(WCHAR));
        if (0 == QString::compare(strCaption, caption))
        {
            m_InputList.removeAt(m_InputList.indexOf(info));
            delete info;
            info = NULL;

            return true;
        }
    }
    return false;
}

bool CPortInfoManager::ModifyPortInfoFromList(QString caption, QString waitModifyStr)
{
    for (int i=0; i<m_InputList.count(); i++)
    {
        CPortInfo * info = m_InputList.at(i);
        QString strCaption = WCHARToQString(info->GetCaption(), MAX_PORTINFO_SIZE*sizeof(WCHAR));
        if (0 == QString::compare(strCaption, waitModifyStr))
        {
            WCHAR wstrDescription[MAX_PORTINFO_SIZE];
            memset(wstrDescription,0,MAX_PORTINFO_SIZE * sizeof(WCHAR));
            QStringToWCHAR(wstrDescription, strCaption);
            info->SetCaption(wstrDescription);
            return true;
        }
    }
    return false;
}

bool CPortInfoManager::DeletePortInfoByTyep(int nPortType)
{
    for (int i=0; i<m_InputList.count(); i++)
    {
        CPortInfo * info = m_InputList.at(i);
        if (info->GetPortType() == nPortType)
        {
            delete info;
            info =NULL;
            m_InputList.removeAt(m_InputList.indexOf(info));
            return true;
        }
    }

    return false;
}

bool CPortInfoManager::FindPortByCaption(QString caption)
{
    for (int i=0; i<m_InputList.count(); i++)
    {
        CPortInfo * info = m_InputList.at(i);
        QString strCaption = WCHARToQString(info->GetCaption(), MAX_PORTINFO_SIZE*sizeof(WCHAR));
        if (0 == QString::compare(strCaption, caption))
        {
            return true;
        }
    }
    return false;
}

/************************************************************************
* [功	能]清空列表
* [输	入]	pInfo : PortInfo
* [输	出]	无
************************************************************************/
void CPortInfoManager::ClearPortInotList(void)
{
//	POSITION	pos = m_InputList.GetHeadPosition();
//	CPortInfo*	pInfo = NULL;
//	while (pos)
//	{
//		pInfo = m_InputList.GetNext(pos);
//		if	(pInfo)
//		{
//			delete pInfo;
//		}
//	}
    while ( !m_InputList.isEmpty() )
    {
        delete m_InputList.takeFirst();
        m_InputList.removeFirst();
    }
}

/************************************************************************
* [功	能]获取PORT的个数
* [输	入]	无
* [输	出]	无
************************************************************************/
int CPortInfoManager::GetCount()
{
    return m_InputList.count();
}

/************************************************************************
* [功	能]获取CPortInfo
* [输	入]	无
* [输	出]	无
************************************************************************/
CPortInfo* CPortInfoManager::GetAt(int i)
{
    CPortInfo* p = NULL;

    p = m_InputList.at(i);

    return p;
}

/************************************************************************
* [功	能]	获取当前页的输入输出数据
* [输	入]	ppData - 保存的指针
            nLen - 保存的长度
* [输	出]	无
************************************************************************/
bool CPortInfoManager::GetRawData(char** ppData, int& nLen)
{
    vector<char> DataSave;

    // 数据总长度 + 管脚个数 + [第一个管脚的长度 + 管脚数据]
    int nDataLen = 0;

    // 保存数据长度
    int nPos = AddVectorData(&DataSave, (char*)&nDataLen, sizeof(nDataLen));

    // 管脚个数
    int nPortLen = GetCount();
    // 保存管脚个数
    nPos = AddVectorData(&DataSave, (char*)&nPortLen, sizeof(nPortLen));

    for (int i=0; i<nPortLen; i++)
    {
        char* pRawData = NULL;
        int nPortDataLen = 0;
        CPortInfo* pPortInfo = GetAt(i);
        if(pPortInfo ==NULL)
            return false;
        try
        {
            pPortInfo->GetRawData(&pRawData, nPortDataLen);
            if(pRawData)
            {
                // 管脚的数据长度
                AddVectorData(&DataSave, (char*)&nPortDataLen, sizeof(nPortDataLen));
                // 管脚的数据长度
                AddVectorData(&DataSave, pRawData, nPortDataLen);
                delete[] pRawData;
            }
        }
        catch(QString exception)
        {
            throw QObject::tr("exceptional")+exception;
        }


    }

    // 重写实际的总大小
    nLen = DataSave.size();
    ModifyVertorData(DataSave, 0, (char*)&nLen, sizeof(nLen));
    *ppData = new char[nLen];

    char* p = *ppData;
    if (p)
    {
        CopyVertorToPointer(DataSave, p);
    }

    DataSave.clear();

    return true;
}

/************************************************************************
* [功	能]	设置当前页的输入输出数据
* [输	入]	ppData - 保存的指针
            nLen - 保存的长度
* [输	出]	无
************************************************************************/
bool CPortInfoManager::SetRawData(char* pData, int nLen)
{
    ClearPortInotList();

    // 数据总长度 + 管脚个数 + [第一个管脚的长度 + 管脚数据]
    int nTotalLen = 0;
    UBX_MEMCPY_INC(nTotalLen, pData);
    if (nTotalLen != nLen)
        return false;

    // 管脚个数
    int nPortNum = 0;
    UBX_MEMCPY_INC(nPortNum, pData);

    for (int i=0; i<nPortNum; i++)
    {
        // 管脚数据长度
        int PortDataLen = 0;
        UBX_MEMCPY_INC(PortDataLen, pData);

        // 管脚数据
        char* pPortData = new char[PortDataLen];
        if (pPortData)
        {
            memcpy(pPortData, pData, PortDataLen);
            pData += PortDataLen;

            CPortInfo* pPortInfo = new CPortInfo();
            pPortInfo->SetRawData(pPortData, PortDataLen);
            AddPortInfoToList(pPortInfo);
            delete [] pPortData;
        }
    }

    return true;
}
