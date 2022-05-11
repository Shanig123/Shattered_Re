#include "stdafx.h"
#include "JLegionnaireBullet.h"
#include "JBaseMonster.h"
CJLegionnaireBullet::CJLegionnaireBullet(_DEVICE _pDevice, const wstring& _PrototypeName /*= L""*/)
	:CJBaseWeapon(_pDevice, _PrototypeName)
	, m_fLifeTime(0.f)
	, m_vLook(0.f, 0.f, 0.f)
	, m_vOriginPos(0.f, 0.f, 0.f)
	, m_eBulletState(eBulletState::End)
	, m_bCoolTime(false)
{
}

CJLegionnaireBullet::CJLegionnaireBullet(const CJLegionnaireBullet & _rOther)
	: CJBaseWeapon(_rOther)
	, m_fLifeTime(0.f)
	, m_vLook(0.f, 0.f, 0.f)
	, m_vOriginPos(0.f, 0.f, 0.f)
	, m_eBulletState(eBulletState::End)
	, m_bCoolTime(false)
{
}

HRESULT CJLegionnaireBullet::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
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

	lstrcat(m_tTransformDesc.szFileTitle, L"Bullet");

	if (FAILED(CJLegionnaireBullet::Add_Component()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(Clone_TrailEffect()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}
	m_eBulletState = eBulletState::Ready;


	m_fWeaponDamage = 15.f;
	m_iShaderPass = 0;

	return S_OK;
}

_uint CJLegionnaireBullet::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	switch (m_eBulletState)
	{
	case CJLegionnaireBullet::eBulletState::Ready:
		m_pTrail->Set_RenderOnOff(false);
		break;
	case CJLegionnaireBullet::eBulletState::Shoot:
		m_pTrail->Set_RenderOnOff(true);
		Shooting(_fDeltaTime);
		break;
	default:
		break;
	}	

	m_pTransform->Update_Transform();

	if(eBulletState::Shoot == m_eBulletState)
		HitPosToEffect();

	if (m_bCoolTime)
	{
		if (::CoolTime(_fDeltaTime, m_fLifeTime, 4.f))
		{
			Delete_Rigidbody();
			m_bCoolTime = false;
		}
	}

	return m_iEvent;
}

HRESULT CJLegionnaireBullet::Render_GameObject()
{
	return S_OK;
}

HRESULT CJLegionnaireBullet::Clone_TrailEffect()
{
	// 어떤무기인지에 따라 트레일 크기 및 색상 변경하십시오.
	TRAIL_DESC tTrailDesc;
	tTrailDesc.pObj = this;
	lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");
	tTrailDesc.fTrailWidth = 0.5f;
	tTrailDesc.fTrailChekRate = 0.f;


	tTrailDesc.vColorRGB = _float3(3.f, 0.25f, 0.25f);
	tTrailDesc.bRenderOnOff = false;

	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.iRenderID = 4;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(0.f, 0.f, 0.f), sizeof(_float3));

	tTrailDesc.matLocal = matWorld;

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrail, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrail, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTrail);
	return S_OK;
}

HRESULT CJLegionnaireBullet::WeaponSound(_uint iSoundIndex)
{
	return S_OK;
}

HRESULT CJLegionnaireBullet::Add_Component()
{
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

	return S_OK;
}

void CJLegionnaireBullet::Set_ShootAndLook(const _float3&  _vLook, const _float3& _vPos)
{
	m_vOriginPos = _vPos;
	m_vLook = _vLook;
	Get_Manager()->Add_Weapon(m_tTransformDesc.szFileTitle, m_vOriginPos, 5.f, PxSphereGeometry(0.2f), 0.f);
	Set_Acttack(true);
	m_eBulletState = eBulletState::Shoot;
	m_bHitEffectOnce = true;
}

void CJLegionnaireBullet::Delete_Rigidbody()
{
	Get_Manager()->Delete_Trigger(m_tTransformDesc.szFileTitle);
	m_pTransform->Set_Pos(m_vOriginPos);
	m_fLifeTime = 0.f;
	m_eBulletState = eBulletState::Ready;

	m_bCoolTime = false;
}

void CJLegionnaireBullet::Shooting(const _float & _fDeltaTime)
{
	_float3 vPos;
	GET_MANAGEMENT->ActorMove(m_tTransformDesc.szFileTitle, &vPos, m_vLook, 30.f, _fDeltaTime);
	m_pTransform->Set_Pos(vPos);
	
	/*CManagement::Get_Instance()->Generate_Particle(
		m_iSceneIdx,
		m_pTransform->Get_TransformDesc().vPos,
		0.f,
		1,
		_float4(0.250980f, 0.925490f, 1.f, 1.f)
		, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink)
		, 1, 2.2f);*/
	//Set_Acttack(true);


	m_bCoolTime = true;	
}

CJLegionnaireBullet * CJLegionnaireBullet::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJLegionnaireBullet* pInstance = new CJLegionnaireBullet(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJLegionnaireBullet::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJLegionnaireBullet* pClone = new CJLegionnaireBullet(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJLegionnaireBullet::Free()
{
	CJBaseWeapon::Free();
}
