#include "custreewidget.h"


#include "../zxLib/configs.h"
#include "../Common/UBXPublic.h"
#include "../zxLib/widget/messagebox.h"

CusTreeWidget::CusTreeWidget(QWidget *parent):
    CTemplateActTreeWdg(parent)
{
    setTemplateDir(CConfigs::getLocalCusTemplateDir());
}

CusTreeWidget::~CusTreeWidget()
{

}

//void CusTreeWidget::mouseMoveEvent(QMouseEvent *event)
//{

//    if (!(event->buttons() & Qt::LeftButton))
//        return;

//    if ((event->pos() - m_startDragPoint).manhattanLength()  < QApplication::startDragDistance())
//        return;
//    if(m_DragItem == NULL)
//    {
//        return;
//    }

//    QDrag *drag = new QDrag(this);
//    QMimeData *mimeData = new QMimeData;

//    // 把信息传出
//    mimeData->setData("TemplateAction", m_DragByteArray);
//    mimeData->setText(m_strActionName);
//    drag->setMimeData(mimeData);

//    const QString StrPicFilePathName = CConfigs::getLocalResPath("loginhead.png");

//    // 设置禁止图片
//    QPixmap dragImg(StrPicFilePathName);

//    QPixmap newDragImg = dragImg.scaled(30, 30);
//    drag->setPixmap(newDragImg);
//    Qt::DropAction resultAction = drag->exec(Qt::MoveAction);
//    if(resultAction == Qt::IgnoreAction)
//    {
//        // 确认移动操作
//        // 该操作已经被目标操作接受，且操作类型为MOVE，一般来说应该把被拖动的物体从源窗口删除


//    }
//    if(drag)
//    {
//        drag->destroyed();
//        delete drag;
//        drag = NULL;
//    }
//    if(mimeData)
//    {
//        mimeData = NULL; //指针的内容已经被删除了
//    }


//}

//void CusTreeWidget::mousePressEvent(QMouseEvent *event)
//{
//    if (event->button() == Qt::LeftButton)
//    {
//        m_startDragPoint = event->pos();
//        m_DragItem = itemAt(event->pos());
//        if(m_DragItem == NULL)
//        {
//            return;
//        }
//        QString strNodeName = m_DragItem->text(0);
//        QTreeWidgetItem* parent = m_DragItem->parent();
//        //        if(parent == NULL)
//        //        {
//        //            m_DragItem = NULL;
//        //            return;
//        //        }
//        if(parent && (parent->parent() == NULL))
//        {
//            bool IsFoundData = false;
//             QString strCusActionPath = "";
//            strCusActionPath = CConfigs::getLocalCusTemplateDir();
//            QDir dir(strCusActionPath);
//            if(!dir.exists())
//            {
//                return ; //没有找到文件，即没有数据
//            }

//            //设置过滤器(目录，文件或非上级目录)
//            dir.setFilter(QDir::Files);
//            dir.setSorting(QDir::Name);
//            //取得目录中文件列表(包含目录)
//            QFileInfoList list = dir.entryInfoList();
//            int i=0;
//            do{
//                QFileInfo fileInfo = list.at(i);
//                QString strTmp = fileInfo.fileName();
//                int pos = strTmp.indexOf('.');
//                QString strFileName = strTmp.left(pos);
//                QString strExtName = strTmp.right(strTmp.length() - pos);
//                if(strExtName.compare(UBX_TEMPLATE_EXT, Qt::CaseInsensitive) != 0)
//                {
//                    i++;
//                    continue;
//                }
//                if (!strFileName.compare(strNodeName, Qt::CaseInsensitive))
//                {
//                    m_strActionName = strNodeName;
//                    m_DragByteArray.clear();
//                    IsFoundData = true;
//                    QString strActionFilePathName = fileInfo.absoluteFilePath();
//                    QFile file(strActionFilePathName);
//                    file.open(QFile::ReadOnly);
//                    int nLens = file.size();
//                    char* pData = new char[nLens];
//                    file.read(pData,nLens);
//                    file.close();
//                    //解析数据
//                    ParseData(pData, nLens);
//                    if(pData)
//                    {
//                        delete[]pData;
//                        pData = NULL;
//                    }
//                    break;
//                }

//                i++;
//            }while(i<list.size());
//        }

//    }
//    QTreeWidget::mousePressEvent(event);
//}

//bool CusTreeWidget::ParseData(char *pData, int nLens)
//{
//    int nTotalLen = 0;
//    UBX_MEMCPY_INC(nTotalLen, pData);
//    if(nTotalLen != nLens)
//    {
//        CMessageBox::warning(NULL, tr("data"), tr("invalid file"));
//        return false;
//    }
//    //获取文件头

//    int nTagLen = 0;
//    UBX_MEMCPY_INC(nTagLen, pData);

//    if(nTagLen != strlen(UBX_TEMPLATE_TAG))
//    {
//        CMessageBox::warning(NULL, tr("data"), tr("invalid file"));
//        return false;
//    }
//    char* pTagData =new char[nTagLen];
//    memcpy(pTagData, pData, nTagLen);
//    pData += nTagLen;
//    if(memcmp(pTagData, UBX_TEMPLATE_TAG, nTagLen) != 0)
//    {
//        CMessageBox::warning(NULL, tr("data"), tr("invalid file"));
//        return false;
//    }

//    //获取Data
//    char* pRawData = NULL;
//    int nRawDataLen = 0;
//    UBX_MEMCPY_INC(nRawDataLen, pData);
//    if(nRawDataLen)
//    {
//        pRawData = new char[nRawDataLen];
//        memcpy(pRawData, pData,nRawDataLen);
//        pData += nRawDataLen;
//    }

//    m_DragByteArray = QByteArray(pRawData,nRawDataLen);
//    if(pTagData)
//    {
//        delete []pTagData;
//        pTagData = NULL;
//    }
//    if(pRawData)
//    {
//        delete []pRawData;
//        pRawData = NULL;
//    }
//    return true;
//}
