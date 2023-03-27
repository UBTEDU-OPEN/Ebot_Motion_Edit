/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CConfigs.h
* 创建时间：2016/07/15
* 文件标识：
* 文件摘要：参数设置
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：2016/07/15
* 版本摘要：
*/

#ifndef CCONFIGS_H
#define CCONFIGS_H



#include <QString>


#include "../common/UBXPublic.h"





//!!! app data dir
const QString ACT_LIB_DIR_NAME = "actlib";
const QString CONFIG_DIR_NAME = "Config";

const QString CUS_TEMPLATE_DIR_NAME = "CusTemplate";//!!!it is data dir
const QString DEFAULT_TEMPLATE_DIR_NAME = "defaultTemplate";//!!!this is app package install dir

const QString LOG_DIR_NAME = "Log"; // Log目录名称
const QString MUSIC_DIR_NAME = "Music";//Music目录名称
const QString PREVIEW_IMAGE_DIR_NAME = "PreviewImage";
const QString RECOMMEND_IMAGE_DIR_NAME = "RecommendImage";
const QString TEMP_DIR_NAME = "Temp";// Temp目录名称
const QString WAVE_DIR_NAME = "Wave";// Wave目录名称
const QString DETAILED_IMAGE_DIR_NAME = "DetailedImage"; // 动作详情图片


//!!! app package install dir
const QString LANGUAGES_DIR_NAME = "languages";
const QString RES_DIR_NAME = "res";
const QString RESOURCES_DIR_NAME = "Resources";
const QString ROBOT_DIR_NAME = "Robot";

// 机器人内存卡路径
const QString ROBOT_TFCARD_DIR_ACTION = "action";
const QString ROBOT_TFCARD_DIR_MUSIC = "music";
const QString ROBOT_TFCARD_DIR_MYDOWNLOAD = "my download";


//const QString LAYER_DIR_NAME = "Layer";//???


class ZXLIB_DLL CConfigs
{
public:
    ~CConfigs();

public:
    static CConfigs *instance();

public:
    //!!! all path is end without dir split char "/"

    static QString getFilePath(const QString& fileDir, const QString& fileName);

    //!app basic data dir
    static QString getAppBasicDataDir();
    //!app package install basic dir
    static QString getAppBasicInstallDir();


    //!app data path
    static QString getLocalActLibDir();
    static QString getLocalActLibPath(const QString& fileBasicName);

    //this is for user configs
    static QString getLocalUserConfigDir();
    static QString getLocalUserConfigPath(const QString& fileBasicName);

    static QString getLocalCusTemplateDir();
    static QString getLocalDefaultTemplateDir();

    static QString getLocalLogDir();
    static QString getLocalLogFilePath();

    static QString getLocalMusicDir();
    static QString getLocalMusicPath(const QString& fileBasicName);

    static QString getLocalPreviewImgDir();
    static QString getLocalPreviewImgPath(const QString& fileBasicName);

    // 动作详情页预览图片
    static QString getLocalDetailedImageDir();
    static QString getLocalDetailedImagePath(const QString& fileBasicName);

    static QString getLocalRecommendImgDir();
    static QString getLocalRecommendImgPath(const QString& fileBasicName);

    static QString getLocalTempDir();
    static QString getLocalTempPath(const QString& fileBasicName);

    static QString getLocalWaveDir();
    static QString getLocalWavePath(const QString& fileBasicName);

    static QString getLocalDownloadDir();
    static QString getLocalDownloadPath(const QString& fileName);


    //!app package install path
    static QString getLocalLanguageDir();

    static QString getLocalResDir();
    static QString getLocalResPath(const QString& fileBasicName);

    static QString getLocalResourcesDir();
    static QString getLocalResourcesPath(const QString& fileBasicName);

    static QString getLocalRobotDir();
    static QString getLocalRobotPath(const QString& fileBasicName);


    //some files path
    //动作表配置文件路径
    static QString getIniConfigFile();


    static void createDirsForApp();

    //===================================
    // 获取机器人内存卡内部路径
    static QString getRobotDirAction();     // 获取机器人TF卡内部路径
    static QString getRobotDirMusic();  // 获取机器人TF卡音乐路径
    static QString getRobotDirMyDownload(); // 获取机器人TF卡
    //====================================


private:
    explicit CConfigs();

};

#endif // CCONFIGS_H





