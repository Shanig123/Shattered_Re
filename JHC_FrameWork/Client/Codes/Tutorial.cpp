#include "stdafx.h"
#include "Tutorial.h"
#include "ParticleGenerator.h"
#include "UIBase.h"
#include "Menu_UI.h"
#include "Loading_Scene.h"

CTutorial::CTutorial(_DEVICE _pDevice)
	: CScene(_pDevice)
	, m_bNextScene(false)
{
}

HRESULT CTutorial::Ready_Scene()
{
	CManagement* pManagement = GET_MANAGEMENT;

	CManagement::Get_Instance()->Set_StaticObjToScene((_uint)ESceneType::Tutorial);
	ShowCursor(false);

	return S_OK;
}

_uint CTutorial::Update_Scene(const _float& _fDeltaTime)
{
	CScene::Update_Scene(_fDeltaTime);

	return Scene_Change(_fDeltaTime);
}

_uint CTutorial::LateUpdate_Scene(const _float& _fDeltaTime)
{
	CScene::LateUpdate_Scene(_fDeltaTime);
	
	auto pManagement = GET_MANAGEMENT;
	pManagement->Render_PhysX_Manager(_fDeltaTime);

	return (_uint)EEvent::None;
}

_uint CTutorial::Render_Scene()
{
	return _uint();
}

_bool CTutorial::Scene_Change(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (pManagement->Key_Down(DIK_F6) || m_bNextScene || (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_CHANGE_SCENE))
	{
		pManagement->Free_PhysX_Manager();

		pManagement->Set_SceneToStaticObj((_int)ESceneType::Tutorial);

		if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Loading, CLoading_Scene::Create(m_pDevice, ESceneType::Shrine))))
		{
			ERROR_BOX(L"Failed To Setup_CurrentScene");
			return false;
		}

		pManagement->Clear_ForScene((_int)ESceneType::Tutorial);
		return (_uint)EEvent::Dead;
	}

	if (pManagement->Key_Down(DIK_F7))
	{
		pManagement->Free_PhysX_Manager();

		pManagement->Set_SceneToStaticObj((_int)ESceneType::Tutorial);

		if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Loading, CLoading_Scene::Create(m_pDevice, ESceneType::Stage2))))
		{
			ERROR_BOX(L"Failed To Setup_CurrentScene");
			return false;
		}

		pManagement->Clear_ForScene((_int)ESceneType::Tutorial);
		return (_uint)EEvent::Dead;
	}

	return (_uint)EEvent::None;
}

CTutorial * CTutorial::Create(_DEVICE _pDevice)
{
	CTutorial* pInstance = new CTutorial(_pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTutorial::Free()

{
	CScene::Free();
}
