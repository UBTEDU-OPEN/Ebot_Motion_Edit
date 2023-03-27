#ifndef CSORTHANDLE_H
#define CSORTHANDLE_H

#include <QList>


class CSortHandle
{
public:
    CSortHandle();

public:
    static QList<int> sortInt(const QList<int>& lst, bool bUpSort);


};

#endif // CSORTHANDLE_H
