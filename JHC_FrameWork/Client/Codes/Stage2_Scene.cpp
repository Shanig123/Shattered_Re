#include "stdafx.h"
#include "Stage2_Scene.h"
#include "UIBase.h"
#include "Loading_Scene.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "Shrine_Scene.h"
#include "Ending_Scene.h"

CStage2_Scene::CStage2_Scene(_DEVICE _pDevice)
	: CScene(_pDevice)
	, m_bNextScene(false)
{
}

HRESULT CStage2_Scene::Ready_Scene()
{
	CManagement* pManagement = GET_MANAGEMENT;

	CManagement::Get_Instance()->Set_StaticObjToScene((_uint)ESceneType::Stage2);
	
	ShowCursor(false);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pManagement->Get_LayerContainer((_uint)ESceneType::Stage2, L"Layer_Player")->front());
	pPlayer->Set_Init(_float3(8.4f, 15.1f, 1.1f));
	CPlayer_Weapon* pPlayerWeapon = dynamic_cast<CPlayer_Weapon*>(pManagement->Get_LayerContainer((_uint)ESceneType::Stage2, L"Layer_Player_Weapon")->front());
	pPlayerWeapon->Set_Init();
	CManagement::Get_Instance()->StopSound((_uint)CSound_Manager::ESoundID::BGM);
	CManagement::Get_Instance()->PlayBGM(L"Themes_Stage2.ogg", BGM_VOLUME);
	return S_OK;
}

_uint CStage2_Scene::Update_Scene(const _float& _fDeltaTime)
{
	CScene::Update_Scene(_fDeltaTime);

	return Scene_Change(_fDeltaTime);
}

_uint CStage2_Scene::LateUpdate_Scene(const _float& _fDeltaTime)
{
	CScene::LateUpdate_Scene(_fDeltaTime);
	

	auto pManagement = GET_MANAGEMENT;
	pManagement->Render_PhysX_Manager(_fDeltaTime);

	return (_uint)EEvent::None;
}

_uint CStage2_Scene::Render_Scene()
{
	return _uint();
}

_bool CStage2_Scene::Scene_Change(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_CHANGE_SCENE)
	{
		pManagement->Free_PhysX_Manager();

		if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Ending, CEnding_Scene::Create(m_pDevice))))
		{
			ERROR_BOX(L"Failed To Setup_CurrentScene");
			return false;
		}

		pManagement->Clear_ForScene((_int)ESceneType::Stage2);
		return (_uint)EEvent::Dead;
	}
	else
	{
		if (m_bNextScene)
		{
			pManagement->Free_PhysX_Manager();

			pManagement->Set_SceneToStaticObj((_int)ESceneType::Stage2);

			if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Loading, CLoading_Scene::Create(m_pDevice, ESceneType::Shrine))))
			{
				ERROR_BOX(L"Failed To Setup_CurrentScene");
				return false;
			}

			pManagement->Clear_ForScene((_int)ESceneType::Stage2);
			return (_uint)EEvent::Dead;
		}
	}	

	return (_uint)EEvent::None;
}

CStage2_Scene * CStage2_Scene::Create(_DEVICE _pDevice)
{
	CStage2_Scene* pInstance = new CStage2_Scene(_pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage2_Scene::Free()

{
	CScene::Free();
}
