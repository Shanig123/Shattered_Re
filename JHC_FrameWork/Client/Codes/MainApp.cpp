#include "stdafx.h"
#include "MainApp.h"

#include "Logo_Scene.h"
#include "UI_Screen.h"
#include "Menu_UI.h"
#include "Loading_UI.h"
#include "Loading_Circle_H.h"
#include "Loading_Circle_Min.h"
#include "EndingCredit.h"
#include "MainCamera.h"
#include "CinematicCamera_0.h"
#include "ScreenShotCamera.h"
#include "TrailEffect.h"

CMainApp::CMainApp()
	: m_pManagement(GET_MANAGEMENT)
{
	Safe_AddRef(m_pManagement);
}

HRESULT CMainApp::Ready_MainApp()
{
	if (nullptr == m_pManagement)
		return E_FAIL;
#ifdef _DEBUG
	if (FAILED(m_pManagement->Ready_Engine(g_hInst, g_hWnd, WINCX, WINCY, EDisplayMode::Win, EBuildSetting::Client, (_uint)ESceneType::EndID, false)))
	{
		ERROR_BOX(L"Failed To Ready_Engine");
		return E_FAIL;
	}
#else
	if (FAILED(m_pManagement->Ready_Engine(g_hInst, g_hWnd, WINCX, WINCY, EDisplayMode::Win, EBuildSetting::Client, (_uint)ESceneType::EndID,false)))
	{
		ERROR_BOX(L"Failed To Ready_Engine");
		return E_FAIL;	
	}
#endif // _DEBUG

	if (FAILED(m_pManagement->Ready_Sound_Manager()))
	{
		ERROR_BOX(L"Failed To Ready_Engine");
		return E_FAIL;
	}

	m_pDevice = m_pManagement->Get_Device();
	if (nullptr == m_pDevice)
	{
		ERROR_BOX(L"Failed To Get_Device");
		return E_FAIL;
	}

	Safe_AddRef(m_pDevice);

	if (FAILED(Add_Component()))
	{
		ERROR_BOX(L"Failed To Add_Component");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Setup_CurrentScene((_int)ESceneType::Logo, CLogo_Scene::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed To Setup_CurrentScene");
		return E_FAIL;
	}

	if (FAILED(Setup_DefaultSetting()))
	{
		ERROR_BOX(L"Failed To Setup_DefaultSetting");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Ready_Font(m_pDevice, L"Font_Default", L"궁서", 15, 20, FW_HEAVY)))
	{
		ERROR_BOX(L"Failed To Ready_Font");
		return E_FAIL;
	}
		
		
	return S_OK;
}

_uint CMainApp::Update_MainApp(_float & _rfDeltaTime)
{
	_uint iEvent = m_pManagement->Update_Engine(_rfDeltaTime);
	if (1 == iEvent)
		return 0;
	
	m_pManagement->Render_Begin(D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f));

	m_pManagement->Render_RenderList(g_hWnd);

	m_fTime += _rfDeltaTime;

	++m_dwRenderCnt;

	if (m_fTime >= 1.f)
	{
		wsprintf(m_szFPS, L"FPS : %d", m_dwRenderCnt);
		m_fTime = 0.f;
		m_dwRenderCnt = 0;
		SetWindowText(g_hWnd, m_szFPS);
	}

	//m_pManagement->Render_Font(L"Font_Default", m_szFPS, &_float2(1080, 20), D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

	m_pManagement->Render_End();

	Key_Input();

	return _uint();
}

HRESULT CMainApp::Setup_DefaultSetting()
{
	/*
	D3DCULL_CCW : 반시계방향인 면을 추려낸다. 디폴트값.
	D3DCULL_CW : 시계방향인 면을 추려낸다.
	D3DCULL_NONE : 아무것도 추려내지 않는다.
	*/
	if (FAILED(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Add_Component()
{
	CManagement* pManagement = GET_MANAGEMENT;

#pragma region Component
	/* For.Component_VIBuffer_RectTexture */
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_VIBuffer_RectTexture",
		CVIBuffer_RectTexture::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_RectTexture */
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_VIBuffer_RectTexture_Dynamic",
		CVIBuffer_RectTextureDynamic::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_CubeTexture */
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_VIBuffer_CubeTexture",
		CVIBuffer_CubeTexture::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	/* For.Component_Transform */
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Transform",
		CTransform::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	/* For.Component_Optimization */
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Optimization",
		COptimization::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	/* For.Component_Texture_Logo */
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Logo",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/Logo/Logo.tga", 1))))

	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	/* For.Component_Texture_Dissolve */
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Dissolve",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Dissolve/Dissolve_%d.png", 7))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	/* For.Component_Texture_Menu */
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Menu",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/Menu/Menu0%d.tga", 3))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Loading",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/Loading/Load/Loading.tga", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_TitleLoading",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/Loading/TitleLoad/Loading.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Load_Circle_H",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/Loading/Circle/Circle_H.tga", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Load_Circle_Min",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/Loading/Circle/Circle_Min.tga", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Black",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Black.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_White",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Texture/White.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Trail",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Trail/Trail_%d.png", 11))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	///* For.Component_CSphere */
	//if (FAILED(m_pManagement->Add_Component_Prototype
	//(
	//	(_uint)ESceneType::Static,
	//	L"Component_Sphere_Collider",
	//	CSphere_Collider::Create(m_pDevice, 1.f, 20, 20)
	//)))
	//{
	//	ERROR_BOX(L"Failed to Add_Component_Prototype");
	//	return E_FAIL;
	//}

	///* For.Component_Calculator */
	//if (FAILED(pManagement->Add_Component_Prototype(
	//	(_uint)ESceneType::Static,
	//	L"Component_Calculator",
	//	CCalculator::Create(m_pDevice))))
	//{
	//	ERROR_BOX(L"Failed to Create");
	//	return E_FAIL;
	//}
#pragma endregion

#pragma region GameObject
	if (FAILED(pManagement->Add_GameObject_Prototype(
		(_uint)ESceneType::Static,
		L"GameObject_UI_Screen",
		CUI_Screen::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype(
		(_uint)ESceneType::Static,
		L"GameObject_Ending_Screen",
		CEndingCredit::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype(
		(_uint)ESceneType::Static,
		L"GameObject_MenuUI",
		CMenu_UI::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype(
		(_uint)ESceneType::Static,
		L"GameObject_Loading_Circle_H",
		CLoading_Circle_H::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype(
		(_uint)ESceneType::Static,
		L"GameObject_Loading_Circle_Min",
		CLoading_Circle_Min::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype(
		(_uint)ESceneType::Static,
		L"GameObject_MainCamera",
		CMainCamera::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype(
		(_uint)ESceneType::Static,
		L"GameObject_CinematicCamera",
		CCinematicCamera_0::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype(
		(_uint)ESceneType::Static,
		L"GameObject_TrailEffect",
		CTrailEffect::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}


#pragma endregion

	return S_OK;
}

void CMainApp::Key_Input()
{
	auto pManagement = GET_MANAGEMENT;
	if (pManagement->Key_Down(DIK_TAB))
	{
		pManagement->Debug_Render();
	}
}

void CMainApp::Test()
{
	
}

void CMainApp::Test2()
{
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;
	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pManagement);
}