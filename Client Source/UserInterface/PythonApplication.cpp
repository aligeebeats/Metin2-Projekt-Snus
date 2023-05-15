#include "StdAfx.h"
#include "../eterBase/Error.h"
#include "../eterlib/Camera.h"
#include "../eterlib/AttributeInstance.h"
#include "../gamelib/AreaTerrain.h"
#include "../EterGrnLib/Material.h"
#include "../CWebBrowser/CWebBrowser.h"
#include "resource.h"
#include "PythonApplication.h"
#include "PythonCharacterManager.h"
#include "ProcessScanner.h"
#ifdef ENABLE_SWITCHBOT
#include "PythonSwitchbot.h"
#endif

extern void GrannyCreateSharedDeformBuffer();
extern void GrannyDestroySharedDeformBuffer();

float MIN_FOG = 2400.0f;
double g_specularSpd=0.007f;

CPythonApplication * CPythonApplication::ms_pInstance;

float c_fDefaultCameraRotateSpeed = 1.5f;
float c_fDefaultCameraPitchSpeed = 1.5f;
float c_fDefaultCameraZoomSpeed = 0.05f;

CPythonApplication::CPythonApplication() :
m_bCursorVisible(TRUE),
m_bLiarCursorOn(false),
m_iCursorMode(CURSOR_MODE_HARDWARE),
m_isWindowed(false),
m_isFrameSkipDisable(false),
m_poMouseHandler(NULL),
m_dwUpdateFPS(0),
m_dwRenderFPS(0),
m_fAveRenderTime(0.0f),
m_dwFaceCount(0),
m_fGlobalTime(0.0f),
m_fGlobalElapsedTime(0.0f),
m_dwLButtonDownTime(0),
m_dwLastIdleTime(0)
{
#ifndef _DEBUG
	SetEterExceptionHandler();
#endif

	CTimer::Instance().UseCustomTime();
	m_dwWidth = 800;
	m_dwHeight = 600;

	ms_pInstance = this;
	m_isWindowFullScreenEnable = FALSE;

	m_v3CenterPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_dwStartLocalTime = ELTimer_GetMSec();
	m_tServerTime = 0;
	m_tLocalStartTime = 0;

	m_iPort = 0;
	m_iFPS = 60;

	m_isActivateWnd = false;
	m_isMinimizedWnd = true;

	m_fRotationSpeed = 0.0f;
	m_fPitchSpeed = 0.0f;
	m_fZoomSpeed = 0.0f;

	m_fFaceSpd=0.0f;

	m_dwFaceAccCount=0;
	m_dwFaceAccTime=0;

	m_dwFaceSpdSum=0;
	m_dwFaceSpdCount=0;

	m_FlyingManager.SetMapManagerPtr(&m_pyBackground);

	m_iCursorNum = CURSOR_SHAPE_NORMAL;
	m_iContinuousCursorNum = CURSOR_SHAPE_NORMAL;

	m_isSpecialCameraMode = FALSE;
	m_fCameraRotateSpeed = c_fDefaultCameraRotateSpeed;
	m_fCameraPitchSpeed = c_fDefaultCameraPitchSpeed;
	m_fCameraZoomSpeed = c_fDefaultCameraZoomSpeed;

	m_iCameraMode = CAMERA_MODE_NORMAL;
	m_fBlendCameraStartTime = 0.0f;
	m_fBlendCameraBlendTime = 0.0f;

	m_iForceSightRange = -1;

	CCameraManager::Instance().AddCamera(EVENT_CAMERA_NUMBER);
}

CPythonApplication::~CPythonApplication()
{
}

void CPythonApplication::GetMousePosition(POINT* ppt)
{
	CMSApplication::GetMousePosition(ppt);
}

void CPythonApplication::SetMinFog(float fMinFog)
{
	MIN_FOG = fMinFog;
}

void CPythonApplication::SetFrameSkip(bool isEnable)
{
	if (isEnable)
		m_isFrameSkipDisable=false;
	else
		m_isFrameSkipDisable=true;
}

void CPythonApplication::NotifyHack(const char* c_szFormat, ...)
{
	char szBuf[1024];

	va_list args;
	va_start(args, c_szFormat);	
	_vsnprintf(szBuf, sizeof(szBuf), c_szFormat, args);
	va_end(args);
	m_pyNetworkStream.NotifyHack(szBuf);
}

void CPythonApplication::GetInfo(UINT eInfo, std::string* pstInfo)
{
	switch (eInfo)
	{
	case INFO_ACTOR:
		m_kChrMgr.GetInfo(pstInfo);
		break;
	case INFO_EFFECT:
		m_kEftMgr.GetInfo(pstInfo);			
		break;
	case INFO_ITEM:
		m_pyItem.GetInfo(pstInfo);
		break;
	case INFO_TEXTTAIL:
		m_pyTextTail.GetInfo(pstInfo);
		break;
	}
}

void CPythonApplication::Abort()
{
	TraceError("============================================================================================================");
	TraceError("Abort!!!!\n\n");

	PostQuitMessage(0);
}

void CPythonApplication::Exit()
{
	PostQuitMessage(0);
}

bool PERF_CHECKER_RENDER_GAME = false;

void CPythonApplication::RenderGame()
{	
	if (!PERF_CHECKER_RENDER_GAME)
	{
		m_kRenderTargetManager.RenderBackgrounds();
		float fAspect=m_kWndMgr.GetAspect();
		float fFarClip=m_pyBackground.GetFarClip();
#ifdef ENABLE_FOV
		int fov_value = CPythonSystem::Instance().GetFovValue();
		if (fov_value == 0)
			m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
		else if (fov_value == 1)
			m_pyGraphic.SetPerspective(60.0f, fAspect, 110.0, fFarClip-100.0);
		else if (fov_value == 2)
			m_pyGraphic.SetPerspective(70.0f, fAspect, 115.0, fFarClip-110.0);
		else if (fov_value == 3)
			m_pyGraphic.SetPerspective(85.0f, fAspect, 120.0, fFarClip-120.0);
		else if (fov_value == 4)
			m_pyGraphic.SetPerspective(100.0f, fAspect, 125.0, fFarClip-130.0);
#else
			m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
#endif
		CCullingManager::Instance().Process();

		m_kChrMgr.Deform();
		m_kEftMgr.Update();
		m_kRenderTargetManager.DeformModels();

		m_pyBackground.RenderCharacterShadowToTexture();

		m_pyGraphic.SetGameRenderState();
		m_pyGraphic.PushState();

		{
			long lx, ly;
			m_kWndMgr.GetMousePosition(lx, ly);
			m_pyGraphic.SetCursorPosition(lx, ly);
		}

		m_pyBackground.RenderSky();

		m_pyBackground.RenderBeforeLensFlare();

		m_pyBackground.RenderCloud();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.Render();

		m_pyBackground.SetCharacterDirLight();
		m_kChrMgr.Render();
		m_kRenderTargetManager.RenderModels();

		m_pyBackground.SetBackgroundDirLight();
		m_pyBackground.RenderWater();
		m_pyBackground.RenderSnow();
		m_pyBackground.RenderEffect();

		m_pyBackground.EndEnvironment();

		m_kEftMgr.Render();
		m_pyItem.Render();
		m_FlyingManager.Render();

		m_pyBackground.BeginEnvironment();
		m_pyBackground.RenderPCBlocker();
		m_pyBackground.EndEnvironment();

		m_pyBackground.RenderAfterLensFlare();

		return;
	}

	DWORD t1=ELTimer_GetMSec();
	m_kChrMgr.Deform();
	DWORD t2=ELTimer_GetMSec();
	m_kEftMgr.Update();
	DWORD t3=ELTimer_GetMSec();
	m_pyBackground.RenderCharacterShadowToTexture();
	DWORD t4=ELTimer_GetMSec();

	m_pyGraphic.SetGameRenderState();
	m_pyGraphic.PushState();


	float fAspect=m_kWndMgr.GetAspect();
	float fFarClip=m_pyBackground.GetFarClip();

#ifdef ENABLE_FOV
	int fov_value = CPythonSystem::Instance().GetFovValue();
	if (fov_value == 0)
		m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
	else if (fov_value == 1)
		m_pyGraphic.SetPerspective(60.0f, fAspect, 110.0, fFarClip-100.0);
	else if (fov_value == 2)
		m_pyGraphic.SetPerspective(70.0f, fAspect, 115.0, fFarClip-110.0);
	else if (fov_value == 3)
		m_pyGraphic.SetPerspective(85.0f, fAspect, 120.0, fFarClip-120.0);
	else if (fov_value == 4)
		m_pyGraphic.SetPerspective(100.0f, fAspect, 125.0, fFarClip-130.0);
#else
		m_pyGraphic.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
#endif

	DWORD t5=ELTimer_GetMSec();

	CCullingManager::Instance().Process();

	DWORD t6=ELTimer_GetMSec();

	{
		long lx, ly;
		m_kWndMgr.GetMousePosition(lx, ly);
		m_pyGraphic.SetCursorPosition(lx, ly);
	}

	m_pyBackground.RenderSky();
	DWORD t7=ELTimer_GetMSec();
	m_pyBackground.RenderBeforeLensFlare();
	DWORD t8=ELTimer_GetMSec();
	m_pyBackground.RenderCloud();
	DWORD t9=ELTimer_GetMSec();
	m_pyBackground.BeginEnvironment();
	m_pyBackground.Render();

	m_pyBackground.SetCharacterDirLight();
	DWORD t10=ELTimer_GetMSec();
	m_kChrMgr.Render();
	DWORD t11=ELTimer_GetMSec();

	m_pyBackground.SetBackgroundDirLight();
	m_pyBackground.RenderWater();
	DWORD t12=ELTimer_GetMSec();
	m_pyBackground.RenderEffect();
	DWORD t13=ELTimer_GetMSec();
	m_pyBackground.EndEnvironment();
	m_kEftMgr.Render();
	DWORD t14=ELTimer_GetMSec();
	m_pyItem.Render();
	DWORD t15=ELTimer_GetMSec();
	m_FlyingManager.Render();
	DWORD t16=ELTimer_GetMSec();
	m_pyBackground.BeginEnvironment();
	m_pyBackground.RenderPCBlocker();
	m_pyBackground.EndEnvironment();
	DWORD t17=ELTimer_GetMSec();
	m_pyBackground.RenderAfterLensFlare();
	DWORD t18=ELTimer_GetMSec();
	DWORD tEnd=ELTimer_GetMSec();

	if (GetAsyncKeyState(VK_Z))
		STATEMANAGER.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	if (tEnd-t1<3)
		return;

	static FILE* fp=fopen("perf_game_render.txt", "w");

	fprintf(fp, "GR.Total %d (Time %d)\n", tEnd-t1, ELTimer_GetMSec());
	fprintf(fp, "GR.DFM %d\n", t2-t1);
	fprintf(fp, "GR.EFT.UP %d\n", t3-t2);
	fprintf(fp, "GR.SHW %d\n", t4-t3);
	fprintf(fp, "GR.STT %d\n", t5-t4);
	fprintf(fp, "GR.CLL %d\n", t6-t5);
	fprintf(fp, "GR.BG.SKY %d\n", t7-t6);
	fprintf(fp, "GR.BG.LEN %d\n", t8-t7);
	fprintf(fp, "GR.BG.CLD %d\n", t9-t8);
	fprintf(fp, "GR.BG.MAIN %d\n", t10-t9);		
	fprintf(fp, "GR.CHR %d\n",	t11-t10);
	fprintf(fp, "GR.BG.WTR %d\n", t12-t11);
	fprintf(fp, "GR.BG.EFT %d\n", t13-t12);
	fprintf(fp, "GR.EFT %d\n", t14-t13);
	fprintf(fp, "GR.ITM %d\n", t15-t14);
	fprintf(fp, "GR.FLY %d\n", t16-t15);
	fprintf(fp, "GR.BG.BLK %d\n", t17-t16);
	fprintf(fp, "GR.BG.LEN %d\n", t18-t17);


	fflush(fp);
}

void CPythonApplication::UpdateGame()
{
	DWORD t1=ELTimer_GetMSec();
	POINT ptMouse;
	GetMousePosition(&ptMouse);

	CGraphicTextInstance::Hyperlink_UpdateMousePos(ptMouse.x, ptMouse.y);

	DWORD t2=ELTimer_GetMSec();

	{
		CScreen s;
		float fAspect = UI::CWindowManager::Instance().GetAspect();
		float fFarClip = CPythonBackground::Instance().GetFarClip();
#ifdef ENABLE_FOV
		int fov_value = CPythonSystem::Instance().GetFovValue();
		if (fov_value == 0)
			s.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
		else if (fov_value == 1)
			s.SetPerspective(60.0f, fAspect, 110.0, fFarClip - 100.0);
		else if (fov_value == 2)
			s.SetPerspective(70.0f, fAspect, 115.0, fFarClip - 110.0);
		else if (fov_value == 3)
			s.SetPerspective(85.0f, fAspect, 120.0, fFarClip - 120.0);
		else if (fov_value == 4)
			s.SetPerspective(100.0f, fAspect, 125.0, fFarClip - 130.0);
#else
			s.SetPerspective(30.0f, fAspect, 100.0, fFarClip);
#endif
		s.BuildViewFrustum();
	}

	m_kRenderTargetManager.UpdateModels();

	DWORD t3=ELTimer_GetMSec();
	TPixelPosition kPPosMainActor;
	m_pyPlayer.NEW_GetMainActorPosition(&kPPosMainActor);

	DWORD t4=ELTimer_GetMSec();
	m_pyBackground.Update(kPPosMainActor.x, kPPosMainActor.y, kPPosMainActor.z);

	DWORD t5=ELTimer_GetMSec();
	m_GameEventManager.SetCenterPosition(kPPosMainActor.x, kPPosMainActor.y, kPPosMainActor.z);
	m_GameEventManager.Update();

	DWORD t6=ELTimer_GetMSec();
	m_kChrMgr.Update();	
	DWORD t7=ELTimer_GetMSec();
	m_kEftMgr.UpdateSound();
	DWORD t8=ELTimer_GetMSec();
	m_FlyingManager.Update();
	DWORD t9=ELTimer_GetMSec();
	m_pyItem.Update(ptMouse);
	DWORD t10=ELTimer_GetMSec();
	m_pyPlayer.Update();
	DWORD t11=ELTimer_GetMSec();

	m_pyPlayer.NEW_GetMainActorPosition(&kPPosMainActor);
	SetCenterPosition(kPPosMainActor.x, kPPosMainActor.y, kPPosMainActor.z);
	DWORD t12=ELTimer_GetMSec();

	if (PERF_CHECKER_RENDER_GAME)
	{
		if (t12-t1>5)
		{
			static FILE* fp=fopen("perf_game_update.txt", "w");

			fprintf(fp, "GU.Total %d (Time %d)\n", t12-t1, ELTimer_GetMSec());
			fprintf(fp, "GU.GMP %d\n", t2-t1);
			fprintf(fp, "GU.SCR %d\n", t3-t2);
			fprintf(fp, "GU.MPS %d\n", t4-t3);
			fprintf(fp, "GU.BG %d\n", t5-t4);
			fprintf(fp, "GU.GEM %d\n", t6-t5);
			fprintf(fp, "GU.CHR %d\n", t7-t6);
			fprintf(fp, "GU.EFT %d\n", t8-t7);
			fprintf(fp, "GU.FLY %d\n", t9-t8);
			fprintf(fp, "GU.ITM %d\n", t10-t9);
			fprintf(fp, "GU.PLR %d\n", t11-t10);
			fprintf(fp, "GU.POS %d\n", t12-t11);
			fflush(fp);
		}
	}
}

void CPythonApplication::SkipRenderBuffering(DWORD dwSleepMSec)
{
	m_dwBufSleepSkipTime=ELTimer_GetMSec()+dwSleepMSec;
}

bool CPythonApplication::Process()
{
	ELTimer_SetFrameMSec();
	DWORD dwStart = ELTimer_GetMSec();

	static DWORD	s_dwUpdateFrameCount = 0;
	static DWORD	s_dwRenderFrameCount = 0;
	static DWORD	s_dwFaceCount = 0;
	static UINT		s_uiLoad = 0;
	static DWORD	s_dwCheckTime = ELTimer_GetMSec();

	if (ELTimer_GetMSec() - s_dwCheckTime > 1000)
	{
		m_dwUpdateFPS		= s_dwUpdateFrameCount;
		m_dwRenderFPS		= s_dwRenderFrameCount;
		m_dwLoad			= s_uiLoad;

		m_dwFaceCount		= s_dwFaceCount / max(1, s_dwRenderFrameCount);

		s_dwCheckTime		= ELTimer_GetMSec();

		s_uiLoad = s_dwFaceCount = s_dwUpdateFrameCount = s_dwRenderFrameCount = 0;
	}

	static BOOL s_bFrameSkip = false;
	static UINT s_uiNextFrameTime = ELTimer_GetMSec();

	CTimer& rkTimer=CTimer::Instance();
	rkTimer.Advance();

	m_fGlobalTime = rkTimer.GetCurrentSecond();
	m_fGlobalElapsedTime = rkTimer.GetElapsedSecond();

	UINT uiFrameTime = rkTimer.GetElapsedMilliecond();
	s_uiNextFrameTime += uiFrameTime;

	DWORD updatestart = ELTimer_GetMSec();
	m_pyNetworkStream.Process();	

	m_kGuildMarkUploader.Process();

	m_kGuildMarkDownloader.Process();
	m_kAccountConnector.Process();
	UpdateKeyboard();
	POINT Point;
	if (GetCursorPos(&Point))
	{
		ScreenToClient(m_hWnd, &Point);
		OnMouseMove(Point.x, Point.y);		
	}
	__UpdateCamera();
	CResourceManager::Instance().Update();
	OnCameraUpdate();
	OnMouseUpdate();
	OnUIUpdate();

	m_dwCurUpdateTime = ELTimer_GetMSec() - updatestart;

	DWORD dwCurrentTime = ELTimer_GetMSec();
	BOOL  bCurrentLateUpdate = FALSE;

	s_bFrameSkip = false;

	if (dwCurrentTime > s_uiNextFrameTime)
	{
		int dt = dwCurrentTime - s_uiNextFrameTime;
		int nAdjustTime = ((float)dt / (float)uiFrameTime) * uiFrameTime; 

		if ( dt >= 500 )
		{
			s_uiNextFrameTime += nAdjustTime; 
			printf("FrameSkip º¸Á¤ %d\n",nAdjustTime);
			CTimer::Instance().Adjust(nAdjustTime);
		}

		if(!m_isFrameSkipDisable)
			s_bFrameSkip = true;
		bCurrentLateUpdate = TRUE;
	}

	{
		static char i = 0;
		if (m_isMinimizedWnd && 0 == i++)
			CEffectManager::Instance().Update();
	}

	if (m_isFrameSkipDisable && !m_isMinimizedWnd)
		s_bFrameSkip = false;

	if (!s_bFrameSkip)
	{
		CGrannyMaterial::TranslateSpecularMatrix(g_specularSpd, g_specularSpd, 0.0f);

		DWORD dwRenderStartTime = ELTimer_GetMSec();		

		bool canRender = true;

		if (m_isMinimizedWnd)
		{
			canRender = false;
		}
		else
		{
#ifdef ENABLE_TEXT_RENEWAL
			if (DEVICE_STATE_OK != CheckDeviceState())
				canRender = false;
#else
			if (m_pyGraphic.IsLostDevice())
			{
				CPythonBackground& rkBG = CPythonBackground::Instance();
				rkBG.ReleaseCharacterShadowTexture();

				CRenderTargetManager::Instance().ReleaseRenderTargetTextures();

				if (m_pyGraphic.RestoreDevice())					
				{
					CRenderTargetManager::Instance().CreateRenderTargetTextures();
					rkBG.CreateCharacterShadowTexture();
				}
				else
					canRender = false;				
			}
#endif
		}

		if (!IsActive())
		{
			SkipRenderBuffering(3000);
		}
		if (!canRender)
		{
			SkipRenderBuffering(3000);
		}
		else
		{
			CCullingManager::Instance().Update();
			if (m_pyGraphic.Begin())
			{

				m_pyGraphic.ClearDepthBuffer();

#ifdef _DEBUG
				m_pyGraphic.SetClearColor(0.3f, 0.3f, 0.3f);
				m_pyGraphic.Clear();
#endif

				m_pyGraphic.SetInterfaceRenderState();

				OnUIRender();
				OnMouseRender();

				m_pyGraphic.End();
				m_pyGraphic.Show();

				DWORD dwRenderEndTime = ELTimer_GetMSec();

				static DWORD s_dwRenderCheckTime = dwRenderEndTime;
				static DWORD s_dwRenderRangeTime = 0;
				static DWORD s_dwRenderRangeFrame = 0;

				m_dwCurRenderTime = dwRenderEndTime - dwRenderStartTime;			
				s_dwRenderRangeTime += m_dwCurRenderTime;				
				++s_dwRenderRangeFrame;			

				if (dwRenderEndTime-s_dwRenderCheckTime>1000)
				{
					m_fAveRenderTime=float(double(s_dwRenderRangeTime)/double(s_dwRenderRangeFrame));

					s_dwRenderCheckTime=ELTimer_GetMSec();
					s_dwRenderRangeTime=0;
					s_dwRenderRangeFrame=0;
				}										

				DWORD dwCurFaceCount=m_pyGraphic.GetFaceCount();
				m_pyGraphic.ResetFaceCount();
				s_dwFaceCount += dwCurFaceCount;

				if (dwCurFaceCount > 5000)
				{
					if (dwRenderEndTime > m_dwBufSleepSkipTime)
					{	
						static float s_fBufRenderTime = 0.0f;

						float fCurRenderTime = m_dwCurRenderTime;

						if (fCurRenderTime > s_fBufRenderTime)
						{
							float fRatio = fMAX(0.5f, (fCurRenderTime - s_fBufRenderTime) / 30.0f);
							s_fBufRenderTime = (s_fBufRenderTime * (100.0f - fRatio) + (fCurRenderTime + 5) * fRatio) / 100.0f;
						}
						else
						{
							float fRatio = 0.5f;
							s_fBufRenderTime = (s_fBufRenderTime * (100.0f - fRatio) + fCurRenderTime * fRatio) / 100.0f;
						}

						if (s_fBufRenderTime > 100.0f)
							s_fBufRenderTime = 100.0f;

						DWORD dwBufRenderTime = s_fBufRenderTime;

						if (m_isWindowed)
						{						
							if (dwBufRenderTime>58)
								dwBufRenderTime=64;
							else if (dwBufRenderTime>42)
								dwBufRenderTime=48;
							else if (dwBufRenderTime>26)
								dwBufRenderTime=32;
							else if (dwBufRenderTime>10)
								dwBufRenderTime=16;
							else
								dwBufRenderTime=8;
						}

						m_fAveRenderTime=s_fBufRenderTime;
					}

					m_dwFaceAccCount += dwCurFaceCount;
					m_dwFaceAccTime += m_dwCurRenderTime;

					m_fFaceSpd=(m_dwFaceAccCount/m_dwFaceAccTime);

					if (-1 == m_iForceSightRange)
					{
						static float s_fAveRenderTime = 16.0f;
						float fRatio=0.3f;
						s_fAveRenderTime=(s_fAveRenderTime*(100.0f-fRatio)+max(16.0f, m_dwCurRenderTime)*fRatio)/100.0f;


						float fFar=25600.0f;
						float fNear=MIN_FOG;
						double dbAvePow=double(1000.0f/s_fAveRenderTime);
						double dbMaxPow=60.0;
						float fDistance=max(fNear+(fFar-fNear)*(dbAvePow)/dbMaxPow, fNear);
						m_pyBackground.SetViewDistanceSet(0, fDistance);
					}
					else
					{
						m_pyBackground.SetViewDistanceSet(0, float(m_iForceSightRange));
					}
				}
				else
				{
					m_pyBackground.SetViewDistanceSet(0, 25600.0f);
				}

				++s_dwRenderFrameCount;
			}
		}
	}

	int rest = s_uiNextFrameTime - ELTimer_GetMSec();

	if (rest > 0 && !bCurrentLateUpdate )
	{
		s_uiLoad -= rest;
		Sleep(rest);
	}	

	++s_dwUpdateFrameCount;

	s_uiLoad += ELTimer_GetMSec() - dwStart;
	return true;
}

void CPythonApplication::UpdateClientRect()
{
	RECT rcApp;
	GetClientRect(&rcApp);
	OnSizeChange(rcApp.right - rcApp.left, rcApp.bottom - rcApp.top);
}

void CPythonApplication::SetMouseHandler(PyObject* poMouseHandler)
{	
	m_poMouseHandler = poMouseHandler;
}

int CPythonApplication::CheckDeviceState()
{
	CGraphicDevice::EDeviceState e_deviceState = m_grpDevice.GetDeviceState();

	switch (e_deviceState)
	{
	case CGraphicDevice::DEVICESTATE_NULL:
		return DEVICE_STATE_FALSE;

	case CGraphicDevice::DEVICESTATE_BROKEN:
		return DEVICE_STATE_SKIP;

	case CGraphicDevice::DEVICESTATE_NEEDS_RESET:
#ifdef ENABLE_TEXT_RENEWAL
		m_pyBackground.ReleaseCharacterShadowTexture();

		CRenderTargetManager::Instance().ReleaseRenderTargetTextures();

		Trace("DEVICESTATE_NEEDS_RESET - attempting");
#endif

		if (!m_grpDevice.Reset())
			return DEVICE_STATE_SKIP;

#ifdef ENABLE_TEXT_RENEWAL
		m_pyBackground.CreateCharacterShadowTexture();

		CRenderTargetManager::Instance().CreateRenderTargetTextures();

		break;
	case CGraphicDevice::DEVICESTATE_OK: break;
	default:;
#else
		break;
#endif
	}

	return DEVICE_STATE_OK;
}

bool CPythonApplication::CreateDevice(int width, int height, int Windowed, int bit, int frequency)
{
	int iRet;

	m_grpDevice.InitBackBufferCount(2);

	iRet = m_grpDevice.Create(GetWindowHandle(), width, height, Windowed ? true : false, bit,frequency);

	switch (iRet)
	{
	case CGraphicDevice::CREATE_OK:
		return true;

	case CGraphicDevice::CREATE_REFRESHRATE:
		return true;

	case CGraphicDevice::CREATE_ENUM:
	case CGraphicDevice::CREATE_DETECT:
		SET_EXCEPTION(CREATE_NO_APPROPRIATE_DEVICE);
		TraceError("CreateDevice: Enum & Detect failed");
		return false;

	case CGraphicDevice::CREATE_NO_DIRECTX:
		SET_EXCEPTION(CREATE_NO_DIRECTX);
		TraceError("CreateDevice: DirectX 8.1 or greater required to run game");
		return false;

	case CGraphicDevice::CREATE_DEVICE:
		SET_EXCEPTION(CREATE_DEVICE);
		TraceError("CreateDevice: GraphicDevice create failed");
		return false;

	case CGraphicDevice::CREATE_FORMAT:
		SET_EXCEPTION(CREATE_FORMAT);
		TraceError("CreateDevice: Change the screen format");
		return false;

	case CGraphicDevice::CREATE_GET_DEVICE_CAPS:
		PyErr_SetString(PyExc_RuntimeError, "GetDevCaps failed");
		TraceError("CreateDevice: GetDevCaps failed");
		return false;

	case CGraphicDevice::CREATE_GET_DEVICE_CAPS2:
		PyErr_SetString(PyExc_RuntimeError, "GetDevCaps2 failed");
		TraceError("CreateDevice: GetDevCaps2 failed");
		return false;

	default:
		if (iRet & CGraphicDevice::CREATE_OK)
		{
			if (iRet & CGraphicDevice::CREATE_NO_TNL)
			{
				CGrannyLODController::SetMinLODMode(true);
			}
			return true;
		}

		SET_EXCEPTION(UNKNOWN_ERROR);
		TraceError("CreateDevice: Unknown Error!");
		return false;
	}
}

void CPythonApplication::Loop()
{	
	while (1)
	{	
		if (IsMessage())
		{
			if (!MessageProcess())
				break;
		}
		else
		{
			if (!Process())
				break;

			m_dwLastIdleTime=ELTimer_GetMSec();
		}
	}
}

bool LoadLocaleData(const char* localePath)
{
	NANOBEGIN
		CPythonNonPlayer&	rkNPCMgr	= CPythonNonPlayer::Instance();
	CItemManager&		rkItemMgr	= CItemManager::Instance();	
	CPythonSkill&		rkSkillMgr	= CPythonSkill::Instance();
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();

	char szItemList[256];
	char szItemProto[256];
	char szItemDesc[256];	
	char szItemScale[256];
#ifdef ENABLE_SHINING_SYSTEM
	char szShiningTable[256];
#endif
	char szMobProto[256];
	char szSkillDescFileName[256];
	char szSkillTableFileName[256];
	char szInsultList[256];

	snprintf(szItemList,	sizeof(szItemList) ,	"%s/item_list.txt",	localePath);
	snprintf(szSkillTableFileName, sizeof(szSkillTableFileName), "%s/SkillTable.txt", localePath);
	snprintf(szSkillDescFileName, sizeof(szSkillDescFileName), "%s/SkillDesc.txt", localePath);
	snprintf(szItemDesc, sizeof(szItemDesc), "%s/itemdesc.txt", localePath);
	snprintf(szItemScale, sizeof(szItemScale), "%s/item_scale.txt", localePath);
#ifdef ENABLE_SHINING_SYSTEM
	snprintf(szShiningTable, sizeof(szShiningTable), "%s/shiningtable.txt", localePath);
#endif
	snprintf(szItemProto, sizeof(szItemProto), "%s/item_proto", localePath);
	snprintf(szMobProto, sizeof(szMobProto), "%s/mob_proto", localePath);
	snprintf(szInsultList, sizeof(szInsultList), "%s/insult.txt", localePath);

	rkNPCMgr.Destroy();
	rkItemMgr.Destroy();	
	rkSkillMgr.Destroy();

	if (!rkItemMgr.LoadItemList(szItemList))
	{
		TraceError("LoadLocaleData - LoadItemList(%s) Error", szItemList);
	}	

	if (!rkItemMgr.LoadItemTable(szItemProto))
	{
		TraceError("LoadLocaleData - LoadItemProto(%s) Error", szItemProto);
		return false;
	}

#ifdef ENABLE_CHEST_DROP
	char szChestDrop[256];
	snprintf(szChestDrop, sizeof(szChestDrop), "%s/chest_drop", localePath);

	if (!rkItemMgr.LoadChestDropInfo(szChestDrop))
	{
		TraceError("LoadLocaleData - LoadChestDropInfo(%s) Error", szChestDrop);
		return false;
	}
#endif

	if (!rkNPCMgr.LoadNonPlayerData(szMobProto))
	{
		TraceError("LoadLocaleData - LoadMobProto(%s) Error", szMobProto);
		return false;
	}

	if (!rkItemMgr.LoadItemDesc(szItemDesc))
	{
		Tracenf("LoadLocaleData - LoadItemDesc(%s) Error", szItemDesc);	
	}

	if (!rkItemMgr.LoadItemScale(szItemScale))
	{
		Tracenf("LoadLocaleData - LoadItemScale(%s) Error", szItemScale);
	}

#ifdef ENABLE_SHINING_SYSTEM
	if (!rkItemMgr.LoadShiningTable(szShiningTable))
	{
		Tracenf("LoadLocaleData - LoadShiningTable(%s) Error", szShiningTable);
	}
#endif

	if (!rkSkillMgr.RegisterSkillDesc(szSkillDescFileName))
	{
		TraceError("LoadLocaleData - RegisterSkillDesc(%s) Error", szMobProto);
		return false;
	}

	if (!rkSkillMgr.RegisterSkillTable(szSkillTableFileName))
	{
		TraceError("LoadLocaleData - RegisterSkillTable(%s) Error", szMobProto);
		return false;
	}

	if (!rkNetStream.LoadInsultList(szInsultList))
	{
		Tracenf("CPythonApplication - CPythonNetworkStream::LoadInsultList(%s)", szInsultList);				
	}

	NANOEND
		return true;
}

unsigned __GetWindowMode(bool windowed)
{
	if (windowed)
		return WS_OVERLAPPED | WS_CAPTION |   WS_SYSMENU | WS_MINIMIZEBOX;

	return WS_POPUP;
}

bool CPythonApplication::Create(PyObject * poSelf, const char * c_szName, int width, int height, int Windowed)
{
	NANOBEGIN
		Windowed = CPythonSystem::Instance().IsWindowed() ? 1 : 0;

	bool bAnotherWindow = false;

	if (FindWindow(NULL, c_szName))
		bAnotherWindow = true;

	m_dwWidth = width;
	m_dwHeight = height;

	UINT WindowMode = __GetWindowMode(Windowed ? true : false);

	if (!CMSWindow::Create(c_szName, 4, 0, WindowMode, ::LoadIcon( GetInstance(), MAKEINTRESOURCE( IDI_METIN2 ) ), IDC_CURSOR_NORMAL))
	{
		TraceError("CMSWindow::Create failed");
		SET_EXCEPTION(CREATE_WINDOW);
		return false;
	}

	if (m_pySystem.IsUseDefaultIME())
	{
		CPythonIME::Instance().UseDefaultIME();
	}

	AdjustSize(m_pySystem.GetWidth(), m_pySystem.GetHeight());
	SetPosition(-8, 0);

	if (Windowed)
	{
		m_isWindowed = true;

		if (bAnotherWindow)
		{
			RECT rc;

			GetClientRect(&rc);

			int windowWidth = rc.right - rc.left;
			int windowHeight = (rc.bottom - rc.top);

			CMSApplication::SetPosition(GetScreenWidth() - windowWidth, GetScreenHeight() - 60 - windowHeight);
		}
	}
	else
	{
		m_isWindowed = false;
		SetPosition(0, 0);
	}

	NANOEND

		if (!CreateCursors())
		{
			TraceError("CMSWindow::Cursors Create Error");
			SET_EXCEPTION("CREATE_CURSOR");
			return false;
		}

		if (!m_pySystem.IsNoSoundCard())
		{
			if (!m_SoundManager.Create())
			{
			}
		}

		extern bool GRAPHICS_CAPS_SOFTWARE_TILING;

		if (!m_pySystem.IsAutoTiling())
			GRAPHICS_CAPS_SOFTWARE_TILING = m_pySystem.IsSoftwareTiling();

		if (!CreateDevice(m_pySystem.GetWidth(), m_pySystem.GetHeight(), Windowed, m_pySystem.GetBPP(), m_pySystem.GetFrequency()))
			return false;

		GrannyCreateSharedDeformBuffer();

		if (m_pySystem.IsAutoTiling())
		{
			if (m_grpDevice.IsFastTNL())
			{
				m_pyBackground.ReserveSoftwareTilingEnable(false);
			}
			else
			{
				m_pyBackground.ReserveSoftwareTilingEnable(true);
			}
		}
		else
		{
			m_pyBackground.ReserveSoftwareTilingEnable(m_pySystem.IsSoftwareTiling());
		}

		SetVisibleMode(true);

		if (m_isWindowFullScreenEnable)
		{
			SetWindowPos(GetWindowHandle(), HWND_TOP, 0, 0, width, height, SWP_SHOWWINDOW);
		}

		if (!InitializeKeyboard(GetWindowHandle()))
			return false;

		m_pySystem.GetDisplaySettings();

		if (m_pySystem.IsSoftwareCursor())
			SetCursorMode(CURSOR_MODE_SOFTWARE);
		else
			SetCursorMode(CURSOR_MODE_HARDWARE);

		if (!m_netDevice.Create())
		{
			TraceError("NetDevice::Create failed");
			SET_EXCEPTION("CREATE_NETWORK");
			return false;
		}

		if (!m_grpDevice.IsFastTNL())
			CGrannyLODController::SetMinLODMode(true);

		m_pyItem.Create();

		DefaultFont_Startup();

		CPythonIME::Instance().Create(GetWindowHandle());
		CPythonIME::Instance().SetText("", 0);
		CPythonTextTail::Instance().Initialize();

		m_LightManager.Initialize();

		CGraphicImageInstance::CreateSystem(32);

		STICKYKEYS sStickKeys;
		memset(&sStickKeys, 0, sizeof(sStickKeys));
		sStickKeys.cbSize = sizeof(sStickKeys);
		SystemParametersInfo( SPI_GETSTICKYKEYS, sizeof(sStickKeys), &sStickKeys, 0 );
		m_dwStickyKeysFlag = sStickKeys.dwFlags;

		sStickKeys.dwFlags &= ~(SKF_AVAILABLE|SKF_HOTKEYACTIVE);
		SystemParametersInfo( SPI_SETSTICKYKEYS, sizeof(sStickKeys), &sStickKeys, 0 );

		CGrannyMaterial::CreateSphereMap(0, "d:/ymir work/special/spheremap.jpg");
		CGrannyMaterial::CreateSphereMap(1, "d:/ymir work/special/spheremap01.jpg");
		return true;
}

void CPythonApplication::SetGlobalCenterPosition(LONG x, LONG y)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.GlobalPositionToLocalPosition(x, y);

	float z = CPythonBackground::Instance().GetHeight(x, y);

	CPythonApplication::Instance().SetCenterPosition(x, y, z);
}

void CPythonApplication::SetCenterPosition(float fx, float fy, float fz)
{
	m_v3CenterPosition.x = +fx;
	m_v3CenterPosition.y = -fy;
	m_v3CenterPosition.z = +fz;
}

void CPythonApplication::GetCenterPosition(TPixelPosition * pPixelPosition)
{
	pPixelPosition->x = +m_v3CenterPosition.x;
	pPixelPosition->y = -m_v3CenterPosition.y;
	pPixelPosition->z = +m_v3CenterPosition.z;
}


void CPythonApplication::SetServerTime(time_t tTime)
{
	m_dwStartLocalTime	= ELTimer_GetMSec();
	m_tServerTime		= tTime;
	m_tLocalStartTime	= time(0);
}

time_t CPythonApplication::GetServerTime()
{
	return (ELTimer_GetMSec() - m_dwStartLocalTime) + m_tServerTime;
}

time_t CPythonApplication::GetServerTimeStamp()
{
	return (time(0) - m_tLocalStartTime) + m_tServerTime;
}

#ifdef ENABLE_ITEMSHOP
time_t CPythonApplication::GetUNIX()
{
	return time(0);
}
#endif

float CPythonApplication::GetGlobalTime()
{
	return m_fGlobalTime;
}

float CPythonApplication::GetGlobalElapsedTime()
{
	return m_fGlobalElapsedTime;
}

void CPythonApplication::SetFPS(int iFPS)
{
	m_iFPS = iFPS;
}

int CPythonApplication::GetWidth()
{
	return m_dwWidth;
}

int CPythonApplication::GetHeight()
{
	return m_dwHeight;
}

void CPythonApplication::SetConnectData(const char * c_szIP, int iPort)
{
	m_strIP = c_szIP;
	m_iPort = iPort;
}

void CPythonApplication::GetConnectData(std::string & rstIP, int & riPort)
{
	rstIP	= m_strIP;
	riPort	= m_iPort;
}

void CPythonApplication::EnableSpecialCameraMode()
{
	m_isSpecialCameraMode = TRUE;
}

void CPythonApplication::SetCameraSpeed(int iPercentage)
{
	m_fCameraRotateSpeed = c_fDefaultCameraRotateSpeed * float(iPercentage) / 100.0f;
	m_fCameraPitchSpeed = c_fDefaultCameraPitchSpeed * float(iPercentage) / 100.0f;
	m_fCameraZoomSpeed = c_fDefaultCameraZoomSpeed * float(iPercentage) / 100.0f;
}

void CPythonApplication::SetForceSightRange(int iRange)
{
	m_iForceSightRange = iRange;
}

void CPythonApplication::Clear()
{
	m_pySystem.Clear();
}

void CPythonApplication::Destroy()
{
	WebBrowser_Destroy();

	CGrannyMaterial::DestroySphereMap();

	m_kWndMgr.Destroy();

	CPythonSystem::Instance().SaveConfig();

	m_kRenderTargetManager.Destroy();

	DestroyCollisionInstanceSystem();

	m_pySystem.SaveInterfaceStatus();

	m_pyEventManager.Destroy();	
	m_FlyingManager.Destroy();

	m_pyMiniMap.Destroy();

	m_pyTextTail.Destroy();
	m_pyChat.Destroy();	
	m_kChrMgr.Destroy();
	m_RaceManager.Destroy();

	m_pyItem.Destroy();
	m_kItemMgr.Destroy();

	m_pyBackground.Destroy();

	m_kEftMgr.Destroy();
	m_LightManager.Destroy();

	DefaultFont_Cleanup();

	GrannyDestroySharedDeformBuffer();

	m_pyGraphic.Destroy();

	m_pyRes.Destroy();

	m_kGuildMarkDownloader.Disconnect();

	CGrannyModelInstance::DestroySystem();
	CGraphicImageInstance::DestroySystem();

	m_SoundManager.Destroy();
	m_grpDevice.Destroy();

	CAttributeInstance::DestroySystem();
	CTextFileLoader::DestroySystem();
	DestroyCursors();

	CMSApplication::Destroy();

	STICKYKEYS sStickKeys;
	memset(&sStickKeys, 0, sizeof(sStickKeys));
	sStickKeys.cbSize = sizeof(sStickKeys);
	sStickKeys.dwFlags = m_dwStickyKeysFlag;
	SystemParametersInfo( SPI_SETSTICKYKEYS, sizeof(sStickKeys), &sStickKeys, 0 );
}
