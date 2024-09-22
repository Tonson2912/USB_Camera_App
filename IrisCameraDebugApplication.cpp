/********************************************************************************
** Form generated from reading UI file 'IrisCameraDebugApplication.ui'
**
** Created by: Qt User Interface Compiler version 6.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#include"IrisCameraDebugApplication.h"
static const int QUEUE_SIZE = 64;

IrisCameraDebugApplicationWindow::IrisCameraDebugApplicationWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::IrisCameraDebugApplicationClass)
{
	ui->setupUi(this);
	//���غ����Դ�������ڴ�ռ�
	Init();
	//���¼�
	Binding();
}

IrisCameraDebugApplicationWindow::~IrisCameraDebugApplicationWindow()
{
	delete ui;
	if (ICamera != nullptr) { delete ICamera; ICamera = nullptr; }
	if (thread_display_image != nullptr) { delete thread_display_image; thread_display_image = nullptr; }
	if (thread_get_image != nullptr) { delete thread_get_image; thread_get_image = nullptr; }
	if (raw_data != nullptr) { delete[] raw_data; raw_data = nullptr; }
}

void IrisCameraDebugApplicationWindow::Init()
{
	if (ICamera == nullptr)ICamera = new IrisCamera(height, width);
	if (raw_data == nullptr)raw_data = new unsigned char[height * width]();
	// ���߳��¼�
	if (thread_display_image == nullptr)thread_display_image = new ImageDisplayThread(ICamera, raw_data);
	if (thread_get_image == nullptr)thread_get_image = new ImageGetThread(ICamera);
}

void IrisCameraDebugApplicationWindow::Binding()
{
	// ���̻߳ص��¼�
	connect(thread_display_image, SIGNAL(mess()), this, SLOT(displayResult()));
	// �󶨰�ť�¼�
	connect(ui->openCameraButton, &QPushButton::clicked, this, &IrisCameraDebugApplicationWindow::openCameraCommand);
	connect(ui->startAcquireButton, &QPushButton::clicked, this, &IrisCameraDebugApplicationWindow::startAcquireCommand);
	connect(ui->stopAcquireButton, &QPushButton::clicked, this, &IrisCameraDebugApplicationWindow::stopAcquireCommand);
	connect(ui->closeCameraButton, &QPushButton::clicked, this, &IrisCameraDebugApplicationWindow::closeCameraCommand);
}

bool IrisCameraDebugApplicationWindow::openCameraCommand()
{
    return ICamera->openCamera();
}

void IrisCameraDebugApplicationWindow::startAcquireCommand()
{
	thread_get_image->start();
	thread_display_image->start();
}

void IrisCameraDebugApplicationWindow::stopAcquireCommand()
{
	ICamera->stopAcquire();
}

void IrisCameraDebugApplicationWindow::closeCameraCommand()
{
	ICamera->closeCamera();
}



void IrisCameraDebugApplicationWindow::displayResult()
{
	QImage tmpImg(raw_data, height, width, QImage::Format_Indexed8);
	ui->label->setPixmap(QPixmap::fromImage(tmpImg));//��ʾͼ��Label
}