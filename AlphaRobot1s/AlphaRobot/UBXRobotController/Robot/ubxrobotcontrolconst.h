#ifndef UBXROBOTCONTROLCONST
#define UBXROBOTCONTROLCONST

const int TIMEOUT_1_MINUTES = 60 * 1000; //1分钟
const int TIMEOUT_15_MINUTES = 15 * TIMEOUT_1_MINUTES; // 15分钟
// 发送数据队列容量，如果配置过小，当点击
// 3D模型舵机角度设置按钮+/-不放时，会出现队列满的情况
const int CAPACITY_SEND_QUEUE = 360;

const int TIMEOUT_5000 = 5000;  // 5000ms
const int TIMEOUT_1000 = 1000;  // 1000ms
const int TIMEOUT_500 = 500;    // 500ms
const int TIMEOUT_300 = 300;    // 300ms

const int TIMEOUT_CMD_TRANSFILE_PREPARE = TIMEOUT_5000;  // 准备传输文件命令超时时间
const int TIMEOUT_CMD_TRANSFILE_ING = TIMEOUT_5000;   // 正在传输文件命令超时时间
const int TIMEOUT_CMD_TRANSFILE_END = TIMEOUT_5000; // 文件传输结束命令超时时间

const int TIMIEOUT_CMD_TRANSFILE_CANCEL = TIMEOUT_500; // 取消文件传输命令超时时间

const int TIMEOUT_CMD_GET_OFFSET = TIMEOUT_300; // 获取偏移值超时时间
const int TIMEOUT_CMD_SET_OFFSET = TIMEOUT_300; // 设置偏移值超时时间
const int TIMEOUT_CMD_GET_ANGLE = TIMEOUT_500; // 获取舵机角度超时时间
const int TIMEOUT_CMD_MOVE = TIMEOUT_500; // 舵机运动到指定角度超时时间
const int TIMEOUT_CMD_GET_ONE_ANGLE = TIMEOUT_500; // 获取单个舵机角度超时时间
const int TIMEOUT_CMD_GET_ID = TIMEOUT_300; // 获取机器人ID超时时间
const int TIMEOUT_CMD_SET_ID = TIMEOUT_300; // 设置机器人ID超时时间

const int TIMEOUT_CMD_GET_MOTOR_VERSION = TIMEOUT_300; // 获取舵机版本超时时间

const int TIMEOUT_CMD_UPDATE_PREPARE = TIMEOUT_1000;  // 舵机升级准备命令超时时间
const int TIMEOUT_CMD_UPDATE_ING = TIMEOUT_1000; // 舵机升级命令超时时间
const int TIMEOUT_CMD_UPDATE_END = TIMEOUT_1000; // 舵机升级结束命令超时时间

const int TIMEOUT_CMD_CONNECT_ROBOT = TIMEOUT_1000; // 连接机器人命令超时时间
const int TIMEOUT_CMD_DISCONNECT_ROBOT = TIMEOUT_1000; // 断开机器人连接命令超时时间

const int TIMEOUT_CMD_DEBUG = TIMEOUT_500; // 仿真机器人命令超时时间
const int TIMEOUT_CMD_SWITCH_MODE = TIMEOUT_5000; // 切换U盘模式超时时间

const int TIMEOUT_CMD_RESET = TIMEOUT_300; // 机器人复位命令超时时间
const int TIMEOUT_CMD_READ_SN = TIMEOUT_300; // 读取机器人序列号命令超时时间
const int TIMEOUT_CMD_WRITE_SN = TIMEOUT_300; // 写机器人序列号命令超时时间

const int TIMEOUT_CMD_READ_UID = TIMEOUT_300; // 读取机器人UID命令超时时间
const int TIMEOUT_CMD_CHECK_UTF8 = TIMEOUT_300; // 检查是否支持UTF8命令超时时间

const int TIMEOUT_CMD_GET_SYS_EMBEDED_VERSION = TIMEOUT_1000; // 读取机器人嵌入式版本命令超时时间
const int TIMEOUT_CMD_GET_HARDWARE_VERSION = TIMEOUT_1000;  // 读取机器人硬件类型命令超时时间

const int TIMEOUT_CMD_GET_BATTERY_STATUS = TIMEOUT_300; // 获取机器人电池信息命令超时时间
const int TIMEOUT_CMD_GET_DEVICE_STATUS = TIMEOUT_300; // 获取机器人设备信息命令超时时间

const int TIMEOUT_CMD_GET_ROBOT_RES_INFOS = TIMEOUT_1_MINUTES; // 获取RES INFO
const int TIMEOUT_CMD_GET_ROBOT_ACTION_LIST = TIMEOUT_15_MINUTES; // 获取机器人动作列表
const int TIMEOUT_CMD_DELETE_ROBOT_ACTION = TIMEOUT_1000;  // 删除机器人动作


const int TIMEOUT_CMD_GET_ROBOT_WIFIINFO_LIST = TIMEOUT_1_MINUTES; // 获取机器人WIFI INFO 列表
const int TIMEOUT_CMD_ROBOT_CONNECT_WIFII = TIMEOUT_1_MINUTES; // 机器人connect to WIFI
const int TIMEOUT_CMD_ROBOT_MANAL_UPGRADE = TIMEOUT_1_MINUTES; // 机器人upgrade cmd


const int PUSH_DATA_TIME_OUT = 60 * 1000; // 数据入队超时时间
const int WAIT_SERIAL_PORT_IDLE_TIME = 100; // 等待串口空闲时间
const int ROBOT_CMD_RESEND_CNT = 1; // 机器人指令超时重发的次数

const int DEFAULT_TRANS_FILE_FRAME_SIZE = 58; // 默认传输数据帧长度
const int DEFAULT_UPDATE_FRAME_SIZE = 64; // 默认升级数据帧长度

const int MIN_MOTOR_ID = 1;  // 舵机ID最小值
const int MAX_MOTOR_ID = 240; // 舵机ID最大值

const QString STM_DEVICE_NAME_NAME1 = "STMicroelectronics Virtual COM Port";//要查找的STM设备名称字符串
const QString STM_DEVICE_NAME_NAME2 = "Prolific USB-to-Serial Comm Port";//要查找的STM设备名称字符串

//舵机指令类型
enum emRobotCmdType
{
    eRobotCmdTypeUnkonwn,
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

    eRobotCmdTypeGetBatteryStatus,  // 获取机器人电池信息
    eRobotCmdTypeGetDeviceStatus,    // 获取机器人设备信息

    eRobotCmdTypeGetActionList,   // 获取机器人动作列表
    eRobotCmdTypeGetSomeResInfos, //pull and push modle
    eRobotCmdTypeDeleteAction,    // 删除机器人动作



    /////////////////////////////////////////////
    ////   new bluetooth protocol cmd
    eRobotCmdTypeWifiScan, //get the wifi scan list
    eRobotCmdTypeConnectWifi,//connect to the wifi
    eRobotCmdTypePlayAct, //play action file
    eRbootCmdTypeDeleteAction,    // 删除机器人动作

    eRobotCmdTypeSetTime, // 时间设置

    eRobotCmdTypeAutoUpGradeState,  //读取自动升级状态
    eRobotCmdTypeSetAutoUpGradeState,   //切换自动升级状态
    eRobotCmdTypeManalUpGrade,           //手动升级
    eRobotCmdTypeGetAutoUpdadeProgress,  //获取自动升级进度
    eRobotCmdTypeIsInstallingUpdate,       //是否进入安装升级


};

// 机器人命令优先级
enum emRobotCmdPriority
{
    eRobotCmdPriorityLow = 100,  // 低优先级
    eRobotCmdPriorityMiddle = 500, // 中等优先级
    eRobotCmdPriorityHigh = 1000, // 高优先级
};

// 删除机器人动作响应
enum emDeleteRobotActionRsp
{
    eDeleteRobotActionRspOk = 0,  // 成功删除
    eDeleteRobotActionRspFileNotExist,  // 文件不存在
    eDeleteRobotActionRspFailed,  // 删除失败
    eDeleteRobotActionRspActionIsPlaying, // 动作文件正在播放
    eDeleteRobotActionRspNullFile, // 空文件名
    eDeleteRobotActionRspTimeout, // 超时
};

//检查更新类型
enum CheckUpdateType
{
    CheckUpdateTypePC,        //更新pc端软件
    CheckUpdateTypeOldRobot1S,//更新旧版1S机器人端软件
    CheckUpdateTypeNewRobot1S,//更新新版1S机器人端软件
    CheckUpdateTypeRobot1P,   //更新1P机器人端软件
    CheckUpdateTypeNewRobot1SV2,//更新新版1SV2.0机器人端软件
};

enum emTimeType
{
    eTimeTypeUnkown = 0,
    eTimeTypeYear,  // 年
    eTimeTypeMonth, // 月
    eTimeTypeDay, // 日
    eTimeTypeHour, // 时
    eTimeTypeMinute, // 分
    eTimeTypeSecond, // 秒
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

extern UBXROBOTCONTROLLER_API DEVIDTYPE    g_devidarr[];

#define TIMEDELAY_CONNECTROBOT     1500   //连接机器人延时 2000
#define TIMEDELAY_DISCONNECTROBOT  200   //连接机器人延时

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

#endif // UBXROBOTCONTROLCONST

