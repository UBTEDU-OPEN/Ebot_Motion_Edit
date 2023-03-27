#include "cdownloadpic.h"

#include <QFile>


#include "QtHttpOperSync.h"


CDownLoadPic::CDownLoadPic()
{

}

CDownLoadPic::~CDownLoadPic()
{

}

bool CDownLoadPic::downPic(const QString &strNetPath, const QString &strPicPathToSave, bool bDelWhenExist)
{
    if(strNetPath.isEmpty() || strPicPathToSave.isEmpty())
    {
        return false;
    }

    if(bDelWhenExist)
    {
        QFile::remove(strPicPathToSave);
    }

    QtHttpOperSync httpQperSync;
    byte* pData = NULL;
    qint64 nLen = 0;
    httpQperSync.ExecuteRequest( eRequestGet, strNetPath, NULL, NULL, NULL, pData, nLen, NULL);
    bool bOk = false;
    if(pData)
    {
        QPixmap pixmap;
        pixmap.loadFromData( pData, nLen);
        pixmap.save(strPicPathToSave);

        bOk = true;
    }
    SAFE_DELETE_ARRAY(pData);

    return bOk;
}

