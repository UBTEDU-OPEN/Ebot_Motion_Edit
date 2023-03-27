/*********************************************
* Copyright (c) 2015, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CLanguageMgr
* 创建时间：2017/03/02
* 文件标识：
* 文件摘要：CLanguageMgr
*
* 当前版本：1.0.0.0
* 作    者：asu
* 完成时间：2017/08/02
* 版本摘要：
*/

#ifndef CLANGUAGEMGR_H
#define CLANGUAGEMGR_H


#include <QObject>
#include <QApplication>


#include "stable.h"


class UBXBASESHARED_EXPORT CLanguageMgr : public QObject
{
    Q_OBJECT

public:
    static CLanguageMgr* getInstance();


    QString getLocalLanguageDir() const;
    void setLocalLanguageDir(const QString &strLocalLanguageDir);

    QString getLanguageKeyword() const;
    void setLanguageKeyword(const QString &strLanguageKeyword);

    QApplication *getApplication() const;
    void setApplication(QApplication *pApplication);

    bool setLanguage(QString strLocalLanguageDir, QString strLanguageKeyword);

    //for login language on server
    static QString getNetLanguageKeyword(const QString& strLanguageKeyword);

private:
    explicit CLanguageMgr(QObject *parent = 0);
    ~CLanguageMgr();

private:
    QString m_strLocalLanguageDir;
    QString m_strLanguageKeyword;

    QApplication* m_pApplication;
};

#endif // CLANGUAGEMGR_H
