#include "stdafx.h"
#include "JBaseWeapon.h"
#include "JBaseBoss.h"
#include "JBaseMonster.h"
#include "JLegionnaireBullet.h"
#include "MainCamera.h"
CJBaseWeapon::CJBaseWeapon(_DEVICE _pDevice, const wstring& _PrototypeName /*= L""*/)
	:CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_bUpdate(false), m_bDissolve(false), m_PxRigidCreate(false)
	, m_fWeaponDamage(0.f)
	, m_fDissolveTime(0.f), m_fDissolveSpeed(0.f)
	, m_iShaderPass(0), m_iDiffuseIndex(0)
	, m_pParentObject(nullptr), m_pTrail(nullptr)
	, m_bHitEffectOnce(false)
	, m_eParentCategory(eMonsterParent::End)
	, m_bAttack(false)
	, m_fSoundMultiple(0.f)
	, m_bCamMode(false)
{
	m_PrototypeName = _PrototypeName;
}

CJBaseWeapon::CJBaseWeapon(const CJBaseWeapon & _rOther)
	:CGameObject(_rOther)
	, m_pMesh(_rOther.m_pMesh)
	, m_pTransform(_rOther.m_pTransform)
	, m_pShader(_rOther.m_pShader)
	, m_bUpdate(false), m_bDissolve(false), m_PxRigidCreate(false)
	, m_fWeaponDamage(0.f)
	, m_fDissolveTime(0.f), m_fDissolveSpeed(0.f)
	, m_iShaderPass(0), m_iDiffuseIndex(0)
	, m_pParentObject(nullptr), m_pTrail(nullptr)
	, m_bHitEffectOnce(false)
	, m_eParentCategory(eMonsterParent::End)
	, m_bAttack(false)
	, m_fSoundMultiple(0.f)
	, m_bCamMode(false)
{
}

HRESULT CJBaseWeapon::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CJBaseWeapon::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
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

	if (FAILED(CJBaseWeapon::Add_Component()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(Clone_TrailEffect()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	return S_OK;
}

_uint CJBaseWeapon::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_bDead || !m_pParentObject)
		return m_iEvent = (_int)EEvent::Dead;
	
	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
	if ((_uint)pCamera->Get_CamMode() > 1)
		m_bCamMode = true;
	else
		m_bCamMode = false;

	if(m_eParentCategory == eMonsterParent::Normal)
	{
		if (dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Ready ||
			dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Sleep ||
			dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::End)
			return m_iEvent;
		else
		{
			if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::NoAlpha, this)))
			{
				ERROR_BOX(L"Failed to Add_RenderList");
				return 0;
			}
			if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
			{
				ERROR_BOX(L"Failed to Add_RenderList");
				return 0;
			}

			HitPosToEffect();
		}
	}
	else
	{
		if (dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Ready ||
			dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Sleep ||
			dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::End)
			return m_iEvent;
		else
		{
			if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::NoAlpha, this)))
			{
				ERROR_BOX(L"Failed to Add_RenderList");
				return 0;
			}
			if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
			{
				ERROR_BOX(L"Failed to Add_RenderList");
				return 0;
			}

			HitPosToEffect();
		}
	}
	if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_CINEMATICSTART) &&
		!(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_CINEMATICEND))
	{
		m_fSoundMultiple = 0.f;
	}
	else
	{
		m_fSoundMultiple = 1.f;

	}
	CGameObject::Update_GameObject(_fDeltaTime);

	m_pTransform->Update_Transform();

	return m_iEvent;
}

_uint CJBaseWeapon::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (m_bDead || !m_pParentObject)
		return m_iEvent = (_int)EEvent::Dead;

	if (m_eParentCategory == eMonsterParent::Normal)
	{
		if (dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Ready ||
			dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Sleep ||
			dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::End)
			return m_iEvent;
		else
		{
			Check_ParentDissolve();
		}
	}
	else
	{
		wstring wstrTrigger = m_tTransformDesc.szFileTitle;

		if (dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Ready ||
			dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Sleep ||
			dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::End)
			return m_iEvent;
		else
		{
			Check_ParentDissolve();
		}
	}
	CGameObject::LateUpdate_GameObject(_fDeltaTime);

	return m_iEvent;
}

HRESULT CJBaseWeapon::Render_GameObject()
{
	if (m_eParentCategory == eMonsterParent::Normal)
	{
		if (dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Ready ||
			dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Sleep ||
			dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::End)
			return S_OK;
		else
		{
			m_matParentWorld = dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_ChildWeaponTransform();
			m_pTransform->Set_ParentMatrix(&m_matParentWorld);
			Set_PxWeaponWorld();
		}
	}
	else
	{
		wstring wstrTrigger = m_tTransformDesc.szFileTitle;

		if (dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Ready ||
			dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Sleep ||
			dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::End)
			return S_OK;
		else
		{
			if (wstrTrigger.find(L"Shield") != wstring::npos)
				m_matParentWorld = dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_SecondChildWeaponTransform();
			else
				m_matParentWorld = dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_ChildWeaponTransform();
			if (m_matParentWorld._11 > 1.f)
			{
				return S_OK;
			}
			m_pTransform->Set_ParentMatrix(&m_matParentWorld);
			Set_BossPxWeaponWorld();
		}
	}
	Check_ParentDissolve();
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
	
	m_pMesh->Render_Meshes(pEffect, m_iDiffuseIndex);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CJBaseWeapon::Add_Component()
{
	// 원본
	wstring wstrPrototypeName = L"StaticMesh_" + m_PrototypeName;
	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		wstrPrototypeName,
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
		(CComponent**)&m_pTransform,
		&m_tTransformDesc
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	//현재는 베이스몬스터에서 쉐이더를 하지만 만약 개개의 쉐이더가 필요하면 각각의 몬스터에 넣어줄것
	/* For.Shader_Monster */
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

HRESULT CJBaseWeapon::SetUp_ConstantTable(_EFFECT & _pEffect)
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
	_float4 vColor = _float4(2.2f, 0.13f, 0.0f, 0.5f);
	if (m_pParentObject&&m_eParentCategory == eMonsterParent::Normal)
		_pEffect->SetVector("g_vColor", &dynamic_cast<CJBaseMonster*>(m_pParentObject)->Get_DissolveColor());
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	pManagement->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture");

	return S_OK;
}

void CJBaseWeapon::Set_Acttack(_bool _bAttack)
{
	void* Data;
	Get_Manager()->GetData_Trigger(m_tTransformDesc.szFileTitle, &Data);
	if (nullptr == Data)
		return;

	MyPublicData* WeaponData = (MyPublicData*)Data;

	if (_bAttack)
	{
		if (!WeaponData->CollisionCheck && !WeaponData->Collision)
			WeaponData->Collision = true;
		else
			WeaponData->Collision = false;
	}
	else
	{
		WeaponData->CollisionCheck = false;
		WeaponData->Collision = false;
	}

	Get_Manager()->SetData_Trigger(m_tTransformDesc.szFileTitle, (void*)WeaponData);

	if (m_pTrail)
		m_pTrail->Set_RenderOnOff(_bAttack);

	m_bAttack = _bAttack;
}

void CJBaseWeapon::HitPosToEffect()
{
	auto pManagement = GET_MANAGEMENT;

	void* Data;
	pManagement->GetData_Trigger(m_tTransformDesc.szFileTitle, &Data);
	MyPublicData* WeaponData = (MyPublicData*)Data;
	if(!Data)
		return;

	if (!WeaponData->Collision)
	{
		m_bHitEffectOnce = true;
		return;
	}
	else
	{
		const _float4x4 matWorld = *m_pTransform->Get_WorldMatrix();
		_float3 vScale;
		_float4Q Quat;
		_float3 vPos;
		D3DXMatrixDecompose(&vScale, &Quat, &vPos, &matWorld);

		_float fRadius = 0.2f;
		_float3 vRaycast = _float3(0.f, 100.f, 0.f);

		wstring wstrWeaponName = m_tTransformDesc.szFileTitle;
		if (wstrWeaponName.find(L"Yantari") != wstring::npos)
		{
			fRadius = 1.f;
			vRaycast = _float3(0.f, 130.f, 0.f);
		}

		if (wstrWeaponName.find(L"Malkhel") != wstring::npos)
		{
			fRadius = 1.f;
			vRaycast = _float3(0.f, 50.f, 0.f);
		}
					
		D3DXVec3TransformCoord(&vRaycast, &vRaycast, &matWorld);

		_float3 vNormal = vRaycast - vPos;
		D3DXVec3Normalize(&vNormal, &vNormal);

		PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
		transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);		

		MyRayCast* RayCast = pManagement->RayCast_Update(m_tTransformDesc.szFileTitle, transform, vNormal, 2.5f, fRadius);

		if (RayCast)
		{
			//if (RayCast->vPos == _float3(0.f, 0.f, 0.f))
			//	RayCast->vPos = vRaycast;

			//Generate_Particle(RayCast, Quat);
			////tDesc.vScale = _float3(2.5f, 2.5f, 2.5f);
			////Get_Manager()->Generate_Decal(m_iSceneIdx, tDesc, _float4(1.f, 1.f, 1.f, 1.f), 8);
			//CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
			//pCamera->CamShakerWeak();
			//dynamic_cast<CTrailEffect*>(m_pTrailEffect)->Set_RenderOnOff(false);
		}
	}

	//if (m_bHitEffectOnce)
	//{
	//	const _float4x4 matWorld = *m_pTransform->Get_WorldMatrix();
	//	_float3 vScale;
	//	_float4Q Quat;
	//	_float3 vPos;

	//	D3DXMatrixDecompose(&vScale, &Quat, &vPos, &matWorld);

	//	_float3 vRaycast = _float3(0.f, 0.f, 0.f);
	//	_float fDist = 0.f;

	//	wstring wstrWeaponName = m_tTransformDesc.szFileTitle;
	//	if (wstrWeaponName.find(L"Yantari") != wstring::npos)
	//	{
	//		fDist = 5.f;
	//		vRaycast = _float3(0.f, 160.f, -28.f);
	//		_float3 rePos = _float3(0.f, -130.f, 0.f);
	//		D3DXVec3TransformCoord(&rePos, &rePos, &matWorld);

	//		vPos = rePos;
	//	}
	//	else if (wstrWeaponName.find(L"Bullet") != wstring::npos)
	//	{
	//		fDist = 1.5f;
	//		vRaycast = _float3(0.f, 0.f, 10.f);
	//	}
	//	else
	//	{
	//		fDist = 2.5f;
	//		vRaycast = _float3(0.f, 160.f, 0.f);
	//	}

	//	D3DXVec3TransformCoord(&vRaycast, &vRaycast, &matWorld);

	//	_float3 vNormal = vRaycast - vPos;
	//	D3DXVec3Normalize(&vNormal, &vNormal);

	//	MyRayCast* RayCast = pManagement->RayCast_Update(m_tTransformDesc.szFileTitle, vPos, vNormal, fDist);

	//	if (RayCast)
	//	{
	//		//_float3 vPos = RayCast->vPos;

	//		//Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, RayCast->vPos, 1.f, nullptr, 4,
	//		//	_ARGB(255, 255, 93, 48), ePartilcePatternType::OnStratBlinkAndGoUp, 0, 2.2f, D3DX_PI, 3.f);
	//		//Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, RayCast->vPos, 1.f, nullptr, 4,
	//		//	_ARGB(255, 255, 93, 48), ePartilcePatternType::OnStratBlinkAndGoUp, 3, 2.2f, D3DX_PI, 1.f);
	//		//Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, RayCast->vPos, 1.f, nullptr, 4,
	//		//	_ARGB(255, 255, 93, 48), ePartilcePatternType::OnStratBlinkAndGoUp, 4, 2.2f, D3DX_PI, 1.f);
	//		//Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, RayCast->vPos, 1.f, nullptr, 4,
	//		//	_ARGB(255, 255, 93, 48), ePartilcePatternType::OnStratBlinkAndGoUp, 5, 2.2f, D3DX_PI, 1.f);
	//		//TRANSFORM_DESC tDesc;
	//		//tDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	//		//tDesc.vPos = RayCast->vPos;
	//		//tDesc.vRotate = m_pTransform->Get_TransformDesc().vRotate;
	//		//tDesc.vRotate.z += D3DXToRadian(90.f);
	//		//tDesc.fSpeedPerSecond = 3.f;
	//		//Get_Manager()->Generate_Decal(m_iSceneIdx, tDesc, _float4(1.f, 93 / 255.f, 48 / 255.f, 1.f), 6);
	//		////tDesc.vScale = _float3(2.5f, 2.5f, 2.5f);
	//		////Get_Manager()->Generate_Decal(m_iSceneIdx, tDesc, _float4(1.f, 1.f, 1.f, 1.f), 8);
	//		//CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
	//		//pCamera->CamShakerWeak();

	//		if (wstrWeaponName.find(L"Bullet") != wstring::npos)
	//		{
	//			CJLegionnaireBullet* pBullet = dynamic_cast<CJLegionnaireBullet*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Weapon", EFindGameObject::Clone, m_tTransformDesc.szFileTitle));
	//			pBullet->Delete_Rigidbody();
	//		}

	//		m_bHitEffectOnce = false;

	//		//dynamic_cast<CTrailEffect*>(m_pTrailEffect)->Set_RenderOnOff(false);
	//	}
	//}
}

void CJBaseWeapon::Set_TriggerDead()
{
	Get_Manager()->Delete_Trigger(m_tTransformDesc.szFileTitle);
}

void CJBaseWeapon::Set_PxWeaponWorld()
{
	if (dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Awake ||
		dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Walk ||
		dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Idle ||
		dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Hit ||
		dynamic_cast<CJBaseMonster*> (m_pParentObject)->Get_MonsterPattern() == eMonsterPattern::Attack)
	{
		_float4x4 matWorld = *m_pTransform->Get_WorldMatrix();
		_float3 vScale;
		_float4Q vQuat;
		_float3 vPos;
		D3DXMatrixDecompose(&vScale, &vQuat, &vPos, &matWorld);
		Get_Manager()->Weapon_Update(m_tTransformDesc.szFileTitle, vPos, vQuat);
	}
}

void CJBaseWeapon::Set_BossPxWeaponWorld()
{
	if (dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Awake ||
		dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Move ||
		dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Idle ||
		dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Hit ||
		dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Attack)
	{
		_float4x4 matWorld = *m_pTransform->Get_WorldMatrix();
		_float3 vScale;
		_float4Q vQuat;
		_float3 vPos;
		D3DXMatrixDecompose(&vScale, &vQuat, &vPos, &matWorld);
		Get_Manager()->Weapon_Update(m_tTransformDesc.szFileTitle, vPos, vQuat);
	}
}

void CJBaseWeapon::Check_ParentDissolve()
{
	NULL_CHECK_RETURN(m_pParentObject, );
	if (m_eParentCategory == eMonsterParent::Normal)
	{
		const LPBASEMONSTER& pParent = dynamic_cast<CJBaseMonster*>(m_pParentObject);
	
		m_fDissolveTime = pParent->Get_DissolveTime();
		m_fDissolveSpeed = pParent->Get_DissolveSpeed();//이 부분 주석 쳐야 할 수 있음. 필요 없을 수 있음.
		const _uint& iPass = pParent->Get_ShaderPass();

		if (iPass < 4)
			m_iShaderPass = 2;
		else if (iPass > 4 && iPass <8)
			m_iShaderPass = 7;
		else if (iPass > 8 && iPass < 12)
			m_iShaderPass = 11;
	}
	else if (m_eParentCategory == eMonsterParent::Boss)
	{
		class CJBaseBoss* pParent = dynamic_cast<CJBaseBoss*>(m_pParentObject);

		m_fDissolveTime = pParent->Get_DissolveTime();
		m_fDissolveSpeed = pParent->Get_DissolveSpeed();//이 부분 주석 쳐야 할 수 있음. 필요 없을 수 있음.
		const _uint& iPass = pParent->Get_ShaderPass();
		m_iShaderPass = iPass;
	}
	else
		return;
	
}

void CJBaseWeapon::Free()
{
	if (m_bCheckClone)
		Safe_Release(m_pTrail);
	CGameObject::Free();
}
