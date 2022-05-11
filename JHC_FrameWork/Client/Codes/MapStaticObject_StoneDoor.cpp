#include "stdafx.h"
#include "MapStaticObject_StoneDoor.h"

CMapStaticObject_StoneDoor::CMapStaticObject_StoneDoor(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
	, m_fUpTime(0.f)
{
}

CMapStaticObject_StoneDoor::CMapStaticObject_StoneDoor(const CMapStaticObject_StoneDoor & _rOther)
	: CMapStaticObjectBase(_rOther)
	, m_fUpTime(0.f)
{
}

HRESULT CMapStaticObject_StoneDoor::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_StoneDoor::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMapStaticObject_StoneDoor::Update_GameObject(const _float & _fDeltaTime)
{
	

//	auto pManagement = GET_MANAGEMENT;
	//list<CGameObject*> listObj = pManagement->Get_LayerContainer(m_iSceneIdx, L"Layer_Monster_Appear1");

	//if (0 == listObj.size())
	//{
	//	_float3 vPos;
	//	pManagement->ActorMove(m_tTransformDesc.szFileTitle, &vPos, _float3(0.f, 1.f, 0.f), 0.5f, _fDeltaTime);
	//	vPos.y -= 3.f;
	//	m_pTransform->Set_Pos(vPos);

	//	Door_Move(_fDeltaTime);
	//}
	if(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_DOOR_OPEN_STAGE_1)
		Door_Move(_fDeltaTime);

	CMapStaticObjectBase::Update_GameObject(_fDeltaTime);
	//m_pTransform->Update_Transform();
	
	return m_iEvent;
}

void CMapStaticObject_StoneDoor::Door_Move(const _float & _fDeltaTime)
{
	_float3 vPos;
	Get_Manager()->ActorMove(m_tTransformDesc.szFileTitle, &vPos, _float3(0.f, 1.f, 0.f), 0.5f, _fDeltaTime);
	vPos.y -= 3.f;
	m_pTransform->Set_Pos(vPos);

	m_fUpTime += _fDeltaTime;

	if (m_fUpTime >= 15.f)
	{
		Get_Manager()->Delete_Trigger(m_tTransformDesc.szFileTitle);
		m_iEvent = (_uint)EEvent::Dead;
	}
}

CMapStaticObject_StoneDoor * CMapStaticObject_StoneDoor::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_StoneDoor* pInstance = new CMapStaticObject_StoneDoor(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_StoneDoor::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_StoneDoor* pClone = new CMapStaticObject_StoneDoor(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_StoneDoor::Free()
{
	CMapStaticObjectBase::Free();
}
