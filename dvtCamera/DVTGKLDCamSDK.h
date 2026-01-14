#pragma once

//#include"stdafx.h"
#include "windows.h"

#ifndef _DVTGKLDCAMSDK_H_
#define _DVTGKLDCAMSDK_H_

#define DVTGKLDCAMSDK_API __declspec(dllexport)
#define STDCALL _stdcall 

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/

	//-------------------------------------------------------------------------------------------------------
	//Video format type definitions
	typedef enum
	{
		LD_FMT_UNKOWN,
		LD_FMT_YUY2,
		LD_FMT_MJPG,
		LD_FMT_YUYV,
		LD_FMT_UYVY,
		LD_FMT_I420,
		LD_FMT_RGB24,
		LD_FMT_RGB32
	}LD_FMT;
	//========================================================================================================



	//--------------------------------------------------------------------------------------------------
	//Image Type definitions
	typedef enum
	{
		IMG_RGB24 = 0,            //RGB24 image
		IMG_JPG,				  //JPG image
		IMG_RGB24_BASE64,         //RGB24 image with base64 encoded
		IMG_JPG_BASE64			  //JPG image with base64 encoded
	}LD_IMG_TYPE;

	//---------------------------------------------------------------------------------------------------------
	//Compare data Type  (3:jpg)(2:png)(1:bmp)(0:rgb24)
	typedef enum
	{
		CMP_RGB24 = 0,
		CMP_BMP,
		CMP_PNG,
		CMP_JPG

	}LD_CMP_TYPE;
	//API return definitions

#define LD_RET_OK						0
#define LD_RET_NOCAM					-1
#define LD_RET_NO_VISCAM				-2
#define LD_RET_NO_NIRCAM				-3
#define LD_RET_ERROR_SET_FORMAT			-4
#define LD_RET_ERROR_START_CAM			-5
#define LD_RET_OPEND_ALREADY			-6
#define LD_RET_ERROR_OPEN				-8
#define LD_RET_NO_MEMORY				-9
#define LD_RET_INVALID_PARAMETER		-10
#define LD_RET_VIDEO_NOT_STARTED		-11
#define LD_RET_DETECT_STARTED_ALREADY	-12
#define LD_RET_TIMEOUT					-13
#define LD_RET_NO_DATA					-14
#define LD_RET_BUFF_TOO_SMALL			-15
#define LD_RET_ERROR					-16
#define LD_RET_ERROR_START_DETECT		-17
#define LD_RET_ERROR_ENGINE				-18
#define LD_RET_ERROR_CONFIG_FILE		-19
#define LD_RET_ERROR_NOT_INIT			-20
#define LD_RET_ERROR_NOT_SUPPORT		-21
	//============================================================================================================





	//---------------------------------------------------------------------------------------------------------------
	//Event ID and event callback definition
#define EVENT_ONGOING	     0        // detection ongoing
#define EVENT_OK			 1        // all OK
#define EVENT_TIMEOUT		-1		  // detection timeout with no qualified picture.
#define EVENT_CALLCED		-2		  // user canceled	


	// 活体检测捕获时状态iFrameStatus的值
#define LD_CAM_STATUS_CAPTURED				0							// 捕获到质量合格的照片
#define LD_CAM_STATUS_NO_FACE				1							// 没有检测到人脸
#define LD_CAM_STATUS_FACE_BEYOND_MASK		2							// 人脸超出蒙层
#define LD_CAM_STATUS_ANGLE				    3						    // 人脸角度不对、属于低头， 抬头， 左右转情况
#define LD_CAM_STATUS_TOO_BIG				4							// 人脸太大
#define LD_CAM_STATUS_TOO_SMALL			    5							// 人脸太小
#define LD_CAM_STATUS_NEAR_BOUNDARY			6							// 人脸靠近边界
#define LD_CAM_STATUS_FOREHEAD_COVERED		7						    // 额头被遮挡
#define LD_CAM_STATUS_EYE_COVERED			8							// 眼睛被遮挡
#define LD_CAM_STATUS_MOUTH_COVERED			9							// 嘴巴被遮挡
#define LD_CAM_STATUS_FACE_MOVED			10							// 人脸质量不合格
#define LD_CAM_LIVENESS_FAILURE			    11							// 非活体
#define LD_CAM_STATUS_MULTI_FACE			12							// 检测到多人脸
#define  ITEM_CNT                           13	


	typedef int(WINAPI *LDEventCallBackFun)(int event_id, void* context, int iFrameStatus);
	/*
	event_id:  refer to above definitions
	context:   user context  set in DVTGKLDCam_SetEventCallback,user can use this in the callback.
	*/

	//============================================================================================================


	//----------------------------------------------------------------------------------------------
	//Video frame arrival callback function definition, image format is RGB24
	typedef int(WINAPI *LDFrameCallBackFun)(void* context, unsigned char* pFrameData, int nFrameLen,int w,int h);
	/*
	context:     context  set in DVTLDCam_SetFrameCallback, user can use this in the callback.
	pFrameData:  frame data buffer
	nFrameLen :  frame length

	*/
	//======================================================================================================================
	/*
	Function:   DVTLDCam_Init(HANDLE *phCam);
	Description:  Init sdk. First Api to call to initial sdk environment, Call one time only
	Parameters:

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_Init(HANDLE *phCam);
	//=======================================================================================================================


	/*
	Function:   DVTLDCam_UnInit();
	Description:  UnInit camera.   Last Api to call to release sdk environment.
	Parameters:

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_UnInit(HANDLE &hCam);
	//================================================================================
	/*
	Function:   DVTLDCam_Open;
	Description:  Open the live-detect camera.
	Parameters:
	phCam -- receive the handle of live-detect camera
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_Open(HANDLE hCam);
	//================================================================================


	/*
	Function:   DVTLDCam_Close;
	Description:  Close the live-detect camera.
	Parameters:
	hCam -- Handle of the opened camera.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_Close(HANDLE hCam);
	//====================================================================================================================
	/*
	Function:   DVTLDCam_SetEventCallback();
	Description:  Set callback functions for event incoming . must call this before DVTGKLDCam_StartVideo
	Parameters:
	hCam -- Handle of the opened camera.
	pCallback  -- callback function.
	context -- user context used in the callback function

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_SetEventCallback(HANDLE hCam, LDEventCallBackFun pCallback, PVOID context);
	//====================================================================================================================
	/*
	Function:   DVTLDCam_SetFrameCallback();
	Description:  Set callback functions for frame arrival . must call this before DVTGKLDCam_StartVideo
	Parameters:
	hCam -- Handle of the opened camera.
	nCamIndex -- 0: visual camera, 1: near ir camera
	pCallback  -- callback function.
	context -- user context used in the callback function

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_SetFrameCallback(HANDLE hCam, LDFrameCallBackFun pCallback, PVOID context);
	//====================================================================================================================

	/*
	Function:   DVTLDCam_StartVideo();
	Description:  Start video, detect not started yet.
	Parameters:
	hCam -- Handle of the opened camera.
	hParentWnd  -- Parent window of the video window. if not want to show view ,set it NULL
	nX -- left start position of video window in parent window
	nY -- up start position of video window in parent window
	nVideoWindowWidth -- Video window width.  do not need to be same with the format set in DVTGKLDCam_SetVisCamFormat.
	nVideoWindowHeight -- video window height. do not need to be same with the format set in DVTGKLDCam_SetVisCamFormat.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StartVideo(HANDLE hCam, HWND hParentWnd, int nX, int nY, int nVideoWindowWidth, int nVideoWindowHeight);
	//==========================================================================================================================================
	/*
	Function:   DVTGKLDCam_StopVideo();
	Description:  Stop the video.
	Parameters:
	hCam -- Handle of the opened camera.
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StopVideo(HANDLE hCam);
	//==========================================================================================================================================
	/*
	Function:   DVTLDCam_StartTrack()/DVTLDCam_StopTrack;
	Description:  start/stop  face  track.
	Parameters:
	hCam -- Handle of the opened camera.
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StartTrack(HANDLE hCam);
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StopTrack(HANDLE hCam);

	/*
	Function:   DVTLDCam_StartDetection()/DVTLDCam_StopDetection;
	Description:  Start/stop live body detection.
	Parameters:
	hCam -- Handle of the opened camera.
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StartDetection(HANDLE hCam);
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StopDetection(HANDLE hCam);
	//==========================================================================================================================================

	/*
	Function:   DVTLDCam_StartFaceCapture()/DVTLDCam_StopFaceCapture;
	Description:  start/stop face detection.
	Parameters:
	hCam -- Handle of the opened camera.

	return:
	0 -- No error
	others-- Error
	*/
	//==========================================================================================================================================
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StartFaceCapture(HANDLE hCam);
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StopFaceCapture(HANDLE hCam);
	//==========================================================================================================================================

	/*
	Function:   DVTLDCam_FreeData();
	Description: free data.
	Parameters:
	pBuff -- data .

	return:
	0 -- No error
	others-- Error
	*/
	//==========================================================================================================================================
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_FreeData(unsigned char* &pBuff);

	/*
	Function:   DVTLDCam_GetImage();
	Description:  Get normal image after event returned EVENT_OK, 3 image types supported, refer to LD_IMG_TYPE .
	Parameters:
	hCam -- Handle of the opened camera.
	nImageType -- image type, refer to LD_IMG_TYPE.
	pBuff -- buffer to receive image data, recommended buffer length is width*height*5.
	nBuffLen -- length of buffer
	pRetLen -- returned data length.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetImage(HANDLE hCam, int nImageType, unsigned char** pBuff, unsigned long *pRetLen);
	//==========================================================================================================================================
	/*
	Function:   DVTLDCam_GetFaceImage();
	Description:  Get face image after live-body detected sucessfully.
	Parameters:
	hCam -- Handle of the opened camera.
	nImageType -- image type, refer to LD_IMG_TYPE.
	pBuff -- buffer to receive image data, recommended buffer length is width*height*4.
	nBuffLen -- length of buffer
	pFaceWidth    -- returned face width.
	pFaceHeight   -- returned face height.
	pRetLen  -- returned data length.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetFaceImage(HANDLE hCam, int nImageType, unsigned char** pBuff, int *pFaceWidth, int *pFaceHeight, unsigned long *pRetLen);
	/*
	Function:   DVTLDCam_Snapshot();
	Description:  Do snapshot, 3 image types supported, refer to LD_IMG_TYPE .

	Parameters:
	hCam -- Handle of the opened camera.
	nImageType -- image type, refer to LD_IMG_TYPE.
	pBuff -- buffer to receive image data, recommended buffer length is width*height*5.
	nBuffLen -- length of buffer
	nBuffLen -- length of buffer
	#
	pRetLen -- returned data length.

	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_Snapshot(HANDLE hCam, int nImageType, unsigned char** pBuff, int *pRetLen);
	//==========================================================================================================================================
	/*
	Function:   DVTLDCam_FaceCompFeature();
	Description:  Get two picture Similarity Degree.
	Parameters:
	hCam -- Handle of the opened camera.
	pfSimilarity -- Similarity Degree.
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_FaceCompFeature(HANDLE hCam, const char* szFileName1, const char* szFileName2, float *pfSimilarity);
	//==========================================================================================================================================
	/*
	Function:   DVTLDCam_FaceCompFeatureEX();
	Description:  Get two picture datat Similarity Degree.
	Parameters:
	hCam -- Handle of the opened camera.
	pfSimilarity -- Similarity Degree.
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_FaceCompFeatureEX(HANDLE hCam, int imgType, unsigned char* imgData1, int data1Len, unsigned char* imgData2, int data2Len, float *pfSimilarity);
	//==========================================================================================================================================
	/*
	Function:   DVTLDCam_FaceCompFeatureByFs();
	Description:  Get two Feature datas Similarity Degree.
	Parameters:
	hCam -- Handle of the opened camera.
	pfSimilarity -- Similarity Degree.
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_FaceCompFeatureByFs(HANDLE hCam, float*fs1, float*fs2, int len, float* pfSimilarity);
	//==========================================================================================================================================
	/*
	Function:   DVTLDCam_GetFaceFeatureByData();
	Description:  Get  Feature from Image Data.
	Parameters:
	hCam -- Handle of the opened camera.
	imgType ---LD_CMP_TYPE
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetFaceFeatureByData(HANDLE hCam, int imgType, unsigned char* imgData, int dataLen, float*pfFeature, int pfLen, int *featureLen);
	//==========================================================================================================================================
	/*
	Function:   DVTLDCam_GetFaceFeatureByPath();
	Description:  Get  Feature from Image path.
	Parameters:
	hCam -- Handle of the opened camera.
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetFaceFeatureByPath(HANDLE hCam, const char* szFileName, float*pfFeature, int pfLen, int *featureLen);
	//==========================================================================================================================================

	/*
	Function:   DVTLDCam_GetVersion();
	Description:  Get version information.
	Parameters:
	pBuff -- buffer to receive data.
	nBuffLen -- length of buffer
	return:
	0 -- No error
	others-- Error
	*/
	DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetVersion(char *pBuff, int nBuffLen);
	//==========================================================================================================================================

#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif

