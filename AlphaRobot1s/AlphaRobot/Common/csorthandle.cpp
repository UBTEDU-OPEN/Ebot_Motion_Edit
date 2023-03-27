#include "csorthandle.h"


CSortHandle::CSortHandle()
{

}

QList<int> CSortHandle::sortInt(const QList<int> &lst, bool bUpSort)
{
    QList<int> lstTemp = lst;
    if(bUpSort)
    {
        qStableSort(lstTemp.begin(), lstTemp.end(), qGreater<int>());
    }
    else
    {
        qStableSort(lstTemp.begin(), lstTemp.end());
    }
    return lstTemp;
}

