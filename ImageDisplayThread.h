#pragma once
#include <QThread>
#include <QObject>
#include"IrisCameraControl.h"

class ImageDisplayThread :public QThread
{
	Q_OBJECT
public:
	ImageDisplayThread(CameraInterface* camera,unsigned char* buffer);
	~ImageDisplayThread();
	virtual void run();
private:
	CameraInterface* ICamera;
	unsigned char* buffer_ptr;
signals:
	void mess();//пе╨е
};