#include "stdafx.h"
#include "Shrine_Scene.h"
#include "ParticleGenerator.h"
#include "UIBase.h"
#include "Loading_Scene.h"
#include "Player.h"
#include "Player_Weapon.h"


CShrine_Scene::CShrine_Scene(_DEVICE _pDevice)
	: CScene(_pDevice)
	, m_bNextScene(false)
{
}

HRESULT CShrine_Scene::Ready_Scene()
{
	CManagement* pManagement = GET_MANAGEMENT;

	CManagement::Get_Instance()->Set_StaticObjToScene((_uint)ESceneType::Shrine);
	
	ShowCursor(false);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pManagement->Get_LayerContainer((_uint)ESceneType::Shrine, L"Layer_Player")->front());
	CTransform* pTrans = dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform"));
	pPlayer->Set_Init(_float3(0.f, 1.992630f, 0.f));
	pTrans->Set_Pos(_float3(0.f, 1.992630f, 0.f));
	CPlayer_Weapon* pPlayerWeapon = dynamic_cast<CPlayer_Weapon*>(pManagement->Get_LayerContainer((_uint)ESceneType::Shrine, L"Layer_Player_Weapon")->front());
	pPlayerWeapon->Set_Init();
	CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
	CManagement::Get_Instance()->PlayBGM(L"Shrine.mp3", BGM_VOLUME);

	return S_OK;
}

_uint CShrine_Scene::Update_Scene(const _float& _fDeltaTime)
{
	CScene::Update_Scene(_fDeltaTime);
	//////////////////////////////////////////퀘스트 테스트///////////////////////////////////////////////////////
	if (Get_Manager()->Key_Down(DIK_F6))
	{
		if (!(Get_EventMgr()->Get_QuestFlag() & QUEST_START_1))
		{
			_uint i = Get_EventMgr()->Get_QuestFlag();
			Get_EventMgr()->Add_Quest_Flag(QUEST_START_1);
			i = Get_EventMgr()->Get_QuestFlag();

		}
		if((Get_EventMgr()->Get_QuestFlag() & QUEST_START_1) && !(Get_EventMgr()->Get_QuestFlag() & QUEST_END_1))
			Get_EventMgr()->Add_Quest_Flag(QUEST_END_1);
	}
	//////////////////////////////////////////퀘스트 테스트///////////////////////////////////////////////////////
	return Scene_Change(_fDeltaTime);
}

_uint CShrine_Scene::LateUpdate_Scene(const _float& _fDeltaTime)
{
	CScene::LateUpdate_Scene(_fDeltaTime);
	
	auto pManagement = GET_MANAGEMENT;
	pManagement->Render_PhysX_Manager(_fDeltaTime);

		
	return (_uint)EEvent::None;
}

_uint CShrine_Scene::Render_Scene()
{
	return _uint();
}

_bool CShrine_Scene::Scene_Change(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (m_bNextScene)
	{
		pManagement->Free_PhysX_Manager();

		pManagement->Set_SceneToStaticObj((_int)ESceneType::Shrine);


		if (FAILED(pManagement->Setup_CurrentScene((_int)ESceneType::Loading, CLoading_Scene::Create(m_pDevice, ESceneType::Stage2))))
		{
			ERROR_BOX(L"Failed To Setup_CurrentScene");
			return false;
		}

		pManagement->Clear_ForScene((_int)ESceneType::Shrine);
		return (_uint)EEvent::Dead;
	}

	return (_uint)EEvent::None;
}

CShrine_Scene * CShrine_Scene::Create(_DEVICE _pDevice)
{
	CShrine_Scene* pInstance = new CShrine_Scene(_pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShrine_Scene::Free()
{
	CScene::Free();
}
