#ifndef MYUSBDEVICE
#define MYUSBDEVICE

//
//  MyUSBDevice.h
//  4 //
//  Created by Jojo on 12-6-7.
//  Copyright (c) 2012年 Mars. All rights reserved.
//

#include <vector>
#include <set>

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <mach/mach.h>
#include <QString>

typedef void * HANDLE;
typedef char * PCHAR;
typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

using namespace std;

//设备信息
typedef struct tagDEVICEDATA
{
    int nVID; //vid
    int nPID;  //pid
    QString sVolumePath; //设备卷路径
    QString sBSDName;  //bsd名称

    tagDEVICEDATA()
    {
        nVID = 0;
        nPID = 0;
        sVolumePath = "";
        sBSDName = "";
    }

    tagDEVICEDATA(const tagDEVICEDATA& devobj)
    {
        nVID = devobj.nVID;
        nPID = devobj.nPID;
        sVolumePath = devobj.sVolumePath;
        sBSDName = devobj.sBSDName;
    }

    tagDEVICEDATA& operator = (const tagDEVICEDATA& devobj)
    {
        nVID = devobj.nVID;
        nPID = devobj.nPID;
        sVolumePath = devobj.sVolumePath;
        sBSDName = devobj.sBSDName;
        return *this;
    }
}DEVICEDATA;

typedef vector<DEVICEDATA*>  VecDeviceDataType;

class MyUSBDevice {
private:
    static MyUSBDevice        *uniqueObj;
    static SInt32                refCount;
public:
    typedef struct tag_usbHandle {
        IOUSBDeviceInterface    **dev;
        IOUSBInterfaceInterface    **interface;
        UInt8                    pipeIn;
        UInt8                    pipeOut;
        UInt16                    maxPacketSizeIn;
        UInt16                    maxPacketSizeOut;
        UInt32                    locationID;
    }UsbHandle;
private:
    vector<UsbHandle *> devVector;
public:
    /* The only way to create the class driver instance is the allocate method */
    static MyUSBDevice* Allocate(void);

    /* Get the instance of this class */
    static MyUSBDevice * GetInstance(void) { return uniqueObj; }

    /* Before any operation the instance should have been initialized */
    IOReturn initialize();

    /* Get the UsbHandle pointer at nIndex */
    HANDLE GetDeviceHandle(u32 nIndex);

    /* Release the class if you don't need it anymore */
    void Release(UsbHandle *dev);
    void Release();

    /* Write data into the in bulk pipe synchronous or asynchronous */
    IOReturn WriteSync(UsbHandle *dev, void *buff, u32 size);
    IOReturn WriteAsync(UsbHandle *dev, void *buff, u32 size, u32 *pWrite);

    /* Read data from the out bulk pipe synchronous or asynchronous */
    IOReturn ReadSync(UsbHandle *dev, void *buff, u32 *pSize);
    IOReturn ReadAsync(UsbHandle *dev, void *buff, u32 size, u32 *pRead);

    /*
     Sends a USB request with a command on the default control pipe.
     request command below:

     enum {
     kUSBRqGetStatus     = 0,
     kUSBRqClearFeature  = 1,
     kUSBRqGetState      = 2,
     kUSBRqSetFeature    = 3,
     kUSBRqReserved2     = 4,
     kUSBRqSetAddress    = 5,
     kUSBRqGetDescriptor = 6,
     kUSBRqSetDescriptor = 7,
     kUSBRqGetConfig     = 8,
     kUSBRqSetConfig     = 9,
     kUSBRqGetInterface  = 10,
     kUSBRqSetInterface  = 11,
     kUSBRqSyncFrame     = 12
     };
     */
    IOReturn DeviceRequestSync(UsbHandle *dev, UInt8 cmd , UInt16 deviceAddress,
                           UInt16 length, UInt8 *Buffer);
    IOReturn DeviceRequestAsync(UsbHandle *dev, UInt8 cmd , UInt16 deviceAddress,
                           UInt16 length, UInt8 *Buffer);

    //根据vid和pid获取设备卷路径
    QString GetDeviceVolumePath(int nVID, int nPID);

    //根据vid和pid获取设备bsd名称
    QString GetDeviceBSDName(int nVID, int nPID);

    //根据vid和pid获取设备bsd路径
    QString GetDeviceBSDPath(int nVID, int nPID);

private:
    MyUSBDevice();
    ~MyUSBDevice();
    IOReturn GetUSBInterface(io_iterator_t iterator);
    IOReturn FindUSBInterface(IOUSBDeviceInterface **dev);
    bool IsDeviceExist(IOUSBDeviceInterface **dev);
    static void OperationCallBack(void *refcon, IOReturn result, void *arg0);

    VecDeviceDataType m_vecDeviceData; //设备信息集合
};

#endif // MYUSBDEVICE

