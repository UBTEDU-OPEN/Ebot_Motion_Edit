#ifndef CDOWNLOADPIC_H
#define CDOWNLOADPIC_H

#include <QString>

class CDownLoadPic
{
public:
    CDownLoadPic();
    ~CDownLoadPic();

public:
    //asu
    static bool downPic(const QString& strNetPath, const QString& strPicPathToSave, bool bDelWhenExist = true);

};

#endif // CDOWNLOADPIC_H
