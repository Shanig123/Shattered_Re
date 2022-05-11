#include "stdafx.h"
#include "JPretoShield.h"

CJPretoShield::CJPretoShield(_DEVICE _pDevice, const wstring& _PrototypeName /*= L""*/)
	:CJBaseWeapon(_pDevice, _PrototypeName)
{
}

CJPretoShield::CJPretoShield(const CJPretoShield & _rOther)
	: CJBaseWeapon(_rOther)
{
}

HRESULT CJPretoShield::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseWeapon::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	//_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	//Get_Manager()->Add_Weapon(m_tTransformDesc.szFileTitle, vPos, PxCapsuleGeometry(1.f, 5.f), 0.f);

	m_fWeaponDamage = 30.f;
	m_iShaderPass = 2;

	return S_OK;
}

_uint CJPretoShield::Update_GameObject(const _float & _fDeltaTime)
{
	CJBaseWeapon::Update_GameObject(_fDeltaTime);

	return m_iEvent;
}

HRESULT CJPretoShield::Clone_TrailEffect()
{
	return S_OK;
}

HRESULT CJPretoShield::WeaponSound(_uint iSoundIndex)
{
	return S_OK;
}

CJPretoShield * CJPretoShield::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJPretoShield* pInstance = new CJPretoShield(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJPretoShield::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJPretoShield* pClone = new CJPretoShield(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJPretoShield::Free()
{
	CJBaseWeapon::Free();
}
