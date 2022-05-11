#include "stdafx.h"
#include "MapStaticObject_IronDoor.h"

CMapStaticObject_IronDoor::CMapStaticObject_IronDoor(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
	, m_fDownTime(0.f), m_fParticleTime(0.f)
{
}

CMapStaticObject_IronDoor::CMapStaticObject_IronDoor(const CMapStaticObject_IronDoor & _rOther)
	: CMapStaticObjectBase(_rOther)
	, m_fDownTime(0.f), m_fParticleTime(0.f)
{
}

HRESULT CMapStaticObject_IronDoor::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_IronDoor::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	Get_Manager()->Add_Trigger(m_tTransformDesc.szFileTitle, PxTransform(PxVec3(vPos.x + 1.5f, vPos.y, vPos.z - 1.5f)), EInteractionType::Sphere, PxSphereGeometry(2.f));

	m_iShaderPass = 1;
	m_bCheckRimDistance = true;
	m_fRimDistanceFix = 5.f;

	return S_OK;
}

_uint CMapStaticObject_IronDoor::Update_GameObject(const _float & _fDeltaTime)
{
	if (Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_DOOR_OPEN_SHRINE)
	{
		Door_Move(_fDeltaTime);
	
	}
	if(!(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_DOOR_OPEN_SHRINE)&&(Get_EventMgr()->Get_QuestFlag() & QUEST_END_1))
		m_iShaderPass = 14;

	CMapStaticObjectBase::Update_GameObject(_fDeltaTime);

	return m_iEvent;
}

void CMapStaticObject_IronDoor::Door_Move(const _float & _fDeltaTime)
{
	_float3 vPos;
	Get_Manager()->ActorMove(m_tTransformDesc.szFileTitle, &vPos, _float3(0.f, -1.f, 0.f), 1.f, _fDeltaTime);
	m_pTransform->Set_Pos(vPos);

	m_fDownTime += _fDeltaTime;

	if (m_fDownTime >= 10.f)
	{
		Get_Manager()->Delete_Trigger(m_tTransformDesc.szFileTitle);
		m_iEvent = (_uint)EEvent::Dead;
	}
}

//void CMapStaticObject_IronDoor::Create_Effect(const _float & _fDeltaTime)
//{
//	
//	if (::CoolTime(_fDeltaTime, m_fParticleTime, 0.1f))
//	{
//		//_float3 vMin = m_pMesh->Get_ObjectMinPos();
//		//_float3 vMax = m_pMesh->Get_ObjectMaxPos();
//		//_float3 vThis = m_pTransform->Get_TransformDesc().vPos;
//		//_float3 vCreatePos = { Mersen_ToFloat(vMin.x,vMax.x),Mersen_ToFloat(vMin.y,vMax.y*0.5f) ,Mersen_ToFloat(vMin.z,vMax.z) };
//
//		m_iShaderPass = 14;
//
//		//Get_Manager()->Generate_ItemParticle(
//		//	m_iSceneIdx, vCreatePos, 0.f, 8, _float4(1.f, 1.f, 1.f, 0.2f)
//		//	, ePartilcePatternType::OnStratBlinkAndGoUp, Mersen_ToInt(3, 5)
//		//);
//
//		m_fParticleTime = 0.f;
//	}
//}

CMapStaticObject_IronDoor * CMapStaticObject_IronDoor::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_IronDoor* pInstance = new CMapStaticObject_IronDoor(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_IronDoor::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_IronDoor* pClone = new CMapStaticObject_IronDoor(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_IronDoor::Free()
{
	CMapStaticObjectBase::Free();
}
