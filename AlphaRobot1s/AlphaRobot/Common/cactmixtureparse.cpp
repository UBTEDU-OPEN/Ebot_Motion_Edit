#include "cactmixtureparse.h"


#include "filedirhandle.h"

#include "cxmlhandle.h"





CActMixtureParse::CActMixtureParse()
{

}

bool CActMixtureParse::htsToMixXml(const unsigned char * const pBufHts, int nLenTotalHts, const QString &strFileXml)
{
    if(!pBufHts || (nLenTotalHts <= 0) || strFileXml.isEmpty())
    {
        return false;
    }

    if(!CHtsParser::isHtsFormat(pBufHts, nLenTotalHts))
    {
        return false;
    }

    const unsigned char * const pBufHtsDataHead = pBufHts;
    const unsigned char *pBufPerPageHead = NULL;

    int nHtsDataHadPaser = 0;


    QDomDocument doc;
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    QDomElement root = doc.createElement("frame_xml");
    root.setAttribute("actionname", "NULL");
    root.setAttribute("versionname", "NULL");
    root.setAttribute("actionImagePath", "NULL");
    root.setAttribute("actionVideoPath", "NULL");
    doc.appendChild(root);

    for(; nHtsDataHadPaser < nLenTotalHts;)
    {
        bool bFindData = CHtsParser::findHtsPerPageHeadPoint((const unsigned char *)pBufHtsDataHead + nHtsDataHadPaser,
                                                             nLenTotalHts - nHtsDataHadPaser,
                                                             pBufPerPageHead,
                                                             HTS_PER_PAGE_SIZE);
        if(!bFindData || !pBufPerPageHead)
        {
            break;//!!!!when is not find perpage data, maybe finish.
        }

        HTS_PER_PAGE_INFO htsPerPageInfo;
        bool bGetData = CHtsParser::parserHtsPerPageInfo(pBufPerPageHead, HTS_PER_PAGE_SIZE, htsPerPageInfo);
        if(bGetData)
        {
            QDomElement elementPerPage = CActMixtureParse::getXmlElementActFrame(htsPerPageInfo);
            root.appendChild(elementPerPage);
        }

        nHtsDataHadPaser = (char *)pBufPerPageHead + HTS_PER_PAGE_SIZE - (char *)pBufHtsDataHead;
        if(nLenTotalHts <= nHtsDataHadPaser)
        {
            break;
        }
    }

    int nTotalTimeSecond = 0;
    CHtsParser::getActTotalTime((const char *const)pBufHtsDataHead, nLenTotalHts, nTotalTimeSecond);
    QDomElement eleAllRunTime = CActMixtureParse::getXmlElementAllRunTime(nTotalTimeSecond);
    root.appendChild(eleAllRunTime);

    bool bOk = CXmlHandle::saveXml(strFileXml, doc);
    return bOk;
}

QDomElement CActMixtureParse::getXmlElementActFrame(const HTS_PER_PAGE_INFO &htsPerPageInfo)
{
    QDomDocument doc;
    QDomElement ele = doc.createElement("frame");
    ele.setTagName("frame");

    QString strAngles = "";
    for(int j = ROBOT_ID_DEFAULT_START_NUMBER; j < (ROBOT_ID_DEFAULT_START_NUMBER + ROBOT_TOTAL_MOTOR_NUMBER); j++)
        for(int i = 0; i < ROBOT_TOTAL_MOTOR_NUMBER; i++)
        {
            const int nCurrentID = j;
            if(nCurrentID == htsPerPageInfo.motorData[i].nID)
            {
                strAngles += QString::number(htsPerPageInfo.motorData[i].nAngle);

                if(nCurrentID != (ROBOT_ID_DEFAULT_START_NUMBER + ROBOT_TOTAL_MOTOR_NUMBER - 1))
                {
                    strAngles += "#";
                }
            }
        }

    ele.setAttribute("xmldata", strAngles);
    //ele.setAttribute("xmltime", QString::number(nAllRunTime));
    ele.setAttribute("xmlRunTime", QString::number(htsPerPageInfo.nRunTime * DEFAULT_RUNTIME_UNIT));//ms
    ele.setAttribute("xmlAllRunTime", QString::number(htsPerPageInfo.nAllRunTime * DEFAULT_RUNTIME_UNIT));//ms

    ele.setAttribute("xmlFrameStatus", QString::number(htsPerPageInfo.eHtsFrameStat));
    ele.setAttribute("xmlFrameIndex", QString::number(htsPerPageInfo.nCurrentFrame));
    ele.setAttribute("xmlFrameAll", QString::number(htsPerPageInfo.nAllFrame));

    return ele;
}

QDomElement CActMixtureParse::getXmlElementAllRunTime(int nAllRunTimeMillisecond)
{
    QDomDocument doc;
    QDomElement ele = doc.createElement("trail");
    ele.setTagName("trail");

    ele.setAttribute("xmlAllRunTime", QString::number(nAllRunTimeMillisecond));
    return ele;
}
