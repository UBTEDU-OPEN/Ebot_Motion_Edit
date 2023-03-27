#include "cjsonutil.h"


#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>


CJsonUtil::CJsonUtil()
{

}

CJsonUtil::~CJsonUtil()
{

}

QString CJsonUtil::getValue(const QString &strJson, const QString &strKey)
{
    QByteArray data(strJson.toStdString().c_str(), strJson.toStdString().length());
    QJsonParseError jsonError;
    QJsonDocument parseDocument = QJsonDocument::fromJson(data, & jsonError);

    QString strValue;
    if(QJsonParseError::NoError == jsonError.error)
    {
        if(parseDocument.isObject())
        {
            QJsonObject obj = parseDocument.object();
            if(!obj.empty())
            {
                strValue = obj.find(strKey).value().toString();
            }
        }
    }

    return strValue;
}
