#include "stdafx.h"
#include "JPretoSickle.h"
#include "JBaseBoss.h"

CJPretoSickle::CJPretoSickle(_DEVICE _pDevice, const wstring& _PrototypeName /*= L""*/)
	:CJBaseWeapon(_pDevice, _PrototypeName)
	, m_pTrail0(nullptr), m_pTrail1(nullptr), m_pTrail2(nullptr), m_pDistortTrail(nullptr)
{
}

CJPretoSickle::CJPretoSickle(const CJPretoSickle & _rOther)
	: CJBaseWeapon(_rOther)
	, m_pTrail0(nullptr), m_pTrail1(nullptr), m_pTrail2(nullptr), m_pDistortTrail(nullptr)
{
}

HRESULT CJPretoSickle::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (_pArg)
	{
		memcpy(&m_tTransformDesc, _pArg, sizeof(TRANSFORM_DESC));
	}


	/* For.Com_Transfrom */
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Transform",
		L"Com_Transform",
		EComponentType::Static,
		(CComponent**)&m_pTransform,
		&m_tTransformDesc
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	/* For.Shader_General_Static */
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Shader_General_Static",
		L"Shader_General_Static",
		EComponentType::Static,
		(CComponent**)&m_pShader
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	if (FAILED(Clone_TrailEffect()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	//Get_Manager()->Add_Weapon(m_tTransformDesc.szFileTitle, vPos, PxCapsuleGeometry(1.f, 5.f), 0.f);

	m_fWeaponDamage = 30.f;
	m_iShaderPass = 2;

	return S_OK;
}

_uint CJPretoSickle::Update_GameObject(const _float & _fDeltaTime)
{
	

	//m_pTrail->Set_Color(_float3(1.f, 78.f / 255.f, 11.f / 255.f));
	//m_pTrail->Set_Color(_float3(207.f / 255.f, 55.f / 255.f, 26.f / 255.f));
	//m_pTrail->Set_Color(_float3(powf(1.f, 2.2f), powf(78.f / 255.f, 2.2f), powf(11.f / 255.f, 2.2f)));
	//m_pTrail->Set_Color(_float3(powf(207.f / 255.f, 2.2f), powf(55.f / 255.f, 2.2f), powf(26.f / 255.f, 2.2f)));
	//m_pTrail->Set_Color(_float3(powf(225.f / 255.f, 2.2f), powf(36.f / 255.f, 2.2f), powf(3.f / 255.f, 2.2f)));

	//m_pTrail->Set_Color(_float3(225.f / 255.f, 36.f / 255.f, 3.f / 255.f));
	//m_pTrail->Set_Color(_float3(242.f / 255.f, 164.f / 255.f, 5.f / 255.f));

	//m_pTrail->Set_Color(_float3(powf(242.f / 255.f, 2.2f), powf(164.f / 255.f, 2.2f), powf(5.f / 255.f, 2.2f)));
	
	
	CGameObject::Update_GameObject(_fDeltaTime);
	m_pTransform->Update_Transform();

	m_matParentWorld = dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_ChildWeaponTransform();
	
	m_pTransform->Set_ParentMatrix(&m_matParentWorld);
	m_pTrail0->Set_RenderOnOff(m_pTrail->Get_RenderOnOff());
	m_pTrail1->Set_RenderOnOff(m_pTrail->Get_RenderOnOff());
	m_pTrail2->Set_RenderOnOff(m_pTrail->Get_RenderOnOff());
	m_pDistortTrail->Set_RenderOnOff(m_pTrail->Get_RenderOnOff());

	m_pTrail->Set_ParticleOnOff(m_pTrail->Get_RenderOnOff());
	return m_iEvent;
}

void CJPretoSickle::Set_Trail_LocalWorld(const _float4x4 & _matLocal)
{
	if (!m_pTrail)
		return;

	m_pTrail->Set_LocalWorld(_matLocal);

}

HRESULT CJPretoSickle::WeaponSound(_uint iSoundIndex)
{
	return E_NOTIMPL;
}

CJPretoSickle * CJPretoSickle::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJPretoSickle* pInstance = new CJPretoSickle(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJPretoSickle::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJPretoSickle* pClone = new CJPretoSickle(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJPretoSickle::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pTrail);
		Safe_Release(m_pTrail0);
		Safe_Release(m_pTrail1);
		Safe_Release(m_pTrail2);
		Safe_Release(m_pDistortTrail);
	}
	CJBaseWeapon::Free();
}

HRESULT CJPretoSickle::Clone_TrailEffect()
{
	// 어떤무기인지에 따라 트레일 크기 및 색상 변경하십시오.
	TRAIL_DESC tTrailDesc;
	tTrailDesc.pObj = this;
	lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");

	//tTrailDesc.vColorRGB = _float3(0.25f, 0.25f, 0.25f);
	//tTrailDesc.vColorRGB = _float3(powf(207.f / 255.f,2.2f), powf(55.f / 255.f,2.2f), powf(26.f / 255.f,2.2f));
	
	//tTrailDesc.vColorRGB = _float3(powf(242.f / 255.f, 2.2f), powf(164.f / 255.f, 2.2f), powf(5.f / 255.f, 2.2f));
	tTrailDesc.vColorRGB = _float3(242.f / 255.f, 164.f / 255.f, 5.f / 255.f);
	tTrailDesc.fTrailWidth = 100.f;
	tTrailDesc.fTrailChekRate = 0.01f;


	tTrailDesc.bRenderOnOff = false;

	tTrailDesc.iRenderID = 3;
	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(50.f, 200.f, 0.f), sizeof(_float3));

	tTrailDesc.matLocal = matWorld;

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrail, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrail, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrail);


	///////////////////////////////////////

	tTrailDesc.fTrailWidth = 35.f;
	tTrailDesc.vColorRGB = _float3(0.1f, 0.1f, 0.1f);
	tTrailDesc.iRenderID = 10;
	memcpy(&tTrailDesc.matLocal.m[3][0], _float3(60.f, 175.f, 0.f), sizeof(_float3));


	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrail0, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrail0, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrail0);
	
	///////////////////////////////////////
	
	tTrailDesc.fTrailWidth = 35.f;
	memcpy(&tTrailDesc.matLocal.m[3][0], _float3(70.f, 140.f, 0.f), sizeof(_float3));

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrail1, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrail1, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrail1);


	///////////////////////////////////////

	tTrailDesc.fTrailWidth = 20.f;
	memcpy(&tTrailDesc.matLocal.m[3][0], _float3(0.f, 310.f, 0.f), sizeof(_float3));

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrail2, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrail2, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrail2);

	///////////////////////////////////////
	tTrailDesc.fTrailWidth = 300.f;
	tTrailDesc.iRenderID = 9;
	tTrailDesc.fTrailChekRate = 0.01f;
	tTrailDesc.iRenderID_Filter = 1;
	tTrailDesc.eRenderState = ERenderPriority::Environment;
	tTrailDesc.iRenderPass = 4;
	memcpy(&tTrailDesc.matLocal.m[3][0], _float3(50.f, 250.f, 0.f), sizeof(_float3));

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&	m_pDistortTrail, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pDistortTrail, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pDistortTrail);
	m_pDistortTrail->Set_RenderID_Filter(1);
	m_pDistortTrail->Set_RenderID(5);
	m_pDistortTrail->Set_Pass(8);
	return S_OK;
}
