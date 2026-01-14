#include "DVTGKLDCamSDK.h"
#include"stdafx.h"
#include "DVTBusManger.h"

DVTBusManger *g_DVTBusManger = NULL;

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_Init(){
	if (g_DVTBusManger!=NULL)
	{
		delete g_DVTBusManger;
		g_DVTBusManger = NULL;
	}
	g_DVTBusManger = new DVTBusManger();
	int ret = g_DVTBusManger->LDCam_InitSDK();
	return ret;
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_UnInit(){
	if (g_DVTBusManger != NULL)
	{
		delete g_DVTBusManger;
		g_DVTBusManger = NULL;
	}
	return 0;
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_Open(HANDLE *phCam){
	if (g_DVTBusManger == NULL){
		return LD_RET_ERROR_NOT_INIT;
	}
	int ret = g_DVTBusManger->LDCam_Open();
	if (ret == 0){
		*phCam = (HANDLE)g_DVTBusManger;
	}
	return ret;
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_Close(HANDLE hCam){
	DVTBusManger *pLDCam;
	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;
	pLDCam->LDCam_Close();

	return LD_RET_OK;
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_SetEventCallback(HANDLE hCam, LDEventCallBackFun pCallback, PVOID context){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_SetEventCallback(pCallback, context);
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_SetFrameCallback(HANDLE hCam, int nCamIndex, LDFrameCallBackFun pCallback, PVOID context){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_SetFrameCallback(pCallback, context);

}


DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StartVideo(HANDLE hCam, HWND hParentWnd, int nX, int nY, int nVideoWindowWidth, int nVideoWindowHeight){
	DVTBusManger *pLDCam;
	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_StartVideo(hParentWnd, nX,  nY,  nVideoWindowWidth,  nVideoWindowHeight);
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StopVideo(HANDLE hCam){
	DVTBusManger *pLDCam;
	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_StopVideo();
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StartTrack(HANDLE hCam){
	DVTBusManger *pLDCam;
	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;
	return pLDCam->LDCam_StartTrack();
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StopTrack(HANDLE hCam){
	DVTBusManger *pLDCam;
	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;
	return pLDCam->LDCam_StopTrack();
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StartDetection(HANDLE hCam){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_StartDetect(0, 15000);
}
DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StopDetection(HANDLE hCam){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_StopDetect();
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StartFaceCapture(HANDLE hCam){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_StartRec();
}
DVTGKLDCAMSDK_API int STDCALL DVTLDCam_StopFaceCapture(HANDLE hCam){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_StopRec();
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_FreeData(unsigned char* &pBuff){
	if (pBuff != NULL&&pBuff){
		delete[] pBuff;
		pBuff = NULL;
	}
	return 0;
}


DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetImage(HANDLE hCam, int nImageType, unsigned char** pBuff, unsigned long *pRetLen){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_GetImage(nImageType,   pBuff, pRetLen);
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetFaceImage(HANDLE hCam, int nImageType, unsigned char** pBuff, int *pFaceWidth, int *pFaceHeight, unsigned long *pRetLen){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_GetFaceImage(nImageType, pBuff, pFaceWidth, pFaceHeight, pRetLen);

}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_Snapshot(HANDLE hCam, int nImageType, unsigned char* pBuff, int nBuffLen, int *pRetLen){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_Snapshot(nImageType, pBuff, nBuffLen, pRetLen);
}


DVTGKLDCAMSDK_API int STDCALL DVTLDCam_FaceCompFeature(HANDLE hCam, const char* szFileName1, const char* szFileName2, int height1, int width1, int height2, int width2, float *pfSimilarity){

	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_FaceCompFeature(szFileName1, szFileName2, height1, width1, height2, width2, pfSimilarity);

}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_FaceCompFeatureEX(HANDLE hCam, int imgType, unsigned char* imgData1, int data1Len, unsigned char* imgData2, int data2Len, float *pfSimilarity){

	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_FaceCompFeatureEX(imgType, imgData1, data1Len, imgData2, data2Len, pfSimilarity);

}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_FaceCompFeatureByFs(HANDLE hCam, float*fs1, float*fs2, int len, float* pfSimilarity){
	DVTBusManger *pLDCam;

	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;

	return pLDCam->LDCam_FaceCompByFeature(fs1, fs2, len, pfSimilarity);
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetFaceFeatureByData(HANDLE hCam, int imgType, unsigned char* imgData, int dataLen, float*pfFeature, int pfLen, int *featureLen)
{
	DVTBusManger *pLDCam;
	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;
	return pLDCam->LDCam_GetFaceFeatureByData(imgType, imgData,  dataLen, pfFeature,  pfLen,  featureLen);
}
DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetFaceFeatureByPath(HANDLE hCam, const char* szFileName, float*pfFeature, int pfLen, int *featureLen){
	DVTBusManger *pLDCam;
	if (hCam == NULL || hCam == INVALID_HANDLE_VALUE)
		return LD_RET_INVALID_PARAMETER;

	pLDCam = (DVTBusManger *)hCam;
	return pLDCam->LDCam_GetFaceFeatureByPath(szFileName,pfFeature, pfLen, featureLen);
}

DVTGKLDCAMSDK_API int STDCALL DVTLDCam_GetVersion(char *pBuff, int nBuffLen){
	 CString strData = _T("DSTD.1.0.0.0");
	if ((strData.GetLength() + 1) > nBuffLen)
		return LD_RET_BUFF_TOO_SMALL;

	// 获取CString的内部缓冲区
	int len = strData.GetLength();
	char* charStr = strData.GetBuffer(len);
	memcpy(pBuff, charStr,len);
	// 释放缓冲区
	strData.ReleaseBuffer();
	return LD_RET_OK;
}