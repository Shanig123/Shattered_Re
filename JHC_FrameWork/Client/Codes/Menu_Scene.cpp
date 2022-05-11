#include "stdafx.h"
#include "Menu_Scene.h"
#include "Menu_UI.h"

#include "Loading_Scene.h"
CMenu_Scene::CMenu_Scene(_DEVICE _pDevice)
	: CScene(_pDevice)
{
}

HRESULT CMenu_Scene::Ready_Scene()
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (FAILED(CScene::Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Ready_Scene");
		return E_FAIL;
	}

	if (FAILED(Add_MenuUI_Layer(L"Layer_MenuUI")))
	{
		ERROR_BOX(L"Failed to Add_LogoUI_Layer");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMenu_Scene::Update_Scene(const _float& _fDeltaTime)
{
	CScene::Update_Scene(_fDeltaTime);

	return Scene_Change();
}

_uint CMenu_Scene::LateUpdate_Scene(const _float& _fDeltaTime)
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

_uint CMenu_Scene::Render_Scene()
{
	return _uint();
}

_bool CMenu_Scene::Scene_Change()
{
	CManagement* pManagement = GET_MANAGEMENT;

	CMenu_UI* pUI = (CMenu_UI*)pManagement->Get_GameObject((_int)ESceneType::Menu, L"Layer_MenuUI");

	if (pManagement->Key_Down(DIK_RETURN))
	{
		if (pUI->Get_IScene() == 0)
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::Effect);
			Get_Manager()->StopSound(CSound_Manager::ESoundID::BGM);

			Get_Manager()->PlayBGM(L"Loading.mp3");
			Get_Manager()->PlaySounds(L"UI_TitleStart.ogg", CSound_Manager::ESoundID::Effect, 1.f);

			if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Loading, CLoading_Scene::Create(m_pDevice, ESceneType::Tutorial))))
			{
				ERROR_BOX(L"Failed To Setup_CurrentScene");
				return false;
			}

			pManagement->Clear_ForScene((_int)ESceneType::Menu);
			return (_uint)EEvent::Dead;
		}
		else if (pUI->Get_IScene() == 1)
		{
			return (_uint)EEvent::None;
		}
		if (pUI->Get_IScene() == 2)
		{
			PostQuitMessage(0);
		}
	}

	return (_uint)EEvent::None;
}

HRESULT CMenu_Scene::Add_MenuUI_Layer(const wstring & _szLayer)
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
	_tchar szBuff[MAX_PATH] = L"Component_Texture_Menu";
	lstrcpy(tDesc.szTexName, szBuff);


	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_MenuUI",
		(_uint)ESceneType::Menu,
		_szLayer, &tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

CMenu_Scene * CMenu_Scene::Create(_DEVICE _pDevice)
{
	CMenu_Scene* pInstance = new CMenu_Scene(_pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenu_Scene::Free()
{
	CScene::Free();
}
