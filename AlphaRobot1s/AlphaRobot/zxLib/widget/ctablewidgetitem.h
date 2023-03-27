/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：CTableWidgetItem
* 创建时间：20170715
* 文件标识：
* 文件摘要：CTableWidgetItem
*
* 当前版本：1.0.0.0
* 作    者：asu
*/

#ifndef CTABLEWIDGETITEM_H
#define CTABLEWIDGETITEM_H


#include <QTableWidgetItem>



class ZXLIB_DLL CTableWidgetItem : public QTableWidgetItem
{
public:    
    explicit CTableWidgetItem(const QString &text);
    ~CTableWidgetItem();

public:
    void setTextRetranslate(const QString& strRetranslateContextParentName, const QString&  strKey, const QString& strTr);
    void setTextRetranslate(const QString&  strKey, const QString& strTr);

    QString textKeyForRetranslate() const;
    bool isEmptyRetranslateKey()const;
    QString retranslateContext() const;
    void setRetranslateContext(const QString &strRetranslateContext);

private:
    QString m_strTextKeyForRetranslate;
    QString m_strRetranslateContext;//class name of parent
};

#endif // CTABLEWIDGETITEM_H
