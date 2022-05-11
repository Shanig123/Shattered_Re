#include "stdafx.h"
#include "JBaseMonster.h"
#include "JLegionnaireGun.h"
#include "JLegionnaireBullet.h"

CJLegionnaireGun::CJLegionnaireGun(_DEVICE _pDevice, const wstring& _PrototypeName /*= L""*/)
	:CJBaseWeapon(_pDevice, _PrototypeName)
	, m_pBullet(nullptr)
	, m_vMuzzlePos(0.f, 0.f, 0.f)
	, m_vMuzzleLook(0.f, 0.f, 0.f)
	, m_vWorldMuzzlePos(0.f, 0.f, 0.f)
{
}

CJLegionnaireGun::CJLegionnaireGun(const CJLegionnaireGun & _rOther)
	: CJBaseWeapon(_rOther)
	, m_pBullet(nullptr)
	, m_vMuzzlePos(0.f, 0.f, 0.f)
	, m_vMuzzleLook(0.f, 0.f, 0.f)
	, m_vWorldMuzzlePos(0.f, 0.f, 0.f)
{
}

HRESULT CJLegionnaireGun::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseWeapon::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	m_vMuzzlePos = { -97.f, 0.f ,0.f };
	m_fWeaponDamage = 5.f;
	m_iShaderPass = 2;

	return S_OK;
}

_uint CJLegionnaireGun::Update_GameObject(const _float & _fDeltaTime)
{
	CJBaseWeapon::Update_GameObject(_fDeltaTime);

	if (m_pBullet == nullptr)
		BulletMaker();	

	return m_iEvent;
}

HRESULT CJLegionnaireGun::Clone_TrailEffect()
{
	return S_OK;
}

HRESULT CJLegionnaireGun::WeaponSound(_uint iSoundIndex)
{
	_uint iCount = Mersen_ToInt(0, 2);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Legio_GunSwing%d.ogg", iCount);

	CSound_Manager::ESoundID eSound;
	switch (iSoundIndex)
	{
	case 0:
		eSound = CSound_Manager::ESoundID::Monster_Weapon3D_0;
		break;
	case 1:
		eSound = CSound_Manager::ESoundID::Monster_Weapon3D_1;
		break;
	case 2:
		eSound = CSound_Manager::ESoundID::Monster_Weapon3D_2;
		break;
	}

	GET_MANAGEMENT->StopSound(eSound);
	_float3 vPos;
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	{
		if (m_bCamMode)
			GET_MANAGEMENT->PlaySounds(szFileName, eSound, 1.f);
		else
			GET_MANAGEMENT->Play3DSounds(szFileName, eSound, vPos, 0.f, 50.f, false, 1.f*m_fSoundMultiple);
	}
	return S_OK;
}

void CJLegionnaireGun::BulletMaker()
{
	auto pManagement = GET_MANAGEMENT;

	if (FAILED(pManagement->Clone_GameObject(
		(_uint)ESceneType::Static,
		L"GameObject_LegionnaireBullet",
		(CGameObject**)&m_pBullet,
		&m_tTransformDesc,
		m_iSceneIdx)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return;
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pBullet, m_iSceneIdx, L"Layer_Weapon")))
	{
		ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
		return;
	}
	m_pBullet->Set_ParentClass(m_pParentObject, CJBaseWeapon::eMonsterParent::Normal);

	Safe_AddRef(m_pBullet);
}

void CJLegionnaireGun::Set_BulletFire(const _float3 & _vLook)
{
	Set_MuzzleWorld();
	if (dynamic_cast<CJLegionnaireBullet*>(m_pBullet)->Get_BulletState() == 0)
	{
		_float3 vLook = _vLook - m_vWorldMuzzlePos;
		D3DXVec3Normalize(&vLook, &vLook);
		dynamic_cast<CJLegionnaireBullet*>(m_pBullet)->Set_ShootAndLook(vLook, m_vWorldMuzzlePos);
	}
}

void CJLegionnaireGun::Set_MuzzleWorld()
{
	D3DXVec3TransformCoord(&m_vWorldMuzzlePos, &m_vMuzzlePos, m_pTransform->Get_WorldMatrix());

}

CJLegionnaireGun * CJLegionnaireGun::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJLegionnaireGun* pInstance = new CJLegionnaireGun(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJLegionnaireGun::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJLegionnaireGun* pClone = new CJLegionnaireGun(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJLegionnaireGun::Free()
{
	if (m_bCheckClone)
		Safe_Release(m_pBullet);
	CJBaseWeapon::Free();
}
