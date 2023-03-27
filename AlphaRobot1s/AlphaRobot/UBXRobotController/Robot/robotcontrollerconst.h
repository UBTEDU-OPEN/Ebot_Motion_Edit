#ifndef ROBOTCONTROLLERCONST_H
#define ROBOTCONTROLLERCONST_H

#include "globaldef.h"

const int PUSH_DATA_TIME_OUT = 5 * 1000; // 数据入队超时时间

//检查更新类型
enum CheckUpdateType
{
    CheckUpdateTypePC,        //更新pc端软件
    CheckUpdateTypeOldRobot1S,//更新旧版1S机器人端软件
    CheckUpdateTypeNewRobot1S,//更新新版1S机器人端软件
    CheckUpdateTypeRobot1P,   //更新1P机器人端软件
    CheckUpdateTypeNewRobot1SV2,//更新新版1SV2.0机器人端软件
};

//端口类型
enum emPortType
{
        ePortTypeVCP,  //虚拟串口
        ePortTypeHID,  //HID
};

typedef struct DEVIDTYPE
{
        int   nType;   //端口类型
        DWORD dwVid;   //vid
        DWORD dwPid;   //pid
        DEVIDTYPE(int type, DWORD vid, DWORD pid)
        {
                nType = type;
                dwVid = vid;
                dwPid = pid;
        }
        STRUCT_INIT(DEVIDTYPE)
}DEVIDTYPE;


//舵机指令类型
enum emRobotCmdType
{
        eRobotCmdTypeNone,
        eRobotCmdTypeTransFilePrepare,//准备传输文件
        eRobotCmdTypeTransFileIng,    //正在传输文件
        eRobotCmdTypeTransFileEnd,    //结束传输文件
        eRobotCmdTypeTransFileCancel,  // 取消文件传输

        eRobotCmdTypeGetOffset,       //获取偏移值
        eRobotCmdTypeSetOffset,       //设置偏移值
        eRobotCmdTypeGetAngle,        //获取角度
        eRobotCmdTypeMove,            //运动到指定角度
        eRobotCmdTypeGetOneAngle,     //获取单个舵机角度
        eRobotCmdTypeGetID,           //获取ID
        eRobotCmdTypeSetID,           //设置ID
        eRobotCmdTypeGetMotorVersion, //获取舵机版本
        eRobotCmdTypeUpgradePrepare,  //升级准备
        eRobotCmdTypeUpgradeIng,      //正在升级
        eRobotCmdTypeUpgradeEnd,      //结束升级

        eRobotCmdTypeConnectRobot,    //连接机器人
        eRobotCmdTypeDisconnectRobot, //断开机器人

        eRobotCmdTypeDebug,           //仿真机器人
        eRobotCmdTypeSwitchMode,      //切换模式

        eRobotCmdTypeReset,           //机器人复位
        eRobotCmdTypeReadSN,          //读序列号
        eRobotCmdTypeWriteSN,         //写序列号

        eRobotCmdTypeReadUID,         //读UID
        eRobotCmdTypeCheckUtf8,       //检查是否支持UTF8
        eRobotCmdTypeGetRobotSysEmbeddedVersion, //获取机器人嵌入式软件版本
        eRobotCmdTypeGetRobotHardwareVer,  // 获取机器人硬件类型，如Alpha1S

        eRobotCmdTypeGetBatteryStatus //
};

//切换模式
enum emRobotSwitchMode
{
        eRobotSwitchModeUdisk = 1,
        eRobotSwitchModeVCP,
        eRobotSwitchModeHID
};

//发送数据结构体
typedef struct SENDDATAOBJ
{
        emTranFileType m_eType;  // 文件传输的类型
        char    m_szSrcPath[255];  //源文件路径
        char    m_szTransFilePath[255];  //传输文件路径
        char    m_szDestPath[255]; //目标文件路径
        MYDATA   m_senddata;	    //要发送的文件数据
        int      m_nCurFrame;		//当前帧
        int      m_nCurSendSize;	//当前发送数据长度
        QString  m_sGuid;           //guid

        //重置
        void Reset()
        {      m_eType = emTranFileSynAction;
                m_senddata.Release();
                m_nCurFrame = 1;
                m_nCurSendSize = 0;

                //memset(m_szSrcPath, 0, sizeof(m_szSrcPath));  //源文件路径
                //memset(m_szTransFilePath, 0, sizeof(m_szTransFilePath));  //传输文件路径
                //memset(m_szDestPath, 0, sizeof(m_szDestPath)); //目标文件路径

                //m_sGuid = NULLSTR;
        }
}SENDDATAOBJ;

//传输文件的信息
 typedef struct TRANSFILEINFO
 {
        char      sSrcFilePath[255];    //源文件路径
        char      sTransFilePath[255];  //传输文件路径
        char      sDestFilePath[255];   //目标文件路径
        ULONGLONG  lFileSize;            //文件大小
        ULONGLONG  lSendedSize;          //已发送文件大小
        qint64      lTransTimeBegin;      //开始传输时间
        char      sGuid[255];           //guid

        STRUCT_INIT(TRANSFILEINFO)
 }TRANSFILEINFO;

typedef struct TRANSFILEINFOLIST
{
        vector<TRANSFILEINFO> vecTransFileInfo;        //传输文件的信息列表
        int   nCurIndex;   //当前文件索引

        TRANSFILEINFOLIST()
        {
                nCurIndex = 0;
        }

        /**************************************************************************
        * 函数名: Reset
        * 功能: 重置
        * 时间: 2015/11/17 20:07
        * 作者: ZDJ
        */
        void Reset()
        {
                vecTransFileInfo.clear();
                nCurIndex = 0;
        }

        /**************************************************************************
        * 函数名: GetTotalSize
        * 功能: 获取计划发送文件大小之和
        * 时间: 2015/10/20 10:30
        * 作者: ZDJ
        */
        ULONGLONG GetTotalSize()
        {
                ULONGLONG lSize = 0;
                for(int i = 0; i < vecTransFileInfo.size(); i++)
                {
                        lSize += vecTransFileInfo[i].lFileSize;
                }
                return lSize;
        }

        /**************************************************************************
        * 函数名: GetSendedSize
        * 功能: 获取已经发送文件大小之和
        * 时间: 2015/10/20 10:30
        * 作者: ZDJ
        */
        ULONGLONG GetSendedSize()
        {
                ULONGLONG lSize = 0;
                for(int i = 0; i < vecTransFileInfo.size(); i++)
                {
                        lSize += vecTransFileInfo[i].lSendedSize;
                }
                return lSize;
        }

        /**************************************************************************
        * 函数名: FindData
        * 功能: 查找源文件对应文件信息
        * 参数:
        *    @[in ] pszSrcFilePath: 源文件路径
        * 返回值: 成功返回文件信息，失败返回NULL
        * 时间: 2015/10/20 10:42
        * 作者: ZDJ
        */
        TRANSFILEINFO* FindData(const char* pszSrcFilePath)
        {
        int nIndex = FindIndex(pszSrcFilePath);
                if(nIndex != -1)
                        return &vecTransFileInfo[nIndex];

                return NULL;
        }

        /**************************************************************************
        * 函数名: FindData
        * 功能: 查找源文件对应文件信息
        * 参数:
        *    @[in ] pszGuid: 源文件路径
        * 返回值: 成功返回文件信息，失败返回NULL
        * 时间: 2015/10/20 10:42
        * 作者: ZDJ
        */
        TRANSFILEINFO* FindDataByGuid(const char* pszGuid)
        {
        int nIndex = FindIndexByGuid(pszGuid);
                if(nIndex != -1)
                        return &vecTransFileInfo[nIndex];

                return NULL;
        }

        /**************************************************************************
        * 函数名: FindIndex
        * 功能: 查找源文件对应列表索引
        * 参数:
        *    @[in ] pszSrcFilePath: 源文件路径
        * 返回值: 成功返回列表索引，失败返回-1
        * 时间: 2015/10/20 10:42
        * 作者: ZDJ
        */
        int FindIndex(const char* pszSrcFilePath)
        {
        const char* pszTemp = pszSrcFilePath;
                for(int i = 0; i < vecTransFileInfo.size(); i++)
                {
            if(strcmp(pszTemp, vecTransFileInfo[i].sSrcFilePath) == 0)
                                return i;
                }
                return -1;
        }

        /**************************************************************************
        * 函数名: FindIndexByGuid
        * 功能: 查找源文件对应列表索引
        * 参数:
        *    @[in ] pszGuid: GUID值
        * 返回值: 成功返回列表索引，失败返回-1
        * 时间: 2015/10/20 10:42
        * 作者: ZDJ
        */
        int FindIndexByGuid(const char* pszGuid)
        {
        const char* pszTemp = pszGuid;
                for(int i = 0; i < vecTransFileInfo.size(); i++)
                {
            if(strcmp(pszTemp, vecTransFileInfo[i].sGuid) == 0)
                                return i;
                }
                return -1;
        }


}TRANSFILEINFOLIST;

#endif // ROBOTCONTROLLERCONST_H

