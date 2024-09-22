#pragma once
#include <QThread>
#include <QObject>
#include"IrisCameraControl.h"

class ImageGetThread :public QThread
{
	Q_OBJECT
public:
	ImageGetThread(CameraInterface* camera);
	~ImageGetThread();
	virtual void run();
private:
	CameraInterface* ICamera = nullptr;
signals:
	//void returnResultImageGetThread();//пе╨е
};