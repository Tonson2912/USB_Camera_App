#include "ImageDisplayThread.h"

ImageDisplayThread::ImageDisplayThread(CameraInterface* camera, unsigned char* buffer)
{
	ICamera = camera;
	buffer_ptr = buffer;
}

ImageDisplayThread::~ImageDisplayThread()
{
	//this->destroyed();
}


void ImageDisplayThread::run()
{
	if (ICamera != nullptr)
	{
		while (true)
		{
			if (ICamera->GetOnDataReceivedSignal())
			{
				ICamera->ReadBuffer(buffer_ptr);
				ICamera->SetOnDataReceivedSignal();
				emit mess();
				msleep(20);
			}
		}
	}
}
