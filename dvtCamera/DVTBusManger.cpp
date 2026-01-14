#include "stdafx.h"
#include "DVTBusManger.h"
#include "Logc.h"
#include "util\ImageTool.h"
#include "Resource.h"
#include <atlimage.h>
//彩色相机视频数据回调
int WINAPI S_VisCamFrameCallback(PVOID context, PUCHAR pFrameData, int nFrameLen){
	DVTBusManger* m = (DVTBusManger*)context;
	m->dealRgbData(pFrameData,nFrameLen);
	return 0;
}

//黑白相机视频回调
int WINAPI S_NirCamFrameCallback(PVOID context, PUCHAR pFrameData, int nFrameLen){
	DVTBusManger* m = (DVTBusManger*)context;
	m->dealIRData(pFrameData, nFrameLen);
	return 0;
}

//检测结果回调
int WINAPI XSResultCallBack(int actionType, int resID, RECT rect, void* context, unsigned char *rgbData, unsigned char* irData){
	DVTBusManger* m = (DVTBusManger*)context;
	m->dealResult(actionType, resID, rect,rgbData,irData);
	return 0;
}


void  DVTBusManger::dealResult(int actionType, int resID, RECT rect,unsigned char *rgbData, unsigned char* irData){
	m_currentAction = actionType;
	m_FaceRect = rect;
	//CString log;
	//log.Format("type:%d resID:%d", actionType, resID);
	//Logc::WriteLog(log);
	m_resID = resID;
	if (actionType == Action_nothing){
		m_isLiveStatus = FALSE;
		m_EventID = -99;
	}
	else if (actionType == Action_Track){
		
		m_isLiveStatus = FALSE;
		m_EventID = -99;
	}
	else if (actionType >=Action_FaceRec){
		//获取当前时间
		DWORD currentTime = GetTickCount();
		//判断是否超时
		if (m_TimeoutTime>0 && (currentTime - m_startDetectTime)>m_TimeoutTime)
		{
			m_XSManger->stopLiveDetect();
			m_EventID = EVENT_TIMEOUT;
			m_isLiveStatus = FALSE;
			goto TODO;
		}
		if (resID == LD_CAM_STATUS_CAPTURED){
			//判断是否达到检测最低时间
			m_isLiveStatus = TRUE;
			int sapce = (currentTime - m_startDetectTime);
			//CString log;
			//log.Format("space:%d m_MiniTime:%d", sapce, m_MiniTime);
			//Logc::WriteLog(log);
			if (sapce >= m_MiniTime)
			{
				m_XSManger->stopLiveDetect();
				m_EventID = EVENT_OK;
				//对数据进行拷贝
				m_DestDataMutex.lock();
				int len = m_dataWidth*m_dataHeight * 3;
				memset(m_DestRgbData,0,len);
				memset(m_DestIrData, 0, len);
				memcpy(m_DestRgbData,rgbData,len);
				memcpy(m_DestIrData, irData, len);
				m_DestRect = rect;
				m_DestDataMutex.unlock();
				goto TODO;
			}
		}
		else{
			m_isLiveStatus = FALSE;
		}
		m_EventID = EVENT_ONGOING;
	}
	
TODO:
	if (m_EventID != -99 && m_LDEventCallBackFun)
	{
		m_LDEventCallBackFun(m_EventID, m_LDEventContext, resID);
	}
	return;
	
	
	
}


void DVTBusManger::degreeData(unsigned char *data, unsigned long len){
	//判断是否需要旋转
	if (m_Config->m_Degree == 90){
		rotateRgb24Degree90(data, m_Config->m_VideoWidth, m_Config->m_VideoHeight);
	}
	else if (m_Config->m_Degree == 270){
		rotateRgb24Degree270(data, m_Config->m_VideoWidth, m_Config->m_VideoHeight);
	}
}

void  DVTBusManger::dealRgbData(unsigned char* data, unsigned long len){
	
	//degreeData(data,len);

	//对数据进行拷贝
	if (!m_RGBCopyed)
	{
		memcpy(m_RGBFrameData,data,len);
		m_RGBCopyed = TRUE;
		SetEvent(m_FrameCopyEvents[0]);
	}
	//执行拍照
	if (m_PhotoCopying)
	{
		m_PhotoCopying = FALSE;
		//memset(m_PhotoData, 0, len);
		memcpy(m_PhotoData,data,len);
		UpsideDownImage(m_PhotoData, m_dataWidth,m_dataHeight, 3);
		SetEvent(m_hSnapshotGotEvent);
	}

	
		//绘制数据,文字、人脸框
		if (m_COsd->CreateMemBitmap(m_dataWidth,m_dataHeight))
		{
			char sInfo[255];
			LOGFONT lf;
			::ZeroMemory(&lf, sizeof(lf));
			lf.lfHeight = 30;
			lf.lfWeight = FW_HEAVY;
			lf.lfQuality = PROOF_QUALITY;
			::lstrcpy(lf.lfFaceName, _T("黑体"));
			HFONT hFontVis = CreateFontIndirect(&lf);

			POINT pt;
			pt.x = 5;
			pt.y = 5;
			if (m_currentAction > 0){

				//只进行人脸追踪
				//正在进行活体检测
				if (m_Config->m_isShowFaceFrame == 1){
					if (m_FaceRect.left >= 0)
					{
						COLORREF recColor = RGB(255, 0, 0);
						if (m_isLiveStatus)
						{
							//绿色
							recColor = RGB(0, 255, 0);
						}
						//m_COsd->DrawRectangle(data, m_FaceRect.left, m_FaceRect.top, m_FaceRect.right, m_FaceRect.bottom, recColor, 3);
						m_COsd->DrawTempRectangle(data, m_dataWidth, m_dataHeight, m_FaceRect.left, m_FaceRect.top, m_FaceRect.right, m_FaceRect.bottom, recColor, 3);
					}
				}
				if (m_Config->m_isText == 1){
					//COLORREF textGreenColor = RGB(0, 255, 0);
					COLORREF textRedColor = RGB(m_Config->textColorR, m_Config->textColorG, m_Config->textColorB);
					if (m_EventID == EVENT_ONGOING){
						if (m_TimeoutTime > 0){
							//倒计时
							long currentTime = GetTickCount();
							int nLeftSeconds = m_TimeoutTime / 1000 - (currentTime - m_startDetectTime) / 1000;
							nLeftSeconds = nLeftSeconds <= 0 ? 0 : nLeftSeconds;
							//sprintf_s(sInfo, sizeof(sInfo), "%s(%ds)", m_detect_text[m_resID], nLeftSeconds);
							sprintf_s(sInfo, sizeof(sInfo), "%s", m_detect_text[m_resID]);

						}
						else{
							sprintf_s(sInfo, sizeof(sInfo), "%s", m_detect_text[m_resID]);
						}
						m_COsd->AddTextOsd(data, sInfo, hFontVis, textRedColor, pt);
					}
				}

			}
			

			if (m_Config->m_displayMask)//模板显示/每一帧添加底图
			{
				if (m_pBuffBMP!=NULL)
				{
					rgb24_to_draw_line(m_pBuffBMP, data, m_maskW, m_maskH);
				}
			}
			//不执行这个会导致卡死，原因不详
			DeleteObject(hFontVis);
	    }

		m_COsd->DestoryOsdObject();
	


	if (m_LDFrameCallBackFun != NULL){
		unsigned char *renderData = new unsigned char[len];
		memcpy(renderData,data,len);
		m_LDFrameCallBackFun(m_LDFrameContext, renderData,len);
		if (renderData){
			delete[] renderData;
			renderData = NULL;
		}
	}

}
void  DVTBusManger::dealIRData(unsigned char* data, unsigned long len){
	//degreeData(data, len);
	//对数据进行拷贝
	if (!m_IRCopyed){
		m_IRCopyed = TRUE;
		memcpy(m_IRFrameData, data, len);
		SetEvent(m_FrameCopyEvents[1]);
	}
}


void DVTBusManger::threadProcessForData(){
	Logc::WriteLog("threadProcessForData start",5);
	while (m_ThreadRuning)
	{
		//重置事件，获取当前的相机数据
		ResetEvent(m_FrameCopyEvents[0]);
		ResetEvent(m_FrameCopyEvents[1]);
		m_RGBCopyed = FALSE;
		m_IRCopyed = FALSE;
		int nIndex = -1;

		//等待双目数据都完成拷贝
		nIndex = WaitForMultipleObjects(2, m_FrameCopyEvents, TRUE, 500);
		if (!m_ThreadRuning)
		{
			break;
		}
		if (nIndex == WAIT_OBJECT_0)
		{
			//rgb和ir都有数据了
			UpsideDownImage(m_RGBFrameData, m_dataWidth,m_dataHeight, 3);
			UpsideDownImage(m_IRFrameData, m_dataWidth,m_dataHeight, 3);
			//送入算法
			m_XSManger->XSDealData(m_RGBFrameData, m_IRFrameData);
		}
		Sleep(50);
	}
	Logc::WriteLog("threadProcessForData stop",5);
}



void UpsideDownRGB32(PUCHAR pData, int nWidth, int nHeight)
{
	int i;
	PUCHAR pBuffLine = new UCHAR[nWidth * 4];
	if (!pBuffLine)
		return;

	for (i = 0; i < nHeight / 2; i++)
	{
		memcpy(pBuffLine, pData + (i*nWidth * 4), nWidth * 4);
		memcpy(pData + (i*nWidth * 4), pData + ((nHeight - 1 - i)*nWidth * 4), nWidth * 4);

		memcpy(pData + ((nHeight - 1 - i)*nWidth * 4), pBuffLine, nWidth * 4);
	}

	delete[]pBuffLine;
}

void UpsideDownRGB24(unsigned char* pData, int nWidth, int nHeight) {
	int i;
	unsigned char* pBuffLine = new unsigned char[nWidth * 3];
	for (i = 0; i < nHeight / 2; i++) {
		// 复制当前行的数据到缓冲区
		memcpy(pBuffLine, pData + (i * nWidth * 3), nWidth * 3);

		// 将最后一行的数据复制到当前行
		memcpy(pData + (i * nWidth * 3), pData + ((nHeight - 1 - i) * nWidth * 3), nWidth * 3);

		// 将缓冲区的数据复制到最后一行
		memcpy(pData + ((nHeight - 1 - i) * nWidth * 3), pBuffLine, nWidth * 3);
	}

	delete[] pBuffLine;
}




int read_file_data2(const char* szFileName, unsigned char*& szImageData, MVInt32& imageLen)
{
	//FILE* pFile = fopen(szFileName, "rb");
	FILE* pFile;
	errno_t err = fopen_s(&pFile, szFileName, "rb");
	if (err != 0){
		return -1;
	}

	if (!pFile)
	{
		return -1;
	}

	char szBuff[1024] = { 0 };
	std::string strRead;
	while (!feof(pFile))
	{
		size_t size = fread(szBuff, 1, sizeof(szBuff) - 1, pFile);
		strRead.append(szBuff, size);
	}
	size_t allocSz = strRead.size();
	szImageData = new MVUInt8[allocSz + 1];
	memcpy(szImageData, strRead.c_str(), allocSz);
	szImageData[allocSz] = '\0';
	imageLen = (MVInt32)allocSz;

	fclose(pFile);

	return 0;
}



int read_fileData(const char* szFileName, unsigned char*& szImageData, int& imageLen)
{
	//FILE* pFile = fopen(szFileName, "rb");
	FILE* pFile;
	errno_t err = fopen_s(&pFile, szFileName, "rb");
	if (err != 0){
		return -1;
	}

	if (!pFile)
	{
		return -1;
	}

	char szBuff[1024] = { 0 };
	std::string strRead;
	while (!feof(pFile))
	{
		size_t size = fread(szBuff, 1, sizeof(szBuff) - 1, pFile);
		strRead.append(szBuff, size);
	}
	size_t allocSz = strRead.size();
	szImageData = new unsigned char[allocSz + 1];
	memcpy(szImageData, strRead.c_str(), allocSz);
	szImageData[allocSz] = '\0';
	imageLen = (int)allocSz;

	fclose(pFile);

	return 0;
}


DVTBusManger::DVTBusManger()
{
	//加载配置文件
	loadParms();

	if (m_Config->m_displayMask == 1){

		CBitmap BitMap;  //BitMap.LoadBitmap(IDB_BITMAP1)
		CImage  ciImage;
		CString strPath = m_Config->m_maskImgPath;//_T("E:\\code\\windows\\ccUP\\GKLDSDkDemo\\Release\\test.bmp");
		int maskDataLen = 0;
		if (ciImage.Load(m_Config->m_maskImgPath) == S_OK){
			m_maskW = ciImage.GetWidth();
			m_maskH = ciImage.GetHeight();
			int pitch = ciImage.GetBPP();

			if (m_maskW != m_Config->m_VideoWidth || m_maskH != m_Config->m_VideoHeight){
				CString fm;
				fm.Format("%d %d", m_maskW, m_maskH);
				Logc::WriteLog("模板图片分辨率不对" + fm,0);
			}else
			{
				// 分配内存
				int siz = m_maskW * m_maskH * 3;
				m_pBuffBMP = new unsigned char[siz];
				// 将图像数据复制到 unsigned char*
				if (pitch == 32){
					for (int y = 0; y < m_maskH; ++y) {
						for (int x = 0; x < m_maskW; ++x) {
							COLORREF color = ciImage.GetPixel(x, y);
							(m_pBuffBMP)[(y * m_maskW + x) * 3] = GetRValue(color);
							(m_pBuffBMP)[(y * m_maskW + x) * 3 + 1] = GetGValue(color);
							(m_pBuffBMP)[(y * m_maskW + x) * 3 + 2] = GetBValue(color);
						}
					}
					UpsideDownRGB24(m_pBuffBMP, m_Config->m_VideoWidth, m_Config->m_VideoHeight);
				}
				else if (pitch == 24){
					for (int y = 0; y < m_maskH; ++y) {
						memcpy(m_pBuffBMP + y * m_maskW * 3, ciImage.GetPixelAddress(0, y), m_maskW * 3);
					}
					UpsideDownRGB24(m_pBuffBMP, m_Config->m_VideoWidth, m_Config->m_VideoHeight);
					//memcpy(m_pBuffBMP, ciImage.GetBits(), m_maskW * m_maskH);
				}
				else{
					if (m_pBuffBMP != NULL){
						delete[] m_pBuffBMP;
						m_pBuffBMP = NULL;
					}
					CString fm;
					fm.Format("模板图片位数不对：%d", pitch);
					Logc::WriteLog(fm,0);
				}
			
				
			}
		}else{
			Logc::WriteLog("mask path error",0);
		}
		
#if 0
		unsigned char *imgData = NULL;
		int len = 0;
		int ret = read_file_data2(strPath, imgData, len);
		if (ret == 0){
			m_pBuffBMP = new UCHAR[len];
			m_maskW = 640;
			m_maskH = 480;
			memcpy(m_pBuffBMP, imgData,len);
			if (imgData != NULL){
				delete[] imgData;
				imgData = NULL;
			}
		}
#endif

#if 0
		HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, strPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		if (hBitmap != NULL){
			BitMap.Attach(hBitmap);
		}

		if (BitMap.LoadBitmap(IDB_BITMAP1)){//只能加载啊sourch里面的，不能加载系统本地路径的
			BITMAP pBitMap;
			BitMap.GetBitmap(&pBitMap); //获取BITMAP
			m_maskW = pBitMap.bmWidth;
			m_maskH = pBitMap.bmHeight;
			if (m_maskW != m_Config->m_VideoWidth || m_maskH != m_Config->m_VideoHeight){
				CString fm;
				fm.Format("%d %d", m_maskW, m_maskH);
				Logc::WriteLog("模板图片分辨率不对"+fm);
			}
			else{
				m_pBuffBMP = new UCHAR[pBitMap.bmWidthBytes * pBitMap.bmHeight];
				BitMap.GetBitmapBits(pBitMap.bmWidthBytes * pBitMap.bmHeight, m_pBuffBMP);
				UpsideDownRGB32(m_pBuffBMP, pBitMap.bmWidth, pBitMap.bmHeight);
			}
		}
		else{
			Logc::WriteLog("加载mask图片失败" + m_Config->m_maskImgPath);
		}
#endif
	}

	m_dataWidth = m_Config->m_VideoWidth;
	m_dataHeight = m_Config->m_VideoHeight;
	/*
	//判断是否有旋转
	if (m_Config->m_Degree == 90 || m_Config->m_Degree == 270){
	m_dataWidth = m_Config->m_VideoHeight;
	m_dataHeight = m_Config->m_VideoWidth;
	}
	else{
	m_dataWidth = m_Config->m_VideoWidth;
	m_dataHeight = m_Config->m_VideoHeight;
	}
	*/
	

	//同步事件对象
	m_FrameCopyEvents[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_FrameCopyEvents[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hSnapshotGotEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//初始化变量
	int buffLen = m_Config->m_VideoWidth*m_Config->m_VideoHeight*3;
	m_RGBFrameData = new unsigned char[buffLen];
	m_IRFrameData = new unsigned char[buffLen];
	m_DestRgbData = new unsigned char[buffLen];
	m_DestIrData = new unsigned char[buffLen];
	m_PhotoData = new unsigned char[buffLen];

	
	strcpy_s(m_detect_text[LD_CAM_STATUS_NO_FACE], sizeof(m_detect_text[LD_CAM_STATUS_NO_FACE]), m_Config->LD_CAM_STATUS_NO_FACE_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_FACE_BEYOND_MASK], sizeof(m_detect_text[LD_CAM_STATUS_FACE_BEYOND_MASK]), m_Config->LD_CAM_STATUS_FACE_BEYOND_MASK_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_ANGLE], sizeof(m_detect_text[LD_CAM_STATUS_ANGLE]), m_Config->LD_CAM_STATUS_ANGLE_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_TOO_BIG], sizeof(m_detect_text[LD_CAM_STATUS_TOO_BIG]), m_Config->LD_CAM_STATUS_TOO_BIG_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_TOO_SMALL], sizeof(m_detect_text[LD_CAM_STATUS_TOO_SMALL]), m_Config->LD_CAM_STATUS_TOO_SMALL_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_NEAR_BOUNDARY], sizeof(m_detect_text[LD_CAM_STATUS_NEAR_BOUNDARY]), m_Config->LD_CAM_STATUS_NEAR_BOUNDARY_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_FOREHEAD_COVERED], sizeof(m_detect_text[LD_CAM_STATUS_FOREHEAD_COVERED]), m_Config->LD_CAM_STATUS_COVERED_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_EYE_COVERED], sizeof(m_detect_text[LD_CAM_STATUS_EYE_COVERED]), m_Config->LD_CAM_STATUS_COVERED_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_MOUTH_COVERED], sizeof(m_detect_text[LD_CAM_STATUS_MOUTH_COVERED]), m_Config->LD_CAM_STATUS_COVERED_TEXT);
	strcpy_s(m_detect_text[LD_CAM_LIVENESS_FAILURE], sizeof(m_detect_text[LD_CAM_LIVENESS_FAILURE]), m_Config->LD_CAM_LIVENESS_FAILURE_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_MULTI_FACE], sizeof(m_detect_text[LD_CAM_STATUS_MULTI_FACE]), m_Config->LD_CAM_STATUS_MULTI_FACE_TEXT);
	strcpy_s(m_detect_text[LD_CAM_STATUS_FACE_MOVED], sizeof(m_detect_text[LD_CAM_STATUS_FACE_MOVED]), m_Config->LD_CAM_STATUS_FACE_MOVED_TEXT);

	//相机对象
	m_CameraManger = new CameraManger();
	//渲染对象
	m_COsd = new COsd();
	//算法对象
	m_XSManger = new XSManger(m_Config, m_dataWidth, m_dataHeight);
	m_XSManger->setResultCallBack(XSResultCallBack,this);
	
}



DVTBusManger::~DVTBusManger()
{
	stopThreadForData();

	if (m_CameraManger!=NULL)
	{
		delete m_CameraManger;
		m_CameraManger = NULL;
	}

	if (m_Config != NULL){
		delete m_Config;
		m_Config = NULL;
	}
	if (m_FrameCopyEvents[0]!=NULL)
	{
		CloseHandle(m_FrameCopyEvents[0]);
		m_FrameCopyEvents[0] = NULL;
	}
	if (m_FrameCopyEvents[1] != NULL)
	{
		CloseHandle(m_FrameCopyEvents[1]);
		m_FrameCopyEvents[1] = NULL;
	}

	if (m_RGBFrameData != NULL){
		delete m_RGBFrameData;
		m_RGBFrameData = NULL;
	}
	if (m_IRFrameData != NULL){
		delete m_IRFrameData;
		m_IRFrameData = NULL;
	}

	if (m_DestRgbData!=NULL)
	{
		delete m_DestRgbData;
		m_DestRgbData = NULL;
	}

	if (m_DestIrData != NULL)
	{
		delete m_DestIrData;
		m_DestIrData = NULL;
	}

	if (m_COsd != NULL){
		delete m_COsd;
		m_COsd = NULL;
	}

	if (m_XSManger != NULL){
		delete m_XSManger;
		m_XSManger = NULL;
	}

	if (m_hSnapshotGotEvent!=NULL)
	{
		CloseHandle(m_hSnapshotGotEvent);
		m_hSnapshotGotEvent = NULL;

	}

	if (m_PhotoData!=NULL)
	{
		delete m_PhotoData;
		m_PhotoData = NULL;
	}

	if (m_MaskBMPData!=NULL)
	{
		delete m_MaskBMPData;
		m_MaskBMPData = NULL;
	}
}


void DVTBusManger::loadParms(){

	TCHAR szBuffer[MAX_PATH] = { 0 };
	HMODULE hMod = GetModuleHandle(_T("DVTCamSDK.dll"));
	if (hMod != NULL)
	{
		GetModuleFileName(hMod, szBuffer, sizeof(szBuffer) / sizeof(TCHAR) - 1);
		PathRemoveFileSpec(szBuffer);
	}

	if (m_Config!=NULL)
	{
		delete m_Config;
		m_Config = NULL;
	}
	
	//需要判断配资文件是否存在

	//加载配置文件
	m_Config = new Config(szBuffer);
}


int DVTBusManger::LDCam_InitSDK(){
	bool success = m_XSManger->initSDK();
	if (!success)
	{
		CString oLog;
		oLog.Format("initXS failed");
		Logc::WriteLog(oLog,0);
		return LD_RET_ERROR;
	}
	m_authSuccess = TRUE;
	return 0;
	
}


int  DVTBusManger::LDCam_Open(){

	
	int nRet = 0;
	{
		CString log;
		log.Format("begin open:%s  %s", m_Config->m_ColorId, m_Config->m_InfraredId);
		Logc::WriteLog(log,0);
		
		if (m_CameraManger == NULL)
		{
			Logc::WriteLog("error m_CameraManger null",0);
			return LD_RET_ERROR;
		}
		m_CameraManger->load(2, m_Config->m_ColorId, m_Config->m_InfraredId);
		nRet = m_CameraManger->open();
		if (nRet != 0)
		{
			CString oLog;
			oLog.Format("open failed:%d", nRet);
			Logc::WriteLog(oLog,0);
			if (m_CameraManger != NULL){
				delete m_CameraManger;
				m_CameraManger = NULL;
			}
		}
	}
	return nRet;
}


int  DVTBusManger::LDCam_Close(){
	if (m_CameraManger == NULL)
	{
		return LD_RET_ERROR;
	}
	m_CameraManger->close();
	return 0;
}

int  DVTBusManger::LDCam_StopVideo(){
	if (m_CameraManger == NULL)
	{
		return LD_RET_ERROR;
	}
	stopThreadForData();
	Logc::WriteLog("LDCam_StopVideo",3);
	return  m_CameraManger->stopPreview();
}

int DVTBusManger::LDCam_SetEventCallback(LDEventCallBackFun pCallback, PVOID context){
	m_LDEventCallBackFun = pCallback;
	m_LDEventContext = context;
	return 0;
}

int DVTBusManger::LDCam_SetFrameCallback(LDFrameCallBackFun pCallback, PVOID context){
	m_LDFrameCallBackFun = pCallback;
	m_LDFrameContext = context;
	return 0;
}

int  DVTBusManger::LDCam_StartVideo(HWND hParentWnd, int nX, int nY, int nVideoWindowWidth, int nVideoWindowHeight){
	if (m_CameraManger == NULL)
	{
		return LD_RET_ERROR;
	}
	if (m_PreviewIng)
	{
		Logc::WriteLog("camera PreviewIng",4);
		return 0;
	}
	int nRet = 0;

	nRet = m_CameraManger->setFormat(m_Config->m_VideoWidth, m_Config->m_VideoHeight, 25);
	if (nRet == 0){
		nRet = m_CameraManger->setPreviewFrameBack(S_VisCamFrameCallback, this, S_NirCamFrameCallback, this);
		if (nRet == 0){
			RECT rgbRECT;
			rgbRECT.left = nX; rgbRECT.top = nY;
			rgbRECT.right = nX + nVideoWindowWidth;
			rgbRECT.bottom = nY + nVideoWindowHeight;
			nRet = m_CameraManger->startPreview(hParentWnd, rgbRECT, NULL, rgbRECT);
			if (nRet!=0)
			{
				//预览失败
				CString log;
				log.Format("startPreview error:%d",nRet);
				Logc::WriteLog(log,0);
			}
		}
		else{
			//设置回调失败
			CString log;
			log.Format("setPreviewFrameBack error:%d", nRet);
			Logc::WriteLog(log,0);
		}
	}
	else{
	  //设置分辨率失败
		CString log;
		log.Format("setFormat error:%d", nRet);
		Logc::WriteLog(log,0);
	}

	if (nRet==0)
	{
		//开启双目图像数据处理线程
		nRet = startThreadForData();
	}

	CString oLog;
	oLog.Format("LDCam_StartVideo:%d", nRet);
	Logc::WriteLog(oLog,3);

	return nRet;
}

int  DVTBusManger::LDCam_StartTrack(){
	m_XSManger->addFaceTrack();
	return 0;
}
int  DVTBusManger::LDCam_StopTrack(){
	m_XSManger->removeFaceTrack();
	return 0;
}

int  DVTBusManger::LDCam_StartRec(){
	m_MiniTime = m_Config->miniTime;
	m_TimeoutTime = m_Config->timeout;
	m_XSManger->addFaceTrack();
	m_XSManger->startFaceDetect();
	m_startDetectTime = GetTickCount();
	return 0;
}
int  DVTBusManger::LDCam_StopRec(){
	m_XSManger->stopFaceDetect();
	m_XSManger->removeFaceTrack();
	return 0;
}

int  DVTBusManger::LDCam_StartDetect(int nDetectMilliSeconds, int nTimeOutMilliSeconds){
	m_MiniTime = m_Config->miniTime;
	m_TimeoutTime = m_Config->timeout;
	m_XSManger->addFaceTrack();
	m_XSManger->startLiveDetect();
	m_startDetectTime = GetTickCount();
	return 0;
}
int  DVTBusManger::LDCam_StopDetect(){
	m_XSManger->stopLiveDetect();
	m_XSManger->removeFaceTrack();
	return 0;
}


int DVTBusManger::LDCam_GetImage(int nImageType, unsigned char** pBuff, unsigned long *pRetLen){
	int nRet = 0;
	unsigned char * data = NULL;
	if (nImageType < IMG_RGB24 || nImageType > IMG_JPG_BASE64)
	{
		Logc::WriteLog("LDCam_GetImage invalid parameter",0);
		nRet = LD_RET_INVALID_PARAMETER;
		goto done;
	}
	unsigned long dataLen = 0;
	//加锁是为了防止，冲突
	m_DestDataMutex.lock();
	data = rgb_to_OtherData(nImageType, m_DestRgbData, m_dataWidth,m_dataHeight, &dataLen, m_Config->m_jpgQuality);
	m_DestDataMutex.unlock();
	 if (data == NULL || dataLen==0)
	{
		nRet = LD_RET_ERROR;
		goto done;
	}
	
	*pBuff = new unsigned char[dataLen];
	memcpy(*pBuff, data, dataLen);
	*pRetLen = dataLen;

done:
	if (data != NULL){
		free(data);
		data = NULL;
	}
	return nRet;
}


int  DVTBusManger::LDCam_GetFaceImage(int nImageType, unsigned char** pBuff,  int *pWidth, int *pHeight, unsigned long *pRetLen){
	int nRet = 0;
	//裁剪数据 
	int cropped_size = 0;
	int faceW = m_DestRect.right - m_DestRect.left;
	int faceH = m_DestRect.bottom - m_DestRect.top;

	RECT rect;
	rect.left = m_DestRect.left - faceW / 2 <= 0 ? 0 : m_DestRect.left - faceW / 2;
	rect.right = m_DestRect.right + faceW / 2 >= m_dataWidth ? m_dataWidth : m_DestRect.right + faceW / 2;
	rect.top = m_DestRect.top - faceH / 2 <= 0 ? 0 : m_DestRect.top - faceH / 2;
	rect.bottom = m_DestRect.bottom + faceH / 2 >= m_dataHeight ? m_dataHeight : m_DestRect.bottom + faceH / 2;

	faceW = rect.right - rect.left;
	faceH = rect.bottom - rect.top;

	unsigned char* data = crop_rgb_data(m_DestRgbData, m_dataWidth, m_dataHeight, rect, &cropped_size);
	if (cropped_size<=0)
	{
		free(data);
		nRet = LD_RET_ERROR;
		goto done;
	}

	//数据格式转换
	unsigned long resDataLen = 0;
	unsigned char* retData = rgb_to_OtherData(nImageType, data, faceW, faceH, &resDataLen, m_Config->m_jpgQuality);
	if (resDataLen>0)
	{
		*pBuff = new unsigned char[resDataLen];
		//拷贝数据
		memcpy(*pBuff, retData, resDataLen);
		*pWidth = faceW;
		*pHeight = faceH;
		*pRetLen = resDataLen;
	}
	else{
		nRet = LD_RET_ERROR;
	}

done:
	return nRet;
	
}


DWORD WINAPI ThreadFunc_Detecting(LPVOID lpParam){
	DVTBusManger *bs = (DVTBusManger*)lpParam;
	if (bs != NULL){
		bs->threadProcessForData();
	}
	return 0;
}

int DVTBusManger::startThreadForData(){
	DWORD dwThreadId;
	HANDLE hThread;
	m_ThreadRuning = TRUE;
	hThread = CreateThread(
		NULL,
		0,                           // use default stack size  
		ThreadFunc_Detecting,        // thread function 
		this, 						 // argument to thread function 
		0,                           // use default creation flags 
		&dwThreadId);
	if (hThread==NULL){
		m_ThreadRuning = FALSE;
		Logc::WriteLog("ThreadFunc_Detecting thread error",0);
		return LD_RET_ERROR;
	}
	return 0;
}
int DVTBusManger::stopThreadForData(){
	m_ThreadRuning = FALSE;
	return 0;
}

int DVTBusManger::LDCam_Snapshot(int nImageType, unsigned char* pBuff, int nBuffLen, int *pRetLen){
	int nRet = 0;
	*pRetLen = 0;
	if (nImageType < IMG_RGB24 || nImageType > IMG_JPG_BASE64)
	{
		Logc::WriteLog("error LDCam_Snapshot invalid parameter",0);
		nRet = LD_RET_INVALID_PARAMETER;
		return nRet;
	}
	ResetEvent(m_hSnapshotGotEvent);
	m_PhotoCopying = TRUE;
	DWORD dwWait = WaitForSingleObject(m_hSnapshotGotEvent, 1000);
	if (WAIT_OBJECT_0 == dwWait)
	{
		unsigned char * photo = NULL;
		unsigned long photoLen = 0;
		if (nImageType == IMG_RGB24)
		{
			int rgbLen = m_dataWidth*m_dataHeight * 3;
			if (rgbLen<=nBuffLen){
				memset(pBuff, 0, nBuffLen);
				memcpy(pBuff, m_PhotoData, rgbLen);
			}
			else{
				nRet = LD_RET_BUFF_TOO_SMALL;
			}
		}
		else
		{
			photo = rgb_to_OtherData(nImageType, m_PhotoData, m_dataWidth,m_dataHeight, &photoLen, 100);
			*pRetLen = photoLen;
			if (photoLen>0)
			{
				if (photoLen > nBuffLen){
					nRet = LD_RET_BUFF_TOO_SMALL;
				}
				else{
					memcpy(pBuff, photo, photoLen);
				}
			}
			else{
				nRet = LD_RET_ERROR;
			}
			if (photo != NULL)
			{
				delete(photo);
				photo = NULL;
			}
		}
		
		
	}else{
		//拍照超时
		Logc::WriteLog("error take photo timeout",0);
		nRet = LD_RET_TIMEOUT;
	}
	return nRet;
}


int  DVTBusManger::LDCam_FaceCompFeature(const char* szFileName1, const char* szFileName2, int height1, int width1, int height2, int width2, float *pfSimilarity){
	return m_XSManger->FaceCompFeature(szFileName1, szFileName2, height1, width1, height2, width2, pfSimilarity);
}

int  DVTBusManger::LDCam_FaceCompFeatureEX(int imgType, unsigned char* imgData1, int data1Len, unsigned char* imgData2, int data2Len, float *pfSimilarity){
	return m_XSManger->FaceCompFeatureEX(imgType, imgData1, data1Len, imgData2, data2Len, pfSimilarity);
}

int DVTBusManger::LDCam_FaceCompByFeature(float*fs1, float*fs2, int len, float *pfSimilarity){
	return m_XSManger->FaceCompareByFeature(fs1, fs2, len, pfSimilarity);
}

int DVTBusManger::LDCam_GetFaceFeatureByData(int imgType, unsigned char* imgData, int dataLen, float*pfFeature, int pfLen, int *featureLen){
	return m_XSManger->GetFaceFeatureByData(imgType, imgData, dataLen, pfFeature, pfLen, featureLen);
}
int DVTBusManger::LDCam_GetFaceFeatureByPath(const char* szFileName, float*pfFeature, int pfLen, int *featureLen){
	return m_XSManger->GetFaceFeatureByPath(szFileName, pfFeature, pfLen, featureLen);
}