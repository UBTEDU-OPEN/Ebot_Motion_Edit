/*
* Copyright (c) 2016, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：SINGLETON.h
* 创建时间：20160808
* 文件标识：
* 文件摘要：单例模式宏
*
* 当前版本：1.0.0.0
* 作    者：asu
* 版本摘要：
*/

#ifndef SINGLETON_H
#define SINGLETON_H



//单例模式宏
#define SINGLETON_INSTANCE(_CLASS_)         \
public:                                     \
    inline static _CLASS_& getInstance()    \
    {                                       \
    static _CLASS_ s_instance;              \
    return s_instance;                      \
    }                                       \


//单例模式宏
#define SINGLETON(_CLASS_)                  \
SINGLETON_INSTANCE(_CLASS_)                 \
private:                                    \
    _CLASS_();                              \
    _CLASS_(_CLASS_ const&){}               \
    ~_CLASS_();                             \


//单例模式默认构造函数与析构函数（配合单例模式宏使用）
#define SINGLETON_C_D(_CLASS_)      \
    _CLASS_::_CLASS_(){}            \
    _CLASS_::~_CLASS_(){}           \


#endif // SINGLETON_H
