#include "stdafx.h"
#include "Ending_Scene.h"
#include "UIBase.h"

CEnding_Scene::CEnding_Scene(_DEVICE _pDevice)
	: CScene(_pDevice)
{
}

HRESULT CEnding_Scene::Ready_Scene()
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (FAILED(CScene::Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Ready_Scene");
		return E_FAIL;
	}

	if (FAILED(Add_EndingUI_Layer(L"Layer_EndingUI")))
	{
		ERROR_BOX(L"Failed to Add_LogoUI_Layer");
		return E_FAIL;
	}

	return S_OK;
}

_uint CEnding_Scene::Update_Scene(const _float& _fDeltaTime)
{
	CScene::Update_Scene(_fDeltaTime);

	return (_uint)EEvent::None;
}

_uint CEnding_Scene::LateUpdate_Scene(const _float& _fDeltaTime)
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

_uint CEnding_Scene::Render_Scene()
{
	return _uint();
}


HRESULT CEnding_Scene::Add_EndingUI_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}
	CUIBase::UI_DESC tDesc;
	tDesc.vPos = { 0.f,-2400.f,1.f };
	tDesc.vSize = { (_float)WINCX, 5267.f};
	_tchar szBuff[MAX_PATH] = L"Component_Texture_EndingCredit";
	lstrcpy(tDesc.szTexName, szBuff);
	
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Ending_Screen",
		(_uint)ESceneType::Logo,
		_szLayer, &tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

CEnding_Scene * CEnding_Scene::Create(_DEVICE _pDevice)
{
	CEnding_Scene* pInstance = new CEnding_Scene(_pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnding_Scene::Free()
{
	CScene::Free();
}
