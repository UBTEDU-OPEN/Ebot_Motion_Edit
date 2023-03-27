#ifndef ALPHAROBOTCONST_H
#define ALPHAROBOTCONST_H

#include <QString>
#include <QColor>

const QString ChineseHelpURL = "http://www.cn.ubtrobot.com/upload/download/2016103121164738.pdf";
const QString EnglishHelpURL = "http://www.en.ubtrobot.com/upload/download/alpha1/PC%20software%20user%20manual_V3.1.pdf";
//const QString ChineseHelpURL = "services.ubtrobot.com/ubx/system/introduction?systemLanguage=CN";
//const QString EnglishHelpURL = "services.ubtrobot.com/ubx/system/introduction?systemLanguage=EN";

//todo
const QString ChineseVideoTutorialURL = "services.ubtrobot.com/ubx/system/introduction?systemLanguage=CN";
const QString EnglishVideoTutorialURL = "services.ubtrobot.com/ubx/system/introduction?systemLanguage=EN";

const QString DEFAULT_FILE_NAME = "default";

const int DELAY_LOADED_TIME = 2000; // 延时加载时间

// 机器人动作列表start
const QString MYALPHA_QSS_PATH  = ":/res/qss/myalpha.qss";
const QString ALPHA_DEFAULT_QSS_PATH = ":/res/qss/alphadefault.qss";

const int MY_ALPHA_LIST_CHECKBOX_COL_WIDTH = 36;
const int MY_ALPHA_LIST_ACTION_NAME_COL_WIDTH = 480;
const int MY_ALPHA_LIST_OPERATION_COL_WIDTH = 64 + 16;

const QColor COLOR_ITEM_HOVER =  QColor("#5d5d5d");//动作列表Item Hover背景色
const QColor COLOR_ITEM_HOVER_CLEAR =  Qt::transparent;//清空hover

const int INT_INVALID_VALUE = -1;
const int MOUSE_MOVE_MIN_OFFSET = 10;  // 判断是否鼠标拖动操作，鼠标按下位置和释放位置偏移值大于该值则认为是鼠标拖动操作

const int MY_ALPHA_TABLE_COLUMN_NUMBER = 3;  // 我的机器人动作列表列数
//const int MY_ALPHA_TABLE_ID_COL = 0;                     // id列
const int MY_ALPHA_TABLE_CHECKBOX_COL = 0;  // checkebox列
const int MY_ALPHA_TABLE_ACTION_NAME_COL  = 1; // 动作名称列
const int MY_ALPHA_TABLE_OPERATE_COL = 2;  // 操作列
// 机器人动作列表end

const int BTN_DEFAULT_WITH = 18;
const int BTN_DEFAULT_HEIGHT = 17;

const int MAX_DELETE_ACTION_NUMBER = 10000;  // 删除动作最大数目

// stretch fator
const int STRETCH_FATOR_TREE_NAV = 3;  // 导航树
const int STRETCH_FATOR_ACTION_EDIT = 10; // 动作编辑
const int STRETCH_FATOR_3D_VIEW = 4;  // 3D视图

enum emLanguageType
{
    eLanguageChinese = 0, // 简体中文
    eLanguagenglish,            // 英文
    eLanguageHungary      //匈牙利文
};

// 动作编辑界面控件在水平布局中的下标
enum emEditPageIndex
{
    eEditPageIndexTreeNavi = 0,  // 导航树
    eEditPageIndexActionEdit, // 动作编辑
    eEditPageIndex3DView, // 3D视图模型
};

#endif // ALPHAROBOTCONST_H

