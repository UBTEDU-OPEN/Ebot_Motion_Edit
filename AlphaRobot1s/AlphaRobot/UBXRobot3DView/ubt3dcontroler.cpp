#include "ubt3dcontroler.h"

#include <QMap>


Ubt3DControler::Ubt3DControler():
    m_pRobot3DView(NULL)   
{

}

Ubt3DControler::~Ubt3DControler()
{
    
}

Ubt3DControler* Ubt3DControler::instance()
{
    static Ubt3DControler Ubt3DControler;
    return &Ubt3DControler;
}

void Ubt3DControler::setRobot3DView(IUBXRobot3DView *pRobot3DView)
{
    m_pRobot3DView = pRobot3DView;
}

IUBXRobot3DView *Ubt3DControler::getRobot3DView()
{
    return m_pRobot3DView;
}

int Ubt3DControler::getMotorAngle(const int nNodeID) const
{
    QMap<int , int> mapMotors;
    m_pRobot3DView->getMotorInfo(mapMotors);

    return mapMotors.value(nNodeID);
}

bool Ubt3DControler::testNodeValue(const int nNodeID, const int /*nOldValue*/, const int nTestValue) const
{
    if(!m_pRobot3DView)
    {
        return false;
    }

    bool bCanSet = true;

    if((7 ==  nNodeID) || (12 == nNodeID))
    {
        if(7 == nNodeID)
        {
            const int nNode12Angle = getMotorAngle(12);
            if(nTestValue > nNode12Angle)
            {
                bCanSet = false;
            }
        }
        else if (12 == nNodeID)
        {
            const int nNode7Angle = getMotorAngle(7);
            if(nTestValue < nNode7Angle)
            {
                bCanSet = false;
            }
        }
    }

    return bCanSet;
}

bool Ubt3DControler::getFitedNodeValue(const int nNodeID, const int nOldValue, const int nTestValue, int& nFitedValue) const
{
    bool bCanSet = false;
    if(testNodeValue(nNodeID, nOldValue, nTestValue))
    {
        bCanSet = true;
        return bCanSet;
    }

    if(!m_pRobot3DView)
    {
        return false;
    }

    if((7 ==  nNodeID) || (12 == nNodeID))
    {
        if(7 == nNodeID)
        {
            const int nNode12Angle = getMotorAngle(12);
            if(nTestValue > nNode12Angle)
            {
                bCanSet = false;

                nFitedValue = nNode12Angle;
            }
        }
        else if (12 == nNodeID)
        {
            const int nNode7Angle = getMotorAngle(7);
            if(nTestValue < nNode7Angle)
            {
                bCanSet = false;

                nFitedValue = nNode7Angle;
            }
        }
    }

    return bCanSet;
}
