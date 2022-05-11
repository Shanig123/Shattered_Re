#include "stdafx.h"
#include "Loading.h"
#include "Loading_Include.h"

#pragma region CheckFinishDefine


#define PLAYER_COMPLETE			(0x01)
#define MONSTER_COMPLETE		(0x02)
#define MAP_COMPLETE			(0x04)
#define SOUND_COMPLETE			(0x08)
#define Texture_COMPLETE		(0x10)

#pragma endregion



CLoading::CLoading(_DEVICE _pDevice, ESceneType _eSceneType)
	: m_pDevice(_pDevice)
	, m_eLoadingScene(_eSceneType)
	, m_iFinish((_uint)EEvent::None)
{
	Safe_AddRef(m_pDevice);
	ZeroMemory(m_hResourceThread, sizeof(HANDLE)*(_uint)eLoadingCategory::End);
}

HRESULT CLoading::Ready_Loading(_bool _bMultiThread)
{
	InitializeCriticalSection(&m_tCriticalSection);
	if (_bMultiThread)
	{
		m_hLoadingThread = (HANDLE)_beginthreadex(0, 0, LoadingByThread, this, 0, nullptr);

		if (nullptr == m_hLoadingThread)
		{
			ERROR_BOX(L"m_hLoadingThread is nullptr");
			return E_FAIL;
		}
	}
	else
	{

	}
	return S_OK;
}

unsigned CLoading::LoadingByThread(void * _pParam)
{
	CManagement* pManagement = GET_MANAGEMENT;

	CLoading* pLoading = (CLoading*)_pParam;

	if (nullptr == pLoading)
	{
		ERROR_BOX(L"pLoding is nullptr");
		return 0;
	}

	EnterCriticalSection(&pLoading->m_tCriticalSection);

	HRESULT hr = 0;
	switch (pLoading->m_eLoadingScene)
	{
	case ESceneType::Menu:
		/*CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
		CManagement::Get_Instance()->PlayBGM(L"Themes_Menu.ogg", 0.5f);*/
		hr = pLoading->Load_PublicResources(pLoading->m_eLoadingScene);
		break;
	case ESceneType::Tutorial:
		CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
		CManagement::Get_Instance()->PlayBGM(L"Loading.mp3", 0.5f);
		hr = pLoading->Load_Stage1Resources();
		break;
	case ESceneType::Shrine:
		CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
		CManagement::Get_Instance()->PlayBGM(L"Loading.mp3", 0.5f);
		hr = pLoading->Load_ShrineResources();
		break;
	case ESceneType::Stage2:
		CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
		CManagement::Get_Instance()->PlayBGM(L"Loading.mp3", 0.5f);
		hr = pLoading->Load_Stage2Resources();
		break;
	/*case ESceneType::Stage2Area:
		hr = pLoading->Load_Stage2AreaResources();
		break;*/
	}
	while (pLoading->m_iFinish != (_uint)EEvent::Dead)
	{

	}
	if((_uint)EEvent::Dead == pLoading->m_iFinish)
	{
		LeaveCriticalSection(&pLoading->m_tCriticalSection);
		_endthreadex(0);
		return hr;
	}

	//WaitForSingleObject(pLoading->m_hLoadingThread, INFINITE);

	//pLoading->m_iFinish = (_uint)EEvent::Dead;

	LeaveCriticalSection(&pLoading->m_tCriticalSection);


	return hr;
}

HRESULT CLoading::Load_PublicResources(ESceneType _eLoadingScene)
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"Failed To Load_StageResources");
		return E_FAIL;
	}

	//테스트 해봐야 함

	if (ESceneType::Menu == _eLoadingScene)
	{
		m_hResourceThread[eLoadingCategory::PlayerMesh] = (HANDLE)_beginthreadex(0, 0, Load_PlayerResource, this, 0, nullptr);

		if (nullptr == m_hResourceThread[eLoadingCategory::PlayerMesh])
		{
			ERROR_BOX(L"m_hLoadingThread is nullptr");
			return E_FAIL;
		}

		m_hResourceThread[eLoadingCategory::MonsterMesh] = (HANDLE)_beginthreadex(0, 0, Load_MonsterResource, this, 0, nullptr);

		if (nullptr == m_hResourceThread[eLoadingCategory::MonsterMesh])
		{
			ERROR_BOX(L"m_hLoadingThread is nullptr");
			return E_FAIL;
		}

		m_hResourceThread[eLoadingCategory::MapMesh] = (HANDLE)_beginthreadex(0, 0, Load_MapStage1Resource, this, 0, nullptr);

		if (nullptr == m_hResourceThread[eLoadingCategory::MapMesh])
		{
			ERROR_BOX(L"m_hLoadingThread is nullptr");
			return E_FAIL;
		}

		m_hResourceThread[eLoadingCategory::NoMeshObj] = (HANDLE)_beginthreadex(0, 0, Load_NoMeshObjResource, this, 0, nullptr);

		if (nullptr == m_hResourceThread[eLoadingCategory::NoMeshObj])
		{
			ERROR_BOX(L"m_hLoadingThread is nullptr");
			return E_FAIL;
		}

		m_hResourceThread[eLoadingCategory::Texture] = (HANDLE)_beginthreadex(0, 0, Load_TextureResource, this, 0, nullptr);

		if (nullptr == m_hResourceThread[eLoadingCategory::Texture])
		{
			ERROR_BOX(L"m_hLoadingThread is nullptr");
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < (_uint)eLoadingCategory::End; ++i)
	{
		WaitForSingleObject(m_hResourceThread[i], INFINITE);
	}
	for (_uint i = 0; i < eLoadingCategory::End; ++i)
	{
		CloseHandle(m_hResourceThread[i]);
	}

	m_iFinish = (_uint)EEvent::Dead;


	return S_OK;
}

HRESULT CLoading::Load_Stage1Resources()
{
	auto pManagement = GET_MANAGEMENT;

	pManagement->Ready_PhysX_Manager();

	if (FAILED(Add_Player_Layer(L"Layer_Player",ESceneType::Tutorial)))
	{
		ERROR_BOX(L"Failed to Add_Player_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_Player_Weapon_Layer(L"Layer_Player_Weapon")))
	{
		ERROR_BOX(L"Failed to Add_Player_Weapon_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_Camera_Layer(L"Layer_Camera")))
	{
		ERROR_BOX(L"Failed to Add_Camera_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_MapStage1_Layer(L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	if (FAILED(Add_MonsterStage1_Layer(L"Layer_Monster")))
	{
		ERROR_BOX(L"Failed to Add_Player_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_Cinematic_Camera_Layer(L"Layer_CinemaCamera")))
	{
		ERROR_BOX(L"Failed to Add_Camera_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_EffectGenerator_Layer(L"Layer_Effect")))
	{
		ERROR_BOX(L"Failed to Add_EffectGenerator_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_Light()))
	{
		ERROR_BOX(L"Failed to Add_Light");
		return E_FAIL;
	}
	if (FAILED(Add_SkyBox(L"Layer_SkyBox")))
	{
		ERROR_BOX(L"Failed to Add_SkyBox");
		return E_FAIL;
	}

	m_iFinish = (_uint)EEvent::Dead;


	return S_OK;
}

HRESULT CLoading::Load_ShrineResources()
{
	CManagement* pManagement = GET_MANAGEMENT;

	pManagement->Ready_PhysX_Manager();

	{
		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Shrine.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		vector<RESOURCEINFO_DESC*> vecMapResource = pManagement->Get_MapResource();	
		wstring wstrPrototypeName = L"";
		CGameObject* pGameObject = nullptr;

		for (_uint i = 0; i < vecMapResource.size(); ++i)
		{
			wstrPrototypeName = L"StaticMesh_Map_" + (wstring)vecMapResource[i]->szFileTitle;
			if (pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, wstrPrototypeName))
				continue;

			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CStaticMesh::Create(m_pDevice, vecMapResource[i]->szFilePath, vecMapResource[i]->szFileName)
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}
		}

#pragma region Texture
		if (FAILED(pManagement->Add_Texture_Prototype(
			(_uint)ESceneType::Static,
			L"Component_Texture_ChoronUI",
			CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/NPC_Choron_UI/%d.png", 8))))
		{
			ERROR_BOX(L"Failed to Create");
			return E_FAIL;
		}

		if (FAILED(pManagement->Add_Texture_Prototype(
			(_uint)ESceneType::Static,
			L"Component_Texture_ChoronScript",
			CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/NPC_Choron_Script/%d.png", 13))))
		{
			ERROR_BOX(L"Failed to Create");
			return E_FAIL;
		}
		if (FAILED(pManagement->Add_Texture_Prototype(
			(_uint)ESceneType::Static,
			L"Component_Texture_HelloScript",
			CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/NPC_HelloWorld/%d.png", 4))))
		{
			ERROR_BOX(L"Failed to Create");
			return E_FAIL;
		}

#pragma endregion

#pragma region MapObject
		if (FAILED(pManagement->MapObjectResourcesLoad(L"../../Data/Shrine_Object.Resourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		vector<RESOURCEINFO_DESC*> vecMapObjectResource = pManagement->Get_MapObjectResource();

		for (_uint i = 0; i < vecMapObjectResource.size(); ++i)
		{
			wstring wstrFilePath = vecMapObjectResource[i]->szFilePath;
			if (wstrFilePath.find(L"Static") != wstring::npos)
			{
				wstrPrototypeName = L"StaticMesh_" + (wstring)vecMapObjectResource[i]->szFileTitle;
				if (pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, wstrPrototypeName))
					continue;

				if (FAILED(pManagement->Add_Mesh_Prototype
				(
					(_uint)ESceneType::Static,
					wstrPrototypeName,
					CStaticMesh::Create(m_pDevice, vecMapObjectResource[i]->szFilePath, vecMapObjectResource[i]->szFileName)
				)))
				{
					ERROR_BOX(L"Failed to Add_Component_Prototype");
					return E_FAIL;
				}

				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"ShrineDeco"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_Deco::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"IronDoor"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_IronDoor::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"IronDoorFrame"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_IronDoorFrame::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"LevelStone"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_LevelStone::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
			}
			else
			{
				/*wstrPrototypeName = L"DynamicMesh_" + (wstring)vecMapObjectResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_Mesh_Prototype
				(
					(_uint)ESceneType::Static,
					wstrPrototypeName,
					CDynamicMesh::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFilePath, vecMapObjectResource[i]->szFileName)
				)))
				{
					ERROR_BOX(L"Failed to Add_Component_Prototype");
					return E_FAIL;
				}*/
			}

			if (nullptr != pGameObject)
			{
				wstrPrototypeName = L"GameObject_" + (wstring)vecMapObjectResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObject)))
				{
					ERROR_BOX(L"Failed To Add_GameObject_Prototype");
					return E_FAIL;
				}
			}
		}


		if (FAILED(pManagement->MapObjectLoad(L"../../Data/Shrine_Object.Monsterdat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

#pragma endregion

#pragma region NPC
		/* DynamicMesh_Choron */

		if (!pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, L"DynamicMesh_Choron"))
		{
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				L"DynamicMesh_Choron",
				CDynamicMesh::Create(m_pDevice, L"../../Resources/Mesh/Dynamic/NPC/Choron/", L"Choron.x")
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			/* For.GameObject_Choron */
			if (FAILED(pManagement->Add_GameObject_Prototype(
				(_uint)ESceneType::Static,
				L"GameObject_Choron",
				CNPC_Choron::Create(m_pDevice, L"Choron"))))
			{
				ERROR_BOX(L"Failed To Add_GameObject_Prototype");
				return E_FAIL;
			}
		}
		
		if (!pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, L"DynamicMesh_Valnir"))
		{
			/* DynamicMesh_Valnir */
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				L"DynamicMesh_Valnir",
				CDynamicMesh::Create(m_pDevice, L"../../Resources/Mesh/Dynamic/NPC/Valnir/", L"Valnir.x")
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			/* For.GameObject_Valnir */
			if (FAILED(pManagement->Add_GameObject_Prototype(
				(_uint)ESceneType::Static,
				L"GameObject_Valnir",
				CNPC_Valnir::Create(m_pDevice, L"Valnir"))))
			{
				ERROR_BOX(L"Failed To Add_GameObject_Prototype");
				return E_FAIL;
			}
		}
		
#pragma endregion

	}

	if (FAILED(Add_NPC_Layer(L"Layer_NPC")))
	{
		ERROR_BOX(L"Failed to Layer_NPC");
		return E_FAIL;
	}

	if (FAILED(Add_Camera_Layer(L"Layer_Camera")))
	{
		ERROR_BOX(L"Failed to Add_Camera_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_MapShrine_Layer(L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	if (FAILED(Add_Light()))
	{
		ERROR_BOX(L"Failed to Add_Light");
		return E_FAIL;
	}

	if (FAILED(Add_SkyBox(L"Layer_SkyBox")))
	{
		ERROR_BOX(L"Failed to Add_SkyBox");
		return E_FAIL;
	}

	//Load_Stage2AreaResources();

	m_iFinish = (_uint)EEvent::Dead;

	return S_OK;
}

HRESULT CLoading::Load_Stage2Resources()
{
	CManagement* pManagement = GET_MANAGEMENT;

	pManagement->Ready_PhysX_Manager();

	{
		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Area1.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Area2.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Area3.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Area4.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Area5.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Area6.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Area7.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Area8.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2BossRoom.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Chess.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage2Trap.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		vector<RESOURCEINFO_DESC*> vecMapResource = pManagement->Get_MapResource();
		wstring wstrPrototypeName = L"";
		CGameObject* pGameObject = nullptr;

		for (_uint i = 0; i < vecMapResource.size(); ++i)
		{
			wstrPrototypeName = L"StaticMesh_Map_" + (wstring)vecMapResource[i]->szFileTitle;
			if (pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, wstrPrototypeName))
				continue;

			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CStaticMesh::Create(m_pDevice, vecMapResource[i]->szFilePath, vecMapResource[i]->szFileName)
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}
		}

#pragma region Monster
		if (FAILED(pManagement->MonsterResourcesLoad(L"../../Data/Stage2_Monster.Resourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		vector<RESOURCEINFO_DESC*> vecMonsterResource = pManagement->Get_MonsterResource();
		CGameObject* pGameObjectMonster = nullptr;
		CGameObject* pGameObjectBoss = nullptr;

		for (_uint i = 0; i < vecMonsterResource.size(); ++i)
		{
			wstrPrototypeName = L"";
			pGameObjectMonster = nullptr;
			pGameObjectBoss = nullptr;

			wstrPrototypeName = L"DynamicMesh_" + (wstring)vecMonsterResource[i]->szFileTitle;
			if (pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, wstrPrototypeName))
				continue;

			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CDynamicMesh::Create(m_pDevice, vecMonsterResource[i]->szFilePath, vecMonsterResource[i]->szFileName)
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			if (!lstrcmpi(vecMonsterResource[i]->szFileTitle, L"Yantari"))
			{
				pGameObjectMonster = dynamic_cast<CGameObject*>(CJYantari::Create(m_pDevice, vecMonsterResource[i]->szFileTitle));
			}

			if (!lstrcmpi(vecMonsterResource[i]->szFileTitle, L"Malkhel"))
			{
				pGameObjectBoss = dynamic_cast<CGameObject*>(CJMalkhel::Create(m_pDevice, vecMonsterResource[i]->szFileTitle));
			}

			if (nullptr != pGameObjectMonster)
			{
				wstrPrototypeName = L"GameObject_" + (wstring)vecMonsterResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
				{
					ERROR_BOX(L"Failed To Add_GameObject_Prototype");
					return E_FAIL;
				}
			}

			if (nullptr != pGameObjectBoss)
			{
				wstrPrototypeName = L"GameObject_" + (wstring)vecMonsterResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectBoss)))
				{
					ERROR_BOX(L"Failed To Add_GameObject_Prototype");
					return E_FAIL;
				}
			}
		}

#pragma region Malkhel_Lughan
#pragma region Weapon
		wstrPrototypeName = L"StaticMesh_Malkhel_Lughan";
		if (!pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, wstrPrototypeName))
		{
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CStaticMesh::Create(m_pDevice, L"../../Resources/Mesh/Static/Monster_Weapon/Malkhel_Lughan/", L"Malkhel_Lughan.X")
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			pGameObjectMonster = dynamic_cast<CGameObject*>(CJMalkhel_Lughan::Create(m_pDevice, L"Malkhel_Lughan"));
			wstrPrototypeName = L"GameObject_Malkhel_Lughan";
			if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
			{
				ERROR_BOX(L"Failed To Add_GameObject_Prototype");
				return E_FAIL;
			}
		}
#pragma endregion

#pragma region FireBall
		wstrPrototypeName = L"StaticMesh_Malkhel_FireBall";
		if (!pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, L"Malkhel_FireBall"))
		{
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CStaticMesh::Create(m_pDevice, L"../../Resources/Mesh/Static/Malkhel_Effect/", L"Malkhel_FireBall.X")
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			pGameObjectMonster = dynamic_cast<CGameObject*>(CJMalkhel_FireBall::Create(m_pDevice, L"Malkhel_FireBall"));
			wstrPrototypeName = L"GameObject_Malkhel_FireBall";
			if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
			{
				ERROR_BOX(L"Failed To Add_GameObject_Prototype");
				return E_FAIL;
			}

			wstrPrototypeName = L"StaticMesh_Malkhel_FireBall_Page2";
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CStaticMesh::Create(m_pDevice, L"../../Resources/Mesh/Static/Malkhel_Effect/", L"Malkhel_FireBall_Page2.X")
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			wstrPrototypeName = L"StaticMesh_Malkhel_FireBall_BackEffect";
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CStaticMesh::Create(m_pDevice, L"../../Resources/Mesh/Static/Malkhel_Effect/", L"Malkhel_FireBall_BackEffect.X")
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			pGameObjectMonster = dynamic_cast<CGameObject*>(CJMalkhel_FireBallBack::Create(m_pDevice, L"Malkhel_FireBall_BackEffect"));
			wstrPrototypeName = L"GameObject_Malkhel_FireBall_BackEffect";
			if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
			{
				ERROR_BOX(L"Failed To Add_GameObject_Prototype");
				return E_FAIL;
			}
		}
#pragma endregion

#pragma region Spear
		wstrPrototypeName = L"StaticMesh_Malkhel_Spear";
		if (!pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, wstrPrototypeName))
		{
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CStaticMesh::Create(m_pDevice, L"../../Resources/Mesh/Static/Malkhel_Effect/", L"Malkhel_Spear.X")
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			pGameObjectMonster = dynamic_cast<CGameObject*>(CJMalkhel_Spear::Create(m_pDevice, L"Malkhel_Spear"));
			wstrPrototypeName = L"GameObject_Malkhel_Spear";
			if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
			{
				ERROR_BOX(L"Failed To Add_GameObject_Prototype");
				return E_FAIL;
			}
		}
#pragma endregion

#pragma region Effect_360
		wstrPrototypeName = L"StaticMesh_Malkhel_Effect_360";
		if (!pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, L"Malkhel_Effect_360"))
		{
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CStaticMesh::Create(m_pDevice, L"../../Resources/Mesh/Static/Malkhel_Effect/", L"Malkhel_Effect_360.X")
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			pGameObjectMonster = dynamic_cast<CGameObject*>(CJMalkhel_Effect_360::Create(m_pDevice, L"Malkhel_Effect_360"));
			wstrPrototypeName = L"GameObject_Malkhel_Effect_360";
			if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
			{
				ERROR_BOX(L"Failed To Add_GameObject_Prototype");
				return E_FAIL;
			}
		}
#pragma endregion

#pragma endregion

#pragma region MapObject
		if (FAILED(pManagement->MapObjectResourcesLoad(L"../../Data/Stage2_Object.Resourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapObjectResourcesLoad(L"../../Data/Stage2_Chess.Resourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		vector<RESOURCEINFO_DESC*> vecMapObjectResource = pManagement->Get_MapObjectResource();

		for (_uint i = 0; i < vecMapObjectResource.size(); ++i)
		{
			pGameObject = nullptr;

			wstring wstrFilePath = vecMapObjectResource[i]->szFilePath;
			if (wstrFilePath.find(L"Static") != wstring::npos)
			{
				wstrPrototypeName = L"StaticMesh_" + (wstring)vecMapObjectResource[i]->szFileTitle;
				if (pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, wstrPrototypeName))
					continue;

				if (FAILED(pManagement->Add_Mesh_Prototype
				(
					(_uint)ESceneType::Static,
					wstrPrototypeName,
					CStaticMesh::Create(m_pDevice, vecMapObjectResource[i]->szFilePath, vecMapObjectResource[i]->szFileName)
				)))
				{
					ERROR_BOX(L"Failed to Add_Component_Prototype");
					return E_FAIL;
				}

				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"Bridge"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_Bridge::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}

				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"ShortCutDoor"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_ShortCutDoor::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}

				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"Queen"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CChessPiece_Queen::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}

				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"Knight"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CChessPiece_Knight::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}

				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"YaakStand"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_YaakStand::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}

				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"Stage2Deco"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_Deco::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}

				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"ChurchDoor"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_ChurchDoor::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
			}
			else
			{
				wstrPrototypeName = L"DynamicMesh_" + (wstring)vecMapObjectResource[i]->szFileTitle;
				if (pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, wstrPrototypeName))
					continue;

				if (FAILED(pManagement->Add_Mesh_Prototype
				(
					(_uint)ESceneType::Static,
					wstrPrototypeName,
					CDynamicMesh::Create(m_pDevice, vecMapObjectResource[i]->szFilePath, vecMapObjectResource[i]->szFileName)
				)))
				{
					ERROR_BOX(L"Failed to Add_Component_Prototype");
					return E_FAIL;
				}
				/*if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"SavePoint"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapDynamicObject_SavePoint::Create(m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}*/
			}

			if (nullptr != pGameObject)
			{
				wstrPrototypeName = L"GameObject_" + (wstring)vecMapObjectResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObject)))
				{
					ERROR_BOX(L"Failed To Add_GameObject_Prototype");
					return E_FAIL;
				}
			}
		}

		if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, L"GameObject_Chess", CChess::Create(m_pDevice))))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}

#pragma endregion
		if (FAILED(pManagement->MonsterLoad(L"../../Data/Stage2_Monster.Monsterdat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->MapObjectLoad(L"../../Data/Stage2_Object.Monsterdat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		
#pragma region ChessUI
		if (!pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, L"DynamicMesh_Yaak"))
		{
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				L"DynamicMesh_Yaak",
				CDynamicMesh::Create(m_pDevice, L"../../Resources/Mesh/Dynamic/NPC/Yaak/", L"Yaak.x")
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			/* For.GameObject_Yaak */
			if (FAILED(pManagement->Add_GameObject_Prototype(
				(_uint)ESceneType::Static,
				L"GameObject_Yaak",
				CNPC_Yaak::Create(m_pDevice, L"Yaak"))))
			{
				ERROR_BOX(L"Failed To Add_GameObject_Prototype");
				return E_FAIL;
			}
		}		

		if (FAILED(pManagement->Add_Texture_Prototype(
			(_uint)ESceneType::Static,
			L"Component_Texture_YaakUI",
			CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/NPC_Yakk_UI/%d.png", 5))))
		{
			ERROR_BOX(L"Failed to Create");
			return E_FAIL;
		}

		if (FAILED(pManagement->Add_Texture_Prototype(
			(_uint)ESceneType::Static,
			L"Component_Texture_YaakScript",
			CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/UI/NPC_Yakk_Script/%d.png", 2))))
		{
			ERROR_BOX(L"Failed to Create");
			return E_FAIL;
		}
#pragma endregion

	}

	if (FAILED(Add_Camera_Layer(L"Layer_Camera")))
	{
		ERROR_BOX(L"Failed to Add_Camera_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_MapStage2_Layer(L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

	if (FAILED(Add_MonsterStage2_Layer(L"Layer_Monster")))
	{
		ERROR_BOX(L"Failed to Add_Player_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_MonsterStage2ChessQueen_Layer(L"Layer_ChessQueen")))
	{
		ERROR_BOX(L"Failed to Add_Player_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_MonsterStage2ChessKnight_Layer(L"Layer_ChessKnight")))
	{
		ERROR_BOX(L"Failed to Add_Player_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_NPCStage2_Layer(L"Layer_NPC")))
	{
		ERROR_BOX(L"Failed to Add_NPCStage2_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_Light_Stage2()))
	{
		ERROR_BOX(L"Failed to Add_Light");
		return E_FAIL;
	}

	if (FAILED(Add_SkyBox(L"Layer_SkyBox")))
	{
		ERROR_BOX(L"Failed to Add_SkyBox");
		return E_FAIL;
	}

	if (FAILED(Add_Cinematic_Camera_Layer(L"Layer_CinemaCamera")))
	{
		ERROR_BOX(L"Failed to Add_Camera_Layer");
		return E_FAIL;
	}

	m_iFinish = (_uint)EEvent::Dead;

	return S_OK;
}

//void CLoading::Load_Stage2AreaResources()
//{
//	auto pManagement = GET_MANAGEMENT;
//
//	
//
//	vector<RESOURCEINFO_DESC*> vecMapResource = pManagement->Get_MapResource();
//	wstring wstrPrototypeName = L"";
//	CGameObject* pGameObject = nullptr;
//
//	for (_uint i = 0; i < vecMapResource.size(); ++i)
//	{
//		wstrPrototypeName = L"StaticMesh_Map_" + (wstring)vecMapResource[i]->szFileTitle;
//		if (pManagement->Find_Mesh_Prototype((_uint)ESceneType::Static, wstrPrototypeName))
//			continue;
//
//		if (FAILED(pManagement->Add_Mesh_Prototype
//		(
//			(_uint)ESceneType::Static,
//			wstrPrototypeName,
//			CStaticMesh::Create(m_pDevice, vecMapResource[i]->szFilePath, vecMapResource[i]->szFileName)
//		)))
//		{
//			ERROR_BOX(L"Failed to Add_Component_Prototype");
//			return ;
//		}
//	}
//}

#pragma region Resource
unsigned CLoading::Load_PlayerResource(void * _pParam)
{
	CManagement* pManagement = GET_MANAGEMENT;

	CLoading* pLoading = (CLoading*)_pParam;

	if (nullptr == pLoading)
	{
		ERROR_BOX(L"pLoding is nullptr");
		return 0;
	}

	{
		/* DynamicMesh_Player */
		if (FAILED(pManagement->Add_Mesh_Prototype
		(
			(_uint)ESceneType::Static,
			L"DynamicMesh_Player",
			CDynamicMesh::Create(pLoading->m_pDevice, L"../../Resources/Mesh/Dynamic/Player/", L"Player.x")
		)))
		{
			ERROR_BOX(L"Failed to Add_Component_Prototype");
			return E_FAIL;
		}
		/* For.GameObject_Player */
		if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static,
			L"GameObject_Player",
			CPlayer::Create(pLoading->m_pDevice))))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}


		if (FAILED(pManagement->Add_Mesh_Prototype
		(
			(_uint)ESceneType::Static,
			L"StaticMesh_Player_Normal_Weapon",
			CStaticMesh::Create(pLoading->m_pDevice, L"../../Resources/Mesh/Static/Player_Weapon/1/", L"Normal_Weapon.x")
		)))
		{
			ERROR_BOX(L"Failed to Add_Component_Prototype");
			return E_FAIL;
		}
		if (FAILED(pManagement->Add_Mesh_Prototype
		(
			(_uint)ESceneType::Static,
			L"StaticMesh_Player_Moon_Sword",
			CStaticMesh::Create(pLoading->m_pDevice, L"../../Resources/Mesh/Static/Player_Weapon/2/MoonSword/", L"MoonSword.x")
		)))
		{
			ERROR_BOX(L"Failed to Add_Component_Prototype");
			return E_FAIL;
		}
		if (FAILED(pManagement->Add_GameObject_Prototype
		(
			(_uint)ESceneType::Static,
			L"GameObject_Player_Weapon",
			CPlayer_Weapon::Create(pLoading->m_pDevice)
		)))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}
	}


	_endthreadex(0);


	return 0;
}

unsigned CLoading::Load_MonsterResource(void * _pParam)
{
	CManagement* pManagement = GET_MANAGEMENT;

	CLoading* pLoading = (CLoading*)_pParam;

	if (nullptr == pLoading)
	{
		ERROR_BOX(L"pLoding is nullptr");
		return 0;
	}


	{
		if (FAILED(pManagement->MonsterResourcesLoad(L"../../Data/Stage1_Monster.Resourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		vector<RESOURCEINFO_DESC*> vecMonsterResource = pManagement->Get_MonsterResource();
		wstring wstrPrototypeName = L"";
		CGameObject* pGameObjectMonster = nullptr;
		CGameObject* pGameObjectBoss = nullptr;

		for (_uint i = 0; i < vecMonsterResource.size(); ++i)
		{
			wstrPrototypeName = L"";
			pGameObjectMonster = nullptr;
			pGameObjectBoss = nullptr;

			wstrPrototypeName = L"DynamicMesh_" + (wstring)vecMonsterResource[i]->szFileTitle;
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CDynamicMesh::Create(pLoading->m_pDevice, vecMonsterResource[i]->szFilePath, vecMonsterResource[i]->szFileName)
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}

			if (!lstrcmpi(vecMonsterResource[i]->szFileTitle, L"Homonculus"))
			{
				pGameObjectMonster = dynamic_cast<CGameObject*>(CJHomonculus::Create(pLoading->m_pDevice, vecMonsterResource[i]->szFileTitle));
			}
			if (!lstrcmpi(vecMonsterResource[i]->szFileTitle, L"Legionnaire"))
			{
				pGameObjectMonster = dynamic_cast<CGameObject*>(CJLegionnaire::Create(pLoading->m_pDevice, vecMonsterResource[i]->szFileTitle));
			}
			
			if (!lstrcmpi(vecMonsterResource[i]->szFileTitle, L"Preto"))
			{
				pGameObjectBoss = dynamic_cast<CGameObject*>(CJPreto::Create(pLoading->m_pDevice, vecMonsterResource[i]->szFileTitle));
			}

			if (nullptr != pGameObjectMonster)
			{
				wstrPrototypeName = L"GameObject_" + (wstring)vecMonsterResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
				{
					ERROR_BOX(L"Failed To Add_GameObject_Prototype");
					return E_FAIL;
				}
			}

			if (nullptr != pGameObjectBoss)
			{
				wstrPrototypeName = L"GameObject_" + (wstring)vecMonsterResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectBoss)))
				{
					ERROR_BOX(L"Failed To Add_GameObject_Prototype");
					return E_FAIL;
				}
			}
		}

		if (FAILED(pManagement->MonsterLoad(L"../../Data/Stage1_Monster.Monsterdat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}



#pragma region Weapon

#pragma region Legionnaire
		wstrPrototypeName = L"StaticMesh_LegionnaireSword";
		if (FAILED(pManagement->Add_Mesh_Prototype
		(
			(_uint)ESceneType::Static,
			wstrPrototypeName,
			CStaticMesh::Create(pLoading->m_pDevice, L"../../Resources/Mesh/Static/Monster_Weapon/Legionnaire/", L"LegionnaireSword.X")
		)))
		{
			ERROR_BOX(L"Failed to Add_Component_Prototype");
			return E_FAIL;
		}

		pGameObjectMonster = dynamic_cast<CGameObject*>(CJLegionnaireSword::Create(pLoading->m_pDevice, L"LegionnaireSword"));
		wstrPrototypeName = L"GameObject_LegionnaireSword";
		if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}

		wstrPrototypeName = L"StaticMesh_LegionnaireGun";
		if (FAILED(pManagement->Add_Mesh_Prototype
		(
			(_uint)ESceneType::Static,
			wstrPrototypeName,
			CStaticMesh::Create(pLoading->m_pDevice, L"../../Resources/Mesh/Static/Monster_Weapon/Legionnaire/", L"LegionnaireGun.X")
		)))
		{
			ERROR_BOX(L"Failed to Add_Component_Prototype");
			return E_FAIL;
		}

		pGameObjectMonster = dynamic_cast<CGameObject*>(CJLegionnaireGun::Create(pLoading->m_pDevice, L"LegionnaireGun"));
		wstrPrototypeName = L"GameObject_LegionnaireGun";
		if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}

		//wstrPrototypeName = L"StaticMesh_LegionnaireBullet";
		//if (FAILED(pManagement->Add_Mesh_Prototype
		//(
		//	(_uint)ESceneType::Static,
		//	wstrPrototypeName,
		//	CStaticMesh::Create(pLoading->m_pDevice, L"../../Resources/Mesh/Static/Monster_Weapon/Legionnaire/", L"LegionnaireGun.X")
		//)))
		//{
		//	ERROR_BOX(L"Failed to Add_Component_Prototype");
		//	return E_FAIL;
		//}

		pGameObjectMonster = dynamic_cast<CGameObject*>(CJLegionnaireBullet::Create(pLoading->m_pDevice, L"LegionnaireBullet"));
		wstrPrototypeName = L"GameObject_LegionnaireBullet";
		if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}
#pragma endregion

#pragma region YantariAxe
		wstrPrototypeName = L"StaticMesh_YantariAxe";
		if (FAILED(pManagement->Add_Mesh_Prototype
		(
			(_uint)ESceneType::Static,
			wstrPrototypeName,
			CStaticMesh::Create(pLoading->m_pDevice, L"../../Resources/Mesh/Static/Monster_Weapon/YantariAxe/", L"YantariAxe.X")
		)))
		{
			ERROR_BOX(L"Failed to Add_Component_Prototype");
			return E_FAIL;
		}

		pGameObjectMonster = dynamic_cast<CGameObject*>(CJYantariAxe::Create(pLoading->m_pDevice,L"YantariAxe"));
		wstrPrototypeName = L"GameObject_YantariAxe";
		if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectMonster)))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}
#pragma endregion

#pragma region PretoWeapon
		wstrPrototypeName = L"StaticMesh_PretoShield";
		if (FAILED(pManagement->Add_Mesh_Prototype
		(
			(_uint)ESceneType::Static,
			wstrPrototypeName,
			CStaticMesh::Create(pLoading->m_pDevice, L"../../Resources/Mesh/Static/Monster_Weapon/PretoShield/", L"PretoShield.X")
		)))
		{
			ERROR_BOX(L"Failed to Add_Component_Prototype");
			return E_FAIL;
		}
		pGameObjectBoss = dynamic_cast<CGameObject*>(CJPretoShield::Create(pLoading->m_pDevice, L"PretoShield"));
		wstrPrototypeName = L"GameObject_PretoShield";
		if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObjectBoss)))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}
		pGameObjectBoss = dynamic_cast<CGameObject*>(CJPretoSickle::Create(pLoading->m_pDevice, L"PretoSickle"));
		wstrPrototypeName = L"GameObject_PretoSickle";
		if (FAILED(pManagement->Add_GameObject_Prototype(
			(_uint)ESceneType::Static, wstrPrototypeName, pGameObjectBoss)))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}
#pragma endregion

#pragma endregion

	}


	_endthreadex(0);

	return 0;
}

unsigned CLoading::Load_MapStage1Resource(void * _pParam)
{
	CManagement* pManagement = GET_MANAGEMENT;

	CLoading* pLoading = (CLoading*)_pParam;

	if (nullptr == pLoading)
	{
		ERROR_BOX(L"pLoding is nullptr");
		return 0;
	}

	{
		if (FAILED(pManagement->MapResourcesLoad(L"../../Data/Stage1.MapResourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		vector<RESOURCEINFO_DESC*> vecMapResource = pManagement->Get_MapResource();
		wstring wstrPrototypeName = L"";
		CGameObject* pGameObject = nullptr;

		for (_uint i = 0; i < vecMapResource.size(); ++i)
		{
			wstrPrototypeName = L"StaticMesh_Map_" + (wstring)vecMapResource[i]->szFileTitle;
			if (FAILED(pManagement->Add_Mesh_Prototype
			(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				CStaticMesh::Create(pLoading->m_pDevice, vecMapResource[i]->szFilePath, vecMapResource[i]->szFileName)
			)))
			{
				ERROR_BOX(L"Failed to Add_Component_Prototype");
				return E_FAIL;
			}
		}
#pragma region MapObject
		if (FAILED(pManagement->MapObjectResourcesLoad(L"../../Data/Stage1_Object.Resourcedat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		vector<RESOURCEINFO_DESC*> vecMapObjectResource = pManagement->Get_MapObjectResource();

		for (_uint i = 0; i < vecMapObjectResource.size(); ++i)
		{
			wstring wstrFilePath = vecMapObjectResource[i]->szFilePath;
			if (wstrFilePath.find(L"Static") != wstring::npos)
			{
				wstrPrototypeName = L"StaticMesh_" + (wstring)vecMapObjectResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_Mesh_Prototype
				(
					(_uint)ESceneType::Static,
					wstrPrototypeName,
					CStaticMesh::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFilePath, vecMapObjectResource[i]->szFileName)
				)))
				{
					ERROR_BOX(L"Failed to Add_Component_Prototype");
					return E_FAIL;
				}

				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"WeaponStand"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_WeaponStand::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"Elevator"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_Elevator::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"Item"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CItemObj::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"PortalGate"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_PortalGate::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"Stage1Deco"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_Deco::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"StoneDoor"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapStaticObject_StoneDoor::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
			}
			else
			{
				wstrPrototypeName = L"DynamicMesh_" + (wstring)vecMapObjectResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_Mesh_Prototype
				(
					(_uint)ESceneType::Static,
					wstrPrototypeName,
					CDynamicMesh::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFilePath, vecMapObjectResource[i]->szFileName)
				)))
				{
					ERROR_BOX(L"Failed to Add_Component_Prototype");
					return E_FAIL;
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"SavePoint"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapDynamicObject_SavePoint::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"FallingFlat"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapDynamicObject_FallingFlat::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"Lever"))
				{
					pGameObject = dynamic_cast<CGameObject*>(CMapDynamicObject_Lever::Create(pLoading->m_pDevice, vecMapObjectResource[i]->szFileTitle));
				}
			}

			if (nullptr != pGameObject)
			{
				wstrPrototypeName = L"GameObject_" + (wstring)vecMapObjectResource[i]->szFileTitle;
				if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName, pGameObject)))
				{
					ERROR_BOX(L"Failed To Add_GameObject_Prototype");
					return E_FAIL;
				}
				if (!lstrcmpi(vecMapObjectResource[i]->szFileTitle, L"Item"))
				{
					wstring wstrPrototypeName2 = L"GameObject_" + (wstring)vecMapObjectResource[i]->szFileTitle;
					wstrPrototypeName2 += L"_Special";
					CGameObject* pGameObject2 = dynamic_cast<CGameObject*>(CItem_Special::Create(pLoading->m_pDevice, wstrPrototypeName2));

					if (nullptr != pGameObject2)
					{
						if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, wstrPrototypeName2, pGameObject2)))
						{
							ERROR_BOX(L"Failed To Add_GameObject_Prototype");
							return E_FAIL;
						}
					}
				}
			}
		}

		
		if (FAILED(pManagement->MapObjectLoad(L"../../Data/Stage1_Object.Monsterdat")))
		{
			ERROR_BOX(L"Failed To ResourceLoad");
			return E_FAIL;
		}

		if (FAILED(pManagement->Add_Mesh_Prototype
		(
			(_uint)ESceneType::Static,
			L"StaticMesh_Rune",
			CStaticMesh::Create(pLoading->m_pDevice, L"../../Resources/Mesh/Static/Rune/", L"Rune.X")
		)))
		{
			ERROR_BOX(L"Failed to Add_Component_Prototype");
			return E_FAIL;
		}

#pragma endregion

	}

	_endthreadex(0);


	return 0;
}

unsigned CLoading::Load_NoMeshObjResource(void * _pParam)
{
	CManagement* pManagement = GET_MANAGEMENT;

	CLoading* pLoading = (CLoading*)_pParam;


	{

		if (nullptr == pLoading)
		{
			ERROR_BOX(L"pLoding is nullptr");
			return 0;
		}
		if (FAILED(pManagement->Add_GameObject_Prototype(
			(_uint)ESceneType::Static,
			L"GameObject_Effect_Generator",
			CParticleGenerator::Create(pLoading->m_pDevice))))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}

		if (FAILED(pManagement->Add_GameObject_Prototype(
			(_uint)ESceneType::Static,
			L"GameObject_Event_ObjGenerator",
			CShrine_Event_ObjGenerator::Create(pLoading->m_pDevice))))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}


		if (FAILED(pManagement->Add_GameObject_Prototype(
			(_uint)ESceneType::Static,
			L"GameObject_Tex_Obj",
			CEffect_Texture::Create(pLoading->m_pDevice))))
		{
			ERROR_BOX(L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}


	}

	_endthreadex(0);

	
	return 0;
}

unsigned CLoading::Load_TextureResource(void * _pParam)
{
	CManagement* pManagement = GET_MANAGEMENT;

	CLoading* pLoading = (CLoading*)_pParam;


	if (nullptr == pLoading)
	{
		ERROR_BOX(L"pLoding is nullptr");
		return 0;
	}
	{
		///////////////지워야함//////////////////
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Number",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Number/Num_%d.png", 13))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Item_Icon",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Item_Icon/Item_Icon_%d.png", 27))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	///////////////지워야함//////////////////
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_LockOn",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/LockOn/LockOn%d.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_FlavorTexture",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Item_FlavorText/Item_FlavorText_%d.png", 16))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Area",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Area/Area%d.png", 3))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Frame",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Frame/Frame%d.png", 32))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Interact",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Player_UI/Interact/Interact%d.png", 3))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Item_MSG",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Get_Item_MSG/Item_MSG%d.png", 16))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Number",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Number/Num_%d.png", 13))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Item_Icon",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Item_Icon/Item_Icon_%d.png", 27))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Select_Upgrade_Back",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/UI_Select_Stat_Or_Forge/UI_Select_Upgrade_Back/UI_Select_Upgrade_Back_%d.png", 2))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Select_Upgrade",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/UI_Select_Stat_Or_Forge/UI_Select_Upgrade_%d.png", 4))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Player_InterFace",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Player_UI/InterFace/InterFace.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_BossGage",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/BossUI/LIFEBAR_BOSS.tga", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_BossHp",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/BossUI/HpBar.tga", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_ForgeBack",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/UI_Forge_Back/Forge_Back.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_EndingCredit",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Ending/EndingCredit.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Player_HP",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Player_UI/Gage/HP_%d.png", 3))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Player_Gage_Spectrum",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Player_UI/Gage/Gage_Spectrum.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Forge_Slot",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/UI_Forge_Slot/UI_Forge_Slot_%d.png", 2))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Player_Gage",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Player_UI/Gage/Gage.tga", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Player_Stamina",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Player_UI/Gage/Stamina_%d.png", 3))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_InvenSlot",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Inventory/Slot_%d.png", 3))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_InvenBack",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Inventory/Inventory.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_ValnirUI",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/NPC_Valnir_UI/Valnir_%d.png", 3))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_EquipBack",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Inventory/Inventory.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_ShopSlot",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Shop_UI/ShopIcon_%d.png", 2))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_ShopBack",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/ShopBack/ShopBack%d.png", 2))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_KeyUI",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/KeyUI/%d.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_CubeTexture_SkyBox",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Cube, L"../../Resources/Texture/SkyBox/Sky_%d.dds", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_CubeTexture_SkyBox",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Cube, L"../../Resources/Texture/SkyBox/Sky_%d.dds", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_MonsterUIBar",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Monster_UI/Gage_%d.tga", 2))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}	
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_FlavorTexture",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Item_FlavorText/Item_FlavorText_%d.png", 16))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_FlavorTitle",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Item_Title/Item_Name_%d.png", 5))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_ValnirScript",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/NPC_Valnir_UI/Script/Valnir_Script_%d.png", 2))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Status_UpDownButton",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/StatusSlot/Status_Slot_%d.png", 6))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Status_Back",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/UI_Status_Back/UI_Status_Back.png",1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_WeaponName",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/WeaponName/WeaponName_%d.png", 2))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Player_Menu_Icon",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Player_UI/Menu/Icon%d.png", 6))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Player_Menu_Icon_Back",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Player_UI/Menu/Icon_Back.png",1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Player_Menu",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Player_UI/Menu/UI_Menu_Back%d.png", 3))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Distortion",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Distortion/Distortion_%d.png", 16))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Component_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Fog",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Fog/T_Fog_%d.tga", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Wire",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Wire/Wire_%d.png",4))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Filter",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Filter/%d.png",7))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_BossName",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/BossName/%d.png", 2))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Dead",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/UI/Dead_UI/Dead.png", 1))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}


	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_UI_Dead",
		CUI_Dead::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_UI_Dot",
		CUI_Target_Dot::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Lightning0",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Lightning0/%d.png", 16))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Lightning1",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Lightning1/%d.png", 16))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}
	if (FAILED(pManagement->Add_Texture_Prototype(
		(_uint)ESceneType::Static,
		L"Component_Texture_Lightning2",
		CTexture::Create(pLoading->m_pDevice, ETextureType::Normal, L"../../Resources/Texture/Lightning2/%d.png", 16))))
	{
		ERROR_BOX(L"Failed to Create");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_Player_InterFace",
		CPlayer_UI::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_UI_Player_Inventory",
		CUI_Inventory::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}


	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_UI_Player_Equipment",
		CUI_Equipment::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_BossUI",
		CUI_BossMonster::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_Choron_UI",
		CUI_NPC_Choron::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_Yaak_UI",
		CUI_NPC_Yaak::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_Event_Result",
		CUI_Event_Result::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_Valnir_UI",
		CUI_NPC_Valnir::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_Status_UI",
		CUI_Status::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_Forge_UI",
		CUI_Forge::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_Select_Upgrader",
		CUI_Select_Upgrade::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_SkyBox",
		CSkyBox::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_UI_MonsterBar",
		CUI_NormalMonster::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}



	if (FAILED(pManagement->Add_GameObject_Prototype
	(
		(_uint)ESceneType::Static,
		L"GameObject_Effect_Tex_Ani_Obj",
		CEffect_Texture_Ani::Create(pLoading->m_pDevice)
	)))
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}



	}
	_endthreadex(0);

	return 0;
}
#pragma endregion
HRESULT CLoading::Add_Player_Layer(const wstring & _szLayer, ESceneType _eSceneType)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}
	TRANSFORM_DESC tTransformDesc;
	switch (_eSceneType)
	{
	case ESceneType::Tutorial:
		tTransformDesc.vPos = _float3(-6.5f, 4.f, -5.8f);
		tTransformDesc.vScale = OBJECT_SIZE;
		tTransformDesc.fSpeedPerSecond = 5.f;
		tTransformDesc.fRotatePerSecond = 10.f;
		tTransformDesc.vRotate = { D3DXToRadian(0.f), D3DXToRadian(90.f) ,D3DXToRadian(0.f) };
		break;
	case ESceneType::Shrine:
		tTransformDesc.vPos = _float3(-6.5f, 2.2f/*5.f*/, -5.8f);
		tTransformDesc.vScale = OBJECT_SIZE;
		tTransformDesc.fSpeedPerSecond = 5.f;
		tTransformDesc.fRotatePerSecond = 10.f;
		tTransformDesc.vRotate = { D3DXToRadian(0.f), D3DXToRadian(90.f) ,D3DXToRadian(0.f) };
		break;
	case ESceneType::Stage2:
		tTransformDesc.vPos = _float3(-6.5f, 2.2f/*5.f*/, -5.8f);
		tTransformDesc.vScale = OBJECT_SIZE;
		tTransformDesc.fSpeedPerSecond = 5.f;
		tTransformDesc.fRotatePerSecond = 10.f;
		tTransformDesc.vRotate = { D3DXToRadian(0.f), D3DXToRadian(90.f) ,D3DXToRadian(0.f) };
		break;
	default:
		tTransformDesc.vPos = _float3(0.f,0.f,0.f);
		tTransformDesc.vScale = OBJECT_SIZE;
		tTransformDesc.fSpeedPerSecond = 5.f;
		tTransformDesc.fRotatePerSecond = 10.f;
		tTransformDesc.vRotate = { D3DXToRadian(0.f), D3DXToRadian(90.f) ,D3DXToRadian(0.f) };
		break;
	}
	
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Player",
		(_uint)ESceneType::Static,
		_szLayer,
		&tTransformDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading::Add_Player_Weapon_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}
	Player_Weapon eWeapon = Player_Weapon::Player_OnTake;
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Player_Weapon",
		(_uint)ESceneType::Static,
		_szLayer,
		&eWeapon)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLoading::Add_NPC_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	TRANSFORM_DESC tTransformDesc;
	tTransformDesc.vPos = _float3(-1.f, 1.992630f, -33.f);
	tTransformDesc.vScale = OBJECT_SIZE;
	tTransformDesc.vRotate = _float3{ D3DXToRadian(0.f), D3DXToRadian(-150.f) ,D3DXToRadian(0.f) };
	lstrcpy(tTransformDesc.szFileTitle, L"Choron_0");
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Choron",
		(_uint)ESceneType::Static,
		_szLayer,
		&tTransformDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}


	tTransformDesc.vPos = _float3(-7.f, 1.992630f, 7.f);
	tTransformDesc.vScale = OBJECT_SIZE;
	tTransformDesc.vRotate = _float3{ D3DXToRadian(0.f), D3DXToRadian(-45.f) ,D3DXToRadian(0.f) };
	lstrcpy(tTransformDesc.szFileTitle, L"Valnir_0");
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Valnir",
		(_uint)ESceneType::Static,
		_szLayer,
		&tTransformDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading::Add_MapStage1_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CGameObject* pMap = CMap_Stage1::Create(m_pDevice);
	NULL_CHECK_RETURN(pMap, E_FAIL);
	pMap->Set_ForceCheckClone();

	if (FAILED(pManagement->Clone_GameObject_ToLayer(pMap, (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

#pragma region MapObject
	vector<OBJECTINFO_DESC*> vecObjectInfo = pManagement->Get_MapObjectInfo();

	for (_uint i = 0; i < vecObjectInfo.size(); ++i)
	{
		wstring PrototypeName = vecObjectInfo[i]->szFileTitle;
		_uint count = (_uint)PrototypeName.find(L"_", 0);
		wstring wstrinteger = PrototypeName;
		wstrinteger.erase(0, count + 1);

		wstring wstrName = PrototypeName;
		wstrName.erase(count, PrototypeName.size());

		wstring wstrPrototypeName = L"GameObject_" + wstrName;

		TRANSFORM_DESC tTransformDesc;
		tTransformDesc.vPos = vecObjectInfo[i]->vPos;
		tTransformDesc.vScale = vecObjectInfo[i]->vScale;
		tTransformDesc.vRotate = vecObjectInfo[i]->vRotate;
		lstrcpy(tTransformDesc.szFileTitle, vecObjectInfo[i]->szFileTitle);

		if (wstrPrototypeName.find(L"Item") != wstring::npos)
		{
			ITEMTRANSFORM_DESC tItemTransformDesc;
			tItemTransformDesc.ItemID = eItemCode::Heal_Medium;
			tItemTransformDesc.vPos = vecObjectInfo[i]->vPos;
			tItemTransformDesc.vScale = vecObjectInfo[i]->vScale;
			tItemTransformDesc.vRotate = vecObjectInfo[i]->vRotate;
			tItemTransformDesc.bJump = false;
			lstrcpy(tItemTransformDesc.szFileTitle, vecObjectInfo[i]->szFileTitle);

			if (FAILED(pManagement->Clone_GameObject_ToLayer(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				(_uint)ESceneType::Static,
				_szLayer,
				&tItemTransformDesc)))
			{
				ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(pManagement->Clone_GameObject_ToLayer(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				(_uint)ESceneType::Static,
				_szLayer,
				&tTransformDesc)))
			{
				ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
				return E_FAIL;
			}
		}
	}

	Player_Weapon eWeapon = Player_Weapon::Player_Weapon_Normal_Stand;
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Player_Weapon",
		(_uint)ESceneType::Static,
		_szLayer,
		&eWeapon)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}
	////////////////////////////////////////////////////////////////////////////
	//--------------------------------------------------------------------------
	//_int iIdx = 0;
	//for (_int i = -5; i < 5; ++i)
	//{
	//	for (_int j = -5; j < 5; ++j)
	//	{
	//		wstring wstrObj = L"GameObject_Item_Special_";
	//		TRANSFORM_DESC tItemTransformDesc;
	//		tItemTransformDesc.vPos = { (_float)i*2,5.f,(_float)j*2 };
	//		tItemTransformDesc.vScale = { OBJECT_SIZE };
	//		tItemTransformDesc.vRotate = { 0.f,0.f,0.f };
	//		_tchar szBuff[16] = L"";
	//		wsprintf(szBuff, L"%d", iIdx);
	//		wstrObj += szBuff;
	//		lstrcpy(tItemTransformDesc.szFileTitle, wstrObj.c_str());

	//		if (FAILED(pManagement->Clone_GameObject_ToLayer(
	//			(_uint)ESceneType::Static,
	//			L"GameObject_Item_Special",
	//			(_uint)ESceneType::Static,
	//			L"Layer_Monster",
	//			&tItemTransformDesc)))
	//		{
	//			ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//			return E_FAIL;
	//		}
	//		++iIdx;
	//	}
	//}


	//--------------------------------------------------------------------------
	////////////////////////////////////////////////////////////////////////////
#pragma endregion


	return S_OK;
}

HRESULT CLoading::Add_MapShrine_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CGameObject* pMap = CMap_Shrine::Create(m_pDevice);
	NULL_CHECK_RETURN(pMap, E_FAIL);
	pMap->Set_ForceCheckClone();

	if (FAILED(pManagement->Clone_GameObject_ToLayer(pMap, (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_LAyer");
		return E_FAIL;
	}

#pragma region MapObject
	vector<OBJECTINFO_DESC*> vecObjectInfo = pManagement->Get_MapObjectInfo();

	for (_uint i = 0; i < vecObjectInfo.size(); ++i)
	{
		wstring PrototypeName = vecObjectInfo[i]->szFileTitle;
		_uint count = (_uint)PrototypeName.find(L"_", 0);
		wstring wstrinteger = PrototypeName;
		wstrinteger.erase(0, count + 1);

		wstring wstrName = PrototypeName;
		wstrName.erase(count, PrototypeName.size());

		wstring wstrPrototypeName = L"GameObject_" + wstrName;

		TRANSFORM_DESC tTransformDesc;
		tTransformDesc.vPos = vecObjectInfo[i]->vPos;
		tTransformDesc.vScale = vecObjectInfo[i]->vScale;
		tTransformDesc.vRotate = vecObjectInfo[i]->vRotate;
		lstrcpy(tTransformDesc.szFileTitle, vecObjectInfo[i]->szFileTitle);

		if (FAILED(pManagement->Clone_GameObject_ToLayer(
			(_uint)ESceneType::Static,
			wstrPrototypeName,
			(_uint)ESceneType::Static,
			_szLayer,
			&tTransformDesc)))
		{
			ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
			return E_FAIL;
		}
	}




	Player_Weapon eWeapon = Player_Weapon::Player_Weapon_Moon_Stand;
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Player_Weapon",
		(_uint)ESceneType::Static,
		_szLayer,
		&eWeapon)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}


	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Event_ObjGenerator",
		(_uint)ESceneType::Static,
		L"Layer_Generator")))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}


#pragma endregion

	return S_OK;
}

HRESULT CLoading::Add_MapStage2_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CGameObject* pMap = CMap_Stage2::Create(m_pDevice);
	NULL_CHECK_RETURN(pMap, E_FAIL);
	pMap->Set_ForceCheckClone();

	if (FAILED(pManagement->Clone_GameObject_ToLayer(pMap, (_uint)ESceneType::Static, L"Layer_Map")))
	{
		ERROR_BOX(L"Failed to Add_Map_Layer");
		return E_FAIL;
	}
#pragma region MapObject
	vector<OBJECTINFO_DESC*> vecObjectInfo = pManagement->Get_MapObjectInfo();

	for (_uint i = 0; i < vecObjectInfo.size(); ++i)
	{
		wstring PrototypeName = vecObjectInfo[i]->szFileTitle;
		_uint count = (_uint)PrototypeName.find(L"_", 0);
		wstring wstrinteger = PrototypeName;
		wstrinteger.erase(0, count + 1);

		wstring wstrName = PrototypeName;
		wstrName.erase(count, PrototypeName.size());

		wstring wstrPrototypeName = L"GameObject_" + wstrName;

		TRANSFORM_DESC tTransformDesc;
		tTransformDesc.vPos = vecObjectInfo[i]->vPos;
		tTransformDesc.vScale = vecObjectInfo[i]->vScale;
		tTransformDesc.vRotate = vecObjectInfo[i]->vRotate;
		lstrcpy(tTransformDesc.szFileTitle, vecObjectInfo[i]->szFileTitle);
		
		wstring wstrCloneName = tTransformDesc.szFileTitle;
		if (wstrCloneName.find(L"ShortCutDoor") != wstring::npos && (EVENT_STATIC_SHORTCUTDOOR_OPEN_STAGE2 & Get_EventMgr()->Get_CurStaticEvent()))
		{
			continue;
		}

		if (wstrPrototypeName.find(L"Item") != wstring::npos)
		{
			ITEMTRANSFORM_DESC tItemTransformDesc;
			if (wstrCloneName.find(L"Item_3") != wstring::npos)
				tItemTransformDesc.ItemID = eItemCode::MainQuest_Key;
			else
				tItemTransformDesc.ItemID = eItemCode::Heal_Medium;
			
			tItemTransformDesc.vPos = vecObjectInfo[i]->vPos;
			tItemTransformDesc.vScale = vecObjectInfo[i]->vScale;
			tItemTransformDesc.vRotate = vecObjectInfo[i]->vRotate;
			tItemTransformDesc.bJump = false;
			lstrcpy(tItemTransformDesc.szFileTitle, vecObjectInfo[i]->szFileTitle);

			if (FAILED(pManagement->Clone_GameObject_ToLayer(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				(_uint)ESceneType::Static,
				_szLayer,
				&tItemTransformDesc)))
			{
				ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
				return E_FAIL;
			}
		}
		else
		{
			if (FAILED(pManagement->Clone_GameObject_ToLayer(
				(_uint)ESceneType::Static,
				wstrPrototypeName,
				(_uint)ESceneType::Static,
				_szLayer,
				&tTransformDesc)))
			{
				ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
				return E_FAIL;
			}
		}
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Chess",
		(_uint)ESceneType::Static,
		_szLayer)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

#pragma endregion


	return S_OK;
}

HRESULT CLoading::Add_NPCStage2_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	TRANSFORM_DESC tTransformDesc;
	tTransformDesc.vPos = _float3(115.8f, 32.4f, 223.8f);
	tTransformDesc.vScale = OBJECT_SIZE;
	tTransformDesc.vRotate = _float3{ D3DXToRadian(0.f), D3DXToRadian(-45.f) ,D3DXToRadian(0.f) };
	lstrcpy(tTransformDesc.szFileTitle, L"Yaak_0");
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Yaak",
		(_uint)ESceneType::Static,
		_szLayer,
		&tTransformDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading::Add_MonsterStage1_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	wstring wstrLayer = L"";
	wstring wstrAppearLayer1 = L"Layer_Monster_Appear1";

	vector<OBJECTINFO_DESC*> vecObjectInfo = pManagement->Get_MonsterInfo();

	for (_uint i = 0; i < vecObjectInfo.size(); ++i)
	{
		wstrLayer = _szLayer;

		wstring PrototypeName = vecObjectInfo[i]->szFileTitle;
		_uint count = (_uint)PrototypeName.find(L"_", 0);
		wstring wstrinteger = PrototypeName;
		wstrinteger.erase(0, count + 1);

		wstring wstrName = PrototypeName;
		wstrName.erase(count, PrototypeName.size());

		wstring wstrPrototypeName = L"GameObject_" + wstrName;
		TRANSFORM_DESC tTransformDesc;
		tTransformDesc.vPos = vecObjectInfo[i]->vPos;
		tTransformDesc.vRotate = vecObjectInfo[i]->vRotate;
		tTransformDesc.vScale = vecObjectInfo[i]->vScale;
		tTransformDesc.fRotatePerSecond = 5.f;

		if (!lstrcmpi(wstrName.c_str(), L"Legionnaire"))
		{
			if (3 > stoi(wstrinteger))
				wstrLayer = wstrAppearLayer1;
		}

		lstrcpy(tTransformDesc.szFileTitle, vecObjectInfo[i]->szFileTitle);
		if (FAILED(pManagement->Clone_GameObject_ToLayer(
			(_uint)ESceneType::Static,
			wstrPrototypeName,
			(_uint)ESceneType::Static,
			wstrLayer,
			&tTransformDesc)))
		{
			ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoading::Add_MonsterStage2_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	wstring wstrLayer = L"";

	vector<OBJECTINFO_DESC*> vecObjectInfo = pManagement->Get_MonsterInfo();
	
	for (_uint i = 0; i < vecObjectInfo.size(); ++i)
	{
		wstrLayer = _szLayer;

		wstring PrototypeName = vecObjectInfo[i]->szFileTitle;
		_uint count = (_uint)PrototypeName.find(L"_", 0);
		wstring wstrinteger = PrototypeName;
		wstrinteger.erase(0, count + 1);

		wstring wstrName = PrototypeName;
		wstrName.erase(count, PrototypeName.size());

		wstring wstrPrototypeName = L"GameObject_" + wstrName;
		TRANSFORM_DESC tTransformDesc;
		tTransformDesc.vPos = vecObjectInfo[i]->vPos;
		tTransformDesc.vRotate = vecObjectInfo[i]->vRotate;
		tTransformDesc.vScale = vecObjectInfo[i]->vScale;
		tTransformDesc.fRotatePerSecond = 5.f;

		lstrcpy(tTransformDesc.szFileTitle, vecObjectInfo[i]->szFileTitle);
		if (FAILED(pManagement->Clone_GameObject_ToLayer(
			(_uint)ESceneType::Static,
			wstrPrototypeName,
			(_uint)ESceneType::Static,
			wstrLayer,
			&tTransformDesc)))
		{
			ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoading::Add_MonsterStage2ChessQueen_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	if (FAILED(pManagement->MapObjectLoad(L"../../Data/Stage2_ChessQueen.Monsterdat")))
	{
		ERROR_BOX(L"Failed To ResourceLoad");
		return E_FAIL;
	}

	wstring wstrLayer = L"";

	vector<OBJECTINFO_DESC*> vecObjectInfo = pManagement->Get_MapObjectInfo();

	for (_uint i = 0; i < vecObjectInfo.size(); ++i)
	{
		wstrLayer = _szLayer;

		wstring PrototypeName = vecObjectInfo[i]->szFileTitle;
		_uint count = (_uint)PrototypeName.find(L"_", 0);
		wstring wstrinteger = PrototypeName;
		wstrinteger.erase(0, count + 1);

		wstring wstrName = PrototypeName;
		wstrName.erase(count, PrototypeName.size());

		wstring wstrPrototypeName = L"GameObject_" + wstrName;
		TRANSFORM_DESC tTransformDesc;
		tTransformDesc.vPos = vecObjectInfo[i]->vPos;
		tTransformDesc.vRotate = vecObjectInfo[i]->vRotate;
		tTransformDesc.vScale = vecObjectInfo[i]->vScale;
		tTransformDesc.fRotatePerSecond = 5.f;
		_int iIndex = stoi(wstrinteger);
		_uint iCheck = 0;

		wstring CloneName = vecObjectInfo[i]->szFileTitle;
		CloneName += L"_Chess";
		lstrcpy(tTransformDesc.szFileTitle, CloneName.c_str());
		if (FAILED(pManagement->Clone_GameObject_ToLayer(
			(_uint)ESceneType::Static,
			wstrPrototypeName,
			(_uint)ESceneType::Static,
			wstrLayer,
			&tTransformDesc)))
		{
			ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoading::Add_MonsterStage2ChessKnight_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	if (FAILED(pManagement->MapObjectLoad(L"../../Data/Stage2_ChessKnight.Monsterdat")))
	{
		ERROR_BOX(L"Failed To ResourceLoad");
		return E_FAIL;
	}

	wstring wstrLayer = L"";

	vector<OBJECTINFO_DESC*> vecObjectInfo = pManagement->Get_MapObjectInfo();

	for (_uint i = 0; i < vecObjectInfo.size(); ++i)
	{
		wstrLayer = _szLayer;

		wstring PrototypeName = vecObjectInfo[i]->szFileTitle;
		_uint count = (_uint)PrototypeName.find(L"_", 0);
		wstring wstrinteger = PrototypeName;
		wstrinteger.erase(0, count + 1);

		wstring wstrName = PrototypeName;
		wstrName.erase(count, PrototypeName.size());

		wstring wstrPrototypeName = L"GameObject_" + wstrName;
		TRANSFORM_DESC tTransformDesc;
		tTransformDesc.vPos = vecObjectInfo[i]->vPos;
		tTransformDesc.vRotate = vecObjectInfo[i]->vRotate;
		tTransformDesc.vScale = vecObjectInfo[i]->vScale;
		tTransformDesc.fRotatePerSecond = 5.f;
		_int iIndex = stoi(wstrinteger);
		_uint iCheck = 0;

		wstring CloneName = vecObjectInfo[i]->szFileTitle;
		CloneName += L"_Chess";
		lstrcpy(tTransformDesc.szFileTitle, CloneName.c_str());
		if (FAILED(pManagement->Clone_GameObject_ToLayer(
			(_uint)ESceneType::Static,
			wstrPrototypeName,
			(_uint)ESceneType::Static,
			wstrLayer,
			&tTransformDesc)))
		{
			ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoading::Add_Cinematic_Camera_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CAMERA_DESC CameraDesc;
	CameraDesc.vEye = _float3(0.f, 10.f, 0.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vUp = _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovY = D3DXToRadian(60.f);
	CameraDesc.fAspect = (_float)WINCX / WINCY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_CinematicCamera",
		(_uint)ESceneType::Static,
		_szLayer,
		&CameraDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading::Add_Camera_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CAMERA_DESC CameraDesc;

	if (m_eLoadingScene == ESceneType::Tutorial)
	{
		CameraDesc.vEye = _float3(-0.5f, 5.13606787f, -5.8f);
		CameraDesc.vAt = _float3(6.5f, 5.13606787f, -5.9f);
	}
	if (m_eLoadingScene == ESceneType::Shrine)
	{
		CameraDesc.vEye = _float3(7.f, 5.f, 4.f);
		CameraDesc.vAt = _float3(0.f, 5.f, 0.f);
	}
	if (m_eLoadingScene == ESceneType::Stage2)
	{
		CameraDesc.vEye = _float3(14.4603357f, 18.f, 2.21984863f);
		CameraDesc.vAt = _float3(7.46033573f, 18.f, 2.21984816f);
	}
	CameraDesc.vUp = _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovY = D3DXToRadian(60.f);
	CameraDesc.fAspect = (_float)WINCX / WINCY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_MainCamera",
		(_uint)ESceneType::Static,
		_szLayer,
		&CameraDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading::Add_Light()
{
	auto pManagement = GET_MANAGEMENT;

	D3DLIGHT9 tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.Direction = _float3(-0.5f, -1.f, -0.5f);
	tLightInfo.Position = _float3(-20, 30.f, -90.f);

	//tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	//tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	////_float3 vDir;
	////D3DXVec3Normalize(&(_float3)vDir, &_float3(1.f, -1.f, 1.f));
	////tLightInfo.Direction = vDir;
	////tLightInfo.Direction = _float3(0.5f, -1.f, 0.5f);
	//tLightInfo.Direction = _float3(-1.f, -1.f, 0.f);

	/*tLightInfo.Type = D3DLIGHT_POINT;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	tLightInfo.Position = _float3(-1.f, -1.f, -1.f);
	tLightInfo.Falloff = 1.f;
	tLightInfo.Range = 75.f;*/
	if (FAILED(pManagement->Ready_Light(m_pDevice, &tLightInfo, 0)))
	{
		ERROR_BOX(L"pManagement is Ready_Light");
		return E_FAIL;
	}
	/*tLightInfo.Direction = _float3(1.f, -1.f, 0.5f);
	if (FAILED(pManagement->Ready_Light(m_pDevice, &tLightInfo, 1)))
	{
	ERROR_BOX(L"pManagement is Ready_Light");
	return E_FAIL;
	}

	tLightInfo.Direction = _float3(-0.5f, -1.f, -1.f);
	if (FAILED(pManagement->Ready_Light(m_pDevice, &tLightInfo, 2)))
	{
	ERROR_BOX(L"pManagement is Ready_Light");
	return E_FAIL;
	}*/

	//tLightInfo.Type = D3DLIGHT_POINT;
	//tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	//tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	//tLightInfo.Specular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	//tLightInfo.Falloff = 1.f;
	//tLightInfo.Range = 75.f;

	//if (FAILED(pManagement->Ready_Light(m_pDevice, &tLightInfo, 1)))
	//{
	//	ERROR_BOX(L"pManagement is Ready_Light");
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLoading::Add_Light_Stage2()
{
	auto pManagement = GET_MANAGEMENT;

	D3DLIGHT9 tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.Direction = _float3(0.5f, -1.f, 0.5f);
	tLightInfo.Position = _float3(-20, 30.f, -90.f);

	if (FAILED(pManagement->Ready_Light(m_pDevice, &tLightInfo, 0)))
	{
		ERROR_BOX(L"pManagement is Ready_Light");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLoading::Add_EffectGenerator_Layer(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CParticleGenerator::P_GENERATOR_DESC tDesc;
	tDesc.vCreateNormalPos = _float3(-6.5f, 2.5f, -5.8f);
	tDesc.fCreateRange = 15.f;
	tDesc.bCreateOnOff = true;
	tDesc.fCreateCoolTime = 20.f / 60.f;
	tDesc.iCreateCount = 10;
	tDesc.eType = eEffectType::Dust;
	//m_tPaticleGenerator_Desc.
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Effect_Generator",
		(_uint)ESceneType::Static,
		_szLayer, &tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	tDesc.iCreateCount = 5;
	tDesc.eType = eEffectType::Blink;
	//m_tPaticleGenerator_Desc.
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Effect_Generator",
		(_uint)ESceneType::Static,
		_szLayer, &tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	//tDesc.vCreateNormalPos = _float3(-6.5f, 2.1f, -5.8f);
	//tDesc.fCreateRange = 10.f;
	//tDesc.bCreateOnOff = true;
	//tDesc.fCreateCoolTime = 20.f / 60.f;
	//tDesc.iCreateCount = 25;
	//tDesc.bPlayerStakingOnOff = true;
	//tDesc.eType = eEffectType::Dust;
	////m_tPaticleGenerator_Desc.
	//if (FAILED(pManagement->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Effect_Generator",
	//	(_uint)ESceneType::Static,
	//	_szLayer, &tDesc)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}

	//-------------------------------------Test_DistortionObj_Load---------------------------------------//

	//CEffect_Texture::TEXOBJ_DESC tTexDesc;
	//tTexDesc.vArgb = { 1.f,1.f,1.f,1.f };
	//tTexDesc.tTransformDesc.vPos = { 0.f,7.f,0.f };
	//tTexDesc.tTransformDesc.vScale = { 6.f,10.f,1.f };
	//tTexDesc.eTexMode = CEffect_Texture::eTexMode::Distortion;
	////tTexDesc.eMovePattern = ePartilcePatternType::BillBoard;
	////tTexDesc.eMovePattern = ePartilcePatternType::YBillBoard;
	//tTexDesc.eMovePattern = (ePartilcePatternType)0;
	//tTexDesc.iRenderPass = 4;
	//tTexDesc.iRenderId =2;
	//if (FAILED(pManagement->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Tex_Obj",
	//	(_uint)ESceneType::Static,
	//	_szLayer, &tTexDesc)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}


	//CEffect_Texture_Ani::TEXOBJANI_DESC tTexDescAni;
	//tTexDescAni.vArgb = { 1.f,1.f,1.f,1.f };
	//tTexDescAni.tTransformDesc.vPos = { 0.f,7.f,0.f };
	//tTexDescAni.tTransformDesc.vScale = { 5.f,10.f,1.f };
	////tTexDescAni.eTexMode = CEffect_Texture::eTexMode::Distortion;
	//tTexDescAni.eMovePattern = ePartilcePatternType::BillBoard;
	////tTexDescAni.eMovePattern = ePartilcePatternType::YBillBoard;
	//tTexDescAni.eMovePattern = (ePartilcePatternType)0;
	////tTexDescAni.iRenderPass = 2;
	//tTexDescAni.bPlayRepeat = true;
	//tTexDescAni.iRenderId =0;
	//tTexDescAni.iMaxRenderId = 15;
	//tTexDescAni.fPlaySpeed = 1.f;
	//lstrcpy(tTexDescAni.szTexName, L"Component_Texture_Lightning1");
	//if (FAILED(pManagement->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Effect_Tex_Ani_Obj",
	//	(_uint)ESceneType::Static,
	//	L"Layer_Effect_Test", &tTexDescAni)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}

	//CEffect_Texture::TEXOBJ_DESC tTexDesc;
	//tTexDesc.vArgb = { 1.f,1.f,1.f,1.f };
	//tTexDesc.tTransformDesc.vPos = { 0.f,7.f,0.f };
	//tTexDesc.tTransformDesc.vScale = { 5.f,10.f,1.f };
	//tTexDesc.eTexMode = CEffect_Texture::eTexMode::Distortion;
	//tTexDesc.eMovePattern = ePartilcePatternType::BillBoard;
	////tTexDesc.eMovePattern = ePartilcePatternType::YBillBoard;
	////tTexDesc.eMovePattern = (ePartilcePatternType)0;
	//tTexDesc.iRenderPass = 4;
	//tTexDesc.iRenderId = 4;
	//if (FAILED(pManagement->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Tex_Obj",
	//	(_uint)ESceneType::Static,
	//	L"Layer_Effect_Test", &tTexDesc)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}


	//-------------------------------------Test_DistortionObj_Load---------------------------------------//
	return S_OK;
}

HRESULT CLoading::Add_SkyBox(const wstring & _szLayer)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_SkyBox",
		(_uint)ESceneType::Static,
		_szLayer)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}
	return S_OK;
}

CLoading * CLoading::Create(_DEVICE _pDevice, ESceneType _eSceneType, _bool _bMultiThread)
{
	CLoading* pInstance = new CLoading(_pDevice, _eSceneType);

	if (FAILED(pInstance->Ready_Loading(_bMultiThread)))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading::Free()
{
	//WaitForSingleObject(m_hLoadingThread, INFINITE);
	if (m_iFinish != (_uint)EEvent::Dead)
	{
		MSG_BOX(L"Loading Err NotComplete");
		__debugbreak();
	}
	//for (_uint i = 0; i < eLoadingCategory::End; ++i)
	//{
	//	CloseHandle(m_hResourceThread[i]);
	//}
	CloseHandle(m_hLoadingThread);

	DeleteCriticalSection(&m_tCriticalSection);

	Safe_Release(m_pDevice);
}
