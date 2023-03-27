#ifndef _UBXBASE_H_
#define _UBXBASE_H_

//#ifdef UNICODE
//#define	UBX_strcmp	wcscmp
//#define	UBX_strcpy	wcscpy
//#define	UBX_strcat	wcscat
//#define	UBX_strlen	wcslen
//#define	UBX_strstr	wcsstr
//#define	UBX_strrstr	wcsrstr
//#define	UBX_strrstr	wcsrstr
//#define	UBX_strrchr	wcsrchr
//#define	UBX_strchr	wcschr
//#define	UBX_sprintf	wsprintf
//#define	UBX_fopen	_wfopen
//#define	UBX_atoi		_wtoi
//#else
//#define	UBX_strcmp	strcmp
//#define	UBX_strcpy	strcpy
//#define	UBX_strcat	strcpy
//#define	UBX_strlen	strlen
//#define	UBX_strstr	strstr
//#define	UBX_strrstr	strrstr
//#define	UBX_strchr	strchr
//#define	UBX_strrchr	strrchr
//#define	UBX_sprintf	sprintf
//#define	UBX_fopen	fopen
//#define	UBX_atoi		atoi
//#endif
//#include "RawData.h"
//#include "BaseBlock.h"
//#include "PortConnetMap.h"
//#include "XMLreader.h"
//#include "AngleTransform.h"
//#include "UBXThread.h"
//#include "UBXSocket.h"
//#include "UBXCritSec.h"
//#include "UBXBitrate.h"

#include "stable.h"

#include <map>
#include <list>
#include <vector>
#include <QObject>
#include <QMap>

using namespace	std;
typedef map		<unsigned long,void*>	ULONG_POINTER_MAP;
typedef pair	<unsigned long,void*>	ULONG_PAIR;

extern UBXBASESHARED_EXPORT const int NOTIFY_MSG_PLAY_MUSIC; // 播放音乐
extern UBXBASESHARED_EXPORT const int NOTIFY_MSG_PLAY_MOTION; // 播放动作
extern UBXBASESHARED_EXPORT const int NOTIFY_MSG_UNITED_PLAY; // 联合播放
extern UBXBASESHARED_EXPORT const int NOTIFY_MSG_STOP_PLAY; // 停止播放


extern UBXBASESHARED_EXPORT const int NOTIFY_MSG_ROBOT_PRODUCT_TYPE_CHANGE; //



enum emAngleChectWayType
{
    eAngleCheckWayUnknown = 0,
    eAngleCheckWayAuto,
    eAngleCheckWayManual ,
};


class UBXBASESHARED_EXPORT UBXBase : public QObject
{
    Q_OBJECT

public:
    static UBXBase* getInstance();

    emUbtProductType getCurrentRobotProductType() const;
    // if the ProductType value have changed return true, or return false
    bool setCurrentRobotProductType(const emUbtProductType &eCurrentRobotProductType);
     // if the ProductType value have changed return true, or return false
    bool setCurrentRobotProductType(const QString & strCurrentRobotProductType);
    bool setToDefaultRobotProductType();
    static emUbtProductType toProductType(const QString& strProductType);
    static QString productTypeToStr(emUbtProductType eUbtProductType);

    int getDefaultActAngleValueMax(int nID);
    void setMotorDefaultMaxAngle(const vector<MOTOR_DATA> &vecMotorDataDefaultMaxAngle, emUbtProductType eUbtProductType);
    static bool readMotorDefaultMaxAngle(const QString& strPath, vector<MOTOR_DATA> &vecMotorDataDefaultMaxAngle);

signals:
    void sigRobotProductTypeChange(int eUbtProductTypeOld, int eUbtProductTypeNew);
    void sigAngleValueCheckWay(emAngleChectWayType eAngleChectWayType);


private:
    explicit UBXBase(QObject *parent = 0);
    ~UBXBase();


private:
    emUbtProductType m_eCurrentRobotProductType;

    typedef QMap<emUbtProductType, vector<MOTOR_DATA> > MapVecMotorDefaultMaxAngle;
    MapVecMotorDefaultMaxAngle  m_mapVecMotorDataDefaultMaxAngle; //默认舵机数据max angle
};

#endif  //_UBXBASE_H_
