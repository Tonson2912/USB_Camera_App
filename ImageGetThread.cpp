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
		//�����ź�������ʾ�����ʵ��
	}
	//emit returnResultImageGetThread();
}
