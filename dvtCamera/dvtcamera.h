#ifndef DVTCAMERA_H
#define DVTCAMERA_H

#include "dvtCamera_global.h"
#include "DVTGKLDCamSDK.h"

#include <QObject>

class DVTCAMERA_EXPORT DvtCamera : public QObject
{
    Q_OBJECT
public:
    explicit DvtCamera(QObject *parent = nullptr);

    ~DvtCamera();

    ///
    /// \brief iniCamera 初始化相机
    ///
    void iniCamera();

    ///
    /// \brief uniniCamera 释放相机
    ///
    void uniniCamera();

    ///
    /// \brief openCamera 打开相机
    /// \param m_hWnd 显示图像的窗口的id
    /// \param nX 窗口横向起点
    /// \param nY 窗口纵向起点
    /// \param nVideoWindowWidth 窗口宽度
    /// \param nVideoWindowHeight 窗口高度
    ///
    void openCamera(HWND m_hWnd, int nX, int nY, int nVideoWindowWidth, int nVideoWindowHeight);

    ///
    /// \brief closeCamera 关闭相机
    ///
    void closeCamera();

    ///
    /// \brief startFaceDetaction 开启人脸检测
    ///
    void startFaceDetaction();

    void stopFaceDetaction();
    
    ///
    /// \brief facialComparision 人脸相似度对比
    ///
    void facialComparision(unsigned char* imgData1, int data1Len, unsigned char* imgData2, int data2Len, float *pfSimilarity);

    ///
    /// \brief detectSuccess 人脸检测成功
    ///
    void detectSuccess();
    
    
    ///
    /// \brief freeData 释放数据
    ///
    void freeData(unsigned char* data);

signals:
    void sendCamMessage(QString message);
    void sendFaceData(unsigned char* faceData, unsigned long faceDataLen);

private:
    HANDLE m_Handle = NULL;
};

#endif // DVTCAMERA_H
