#pragma once
#include <afx.h>

class Config
{
public:
	Config(CString path);
	~Config();


public:
	CString m_face_model_path;
	CString m_face_ir_model_path;
	CString m_ColorId;
	CString m_InfraredId;
	int m_doubleCamera = 1;
	//分辨率
	int m_VideoWidth = 640;
	int m_VideoHeight = 480;

	int m_Degree;//旋转角度
	int miniTime = 0;
	int timeout = 15000;
	int liveLevel = 1; //活体等级 1:
	int     m_Confidence;
	int     m_FaceWidth;
	int     m_FaceHeight;
	int     m_FaceYaw;
	int     m_FacePitch;
	int     m_FaceRoll;
	int     m_FaceQuality;
	int     m_isCover;
	int     m_displayMask;
	CString m_maskImgPath;
	int showMaskV = 0;
	int maskL = 200;
	int maskT = 70;
	int maskR = 440;
	int maskB = 380;
	int     m_jpgQuality;
	int     m_isText;
	int     m_IsAudio;
	float   m_expandFace;
	float   m_liveThreshold = 2.4;
	int     m_isMultiFace;
	int     m_isPreviewTrackFace;
	int     m_isShowFaceFrame;
	int     m_saveLog;
	CString m_LogDir;
	int     m_isFaceCompare;
	//
	int  textColorR = 0;
	int  textColorG = 0;
	int  textColorB = 0;
	CString ing;
	CString LD_CAM_STATUS_NO_FACE_TEXT;
	CString LD_CAM_STATUS_FACE_BEYOND_MASK_TEXT;
	CString LD_CAM_STATUS_ANGLE_TEXT;
	CString LD_CAM_STATUS_TOO_BIG_TEXT;
	CString LD_CAM_STATUS_TOO_SMALL_TEXT;
	CString LD_CAM_STATUS_NEAR_BOUNDARY_TEXT;
	CString LD_CAM_STATUS_COVERED_TEXT;
	CString LD_CAM_LIVENESS_FAILURE_TEXT;
	CString LD_CAM_STATUS_MULTI_FACE_TEXT;
	CString LD_CAM_STATUS_FACE_MOVED_TEXT;



};

