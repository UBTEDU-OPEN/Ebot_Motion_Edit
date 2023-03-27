#include "actitemdatahandle.h"



CActItemDataHandle::CActItemDataHandle()
{

}

CActItemDataHandle::~CActItemDataHandle()
{

}

//ActItemData to MapStringDataType
void CActItemDataHandle::ActItemDataToMapStrType(const ActItemData &actData,
                                                 MapStringDataType &mapParams)
{
    mapParams.clear();

    const QString strUndefindKey = "strUndefindNetActKey";

    //todo
    if(!actData.m_sGuid.isEmpty())
    {
        mapParams.insert(strUndefindKey, actData.m_sGuid.toUtf8());
    }

    // schemeId
    if (DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nSchemId)
    {
       mapParams.insert(NET_SCHEME_ID, QString::number(actData.m_nSchemId).toLatin1());
    }

    // schemeName
    if(!actData.m_strSchemeName.isEmpty())
    {
        mapParams.insert(NET_SCHEME_NAME, actData.m_strSchemeName.toUtf8());
    }

    // actionId
    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nItemID)
    {
        mapParams.insert(NET_ACTION_ID, QString::number(actData.m_nItemID).toLatin1());
    }

    // actionBrownTime
    if (DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nActionBrownTime)
    {
       mapParams.insert(NET_ACTION_BROWN_TIME, QString::number(actData.m_nActionBrownTime).toLatin1());
    }

    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nActionOriginalId)
    {
        mapParams.insert(NET_ACTION_ORIGINAL_ID, QString::number(actData.m_nActionOriginalId).toLatin1());
    }

    if(!actData.m_sImageHeadUrl.isEmpty())
    {
        mapParams.insert(NET_ACTION_HEADURL, actData.m_sImageHeadUrl.toUtf8());
    }

    if( !actData.m_sImageURL.isEmpty())
    {
        mapParams.insert(NET_ACTION_IMAGE_PATH, actData.m_sImageURL.toUtf8());
    }

    if( !actData.m_sVideoURL.isEmpty())
    {
        mapParams.insert(NET_ACTION_VIDEO_PATH, actData.m_sVideoURL.toUtf8());
    }

    if( !actData.m_sActName.isEmpty())
    {
        mapParams.insert(NET_ACTION_NAME, actData.m_sActName.toUtf8());
    }

    if( !actData.m_sActDesc.isEmpty())
    {
        mapParams.insert(NET_ACTION_DESCIBER, actData.m_sActDesc.toUtf8());
    }

    if( !actData.m_sPublishTime.isEmpty())
    {
        mapParams.insert(NET_ACTION_PUBLIC_DATE, actData.m_sPublishTime.toUtf8());
    }

    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nActType)
    {
//        mapParams.insert("actionType", QString::number(actData.m_nActType).toLatin1());////动作分类
        mapParams.insert(NET_ACTION_TYPE, QString::number(actData.m_nActType).toLatin1());////动作分类
    }

    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_dwActTimeLen)
    {
        mapParams.insert(NET_ACTION_TIME, QString::number(actData.m_dwActTimeLen).toLatin1());
    }

    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nActionDownloads)
    {
        mapParams.insert(NET_ACTION_DOWNLOAD_TIME, QString::number(actData.m_nActionDownloads).toLatin1());
    }

    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nActionComments)
    {
        mapParams.insert(NET_ACTION_COMMENT_TIME, QString::number(actData.m_nActionComments).toLatin1());
    }

    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nActionLikes)
    {
        mapParams.insert(NET_ACTION_PRAISE_TIME, QString::number(actData.m_nActionLikes).toLatin1());
    }

    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nActionCollectTime)
    {
        mapParams.insert(NET_ACTION_COLLECT_TIME, QString::number(actData.m_nActionCollectTime).toLatin1());
    }

    if (DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nActionHotValue)
    {
        mapParams.insert(NET_ACTION_HOT_VALUE, QString::number(actData.m_nActionHotValue).toLatin1());
    }

    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nActionStatus)
    {
        mapParams.insert(NET_ACTION_STATUS, QString::number(actData.m_nActionStatus).toLatin1());
    }

    if( !actData.m_sPublisher.isEmpty())
    {
        mapParams.insert(NET_ACTION_USER, actData.m_sPublisher.toUtf8());
    }

    if( !actData.m_sActionTitle.isEmpty())
    {
        mapParams.insert(NET_ACTION_TITLE, actData.m_sActionTitle.toUtf8());
    }

    if( !actData.m_sLocalPath.isEmpty())
    {
        mapParams.insert(strUndefindKey, actData.m_sLocalPath.toUtf8());
    }

    if(DEFAULT_INVALID_NOINIT_INT_NUMBER != actData.m_nHasDownloadedBytes)
    {
        mapParams.insert(NET_ACTION_DOWNLOAD_TIME, QString::number(actData.m_nHasDownloadedBytes).toLatin1());
    }
}

