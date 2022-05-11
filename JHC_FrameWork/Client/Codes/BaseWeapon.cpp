#include "stdafx.h"
#include "BaseMonster.h"
#include "BaseWeapon.h"

CBaseWeapon::CBaseWeapon(_DEVICE _pDevice, OBJECTINFO_DESC * _tDesc)
	:CGameObject(_pDevice)
	, m_bAttack(false)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pParentBoneMatrix()
	, m_pParentWorldMatrix()
	, m_pMonsterMeshCom(nullptr)
	, m_fDeathTime(0.f)
	, m_vDir(0.f, 0.f, 0.f)
	, m_iShaderPass(0)
	, m_bDissolve(false)
	, m_fDissolveTime(0.f)
	, m_fDissolveSpeed(0.f)
	, m_fWeaponDamage(0.f)
	, m_bRender(true)
	, m_bCollision(false)
	, m_bWeaponTrigger(false)
	, m_WeaponData(nullptr)
{
}

CBaseWeapon::CBaseWeapon(const CBaseWeapon & _rOther)
	:CGameObject(_rOther)
	, m_bAttack(_rOther.m_bAttack)
	, m_pMesh(_rOther.m_pMesh)
	, m_pTransform(_rOther.m_pTransform)
	, m_pShader(_rOther.m_pShader)
	, m_pParentBoneMatrix(_rOther.m_pParentBoneMatrix)
	, m_pParentWorldMatrix(_rOther.m_pParentWorldMatrix)
	, m_pMonsterMeshCom(_rOther.m_pMonsterMeshCom)
	, m_fDeathTime(_rOther.m_fDeathTime)
	, m_vDir(_rOther.m_vDir)
	, m_iShaderPass(_rOther.m_iShaderPass)
	, m_bDissolve(_rOther.m_bDissolve)
	, m_fWeaponDamage(_rOther.m_fWeaponDamage)
	, m_fDissolveTime(0.f)
	, m_fDissolveSpeed(0.f)
	, m_bRender(_rOther.m_bRender)
	, m_bCollision(_rOther.m_bCollision)
	, m_bWeaponTrigger(_rOther.m_bWeaponTrigger)
	, m_WeaponData(_rOther.m_WeaponData)
{
}

HRESULT CBaseWeapon::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CBaseWeapon::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(CBaseWeapon::Add_Component()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	m_iShaderPass = 2;
	m_fDissolveSpeed = 3.f;

	memcpy(&m_tTransformDesc, _pArg, sizeof(TRANSFORM_DESC));
	lstrcat(m_tTransformDesc.szFileTitle, L"_Weapon");

	return S_OK;
}

_uint CBaseWeapon::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_bDead)
		m_iEvent = (_int)EEvent::Dead;

	CGameObject::Update_GameObject(_fDeltaTime);

	auto pManagement = GET_MANAGEMENT;

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::NoAlpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}
	if (FAILED(pManagement->Add_RenderList(ERenderPriority::Shadow, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	if (m_bDissolve)
	{
		m_fDissolveTime += _fDeltaTime;
		if (m_iShaderPass == 4)
			m_iShaderPass += 4;
		else if (m_iShaderPass < 4)
			m_iShaderPass += 5;
	}

	//m_pTransform->Update_Transform();

	return m_iEvent;
}

_uint CBaseWeapon::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(_fDeltaTime);
	
	if (m_bRender)
	{
		if (m_bCollision)
		{
			m_WeaponData->Collision = true;
		}
		else
		{
			m_WeaponData->Collision = false;
		}

		auto pManagement = GET_MANAGEMENT;
		wstring WeaponName = m_tTransformDesc.szFileTitle;
		pManagement->SetData_Trigger(WeaponName, m_WeaponData);
	}

	return m_iEvent;
}

HRESULT CBaseWeapon::Render_GameObject()
{
	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(m_iShaderPass);
	if (m_bRender)
		m_pMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	_float4x4 matWorld = m_matWorld;
	matWorld *= m_pParentBoneMatrix * m_pParentWorldMatrix;
	if (m_pParentBoneMatrix != nullptr&&m_pParentWorldMatrix != nullptr)
		m_pTransform->Set_WorldMatrix(&matWorld);

	return S_OK;
}

HRESULT CBaseWeapon::Add_Component()
{
	// 원본
	//wstring wstrPrototypeName = L"StaticMesh_" + (wstring)m_tObjectInfoDesc.szFileTitle;
	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		m_strStaticName,
		L"Mesh_Mesh",
		(CMesh**)&m_pMesh
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
	/* For.Com_Transfrom */
	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Component_Transform",
		L"Com_Transform",
		EComponentType::Static,
		(CComponent**)&m_pTransform
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	//현재는 베이스몬스터에서 쉐이더를 하지만 만약 개개의 쉐이더가 필요하면 각각의 몬스터에 넣어줄것
	///* For.Shader_Monster */
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

	return S_OK;
}

HRESULT CBaseWeapon::SetUp_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4			matWorld, matView, matProj;
	auto pManagement = GET_MANAGEMENT;
	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matLightView", pManagement->Get_LightView());
	_pEffect->SetMatrix("g_matLightProj", pManagement->Get_LightProj());
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_ftime", m_fDissolveTime);
	_pEffect->SetFloat("g_fDissolveSpeed", m_fDissolveSpeed);
	_float4 vColor = _float4(0.f, 1.f, 1.f, 1.f);
	_pEffect->SetVector("g_vColor", &vColor);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	return S_OK;
}

void CBaseWeapon::Set_MonsterWorld(_float4x4 _pMonWorldMatrix)
{
	m_pParentWorldMatrix = _pMonWorldMatrix;
}

void CBaseWeapon::Set_BoneWorld(_float4x4 _pBoneWorldMatrix)
{
	m_pParentBoneMatrix = _pBoneWorldMatrix;
}

void CBaseWeapon::Set_Render(_bool _bRender)
{
	m_bRender = _bRender;
}

void CBaseWeapon::Set_MonsterDir(const _float3	_pDir)
{
	m_vDir = _pDir;
}

void CBaseWeapon::Set_MonsterDead(_bool _bDead)
{
	m_bDead = _bDead;
}

void CBaseWeapon::Set_MonsterAttack(_bool _bAttack)
{
	m_bAttack = _bAttack;
}

void CBaseWeapon::Set_Collision(_bool _bColl)
{
	m_bCollision = _bColl;
}

void CBaseWeapon::Set_Dissolve(_bool _bDissolve,_int iShaderPass)
{
	m_bDissolve = _bDissolve;
	m_iShaderPass += iShaderPass;
}

void CBaseWeapon::Set_Damage(_float _fAtt, _float _fDef)
{
	m_WeaponData->Damage = _fAtt * 0.5f - _fDef * 0.25f + m_fWeaponDamage;
}

void CBaseWeapon::Free()
{
	Safe_Delete(m_WeaponData);

	CGameObject::Free();
}
