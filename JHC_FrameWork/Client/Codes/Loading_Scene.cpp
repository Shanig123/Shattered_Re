#include "stdafx.h"
#include "Loading_Scene.h"

#include "Loading.h"
#include "Tutorial.h"
#include "Shrine_Scene.h"
#include "Stage2_Scene.h"
#include "UIBase.h"
#include "Menu_Scene.h"
CLoading_Scene::CLoading_Scene(_DEVICE _pDevice, ESceneType _eLoadSceneType)
	: CScene(_pDevice), m_eLoadSceneId(_eLoadSceneType)
	, m_fDeltaTime(0.f)
	, m_bFadeSound(false)
{
}

HRESULT CLoading_Scene::Ready_Scene()
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (FAILED(CScene::Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Ready_Scene");
		return E_FAIL;
	}

	m_pLoading = CLoading::Create(m_pDevice, m_eLoadSceneId);
	if (nullptr == m_pLoading)
	{
		ERROR_BOX(L"m_pLoading is nullptr");
		return E_FAIL;
	}

	if (FAILED(Add_LoadingUI_Layer(L"Layer_UI")))
	{
		ERROR_BOX(L"Failed to Add_LogoUI_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_Circle_H_Layer(L"Layer_UI")))
	{
		ERROR_BOX(L"Failed to Add_LogoUI_Layer");
		return E_FAIL;
	}
	if (FAILED(Add_Circle_Min_Layer(L"Layer_UI")))
	{
		ERROR_BOX(L"Failed to Add_LogoUI_Layer");
		return E_FAIL;
	}

	CUIBase::UI_DESC tDesc;
	tDesc.vPos = { 0.f,0.f,1.f };
	tDesc.vSize = { (_float)WINCX,(_float)WINCY };

	_tchar szBuff[MAX_PATH] = L"Component_Texture_Black";
	lstrcpy(tDesc.szTexName, szBuff);

	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_UI_Screen",
		(_uint)ESceneType::Loading,
		L"Layer_UI", &tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	m_bFadeSound = false;

	return S_OK;
}

_uint CLoading_Scene::Update_Scene(const _float& _fDeltaTime)
{
	CScene::Update_Scene(_fDeltaTime);

	return Scene_Change(_fDeltaTime);
}

_uint CLoading_Scene::LateUpdate_Scene(const _float& _fDeltaTime)
{
	CScene::LateUpdate_Scene(_fDeltaTime);

	CManagement* pManagement = GET_MANAGEMENT;

	if (FAILED(pManagement->Add_RenderSceneList(this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	return (_uint)EEvent::None;
}

_uint CLoading_Scene::Render_Scene()
{
	//CManagement* pManagement = GET_MANAGEMENT;

	//D3DCOLOR Color = D3DCOLOR_ARGB(255, 255, 0, 0);
	//pManagement->Render_Font(L"Font_Default", m_pLoading->Get_Text(), &_float2(15, 15), Color);

	return _uint();
}

_bool CLoading_Scene::Scene_Change(const _float& _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;

	if ((_uint)EEvent::Dead != m_pLoading->Get_Finish())
		return 0;
	m_fDeltaTime += _fDeltaTime;

	if (!m_bFadeSound && (m_eLoadSceneId != ESceneType::Menu))
	{
		Get_Manager()->FadeOutSounds(CSound_Manager::ESoundID::BGM, 7.f);
		m_bFadeSound = true;
	}

	if (m_fDeltaTime>7.f)
	{

		switch (m_eLoadSceneId)
		{
		case ESceneType::Tutorial:
			if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Tutorial, CTutorial::Create(m_pDevice))))
			{
				ERROR_BOX(L"Failed To Setup_CurrentScene");
				return false;
			}
			break;

		case ESceneType::Shrine:
			if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Shrine, CShrine_Scene::Create(m_pDevice))))
			{
				ERROR_BOX(L"Failed To Setup_CurrentScene");
				return false;
			}
			break;

		case ESceneType::Stage2:
			if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Stage2, CStage2_Scene::Create(m_pDevice))))
			{
				ERROR_BOX(L"Failed To Setup_CurrentScene");
				return false;
			}
			break;
			
		case ESceneType::Menu:
			if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Menu, CMenu_Scene::Create(m_pDevice))))
			{
				ERROR_BOX(L"Failed To Setup_CurrentScene");
				return false;
			}
			break;
		default:
			break;
		}
		pManagement->Clear_ForScene((_int)ESceneType::Loading);
		return (_uint)EEvent::Dead;
	}

	return (_uint)EEvent::None;
}

HRESULT CLoading_Scene::Add_LoadingUI_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CUIBase::UI_DESC tDesc;
	tDesc.vPos = { 0.f,0.f,1.f };
	tDesc.vSize = { (_float)WINCX,(_float)WINCY };
	if (ESceneType::Menu == m_eLoadSceneId)
	{
		_tchar szBuff[MAX_PATH] = L"Component_Texture_TitleLoading";
		lstrcpy(tDesc.szTexName, szBuff);
	}
	else
	{
		_tchar szBuff[MAX_PATH] = L"Component_Texture_Loading";
		lstrcpy(tDesc.szTexName, szBuff);
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_UI_Screen",
		(_uint)ESceneType::Loading,
		_szLayer, &tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_Scene::Add_Circle_H_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CUIBase::UI_DESC tDesc;
	if (ESceneType::Menu == m_eLoadSceneId)
	{
		tDesc.vPos = { 0.f, -_float(WINCY >> 2)+ 80.f,1.f };
		tDesc.vSize = { _float(512>>1),_float(512 >> 1) };
	}
	else
	{
		tDesc.vPos = { 0.f,0.f,1.f };
		tDesc.vSize = { 512.f,512.f };
	}
	
	_tchar szBuff[MAX_PATH] = L"Component_Texture_Load_Circle_H";
	lstrcpy(tDesc.szTexName, szBuff);


	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Loading_Circle_H",
		(_uint)ESceneType::Loading,
		_szLayer, &tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_Scene::Add_Circle_Min_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CUIBase::UI_DESC tDesc;
	if (ESceneType::Menu == m_eLoadSceneId)
	{
		tDesc.vPos = { 0.f,-_float(WINCY >> 2) + 80.f,1.f };
		tDesc.vSize = { _float(512 >> 1),_float(512 >> 1) };
	}
	else
	{
		tDesc.vPos = { 0.f,0.f,1.f };
		tDesc.vSize = { 512.f,512.f };
	}

	_tchar szBuff[MAX_PATH] = L"Component_Texture_Load_Circle_Min";
	lstrcpy(tDesc.szTexName, szBuff);


	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Loading_Circle_Min",
		(_uint)ESceneType::Loading,
		_szLayer, &tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

CLoading_Scene * CLoading_Scene::Create(_DEVICE _pDevice, ESceneType _eLoadSceneType)
{
	CLoading_Scene* pInstance = new CLoading_Scene(_pDevice, _eLoadSceneType);
	if (FAILED(pInstance->Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading_Scene::Free()
{
	Safe_Release(m_pLoading);

	CScene::Free();
}
