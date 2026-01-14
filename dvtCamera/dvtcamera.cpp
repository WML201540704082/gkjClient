#include "dvtcamera.h"

#include <QDebug>

#pragma execution_character_set("utf-8")

DvtCamera::DvtCamera(QObject *parent)
{
    
}

DvtCamera::~DvtCamera()
{
    uniniCamera();
}

void DvtCamera::iniCamera()
{
    qDebug()<<"m_Handle: "<<m_Handle;
    int ret = DVTLDCam_Init(&m_Handle);
    qDebug()<<"m_Handle: "<<m_Handle;
    qDebug()<<"ret = "<<ret;
    if(ret != 0)
    {
        qDebug()<<"init fail";
        m_Handle = NULL;
    }
    else
    {
        qDebug()<<"init success";       
        emit sendCamMessage("相机初始化成功");
    }
    return;
}

void DvtCamera::uniniCamera()
{
    qDebug()<<"m_Handle: "<<m_Handle;
    int ret = DVTLDCam_UnInit(m_Handle);
    qDebug()<<"m_Handle: "<<m_Handle;
    qDebug()<<"ret = "<<ret;
    if(ret != 0)
    {
        qDebug()<<"uninit failed";
    }
    else
    {
        qDebug()<<"uninit successfully";
    }
}

int WINAPI SDKEventCallBackFun(int event_id, void* context, int iFrameStatus)
{
    DvtCamera* cam = (DvtCamera*)context;
    if (event_id == EVENT_TIMEOUT){
        //检测超时
        qDebug()<<"检测超时，请点击按钮重试！";
        emit cam->sendCamMessage("检测超时，请点击按钮重试！");
    }
    else if (event_id == EVENT_OK){
        //检测成功
        cam->detectSuccess();
    }
    return 0;
}

void DvtCamera::openCamera(HWND m_hWnd, int nX, int nY, int nVideoWindowWidth, int nVideoWindowHeight)
{
    if(m_Handle == NULL)
    {
        emit sendCamMessage("相机未初始化");
        qDebug()<<"相机未初始化";     
        return;
    }

    int nRet = DVTLDCam_Open(m_Handle);
    if(nRet != 0)
    {
        qDebug()<<"open camera failed, nRet = "<<nRet;
        emit sendCamMessage("打开相机失败");
        return;
    }
    else
    {
        qDebug()<<"open camera successfully";
    }
    nRet = DVTLDCam_StartVideo(m_Handle, m_hWnd, nX, nY, nVideoWindowWidth, nVideoWindowHeight);
    if (nRet != 0)
    {
        qDebug()<<"预览失败 nRet = "<< nRet;
        emit sendCamMessage("相机预览失败");
        return;
    }

    //设置
    DVTLDCam_SetEventCallback(m_Handle, SDKEventCallBackFun, this);
}

void DvtCamera::closeCamera()
{
    if(this == NULL|| m_Handle == NULL)
    {
        qDebug()<<"未开启预览";
        return;
    }
    DVTLDCam_StopVideo(m_Handle);
    DVTLDCam_Close(m_Handle);
    qDebug()<<"关闭预览";
}

void DvtCamera::startFaceDetaction()
{
    if (m_Handle == NULL){
        qDebug()<<"未开启预览";
        return;
    }
    qDebug()<<"开始人脸检测";
//    DVTLDCam_StartFaceCapture(m_Handle);
    DVTLDCam_StartDetection(m_Handle);
}

void DvtCamera::stopFaceDetaction()
{
    
}

void DvtCamera::facialComparision(unsigned char *imgData1, int data1Len, unsigned char *imgData2, int data2Len, float *fSimilarity)
{
    if (m_Handle == NULL)
    {
        qDebug()<<"先打开相机";
        return;
    }
    
    int nRet = LD_RET_OK;
    
    nRet = DVTLDCam_FaceCompFeatureEX(m_Handle, IMG_JPG, imgData1, data1Len, imgData2, data2Len, fSimilarity);
    if(nRet == LD_RET_OK)
    {
        qDebug()<<"相似度："<<*fSimilarity;
    }
    else
    {
        qDebug()<<"error: "<<nRet;
    }
}

void DvtCamera::detectSuccess()
{
    qDebug()<<"活体检测完成";

    int ret = 0;
    int faceW, faceH = 0;
    unsigned char*faceData = NULL;
    unsigned long faceDataLen = 0;
    ret = DVTLDCam_GetFaceImage(m_Handle, IMG_JPG, &faceData, &faceW, &faceH, &faceDataLen);
    if(ret == 0)
    {
        emit sendFaceData(faceData, faceDataLen);
    }
}

void DvtCamera::freeData(unsigned char *data)
{
    if (data != NULL)
    {
        DVTLDCam_FreeData(data);
    }
}





