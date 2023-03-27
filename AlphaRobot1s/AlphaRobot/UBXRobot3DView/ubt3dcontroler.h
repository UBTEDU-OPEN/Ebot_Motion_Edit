#ifndef UBT3DCONTROLER_H
#define UBT3DCONTROLER_H


#include "IUBXRobot3DView.h"


class Ubt3DControler
{
public:
    ~Ubt3DControler();

public:
    static Ubt3DControler* instance();

public:
    void setRobot3DView(IUBXRobot3DView* pRobot3DView);
    IUBXRobot3DView* getRobot3DView();

    int getMotorAngle(const int nNodeID)const;

    bool testNodeValue(const int nNodeID, const int nOldValue, const int nTestValue)const;
    bool getFitedNodeValue(const int nNodeID, const int nOldValue, const int nTestValue, int& nFitedValue)const;


private:
    explicit Ubt3DControler();

private:
    IUBXRobot3DView* m_pRobot3DView;
};

#endif // UBT3DCONTROLER_H
