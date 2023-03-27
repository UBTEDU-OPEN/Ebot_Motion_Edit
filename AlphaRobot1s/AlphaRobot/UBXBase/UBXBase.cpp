#include "UBXBase.h"

#include "../Common/robotDefine.h"


#include <QDomDocument>



const int NOTIFY_MSG_PLAY_MUSIC = QEvent::registerEventType(); // 播放音乐
const int NOTIFY_MSG_PLAY_MOTION = QEvent::registerEventType(); // 播放动作
const int NOTIFY_MSG_UNITED_PLAY =  QEvent::registerEventType(); // 联合播放
const int NOTIFY_MSG_STOP_PLAY =  QEvent::registerEventType(); // 联合播放


const int NOTIFY_MSG_ROBOT_PRODUCT_TYPE_CHANGE =  QEvent::registerEventType(); // robot type change


UBXBase* UBXBase::getInstance()
{
    static UBXBase ubxBase(NULL);
    return &ubxBase;
}

UBXBase::UBXBase(QObject *parent) : QObject(parent)
{
    m_eCurrentRobotProductType =eUbtProductUnknown;// eUbtProduct1S1P;//eUbtProduct1E;//eUbtProductUnknown;
}

UBXBase::~UBXBase()
{

}

void UBXBase::setMotorDefaultMaxAngle(const vector<MOTOR_DATA> &vecMotorDataDefaultMaxAngle, emUbtProductType eUbtProductType)
{
    const int nSize = vecMotorDataDefaultMaxAngle.size();
    vector<MOTOR_DATA> tempVecMotorData;
    for(int i = 0; i < nSize; i++)
    {
        tempVecMotorData.push_back(vecMotorDataDefaultMaxAngle.at(i));
    }
    if(tempVecMotorData.size() > 0)
    {
        m_mapVecMotorDataDefaultMaxAngle.insert(eUbtProductType, tempVecMotorData);
    }
}

bool UBXBase::readMotorDefaultMaxAngle(const QString &strPath, vector<MOTOR_DATA> &vecMotorDataDefaultMaxAngle)
{
    vecMotorDataDefaultMaxAngle.clear();
    if(strPath.isEmpty())
    {
        return false;
    }

    QFile file(strPath);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QDomDocument XMLDoc;
    if(!XMLDoc.setContent(&file))
    {
        file.close();
        return false;
    }
    file.close();

    // 获取doc的第一个节点，即XML说明
    QDomNode  firstNode = XMLDoc.firstChild();

    //返回根元素
    QDomElement  docELem = XMLDoc.documentElement();
    try
    {
        QDomNodeList iObjList = docELem.childNodes();
        int iObjCnt = iObjList.count();
        if (iObjCnt == 0)
        {
            return false;
        }
    }
    catch (...)
    {
        return false;
    }

    //返回根节点的第一个子节点
    QDomNode NodePtr = docELem.firstChild();

    int iNum = 0;
    while(!NodePtr.isNull())
    {
        //如果节点是元素
        if(NodePtr.isElement())
        {
            //将其转化为元素
            QDomElement elem = NodePtr.toElement();

            //获得元素e的所有子节点的列表
            QDomNodeList list = elem.childNodes();

            MOTOR_DATA motoData;

            //遍历该列表
            for(int i = 0; i < list.count(); i++)
            {
                QDomNode node = list.at(i);
                if(node.isElement())
                {
                    QString strKey = node.toElement().tagName();
                    QByteArray byteKey = strKey.toLatin1();
                    char * szKey = byteKey.data();
                    QString strValue = node.toElement().text();
                    QByteArray byteValue = strValue.toLatin1();
                    char * szValue = byteValue.data();

                    if(strcmp("ID",szKey) == 0)                    // ID
                    {
                        motoData.nID = atoi(szValue);
                        continue;
                    }
                    else if(strcmp("DefaultMaxAngle", szKey) == 0)         // INIANGLE
                    {
                        int nMaxAngle = atoi(szValue);
                        motoData.nAngle = nMaxAngle;
                        continue;
                    }
                }
            }

            vecMotorDataDefaultMaxAngle.push_back(motoData);

            iNum ++;
        }
        //转到下一个兄弟节点
        NodePtr = NodePtr.nextSibling();
    }

    return true;
}

emUbtProductType UBXBase::getCurrentRobotProductType() const
{
    return m_eCurrentRobotProductType;
}

// if the ProductType value have changed return true, or return false
bool UBXBase::setCurrentRobotProductType(const emUbtProductType &eCurrentRobotProductType)
{
    bool bChanged = false;
    if(m_eCurrentRobotProductType != eCurrentRobotProductType)
    {
        emUbtProductType eTempOld = m_eCurrentRobotProductType;
        m_eCurrentRobotProductType = eCurrentRobotProductType;
        bChanged = true;

        emit sigRobotProductTypeChange( eTempOld, m_eCurrentRobotProductType);
    }

    return bChanged;
}
// if the ProductType value have changed return true, or return false
bool UBXBase::setCurrentRobotProductType(const QString &strCurrentRobotProductType)
{
    bool bChanged = false;
    const emUbtProductType eType = UBXBase::toProductType(strCurrentRobotProductType);
    if(eUbtProductUnknown != eType)
    {
        bChanged = setCurrentRobotProductType(eType);
    }

    return bChanged;
}

bool UBXBase::setToDefaultRobotProductType()
{
    return UBXBase::setCurrentRobotProductType(eUbtProduct1S1P);
}

emUbtProductType UBXBase::toProductType(const QString &strProductType)
{
    emUbtProductType eProductType = eUbtProductUnknown;
    if( ROBOT_TYPE_ROBOT1E == strProductType)
    {
        eProductType = eUbtProduct1E;
    }
    else if( ROBOT_TYPE_ROBOT1P == strProductType)
    {
        eProductType = eUbtProduct1S1P;
    }

    return eProductType;
}

QString UBXBase::productTypeToStr(emUbtProductType eUbtProductType)
{
    QString strType;
    switch (eUbtProductType) {
    case eUbtProduct1S1P:
        strType = ROBOT_TYPE_ROBOT1P;
        break;

    case eUbtProduct1E:
        strType = ROBOT_TYPE_ROBOT1E;
        break;

    default:
        break;
    }

    return strType;
}

int UBXBase::getDefaultActAngleValueMax(int nID)
{
    if(eUbtProduct1E == m_eCurrentRobotProductType)
    {
        vector<MOTOR_DATA> vecData = m_mapVecMotorDataDefaultMaxAngle.value(m_eCurrentRobotProductType);
        if(vecData.size() > 0)
        {
            const int nSize = vecData.size();
            for(int i = 0; i < nSize; i++)
            {
                const MOTOR_DATA& tempMotorData = vecData.at(i);
                if( tempMotorData.nID == nID)
                {
                    const int nAngle = tempMotorData.nAngle;
                    if((nAngle >= ROBOT_DEFALUT_ACT_ANGLE_VALUE_MIN)
                            && (nAngle <= ROBOT_DEFALUT_ACT_ANGLE_VALUE_MAX_255))
                    {
                        return nAngle;
                    }

                    return ROBOT_DEFALUT_ACT_ANGLE_VALUE_MAX_255;
                }
            }
        }

        return ROBOT_DEFALUT_ACT_ANGLE_VALUE_MAX_255;
    }

    return ROBOT_DEFALUT_ACT_ANGLE_VALUE_MAX_180;
}
