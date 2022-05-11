#include "stdafx.h"
#include "Player_Weapon.h"
#include "TrailEffect.h"
#include "Player.h"
#include "MapStaticObject_WeaponStand.h"
#include "MainCamera.h"


CPlayer_Weapon::CPlayer_Weapon(_DEVICE _pDevice)
	: CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_bTake_Normal(false)
	, m_bTake_Moon(false)
	, m_bOnHand(false)
	, m_bStand(false)
	, m_eState(WEAPON_STATE_END)
	, m_bAttackColl(false)
	, m_iAttackCount(0)
	, m_fDsvTime(0)
	, m_bOnStand_Dsv(false)
	, m_bOnMoon_Dsv(false)
	, m_bOnNormal_Dsv(false)
	, m_bOnMoon_ReverseDsv(false)
	, m_bOnNormal_ReverseDsv(false)
	, m_fDeltaTime(0.f)
	, m_fReDsvTime(0.f)
	, m_fDsvTimer(0.f)
	, m_bAttackSound(true)
	, m_bCollWallSound(true)
	, m_bCheat(false)
	, m_bCollision(false)
{
	m_PrototypeName = L"Player_Weapon";
}

CPlayer_Weapon::CPlayer_Weapon(const CPlayer_Weapon & _rOther)
	: CGameObject(_rOther)
	, m_pMesh(nullptr)
	, m_pTransform(_rOther.m_pTransform)
	, m_pShader(_rOther.m_pShader)
	, m_bTake_Normal(_rOther.m_bTake_Normal)
	, m_bTake_Moon(_rOther.m_bTake_Moon)
	, m_bOnHand(_rOther.m_bOnHand)
	, m_bStand(_rOther.m_bStand)
	, m_eState(_rOther.m_eState)
	, m_bAttackColl(_rOther.m_bAttackColl)
	, m_iAttackCount(_rOther.m_iAttackCount)
	, m_fDsvTime(_rOther.m_fDsvTime)
	, m_bOnStand_Dsv(_rOther.m_bOnStand_Dsv)
	, m_bOnMoon_Dsv(_rOther.m_bOnMoon_Dsv)
	, m_bOnNormal_Dsv(_rOther.m_bOnMoon_Dsv)
	, m_bOnMoon_ReverseDsv(_rOther.m_bOnMoon_ReverseDsv)
	, m_bOnNormal_ReverseDsv(_rOther.m_bOnNormal_ReverseDsv)
	, m_fDeltaTime(0.f)
	, m_fReDsvTime(0.f)
	, m_fDsvTimer(0.f)
	, m_bAttackSound(true)
	, m_bCollWallSound(true)
	, m_bCheat(false)
	, m_bCollision(false)
{
}

HRESULT CPlayer_Weapon::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Weapon::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (_pArg)
	{
		memcpy(&m_eWeapon, _pArg, sizeof(Player_Weapon));
	}
	if (FAILED(Add_Component()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	switch (m_eWeapon)
	{
	case Player_Weapon::Player_Weapon_Normal_Stand:
		m_bStand = true;
		m_eState = WEAPON_STATE::NORMAL;
		break;
	case Player_Weapon::Player_Weapon_Moon_Stand:
		m_bStand = true;
		m_eState = WEAPON_STATE::MOON;
		break;
	case Player_Weapon::Player_OnTake:
		m_bStand = false;
		Set_Init();
		break;
	}
	////////////////////////////////////////////////////////////////////////////////
	////Trail Test
	if(!m_bStand)
	{
		//트레일 생성 함수
		FAILED_CHECK_RETURN(Clone_Trail(),E_FAIL);
	}
	////////////////////////////////////////////////////////////////////////////////

	m_tWeaponStat.iNormalLevel = 0;	// 기본0
	m_tWeaponStat.iMoonLevel = 0;		// 기본0 
	m_tWeaponStat.fNormalDamage = 10.f;	// 기본10
	m_tWeaponStat.fMoonDamage = 7.f;	// 기본7


	return S_OK;
}

_uint CPlayer_Weapon::Update_GameObject(const _float & _fDeltaTime)
{
	CGameObject::Update_GameObject(_fDeltaTime);

	auto pManagement = GET_MANAGEMENT;

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::NoAlpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}
	if (m_bTake_Moon || m_bTake_Normal || m_bStand)
	{
		if (FAILED(pManagement->Add_RenderList(ERenderPriority::Shadow, this)))
		{
			ERROR_BOX(L"Failed to Add_RenderList");
			return 0;
		}
	}
	
	m_fDeltaTime = _fDeltaTime;


	if (m_bStand)
		Update_OnStand(_fDeltaTime);
	else
	{
		Update_Damage();
		Update_OnPlayer();
		Update_AttackColl();
		HitPosToEffect();
	}
	Update_Sound();
	if(m_bDead)
		return (_uint)EEvent::Dead;

	return (_uint)EEvent::None;
}

_uint CPlayer_Weapon::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(_fDeltaTime);
	
	auto pManagement = GET_MANAGEMENT;
	

	Update_Dsv(_fDeltaTime);

	if (m_bStand && m_bOnStand_Dsv && m_fDsvTime >= 3.f)
		m_bDead = true;

	if (m_bDead)
	{
		if (m_iSceneIdx == (_uint)ESceneType::Tutorial)
		{
			CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
			CManagement::Get_Instance()->PlayBGM(L"Tutorial.ogg", BGM_VOLUME);
		}
		return (_uint)EEvent::Dead;
	}

	return (_uint)EEvent::None;
}

HRESULT CPlayer_Weapon::Render_GameObject()
{
	if (m_bTake_Moon || m_bTake_Normal || m_bStand)
	{
		Render_Choice();
		auto pManagement = GET_MANAGEMENT;

		if (FAILED(m_pTransform->Set_Transform()))
		{
			ERROR_BOX(L"Failed to Set_Transform");
			return E_FAIL;
		}

		LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();
		pEffect->AddRef();

		
		if (FAILED(SetUp_ConstantTable(pEffect)))
		{
			ERROR_BOX(L"Failed To SetRenderState");
			return E_FAIL;
		}

		_uint	iPassMax = 0;
		_uint	iBeginPass = 1;
		if (m_bStand)
			iBeginPass = 14;


		if (m_bOnStand_Dsv || m_bOnNormal_Dsv || m_bOnMoon_Dsv)
		{
			pManagement->Set_Texture(0, L"Component_Texture_Dissolve", pEffect, "g_DissolveTexture");
			iBeginPass = 6;
			// 10
		}
		else if (m_bOnNormal_ReverseDsv || m_bOnMoon_ReverseDsv)
		{
			pManagement->Set_Texture(0, L"Component_Texture_Dissolve", pEffect, "g_DissolveTexture",2);
			iBeginPass = 10;
		}
		pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
		pEffect->BeginPass(iBeginPass);
	
		//m_Meshs.begin()->
		
		m_pMesh->Render_Meshes(pEffect);

		pEffect->CommitChanges();
		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);

	}

	return S_OK;
}

void CPlayer_Weapon::Update_Damage()
{
	auto pManagement = GET_MANAGEMENT;
	if (pManagement->Key_Down(DIK_K))
		m_bCheat = !m_bCheat;

	CDynamicMesh* pPlayerMesh = (CDynamicMesh*)pManagement->Get_Mesh(m_iSceneIdx, L"Layer_Player", L"Mesh_Mesh");

	STAT_DESC	tState = pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player")->Get_Stat();
	_float fAtt=0.f;

	if (m_bTake_Normal)
	{
		if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK1
			|| pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK2
			|| pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK3)
			fAtt = 15.f;
		else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK1
			|| pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK2)
			fAtt = 20.f;
		else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_CHARGING_ATTACK)
			fAtt = 10.f;

		m_tState.fAtt = fAtt + (m_tWeaponStat.iNormalLevel * m_tWeaponStat.fNormalDamage);
	}
	else if (m_bTake_Moon)
	{
		if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::MOON_ATTACK1
			|| pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::MOON_ATTACK2)
			fAtt = 7.f;
		m_tState.fAtt = fAtt + (m_tWeaponStat.iMoonLevel * m_tWeaponStat.fMoonDamage);
	}

	if (m_bCheat)
		m_tState.fAtt = 999.f;
#ifdef _DEBUG
	//m_tState.fAtt = 999.f;
#endif // _debug

	void* Data;
	pManagement->GetData_Trigger(L"PlayerWeapon", &Data);
	MyPublicData* WeaponData = (MyPublicData*)Data;
	WeaponData->Damage = (tState.fAtt * 0.5f) - (tState.fDef * 0.25f) + m_tState.fAtt;
	pManagement->SetData_Trigger(L"PlayerWeapon", WeaponData);
}

void CPlayer_Weapon::Set_TrailRenderOnOff(_bool _bRenderOnOff)
{
	if (!m_pTrailEffect)
		return;
	
	m_pTrailEffect->Set_RenderOnOff(_bRenderOnOff);
	m_pTrailEffect->Set_ParticleOnOff(_bRenderOnOff);
	if (m_pTrailEffectSecond)
		m_pTrailEffectSecond->Set_RenderOnOff(_bRenderOnOff);

	if (m_pTrailEffectDistortion)
		m_pTrailEffectDistortion->Set_RenderOnOff(_bRenderOnOff);
}

void CPlayer_Weapon::Set_Init()
{
	auto pManagement = GET_MANAGEMENT;
	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	pManagement->Add_Weapon(L"PlayerWeapon", vPos, 0.f, PxCapsuleGeometry(0.2f, 1.5f), 1.5f);
}

HRESULT CPlayer_Weapon::Add_Component()
{
	/* For.Com_DynamicMesh */

	/*if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		L"StaticMesh_Player_Normal_Weapon",
		L"Mesh_Normal",
		(CMesh**)&m_pMesh[0]
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		L"StaticMesh_Player_Moon_Sword",
		L"Mesh_Moon",
		(CMesh**)&m_pMesh[1]
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}*/

	

	/*m_Meshs.emplace(m_pMesh[0]);
	m_Meshs.emplace(m_pMesh[1]);*/
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

HRESULT CPlayer_Weapon::SetUp_ConstantTable(_EFFECT _pEffect)
{
	_float4x4			matWorld, matView, matProj;

	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	if (m_bOnStand_Dsv)
	{
		_pEffect->SetFloat("g_ftime", m_fDsvTime);
		_pEffect->SetFloat("g_fDissolveSpeed", 3.f);
		_float4 vColor = _float4(0.f, 1.f, 1.f, 1.f);
		_pEffect->SetVector("g_vColor", &vColor);
		_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	}
	if ( m_bOnMoon_Dsv || m_bOnNormal_Dsv)
	{
		_pEffect->SetFloat("g_ftime", m_fDsvTime);
		_pEffect->SetFloat("g_fDissolveSpeed", 5.f);
		_float4 vColor = _float4(0.f, 1.f, 1.f, 1.f);
		_pEffect->SetVector("g_vColor", &vColor);
		_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	}
	if (m_bOnMoon_ReverseDsv || m_bOnNormal_ReverseDsv)
	{
		_pEffect->SetFloat("g_ftime", m_fReDsvTime);
		_pEffect->SetFloat("g_fDissolveSpeed", 5.f);
		_float4 vColor = _float4(0.f, 1.f, 1.f, 1.f);
		_pEffect->SetVector("g_vColor", &vColor);
		_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	}
	
	/*	float g_ftime;
	float g_fDissolveSpeed;*/
	auto pManagement = GET_MANAGEMENT;

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);


	_pEffect->SetFloat("g_RimPower", 13.f);
	return S_OK;
}

HRESULT CPlayer_Weapon::Clone_Trail()
{
	// 어떤무기인지에 따라 트레일 크기 및 색상 변경하십시오.
	TRAIL_DESC tTrailDesc;
	tTrailDesc.pObj = this;
	lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");
	tTrailDesc.fTrailWidth = 130.f;
	tTrailDesc.fTrailChekRate = 0.02f;

	//75 137 220
	//tTrailDesc.vColorRGB = _float3(75 / 255.f, 137 / 255.f, 220 / 255.f);
	//tTrailDesc.vColorRGB = _float3(0.6f, 0.925490f, 1.f)/*_ARGB(255, 153, 236, 255)*/;
	tTrailDesc.vColorRGB = _float3(0.25f, 0.25f, 0.25f);
	//tTrailDesc.vColorRGB = _float3(1.f, 0.f, 1.f);
	tTrailDesc.bRenderOnOff = false;
	//const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-Head");
	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.iRenderID = 1;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(10, 100.f, -10.f), sizeof(_float3));

	tTrailDesc.matLocal = matWorld;
	m_pTrailEffect = nullptr;
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrailEffect, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrailEffect, m_iSceneIdx, L"Layer_Player_Effect"), E_FAIL);
	Safe_AddRef(m_pTrailEffect);
	///////////////////////////////////////////////////////////////////////////////////////////////
	tTrailDesc.fTrailWidth = 25.f;
	tTrailDesc.fTrailChekRate = 0.005f;

	//75 137 220
	//tTrailDesc.vColorRGB = _float3(75 / 255.f, 137 / 255.f, 220 / 255.f);

	//tTrailDesc.vColorRGB = _float3(0.607843f, 0.529411f, 1.f);
	tTrailDesc.vColorRGB = _float3(0.1f, 0.1f, 0.1f);

	//tTrailDesc.vColorRGB = _float3(1.f, 0.f, 1.f);
	tTrailDesc.bRenderOnOff = false;
	//const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-Head");
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.iRenderID = 9;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(20, 190.f, -10.f), sizeof(_float3));

	tTrailDesc.matLocal = matWorld;
	m_pTrailEffectSecond = nullptr;
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrailEffectSecond, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrailEffectSecond, m_iSceneIdx, L"Layer_Player_Effect"), E_FAIL);
	Safe_AddRef(m_pTrailEffectSecond);

	///////////////////////////////////////////////////////////////////////////////////////////////
	tTrailDesc.fTrailWidth = 200.f;
	tTrailDesc.bRenderOnOff = false;
	D3DXMatrixIdentity(&matWorld);
	tTrailDesc.iRenderID = 11;
	tTrailDesc.fTrailChekRate = 0.01f;
	tTrailDesc.eRenderState = ERenderPriority::Environment;
	tTrailDesc.iRenderPass = 8;
	tTrailDesc.iRenderID_Filter = 1;
	D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	memcpy(&matWorld.m[3][0], _float3(20, 100.f, -10.f), sizeof(_float3));

	tTrailDesc.matLocal = matWorld;
	m_pTrailEffectDistortion = nullptr;
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_TrailEffect", (CGameObject**)&m_pTrailEffectDistortion, &tTrailDesc), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrailEffectDistortion, m_iSceneIdx, L"Layer_Player_Effect"), E_FAIL);
	Safe_AddRef(m_pTrailEffectDistortion);
	m_pTrailEffectDistortion->Set_Pass(8);
	return S_OK;
}

HRESULT CPlayer_Weapon::Render_Choice()
{
	CMesh* pMesh;
	if (m_eState == NORMAL)
	{
		auto iter_find = m_Meshs.find(L"Mesh_Normal");
		if (m_Meshs.end() == iter_find)
		{
			for_each(m_Meshs.begin(), m_Meshs.end(), [](auto& Pair)
			{
				{
					Safe_Release(Pair.second);
				}
			});
			m_Meshs.clear();

			pMesh = GET_MANAGEMENT->Clone_Mesh(0, L"StaticMesh_Player_Normal_Weapon");
			if (nullptr == pMesh)
			{
				ERROR_BOX(L"pMesh is nullptr");
				return E_FAIL;
			}
			m_Meshs.emplace(L"Mesh_Normal", pMesh);
			auto iter_find = m_Meshs.find(L"Mesh_Normal");
			m_pMesh = (CStaticMesh*)iter_find->second;
		}
	}
	else if (m_eState == MOON)
	{
		auto iter_find = m_Meshs.find(L"Mesh_Moon");
		if (m_Meshs.end() == iter_find)
		{
			for_each(m_Meshs.begin(), m_Meshs.end(), [](auto& Pair)
			{
				{
					Safe_Release(Pair.second);
				}
			});
			m_Meshs.clear();


		/*	for_each(m_Meshs.begin(), m_Meshs.end(), Safe_Delete<CMesh>);
			m_Meshs.clear();*/
			pMesh = GET_MANAGEMENT->Clone_Mesh(0, L"StaticMesh_Player_Moon_Sword");
			if (nullptr == pMesh)
			{
				ERROR_BOX(L"pMesh is nullptr");
				return E_FAIL;
			}
			m_Meshs.emplace(L"Mesh_Moon", pMesh);
			auto iter_find = m_Meshs.find(L"Mesh_Moon");
			m_pMesh = (CStaticMesh*)iter_find->second;
		}
	}



	/*CMesh* pMesh = GET_MANAGEMENT->Clone_Mesh(m_iSceneIdx, L"StaticMesh_Player_Normal_Weapon");
	if (nullptr == pMesh)
	{
		ERROR_BOX(L"pMesh is nullptr");
		return E_FAIL;
	}
	m_Meshs.emplace(L"Mesh_Normal", pMesh);
	pMesh = GET_MANAGEMENT->Clone_Mesh(m_iSceneIdx, L"StaticMesh_Player_Moon_Sword");
	if (nullptr == pMesh)
	{
		ERROR_BOX(L"pMesh is nullptr");
		return E_FAIL;
	}
	m_Meshs.emplace(L"Mesh_Moon", pMesh);*/
	

	return S_OK;
}

CPlayer_Weapon * CPlayer_Weapon::Create(_DEVICE _pDevice)
{
	CPlayer_Weapon* pInstance = new CPlayer_Weapon(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Weapon::Clone(const _uint& _iSceneIdx, void * _pArg)
{
	CPlayer_Weapon* pClone = new CPlayer_Weapon(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPlayer_Weapon::Free()
{
	if (m_bCheckClone && !m_bStand)
	{
		Safe_Release(m_pTrailEffect);
		Safe_Release(m_pTrailEffectSecond);
		Safe_Release(m_pTrailEffectDistortion);
	}
	CGameObject::Free();
}

void CPlayer_Weapon::Update_OnPlayer()
{
	auto pManagement = GET_MANAGEMENT;



	CDynamicMesh* pPlayerMesh = (CDynamicMesh*)pManagement->Get_Mesh(m_iSceneIdx, L"Layer_Player", L"Mesh_Mesh");
	const MY_FRAME* pFrame = nullptr;
	if (m_bTake_Normal)
	{
		m_eState = NORMAL;

		if (!m_bOnHand)
			pFrame = pPlayerMesh->Get_FrameByName("Dummy_sheath");
		else
			pFrame = pPlayerMesh->Get_FrameByName("Dummy_hand");

		m_pTransform->Set_Rotate({ D3DXToRadian(270.f),D3DXToRadian(0.f),D3DXToRadian(180.f) });
		m_pTransform->Set_Pos({ 0.f, 0.f, 0.f });
	}
	else if (m_bTake_Moon)
	{
		m_eState = MOON;
		if (!m_bOnHand)
		{
			pFrame = pPlayerMesh->Get_FrameByName("Bip001-Xtra01");
			m_pTransform->Set_Rotate({ D3DXToRadian(0.f),D3DXToRadian(0.f),D3DXToRadian(165.f) });
			m_pTransform->Set_Pos({ 0.f, 40.f, 10.f });
		}
		else
		{
			pFrame = pPlayerMesh->Get_FrameByName("Dummy_hand");
			m_pTransform->Set_Pos({ 0.f, 0.f, 0.f });
			if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::PARRY && pPlayerMesh->Get_AnimationFrame(40, 18))
				m_pTransform->Rotate(EAxis::X, D3DXToRadian(90.f),10.f, m_fDeltaTime);
			else
				m_pTransform->Set_Rotate({ D3DXToRadian(90.f),D3DXToRadian(0.f),D3DXToRadian(0.f) });
		}
	}
	else
	{
		pFrame = pPlayerMesh->Get_FrameByName("Dummy_sheath");
	}

	m_ParentBoneMatrix = &pFrame->CombinedTranformationMatrix;
	CTransform*	pPlayerTransformCom = (CTransform*)pManagement->Get_Component(m_iSceneIdx, L"Layer_Player", L"Com_Transform");
	m_ParentWorldMatrix = pPlayerTransformCom->Get_WorldMatrix();

	m_pTransform->Update_Transform();
	m_pTransform->Set_ParentMatrix(&(*m_ParentBoneMatrix * *m_ParentWorldMatrix));

	_float4x4 matWorld = *m_pTransform->Get_WorldMatrix();
	_float3 vScale;
	_float4Q vQuat;
	_float3 vPos;
	D3DXMatrixDecompose(&vScale, &vQuat, &vPos, &matWorld);
	pManagement->Weapon_Update(L"PlayerWeapon", vPos, vQuat);

	//CPlayer_Weapon::Set_TrailRenderOnOff(true);
	CheckTrailEffect();
}

void CPlayer_Weapon::Update_OnStand(const _float & _fDeltaTime)
{
	_float3 vPos, vScale;
	auto pManagement = GET_MANAGEMENT;
	CMapStaticObject_WeaponStand* pStand = dynamic_cast<CMapStaticObject_WeaponStand*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Proto, L"WeaponStand"));
	CTransform* pTransform = dynamic_cast<CTransform*>(pStand->Get_Component(L"Com_Transform"));
	vPos = pTransform->Get_TransformDesc().vPos;
	vPos.y += 4.f;
	vScale = pTransform->Get_TransformDesc().vScale;
	m_pTransform->Set_Scale(vScale);
	m_pTransform->Set_Pos(vPos);
	m_pTransform->Rotate(EAxis::Y, D3DXToRadian(20.f), _fDeltaTime);
	m_pTransform->Update_Transform();
}

void CPlayer_Weapon::Update_AttackColl()
{
	auto pManagement = GET_MANAGEMENT;
	CDynamicMesh* pPlayerMesh = (CDynamicMesh*)pManagement->Get_Mesh(m_iSceneIdx, L"Layer_Player", L"Mesh_Mesh");
	CPlayer* pPlayer = (CPlayer*)pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player");

	if (m_bOnHand)
	{
		if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK1)
		{
			if(pPlayerMesh->Get_AnimationFrame(44, 0) && !pPlayerMesh->Get_AnimationFrame(44, 17))
				pPlayer->Set_Rotate(true);

			if (pPlayerMesh->Get_AnimationFrame(44, 17) && !pPlayerMesh->Get_AnimationFrame(44, 30))// 1타 17프레임(공격 직전)
			{
				m_bCollision = true;
				CPlayer_Weapon::Set_TrailRenderOnOff(true);
				pPlayer->Set_Rotate(false);
				if (!pPlayerMesh->Get_AnimationFrame(44, 20))
					pPlayer->Set_AttackMove(true);
				if (pPlayerMesh->Get_AnimationFrame(44, 21))
					pPlayer->Set_AttackMove(false);
			}				
			else
			{
				m_bCollision = false;
				CPlayer_Weapon::Set_TrailRenderOnOff(false);
				pPlayer->Set_AttackMove(false);
			}
		}
		else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK2)
		{
			if (pPlayerMesh->Get_AnimationFrame(34, 0) && !pPlayerMesh->Get_AnimationFrame(34, 11))
				pPlayer->Set_Rotate(true);

			if (pPlayerMesh->Get_AnimationFrame(34, 11))// 2타 13프레임(공격 직전)
			{
				m_bCollision = true;
				CPlayer_Weapon::Set_TrailRenderOnOff(true);
				pPlayer->Set_Rotate(false);
				if (!pPlayerMesh->Get_AnimationFrame(34, 14))
					pPlayer->Set_AttackMove(true);
				if (pPlayerMesh->Get_AnimationFrame(34, 15))
					pPlayer->Set_AttackMove(false);
			}
			else
			{
				m_bCollision = false;
				CPlayer_Weapon::Set_TrailRenderOnOff(false);
				pPlayer->Set_AttackMove(false);
			}	
		}
		else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK3)
		{
			if (pPlayerMesh->Get_AnimationFrame(53, 0) && !pPlayerMesh->Get_AnimationFrame(53, 15))
				pPlayer->Set_Rotate(true);
			if (pPlayerMesh->Get_AnimationFrame(53, 15) && !pPlayerMesh->Get_AnimationFrame(53, 28))// 3타 15프레임(찌르기 직전)
			{
				m_bCollision = true;
				CPlayer_Weapon::Set_TrailRenderOnOff(true);
				pPlayer->Set_Rotate(false);
				if (!pPlayerMesh->Get_AnimationFrame(53, 18))
					pPlayer->Set_AttackMove(true);
				if (pPlayerMesh->Get_AnimationFrame(53, 19))
					pPlayer->Set_AttackMove(false);
			}
			else
			{
				if (pPlayerMesh->Get_AnimationFrame(53, 30) && !pPlayerMesh->Get_AnimationFrame(53, 36))// 3타 31프레임(돌려베기 직전)
				{
					m_bCollision = true;
					CPlayer_Weapon::Set_TrailRenderOnOff(true);
					pPlayer->Set_Rotate(false);
					pPlayer->Set_AttackMove(false);
				}
				else
				{
					m_bCollision = false;
					CPlayer_Weapon::Set_TrailRenderOnOff(false);
					pPlayer->Set_AttackMove(false);
				}
			}

		}	
		else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK1)
		{
			if (pPlayerMesh->Get_AnimationFrame(30, 23)/* && !pPlayerMesh->Get_AnimationFrame(53, 28)*/)// 3타 15프레임(찌르기 직전)
			{

				m_bCollision = true;
				CPlayer_Weapon::Set_TrailRenderOnOff(true);
				pPlayer->Set_Rotate(false);
				pPlayer->Set_AttackMove(true);
			}
			else
			{
				m_bCollision = false;
				CPlayer_Weapon::Set_TrailRenderOnOff(false);
				pPlayer->Set_Rotate(false);
				pPlayer->Set_AttackMove(false);
			}
		}
		else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK2)
		{
			if (pPlayerMesh->Get_AnimationFrame(44, 24) && !pPlayerMesh->Get_AnimationFrame(44, 30))// 3타 15프레임(찌르기 직전)
			{
				m_bCollision = true;
				CPlayer_Weapon::Set_TrailRenderOnOff(true);
				pPlayer->Set_Rotate(false);
			}
			else
			{
				m_bCollision = false;
				CPlayer_Weapon::Set_TrailRenderOnOff(false); 
				pPlayer->Set_Rotate(false);
			}
		}
		else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_CHARGING_ATTACK)
		{
			if (pPlayerMesh->Get_AnimationFrame(92, 0) && !pPlayerMesh->Get_AnimationFrame(92, 33))
				pPlayer->Set_Rotate(true);
			if (pPlayerMesh->Get_AnimationFrame(92, 33) && !pPlayerMesh->Get_AnimationFrame(92, 63))// 3타 15프레임(찌르기 직전)
			{
				if(!m_bCollision)
					m_bCollision = true;
				else if(m_bCollision)
					m_bCollision = false;
				CPlayer_Weapon::Set_TrailRenderOnOff(true);
				pPlayer->Set_Rotate(false);
				pPlayer->Set_AttackMove(false);
			}
			else
			{
				m_bCollision = false;
				CPlayer_Weapon::Set_TrailRenderOnOff(false);
				pPlayer->Set_AttackMove(false);
			}
		}
		else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::MOON_ATTACK1)
		{
		/*	if (pPlayerMesh->Get_AnimationFrame(92, 0) && !pPlayerMesh->Get_AnimationFrame(92, 33))
				pPlayer->Set_Rotate(true);*/
			if (pPlayerMesh->Get_AnimationFrame(92, 33) && !pPlayerMesh->Get_AnimationFrame(92, 63))// 3타 15프레임(찌르기 직전)
			{
				m_bCollision = true;
				CPlayer_Weapon::Set_TrailRenderOnOff(true);
				pPlayer->Set_Rotate(false);
				pPlayer->Set_AttackMove(false);
			}
			else
			{
				m_bCollision = false;
				CPlayer_Weapon::Set_TrailRenderOnOff(false);
				pPlayer->Set_AttackMove(false);
			}
		}
		else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::MOON_ATTACK2)
		{
			if (pPlayerMesh->Get_AnimationFrame(92, 33) && !pPlayerMesh->Get_AnimationFrame(92, 63))// 3타 15프레임(찌르기 직전)
			{
				m_bCollision = true;
				CPlayer_Weapon::Set_TrailRenderOnOff(true); 
				pPlayer->Set_Rotate(false);
				pPlayer->Set_AttackMove(false);
			}
			else
			{
				m_bCollision = false;
				CPlayer_Weapon::Set_TrailRenderOnOff(false);
				pPlayer->Set_AttackMove(false);
			}
		}
		else
		{
			m_bCollision = false;
			CPlayer_Weapon::Set_TrailRenderOnOff(false);
			pPlayer->Set_Rotate(true);
			pPlayer->Set_AttackMove(false);
		}
	}
	else
	{
		m_bCollision = false;
		CPlayer_Weapon::Set_TrailRenderOnOff(false);
		pPlayer->Set_Rotate(true);
		pPlayer->Set_AttackMove(false);
	}

	void* Data;
	pManagement->GetData_Trigger(L"PlayerWeapon", &Data);
	MyPublicData* WeaponData = (MyPublicData*)Data;
	WeaponData->Collision = m_bCollision;
	pManagement->SetData_Trigger(L"PlayerWeapon", WeaponData);

	if (!m_bCollision)
		pManagement->AllMonsterCollision(m_bCollision);
}

void CPlayer_Weapon::Update_Dsv(const _float & _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Player"));

	/*if (pPlayer->Get_bAttack())
	m_bOnHand = true;
	else if (!pPlayer->Get_bAttack())
	m_bOnHand = false;*/

	if (!m_bOnMoon_Dsv && !m_bOnNormal_Dsv && !m_bOnStand_Dsv)
		m_fDsvTime = 0.f;
	if (!m_bOnMoon_ReverseDsv && !m_bOnNormal_ReverseDsv)
		m_fReDsvTime = 0.f;

	//if (GET_MANAGEMENT->Key_Down(DIK_LBRACKET))
	//	m_bOnNormal_Dsv = !m_bOnNormal_Dsv;

	//if (GET_MANAGEMENT->Key_Down(DIK_RBRACKET))
	//	m_bOnNormal_ReverseDsv = !m_bOnNormal_ReverseDsv;

	if (pPlayer->Get_bAttack() && !m_bOnHand)
	{
		if (m_bTake_Normal && !m_bOnNormal_Dsv)
		{
			m_fDsvTime = 0.f;
			m_bOnNormal_Dsv = true;
			m_bOnNormal_ReverseDsv = false;
		}
		else if (m_bTake_Moon&& !m_bOnMoon_Dsv)
		{
			m_fDsvTime = 0.f;
			m_bOnMoon_Dsv = true;
			m_bOnMoon_ReverseDsv = false;
		}
	}
	else if (!pPlayer->Get_bAttack() && m_bOnHand)
	{
		if (m_bTake_Normal &&!m_bOnNormal_Dsv)
		{
			m_fDsvTime = 0.f;
			m_fDsvTimer = 0.f;
			m_bOnNormal_Dsv = true;
			m_bOnNormal_ReverseDsv = false;
		}
		else if (m_bTake_Moon &&  !m_bOnMoon_Dsv)
		{
			m_fDsvTime = 0.f;
			m_fDsvTimer = 0.f;
			m_bOnMoon_Dsv = true;
			m_bOnMoon_ReverseDsv = false;
		}
	}

	if ((m_bOnMoon_Dsv || m_bOnNormal_Dsv) && m_fDsvTime >= 1.f)
	{
		if (!m_bOnHand && pPlayer->Get_bAttack())
		{
			m_fDsvTime = 0.f;
			m_bOnHand = true;
			if (m_bOnMoon_Dsv && !m_bOnMoon_ReverseDsv)
			{
				m_bOnMoon_Dsv = false;
				m_bOnMoon_ReverseDsv = true;
			}
			else if (m_bOnNormal_Dsv && !m_bOnNormal_ReverseDsv)
			{
				m_bOnNormal_Dsv = false;
				m_bOnNormal_ReverseDsv = true;
			}
		}
		else if (m_bOnHand && !pPlayer->Get_bAttack())
		{
			m_fDsvTime = 0.f;
			m_bOnHand = false;
			if (m_bOnMoon_Dsv && !m_bOnMoon_ReverseDsv)
			{
				m_bOnMoon_Dsv = false;
				m_bOnMoon_ReverseDsv = true;
			}
			else if (m_bOnNormal_Dsv && !m_bOnNormal_ReverseDsv)
			{
				m_bOnNormal_Dsv = false;
				m_bOnNormal_ReverseDsv = true;
			}
		}
	}
	if ((m_bOnNormal_ReverseDsv || m_bOnMoon_ReverseDsv) && m_fReDsvTime >= 1.f)
	{
		if (!m_bOnHand && pPlayer->Get_bAttack())
		{
			m_fReDsvTime = 0.f;
			m_bOnHand = true;
			if (m_bOnMoon_Dsv && !m_bOnMoon_ReverseDsv)
			{
				m_bOnMoon_Dsv = false;
				m_bOnMoon_ReverseDsv = true;
			}
			else if (m_bOnNormal_Dsv && !m_bOnNormal_ReverseDsv)
			{
				m_bOnNormal_Dsv = false;
				m_bOnNormal_ReverseDsv = true;
			}
		}
		else if (m_bOnHand && !pPlayer->Get_bAttack())
		{
			m_fDsvTime = 0.f;
			m_bOnHand = false;
			if (m_bOnMoon_Dsv && !m_bOnMoon_ReverseDsv)
			{
				m_bOnMoon_Dsv = false;
				m_bOnMoon_ReverseDsv = true;
			}
			else if (m_bOnNormal_Dsv && !m_bOnNormal_ReverseDsv)
			{
				m_bOnNormal_Dsv = false;
				m_bOnNormal_ReverseDsv = true;
			}
		}
	}


	if (m_bOnStand_Dsv)
		m_fDsvTime += _fDeltaTime * powf(1.5f, m_fDsvTime);

	if (m_bOnNormal_Dsv || m_bOnMoon_Dsv)
		m_fDsvTime += (_fDeltaTime * powf(40.f, m_fDsvTime));

	if (m_bOnMoon_ReverseDsv || m_bOnNormal_ReverseDsv)
		m_fReDsvTime += (_fDeltaTime * powf(40.f, m_fReDsvTime));

}

void CPlayer_Weapon::HitPosToEffect()
{
	
	auto pManagement = GET_MANAGEMENT;

	void* Data;
	pManagement->GetData_Trigger(L"PlayerWeapon", &Data);
	MyPublicData* WeaponData = (MyPublicData*)Data;	

	if (!WeaponData->Collision)
	{
		return;
	}
	else
	{
		const _float4x4 matWorld = *m_pTransform->Get_WorldMatrix();
		_float3 vScale;
		_float4Q Quat;
		_float3 vPos;
		D3DXMatrixDecompose(&vScale, &Quat, &vPos, &matWorld);

		_float3 vRaycast = _float3(0.f, 50.f, 0.f);
		D3DXVec3TransformCoord(&vRaycast, &vRaycast, &matWorld);

		_float3 vNormal = vRaycast - vPos;
		D3DXVec3Normalize(&vNormal, &vNormal);

		PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
		transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);

		MyRayCast* RayCast = pManagement->RayCast_Update(L"PlayerWeapon", transform, vNormal, 2.5f);

		if (RayCast)
		{
			if (RayCast->vPos == _float3(0.f, 0.f, 0.f))
				RayCast->vPos = vRaycast;

			Generate_Particle(RayCast, Quat);
			//tDesc.vScale = _float3(2.5f, 2.5f, 2.5f);
			//Get_Manager()->Generate_Decal(m_iSceneIdx, tDesc, _float4(1.f, 1.f, 1.f, 1.f), 8);
			CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
			pCamera->CamShakerWeak();
			//dynamic_cast<CTrailEffect*>(m_pTrailEffect)->Set_RenderOnOff(false);
		}
	}
}

void CPlayer_Weapon::Generate_Particle(MyRayCast* _pRayCast, const _float4Q& _qQuat)
{
	_float3 vPos = _pRayCast->vPos;
	_float3 vRotate = { 0.f,0.f, 0.f };
	_float3 vNormal = _pRayCast->vNormal;
	_float4Q Quat = _qQuat;

	/*Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 1.f, nullptr, 4,
		_float4(1.f, 0.364705f, 0.188235f, 1.f), (ePartilcePatternType)((_uint)ePartilcePatternType::OnStartBlink | (_uint)ePartilcePatternType::GoUp), 0, 2.2f, D3DX_PI, 3.f);
	Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 1.f, nullptr, 4,
		_float4(1.f, 0.364705f, 0.188235f, 1.f), (ePartilcePatternType)((_uint)ePartilcePatternType::OnStartBlink | (_uint)ePartilcePatternType::GoUp), 3, 2.2f, D3DX_PI, 1.f);
	Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 1.f, nullptr, 4,
		_float4(1.f, 0.364705f, 0.188235f, 1.f), (ePartilcePatternType)((_uint)ePartilcePatternType::OnStartBlink | (_uint)ePartilcePatternType::GoUp), 4, 2.2f, D3DX_PI, 1.f);
	Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 1.f, nullptr, 4,
		_float4(1.f, 0.364705f, 0.188235f, 1.f), (ePartilcePatternType)((_uint)ePartilcePatternType::OnStartBlink | (_uint)ePartilcePatternType::GoUp), 5, 2.2f, D3DX_PI, 1.f);*/
	//Get_Manager()->Generate_ItemParticle(m_iSceneIdx, _pRayCast->vPos, 1.f, 50,
	//	_float4(1.f, 0.364705f, 0.188235f, 1.f)
	//	, (ePartilcePatternType)((_ulong)ePartilcePatternType::GoStraight
	//		| (_ulong)ePartilcePatternType::Gravity),2,5.f);
	//Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 1.f, nullptr, 50,
	//	_float4(1.f, 0.364705f, 0.188235f, 1.f), (ePartilcePatternType)((_uint)ePartilcePatternType::OnStartBlink | (_uint)ePartilcePatternType::Gravity | (_uint)ePartilcePatternType::GoStraight), 9, 2.2f, D3DX_PI, 1.f);

	TRANSFORM_DESC tDesc;
	tDesc.vScale = _float3(0.75f, 0.75f, 0.75f);
	tDesc.vPos = _pRayCast->vPos;
	tDesc.vRotate = m_pTransform->Get_TransformDesc().vRotate;
	vRotate.z = D3DXToRadian(Mersen_ToFloat(85.f, 95.f));
	vRotate.x = D3DXToRadian(Mersen_ToFloat(85.f, 95.f));
	_float4Q qRotate;
	D3DXQuaternionRotationYawPitchRoll(
		&qRotate,
		vRotate.y,
		vRotate.x,
		vRotate.z);
	Quat += qRotate;

	tDesc.vRotate.x = Quat.x / sqrtf(fabs(1.f - powf(Quat.w, 2.f)));
	tDesc.vRotate.y = Quat.y / sqrtf(fabs(1.f - powf(Quat.w, 2.f)));
	tDesc.vRotate.z = Quat.z / sqrtf(fabs(1.f - powf(Quat.w, 2.f)));
	_float3 vRotateFirst = tDesc.vRotate;
	tDesc.fSpeedPerSecond = 3.f;
	Get_Manager()->Generate_Decal(m_iSceneIdx, tDesc, _float4(1.f, 93 / 255.f, 48 / 255.f, 1.f), Mersen_ToInt(11, 14)/*13*/);
	_float3 vRad = vRotateFirst;
	vRad.x = Mersen_ToFloat(vRotateFirst.x - D3DXToRadian(5.f), vRotateFirst.x + D3DXToRadian(5.f));
	vRad.y = Mersen_ToFloat(vRotateFirst.y - D3DXToRadian(5.f), vRotateFirst.y + D3DXToRadian(5.f));
	vRad.z = Mersen_ToFloat(vRotateFirst.z - D3DXToRadian(5.f), vRotateFirst.z + D3DXToRadian(5.f));
	/*	vRad.y = 0.f;*/
/*		Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
			_float4(1.f, 0.364705f, 0.188235f, 1.f),
			(ePartilcePatternType)(
			(_uint)ePartilcePatternType::OnStartBlink
				| (_ulong)ePartilcePatternType::x3
				| (_ulong)ePartilcePatternType::GoUp), 0, 2.2f, vRotateFirst.z, 2.f, EAxis::Z);*/

	Get_Manager()->Generate_SetRadian_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
		_float4(1.f, 0.364705f, 0.188235f, 1.f),
		(ePartilcePatternType)(
		(_uint)ePartilcePatternType::OnStartBlink
			| (_ulong)ePartilcePatternType::x3 | (_ulong)ePartilcePatternType::x4
			| (_ulong)ePartilcePatternType::GoStraight), 0, 0.5f, vRad, 10.f);
	if (m_bTake_Moon)
	{
		//달검 전용 추가 이펙트

		//_float4x4 matView;
		//_float4Q qBill;
		//m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		//D3DXMatrixInverse(&matView, NULL, &matView);
		//D3DXQuaternionRotationMatrix(&qBill, &matView);
		//tDesc.vRotate.x = qBill.x / sqrtf(1.f - powf(qBill.w, 2.f));
		//tDesc.vRotate.y = qBill.y / sqrtf(1.f - powf(qBill.w, 2.f));
		//tDesc.vRotate.z = qBill.z / sqrtf(1.f - powf(qBill.w, 2.f));
		//tDesc.vRotate.z += _qQuat.z / sqrtf(1.f - powf(_qQuat.w, 2.f));

		//Get_Manager()->Generate_Decal(m_iSceneIdx, tDesc, _float4(1.f, 93 / 255.f, 48 / 255.f, 1.f), 26);

		_float4x4 matView;
		_float4Q qBill;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, NULL, &matView);
		D3DXQuaternionRotationMatrix(&qBill, &matView);
		tDesc.vRotate.x = qBill.x / sqrtf(1.f - powf(qBill.w, 2.f));
		tDesc.vRotate.y = qBill.y / sqrtf(1.f - powf(qBill.w, 2.f));
		tDesc.vRotate.z = qBill.z / sqrtf(1.f - powf(qBill.w, 2.f));
		_float qQuatz = _qQuat.z / sqrtf(1.f - powf(_qQuat.w, 2.f));
		tDesc.vRotate.z += Mersen_ToFloat(qQuatz - D3DXToRadian(5.f), qQuatz + D3DXToRadian(5.f));
		//tDesc.vRotate.z += D3DXToRadian(Mersen_ToFloat(44.f, 46.f));
		tDesc.vScale = { 10.f,2.5f,1.f };
		if (_pRayCast->TypeID != RayCastID::Terrain)
		{
		Get_Manager()->Generate_Decal(m_iSceneIdx,
			tDesc, _float4(0.127272f, 0.141176f, 0.521568f, 1.f), 26,
			(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
				| (_ulong)ePartilcePatternType::x3)));


		}
		//else if (_pRayCast->TypeID == RayCastID::Terrain)
		//{
		//	_float3 vRad = vRotateFirst;
		//	vRad.x = Mersen_ToFloat(vRotateFirst.x - D3DXToRadian(5.f), vRotateFirst.x + D3DXToRadian(5.f));
		//	vRad.y = Mersen_ToFloat(vRotateFirst.y - D3DXToRadian(5.f), vRotateFirst.y + D3DXToRadian(5.f));
		//	vRad.z = Mersen_ToFloat(vRotateFirst.z - D3DXToRadian(5.f), vRotateFirst.z + D3DXToRadian(5.f));
		//	/*	vRad.y = 0.f;*/
		///*		Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
		//			_float4(1.f, 0.364705f, 0.188235f, 1.f),
		//			(ePartilcePatternType)(
		//			(_uint)ePartilcePatternType::OnStartBlink
		//				| (_ulong)ePartilcePatternType::x3
		//				| (_ulong)ePartilcePatternType::GoUp), 0, 2.2f, vRotateFirst.z, 2.f, EAxis::Z);*/

		//	Get_Manager()->Generate_SetRadian_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
		//		_float4(1.f, 0.364705f, 0.188235f, 1.f),
		//		(ePartilcePatternType)(
		//		(_uint)ePartilcePatternType::OnStartBlink
		//			| (_ulong)ePartilcePatternType::x3 | (_ulong)ePartilcePatternType::x4
		//			| (_ulong)ePartilcePatternType::GoStraight), 0, 0.5f, vRad, 10.f);

		//}
	}
	else//초반 무기 전용 이펙트
	{

		if(_pRayCast->TypeID != RayCastID::Terrain)
		{
			//-----------------------강타 쇼크 웨이브 ----------------------------//
			TRANSFORM_DESC tImpactDecalDesc;
			tImpactDecalDesc.vScale = _float3(4.f, 4.f, 4.f);
			tImpactDecalDesc.vPos = _pRayCast->vPos;
			tImpactDecalDesc.vPos.y += 0.3f;
			tImpactDecalDesc.vRotate = { D3DXToRadian(90.f),0.f,0.f };


			CDynamicMesh* pPlayerMesh = (CDynamicMesh*)GET_MANAGEMENT->Get_Mesh(m_iSceneIdx, L"Layer_Player", L"Mesh_Mesh");
			if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK1)
			{
				Get_Manager()->Generate_Decal(m_iSceneIdx,
					tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 16,
					(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::RotateY
						| (_ulong)ePartilcePatternType::ScaleUp
						| (_ulong)ePartilcePatternType::x3)));

				tImpactDecalDesc.vScale *= 2.f;

				Get_Manager()->Generate_Decal(m_iSceneIdx,
					tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 19,
					(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::ScaleUp
						| (_ulong)ePartilcePatternType::x3)));

				tImpactDecalDesc.vScale = _float3(1.f, 4.f, 1.f);
				tImpactDecalDesc.vPos.y += 0.5f;
				Get_Manager()->Generate_Decal(m_iSceneIdx,
					tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 21,
					(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::YBillBoard
						| (_ulong)ePartilcePatternType::x3)));

				//tImpactDecalDesc.vRotate.x = Mersen_ToFloat(vRotateFirst.x - D3DXToRadian(5.f), vRotateFirst.x + D3DXToRadian(5.f));
				tImpactDecalDesc.vRotate.y = Mersen_ToFloat(vRotateFirst.y - D3DXToRadian(5.f), vRotateFirst.y + D3DXToRadian(5.f));
				tImpactDecalDesc.vRotate.x = 0.f;
				tImpactDecalDesc.vRotate.z = 0.f;
				//tImpactDecalDesc.vRotate.z = Mersen_ToFloat(vRotateFirst.z - D3DXToRadian(5.f), vRotateFirst.z + D3DXToRadian(5.f));
				Get_Manager()->Generate_SetRadian_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
					_float4(1.f, 0.364705f, 0.188235f, 1.f),
					(ePartilcePatternType)(
					(_uint)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::x3
						| (_ulong)ePartilcePatternType::Gravity
						| (_ulong)ePartilcePatternType::GoStraight), 0, 1.3f, tImpactDecalDesc.vRotate, 4.f);

			}
			else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK2)
			{
				
				Get_Manager()->Generate_Decal(m_iSceneIdx,
					tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 11,
					(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::ScaleUp
						| (_ulong)ePartilcePatternType::x3)));

				tImpactDecalDesc.vScale *= 2.f;

				Get_Manager()->Generate_Decal(m_iSceneIdx,
					tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 18,
					(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::ScaleUp
						| (_ulong)ePartilcePatternType::RotateY
						| (_ulong)ePartilcePatternType::x3)));

				tImpactDecalDesc.vScale = _float3(1.f, 4.f, 1.f);
				tImpactDecalDesc.vPos.y += 2.f;
				Get_Manager()->Generate_Decal(m_iSceneIdx,
					tImpactDecalDesc, _float4(0.674509f, 0.572549f, 0.929411f, 1.f), 24,
					(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::YBillBoard
						| (_ulong)ePartilcePatternType::x4)), 1.3f);

				//tImpactDecalDesc.vRotate.x = Mersen_ToFloat(vRotateFirst.x - D3DXToRadian(5.f), vRotateFirst.x + D3DXToRadian(5.f));
				tImpactDecalDesc.vRotate.y = Mersen_ToFloat(vRotateFirst.y - D3DXToRadian(5.f), vRotateFirst.y + D3DXToRadian(5.f));
				tImpactDecalDesc.vRotate.x = 0.f;
				tImpactDecalDesc.vRotate.z = 0.f;
				//tImpactDecalDesc.vRotate.z = Mersen_ToFloat(vRotateFirst.z - D3DXToRadian(5.f), vRotateFirst.z + D3DXToRadian(5.f));
				Get_Manager()->Generate_SetRadian_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
					_float4(1.f, 0.364705f, 0.188235f, 1.f),
					(ePartilcePatternType)(
					(_uint)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::x3 
						| (_ulong)ePartilcePatternType::Gravity
						| (_ulong)ePartilcePatternType::GoStraight), 0, 1.3f, tImpactDecalDesc.vRotate, 4.f);
			}
			//-----------------------강타 쇼크 웨이브 ----------------------------//
			else
			{
				_float4x4 matView;
				_float4Q qBill;
				m_pDevice->GetTransform(D3DTS_VIEW, &matView);
				D3DXMatrixInverse(&matView, NULL, &matView);
				D3DXQuaternionRotationMatrix(&qBill, &matView);
				tDesc.vRotate.x = qBill.x / sqrtf(1.f - powf(qBill.w, 2.f));
				tDesc.vRotate.y = qBill.y / sqrtf(1.f - powf(qBill.w, 2.f));
				tDesc.vRotate.z = qBill.z / sqrtf(1.f - powf(qBill.w, 2.f));
				_float qQuatz = _qQuat.z / sqrtf(1.f - powf(_qQuat.w, 2.f));
				tDesc.vRotate.z += Mersen_ToFloat(qQuatz - D3DXToRadian(5.f), qQuatz + D3DXToRadian(5.f));
				//tDesc.vRotate.z += D3DXToRadian(Mersen_ToFloat(44.f, 46.f));
				tDesc.vScale = { 20.f,5.f,1.f };

				Get_Manager()->Generate_Decal(m_iSceneIdx,
					tDesc, _float4(1.f, 93 / 255.f, 48 / 255.f, 1.f), 26,
					(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::x3)));
				if (_pRayCast->TypeID == RayCastID::Monster)
				{
					//Get_Manager()->Generate_Decal()
					//Get_Manager()->Ge
					_float3 vRad = vRotateFirst;
					vRad.x = Mersen_ToFloat(vRotateFirst.x - D3DXToRadian(5.f), vRotateFirst.x + D3DXToRadian(5.f));
					vRad.y = Mersen_ToFloat(vRotateFirst.y - D3DXToRadian(5.f), vRotateFirst.y + D3DXToRadian(5.f));
					vRad.z = Mersen_ToFloat(vRotateFirst.z - D3DXToRadian(5.f), vRotateFirst.z + D3DXToRadian(5.f));
					/*	vRad.y = 0.f;*/
				/*		Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
							_float4(1.f, 0.364705f, 0.188235f, 1.f),
							(ePartilcePatternType)(
							(_uint)ePartilcePatternType::OnStartBlink
								| (_ulong)ePartilcePatternType::x3
								| (_ulong)ePartilcePatternType::GoUp), 0, 2.2f, vRotateFirst.z, 2.f, EAxis::Z);*/

					Get_Manager()->Generate_SetRadian_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
						_float4(1.f, 0.364705f, 0.188235f, 1.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3 | (_ulong)ePartilcePatternType::x4
							| (_ulong)ePartilcePatternType::GoStraight), 0, 0.5f, vRad, 10.f);
				}	
				if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_CHARGING_ATTACK)
				{
					Get_Manager()->Generate_SphereParticle(m_iSceneIdx, _pRayCast->vPos, 1.f, &_pRayCast->vPos, 40, _float4(1.f,1.f,1.f,3.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3 | (_ulong)ePartilcePatternType::x4
							| (_ulong)ePartilcePatternType::RevRotX
							| (_ulong)ePartilcePatternType::GoStraight), 27, 1.6f, D3DX_PI*2.f, 1.f);
					if (pPlayerMesh->Get_AnimationFrame(92, 63))
					{
						Get_Manager()->Generate_SphereParticle(m_iSceneIdx, _pRayCast->vPos, 1.f, &_pRayCast->vPos, 250,
							_float4(1.f, 1.f, 1.f, 1.f),
							(ePartilcePatternType)(
							(_uint)ePartilcePatternType::OnStartBlink
								| (_ulong)ePartilcePatternType::x3
								| (_ulong)ePartilcePatternType::RevRotY
								| (_ulong)ePartilcePatternType::GoStraight), 27, 1.6f, D3DX_PI*2.f, 2.f);
					}
				}

			}


		}
		else if (_pRayCast->TypeID == RayCastID::Terrain)		//지형 충돌 시
		{
	
			//-----------------------강타 쇼크 웨이브 ----------------------------//
			TRANSFORM_DESC tImpactDecalDesc;
			tImpactDecalDesc.vScale = _float3(4.f, 4.f, 4.f);
			tImpactDecalDesc.vPos = _pRayCast->vPos;
			tImpactDecalDesc.vPos.y += 0.3f;
			tImpactDecalDesc.vRotate = { D3DXToRadian(90.f),0.f,0.f };


			CDynamicMesh* pPlayerMesh = (CDynamicMesh*)GET_MANAGEMENT->Get_Mesh(m_iSceneIdx, L"Layer_Player", L"Mesh_Mesh");
			if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK1)
			{
				if (pPlayerMesh->Get_AnimationFrame(30, 26))
				{
					Get_Manager()->Generate_Decal(m_iSceneIdx,
						tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 16,
						(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::RotateY
							| (_ulong)ePartilcePatternType::ScaleUp
							| (_ulong)ePartilcePatternType::x3)));

					tImpactDecalDesc.vScale *= 2.f;

					Get_Manager()->Generate_Decal(m_iSceneIdx,
						tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 19,
						(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::ScaleUp
							| (_ulong)ePartilcePatternType::x3)));

					tImpactDecalDesc.vScale = _float3(1.f, 4.f, 1.f);
					tImpactDecalDesc.vPos.y += 0.5f;
					Get_Manager()->Generate_Decal(m_iSceneIdx,
						tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 21,
						(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::YBillBoard
							| (_ulong)ePartilcePatternType::x3)));

					//tImpactDecalDesc.vRotate.x = Mersen_ToFloat(vRotateFirst.x - D3DXToRadian(5.f), vRotateFirst.x + D3DXToRadian(5.f));
					tImpactDecalDesc.vRotate.y = Mersen_ToFloat(vRotateFirst.y - D3DXToRadian(5.f), vRotateFirst.y + D3DXToRadian(5.f));
					tImpactDecalDesc.vRotate.x = 0.f;
					tImpactDecalDesc.vRotate.z = 0.f;
					//tImpactDecalDesc.vRotate.z = Mersen_ToFloat(vRotateFirst.z - D3DXToRadian(5.f), vRotateFirst.z + D3DXToRadian(5.f));
					Get_Manager()->Generate_SetRadian_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
						_float4(1.f, 0.364705f, 0.188235f, 1.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3
							| (_ulong)ePartilcePatternType::Gravity
							| (_ulong)ePartilcePatternType::GoStraight), 0, 1.3f, tImpactDecalDesc.vRotate, 4.f);
				}
			}
			else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK2)
			{
				if (pPlayerMesh->Get_AnimationFrame(44, 27))
				{
					Get_Manager()->Generate_Decal(m_iSceneIdx,
						tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 11,
						(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::ScaleUp
							| (_ulong)ePartilcePatternType::x3)));

					tImpactDecalDesc.vScale *= 2.f;

					Get_Manager()->Generate_Decal(m_iSceneIdx,
						tImpactDecalDesc, _float4(0.25f, 0.25f, 0.25f, 1.f), 18,
						(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::ScaleUp
							| (_ulong)ePartilcePatternType::RotateY
							| (_ulong)ePartilcePatternType::x3)));

					tImpactDecalDesc.vScale = _float3(1.f, 4.f, 1.f);
					tImpactDecalDesc.vPos.y += 2.f;
					Get_Manager()->Generate_Decal(m_iSceneIdx,
						tImpactDecalDesc, _float4(0.674509f, 0.572549f, 0.929411f, 1.f), 24,
						(ePartilcePatternType((_ulong)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::YBillBoard
							| (_ulong)ePartilcePatternType::x4)), 1.3f);


					//tImpactDecalDesc.vRotate.x = Mersen_ToFloat(vRotateFirst.x - D3DXToRadian(5.f), vRotateFirst.x + D3DXToRadian(5.f));
					tImpactDecalDesc.vRotate.y = Mersen_ToFloat(vRotateFirst.y - D3DXToRadian(5.f), vRotateFirst.y + D3DXToRadian(5.f));
					tImpactDecalDesc.vRotate.x = 0.f;
					tImpactDecalDesc.vRotate.z = 0.f;
					//tImpactDecalDesc.vRotate.z = Mersen_ToFloat(vRotateFirst.z - D3DXToRadian(5.f), vRotateFirst.z + D3DXToRadian(5.f));
					Get_Manager()->Generate_SetRadian_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
						_float4(1.f, 0.364705f, 0.188235f, 1.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3 
							| (_ulong)ePartilcePatternType::Gravity
							| (_ulong)ePartilcePatternType::GoStraight), 0, 1.3f, tImpactDecalDesc.vRotate, 4.f);

				}
			}
			else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_CHARGING_ATTACK)
			{
				if (pPlayerMesh->Get_AnimationFrame(92, 63))
				{
					Get_Manager()->Generate_SphereParticle(m_iSceneIdx, _pRayCast->vPos, 1.f, &_pRayCast->vPos, 250,
						_float4(1.f, 0.364705f, 0.188235f, 3.f),
						(ePartilcePatternType)(
						(_uint)ePartilcePatternType::OnStartBlink
							| (_ulong)ePartilcePatternType::x3
							| (_ulong)ePartilcePatternType::RevRotY
							| (_ulong)ePartilcePatternType::GoStraight), 27, 1.6f, D3DX_PI*2.f, 2.f);
				}
			}
			//-----------------------강타 쇼크 웨이브 ----------------------------//
			_float3 vRad = vRotateFirst;
			vRad.x = Mersen_ToFloat(vRotateFirst.x - D3DXToRadian(5.f), vRotateFirst.x + D3DXToRadian(5.f));
			vRad.y = Mersen_ToFloat(vRotateFirst.y - D3DXToRadian(5.f), vRotateFirst.y + D3DXToRadian(5.f));
			vRad.z = Mersen_ToFloat(vRotateFirst.z - D3DXToRadian(5.f), vRotateFirst.z + D3DXToRadian(5.f));
			/*	vRad.y = 0.f;*/
		/*		Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
					_float4(1.f, 0.364705f, 0.188235f, 1.f),
					(ePartilcePatternType)(
					(_uint)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::x3
						| (_ulong)ePartilcePatternType::GoUp), 0, 2.2f, vRotateFirst.z, 2.f, EAxis::Z);*/

			Get_Manager()->Generate_SetRadian_ExplosionParticle(m_iSceneIdx, _pRayCast->vPos, 0.f, nullptr, 200,
				_float4(1.f, 0.364705f, 0.188235f, 1.f),
				(ePartilcePatternType)(
				(_uint)ePartilcePatternType::OnStartBlink
					| (_ulong)ePartilcePatternType::x3 | (_ulong)ePartilcePatternType::x4
					| (_ulong)ePartilcePatternType::GoStraight), 0, 0.5f, vRad, 10.f);

		}
		
	}
}

void CPlayer_Weapon::CheckTrailEffect()
{
	if (m_bTake_Normal && m_pTrailEffect)
	{
		m_pTrailEffect->Set_Color(_float3(0.25f, 0.25f, 0.25f));
		m_pTrailEffect->Set_RenderID(7);
		m_pTrailEffect->Set_Width(180.f);
		m_pTrailEffect->Set_TrailRateTime(0.005f);
		m_pTrailEffectSecond->Set_TrailRateTime(0.005f);
		//m_pTrailEffectDistortion->Set_Pass(8);
		m_pTrailEffectDistortion->Set_RenderID(5);
		if (m_pTrailEffectSecond)
		{
			m_pTrailEffectSecond->Set_Width(25.f);
		}
		CDynamicMesh* pPlayerMesh = (CDynamicMesh*)Get_Manager()->Get_Mesh(m_iSceneIdx, L"Layer_Player", L"Mesh_Mesh");
		
		if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_CHARGING_ATTACK)
		{
			m_pTrailEffectDistortion->Set_Width(500.f);
		}
		else
		{
			m_pTrailEffectDistortion->Set_Width(300.f);
		}

	}
	else if (m_bTake_Moon && m_pTrailEffect)
	{
		m_pTrailEffect->Set_Color(_float3(0.674509f, 0.572549f, 0.929411f));
		//m_pTrailEffect->Set_RenderID(2);
		m_pTrailEffect->Set_RenderID(6);
		m_pTrailEffect->Set_Width(75.f);
		m_pTrailEffectSecond->Set_TrailRateTime(0.001f);
		m_pTrailEffectDistortion->Set_Width(150.f);
		if (m_pTrailEffectSecond)
		{
			m_pTrailEffectSecond->Set_Width(20.f);
		}
		m_pTrailEffectDistortion->Set_RenderID(4);
	}
}

void CPlayer_Weapon::Update_Sound()
{
	CDynamicMesh* pPlayerMesh = (CDynamicMesh*)GET_MANAGEMENT->Get_Mesh(m_iSceneIdx, L"Layer_Player", L"Mesh_Mesh");
	//CPlayer* pPlayer = (CPlayer*)pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player");

	if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK1)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (pPlayerMesh->Get_AnimationFrame(44, 18))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_NormalAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName,CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;

			}
		}
		else
			m_bAttackSound = true;
	}
	else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK2)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (pPlayerMesh->Get_AnimationFrame(34, 11))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_NormalAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;
			}
		}
		else
			m_bAttackSound = true;
	}
	else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK3)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (pPlayerMesh->Get_AnimationFrame(53, 28))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_NormalAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;
			}
		}
		else if (pPlayerMesh->Get_AnimationFrame(53, 20))
			m_bAttackSound = true;
		else if (pPlayerMesh->Get_AnimationFrame(53, 14))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_NormalAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;
			}
		}
		else
			m_bAttackSound = true;
	}
	else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK1)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (pPlayerMesh->Get_AnimationFrame(30, 26))
		{
			if (m_bCollWallSound)
			{
				_uint iCount = Mersen_ToInt(0, 3);
				dynamic_cast<CMainCamera*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Camera"))->CamShakerHeavy();
				_uint iWallCount = Mersen_ToInt(0, 1);
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_HeavyAttack%d.wav", iWallCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Attack_Wall);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Attack_Wall);
				m_bCollWallSound = false;
			}
		}
		else if (pPlayerMesh->Get_AnimationFrame(30, 22))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_NormalAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;
			}
		}
		else
		{
			m_bCollWallSound = true;
			m_bAttackSound = true;
		}
	}
	else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK2)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (pPlayerMesh->Get_AnimationFrame(44, 27))
		{
			if (m_bCollWallSound)
			{
				_uint iCount = Mersen_ToInt(0, 3);
				dynamic_cast<CMainCamera*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Camera"))->CamShakerHeavy();
				_uint iWallCount = Mersen_ToInt(0, 1);
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_HeavyAttack%d.wav", iWallCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Attack_Wall);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Attack_Wall);
				m_bCollWallSound = false;
			}
		}
		if (pPlayerMesh->Get_AnimationFrame(44, 24))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_NormalAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;
			}
		}
		else
		{
			m_bCollWallSound = true;
			m_bAttackSound = true;
		}
	}
	else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_CHARGING_ATTACK)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (pPlayerMesh->Get_AnimationFrame(92, 63))
		{
			if (m_bCollWallSound)
			{
				dynamic_cast<CMainCamera*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Camera"))->CamShakerHeavy();
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Attack_Wall);
				GET_MANAGEMENT->PlaySounds(L"Player_Charge_Wall.wav", CSound_Manager::ESoundID::Player_Attack_Wall);
				m_bCollWallSound = false;
			}
		}
		else if (pPlayerMesh->Get_AnimationFrame(92, 59))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_NormalAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;
			}
		}
		else if (pPlayerMesh->Get_AnimationFrame(92, 50))
			m_bAttackSound = true;
		else if (pPlayerMesh->Get_AnimationFrame(92, 37))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_NormalAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;
			}
		}
		else
		{
			m_bCollWallSound = true;
			m_bAttackSound = true;
		}
	}
	else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::MOON_ATTACK1)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (pPlayerMesh->Get_AnimationFrame(26, 7))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_MoonAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;
			}
		}
		else
			m_bAttackSound = true;
	}
	else if (pPlayerMesh->Get_Animationset() == (_uint)PlayerAniID::MOON_ATTACK2)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (pPlayerMesh->Get_AnimationFrame(13, 4))
		{
			if (m_bAttackSound)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_MoonAttack%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Weapon);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Weapon);
				m_bAttackSound = false;
			}
		}
		else
			m_bAttackSound = true;
	}
	else
	{
		m_bCollWallSound = true;
		m_bAttackSound = true;
	}
}

void CPlayer_Weapon::Set_WeaponLevelUp(WEAPON_STATE _eStat, _int i)
{
	switch (_eStat)
	{
	case CPlayer_Weapon::NORMAL:
		m_tWeaponStat.iNormalLevel += i;
		break;
	case CPlayer_Weapon::MOON:
		m_tWeaponStat.iMoonLevel += i;
		break;
	}
	if (m_tWeaponStat.iNormalLevel > 5)
		m_tWeaponStat.iNormalLevel = 5;
	if (m_tWeaponStat.iMoonLevel > 5)
		m_tWeaponStat.iMoonLevel = 5;
}
