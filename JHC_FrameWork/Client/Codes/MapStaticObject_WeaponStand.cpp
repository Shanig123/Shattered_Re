#include "stdafx.h"
#include "MapStaticObject_WeaponStand.h"
#include "Player.h"

CMapStaticObject_WeaponStand::CMapStaticObject_WeaponStand(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CMapStaticObjectBase(_pDevice, _PrototypeName)
	, m_bOnNormalSword(false)
	, m_bOnMoonSword(false)
{
}

CMapStaticObject_WeaponStand::CMapStaticObject_WeaponStand(const CMapStaticObject_WeaponStand & _rOther)
	: CMapStaticObjectBase(_rOther)
	, m_bOnNormalSword(_rOther.m_bOnNormalSword)
	, m_bOnMoonSword(_rOther.m_bOnMoonSword)
	, m_fParticleTime(0.f)
{
}

HRESULT CMapStaticObject_WeaponStand::Ready_GameObject_Prototype()
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapStaticObject_WeaponStand::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CMapStaticObjectBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}
	m_iShaderPass = 13;
	m_bCheckRimDistance = true;

	return S_OK;
}

_uint CMapStaticObject_WeaponStand::Update_GameObject(const _float & _fDeltaTime)
{
	CMapStaticObjectBase::Update_GameObject(_fDeltaTime);
	if ((Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_GET_WEAPON) && m_bOnNormalSword)
		m_iShaderPass = 2;
	if ((Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_GET_MOON_WEAPON )&& m_bOnMoonSword)
		m_iShaderPass = 2;

	CreateEffect(_fDeltaTime);
	if (!m_bRePos)
	{

		if ((_uint)ESceneType::Tutorial == m_iSceneIdx)
		{
			m_bOnMoonSword = false;
			m_bOnNormalSword = true;
		}
		else if ((_uint)ESceneType::Shrine == m_iSceneIdx)
		{
			m_bOnMoonSword = true;
			m_bOnNormalSword = false;
		}
		m_bRePos = true;
	}

	return 0;
}

CMapStaticObject_WeaponStand * CMapStaticObject_WeaponStand::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CMapStaticObject_WeaponStand* pInstance = new CMapStaticObject_WeaponStand(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapStaticObject_WeaponStand::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CMapStaticObject_WeaponStand* pClone = new CMapStaticObject_WeaponStand(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMapStaticObject_WeaponStand::Free()
{
	CMapStaticObjectBase::Free();
}

void CMapStaticObject_WeaponStand::CreateEffect(const _float & _fDeltaTime)
{

	if (!(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_GET_WEAPON) && m_bOnNormalSword && (::CoolTime(_fDeltaTime, m_fParticleTime, 0.1f)))
	{
		//m_bOnNormalSword
		_float4 vColor = { 0.66666f, 0.9803f, 0.8941f, Mersen_ToFloat(0.2f, 0.5f) };

		m_iShaderPass = 14;

		_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
		vPos.y += 4.f;

		Get_Manager()->Generate_ItemParticle(
			m_iSceneIdx, vPos, 0.5f, 1, vColor
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraight)

			//, ePartilcePatternType::OnStartBlinkAndGoStraight
			, Mersen_ToInt(6, 8)
		);

		vPos.y += 1.f;

		Get_Manager()->Generate_ItemParticle(
			m_iSceneIdx, vPos, 0.5f, 1, vColor
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp | (_ulong)ePartilcePatternType::x2)

			//, ePartilcePatternType::OnStratBlinkAndGoUpx2
			, Mersen_ToInt(6, 8)
		);
		m_fParticleTime = 0.f;
	}
	else if(!(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_GET_MOON_WEAPON) && m_bOnMoonSword && (::CoolTime(_fDeltaTime, m_fParticleTime, 0.1f)))	//플래그 수정 필요
	{
		//m_bOnMoonSword
		_float4 vColor = { 0.1f,0.1f,1.f,2.2f };

		m_iShaderPass = 14;

		_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
		vPos.y += 4.f;

		Get_Manager()->Generate_ItemParticle(
			m_iSceneIdx, vPos, 0.5f, 1, vColor
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraight /*| (_ulong)ePartilcePatternType::x2*/)
			, Mersen_ToInt(6, 8)
		);

		vPos.y += 1.f;

		Get_Manager()->Generate_ItemParticle(
			m_iSceneIdx, vPos, 0.5f, 1, vColor
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp | (_ulong)ePartilcePatternType::x2)
			, Mersen_ToInt(6, 8)
		);
		m_fParticleTime = 0.f;
	}


}
