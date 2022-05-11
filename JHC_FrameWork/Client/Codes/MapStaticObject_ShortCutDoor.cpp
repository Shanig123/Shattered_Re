#include "stdafx.h"
#include "MapStaticObject_ShortCutDoor.h"

CMapStaticObject_ShortCutDoor::CMapStaticObject_ShortCutDoor(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
	, m_iHitCount(3)
	, m_bActivite(false), m_bPower(false)
	, m_fDeleteTime(0.f)
	, m_bGravity(false), m_bGraviStart(false)
	, m_fJumpTime(0.f)
	, m_vGravityPos({ 0.f, 0.f, 0.f })
{
}

CMapStaticObject_ShortCutDoor::CMapStaticObject_ShortCutDoor(const CMapStaticObject_ShortCutDoor & _rOther)
	: CMapStaticObjectBase(_rOther)
	, m_iHitCount(3)
	, m_bActivite(false), m_bPower(false)
	, m_fDeleteTime(0.f)
	, m_bGravity(false), m_bGraviStart(false)
	, m_fJumpTime(0.f)
	, m_vGravityPos({ 0.f, 0.f, 0.f })
{
}

HRESULT CMapStaticObject_ShortCutDoor::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_ShortCutDoor::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMapStaticObject_ShortCutDoor::Update_GameObject(const _float & _fDeltaTime)
{
	CMapStaticObjectBase::Update_GameObject(_fDeltaTime);

	void* Data;
	Get_Manager()->GetData_Trigger(m_tTransformDesc.szFileTitle, &Data);
	MyPublicData* ObjectData = (MyPublicData*)Data;

	if (ObjectData->Collision)
	{
		Set_Hit(ObjectData->Damage);
		ObjectData->Collision = false;
		Get_Manager()->SetData_Trigger(m_tTransformDesc.szFileTitle, ObjectData);
	}

	if (m_bActivite)
	{
		_float3 vPos;
		_float4x4 matRotate;
		_float4Q InQuat = _float4Q(0.f, 0.f, 0.f, 1.f);
		_float3 vPower = _float3(0.f, 0.f, 0.f);
		if (!m_bPower)
		{
			D3DXQuaternionRotationYawPitchRoll(&InQuat, 0.f, D3DXToRadian(10.f), 0.f);
			vPower = _float3(-1.f, -1.f, -1.f);
		}			

		Get_Manager()->ActorMove(m_tTransformDesc.szFileTitle, &vPos, &matRotate, vPower, InQuat, 5.f, _fDeltaTime);
		vPos.x -= 1.f;
		vPos.y += 1.5f;
		m_pTransform->Set_Pos(vPos);
		m_pTransform->Set_RotateMatrix(&matRotate);

		m_fDeleteTime += _fDeltaTime;
		if (m_fDeleteTime >= 3.f)
		{
			if(m_fDeleteTime >= 1.f)
				m_bPower = true;

			//Get_Manager()->Delete_Trigger(m_tTransformDesc.szFileTitle);
			//m_iEvent = (_uint)EEvent::Dead;
		}
	}
	
	return m_iEvent;
}

void CMapStaticObject_ShortCutDoor::Set_Hit(_float& _fDamage)
{
	if (EVENT_STATIC_BRIDGE_STAGE2 & Get_EventMgr()->Get_CurStaticEvent())
	{
		m_iHitCount -= (_uint)_fDamage;
		Get_Manager()->StopSound(CSound_Manager::ESoundID::Object);
		switch (m_iHitCount)
		{
		case 0:
			Get_Manager()->PlaySounds(L"Environment_ShortCutDoor_3.ogg", CSound_Manager::ESoundID::Object, 1.f);
			break;
		case 1:
			Get_Manager()->PlaySounds(L"Environment_ShortCutDoor_2.ogg", CSound_Manager::ESoundID::Object, 1.f);
			break;
		case 2:
			Get_Manager()->PlaySounds(L"Environment_ShortCutDoor_1.ogg", CSound_Manager::ESoundID::Object, 1.f);
			break;
		}
		
		if (m_iHitCount <= 0)
		{
			m_bActivite = true;
			Get_Manager()->SetDynamic_Actor(m_tTransformDesc.szFileTitle);
		}
	}	
}

CMapStaticObject_ShortCutDoor * CMapStaticObject_ShortCutDoor::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_ShortCutDoor* pInstance = new CMapStaticObject_ShortCutDoor(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_ShortCutDoor::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_ShortCutDoor* pClone = new CMapStaticObject_ShortCutDoor(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_ShortCutDoor::Free()
{
	CMapStaticObjectBase::Free();
}
