#include "IrisCameraControl.h"
#include<stdio.h>

IrisCamera::IrisCamera(unsigned int m_height, unsigned int m_width)
{
	height = m_height;
	width = m_width;
	USBDevice = new CCyUSBDevice;
	raw_data = new unsigned char[height * width]();
}

IrisCamera::~IrisCamera()
{
	if (raw_data != nullptr) { delete[] raw_data; raw_data = nullptr; }
}

bool IrisCamera::openCamera()
{
	bLooping = false;
	//  发送帧同步命令
	PUCHAR ctrlBuf = new UCHAR;
	LONG length = 0;
	ept = USBDevice->ControlEndPt;                                  //  USB
	if (ept == nullptr)return false;
	ept->Target = TGT_DEVICE;
	ept->ReqType = REQ_VENDOR;
	ept->Direction = DIR_TO_DEVICE;
	ept->ReqCode = 0xa9;
	ept->Value = 0;
	ept->Index = 0;
	ept->XferData(ctrlBuf, length);
	// 打开USB
	//printf("%d\n", USBDevice->DeviceCount());
	USBDevice->Open(0);  //打开0号USB设备
	//printf("%s\n", USBDevice->DeviceName);
	//printf("%d\n", USBDevice->VendorID);
	//printf("%d\n", USBDevice->ProductID);

	int eptCount = USBDevice->EndPointCount();
	for (int i = 1; i < eptCount; i++)
	{
		bool bIn = USBDevice->EndPoints[i]->bIn;
		bool bBulk = (USBDevice->EndPoints[i]->Attributes == 2);
		if (bBulk && bIn) InEndpt = (CCyBulkEndPoint*)USBDevice->EndPoints[i];
	}
	if (USBDevice->IsOpen())
	{
		//打开成功再申请内存，考虑到调试可能改变图像大小。或者将这块逻辑写到图像大小变动所触发的事件中
		if (raw_data == nullptr)raw_data = new unsigned char[height * width]();
		bLooping = true;
		return true;
	}
	return false;
}

void IrisCamera::startAcquire()
{
    LONG length = 0;
    PUCHAR ctrlBuf = new UCHAR;
    long len = height * width;                                                 //  1280*1024;
    int QueueSize = QUEUE_SIZE;
    int i = 0;


    InEndpt->TimeOut = 100;
    InEndpt->SetXferSize(len);

    //  Allocate the arrays needed for queueing
    PUCHAR* buffers = new PUCHAR[QueueSize];
    PUCHAR* contexts = new PUCHAR[QueueSize];
    OVERLAPPED  inOvLap[64];

    //  Allocate all the buffers for the queues
    for (i = 0; i < QueueSize; i++)
    {
        buffers[i] = new UCHAR[len];
        inOvLap[i].hEvent = CreateEvent(NULL, false, false, TEXT("CYUSB_IN"));
        memset(buffers[i], 0, len);
    }

    ept = USBDevice->ControlEndPt;                                 //  USB
    ept->Target = TGT_DEVICE;
    ept->ReqType = REQ_VENDOR;
    ept->Direction = DIR_TO_DEVICE;
    ept->ReqCode = 0xa8;
    ept->Value = 0;
    ept->Index = 0;
    ept->XferData(ctrlBuf, length);

    //  Queue-up the first batch of transfer requests
    for (i = 0; i < QueueSize; i++)
    {
        contexts[i] = InEndpt->BeginDataXfer(buffers[i], len, &inOvLap[i]);
        if (InEndpt->NtStatus || InEndpt->UsbdStatus)         //  BeginDataXfer failed
        {
            printf("USB error0!");
            AbortXferLoop(i + 1, true, buffers, contexts, inOvLap);
            return;
        }
    }

    i = 0;
    for (; bLooping;)		//循环
    {
        long rLen = len;	// Reset this each time through because
        // FinishDataXfer may modify it

        if (!InEndpt->WaitForXfer(&inOvLap[i], 2000))
        {
            InEndpt->Abort();
            printf("USB error!");
            if (InEndpt->LastError == ERROR_IO_PENDING)
                WaitForSingleObject(inOvLap[i].hEvent, 2000);
        }

        if (InEndpt->FinishDataXfer(buffers[i], rLen, &inOvLap[i], contexts[i]))
        {
            m_rawindex += rLen;
            if (m_rawindex == len)
            {
                memcpy(raw_data, buffers[i], sizeof(unsigned char) * len);
                isOnDataReceived = true;
                m_rawindex = 0;
            }
        }
        else
        {
            bLooping = false;
            printf("USB error2!");
            return;
        }

        // Re-submit this queue element to keep the queue full
        contexts[i] = InEndpt->BeginDataXfer(buffers[i], rLen, &inOvLap[i]);
        if (InEndpt->NtStatus || InEndpt->UsbdStatus)         //  BeginDataXfer failed
        {
            AbortXferLoop(QUEUE_SIZE, true, buffers, contexts, inOvLap);
            return;
        }

        i++;

        if (i == 64)                                             //  Only update the display once each time through the Queue
        {
            i = 0;
        }
    }
}

void IrisCamera::stopAcquire()
{
    bLooping = false;
    isOnDataReceived = false;
}

void IrisCamera::closeCamera()
{
    USBDevice->Close();
    if (raw_data != nullptr) { delete[] raw_data; raw_data = nullptr; }
}

bool IrisCamera::ReadBuffer(unsigned char* buffer)
{
    if (buffer == nullptr)return false;
    if (raw_data == nullptr)return false;
    memcpy(buffer, raw_data, sizeof(unsigned char) * height * width);
    //if (isOnDataReceived)memcpy(buffer, raw_data, sizeof(unsigned char) * height * width);
    //isOnDataReceived = false;
    return true;
}

void IrisCamera::AbortXferLoop(int pending, bool opt, PUCHAR* buffers, PUCHAR* contexts, OVERLAPPED inOvLap[])
{
	//InEndpt->Reset();
	InEndpt->Abort();

	if (opt)
	{
		for (int j = 0; j < QUEUE_SIZE; j++)
		{
			if (j < pending)
			{
				if (!InEndpt->WaitForXfer(&inOvLap[j], 2000))
				{
					InEndpt->Abort();
					if (InEndpt->LastError == ERROR_IO_PENDING)
						WaitForSingleObject(inOvLap[j].hEvent, 2000);
				}
				long bytes_xfer = height * width;
				InEndpt->FinishDataXfer(buffers[j], bytes_xfer, &inOvLap[j], contexts[j]);
			}

			CloseHandle(inOvLap[j].hEvent);
			delete[] buffers[j];
		}
	}
	else
	{
		for (int j = 0; j < QUEUE_SIZE; j++)
		{

			CloseHandle(inOvLap[j].hEvent);
			delete[] buffers[j];
		}
	}

	delete[] buffers;
	delete[] contexts;

	bLooping = false;
}
