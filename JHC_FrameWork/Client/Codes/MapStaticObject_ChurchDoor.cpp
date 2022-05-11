#include "stdafx.h"
#include "MapStaticObject_ChurchDoor.h"

CMapStaticObject_ChurchDoor::CMapStaticObject_ChurchDoor(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
	, m_fDownTime(0.f), m_fParticleTime(0.f)
	, m_bClose(false)
	, m_fEndY(0.f)
	, m_b3DSound(false)
{
}

CMapStaticObject_ChurchDoor::CMapStaticObject_ChurchDoor(const CMapStaticObject_ChurchDoor & _rOther)
	: CMapStaticObjectBase(_rOther)
	, m_fDownTime(0.f), m_fParticleTime(0.f)
	, m_bClose(false)
	, m_fEndY(0.f)
	, m_b3DSound(false)
{
}

HRESULT CMapStaticObject_ChurchDoor::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_ChurchDoor::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	m_iShaderPass = 1;
	m_bCheckRimDistance = true;
	m_fRimDistanceFix = 5.f;

	m_fEndY = m_tTransformDesc.vPos.y - 10.f;

	return S_OK;
}

_uint CMapStaticObject_ChurchDoor::Update_GameObject(const _float & _fDeltaTime)
{
	if (!m_bClose && (Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_STAGE2_CHURCHDOOR_CLOSE))
	{
		Door_Move(_fDeltaTime);	
	}

	CMapStaticObjectBase::Update_GameObject(_fDeltaTime);

	return m_iEvent;
}

void CMapStaticObject_ChurchDoor::Door_Move(const _float & _fDeltaTime)
{
	_float3 vPos;
	Get_Manager()->ActorMove(m_tTransformDesc.szFileTitle, &vPos, _float3(0.f, -1.f, 0.f), 10.f, _fDeltaTime);
	vPos.y -= 5.f;
	m_pTransform->Set_Pos(vPos);

	m_fDownTime += _fDeltaTime;
	if (!m_b3DSound)
	{
		Get_Manager()->Play3DSounds(L"Environment_ChurchDoor.ogg", CSound_Manager::ESoundID::Object3D_0, m_tTransformDesc.vPos, 0.f, 30.f, false, 1.f);
		m_b3DSound = true;
	}

	if (vPos.y <= m_fEndY)
	{
		m_bClose = true;
	}
}

CMapStaticObject_ChurchDoor * CMapStaticObject_ChurchDoor::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_ChurchDoor* pInstance = new CMapStaticObject_ChurchDoor(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_ChurchDoor::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_ChurchDoor* pClone = new CMapStaticObject_ChurchDoor(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_ChurchDoor::Free()
{
	CMapStaticObjectBase::Free();
}
