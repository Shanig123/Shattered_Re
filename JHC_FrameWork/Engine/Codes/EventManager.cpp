#include "EventManager.h"
#include "Scene_Manager.h"
#include "GameObject_Manager.h"
#include "GameObject.h"

USING(Engine)
IMPLEMENT_SINGLETON(CEvent_Manager)
CEvent_Manager::CEvent_Manager()
	:CBase(), m_pDevice(nullptr)
	, m_iEvent(0), m_iStaticEvent(0), m_iQuestFlag(0)
	, m_iHomonculus_KillCount(0),m_iLegionnaire_KillCount(0),m_iTotal_KillCount(0)
	, m_fStage1EventTime(0.f), m_iStage1CreateCount(0)
{
}

HRESULT CEvent_Manager::Ready_EventMgr(_DEVICE _pDevice)
{
	NULL_CHECK_RETURN(_pDevice, E_FAIL);
	m_pDevice = _pDevice;
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	Safe_AddRef(m_pDevice);


	return S_OK;
}

_uint CEvent_Manager::Update_EventMgr(const _float & _fDeltaTime)
{
	if(m_iEvent & EVENT_VOLATILE_QUEST_END_1)
		m_iEvent &= (~(EVENT_VOLATILE_QUEST_END_1 | EVENT_VOLATILE_QUEST_START_1));

	switch ((ESceneType)CScene_Manager::Get_Instance()->Get_CurSceneidx())
	{
	case ESceneType::Tutorial:
		CEvent_Manager::Stage1Event(_fDeltaTime);
		break;
	case ESceneType::Stage2:
		CEvent_Manager::Stage2Event(_fDeltaTime);
		break;
	case ESceneType::Shrine:
		CEvent_Manager::ShrineEvent(_fDeltaTime);
		break;
	default:
		break;
	}




	return 0;
}

void CEvent_Manager::Stage1Event(const _float & _fDeltaTime)
{
	if (CScene_Manager::Get_Instance()->Get_CurSceneidx() == (_uint)ESceneType::Tutorial)
	{
		list<CGameObject*>* listObjUpdate = CGameObject_Manager::Get_Instance()->
			Get_LayerContainer(
				CScene_Manager::Get_Instance()->Get_CurSceneidx(),
				L"Layer_Monster_Appear1");
		if (listObjUpdate)
		{
			if (!listObjUpdate->front()->Get_UpdateInit())
			{
				for (auto& iter : *listObjUpdate)
				{
					iter->Set_UpdateInit();
				}
			}

		}
		
		if (m_iStaticEvent & EVENT_STATIC_GET_WEAPON)
		{
			
			list<CGameObject*>* listObj = CGameObject_Manager::Get_Instance()->
				Get_LayerContainer(
					CScene_Manager::Get_Instance()->Get_CurSceneidx(),
					L"Layer_Monster_Appear1");
			if (listObj)
			{
				if(::CoolTime(_fDeltaTime,m_fStage1EventTime,1.f))
				{
					if (listObj->front())
					{
						//Safe_AddRef(listObjUpdate->front());
						CGameObject_Manager::Get_Instance()->Clone_GameObject_ToLayer(
							listObj->front(), CScene_Manager::Get_Instance()->Get_CurSceneidx(), L"Layer_Monster");
						listObj->pop_front();
						m_fStage1EventTime = 0.f;
					}
				}
				//for (auto& iter : *listObj)
				//{
				//	Safe_AddRef(iter);c
				//	CGameObject_Manager::Get_Instance()->Clone_GameObject_ToLayer(
				//		iter, CScene_Manager::Get_Instance()->Get_CurSceneidx(), L"Layer_Monster");
				//}
				if (listObj->empty())
					CGameObject_Manager::Get_Instance()->Clear_Layer(CScene_Manager::Get_Instance()->Get_CurSceneidx(), L"Layer_Monster_Appear1");

			}
		}

		if (m_iLegionnaire_KillCount > 2)
		{
			m_iEvent |= EVENT_VOLATILE_DOOR_OPEN_STAGE_1;
		}
	}
}

void CEvent_Manager::Stage2Event(const _float & _fDeltaTime)
{

}

void CEvent_Manager::ShrineEvent(const _float & _fDeltaTime)
{
	//if ((m_iQuestFlag & QUEST_END_1) && !(m_iStaticEvent & EVENT_STATIC_DOOR_OPEN_SHRINE))
	//	m_iStaticEvent |= EVENT_STATIC_DOOR_OPEN_SHRINE;



}

void CEvent_Manager::Free()
{
	Safe_Release(m_pDevice);
}
