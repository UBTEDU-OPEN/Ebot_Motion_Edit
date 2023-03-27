#include "cxmlhandle.h"


#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QDomProcessingInstruction>
#include <QTextStream>
#include <QTextCodec>

CXmlHandle::CXmlHandle()
{

}

CXmlHandle::~CXmlHandle()
{

}

bool CXmlHandle::saveXml(const QString &strXmlFilePathOut, const QDomDocument &docXmlSaved)
{
    if(strXmlFilePathOut.isEmpty())
    {
        return false;
    }

    QFile file(strXmlFilePathOut);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
        return false;

    QTextStream out(&file);
    out.setCodec("UTF-8");
    docXmlSaved.save(out, 4, QDomNode::EncodingFromTextStream);
    file.close();

    return QFile::exists(strXmlFilePathOut);
}

