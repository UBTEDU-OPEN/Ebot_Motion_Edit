#ifndef FILESSAVEASUI_H
#define FILESSAVEASUI_H

#include <QDialog>
#include <QStringList>

#include "../zxLib/widget/ubxdialog.h"
#include "UBXPublic.h"


class IUBXMotionEditor;


namespace Ui {
class CFilesSaveAsUI;
}

enum EFileSaveType
{
    eSaveTypeNoSave = 0,
    eSaveTypeSaveToLocal,
    eSaveTypeSaveToLocalAndUpload
};

class SFileSaveAsDatas
{
public:
    qint64 nFileActionID;

    int nSaveType;

    QString strSaveFileName;//file name
    QString strSaveFilePath;//file dir

    //
    QString strFilePathMusic;
    QString strFilePathHts;
    QString strFileAesx;


public:
    SFileSaveAsDatas()
    {
        clearDatas();
    }

    void clearDatas()
    {
        nFileActionID = DEFAULT_INVALID_NOINIT_INT_NUMBER;
        nSaveType = eSaveTypeNoSave;

        strSaveFileName.clear();
        strSaveFilePath.clear();

        //
        strFilePathMusic.clear();
        strFilePathHts.clear();
        strFileAesx.clear();
    }
};

class CFilesSaveAsUI : public CUBXDialog
{
    Q_OBJECT

public:
    explicit CFilesSaveAsUI(SFileSaveAsDatas* pSaveAsDatas, QWidget *parent = 0);
    ~CFilesSaveAsUI();
    
public:
    void setIUBXMotionEditor(IUBXMotionEditor* pIUBXmEditor);

    //save ui data to datas
    void saveData();
    //update ui
    void initUI(SFileSaveAsDatas datas);

    //save file handle
    static bool saveFile(IUBXMotionEditor* pIUBXmEditor, SFileSaveAsDatas& datas);



protected:
    virtual void resizeEvent(QResizeEvent *);
    RETRANSLATEUIFUNC

protected slots:
    void onTimerout(int nTimerID);

    /************************************
    * 名称: OnUploadProgress
    * 功能: 上传文件进度通知
    * 参数:
    *    @[in ] sRequestType: 请求类型字符串
    *    @[in ] nRequestID:  请求ID
    *    @[in ] nActionID:  动作ID
    *    @[in ] bytesUploaded: 已上传的数据长度
    *    @[in ] bytesTotal: 计划上传的数据总长度
    * 返回: void
    * 时间: 20160817
    * 作者: asu
    */
    void OnUploadProgress(const QString& sRequestType, int nRequestID, qint64 nActionID, qint64 bytesUploaded, qint64 bytesTotal);


private slots:
    void onFilePathClicked();
    void onSaveAndUpLoadClicked();
    void onSaveLocalClicked();
    void onCancelClicked();
    
    
private:
   bool showMessageIfEmpty();
   void showMessageUploadResult(bool bSucceed);
   bool saveFileWhenExist();

private:
    Ui::CFilesSaveAsUI *ui;

private:
    IUBXMotionEditor* m_pIUBXmEditor;//for send edit data

    SFileSaveAsDatas* m_pFileSaveAsDatas;//for user edit
    SFileSaveAsDatas m_FileSaveAsDatasOld;//for cancel to not edit datas
    QString m_filePathPackedReturn;

    QStringList m_fileListUpload;

    int m_nRequestID;
    int m_nUploadActZipPercent;
};

#endif // FILESSAVEASUI_H
