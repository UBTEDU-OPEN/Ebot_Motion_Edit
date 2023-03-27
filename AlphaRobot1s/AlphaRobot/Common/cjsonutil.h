#ifndef CJSONUTIL_H
#define CJSONUTIL_H


#include <QString>
#include <string>

using namespace std;

class CJsonUtil
{
public:
    CJsonUtil();
    ~CJsonUtil();

public:
    static QString getValue(const QString& strJson, const QString& strKey);
};

#endif // CJSONUTIL_H
