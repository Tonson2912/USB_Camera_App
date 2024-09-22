#pragma once
#include<Windows.h>
#include"CyAPI.h"

class CameraInterface
{
public:
    virtual bool openCamera() = 0;
    virtual void startAcquire() = 0;
    virtual void stopAcquire() = 0;
    virtual void closeCamera() = 0;
    virtual bool ReadBuffer(unsigned char* buffer) = 0;
    virtual void SetOnDataReceivedSignal(bool signal = false) = 0;
    virtual bool GetOnDataReceivedSignal() = 0;
};

class IrisCamera :public CameraInterface
{
public:
    IrisCamera(unsigned int m_height, unsigned int m_width);
    ~IrisCamera();
    virtual bool openCamera();
    virtual void startAcquire();
    virtual void stopAcquire();
    virtual void closeCamera();
    virtual bool ReadBuffer(unsigned char* buffer);
    virtual void SetOnDataReceivedSignal(bool signal = false) { isOnDataReceived = signal; }
    virtual bool GetOnDataReceivedSignal() { return isOnDataReceived; }
    
private:
    CCyUSBEndPoint* InEndpt = nullptr;
    CCyUSBDevice* USBDevice = nullptr;
    CCyControlEndPoint* ept = nullptr;

    unsigned int height = 1280;
    unsigned int width = 1024;
    unsigned char* raw_data = nullptr;
    bool bLooping = false;
    unsigned long m_rawindex = 0;
    static const int QUEUE_SIZE = 64;
    bool isOnDataReceived = false;

    void AbortXferLoop(int pending, bool opt, PUCHAR* buffers, PUCHAR* contexts, OVERLAPPED inOvLap[]);
};