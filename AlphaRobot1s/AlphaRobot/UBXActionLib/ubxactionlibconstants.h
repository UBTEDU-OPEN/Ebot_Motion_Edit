#ifndef UBXACTIONLIBCONSTANTS
#define UBXACTIONLIBCONSTANTS
#include <Qt>
#include <QColor>
#include <QNetworkRequest>

namespace Constants
{
    //cover flow相关定义
    const float FLOW_WIDGET_HEIGHT_RATIO = 0.32; // Flow控件占父控件高度比例， Flow控件高度:Home动作预览 = 3 : 4
    const float HOME_PREVIEW_HEIGHT_RATIO = 0.61; // Flow控件高度:Home动作预览 = 3 : 4
    const float FLOW_IMAGE_HEIGHT_RATIO = 0.75; // 播放图片占Flow控件高度
    const float FLOW_IMAGE_WIDTH_RATIO = 0.25;   // 播放图片占flow控件宽度

    const int WINDOW_HEIGHT  = 325; //窗体高度
    const int COVER_FLOW_WIDGET_WIDTH = 454;//播放图片宽度
    const int COVER_FLOW_WIDGET_HEIGHT = 243;//播放的图片高度

    const int HOME_PREVIEW_MIN_HEIGHT = 64;  //  Home Preview动作预览最小高度
    const int HOME_PREVIEW_MAX_HEIGHT = 550; // Home Preview动作预览最大高度

    //down load item 相关定义
    const QColor COLOR_ITEM_HOVER =  QColor("#4b4b4b");//动作列表Item Hover背景色
    const QColor COLOR_ITEM_HOVER_CLEAR =  Qt::transparent;//清空hover

    const int ACTION_TABLE_PREVIEW_MIN_WIDTH = 390;

    const int TEXT_MORE_SPACE = 2; // 额外分配的空间，如果多分配空间，会显示...
    const int ICON_TEXT_MARGIN = 3; // 文本距离边界的距离
    const int CONTENT_SPACE = 8; //项间距
    const int PREVIEW_ICON_WIDTH = 60;//预览图宽度
    const int PREVIEW_ICON_HEIGHT = 60;//预览图高度
    const int DESCRIPTION_DEFAULT_WIDTH = 230;//描述文字最大长度px
    const float DESCRIPION_COL_WIDTH_RATION = 0.5; // 描述文本列列占控件宽度比例
    const float DESCRIPTION_MAX_WIDTH_RATION = 0.5; // 描述文字占整列宽度的比例
    const int BTN_CHECOBOX_CONTAINER_WIDTH = 26;//包含checbox item宽度
    const int BTN_FAV_DOWN_CONTAINER_WIDTH = 65;//包含收藏和下载按钮的item宽度
    const int ICON_STARTS_WIDTH = 12;//收藏图标大小
    const int ICON_STARTS_HEGIHT = 10;
    const int ICON_DOWNLOADS_WIDTH = 12;//下载量图标大小
    const int ICON_DOWNLOADS_HEGIHT = 10;
    const int ICON_COMMENTS_WIDTH = 12;//评论量图标大小
    const int ICON_COMMENTS_HEGIHT = 10;

    // 活动推广
    const int RANK_ICON_WIDTH = 30; // 排行榜图标宽度
    const int RANK_ICON_HEIGHT = 30; // 排行不图标高度
    const int RANK_USER_PICTURE_WIDTH = 50;  // 排行榜用户头像宽度
    const int RANK_USER_PICTURE_HEIGHT = 50; // 排行榜用户头像高度
    const int RANK_USER_NAME_WIDTH = 160; // 排行榜动作名称宽度
    const int RANK_USER_NAME_HEIGHT = 30; // 排行榜动作名称高度
    const int RANK_ICON_ACTION_TYPE_WIDTH = 20; // 排行榜动作类型图标宽度
    const int RANK_ICON_ACTION_TYPE_HEIGHT = 20; // 排行榜动作图标高度
    const int RANK_ACTION_NAME_WIDTH = 140; // 排行榜动作名称宽度
    const int RANK_ACTION_NAME_HEIGHT = 20; // 排行榜动作名称高度
    const int RANK_ICON_HOT_WIDTH = 16;  // 排行热度图标宽度
    const int RANK_ICON_HOT_HEIGHT= 24; // 排行热度图标高度
    const int RANK_HOT_VALUE_WIDTH = 120; // 热度排行值宽度
    const int RANK_HOT_VALUE_HEIGHT = 60; // 热度排行值高度
    const int RANK_TABLE_ROW_PADDING_LEFT = 25; // 表格中各行内容与左边界距离
    const int RANK_TABLE_ROW_PADDING_CENTER = 100; // 表格中各行内容与右边界距离
    const int RANK_TABLE_ROW_PADDING_RIGHT = 25; // 表格中各行内容与右边界距离
    const int RANK_TABLE_GRID_LINE_WIDTH = 1; // 表格行分割线宽度
    const double RANK_TABLE_PADDIN_LEFT_RATIO = 0.2; // 左边空白区域占比
    const double RANK_TABLE_PADDING_CENTER_RATIO = 0.6; // 中间空白区域占比
    const double RANK_TABLE_PADDING_RIGHT_RATIO = 0.2; // 右边空白区域占比
    const int RANK_ACTION_AREA_WIDTH =   RANK_ICON_WIDTH + RANK_USER_PICTURE_WIDTH + RANK_USER_NAME_WIDTH +  2 * CONTENT_SPACE; // 动作区域宽度
    const int RANK_HOT_AREA_WIDTH = RANK_ICON_HOT_WIDTH + RANK_HOT_VALUE_WIDTH + CONTENT_SPACE ; // 热度区域宽度
    const int RANK_TABLE_COL_HEIGHT = RANK_USER_PICTURE_HEIGHT + 2 * CONTENT_SPACE + RANK_TABLE_GRID_LINE_WIDTH; // 排行榜表格列高
    const int RANK_TABLE_ROW_WIDTH = RANK_TABLE_ROW_PADDING_LEFT + RANK_ACTION_AREA_WIDTH + RANK_HOT_AREA_WIDTH
                                                                            + 3 * CONTENT_SPACE + RANK_TABLE_ROW_PADDING_RIGHT+ RANK_TABLE_ROW_PADDING_CENTER; // 排行榜表格行宽
    const int RANK_TABLE_ROW_WIDTH_NOPADDINT =  RANK_ACTION_AREA_WIDTH + RANK_HOT_AREA_WIDTH
                                                                            + 3 * CONTENT_SPACE; // 排行榜表格行宽，除去空白区

    const int RANK_LIST_MAX_NUM = 10;  // 排行榜最多显示个数

    // 关闭按钮大小设置
    const int CLOSE_BUTTON_WIDTH = 12;
    const int CLOSE_BUTTON_HEIGHT = 12;
    const int CLOSE_BUTTON_BORDER_OFFSET = 3;  // 关闭按钮距离边界距离

    const int TIME_INTERVAL_CHANGED_FLOW_COVER_IMAGE = 3000;  // flowcover图片切换的时间间隔设置，默认为3s

    const int ICON_ACTION_UPLOAD_COVERPIC_WIDTH = 84; //动作封面图片宽度
    const int ICON_ACTION_UPLOAD_COVERPIC_HEIGHT = 84;//动作封面图片高度
    const int ICON_ACTION_UPLOAD_DESCPIC_WIDTH = 84;  //动作描述图片宽度
    const int ICON_ACTION_UPLOAD_DESCPIC_HEIGHT = 84; //动作描述图片高度

    const char* const CUSTOM_DOWNLOAD_ITEM_TYPE = "CustomDownLoadItem";//描述下载项信息的类型
    //const char* const CUSTOM_CHECK_ITEM_TYPE = "CustomCheckItem";//checkbox类型（注意不是QCheckbox控件，而是绘制的checkbox）

    //动作列表界面 常量定义
    const int ACTION_LIST_COUNT_PER_LIST = 8;//8;//动作列表界面每列控件显示个数
    const int ACTION_LIST_PER_PAGE_LIST_COUNT = 3;//list count of page
    const int ACTION_LIST_PAGE_SIZE = ACTION_LIST_COUNT_PER_LIST * ACTION_LIST_PER_PAGE_LIST_COUNT;//24动作列表界面，每页显示下载项的总数

    // 推荐动作页面
    const int RECOMMEND_PAGE_ACTION_COUNT_PER_LIST = 5; // 推荐动作列表每张表格的动作数目

    // 动作详情页猜你喜欢推荐的动作数目
    const int ACTION_YOU_MAY_LIKE_NUMBER = 8;
    const int MAX_ACT_DESC_LEN = 350;

    // 播放器和猜你喜欢宽度比：5:2
    const float ACTION_YOU_MAY_LIKE_WIDTH_WIDTH_RATIO = 2.0/7.0;

    //动作库首页
#ifdef Q_OS_WIN
    const int ACTION_PREVIEW_PAGE_SIZE = 6;
#else
    const int ACTION_PREVIEW_PAGE_SIZE = 5;
#endif
    //下载项角色定义
    enum DownLoadItemDataRole
    {
        DownLoadItemType = Qt::UserRole + 1,//用于给Delegate区分是自定义类型，还是Qt内建类型
        ActionItemID,//动作项ID
        ActionItemData,//动作项数据
        RankItemData, // 排名项数据
        //以下的定义先暂时不用
//        PreviewIcon, //预览图
//        Title,       //标题文字
//        Description,    //描述文字
//        DownLoadCount,  //下载总数
//        Starts,         //收藏总数
//        Comments,    //评论量
//        //先注释，改为使用rcc中的默认图片，如有必要动态获取，在开启
//        //DownLoadCountIcon,  //下载总数icon
//        //StartsIcon,     //收藏总数icon
//        CreationDate,   //创建日期
//        Creator,    //创建者
//        DowLoadPath     //下载项路径
    };

    //Comment Item 相关定义
    const int COMMENT_TEXT_MARGIN = 3;
    const int COMMENT_ITEM_SPACE = 10;//间距
    const int COMMENT_HEAD_ICON_WIDTH = 38;// 头像大小
    const int COMMENT_HEAD_ICON_HEIGHT = 38;
    const int COMMENT_REPLY_ICON_WIDTH = 16; // 回复图标
    const int COMMENT_REPLY_ICON_HEIGHT = 14;

    const int COMMENT_TREEVIEW_MARGIN = 120; // 评论数距离边界距离

    const double COMMENT_WIDGET_HEIGHT_RATIO = 1.0; // 评论控件占父控件比例
    const double PLAYER_WIDGET_HEIGHT_RATIO = 0.60; // 播放器控件占父控件比例

    const int COMMIT_COMMENT_TIMEOUT = 5000; // 提交评论超时

    // HTTP请求网络评论类型
    const int HTTP_COMMENT_TYPE = 2;

    const char* const COMMENT_ITEM_TYPE = "CustomCommentItem";

    // 评论类型
    enum emCommentType
    {
        eTypeReplyHost = 1, // 回复楼主
        eTypeReplyComment, // 回复评论
    };

    //评论项角色定义
    enum CommentItemDataRole
    {
        CommentItemType = Qt::UserRole + 1,
        CommentItemData, // 评论数据
    };

    enum emPageType
    {
        ePageTypeUnknown,
        ePageTypeBanner = 3,  // 普通banner页
        ePageTypePush = 4,  // 活动推送
    };

    enum emHomePageType
    {
        eHomePageUnknown = 0,
        eHomePageLatest,  // 最新
        eHomePageHotest, // 最热
        eHomePageComment, // 评论最多
    };

    // 活动推广页下载图片类型
    enum emActivityImageType
    {
        eTypeCoverImage, // 封面图片
        eTypeDetailedImage, // 详情图片
    };

    // 位置信息
    enum emDirection
    {
        eLeft = 0,
        eRight,
        eCenter
    };

    const char* const PROPERTY_PAGE_REAL_NUMBER = "PropertyPageRealNumber";//页码按钮对应的真实页码值属性
//    const char* const PROPERTY_ITEM_ROW = "PropertyItemRow";//table项的行数属性
//    const char* const PROPERTY_ITEM_COL = "PropertyItemCol";//table项的列数

    //http transfer 常量定义 begin
    const char* const HTTP_REQUEST_ACTION_LIST = "HttpRequestActionList";//请求动作列表
    const char* const HTTP_REQUEST_FAVORITES = "HttpReqeustFavorites";//请求收藏列表
    const char* const HTTP_REQUEST_COLLECT_ACTION = "HttpReqeustCollectAction";//收藏动作
    const char* const HTTP_REQUEST_CANCEL_COLLECT = "HttpReqeustCancelCollect";//取消收藏
    const char* const HTTP_REQUEST_IS_COLLECTED = "HttpReqeustIsCollected";//是否已经收藏
    const char* const HTTP_REQUEST_ACTION_DETAIL = "HttpRequestActionDetail";//查询动作详情
    const char* const HTTP_REQUEST_PREVIEW_IMAGE = "HttpRequestPreviewImage";//下载预览图
    const char* const HTTP_REQUEST_USER_IMAGE = "HttpRequestUserImage";  // 下载用户头像
    const char* const HTTP_REQUEST_PROMOTE_IMAGE = "HttpRequestPromoteImage"; // 下载推广图片
    const char* const HTTP_REQUEST_ADD_DOWDLOAD_COUNT = "HttpRequestAddDownLoadCount";//下载预览图
    const char* const HTTP_REQUEST_COMMENT_REPLIER_ICON = "HttpRequestCommentReplierIcon"; // 下载评论用户图标
    const char* const HTTP_REQUEST_DOWNLOAD_IMAGE = "HttpRequestDownloadImage";  // 下载图片

    const char* const HTTP_REQUEST_UPLOADACTIONFILE = "HttpRequestUploadActionFile"; //上传动作文件
    const char* const HTTP_REQUEST_UPLOADACTIONINFO = "HttpRequestUploadActionInfo"; //上传动作信息
    const char* const HTTP_REQUEST_UPLOADACTIONHEADIMG = "HttpRequestUploadActionHeadImg"; //上传动作头像
    const char* const HTTP_REQUEST_UPLOADACTIONDESCIMG = "HttpRequestUploadActionDescImg"; //上传动作描述图片
    const char* const HTTP_REQUEST_UPLOADACTIONDESCVIDEO = "HttpRequestUploadActionDescVideo"; //上传动作描述视频
    const char* const HTTP_REQUEST_QUERY_CREATEACTIONS = "HttpRequestQueryCreateActions"; //请求查询新建动作列表
    const char* const HTTP_REQUEST_DELETE_CREATEACTIONS = "HttpRequestDeleteCreateActions"; //请求删除新建动作

    const char* const HTTP_REQUEST_RECOMMEND_LIST = "HttpRequestRecommendList"; // 请求主界面flow推荐的动作
    const char* const HTTP_REQUEST_RECOMMEND_IMAGE = "HttpRequestRecommendImage"; // 下载主界面flow推荐动作的图片
    const char * const HTTP_REQUEST_ACTION_COMMENT = "HttpRequestActionComment"; // 请求动作评论
    const char * const HTTP_ADD_ACTION_COMMENT= "HttpAddActionComment"; // 添加动作评论
    const char * const HTTP_PRAISE_ACTION = "HttpPraiseAction"; // 对动作点赞

    const char* const HTTP_REQUEST_BUSINESS_PROMOTE_DATA = "HttpRequestBusinessPromoteData"; // 请求运营活动推广数据
    const char* const HTTP_REQUEST_RANK_ACTION_DATA = "HttpRequestRankActionData"; // 请求排行榜动作数据

    const double WIDTH_PERCENT_IN_ACTIVITY_WIDGET = 0.8; // 活动推广界面中子控件占界面的宽度百分比


    enum HTTPAttributeCode
    {
        HTTPRequestID = QNetworkRequest::User + 1,//请求ID
        HTTPRequestType,//请求类型
        HTTPActionID,//Action ID
        HTTPRecommendID,  // 推荐动作的ID
        HTTPSchemeID,  // 运营活动ID
        HTTPTypeID, // 类型ID
        HTTPDownloadFilePath,  // 下载文件路径
    };

    enum HTTPRespCode
    {
        HTTPRespCodeOK = 0,  // 正确应答
        HTTPRespCodeError,  // 应答错误
        HTTPRespCodeIllegalWord = 4001,  // 包含敏感词汇
    };

    const int HTTP_RESULT_SUCESS = -2;
    const int HTTP_RESULT_FAIL = -1;

    const int HTTP_DEFAULT_REQUEST_ID = -1;  // 请求默认ID

    const int POPPU_WIDGET_DELAY_TIME = 2000; // 弹窗延时时间
    const int DELAY_TIME_100 = 100; // 延时时间100ms

    // 图标资源
    const QString ICON_MEDAL_1 = ":/res/images/icon_medal_1.png"; // 金牌图标
    const QString ICON_MEDAL_2 = ":/res/images/icon_medal_2.png"; // 银牌图标
    const QString ICON_MEDAL_3 = ":/res/images/icon_medal_3.png"; // 铜牌图标
    const QString ICON_HOT_1 = ":/res/images/icon_hot_1.png"; // 最热图标
    const QString ICON_HOT_2 = ":/res/images/icon_hot_2.png"; // 次热图标
    const QString ICON_HOT_3 = ":/res/images/icon_hot_3.png"; //
    const QString ICON_HOT_OTHER = ":/res/images/icon_hot_other.png";

    // 动作上传
    const int UPLOAD_DONE = 100; // 上传完成
    const int UPLOAD_ACTION_DESP_MAX_CHAR = 200; // 动作上传描述文本最大数目

    //http transfer 常量定义 end
}

#endif // UBXACTIONLIBCONSTANTS
