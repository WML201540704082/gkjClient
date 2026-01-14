#pragma once

#include <iostream>
#include <mutex>
#include "CameraManger.h"
#include "Config.h"
#include "render\Osd.h"
#include "ALG\xiaoshi\XSManger.h"


class DVTBusManger
{
public:
	DVTBusManger();
	~DVTBusManger();
	//加载配置文件
	void  loadParms();
	//算法授权
	int LDCam_InitSDK();
	//打开相机
	int  LDCam_Open();
	//关闭相机
	int LDCam_Close();
	//开启视频流
	int  LDCam_StartVideo(HWND hParentWnd, int nX, int nY, int nVideoWindowWidth, int nVideoWindowHeight);
	//关闭视频
	int  LDCam_StopVideo();
	//设置事件回调
	int LDCam_SetEventCallback(LDEventCallBackFun pCallback, PVOID context);
	//设置视频回调
	int LDCam_SetFrameCallback(LDFrameCallBackFun pCallback, PVOID context);
	//人脸跟踪
	int  LDCam_StartTrack();
	int  LDCam_StopTrack();
	//人脸检测
	int  LDCam_StartRec();
	int  LDCam_StopRec();
	//活体检测
	int  LDCam_StartDetect(int nDetectMilliSeconds, int nTimeOutMilliSeconds);
	int  LDCam_StopDetect();
	//对外获取图片
	int LDCam_GetImage(int nImageType, unsigned char** pBuff, unsigned long *pRetLen);
	int  LDCam_GetFaceImage(int nImageType, unsigned char** pBuff, int *pWidth, int *pHeight, unsigned long *pRetLen);
	//抓拍
	int LDCam_Snapshot(int nImageType, unsigned char* pBuff, int nBuffLen, int *pRetLen);
	//开启业务处理线程
	BOOL m_ThreadRuning;
	int startThreadForData();
	int stopThreadForData();
	void threadProcessForData();
	//处理数据
	HANDLE  m_FrameCopyEvents[2];
	BOOL  m_RGBCopyed;
	BOOL  m_IRCopyed;
	unsigned char * m_RGBFrameData;
	unsigned char * m_IRFrameData;
	void degreeData(unsigned char *data, unsigned long len);
	void  dealRgbData(unsigned char* data, unsigned long len);
	void  dealIRData(unsigned char* data, unsigned long len);
	void  dealResult(int actionType, int resID, RECT rect, unsigned char *rgbData, unsigned char* irData);
	//检测完成后存储数据,数据格式是rgb
	std::mutex m_DestDataMutex;
	unsigned char *m_DestRgbData;
	unsigned char *m_DestIrData;
	RECT m_DestRect;
	//比对
	int  LDCam_FaceCompFeature(const char* szFileName1, const char* szFileName2, int height1, int width1, int height2, int width2, float *pfSimilarity);
	int LDCam_FaceCompFeatureEX(int imgType, unsigned char* imgData1, int data1Len, unsigned char* imgData2, int data2Len, float *pfSimilarity);
	int LDCam_FaceCompByFeature(float*fs1, float*fs2, int len, float *pfSimilarity);
	int LDCam_GetFaceFeatureByData(int imgType, unsigned char* imgData, int dataLen, float*pfFeature, int pfLen, int *featureLen);
	int LDCam_GetFaceFeatureByPath(const char* szFileName, float*pfFeature, int pfLen, int *featureLen);




public:
	CameraManger *m_CameraManger = NULL;
	Config *m_Config = NULL;
	BOOL m_PreviewIng = false;
private:
	BOOL m_authSuccess = FALSE;
	COsd *m_COsd;
	//小视算法库
	XSManger* m_XSManger;
	RECT m_FaceRect;
	//
	int m_MiniTime = 0;
	long m_TimeoutTime = 0;
	DWORD m_startDetectTime = 0;
	int m_currentAction = 0;
	//事件回调
	LDEventCallBackFun m_LDEventCallBackFun;
	void* m_LDEventContext;
	BOOL  m_isLiveStatus;
	int m_EventID = -99;
	//提示内容
	char  m_detect_text[ITEM_CNT][255];
	int m_resID = 0;
	//拍照
	BOOL m_PhotoCopying;
	unsigned char *m_PhotoData;
	HANDLE m_hSnapshotGotEvent = NULL;
	//模板
	unsigned char * m_MaskBMPData = NULL;
	//视频数据回调
	LDFrameCallBackFun m_LDFrameCallBackFun;
	void*m_LDFrameContext;
	//视频数据的宽高，有可能会要求旋转
	int m_dataWidth;
	int m_dataHeight;
	//模板图片
	unsigned char* m_pBuffBMP = NULL;
	int m_maskW = 640;
	int m_maskH = 480;
	
	
};

