#include "stdafx.h"
#include "JLegionnaireSword.h"

CJLegionnaireSword::CJLegionnaireSword(_DEVICE _pDevice, const wstring& _PrototypeName /*= L""*/)
	:CJBaseWeapon(_pDevice, _PrototypeName)
{
}

CJLegionnaireSword::CJLegionnaireSword(const CJLegionnaireSword & _rOther)
	: CJBaseWeapon(_rOther)
{
}

HRESULT CJLegionnaireSword::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseWeapon::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	m_fWeaponDamage = 5.f;
	m_iShaderPass = 2;

	return S_OK;
}

_uint CJLegionnaireSword::Update_GameObject(const _float & _fDeltaTime)
{
	CJBaseWeapon::Update_GameObject(_fDeltaTime);

	return m_iEvent;
}

HRESULT CJLegionnaireSword::Clone_TrailEffect()
{
	// 어떤무기인지에 따라 트레일 크기 및 색상 변경하십시오.
	TRAIL_DESC tTrailDesc;
	tTrailDesc.pObj = this;
	lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");
	tTrailDesc.fTrailWidth = 75.f;
	tTrailDesc.fTrailChekRate = 0.005f;


	tTrailDesc.vColorRGB = _float3(0.25f, 0.25f, 0.25f);
	tTrailDesc.bRenderOnOff = false;

	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.iRenderID = 5;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(10, 100.f, -10.f), sizeof(_float3));

	tTrailDesc.matLocal = matWorld;

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect",(CGameObject**)&m_pTrail, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrail, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrail);

	return S_OK;
}

HRESULT CJLegionnaireSword::WeaponSound(_uint iSoundIndex)
{
	_uint iCount = Mersen_ToInt(0, 3);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Legio_SwordSwing%d.ogg", iCount);

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
	_float3 vPos;
	m_pTransform->Get_Info(EInfo::Pos, &vPos);

	GET_MANAGEMENT->StopSound(eSound);
	if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
	{
		if (m_bCamMode)
			GET_MANAGEMENT->PlaySounds(szFileName, eSound, 1.f);
		else
			GET_MANAGEMENT->Play3DSounds(szFileName, eSound, vPos, 0.f, 50.f, false, 1.f*m_fSoundMultiple);
	}
	return S_OK;
}

CJLegionnaireSword * CJLegionnaireSword::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJLegionnaireSword* pInstance = new CJLegionnaireSword(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJLegionnaireSword::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJLegionnaireSword* pClone = new CJLegionnaireSword(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJLegionnaireSword::Free()
{
	CJBaseWeapon::Free();
}
