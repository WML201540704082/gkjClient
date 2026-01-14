#include "stdafx.h"
#include "Config.h"
#include "util\IniFile.h"
#include "Logc.h"

Config::Config(CString szBuffer)
{
	CIni Ini;


	CString strConfigFileName;
	CString strFaceExpand;


	m_face_model_path.Format(_T("%s\\model\\model_face"), szBuffer);
	m_face_ir_model_path.Format(_T("%s\\model\\model_face_ir"), szBuffer);

	strConfigFileName.Format(_T("%s\\dvconfig.ini"), szBuffer);

	Ini.SetFile(strConfigFileName);

	Ini.SetSection(SECTION_LOGSETTING);
	Ini.ReadInt(SAVE_LOG, m_saveLog);
	Ini.ReadString(Log_Dir, m_LogDir);

	Ini.SetSection(SECTION_CAMERASETTING);

	Ini.ReadString(COLOR_ID, m_ColorId);
	Ini.ReadString(INFRARED_ID, m_InfraredId);

	if (!Ini.ReadInt("Width", m_VideoWidth)){
		m_VideoWidth = 640;
	}
	if (!Ini.ReadInt("Height", m_VideoHeight)){
		m_VideoHeight = 480;
	}

	Ini.ReadInt("Degree", m_Degree);
	Ini.ReadInt("m_doubleCamera", m_doubleCamera);

	m_ColorId.MakeUpper();
	m_InfraredId.MakeUpper();

	Ini.SetSection(SECTION_DETECTLIVING);

	Ini.ReadInt("timeout", timeout);
	Ini.ReadInt("miniTime", miniTime);
	Ini.ReadInt(CONFIDENCE, m_Confidence);

	Ini.ReadInt(FACE_WIDTH, m_FaceWidth);

	Ini.ReadInt(FACE_HEIGHT, m_FaceHeight);

	Ini.ReadInt(FACE_YAW, m_FaceYaw);

	Ini.ReadInt(FACE_PITCH, m_FacePitch);

	Ini.ReadInt(FACE_ROLL, m_FaceRoll);

	Ini.ReadInt(FACE_QUALITY, m_FaceQuality);

	if (!Ini.ReadInt(IS_COVER, m_isCover))
	{
		//WriteLog("dvconfig.ini isCover no found");
	}

	if (!Ini.ReadInt(IS_FACECOMPARE, m_isFaceCompare))
	{
		//WriteLog("dvconfig.ini isFaceCompare no found");
	}

	Ini.SetSection(SECTION_EXTRASETTING);

	Ini.ReadString("maskImgPath", m_maskImgPath);
	Ini.ReadInt(DISPLAY_MASK, m_displayMask);


	Ini.ReadInt("showMaskV", showMaskV);
	Ini.ReadInt("maskL", maskL);
	Ini.ReadInt("maskT", maskT);
	Ini.ReadInt("maskR", maskR);
	Ini.ReadInt("maskB", maskB);

	Ini.ReadInt(JPG_QUALITY, m_jpgQuality);
	Ini.ReadInt(IS_TEXT, m_isText);
	Ini.ReadInt(IS_AUDIO, m_IsAudio);
	Ini.ReadInt(IS_MULTIFACE, m_isMultiFace);
	//Ini.ReadInt(IS_PREVIEWTRACKFACE, m_isPreviewTrackFace);
	Ini.ReadInt(IS_SHOWFACEFRAME, m_isShowFaceFrame);

	int liveTfStr;
	if (Ini.ReadInt("liveTf", liveTfStr)){
		m_liveThreshold = liveTfStr*0.03;
	}

	CString log;
	log.Format("当前活体阈值=%f m_isCover=%d ", m_liveThreshold, m_isCover);
	Logc::WriteLog(log,0);


	if (!Ini.ReadString(EXPAND_FACE, strFaceExpand))
	{
		//WriteLog("dvconfig.ini expandFace no found");
	}
	else
	{

		m_expandFace = (float)atof(strFaceExpand.GetBuffer(strFaceExpand.GetLength()));
		strFaceExpand.ReleaseBuffer();

		if (m_expandFace<0.2 || m_expandFace>0.8)
		{
			m_expandFace = 0.2f;
		}
	}

	if (m_jpgQuality < 0 || m_jpgQuality > 100)
	{
		m_jpgQuality = 75;
	}

	Ini.ReadInt("textColorR", textColorR);
	Ini.ReadInt("textColorG", textColorG);
	Ini.ReadInt("textColorB", textColorB);
	//miniTime

	Ini.ReadString("ing", ing);
	Ini.ReadString("LD_CAM_STATUS_NO_FACE_TEXT", LD_CAM_STATUS_NO_FACE_TEXT);
	Ini.ReadString("LD_CAM_STATUS_FACE_BEYOND_MASK_TEXT", LD_CAM_STATUS_FACE_BEYOND_MASK_TEXT);
	Ini.ReadString("LD_CAM_STATUS_ANGLE_TEXT", LD_CAM_STATUS_ANGLE_TEXT);
	Ini.ReadString("LD_CAM_STATUS_TOO_BIG_TEXT", LD_CAM_STATUS_TOO_BIG_TEXT);
	Ini.ReadString("LD_CAM_STATUS_TOO_SMALL_TEXT", LD_CAM_STATUS_TOO_SMALL_TEXT);
	Ini.ReadString("LD_CAM_STATUS_NEAR_BOUNDARY_TEXT", LD_CAM_STATUS_NEAR_BOUNDARY_TEXT);
	Ini.ReadString("LD_CAM_STATUS_COVERED_TEXT", LD_CAM_STATUS_COVERED_TEXT);
	Ini.ReadString("LD_CAM_LIVENESS_FAILURE_TEXT", LD_CAM_LIVENESS_FAILURE_TEXT);
	Ini.ReadString("LD_CAM_STATUS_MULTI_FACE_TEXT", LD_CAM_STATUS_MULTI_FACE_TEXT);
	Ini.ReadString("LD_CAM_STATUS_FACE_MOVED_TEXT", LD_CAM_STATUS_FACE_MOVED_TEXT);

}


Config::~Config()
{
}
