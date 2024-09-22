#include "ImageGetThread.h"

ImageGetThread::ImageGetThread(CameraInterface* camera)
{
	ICamera = camera;
}

ImageGetThread::~ImageGetThread()
{
	//this->destroyed();
}

void ImageGetThread::run()
{
	if (ICamera != nullptr) {
		ICamera->startAcquire();
	}
	else
	{
		//发送信号量，提示无相机实例
	}
	//emit returnResultImageGetThread();
}
