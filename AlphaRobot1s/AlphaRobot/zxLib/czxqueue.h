/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：czxqueue.h
* 创建时间：2016/10/20
* 文件标识：
* 文件摘要：线程安全的队列
*
* 当前版本：1.0.0.0
* 作    者：hwx
*/

#ifndef CZXQUEUE_H
#define CZXQUEUE_H

#include <QAtomicInt>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

#include "alpharobotlog.h"
#include "zxlibconst.h"

const int SLEEP_INTERNAL = 300;
const int QUEUE_DEFAULT_CAPACITY = 100;

template <class T>
class CZXQueue
{
public:
    explicit CZXQueue(int nCapacity = QUEUE_DEFAULT_CAPACITY);
    ~CZXQueue();

public:

    // 向队列中添加元素，如果队列已满，则丢弃数据
    bool push(const T &element);

    // 向队列中添加元素，如果队列已满，等待默认时间后再次尝试添加，直到超时
    bool syncPush(const T &element, quint64 timeout = INFINITE);

    // 从队列中获取元素，如果队列已空，返回false
    bool pop(T &element);

    // 从队列中获取元素，如果队列已空，则等待指定的时间后再次尝试获取，直到超时
    bool syncPop(T &element, quint64 timeout = INFINITE);

    // 向队列中指定位置插入指定元素，如果队列已满，则丢弃数据
    bool insert(int index, const T&element);

    // 向队列中指定位置插入指定元素，如果队列已满，则等待默认时间后重新尝试插入，直到超时
    bool syncInsert(int index, const T&element, quint64 timeout = INFINITE);

    // 根据队列下标，返回队列原始
    const T at(int index);

    // 获取队列中元素的大小
    int size();

    bool isEmpty();
    bool isFull();

    // 获取队列容量
    int getCapacity();
    void setCapacity(int nCapacity);

    // 清空队列
    void clear();

private:

    QAtomicInt m_nCurrentSize;  // 当前队列中元素数目
    int m_nCapacity; // 队列容量
    QQueue<T> m_queue;

    QMutex m_mutex;
};

template <class T>
CZXQueue<T>::CZXQueue(int nCapacity): m_nCurrentSize(0), m_nCapacity(nCapacity)
{
}

template <class T>
CZXQueue<T>::~CZXQueue()
{

}

template <class T>
bool CZXQueue<T>::push(const T &element)
{
    QMutexLocker locker(&m_mutex);
    if (m_nCurrentSize < m_nCapacity)
    {
        m_queue.enqueue(element);
        m_nCurrentSize++;

        ///////////////////Log///////////////////////
        QString strLog = QString("Push data: current size = %1").arg(m_nCurrentSize);
        AlphaRobotLog::getInstance()->printDebug(strLog);
        ///////////////////Log///////////////////////

        return true;
    }
    else
    {
        return false;
    }
}

template <class T>
bool CZXQueue<T>::syncPush(const T &element, quint64 timeout)
{
    if (timeout != INFINITE)
    {
        do
        {
            if (push(element))
            {
                return true;
            }
            else//没有插入成功,等待一段时间
            {
                if (timeout < SLEEP_INTERNAL)//最后一次了
                {
                    Sleep(timeout);
                    timeout = 0;
                }
                else
                {
                    Sleep(SLEEP_INTERNAL);
                    timeout = timeout - SLEEP_INTERNAL;
                }
            }
        } while (timeout);
    }
    else
    {
        while (true)
        {
            if (push(element))
            {
                return true;
            }
            else
            {
                Sleep(SLEEP_INTERNAL);//要不然就是死循环
            }
        }
    }
    return false;
}

template <class T>
bool CZXQueue<T>::pop(T &element)
{
    QMutexLocker locker(&m_mutex);
    if (m_nCurrentSize <= 0)//队列已经空了,
    {
        return false;
    }
    else
    {
        element = m_queue.dequeue();
        m_nCurrentSize--;

        ///////////////////Log///////////////////////
        QString strLog = QString("Pop data: current size = %1").arg(m_nCurrentSize);
        AlphaRobotLog::getInstance()->printDebug(strLog);
        ///////////////////Log///////////////////////

        return true;
    }
}

template <class T>
bool CZXQueue<T>::syncPop(T &element, quint64 timeout)
{
    if (timeout != INFINITE)
    {
        do
        {
            if (pop(element))
            {
                return true;
            }
            else//没有获取到,等待一段时间
            {
                if (timeout < SLEEP_INTERNAL)//最后一次了
                {
                    Sleep(timeout);
                    timeout = 0;
                }
                else
                {
                    Sleep(SLEEP_INTERNAL);
                    timeout = timeout - SLEEP_INTERNAL;
                }
            }
        } while (timeout);
    }
    else
    {
        while (true)
        {
            if (pop(element))
            {
                return true;
            }
            else
            {
                Sleep(SLEEP_INTERNAL);//要不然就是死循环
            }
        }
    }
    return false;
}

template <class T>
bool CZXQueue<T>::insert(int index, const T &element)
{
    QMutexLocker locker(&m_mutex);
    if (m_nCurrentSize < m_nCapacity)
    {
        m_queue.insert(index, element);
        m_nCurrentSize++;

        ///////////////////Log///////////////////////
        QString strLog = QString("Insert data: current size = %1").arg(m_nCurrentSize);
        AlphaRobotLog::getInstance()->printDebug(strLog);
        ///////////////////Log///////////////////////

        return true;
    }

    return false;
}

template <class T>
bool CZXQueue<T>::syncInsert(int index, const T &element, quint64 timeout)
{
    if (timeout != INFINITE)
    {
        do
        {
            if (insert(index, element))
            {
                return true;
            }
            else//没有插入成功,等待一段时间
            {
                if (timeout < SLEEP_INTERNAL)//最后一次了
                {
                    Sleep(timeout);
                    timeout = 0;
                }
                else
                {
                    Sleep(SLEEP_INTERNAL);
                    timeout = timeout - SLEEP_INTERNAL;
                }
            }
        } while (timeout);
    }
    else
    {
        while (true)
        {
            if (insert(index, element))
            {
                return true;
            }
            else
            {
                Sleep(SLEEP_INTERNAL);//要不然就是死循环
            }
        }
    }

    AlphaRobotLog::getInstance()->printDebug("Insert data timeout");
    return false;
}

template <class T>
const T CZXQueue<T>::at(int index)
{
     QMutexLocker locker(&m_mutex);
     if (index <0 || index >= m_nCurrentSize)
     {
         return NULL;
     }

     return m_queue.at(index);
}

template <class T>
int CZXQueue<T>::size()
{
    return m_nCurrentSize;
}

template <class T>
bool CZXQueue<T>::isEmpty()
{
    if (m_nCurrentSize <= 0)
    {
        return true;
    }
    return false;
}

template <class T>
bool CZXQueue<T>::isFull()
{
    if (m_nCurrentSize >= m_nCapacity)
    {
        return true;
    }

    return false;
}

template <class T>
int CZXQueue<T>::getCapacity()
{
    return m_nCapacity;
}

template <class T>
void CZXQueue<T>::setCapacity(int nCapacity)
{
    m_nCapacity = nCapacity;
}

template <class T>
void CZXQueue<T>::clear()
{
    QMutexLocker locker(&m_mutex);
    m_queue.clear();
    m_nCurrentSize = 0;
}

#endif // CZXQUEUE_H
