//
//  MyUSBDevice.cpp
//  4 //
//  Created by Jojo on 12-6-7.
//  Copyright (c) 2012 Mars Company LTD. All rights reserved.
//


#include "MyUSBDevice.h"
#include <IOKit/IOBSD.h>
#include <QDir>
#include <DiskArbitration/DASession.h>
#include <DiskArbitration/DADisk.h>

SInt32 MyUSBDevice::refCount = 0;
MyUSBDevice* MyUSBDevice::uniqueObj = NULL;

MyUSBDevice* MyUSBDevice::Allocate()
{
    if (uniqueObj == NULL) {
        uniqueObj = new MyUSBDevice();
    }
    refCount++;
    return uniqueObj;
}

void MyUSBDevice::Release(UsbHandle *dev)
{
    vector<UsbHandle *>::size_type i;
    vector<UsbHandle *>::iterator iter = devVector.begin();
    for (i = 0; i < devVector.size(); i++, iter++)
    {
        UsbHandle *tmp = devVector.at(i);
        if (dev->dev == tmp->dev)
        {
            (*(tmp->interface))->USBInterfaceClose(tmp->interface);
            (*(tmp->interface))->Release(tmp->interface);
            (*(tmp->dev))->USBDeviceClose(tmp->dev);
            (*(tmp->dev))->Release(tmp->dev);
            delete tmp;
            devVector.erase(iter);
            break;
        }
    }

    refCount--;
    if (refCount == 0)
    {
        delete uniqueObj;
        uniqueObj = NULL;
    }
}

void MyUSBDevice::Release()
{
    vector<UsbHandle *>::iterator iter = devVector.begin();
    while (devVector.size() > 0)
    {
        UsbHandle *tmp = devVector.at(0);

        (*(tmp->interface))->USBInterfaceClose(tmp->interface);
        (*(tmp->interface))->Release(tmp->interface);
        (*(tmp->dev))->USBDeviceClose(tmp->dev);
        (*(tmp->dev))->Release(tmp->dev);
        delete tmp;
        iter = devVector.erase(iter);
    }

    delete uniqueObj;
    uniqueObj = NULL;
}

MyUSBDevice::MyUSBDevice()
{
    devVector.clear();
}

MyUSBDevice::~MyUSBDevice()
{
    vector<UsbHandle *>::size_type i;
    for (i = 0; i < devVector.size(); i++) {
        UsbHandle *tmp = devVector.at(i);
        (*(tmp->interface))->USBInterfaceClose(tmp->interface);
        (*(tmp->interface))->Release(tmp->interface);
        (*(tmp->dev))->USBDeviceClose(tmp->dev);
        (*(tmp->dev))->Release(tmp->dev);
        delete tmp;
    }
    devVector.clear();

    VecDeviceDataType::iterator itor = m_vecDeviceData.begin();
    while(itor != m_vecDeviceData.end())
    {
        DEVICEDATA* pDev = *itor;
        itor = m_vecDeviceData.erase(itor);
        delete pDev;
        pDev = NULL;
    }
}

//根据vid和pid获取设备卷路径
QString MyUSBDevice::GetDeviceVolumePath(int nVID, int nPID)
{
    QString sRet;
    VecDeviceDataType::iterator itor = m_vecDeviceData.begin();
    for(; itor != m_vecDeviceData.end(); itor++)
    {
        DEVICEDATA* pDev = *itor;
        if(pDev->nVID == nVID && pDev->nPID == nPID)
        {
            sRet = pDev->sVolumePath;
            break;
        }
    }
    return sRet;
}

//根据vid和pid获取设备bsd名称
QString MyUSBDevice::GetDeviceBSDName(int nVID, int nPID)
{
    QString sRet;
    VecDeviceDataType::iterator itor = m_vecDeviceData.begin();
    for(; itor != m_vecDeviceData.end(); itor++)
    {
        DEVICEDATA* pDev = *itor;
        if(pDev->nVID == nVID && pDev->nPID == nPID)
        {
            sRet = pDev->sBSDName;
            break;
        }
    }
    return sRet;
}

//根据vid和pid获取设备bsd路径
QString MyUSBDevice::GetDeviceBSDPath(int nVID, int nPID)
{
    QString sRet;
    VecDeviceDataType::iterator itor = m_vecDeviceData.begin();
    for(; itor != m_vecDeviceData.end(); itor++)
    {
        DEVICEDATA* pDev = *itor;
        if(pDev->nVID == nVID && pDev->nPID == nPID)
        {
            sRet = "/dev/" + pDev->sBSDName;
            break;
        }
    }
    return sRet;
}

IOReturn MyUSBDevice::initialize()
{
    mach_port_t                masterPort;
    CFMutableDictionaryRef    matchingDict;
    kern_return_t            kr;
    //SInt32                    usbVendor = nVID;
    //SInt32                    usbProduct = nPID;
    io_iterator_t            iter;


    kr = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if(kr != kIOReturnSuccess || !masterPort) {
        printf("Unable to create a master I/O Kit port (%08x)\n", kr);
        return kIOReturnAborted;
    }

    //Set up matching dictionary for class IOUSBDevice and its subclass
    matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
    if(!matchingDict) {
        printf("Unable to create a USB matching dictionary (%08x)\n", kr);
        return kIOReturnAborted;
    }

#if 0
    //Add the vendor and product IDs to the matching dictionary
    CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorName),
                         CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbVendor));
    CFDictionarySetValue(matchingDict, CFSTR(kUSBProductName),
                         CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &usbProduct));
#endif
    //Look up registered IOService objects that match a matching dictionary
    kr = IOServiceGetMatchingServices(masterPort, matchingDict, &iter);
    if (kr != kIOReturnSuccess) {
        mach_port_deallocate(mach_task_self(), masterPort);
        return kr;
    }

    //Get the usb device interface
    kr = GetUSBInterface(iter);
    if (kr != kIOReturnSuccess) {
        mach_port_deallocate(mach_task_self(), masterPort);
        IOObjectRelease(iter);
        return kr;
    }

    IOObjectRelease(iter);
    mach_port_deallocate(mach_task_self(), masterPort);

    return kIOReturnSuccess;
}

IOReturn MyUSBDevice::GetUSBInterface(io_iterator_t iterator)
{
    kern_return_t            kr;
    io_service_t            usbDevice;
    IOCFPlugInInterface        **plugInInterface = NULL;
    IOUSBDeviceInterface    **dev = NULL;
    HRESULT                    result;
    SInt32                    score;
    UInt16                    vendor;
    UInt16                    product;
    UInt16                    release;
    UInt32                    locationID;

    while((usbDevice = IOIteratorNext(iterator)))
    {
        //Create an intermediate plug-in
        kr = IOCreatePlugInInterfaceForService(usbDevice, kIOUSBDeviceUserClientTypeID,
                                               kIOCFPlugInInterfaceID, &plugInInterface, &score);

        CFStringRef bsdName = (CFStringRef)IORegistryEntrySearchCFProperty(
                    usbDevice,kIOServicePlane,CFSTR(kIOBSDNameKey), kCFAllocatorDefault, kIORegistryIterateRecursively);

        QString sDiskVolumePath; //磁盘卷路径
        QString sBSDName;
        if (bsdName)
        {
            QByteArray buffer(1024, 0);
            if (::CFStringGetCString(CFStringRef(bsdName), buffer.data(), buffer.size(), kCFStringEncodingUTF8))
                sBSDName = QString::fromUtf8(buffer);
            ::CFRelease(bsdName);

            DASessionRef session = DASessionCreate ( kCFAllocatorDefault );
            DADiskRef targetDisk = DADiskCreateFromBSDName(kCFAllocatorDefault, session, sBSDName.toStdString().c_str());
            CFDictionaryRef targetDiskDic = DADiskCopyDescription(targetDisk);

            const QString sVolumeRoot = "/Volumes";
            QDir dir(sVolumeRoot);
            dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
            QStringList driverList = dir.entryList();
            foreach (QString driverName, driverList)
            {
                QString sTempDriverPath = sVolumeRoot + "/" + driverName;
                CFStringRef tempDrvPathRef = NULL;
                CFURLRef tempDrvUrlRef = NULL;
                DADiskRef tmpDisk = NULL;
                CFDictionaryRef tmpDiskDic = NULL;
                do
                {
                    tempDrvPathRef = CFStringCreateWithCString(kCFAllocatorDefault, sTempDriverPath.toStdString().c_str(), kCFStringEncodingUTF8);
                    if(!tempDrvPathRef)
                        break;

                    tempDrvPathRef = CFURLCreateStringByAddingPercentEscapes(NULL, tempDrvPathRef, NULL, NULL, kCFStringEncodingUTF8);

                    tempDrvUrlRef = CFURLCreateWithString(kCFAllocatorDefault, tempDrvPathRef, NULL);
                    if(!tempDrvUrlRef)
                        break;

                    tmpDisk = DADiskCreateFromVolumePath(kCFAllocatorDefault, session, tempDrvUrlRef);
                    if(!tmpDisk)
                        break;

                    tmpDiskDic = DADiskCopyDescription(tmpDisk);
                    if(!tmpDiskDic)
                        break;

                    const void *vendor = CFDictionaryGetValue(tmpDiskDic, kDADiskDescriptionDeviceVendorKey);
                    if(!vendor)
                        break;

                    if( CFDictionaryContainsValue(targetDiskDic, vendor))
                    {
                        //NSLog(@'匹配成功');
                        sDiskVolumePath = sTempDriverPath;
                        break;
                    }

                }while(0);

                if(tempDrvPathRef)
                {
                    ::CFRelease(tempDrvPathRef);
                    tempDrvPathRef = NULL;
                }

                if(tempDrvUrlRef)
                {
                    ::CFRelease(tempDrvUrlRef);
                    tempDrvUrlRef = NULL;
                }

                if(tmpDisk)
                {
                    ::CFRelease(tmpDisk);
                    tmpDisk = NULL;
                }

                if(tmpDiskDic)
                {
                    ::CFRelease(tmpDiskDic);
                    tmpDiskDic = NULL;
                }

                if(!sDiskVolumePath.isEmpty())
                    break;
            }

        }

        //Don't need the device object after intermediate plug-in is created
        kr = IOObjectRelease(usbDevice);
        if(kr != kIOReturnSuccess || !plugInInterface) {
            printf("Unable to create a plug-in (%08x)\n", kr);
            continue;
        }

        //Now create the device interface
        result = (*plugInInterface)->QueryInterface(plugInInterface,
                        CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID *)&dev);
        //Don't need the device object after intermediate plug-in is created
        IODestroyPlugInInterface(plugInInterface);
        if(result || !dev) {
            printf("Unable to create a device interface (%08x)\n",
                   (int)result);
            continue;
        }
        if (IsDeviceExist(dev) == true) {
            (*dev)->Release(dev);
            dev = NULL;
            continue;
        }

        //Check these values for confirmation
        kr = (*dev)->GetDeviceVendor(dev, &vendor);
        kr = (*dev)->GetDeviceProduct(dev, &product);
        kr = (*dev)->GetDeviceReleaseNumber(dev, &release);
        kr = (*dev)->GetLocationID(dev, &locationID);

        if(!sDiskVolumePath.isEmpty())
        {
            DEVICEDATA* pNewDev = new DEVICEDATA;
            pNewDev->nVID = vendor;
            pNewDev->nPID = product;
            pNewDev->sVolumePath = sDiskVolumePath;
            pNewDev->sBSDName = sBSDName;
            m_vecDeviceData.push_back(pNewDev);
        }
#if 0
        if(vendor != kOurVendorID || product != kOurProductID)
        {
            printf("Found unwanted device (vendor %d, product %d)\n",
                   vendor, product);
            (void) (*dev)->Release(dev);
            dev = NULL;
            continue;
        }
        else
        {
            printf("Found allwinner usb device (vendor %d, product %d), location id %08x\n",
                   vendor, product, locationID);
            //Open the device to change its state
            kr = (*dev)->USBDeviceOpen(dev);
            if(kr != kIOReturnSuccess) {
                printf("Unable to open usb device: %08x\n", kr);
                (void) (*dev)->Release(dev);
                dev = NULL;
                continue;
            }

            //configure device
            UInt8                numConfig;
            IOUSBConfigurationDescriptorPtr configDesc;

            //Get the number of configurations.
            kr = (*dev)->GetNumberOfConfigurations(dev, &numConfig);
            if(numConfig == 0)
                continue;

            //Get the configuration descriptor for index 0
            kr = (*dev)->GetConfigurationDescriptorPtr(dev, 0, &configDesc);
            if(kr)
            {
                printf("Unable to get configuration descriptor for index 0 (err = %08x)\n", kr);
                continue;
            }
#if 0
            //Set the device's configuration. The configuration value is found in
            //the bConfigurationValue field of the configuration descriptor
            kr = (*dev)->SetConfiguration(dev, configDesc->bConfigurationValue);
            if(kr) {
                printf("Unable to set configuration to value %d (err = %08x)\n",
                       0, kr);
                continue;
            }
            kr = FindUSBInterface(dev);
            if (kr != kIOReturnSuccess) {
                (*dev)->USBDeviceClose(dev);
                (*dev)->Release(dev);
                dev = NULL;
            }
#endif

            (*dev)->USBDeviceClose(dev);
            (void) (*dev)->Release(dev);
            dev = NULL;
        }
#endif
    }
    return kr;
}

IOReturn MyUSBDevice::FindUSBInterface(IOUSBDeviceInterface **dev)
{
    IOReturn                    kr;
    IOUSBFindInterfaceRequest    request;
    io_iterator_t                iterator;
    io_service_t                usbInterface;
    IOCFPlugInInterface            **plugInInterface = NULL;
    IOUSBInterfaceInterface        **interface = NULL;
    HRESULT                        result;
    SInt32                        score;
    UInt8                        interfaceNumEndpoints;
    UInt8                        pipeRef;
    UInt16                        maxPacketSize = 0;
    UInt8                        pipeIn = 0xff;
    UInt8                        pipeOut = 0xff;
    UInt16                        maxPacketSizeIn = 0;
    UInt16                        maxPacketSizeOut = 0;

    //Iterate all usb interface
    request.bInterfaceClass = kIOUSBFindInterfaceDontCare;
    request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
    request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
    request.bAlternateSetting = kIOUSBFindInterfaceDontCare;

    //Get an iterator for the interfaces on the device
    kr = (*dev)->CreateInterfaceIterator(dev, &request, &iterator);
    if(kr != kIOReturnSuccess) {
        printf("Unable to CreateInterfaceIterator %08x\n", kr);
        return kr;
    }

    while((usbInterface = IOIteratorNext(iterator))) {
        pipeIn = 0xff;
        pipeOut = 0xff;
        //Create a intermediate plug-in
        kr = IOCreatePlugInInterfaceForService(usbInterface,
                                               kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID,
                                               &plugInInterface, &score);
        //Release the usbInterface object after getting the plug-in
        kr = IOObjectRelease(usbInterface);
        if(kr != kIOReturnSuccess || !plugInInterface) {
            printf("Unable to create a plug-in (%08x)\n", kr);
            break;
        }

        //Now create the device interface for the device interface
        result = (*plugInInterface)->QueryInterface(plugInInterface,
                            CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID), (LPVOID *)&interface);
        IODestroyPlugInInterface(plugInInterface);
        if(result || !interface) {
            printf("Unable to create a interface for the device interface %08x\n",
                   (int)result);
            break;
        }

        //Now open the interface.This will cause the pipes associated with
        //the endpoints in the interface descriptor to be instantiated
        kr = (*interface)->USBInterfaceOpen(interface);
        if(kr != kIOReturnSuccess) {
            printf("Unable to open interface for the device interface %08x\n", kr);
            (void) (*interface)->Release(interface);
            interface = NULL;
            break;
        }

        //Get the number of endpoints associated with this interface
        kr = (*interface)->GetNumEndpoints(interface, &interfaceNumEndpoints);
        if(kr != kIOReturnSuccess) {
            printf("Unable to get the number of endpoints %08x\n", kr);
            (void) (*interface)->USBInterfaceClose(interface);
            (void) (*interface)->Release(interface);
            interface = NULL;
            break;
        }

        //Access each pipe in turn, starting with the pipe at index 1
        //The pipe at index 0 is the default control pipe and should be
        //accessed using (*usbDevice)->DeviceRequest() instead
        for(pipeRef = 1; pipeRef <= interfaceNumEndpoints; pipeRef++) {
            IOReturn     kr2;
            UInt8        direction;
            UInt8        number;
            UInt8        transferType;
            UInt8        interval;

            kr2 = (*interface)->GetPipeProperties(interface, pipeRef, &direction,
                                                  &number, &transferType, &maxPacketSize, &interval);
            if(kr2 != kIOReturnSuccess) {
                printf("Unable to get properties of pipe %d (%08x)\n",
                       pipeRef, kr2);
            } else {
                if(transferType == kUSBBulk) {
                    if(direction == kUSBIn) {
                        pipeIn = pipeRef;
                        maxPacketSizeIn = maxPacketSize;
                    }
                    else if(direction == kUSBOut) {
                        pipeOut = pipeRef;
                        maxPacketSizeOut = maxPacketSize;
                    }
                }
            }
        }

        if (pipeIn != 0xff && pipeOut != 0xff) {
            UsbHandle *tmp = new UsbHandle;
            tmp->dev = dev;
            tmp->interface = interface;
            tmp->pipeIn = pipeIn;
            tmp->pipeOut = pipeOut;
            tmp->maxPacketSizeIn = maxPacketSizeIn;
            tmp->maxPacketSizeOut = maxPacketSizeOut;
            (*dev)->GetLocationID(dev, &(tmp->locationID));
            devVector.push_back(tmp);

            printf("Found %lu devices\n", devVector.size());

            return kIOReturnSuccess;
        }
        (*interface)->USBInterfaceClose(interface);
        (*interface)->Release(interface);
        interface = NULL;
    }

    return kr;
}

bool MyUSBDevice::IsDeviceExist(IOUSBDeviceInterface **dev)
{
    if (!dev) {
        return false;
    }

    UInt32 locationID;
    kern_return_t kr;
    kr = (*dev)->GetLocationID(dev, &locationID);
    if(kr != kIOReturnSuccess) {
        printf("GetLocationID failed\n");
        return false;
    }
    vector<UsbHandle *>::iterator iter = devVector.begin();
    for (; iter != devVector.end(); iter++) {
        UsbHandle *tmp = *iter;
        if (tmp->locationID == locationID) {
            return true;
        }
    }

    return false;
}

IOReturn MyUSBDevice::WriteSync(UsbHandle *dev, void *buff, u32 size)
{
    if (dev && dev->interface) {
        if(size <= dev->maxPacketSizeOut) {
            u32 writeLen;
            return WriteAsync(dev, buff, size, &writeLen);
        }
        kern_return_t kr;
        char *tmp = (char *)buff;
        u32 nWrite = (size > dev->maxPacketSizeOut ? dev->maxPacketSizeOut : size);
        u32 nLeft = size;

        while(1) {
            if((int)nLeft <= 0) {
                break;
            }
            kr = (*(dev->interface))->WritePipe(dev->interface,
                    dev->pipeOut, (void *)tmp, nWrite);
            if(kr != kIOReturnSuccess)
                break;
            tmp += nWrite;
            nLeft -= nWrite;
            nWrite = (nLeft > dev->maxPacketSizeOut ? dev->maxPacketSizeOut : nLeft);
        }
        return kr;
    }

    return kIOReturnNoDevice;
}

IOReturn MyUSBDevice::WriteAsync(UsbHandle *dev, void *buff, u32 size, u32 *pWrite)
{
    if (dev == NULL || dev->interface == NULL) {
        return kIOReturnNoDevice;
    }

    IOReturn                        err;
    CFRunLoopSourceRef        cfSource;

    err = (*(dev->interface))->CreateInterfaceAsyncEventSource(dev->interface, &cfSource);
    if (err)
    {
        printf("transferData: unable to create event source, err = %08x\n", err);
        return err;
    }
    CFRunLoopAddSource(CFRunLoopGetCurrent(), cfSource, kCFRunLoopDefaultMode);

    err = (*(dev->interface))->WritePipeAsync(dev->interface, dev->pipeOut, (void *)buff, size,
                                         (IOAsyncCallback1)OperationCallBack, (void*)pWrite);
    if (err != kIOReturnSuccess)
    {
        printf("transferData: WritePipeAsyncFailed, err = %08x\n", err);
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(), cfSource, kCFRunLoopDefaultMode);
        *pWrite = 0;
        return err;
    }

    CFRunLoopRun();
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(), cfSource, kCFRunLoopDefaultMode);

    return err;
}

IOReturn MyUSBDevice::ReadSync(UsbHandle *dev, void *buff, u32 *pSize)
{
    if (dev && dev->interface) {
        if(*pSize <= dev->maxPacketSizeIn)
            return ReadAsync(dev, buff, *pSize, pSize);
        kern_return_t kr;
        UInt32 nRead = dev->maxPacketSizeIn;
        u32 nLeft = *pSize;
        char *tmp = (char *)buff;

        while(1) {
            if((int)nLeft <= 0)
                break;

            kr = (*(dev->interface))->ReadPipe(dev->interface,
                    dev->pipeIn, (void *)tmp, &nRead);
            if(kr != kIOReturnSuccess) {
                printf("transferData: Readsync Failed, err = %08x\n", kr);
                break;
            }

            tmp += nRead;
            nLeft -= nRead;
            nRead = dev->maxPacketSizeIn;
        }

        int nRet = ((int)nLeft > 0 ? nLeft : 0);
        *pSize = *pSize - nRet;
        return kr;
    }
    return kIOReturnNoDevice;
}

IOReturn MyUSBDevice::ReadAsync(UsbHandle *dev, void *buff, u32 size, u32 *pRead)
{
    if (dev == NULL || dev->interface == NULL) {
        return kIOReturnNoDevice;
    }

    IOReturn                err;
    CFRunLoopSourceRef        cfSource;

    err = (*(dev->interface))->CreateInterfaceAsyncEventSource(dev->interface, &cfSource);
    if (err)
    {
        printf("transferData: unable to create event source, err = %08x\n", err);
        return err;
    }
    CFRunLoopAddSource(CFRunLoopGetCurrent(), cfSource, kCFRunLoopDefaultMode);

    err = (*(dev->interface))->ReadPipeAsync(dev->interface, dev->pipeIn, buff, size,
                                         (IOAsyncCallback1)OperationCallBack, (void*)pRead);
    if (err != kIOReturnSuccess)
    {
        printf("transferData: size %u, ReadAsyncFailed, err = %08x\n", size, err);
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(), cfSource, kCFRunLoopDefaultMode);
        *pRead = 0;
        return err;
    }

    CFRunLoopRun();
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(), cfSource, kCFRunLoopDefaultMode);

    return err;
}

IOReturn MyUSBDevice::DeviceRequestAsync(UsbHandle *dev, UInt8 cmd, UInt16 deviceAddress,
                                             UInt16 length, UInt8 *Buffer)
{
    if (dev->dev) {
        IOReturn                        err;
        CFRunLoopSourceRef        cfSource;
        IOUSBDevRequest    request;

        err = (*(dev->dev))->CreateDeviceAsyncEventSource(dev->dev, &cfSource);
        if (err)
        {
            printf("transferData: unable to create event source, err = %08x\n", err);
            return err;
        }
        CFRunLoopAddSource(CFRunLoopGetCurrent(), cfSource, kCFRunLoopDefaultMode);

        request.bmRequestType = USBmakebmRequestType(kUSBOut, kUSBVendor, kUSBDevice);
        request.bRequest = cmd;
        request.wValue = deviceAddress;
        request.wIndex = 0;                //the default control pipe
        request.wLength = length;
        request.pData = Buffer;

        err = (*(dev->dev))->DeviceRequest(dev->dev, &request);
        if (err != kIOReturnSuccess)
        {
            printf("transferData: WritePipeAsyncFailed, err = %08x\n", err);
            CFRunLoopRemoveSource(CFRunLoopGetCurrent(), cfSource, kCFRunLoopDefaultMode);
            return err;
        }

        CFRunLoopRun();
        CFRunLoopRemoveSource(CFRunLoopGetCurrent(), cfSource, kCFRunLoopDefaultMode);
        return err;
    }

    return kIOReturnNoDevice;
}

IOReturn MyUSBDevice::DeviceRequestSync(UsbHandle *dev, UInt8 cmd, UInt16 deviceAddress,
                                            UInt16 length, UInt8 *Buffer)
{
    if (dev->dev) {

        IOUSBDevRequest        request;
        request.bmRequestType = USBmakebmRequestType(kUSBOut, kUSBVendor, kUSBDevice);
        request.bRequest = cmd;
        request.wValue = deviceAddress;
        request.wIndex = 0;                //the default control pipe
        request.wLength = length;
        request.pData = Buffer;

        return (*(dev->dev))->DeviceRequestAsync(dev->dev, &request,
                (IOAsyncCallback1)OperationCallBack, (void *)cmd);
    }

    return kIOReturnNoDevice;
}

HANDLE MyUSBDevice::GetDeviceHandle(u32 nIndex)
{
    if (devVector.empty()) {
        printf("vector empty\n");
        return NULL;
    }
    vector<UsbHandle *>::size_type i;
    for (i = 0; i < devVector.size(); i++) {
        UsbHandle *tmp = devVector.at(i);
        if (tmp->locationID == nIndex) {
            return (HANDLE)tmp;
        }
    }
    return NULL;
}

void MyUSBDevice::OperationCallBack(void *refcon, IOReturn result, void *arg0)
{
    if (result == kIOReturnSuccess && refcon) {
        u32 *pLen = (u32 *)refcon;
        *pLen = reinterpret_cast<long long>(arg0);
    }

    CFRunLoopStop(CFRunLoopGetCurrent());
}

bool GetDevIndex(PCHAR devName, u32 &nIndex)
{
    if (devName == NULL) {
        return false;
    }
    PCHAR tmp = devName;
    nIndex = strtoul(tmp, NULL, 16);

    return true;
}

#if 0
//Open USB Device
u32 TEST_Open(HANDLE *phDevice, PCHAR devName)
{
    u32 devIndex = 0;
    kern_return_t                    kr;

    printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    if (phDevice == NULL) {
        return __LINE__;
    }

//    if (false == GetDevIndex(devName, devIndex)) {
//        printf("GetDevIndex Failed\n");
//        return __LINE__;
//    }

    MyUSBDevice *pDev = MyUSBDevice::Allocate();
    kr = pDev->initialize();
    if(kr != kIOReturnSuccess) {
        printf("initialize device failed: %08x", kr);
        return __LINE__;
    }

    //*phDevice = pDev->GetDeviceHandle(devIndex);

    return 0;
}

//Close USB Device
u32 TEST_Close(HANDLE *hDevice)
{
    MyUSBDevice *pDev = MyUSBDevice::GetInstance();

    if (pDev && hDevice && *hDevice) {
        pDev->Release((MyUSBDevice::UsbHandle *)(*hDevice));
        return 0;
    }
    return __LINE__;
}
#endif
u32 TEST_Query(HANDLE *hDevice, void *pToDriver, u32 ToLen, void *pFromDriver, u32 FromLen, u32 &nRead)
{
    return 0;
}

#if 0
u32 TEST_Send(HANDLE *hDevice, u8 *buffer, u32 Len, u32  &nSend)
{
    MyUSBDevice *pDev = MyUSBDevice::GetInstance();

    if (pDev && hDevice && *hDevice) {
        kern_return_t kr;

        kr = pDev->WriteSync((MyUSBDevice::UsbHandle *)(*hDevice), buffer, Len);
        if(kr == kIOReturnSuccess) {
            nSend = Len;
            return 0;
        }
    }
    return __LINE__;
}

u32 TEST_Recv(HANDLE *hDevice, u8 *buffer, u32 Len, u32  &nRecv)
{
    MyUSBDevice *pDev = MyUSBDevice::GetInstance();

    if (pDev && hDevice && *hDevice) {
        kern_return_t kr;
        u32 nRead = Len;

        kr = pDev->ReadSync((MyUSBDevice::UsbHandle *)(*hDevice), buffer, &nRead);
        if(kr == kIOReturnSuccess) {
            nRecv = nRead;
            return 0;
        }

        printf("Read Pipe Error %08x\n", kr);
        return __LINE__;
    }
    return __LINE__;
}

#else
u32 TEST_Send(HANDLE *hDevice, u8 *buffer, u32 Len, u32  &nSend)
{
    MyUSBDevice *pDev = MyUSBDevice::GetInstance();

    if (pDev && hDevice && *hDevice) {
        u32 nWrite = Len;
        kern_return_t kr;

        kr = pDev->WriteAsync((MyUSBDevice::UsbHandle *)(*hDevice), buffer, Len, &nWrite);
        if (kr == kIOReturnSuccess) {
            nSend = nWrite;
            return 0;
        }
    }
    return __LINE__;
}

u32 TEST_Recv(HANDLE *hDevice, u8 *buffer, u32 Len, u32  &nRecv)
{
    MyUSBDevice *pDev = MyUSBDevice::GetInstance();

    memset(buffer, 0, Len);

    if (pDev && hDevice && *hDevice) {
        u32 nRead = Len;
        kern_return_t kr;

        kr = pDev->ReadAsync((MyUSBDevice::UsbHandle *)(*hDevice), buffer, Len, &nRead);
        if (kr == kIOReturnSuccess) {
            nRecv = nRead;
            return 0;
        }
    }

    return __LINE__;
}
#endif
