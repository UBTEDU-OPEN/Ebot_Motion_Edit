
#include "actionlayer.h"
#include "datamodel/ActionBlockGroupManager.h"
#include "datamodel/ActionBlockData.h"
#include "mp3projconstants.h"
#include "mp3widget.h"
#include <QPixmap>

ActionLayer::ActionLayer(MP3Widget *mp3Widget)
{
    m_pMP3Widget = mp3Widget;
    m_pActionBlockDataManager = NULL;
    m_fCurStartTime = 0;
    m_fCurEndTime = 0;
}

void ActionLayer::SetActionBlockDataManager(CActionBlockDataManager *pActionBlockDataManger)
{
    m_pActionBlockDataManager = pActionBlockDataManger;
}

CActionBlockDataManager *ActionLayer::GetActionBlockDataManager()
{
    return m_pActionBlockDataManager;
}

void ActionLayer::SetSelectGroupBlockID(int nID)
{
    if(!m_listGroupSelectID.contains(nID))
    {
        m_listGroupSelectID.append(nID);
    }
}

void ActionLayer::setSelectGroupBlockID(QList<int> lstGroupID)
{
    const int nSize = lstGroupID.size();
    for(int i = 0; i < nSize; i++)
    {
        SetSelectGroupBlockID(lstGroupID.at(i));
    }
}

//void ActionLayer::SetSelectGroupBlockCol(int nCol)
//{
//    if(NULL == m_pActionBlockDataManager)
//    {
//        return;
//    }

//    for(int i = 0; i < m_pActionBlockDataManager->GetCount(); i++)
//    {
//        CActionBlockGroupManager* pManager = m_pActionBlockDataManager->GetAt(i);
//        if(pManager)
//        {
//            float fStartTime = pManager->GetStartTime();
//            float fEndTime = pManager->GetEndTime();
//            if(nCol >= fStartTime && nCol <= fEndTime)
//            {
//                int nID = pManager->GetID();
//                if(!m_listGroupSelectID.contains(nID))
//                {
//                    m_listGroupSelectID.append(nID);
//                    return;
//                }
//            }
//        }
//    }
//}

void ActionLayer::ClearSelectGroupBlock()
{
    m_listGroupSelectID.clear();
}

void ActionLayer::OnDrawActionGroup(QPixmap* pixmap)
{
    DrawBlockGroup(pixmap);
}

void ActionLayer::onDrawActionFrame(QPixmap *pixmap)
{
    drawActionFrames(pixmap);
}

void ActionLayer::DrawBlockGroup(QPixmap *pixmap)
{
    if(NULL == pixmap ||
       NULL == m_pMP3Widget ||
       NULL == m_pActionBlockDataManager)
    {
        return;
    }

    QPainter painter(pixmap);
    double nDisplayEndTime = m_pMP3Widget->GetDisplayEndTime();
    double nDisplayStartTime = m_pMP3Widget->GetDisplayStartTime();
    double xScale = m_pMP3Widget->GetXScale();
    m_fCurStartTime = nDisplayStartTime / m_pMP3Widget->GetTimeScale();
    m_fCurEndTime = nDisplayEndTime / m_pMP3Widget->GetTimeScale();
    SetActionBlockDataType::const_iterator iter = m_pActionBlockDataManager->FirstGroupIndex(m_fCurStartTime);

    for(;iter != m_pActionBlockDataManager->EndIndex(); iter++)
    {
        CActionBlockGroupManager* pGroupManager = *iter;
        if(pGroupManager == NULL)
        {
            continue;
        }
        //ActionBlock存储的时间，需要乘以时间粒度才是真正的时间(ms为单位)
        double nStartTime = pGroupManager->GetStartTime() * m_pMP3Widget->GetTimeScale();
        double nEndTime = pGroupManager->GetEndTime() * m_pMP3Widget->GetTimeScale();

        if( (nStartTime <= nDisplayEndTime) && (nEndTime >= nDisplayStartTime) )
        {
            QRect rect(0, 0, 0, pixmap->height());

            // 动作名称在视窗居中（而不是在整个动作组中居中）
            if (nStartTime >= nDisplayStartTime)
            {
                rect.setLeft( (nStartTime - nDisplayStartTime) *  xScale );
            }
            else
            {
                rect.setLeft(0);
            }

            if(nEndTime <= nDisplayEndTime)
            {
                rect.setRight( (nEndTime- nDisplayStartTime) * xScale );
            }
            else
            {
                rect.setRight( (nDisplayEndTime - nDisplayStartTime) * xScale );
            }

            if(IsSelectGroupBlock(pGroupManager->GetID()))
            {
                QBrush brush(Constants::COLOR_GROUP_BLOCK_SELECT);
                painter.fillRect(rect, brush);
            }
            else
            {
                QBrush brush(Constants::COLOR_GROUP_BLOCK_DEFAULT);
                painter.fillRect(rect, brush);
            }

            //这一段为windows原操作，暂时还不明白作用是什么，有待观察
            char* pdata = NULL;
            int nLen = 0;
            pGroupManager->GetExtBlockData(&pdata, nLen);
            delete pdata;
            if(nLen == 12 || nLen == 14)//判断这个的意义何在
            {
                QBrush brush(QColor(120, 80, 100));
                painter.fillRect(rect, brush);
            }

            //绘制组名称
            QFont font;
            font.setPixelSize(FONT_PIXEL_SIZE_16);
            painter.setFont(font);
            //QFontMetrics fm(font);
            QString strDescInfo = WCHARToQString(pGroupManager->GetDescription(),MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
            //int width = fm.width(strDescInfo);
            //int height = fm.height();
            painter.setBackgroundMode(Qt::TransparentMode);//绘制文字设置背景透明
            //QRect rectFont(rect.topLeft(), QPoint(rect.right(), rect.top() + rect.height()));
            painter.setPen(Constants::COLOR_TIME_FONT);
            QFontMetrics metricsGroupName(font);
            QRect rectGroupName = rect;
            QString strGroupName = metricsGroupName.elidedText(strDescInfo, Qt::ElideRight, rectGroupName.width(), Qt::TextSingleLine);
            painter.drawText(rectGroupName, Qt::TextWrapAnywhere | Qt::AlignCenter, strGroupName, &rectGroupName);
            //绘制底部cell实线
            QPen pen(Constants::COLOR_ACTION_FRAME_SELECT_FILLED, 2 * m_pMP3Widget->GetLineThick());
            painter.setPen(pen);
//            painter.drawLine(rect.topLeft(), rect.topRight());//上面一条横线
            painter.drawLine(rect.topLeft(), rect.bottomLeft());//左边一条竖线
//            painter.drawLine(rect.bottomLeft(), rect.bottomRight());//下面
            painter.drawLine(rect.topRight(), rect.bottomRight());//右边
        }
        else if(nStartTime > nDisplayEndTime)
        {
            return;
        }
    }
}

void ActionLayer::drawActionFrames(QPixmap *pixmap)
{
    if(NULL == pixmap ||
       NULL == m_pMP3Widget ||
       NULL == m_pActionBlockDataManager)
    {
        return;
    }

    CActionBlockGroupManager *pCurrentSelectedGroup = NULL;
    CActionBlockData *pCurrentSelectedFrameData = m_pMP3Widget->getCurrentSelectedFrameData(&pCurrentSelectedGroup);

    QPainter painter(pixmap);
    double nDisplayEndTime = m_pMP3Widget->GetDisplayEndTime();
    double nDisplayStartTime = m_pMP3Widget->GetDisplayStartTime();
    double xScale = m_pMP3Widget->GetXScale();
    m_fCurStartTime = nDisplayStartTime / m_pMP3Widget->GetTimeScale();
    m_fCurEndTime = nDisplayEndTime / m_pMP3Widget->GetTimeScale();
    SetActionBlockDataType::iterator iterator = m_pActionBlockDataManager->FirstGroupIndex(m_fCurStartTime);
    for(;iterator != m_pActionBlockDataManager->EndIndex(); iterator++)
    {
        CActionBlockGroupManager* pGroupManager = *iterator;
        if(pGroupManager == NULL)
        {
            continue;
        }

        CActionBlockGroupManager *pPreviousMgr = getPreviousGroupMgr(pGroupManager);

        //ActionBlock存储的时间，需要乘以时间粒度才是真正的时间(ms为单位)
        double nStartTime = pGroupManager->GetStartTime() * m_pMP3Widget->GetTimeScale();
        double nEndTime = pGroupManager->GetEndTime() * m_pMP3Widget->GetTimeScale();

        if( (nStartTime <= nDisplayEndTime) && (nEndTime >= nDisplayStartTime) )
        {
            // 绘制动作组范围
            QRect rectGroup(0, 0, 0, pixmap->height());
            rectGroup.setLeft( (nStartTime - nDisplayStartTime) *  xScale );
            if(nEndTime <= nDisplayEndTime)
            {
                rectGroup.setRight( (nEndTime- nDisplayStartTime) * xScale );
            }
            else
            {
                rectGroup.setRight( (nDisplayEndTime + 1) * xScale );
            }

            QPen penGroup(Constants::COLOR_ACTION_FRAME_LINE_DIVEDE, m_pMP3Widget->GetLineThick() * 2);
            // 选中动作组线条颜色
            if (pGroupManager == pCurrentSelectedGroup)
            {
                penGroup.setBrush(Constants::COLOR_ACTION_FRAME_SELECT_FILLED);
            }
            painter.setPen(penGroup);

            painter.drawLine(rectGroup.topLeft(), rectGroup.topRight());//上面一条横线

            // 对于当前选中动作组的下一个动作组，不要绘制左边线，否则会覆盖选中动作组的边界线
            if (pPreviousMgr != pCurrentSelectedGroup)
            {
                painter.drawLine(rectGroup.topLeft(), rectGroup.bottomLeft());//左边一条竖线
            }
//            painter.drawLine(rect.bottomLeft(), rect.bottomRight());//下面
            painter.drawLine(rectGroup.topRight(), rectGroup.bottomRight());//右边
//            painter.drawRect(rectGroup);

            // 绘制动作组中每一动作帧范围
            vector<CActionBlockData*> vecFrameData;
            pGroupManager->GetAllBlockData(vecFrameData);
            for (int i  = 0; i < vecFrameData.size(); i++)
            {
                CActionBlockData *pFrameData = vecFrameData[i];
                if (!pFrameData)
                {
                    continue;
                }

                double dFrameStartTime = pGroupManager->GetActionStartTimeByIndex(i) * m_pMP3Widget->GetTimeScale();
                double dFrameEndTime = dFrameStartTime + pFrameData->GetAllTime() * m_pMP3Widget->GetTimeScale();
                QRect rectFrame(0, 0, 0, pixmap->height());
                rectFrame.setLeft((dFrameStartTime - nDisplayStartTime) *  xScale);
                rectFrame.setRight( (dFrameEndTime - nDisplayStartTime) * xScale);

                 // 最后一根线不用画
                if (i != (vecFrameData.size() - 1))
                {
                    QPen penFrame(Constants::COLOR_ACTION_FRAME_LINE_DIVEDE, m_pMP3Widget->GetLineThick());
                    painter.setPen(penFrame);
                    painter.drawLine(rectFrame.topRight(), rectFrame.bottomRight());
                }

                // 选中帧颜色填充
                if(pFrameData == pCurrentSelectedFrameData)
                {
                    QBrush brush(Constants::COLOR_ACTION_FRAME_SELECT_FILLED);
                    painter.fillRect(rectFrame, brush);
                }
            }
        }
    }
}

void ActionLayer::DrawBlockAction(QPixmap *pixmap)
{
    if(NULL == pixmap ||
       NULL == m_pMP3Widget ||
       NULL == m_pActionBlockDataManager)
    {
        return;
    }
    //获取
    int nCol = m_pMP3Widget->GetCurrentSelectCol();
    int nGroupIndex = 0;//动作组索引
    int nActionIndex = 0;//组中动作索引
    int nOffsetTime = 0;//时间线
    GetSelectGroupAction(nCol, nGroupIndex, nActionIndex, nOffsetTime);
    if(nGroupIndex == -1 || nActionIndex == -1)
    {
        return;
    }

    CActionBlockGroupManager* pGroupManager = m_pActionBlockDataManager->GetAt(nGroupIndex);
    if(NULL == pGroupManager)
    {
        return;
    }

    double dActionEndTime = pGroupManager->GetStartTime();
    CActionBlockData* pBlockData = NULL;
    for(int i = 0; i <= nActionIndex; i++)
    {
        pBlockData = pGroupManager->GetAt(i);
        dActionEndTime += pBlockData->GetAllTime();
    }
    pBlockData = pGroupManager->GetAt(nActionIndex);

    double dActionStartTime = dActionEndTime - pBlockData->GetAllTime();
    //ActionBlock中存储的时间需要乘以时间粒度，才是真实ms时间
    dActionEndTime = dActionEndTime * m_pMP3Widget->GetTimeScale();
    dActionStartTime = dActionStartTime * m_pMP3Widget->GetTimeScale();
    QRect rect;
    rect.setLeft( (dActionStartTime - m_pMP3Widget->GetDisplayStartTime()) * m_pMP3Widget->GetXScale() );
    rect.setRight( (dActionEndTime - m_pMP3Widget->GetDisplayStartTime()) * m_pMP3Widget->GetXScale() );
    rect.setTop(1);
    rect.setBottom(pixmap->height() - 1);
    QPainter painter(pixmap);
    QBrush brush(Constants::COLOR_ACTION_BLOCK_SELECT);
    painter.fillRect(rect, brush);

    QRect rectFont;
    rectFont.setTop(0);
    rectFont.setLeft( (pGroupManager->GetStartTime() * m_pMP3Widget->GetTimeScale() - m_pMP3Widget->GetDisplayStartTime()) * m_pMP3Widget->GetXScale() );
    rectFont.setRight( (pGroupManager->GetEndTime() * m_pMP3Widget->GetTimeScale()- m_pMP3Widget->GetDisplayStartTime()) * m_pMP3Widget->GetXScale() );
    rectFont.setBottom(rect.bottom());
    QFont font;
    font.setPixelSize(FONT_PIXEL_SIZE_14);
    painter.setFont(font);
    QString strDescInfo = WCHARToQString(pGroupManager->GetDescription(),MAX_DESCRIPTION_SIZE*sizeof(WCHAR));
    painter.setBackgroundMode(Qt::TransparentMode);//绘制文字设置背景透明
    painter.setPen(Constants::COLOR_TIME_FONT);
    painter.drawText(rectFont, Qt::TextWrapAnywhere, " " + strDescInfo);
}

CActionBlockGroupManager *ActionLayer::getPreviousGroupMgr(CActionBlockGroupManager *pGroupMgr)
{
    if (!pGroupMgr || !m_pActionBlockDataManager)
    {
        return NULL;
    }

    int nCurIndex = m_pActionBlockDataManager->getIndex(pGroupMgr);
    if (nCurIndex <= 0)
    {
        return NULL;
    }

    return m_pActionBlockDataManager->GetAt(nCurIndex - 1);
}

void ActionLayer::GetSelectGroupAction(int nCol, int &nGroupIndex, int &nActionIndex, int &nTimeOffset)
{
    CActionBlockGroupManager* pManager = NULL;
    bool bCursorInGroup = false;
    if(m_pActionBlockDataManager)
    {
        SetActionBlockDataType::const_iterator iterStart = m_pActionBlockDataManager->FirstGroupIndex(m_fCurStartTime);
        //从显示中的索引开始查找，不显示的即使被选中也无法绘制出来
        for(; iterStart != m_pActionBlockDataManager->EndIndex(); iterStart++)
        {
            pManager = *iterStart;
            if(pManager)
            {
                float fGroupEndTime = pManager->GetEndTime();
                if(fGroupEndTime > nCol)
                {
                    nGroupIndex = m_pActionBlockDataManager->GetIndexByID(pManager->GetID());
                    bCursorInGroup = true;
                    break;
                }
            }
        }
    }

    if(!bCursorInGroup)
    {
        nGroupIndex = -1;
        nActionIndex = -1;
        nTimeOffset = -1;
        return;
    }
    CActionBlockData* pActionData = NULL;
    float fTimeSum = 0;
    if(pManager)
    {
        fTimeSum = pManager->GetStartTime();
        for(int i = 0; i < pManager->GetActionCount(); i++)
        {
            pActionData = pManager->GetAt(i);
            if(pActionData)
            {
                float fActionEndTime = pActionData->GetAllTime();
                fTimeSum += fActionEndTime;
                if(fTimeSum > nCol)
                {
                    nActionIndex = i;
                    nTimeOffset = nCol - fTimeSum + fActionEndTime;
                    break;
                }
            }
        }
    }
}

bool ActionLayer::IsSelectGroupBlock(int nID)
{
    return m_listGroupSelectID.contains(nID);
}

