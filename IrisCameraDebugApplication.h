#pragma once

#include <QMainWindow>
#include"ui_IrisCameraDebugApplication.h"
#include"ImageDisplayThread.h"
#include"ImageGetThread.h"


class IrisCameraDebugApplicationWindow : public QMainWindow
{
    Q_OBJECT
public:
    IrisCameraDebugApplicationWindow(QWidget* parent = nullptr);
    ~IrisCameraDebugApplicationWindow();

private slots:
    void displayResult();//ÐÅºÅ»Øµ÷
private:
    Ui::IrisCameraDebugApplicationClass* ui;

    //thread manager class
    ImageDisplayThread* thread_display_image = nullptr;
    ImageGetThread* thread_get_image = nullptr;

    //camera control class
    CameraInterface* ICamera = nullptr;

    //window resources
    unsigned int height = 1280;
    unsigned int width = 1024;
    unsigned char* raw_data = nullptr;

    void Init();
    void Binding();

    //button command
    bool openCameraCommand();
    void startAcquireCommand();
    void stopAcquireCommand();
    void closeCameraCommand();
};