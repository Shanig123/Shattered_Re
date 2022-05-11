#include "stdafx.h"
#include "Logo_Scene.h"
#include "Menu_Scene.h"
#include "UIBase.h"
#include "Loading_Scene.h"

CLogo_Scene::CLogo_Scene(_DEVICE _pDevice)
	: CScene(_pDevice), m_fDeltaTime(0.f)
{
}

HRESULT CLogo_Scene::Ready_Scene()
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (FAILED(CScene::Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Ready_Scene");
		return E_FAIL;
	}

	if (FAILED(Add_LogoUI_Layer(L"Layer_LogoUI")))
	{
		ERROR_BOX(L"Failed to Add_LogoUI_Layer");
		return E_FAIL;
	}

	pManagement->PlayBGM(L"Themes_Logo.wav", BGM_VOLUME);

	return S_OK;
}

_uint CLogo_Scene::Update_Scene(const _float& _fDeltaTime)
{
	CScene::Update_Scene(_fDeltaTime);

	

	return Scene_Change(_fDeltaTime);
}

_uint CLogo_Scene::LateUpdate_Scene(const _float& _fDeltaTime)
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

_uint CLogo_Scene::Render_Scene()
{

	return _uint();
}

_bool CLogo_Scene::Scene_Change(const _float& _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	m_fDeltaTime += _fDeltaTime;
	if (m_fDeltaTime > 15.f || pManagement->Key_Down(DIK_RETURN))
	{
		if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Loading, CLoading_Scene::Create(m_pDevice,ESceneType::Menu))))
		{
			ERROR_BOX(L"Failed To Setup_CurrentScene");
			return false;
		}

		pManagement->Clear_ForScene((_int)ESceneType::Logo);

		return (_uint)EEvent::Dead;
	}

	return (_uint)EEvent::None;
}

HRESULT CLogo_Scene::Add_LogoUI_Layer(const wstring & _szLayer)
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
	_tchar szBuff[MAX_PATH] = L"Component_Texture_Logo";
	lstrcpy(tDesc.szTexName, szBuff);


	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_UI_Screen",
		(_uint)ESceneType::Logo,
		_szLayer,&tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

CLogo_Scene * CLogo_Scene::Create(_DEVICE _pDevice)
{
	CLogo_Scene* pInstance = new CLogo_Scene(_pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogo_Scene::Free()
{
	CScene::Free();
}
