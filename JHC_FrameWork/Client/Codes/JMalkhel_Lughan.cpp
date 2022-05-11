#include "stdafx.h"
#include "JMalkhel_Lughan.h"

CJMalkhel_Lughan::CJMalkhel_Lughan(_DEVICE _pDevice, const wstring & _PrototypeName)
	:CJBaseWeapon(_pDevice, _PrototypeName)
{
}

CJMalkhel_Lughan::CJMalkhel_Lughan(const CJMalkhel_Lughan & _rOther)
	: CJBaseWeapon(_rOther),m_pTrailSmall1(nullptr), m_pTrailSmall2(nullptr), m_pDistortTrail(nullptr)
{
}

HRESULT CJMalkhel_Lughan::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseWeapon::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	m_fWeaponDamage = 7.f;
	m_iShaderPass = 4;

	return S_OK;
}

_uint CJMalkhel_Lughan::Update_GameObject(const _float & _fDeltaTime)
{
	//m_pTrail->Set_RenderOnOff(true);
	CJBaseWeapon::Update_GameObject(_fDeltaTime);
	if (m_pTrail)
	{
		m_pTrailSmall1->Set_RenderOnOff(m_pTrail->Get_RenderOnOff());
		m_pTrailSmall2->Set_RenderOnOff(m_pTrail->Get_RenderOnOff());
		m_pDistortTrail->Set_RenderOnOff(m_pTrail->Get_RenderOnOff());
		m_pTrail->Set_ParticleOnOff(m_pTrail->Get_RenderOnOff());
		m_pTrailSmall1->Set_ParticleOnOff(m_pTrail->Get_RenderOnOff());
		m_pTrailSmall2->Set_ParticleOnOff(m_pTrail->Get_RenderOnOff());
		_float4x4 matWorld;
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
		memcpy(&matWorld.m[3][0], _float3(0.f, 100.f, 0.f), sizeof(_float3));

		m_pTrail->Set_Width(200.f);
		m_pTrail->Set_LocalWorld(matWorld);
		m_pTrail->Set_RenderID(8);
		m_pTrail->Set_Color(_float3(1.f, 0.211764f, 0.f));
	}
	return m_iEvent;
}

HRESULT CJMalkhel_Lughan::Clone_TrailEffect()
{
	// 어떤무기인지에 따라 트레일 크기 및 색상 변경하십시오.
	TRAIL_DESC tTrailDesc;
	tTrailDesc.pObj = this;
	lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");
	tTrailDesc.fTrailWidth = 75.f;
	tTrailDesc.fTrailChekRate = 0.001f;


	tTrailDesc.vColorRGB = _float3(1.f, 0.211764f, 0.f);
	tTrailDesc.bRenderOnOff = false;

	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.iRenderID = 6;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(0.f, 130.f, 0.f), sizeof(_float3));

	tTrailDesc.matLocal = matWorld;

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrail, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrail, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrail);
	////////////////////////////////////////////////////////////////////////////
	tTrailDesc.fTrailWidth = 10.f;
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.iRenderID = 9;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(0.f, 300.f, 0.f), sizeof(_float3));
	tTrailDesc.vColorRGB = _float3(3.f, 0.211764f, 0.f);

	tTrailDesc.matLocal = matWorld;


	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrailSmall1, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrailSmall1, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrailSmall1);
	////////////////////////////////////////////////////////////////////////////
	tTrailDesc.fTrailWidth = 10.f;
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.iRenderID = 4;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(0.f, 60.f, 0.f), sizeof(_float3));
	tTrailDesc.vColorRGB = _float3(0.25f, 0.25f, 0.25f);

	tTrailDesc.matLocal = matWorld;


	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrailSmall2, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrailSmall2, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrailSmall2);
	////////////////////////////////////////////////////////////////////////////
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.fTrailWidth = 350.f;
	tTrailDesc.iRenderID = 4;
	tTrailDesc.fTrailChekRate = 0.01f;
	tTrailDesc.eRenderState = ERenderPriority::Environment;
	tTrailDesc.iRenderPass = 4;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(0.f, 130.f, 0.f), sizeof(_float3));

	tTrailDesc.vColorRGB = _float3(1.f, 1.f, 1.f);
	tTrailDesc.matLocal = matWorld;


	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pDistortTrail, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pDistortTrail, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pDistortTrail);
	return S_OK;
}

HRESULT CJMalkhel_Lughan::WeaponSound(_uint iSoundIndex)
{
	_float3 vPos;
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Malkhel_Weapon%d.ogg", iSoundIndex);
	GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
	GET_MANAGEMENT->Play3DSounds(szFileName, CSound_Manager::ESoundID::Monster_Weapon3D_0, vPos, 0.f, 50.f, false, 1.f);

	return S_OK;
}


CJMalkhel_Lughan * CJMalkhel_Lughan::Create(_DEVICE _pDevice, const wstring & _PrototypeName)
{
	CJMalkhel_Lughan* pInstance = new CJMalkhel_Lughan(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJMalkhel_Lughan::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJMalkhel_Lughan* pClone = new CJMalkhel_Lughan(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJMalkhel_Lughan::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pTrailSmall1);
		Safe_Release(m_pTrailSmall2);
		Safe_Release(m_pDistortTrail);
	}
	CJBaseWeapon::Free();
}
