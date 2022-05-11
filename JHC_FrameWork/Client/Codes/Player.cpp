#include "stdafx.h"
#include "Player.h"
#include "TrailEffect.h"

#include "MainCamera.h"
#include "Player_Weapon.h"
#include "UI_Inventory.h"
#include "UI_Equipment.h"
#include "UI_Dead.h"
#include "Player_UI.h"
#include "CinematicCamera_0.h"
#include "MapStaticObject_Elevator.h"
#include "Tutorial.h"
#include "Shrine_Scene.h"
#include "Stage2_Scene.h"

//  플레이어는 건들기 전에 물어봐 주십시오 제발
CPlayer::CPlayer(_DEVICE _pDevice)
	: CGameObject(_pDevice)
	, m_pTransform(nullptr), m_pShader(nullptr), m_pDeadUI(nullptr)
	, m_pMesh(nullptr), m_bTargeting(false)
	, m_bUse_Heal(false), m_bUse_Item(false)
	, m_bJump(false), m_bJumping(false)
	, m_bRoll(false), m_bStart(true)
	, m_ePreState(STATE_END), m_eCurState(SLEEPING)
	, m_fStartTime(0.f), m_fRollTime(0.f)
	, m_iJump(2), m_eItem(ITEM::HEALTH)
	, m_eWeapon(NORMAL), m_bAttack(false), m_bPxAttack(false)
	, m_eAttack(NO_ATTACK), m_iLightAttack(0), m_iHeavyAttack(0)
	, m_bOnWeapon(false), m_pPxShape(nullptr)
	, m_vUp(0.f, 0.f, 0.f), m_vLook(0.f, 0.f, 0.f)
	, m_vRight(0.f, 0.f, 0.f), m_vLU(0.f, 0.f, 0.f)
	, m_vRU(0.f, 0.f, 0.f), m_bGravity(false)
	, m_bDash(false), m_fDashTime(0.f)
	, m_pInterfaceUI(nullptr), m_bInteract(false)
	, m_iDash(1), m_bDashCool(false)
	, m_bFallDeath(false), m_bCheat_Gravity(false)
	, m_vSpawnPos(0.f, 0.f, 0.f), m_bInteract_Stand(false), m_bInteract_StandTrigger(false)
	, m_fBasicHP(0.f), m_fBasicStamina(0.f)
	, m_fStaminaCool(0.f), m_bStaminaCool(false)
	, m_bInteract_Save(false), m_bHit(false)
	, m_bJumpStart(true), m_bDoubleJump(true)
	, m_bGravStart(true), m_fJumpTime(0.f)
	, m_vJumpPos(0.f, 0.f, 0.f), m_bHeal(true), m_bUse(true)
	, m_fHealTime(0.f), m_fHealing(0.f)
	, m_bDashDir(true), m_vDashDir(0.f, 0.f, 0.f)
	, m_bAttackMove(false)
	, m_bLeverOn(false), m_bHeavyShake(true)
	, m_bElevator(false), m_szTargetName(L"")
	, m_vTargetPos(0.f, 0.f, 0.f), m_bPortalGateOn(false)
	, m_bGet_Weapon_Normal(false), m_bGet_Weapon_Moon(false)
	, m_bGod_Mode(false), m_fAniSpeed(0.f)
	, m_bNpcChoronInteract(false)
	, m_bNpcValnirInteract(false)
	, m_bNpcYaakInteract(false)
	, m_bLevelStoneInteract(false)
	, m_bNPCInteracting(false), m_vChargingDir(0.f, 0.f, 0.f)
	, m_fAttackTime(0.f), m_bChargeAttack(false)
	, m_bCharging(false), m_bChargingDir(true)
	, m_bBoneParticle(false)
	, m_fAirTime(0.f)
	, m_fFallDamage(0.f)
	, m_bRotate(true)
	, m_bParry(false), m_bHitSkill(false)
	, m_bDisolve(false)
	, m_fDsvTime(0.f)
	, m_bDashDsv(false)
	, m_bDashReDsv(false)
	, m_fDashDsvTime(0.f)
	, m_fDashReDsvTime(0.f)
	, m_bOnElevater(false)
	, m_bDeadDsv(false)
	, m_fDeadDsvTime(0.f), m_bResetCamera(false)
	, m_bWeaponParry(false)
	//엔딩 시네마틱
	, m_bCinematicTeleport1(false)
	, m_bCinematicTeleport2(false)
	, m_bEndingStart(false)
	, m_fCinematicTimer(0.f)
	// 사운드
	, m_bRollSound(true), m_bLeftLag(true), m_bRightLag(true)
	, m_bJumpSound(true), m_bJumpOnDown(true), m_bDashSound(true)
	, m_bAttackSound(true)
	, m_bUse_Quick_Item(false)
	, m_bBonfireHeal(false)
	, m_iPreScene(0)
	, m_vCameraAt(0.f, 0.f, 0.f), m_vCameraEye(0.f, 0.f, 0.f)
	, m_fPoiseTime(0.f)
{
	m_PrototypeName = L"Player";
}

CPlayer::CPlayer(const CPlayer & _rOther)
	: CGameObject(_rOther)
	, m_pTransform(_rOther.m_pTransform), m_pShader(_rOther.m_pShader), m_pDeadUI(_rOther.m_pDeadUI)
	, m_pMesh(_rOther.m_pMesh), m_bTargeting(_rOther.m_bTargeting)
	, m_bUse_Heal(_rOther.m_bUse_Heal), m_bUse_Item(_rOther.m_bUse_Item)
	, m_bJump(_rOther.m_bJump), m_bJumping(_rOther.m_bJumping)
	, m_bRoll(_rOther.m_bRoll), m_bStart(_rOther.m_bStart)
	, m_ePreState(_rOther.m_ePreState), m_eCurState(_rOther.m_eCurState)
	, m_fStartTime(_rOther.m_fStartTime), m_fRollTime(_rOther.m_fRollTime)
	, m_iJump(_rOther.m_iJump), m_eItem(_rOther.m_eItem)
	, m_eWeapon(_rOther.m_eWeapon), m_bAttack(_rOther.m_bAttack), m_bPxAttack(_rOther.m_bPxAttack)
	, m_eAttack(_rOther.m_eAttack), m_iLightAttack(_rOther.m_iLightAttack), m_iHeavyAttack(_rOther.m_iHeavyAttack)
	, m_bOnWeapon(_rOther.m_bOnWeapon), m_pPxShape(_rOther.m_pPxShape)
	, m_vUp(_rOther.m_vUp), m_vLook(_rOther.m_vLook)
	, m_vRight(_rOther.m_vRight), m_vLU(_rOther.m_vLU)
	, m_vRU(_rOther.m_vRU), m_bGravity(_rOther.m_bGravity)
	, m_bDash(_rOther.m_bDash), m_fDashTime(_rOther.m_fDashTime)
	, m_pInterfaceUI(nullptr), m_bInteract(_rOther.m_bInteract)
	, m_iDash(_rOther.m_iDash), m_bDashCool(_rOther.m_bDashCool)
	, m_bFallDeath(_rOther.m_bFallDeath), m_bCheat_Gravity(_rOther.m_bCheat_Gravity)
	, m_vSpawnPos(_rOther.m_vSpawnPos), m_bInteract_Stand(_rOther.m_bInteract_Stand), m_bInteract_StandTrigger(_rOther.m_bInteract_StandTrigger)
	, m_fBasicHP(_rOther.m_fBasicHP), m_fBasicStamina(_rOther.m_fBasicHP)
	, m_fStaminaCool(_rOther.m_fStaminaCool), m_bStaminaCool(_rOther.m_bStaminaCool)
	, m_bInteract_Save(_rOther.m_bInteract_Save), m_bHit(_rOther.m_bHit)
	, m_bJumpStart(_rOther.m_bJumpStart), m_bDoubleJump(_rOther.m_bDoubleJump)
	, m_bGravStart(_rOther.m_bGravStart), m_fJumpTime(_rOther.m_fJumpTime)
	, m_vJumpPos(_rOther.m_vJumpPos), m_bHeal(_rOther.m_bHeal), m_bUse(_rOther.m_bUse)
	, m_fHealTime(_rOther.m_fHealTime), m_fHealing(_rOther.m_fHealing)
	, m_bDashDir(_rOther.m_bDashDir), m_vDashDir(_rOther.m_vDashDir)
	, m_bAttackMove(false)
	, m_bLeverOn(false), m_vChargingDir(0.f, 0.f, 0.f)
	, m_bElevator(false), m_szTargetName(L"")
	, m_vTargetPos(0.f, 0.f, 0.f), m_bPortalGateOn(false)
	, m_bGet_Weapon_Normal(false), m_bGet_Weapon_Moon(false)
	, m_bGod_Mode(false), m_bHeavyShake(true)
	, m_bNpcChoronInteract(false)
	, m_bLevelStoneInteract(false)
	, m_bNpcValnirInteract(false), m_bChargingDir(true)
	, m_bNpcYaakInteract(false)
	, m_bNPCInteracting(false), m_bChargeAttack(false)
	, m_fAttackTime(0.f), m_bCharging(false)
	, m_bBoneParticle(false), m_bHitSkill(false)
	, m_fAirTime(0.f)
	, m_fFallDamage(0.f)
	, m_bRotate(true)
	, m_bParry(false), m_bResetCamera(false)
	, m_bOnElevater(false)
	, m_bWeaponParry(false)
	//엔딩 시네마틱
	, m_bCinematicTeleport1(false)
	, m_bCinematicTeleport2(false)
	, m_bEndingStart(false)
	, m_fCinematicTimer(0.f)
	, m_bBonfireHeal(false)
	// 디졸브
	, m_bDisolve(false), m_fDsvTime(0.f), m_bDashDsv(false)
	, m_bDashReDsv(false), m_fDashDsvTime(0.f), m_fDashReDsvTime(0.f)
	, m_bDeadDsv(false), m_fDeadDsvTime(0.f)
	// 사운드
	, m_bRollSound(true), m_bLeftLag(true), m_bRightLag(true)
	, m_bJumpSound(true), m_bJumpOnDown(true), m_bDashSound(true)
	, m_bAttackSound(true)
	, m_bUse_Quick_Item(false)
	, m_iPreScene(0)
	, m_fPoiseTime(0.f)
{
	//m_tObjectInfoDesc = _rOther.m_tObjectInfoDesc;
}

HRESULT CPlayer::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
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

	if (FAILED(Add_Component()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	auto pManagement = GET_MANAGEMENT;
	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;

	Set_Init(vPos);
	
	FAILED_CHECK_RETURN(CPlayer::AddUIClass(), E_FAIL);
	FAILED_CHECK_RETURN(CPlayer::AddTrailEffect_Class(), E_FAIL);
	return S_OK;
}

_uint CPlayer::Update_GameObject(const _float & _fDeltaTime)
{
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
	if (m_iPreScene != m_iSceneIdx)
	{
		m_bResetCamera = true;
		m_iPreScene = m_iSceneIdx;
	}
	Cheat(_fDeltaTime);
	Collider();

	if (!m_bDead && !m_bHit && !m_bStart && !m_bDash && !m_bRoll && !m_bUse_Heal && !m_bUse_Item && !m_bInteract && !m_bNPCInteracting && !m_bJump && !m_bGravity && !m_bEndingStart)
	{
		Light_Attack(_fDeltaTime);
		Heavy_Attack(_fDeltaTime);
	}
	if (!m_bParry && !m_bHit && m_ePreState != JUMP_END && !m_bStart &&!m_bInteract && !m_bNPCInteracting && !m_bEndingStart)
		Update_Move(_fDeltaTime);
	if (m_bStart)
		Wake_Up(_fDeltaTime);
	/////////엔딩씬을 위해 추가한 함수
	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_ENDING)
		Ending(_fDeltaTime);
	/////////엔딩씬을 위해 추가한 함수

	Update_Item();
	Heal(_fDeltaTime);
	if (!m_bInteract && !m_bCheat_Gravity)
		Player_Fall(_fDeltaTime);

	if (!m_bChargeAttack && !m_bCharging && !m_bHit && !m_bJump && !m_bGravity && !m_bRoll && !m_bDash && !m_bAttack && !m_bEndingStart)
		Player_Interact(_fDeltaTime);

	Update_AnimationControl();
	if (!m_bStart && !m_bDead)
	{
		if (!(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::JUMP_AIR && m_pMesh->Is_AnimationSetEnd() && (m_bJump || m_bGravity)))
			m_pMesh->Play_Animationset(_fDeltaTime * m_fAniSpeed);
	}
	
	Update_Stat(_fDeltaTime);
	Update_Dead(_fDeltaTime);
	DashTrailUpdateMatrix();

	Print_To_Debug();
	Update_Parry();
	m_pTransform->Update_Transform();

	Update_Dsv(_fDeltaTime);
	Dash_Dsv(_fDeltaTime);

#ifdef _DEBUG
	//if(!m_bFallDeath)
	//m_tState.fHP = m_tState.fMaxHP;
#endif // _DEBUG

	return (_uint)EEvent::None;
}

_uint CPlayer::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	Update_Sound();
	

	return (_uint)EEvent::None;
}

HRESULT CPlayer::Render_GameObject()
{
	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();
	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}
	
	_uint	iPassMax = 0;
	_uint	iBeginPass = 0;
	// 0 기본
	// 1 디졸브
	// 2 역디졸브
	if (m_bDisolve || m_bDashDsv || m_bDeadDsv)
	{
		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Dissolve", pEffect, "g_DissolveTexture");
		iBeginPass = 1;
	}
	if (m_bDashReDsv)
	{
		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Dissolve", pEffect, "g_DissolveTexture",2);
		iBeginPass = 2;
	}
	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(iBeginPass);

	_float4x4 matProj;
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	m_pMesh->Render_Meshes(pEffect, 0, nullptr, true);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

_bool CPlayer::Get_bPxAttack()
{
	if (m_bAttack && m_bPxAttack)
		return true;
	
	return false;
}

void CPlayer::Min_ObstacleType(EObstacle _eType)
{
	MyPlayerData* Data = (MyPlayerData*)Get_Manager()->GetData_Controller(L"Player");
	if (nullptr == Data)
		return;

	if (Data->ObstacleType & (PxU32)_eType)
	{
		m_pPlayerData->ObstacleType ^= (PxU32)_eType;
		Get_Manager()->SetData_Controller(L"Player", m_pPlayerData);
	}
}

void CPlayer::Set_Init(_float3 _vPos)
{
	auto pManagement = GET_MANAGEMENT;
	m_vSpawnPos = _vPos;
	
	pManagement->Add_Controller(L"Player", _vPos, EInteractionType::Capsule, PxCapsuleGeometry(1.f, 1.f));
}

void CPlayer::Update_Parry()
{
	
}

void CPlayer::Create_Parry()
{
	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	_float3 vRotate = m_pTransform->Get_TransformDesc().vRotate;
	_float4Q Quat;
	D3DXQuaternionRotationYawPitchRoll(&Quat, vRotate.y, vRotate.x, vRotate.z);

	_float3 vLook;
	m_pTransform->Get_Info(EInfo::Look, &vLook);
	D3DXVec3Normalize(&vLook, &vLook);
	vPos -= vLook * 1.3f;

	PxTransform transform = PxTransform(vPos.x, vPos.y + 1.5f, vPos.z);
	transform.q = PxQuat(Quat.x, Quat.y, Quat.z, Quat.w);

	Get_Manager()->Add_Controller(L"Parry", transform, EInteractionType::Box, PxBoxGeometry(2.f, 2.f, 0.2f));

	void* Data = Get_Manager()->GetData_Controller(L"Parry");

	MyPublicData* pParry = (MyPublicData*)Data;
	pParry->Attack = true;

	Get_Manager()->SetData_Controller(L"Parry", (void*)pParry);
}

void CPlayer::Delete_Parry()
{
	Get_Manager()->Delete_Controller(L"Parry");
}

HRESULT CPlayer::Add_Component()
{
	/* For.Com_DynamicMesh */
	if (FAILED(CGameObject::Add_Mesh
	(
		(_uint)ESceneType::Static,
		L"DynamicMesh_Player",
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

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Shader_Player",
		L"Shader_Player",
		EComponentType::Static,
		(CComponent**)&m_pShader
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	Ready_Stat();
	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable(_EFFECT _pEffect)
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

	if (m_bDisolve || m_bDeadDsv)
	{
		_pEffect->SetFloat("g_ftime", m_fDeadDsvTime);
		_pEffect->SetFloat("g_fDissolveSpeed", 0.3f);
		_float4 vColor = _float4(0.f, 0.f, 0.f, 1.f);
		_pEffect->SetVector("g_vColor", &vColor);
		_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	}
	else if (m_bDashDsv)
	{
		_pEffect->SetFloat("g_ftime", m_fDashDsvTime);
		_pEffect->SetFloat("g_fDissolveSpeed", 3.f);
		_float4 vColor = _float4(0.f, 0.f, 0.f, 1.f);
		_pEffect->SetVector("g_vColor", &vColor);
		_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	}
	else if (m_bDashReDsv)
	{
		_pEffect->SetFloat("g_ftime", m_fDashReDsvTime);
		_pEffect->SetFloat("g_fDissolveSpeed", 3.f);
		_float4 vColor = _float4(0.f, 0.f, 0.f, 1.f);
		_pEffect->SetVector("g_vColor", &vColor);
		_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	}


	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	_pEffect->SetFloat("g_RimPower", 13.f);
	//_pEffect->SetFloatArray("g_vRimColor", (_float*)&_float3(1.f, 0.05f, 0.05f),3);	//RGB

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	return S_OK;
}

void CPlayer::Cheat(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (pManagement->Key_Down(DIK_F1))
	{
		if (m_bCheat_Gravity)
			m_bCheat_Gravity = false;
		else if (!m_bCheat_Gravity)
			m_bCheat_Gravity = true;
	}

	


	if (m_bCheat_Gravity)
	{
		if (pManagement->Key_Pressing(DIK_NUMPAD8))
		{
			if (pManagement->Key_Pressing(DIK_NUMPAD4))
				m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vLU, 100.f, _fDeltaTime));
			else if (pManagement->Key_Pressing(DIK_NUMPAD6))
				m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vRU, 100.f, _fDeltaTime));
			else
				m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vLook, 100.f, _fDeltaTime));
		}
		else if (pManagement->Key_Pressing(DIK_NUMPAD5))
		{
			if (pManagement->Key_Pressing(DIK_NUMPAD4))
				m_pTransform->Set_Pos(pManagement->Move(L"Player", -m_vRU, 100.f, _fDeltaTime));
			else if (pManagement->Key_Pressing(DIK_NUMPAD6))
				m_pTransform->Set_Pos(pManagement->Move(L"Player", -m_vLU, 100.f, _fDeltaTime));
			else
				m_pTransform->Set_Pos(pManagement->Move(L"Player", -m_vLook, 100.f, _fDeltaTime));
		}
		else if (pManagement->Key_Pressing(DIK_NUMPAD4))
			m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vRight, 100.f, _fDeltaTime));
		else if (pManagement->Key_Pressing(DIK_NUMPAD6))
			m_pTransform->Set_Pos(pManagement->Move(L"Player", -m_vRight, 100.f, _fDeltaTime));

		if (pManagement->Key_Pressing(DIK_NUMPAD7))
			m_pTransform->Set_Pos(pManagement->Move(L"Player", {0.f,1.f,0.f}, 100.f, _fDeltaTime));
		else if (pManagement->Key_Pressing(DIK_NUMPAD9))
			m_pTransform->Set_Pos(pManagement->Move(L"Player", { 0.f,-1.f,0.f }, 100.f, _fDeltaTime));
	}
	

	if (pManagement->Key_Pressing(DIK_NUMPAD1))
		m_tState.fHP -= 1.f;
	else if (pManagement->Key_Pressing(DIK_NUMPAD2))
		m_tState.fHP += 1.f;

	if (pManagement->Key_Pressing(DIK_NUMPAD3))
		m_tState.fStamina = m_tState.fMaxStamina;
}

void CPlayer::Update_Dsv(const _float & _fDeltaTime)
{
	if (m_bDisolve)
		m_fDsvTime += _fDeltaTime * powf(1.5f, m_fDsvTime);
	else if (!m_bDisolve)
		m_fDsvTime = 0.f;

	if(m_bDeadDsv)
		m_fDeadDsvTime += _fDeltaTime * 0.5f;
	if (!m_bDeadDsv)
		m_fDeadDsvTime = 0.f;
}

void CPlayer::Reset_Parameter()
{
	m_eAttack = NO_ATTACK;
	m_iLightAttack = 0;
	m_iHeavyAttack = 0;
	m_fAirTime = 0.f;
	m_fAttackTime = 0.f;
	m_bAttack = false;
	m_bHit = false;
	m_bRoll = false;
	m_bJump = false;
	m_bJumpStart = true;
	m_bGravStart = true;
	m_bGravity = false;
	m_bDash = false;
	m_bStart = true;
	m_bDead = false;
	m_bFallDeath = false;
	m_bHeal = false;
	m_bUse = false;
	m_bUse_Heal = false;
	m_bUse_Item = false;
	m_bCharging = false;
	m_bChargeAttack = false;
	m_bChargingDir = true;
	m_bParry = false;
	m_bTargeting = false;
	m_vTargetPos = _float3(0.f, 0.f, 0.f);
}

void CPlayer::Update_CameraView()
{
	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
	
	if ((PxU32)EObstacle::Lateral2 & m_pPlayerData->ObstacleType)
	{
		pCamera->Set_CameraMode(eCameraMode::ScrollSecondView);
	}
	else if ((PxU32)EObstacle::Lateral1 & m_pPlayerData->ObstacleType)
	{
		pCamera->Set_CameraMode(eCameraMode::ScrollFirstView);
	}
	else
	{
		pCamera->Set_CameraMode(eCameraMode::ThirdView);
	}
	
}

void CPlayer::Ready_Stat()
{
	m_fBasicHP = 75.f;
	m_fBasicStamina = 78.f;

	m_iVitality = 1;	// 1Lv = 5HP 
	m_iStamina = 1;		// 1Lv = 2St 
	m_iStrength = 1;	// 1Lv = 2Att
	m_iArmor = 1;		// 1Lv = 5Def

	m_tState.fMaxHP = m_fBasicHP + ((_float)m_iVitality * 5.f);
	m_tState.fHP = m_tState.fMaxHP;
	m_tState.fMaxStamina = m_fBasicStamina + ((_float)m_iStamina * 2.f);
	m_tState.fStamina = m_tState.fMaxStamina;
	m_tState.fAtt = 78.f + ((_float)m_iStrength * 2.f);
	m_tState.fDef = 55.f + ((_float)m_iArmor * 5.f);
	m_tState.fPoise = 80.f;
	m_tState.fMaxPoise = m_tState.fPoise;
}

void CPlayer::Update_Stat(const _float & _fDeltaTime)
{
	m_tState.fMaxHP = m_fBasicHP + ((_float)m_iVitality * 5.f);
	m_tState.fMaxStamina = m_fBasicStamina + ((_float)m_iStamina * 2.f);
	m_tState.fAtt = 78.f + ((_float)m_iStrength * 2.f);
	m_tState.fDef = 55.f + ((_float)m_iArmor * 5.f);

	if (m_tState.fStamina <= 0.f)
		m_bStaminaCool = true;

	if (m_bStaminaCool)
	{
		m_fStaminaCool += _fDeltaTime;

		if (m_fStaminaCool >= 2.f)
		{
			m_fStaminaCool = 0.f;
			m_bStaminaCool = false;
		}
	}

	if ((m_pMesh->Get_Animationset() == (_uint)PlayerAniID::WALK) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::IDLE) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::RUN) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::IDLE_WEAPON) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_FRONT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_BACK) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_LEFT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_RIGHT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USEITEM_TIDLE) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_TIDLE) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HARD_DEAL) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HIT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::INTERACTION))
	{
			if (m_tState.fMaxStamina > m_tState.fStamina)
				m_tState.fStamina += 20.f * _fDeltaTime;
			if (m_tState.fMaxStamina < m_tState.fStamina)
				m_tState.fStamina = m_tState.fMaxStamina;
	}

	if (m_tState.fPoise != m_tState.fMaxPoise || m_tState.fPoise <= 0.f)
		m_fPoiseTime += _fDeltaTime;

	if (m_fPoiseTime >= 3.f)
	{
		m_fPoiseTime = 0.f;
		m_tState.fPoise = m_tState.fMaxPoise;
	}

	if (m_tState.fHP <= 0.f)
	{
		m_tState.fHP = 0.f;
		m_bDead = true;
		m_eCurState = DEAD;
	}
	if (m_bBonfireHeal)
	{
		m_tState.fHP += 150.f * _fDeltaTime;
		m_tState.fStamina += 150.f * _fDeltaTime;
	}

	if (m_tState.fHP > m_tState.fMaxHP)
	{
		m_tState.fHP = m_tState.fMaxHP;
		if (m_bBonfireHeal)
			m_bBonfireHeal = false;
	}
	if (m_tState.fStamina > m_tState.fMaxStamina)
	{
		m_tState.fStamina = m_tState.fMaxStamina;
	}
}

void CPlayer::Add_PlayerAttack(_float _fDamage, DAMAGE _eDamage)
{
	if (m_bDead)
		return;
	//데미지 = (공격력 / 2 - 수비력 / 4) + 무기 공격력

	m_tState.fPoise -= _fDamage * 0.25f;
	if(m_tState.fPoise > 0)
		m_fPoiseTime = 0.f;

	if ((m_tState.fPoise <= 0.f && _eDamage == DAMAGE::NORMAL_DAMAGE) || m_ePreState == STATE::INTERACTION)
	{
		if (!m_bGod_Mode)
		{
			// 피격 애니메이션 돌릴거
			m_eCurState = HIT;
			m_bRoll = false;
			m_bDash = false;
			m_iDash = 1;
			m_bHit = true;

			if (m_bAttack || m_bChargeAttack || m_bCharging)
			{
				m_bAttack = false;
				m_bChargeAttack = false;
				m_bCharging = false;
				m_eAttack = NO_ATTACK;
			}
			if (m_bInteract)
			{
				m_bInteract_Stand = false;
				m_bInteract_Save = false;
				m_bNPCInteracting = false;
				m_bNpcValnirInteract = false;
				m_bNpcChoronInteract = false;
				m_bLevelStoneInteract = false;
				m_bInteract = false;
			}
		}
	}
	if (_eDamage != DAMAGE::FALL_DAMAGE)
	{
		_uint iCount = Mersen_ToInt(0, 2);
		TCHAR szFileName[256] = L"";
		wsprintf(szFileName, L"Player_Hit%d.wav", iCount);
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Hit);
		GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Hit);
	}
	
	m_bAttackSound = false;

	if (!m_bGod_Mode)
	{
		_float fDamage;
		if (_eDamage != DAMAGE::FALL_DAMAGE)
			fDamage = _fDamage - (m_tState.fDef * 0.1f);

		if (fDamage <= 0.f)
			fDamage = 1.f;

		m_tState.fHP -= fDamage;

		if (m_tState.fHP <= 0.f)
		{
			// 죽는 애니메이션
			_uint iCount = Mersen_ToInt(0, 1);
			TCHAR szFileName[256] = L"";
			wsprintf(szFileName, L"Player_Dead%d.wav", iCount);
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Hit);
			GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Hit);
			m_eCurState = DEAD;
			m_bDead = true;
		}
	}
}

void CPlayer::Collider()
{
	m_pPlayerData = (MyPlayerData*)GET_MANAGEMENT->PhysXData_Update(L"Player");

	if ((PxU32)EObstacle::Skill1 & m_pPlayerData->ObstacleType && !m_bHitSkill)
	{
		Add_PlayerAttack(m_pPlayerData->Damage, DAMAGE::NORMAL_DAMAGE);
		m_pPlayerData->Damage = 0.f;
		m_pPlayerData->Collision = false;
		m_bHitSkill = true;
		m_pPlayerData->ObstacleType ^= (PxU32)EObstacle::Skill1;
		Get_Manager()->SetData_Controller(L"Player", m_pPlayerData);
	}

	// 피격 콜라이더
	void* Data = Get_Manager()->GetData_Controller(L"Parry");

	//	return;
	MyPublicData* pParry = nullptr;

	if (nullptr != Data)
		pParry = (MyPublicData*)Data;

	if (!(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_MEETBOSS) && nullptr != pParry && pParry->Collision)
	{
		m_bWeaponParry = true;
		pParry->Attack = false;
		pParry->Collision = false;
		Get_Manager()->SetData_Controller(L"Parry", (void*)pParry);
	}
	else if (m_pPlayerData->Collision && !m_bGod_Mode)
	{
		Add_PlayerAttack(m_pPlayerData->Damage, DAMAGE::NORMAL_DAMAGE);
		m_pPlayerData->Damage = 0.f;
		m_pPlayerData->Collision = false;
		Get_Manager()->SetData_Controller(L"Player", m_pPlayerData);
	}


	// 게이트 업데이트
	if (!((PxU32)EObstacle::PortalGate & m_pPlayerData->ObstacleType))
	{
		m_bPortalGateOn = false;
	}

	if ((PxU32)EObstacle::PortalGateIn & m_pPlayerData->ObstacleType && m_bPortalGateOn)
	{
		switch (m_iSceneIdx)
		{
		case (_uint)ESceneType::Tutorial:
		{
			CTutorial* pTutorial = dynamic_cast<CTutorial*>(GET_MANAGEMENT->Get_CurrentScene());
			pTutorial->Set_ShrineScene();
		}
		break;

		case (_uint)ESceneType::Shrine:
		{
			CShrine_Scene* pShrine = dynamic_cast<CShrine_Scene*>(GET_MANAGEMENT->Get_CurrentScene());
			pShrine->Set_ShrineScene();
		}
		break;

		case (_uint)ESceneType::Stage2:
		{
			CStage2_Scene* pShrine = dynamic_cast<CStage2_Scene*>(GET_MANAGEMENT->Get_CurrentScene());
			pShrine->Set_ShrineScene();
		}
		break;
		}
		m_bPortalGateOn = false;
		Get_Manager()->StopSound(CSound_Manager::ESoundID::Object3D_0);
		//Get_Manager()->StopSound(CSound_Manager::ESoundID::BGM);
	}
	Update_CameraView();


	if ((PxU32)EObstacle::Trap & m_pPlayerData->ObstacleType)
	{
		Add_PlayerAttack(m_pPlayerData->Damage, DAMAGE::NORMAL_DAMAGE);
		m_pPlayerData->Damage = 0.f;
		m_pPlayerData->Collision = false;
		Get_Manager()->SetData_Controller(L"Player", m_pPlayerData);
		Min_ObstacleType(EObstacle::Trap);
	}

	if ((PxU32)EObstacle::ChurchDoor & m_pPlayerData->ObstacleType)
	{
		if (!(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_STAGE2_CHURCHDOOR_CLOSE))
			Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_STAGE2_CHURCHDOOR_CLOSE);
	}

	if ((PxU32)EObstacle::Stage2BossStart & m_pPlayerData->ObstacleType)
	{
		if (!(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_STAGE2_BOSS_START))
			Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_STAGE2_BOSS_START);
	}
}

void CPlayer::Update_Dead(const _float & _fDeltaTime)
{
	if (m_fAirTime >= 2.f)
	{
		m_bFallDeath = true;
		m_tState.fHP = 0.f;
		m_fAirTime = 0.f;
	}
	if (m_bDead)
	{
		m_bDeadDsv = true;
		m_bDashReDsv = false;
		m_bDashDsv = false;
		if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DEAD && !m_pMesh->Is_AnimationSetEnd())
		{
			m_pMesh->Play_Animationset(_fDeltaTime * m_fAniSpeed);
		}
		else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DEAD && m_pMesh->Is_AnimationSetEnd())
		{

			Get_EventMgr()->Add_Event(EVENT_VOLATILE_PLAYER_DEAD);
			Get_EventMgr()->Min_Event(EVENT_VOLATILE_PLAYER_REVIVE | EVENT_VOLATILE_PLAYER_LIFE);
			
			Reset_Parameter();
			m_tState.fHP = m_tState.fMaxHP;
			m_tState.fStamina = m_tState.fMaxStamina;
			m_eCurState = SLEEPING;
			m_bDeadDsv = false;
			m_pTransform->Set_Pos((GET_MANAGEMENT->Set_GlobalPos(L"Player", m_vSpawnPos)));
			CMainCamera* pCamera = dynamic_cast<CMainCamera*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
			pCamera->Set_Camera_At(m_vCameraAt);
			pCamera->Set_Camera_Eye(m_vCameraEye);

		}
	}
}

void CPlayer::Heal(const _float & _fDeltaTime)
{
	if (m_fHealTime > 0.f)
	{
		m_tState.fHP += (m_fHealing * 1.f * _fDeltaTime);
		m_fHealTime -= _fDeltaTime;
	}
	else if (m_fHealTime <= 0.f)
	{
		m_fHealing = 0.f;
		m_fHealTime = 0.f;
	}
}

void CPlayer::Dash_Dsv(const _float & _fDeltaTime)
{
	if (m_eCurState == DASH_AIR || m_eCurState == DASH_FRONT
		|| m_eCurState == DASH_BACK || m_eCurState == DASH_LEFT || m_eCurState == DASH_RIGHT)
	{
		if (!m_bDashDsv && !m_bDashReDsv)
		{
			m_bDashReDsv = false;
			m_bDashDsv = true;
		}
	}

	if (m_bDashDsv && m_fDashDsvTime >= 1.f)
	{
		m_bDashReDsv = true;
		m_bDashDsv = false;
		m_fDashDsvTime = 0.f;
	}

	if (m_fDashReDsvTime >= 1.f)
	{
		m_bDashReDsv = false;
		m_bDashDsv = false;
		m_fDashDsvTime = 0.f;
	}
	if (!m_bDashDsv && !m_bDash)
		m_fDashDsvTime = 0.f;
	if (!m_bDashReDsv && !m_bDash)
		m_fDashReDsvTime = 0.f;

	if (m_bDashDsv)
		m_fDashDsvTime += (_fDeltaTime * powf(40.f, m_fDashDsvTime));
	if (m_bDashReDsv)
		m_fDashReDsvTime += (_fDeltaTime * powf(40.f, m_fDashDsvTime));
}


void CPlayer::CreateBoneParticle()
{
	if (!m_bBoneParticle)
	{
		_float4x4* pPlayerBoneMatrix = nullptr;
		_ulong	ulongCount = 0;
		_float3 vCreatePos;
		_float4x4 matPlayerWorld;
		CDynamicMesh* pMesh = nullptr;
		//vector<_float4x4> MeshBone;
		m_pMesh->Get_AllBoneMetrix(&pPlayerBoneMatrix, &ulongCount);
		for (_ulong i = 0; i < ulongCount; ++i)
		{
			matPlayerWorld = *(m_pTransform->Get_WorldMatrix());
			//memcpy(vCreatePos, &(pPlayerBoneMatrix[i] * matPlayerWorld).m[3][0], sizeof(_float3));
			if (i % 2)
			{
				GET_MANAGEMENT->Generate_ExplosionParticle(
					m_iSceneIdx,
					*(_float3*)&((pPlayerBoneMatrix[i] * matPlayerWorld).m[3][0]),
					3.f,
					//(_float3*)&((pPlayerBoneMatrix[i] * matPlayerWorld).m[3][0]),
					nullptr,
					3,
					_float4(0.6f, 0.925490f, 1.f, 1.f)/*_ARGB(255, 153, 236, 255)*/
					//,ePartilcePatternType::OnStartBlinkAnd_GoUpToTarget
					, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUpToTarget | (_ulong)ePartilcePatternType::x2)
					, 0, 1.5f, 0.f, 0.5f);
			}
			else
			{
				GET_MANAGEMENT->Generate_ExplosionParticle(
					m_iSceneIdx,
					*(_float3*)&((pPlayerBoneMatrix[i] * matPlayerWorld).m[3][0]),
					3.f,
					//(_float3*)&((pPlayerBoneMatrix[i] * matPlayerWorld).m[3][0]),
					nullptr,
					3,
					_float4(0.607843f, 0.529411f, 1.f, 1.f)
					, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUpToTarget | (_ulong)ePartilcePatternType::x2)
					, 0, 1.5f, 0.f, 0.5f);
			}
		}

		m_bBoneParticle = true;
	}	
}

HRESULT CPlayer::AddTrailEffect_Class()
{
	//for (auto& arr : m_pArrDash_TrailEffect)
	//	arr = nullptr;
	//for (auto& arr : m_pArrDash_TrailEffect)
	//	arr = nullptr;
	//////////////////////////////////////////////////////////////////////////////////
////Trail Test
//TRAIL_DESC tTrailDesc;
//tTrailDesc.pObj = this;
//lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");
//tTrailDesc.fTrailWidth = 3.f;
//tTrailDesc.fTrailChekRate = 0.01f;
//tTrailDesc.vColorRGB = _float3(0.8f, 0.8f, 0.8f);
//tTrailDesc.bRenderOnOff = true;
//const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-Head");
//tTrailDesc.matLocal = pFrame1->CombinedTranformationMatrix;
////tTrailDesc.matLocal.m[3][0] += 5.f;

//FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(
//	(_uint)ESceneType::Static, L"GameObject_TrailEffect", m_iSceneIdx, L"Layer_Effect",&tTrailDesc), E_FAIL);

//////////////////////////////////////////////////////////////////////////////////
	for (_uint i = 0; i < 5; ++i)
	{
		m_pArrDash_TrailEffect[i] = nullptr;
		m_pArrDash_TrailDistortEffect[i] = nullptr;
		TRAIL_DESC tTrailDesc;
		_float4x4 matWorld;
		D3DXMatrixIdentity(&matWorld);

		tTrailDesc.pObj = this;
		lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");
		tTrailDesc.fTrailChekRate = 0.01f;
		tTrailDesc.fTrailWidth = 20.f;

		//75 137 220
	//tTrailDesc.vColorRGB = _float3(75 / 255.f, 137 / 255.f, 220 / 255.f);
	//tTrailDesc.vColorRGB = _float3(0.6f, 0.925490f, 1.f)/*_ARGB(255, 153, 236, 255)*/;
		tTrailDesc.vColorRGB = _float3(0.25f, 0.25f, 0.25f);
		//tTrailDesc.vColorRGB = _float3(1.f, 0.f, 1.f);
		tTrailDesc.bRenderOnOff = false;
		//const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-Head");

		tTrailDesc.iRenderID = 1;
		if (0 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-Pelvis");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}
		else if (1 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-R-Hand");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}
		else if (2 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-L-Hand");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}
		else if (3 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-R-Clavicle");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}
		else if (4 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-L-Clavicle");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}



		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
			L"GameObject_TrailEffect", (CGameObject**)&m_pArrDash_TrailEffect[i], &tTrailDesc), E_FAIL);
		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer((CGameObject*)(m_pArrDash_TrailEffect[i]), m_iSceneIdx, L"Layer_Player_Effect"), E_FAIL);

		Safe_AddRef(m_pArrDash_TrailEffect[i]);

		tTrailDesc.fTrailWidth = 40.f;
		tTrailDesc.iRenderID = 4;
		tTrailDesc.eRenderState = ERenderPriority::Environment;
		tTrailDesc.iRenderPass = 4;
		
		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
			L"GameObject_TrailEffect", (CGameObject**)&m_pArrDash_TrailDistortEffect[i], &tTrailDesc), E_FAIL);
		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer((CGameObject*)(m_pArrDash_TrailDistortEffect[i]), m_iSceneIdx, L"Layer_Player_Effect"), E_FAIL);
		Safe_AddRef(m_pArrDash_TrailDistortEffect[i]);
	}


	







	return S_OK;
}

void CPlayer::DashTrailSwitch(_bool _bOnOff)
{
	for (_uint i = 0; i < 5; ++i)
	{
		m_pArrDash_TrailEffect[i]->Set_RenderOnOff(_bOnOff);
		m_pArrDash_TrailDistortEffect[i]->Set_RenderOnOff(_bOnOff);
	}
}

void CPlayer::DashTrailRandomSwitch()
{
	if (Mersen_ToInt(0, 1))
	{
		_uint iOffTrail1 = Mersen_ToInt(0, 5);
		_uint iOffTrail2 = Mersen_ToInt(0, 5);
		_uint iOffTrail3 = Mersen_ToInt(0, 5);
		for (_uint i = 0; i < 5; ++i)
		{
			if (i == iOffTrail1 || i == iOffTrail2 || i == iOffTrail3 )
				m_pArrDash_TrailEffect[i]->Set_RenderOnOff(true);
			else
				m_pArrDash_TrailEffect[i]->Set_RenderOnOff(false);
		}
	}
	else
	{
		_uint iOffTrail1 = Mersen_ToInt(0, 5);
		_uint iOffTrail2 = Mersen_ToInt(0, 5);
		for (_uint i = 0; i < 5; ++i)
		{
			if (i == iOffTrail1 || i == iOffTrail2)
				m_pArrDash_TrailEffect[i]->Set_RenderOnOff(true);
			else
				m_pArrDash_TrailEffect[i]->Set_RenderOnOff(false);
		}

	}
	
	
}

void CPlayer::DashTrailColor_Set(const _float3& _vColorRgb)
{
	for (_uint i = 0; i < 5; ++i)
	{
		m_pArrDash_TrailEffect[i]->Set_Color(_vColorRgb);
	}
}

void CPlayer::DashTrailWidth_Set(const _float & _fWidth)
{
	for (_uint i = 0; i < 5; ++i)
	{
		m_pArrDash_TrailEffect[i]->Set_Width(_fWidth);
	}
}

void CPlayer::DashTrailRenderId_Set(const _uint & _iRenderId)
{
	for (_uint i = 0; i < 5; ++i)
	{
		m_pArrDash_TrailEffect[i]->Set_RenderID(_iRenderId);
	}
}

void CPlayer::DashTrailUpdateMatrix()
{
	for (_uint i = 0; i < 5; ++i)
	{
		_float4x4 matWorld;
		if (0 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-Pelvis");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			m_pArrDash_TrailEffect[i]->Set_LocalWorld(matWorld);
			m_pArrDash_TrailDistortEffect[i]->Set_LocalWorld(matWorld);
		}
		else if (1 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-R-Hand");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			m_pArrDash_TrailEffect[i]->Set_LocalWorld(matWorld);
			m_pArrDash_TrailDistortEffect[i]->Set_LocalWorld(matWorld);
		}
		else if (2 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-L-Hand");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			m_pArrDash_TrailEffect[i]->Set_LocalWorld(matWorld);
			m_pArrDash_TrailDistortEffect[i]->Set_LocalWorld(matWorld);
		}
		else if (3 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-R-Clavicle");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			m_pArrDash_TrailEffect[i]->Set_LocalWorld(matWorld);
			m_pArrDash_TrailDistortEffect[i]->Set_LocalWorld(matWorld);
		}
		else if (4 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-L-Clavicle");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			m_pArrDash_TrailEffect[i]->Set_LocalWorld(matWorld);
			m_pArrDash_TrailDistortEffect[i]->Set_LocalWorld(matWorld);
		}
	}
}

void CPlayer::ChargingEffect()
{
	if (!m_bChargeAttack)
	{
		//------------------------------차징 데칼 생성--------------------------------------//

		TRANSFORM_DESC tTempTRANSFORM_DESC;
		tTempTRANSFORM_DESC.vScale = { 3.f,3.f,0.6f };
		const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-R-Hand");
		tTempTRANSFORM_DESC.matWorld = pFrame1->CombinedTranformationMatrix * m_pTransform->Get_TransformDesc().matWorld;
		memcpy(&tTempTRANSFORM_DESC.vPos, &tTempTRANSFORM_DESC.matWorld.m[3][0], sizeof(_float3));
		tTempTRANSFORM_DESC.vRotate = m_pTransform->Get_TransformDesc().vRotate;
		tTempTRANSFORM_DESC.vRotate.x = D3DXToRadian(90.f);
		Get_Manager()->Generate_Decal(m_iSceneIdx, tTempTRANSFORM_DESC
			, _float4(1.f, 1.f, 1.f, 1.f), 22
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OffStartBlinkPow
				| (_ulong)ePartilcePatternType::GoStraight
				| (_ulong)ePartilcePatternType::ScaleUp
				| (_ulong)ePartilcePatternType::BillBoard
				| (_ulong)ePartilcePatternType::x3));
		tTempTRANSFORM_DESC.vScale *= 3.f;
		Get_Manager()->Generate_Decal(m_iSceneIdx, tTempTRANSFORM_DESC
			, _float4(0.2f, 0.2f, 0.2f, 0.2f), 18
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OffStartBlinkPow
				| (_ulong)ePartilcePatternType::GoStraight
				| (_ulong)ePartilcePatternType::ScaleUp
				| (_ulong)ePartilcePatternType::BillBoard
				| (_ulong)ePartilcePatternType::x3));

		//------------------------------차징 데칼 생성--------------------------------------//
	}
	TRANSFORM_DESC tTempTRANSFORM_DESC;
	const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Bip001-R-Hand");
	tTempTRANSFORM_DESC.matWorld = pFrame1->CombinedTranformationMatrix * m_pTransform->Get_TransformDesc().matWorld;
	memcpy(&tTempTRANSFORM_DESC.vPos, &tTempTRANSFORM_DESC.matWorld.m[3][0], sizeof(_float3));
	
	tTempTRANSFORM_DESC.vRotate = m_pTransform->Get_TransformDesc().vRotate;
	tTempTRANSFORM_DESC.vRotate.x = D3DXToRadian(90.f);

	Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, tTempTRANSFORM_DESC.vPos, 1.f,nullptr,1
		, _float4(0.6f, 0.925490f, 1.f,2.2f), (ePartilcePatternType)(
		(_ulong)ePartilcePatternType::OffStartBlink
			| (_ulong)ePartilcePatternType::GoStraightToTarget
			| (_ulong)ePartilcePatternType::x2
			)
		, Mersen_ToInt(6,8),2.2f);

	const MY_FRAME*  pFrame2 = m_pMesh->Get_FrameByName("Bip001-L-Hand");
	tTempTRANSFORM_DESC.matWorld = pFrame2->CombinedTranformationMatrix * m_pTransform->Get_TransformDesc().matWorld;
	memcpy(&tTempTRANSFORM_DESC.vPos, &tTempTRANSFORM_DESC.matWorld.m[3][0], sizeof(_float3));

	tTempTRANSFORM_DESC.vRotate = m_pTransform->Get_TransformDesc().vRotate;
	tTempTRANSFORM_DESC.vRotate.x = D3DXToRadian(90.f);

	Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, tTempTRANSFORM_DESC.vPos, 1.f, nullptr, 1
		, _float4(0.607843f, 0.529411f, 1.f, 2.2f), (ePartilcePatternType)(
		(_ulong)ePartilcePatternType::OffStartBlink
			| (_ulong)ePartilcePatternType::GoStraightToTarget
			| (_ulong)ePartilcePatternType::x3
			)
		, Mersen_ToInt(6, 8), 2.2f);

}

CPlayer * CPlayer::Create(_DEVICE _pDevice)
{
	CPlayer* pInstance = new CPlayer(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(const _uint& _iSceneIdx, void * _pArg)
{
	CPlayer* pClone = new CPlayer(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}
	return pClone;
}

void CPlayer::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pInterfaceUI);
		Safe_Release(m_pDeadUI);
		for (_uint i = 0; i < 5; ++i)
		{
			//Safe_AddRef(m_pArrDash_TrailEffect[i]);
			Safe_Release(m_pArrDash_TrailEffect[i]);
			Safe_Release(m_pArrDash_TrailDistortEffect[i]);		
		}
	}
	CGameObject::Free();
}

void CPlayer::Update_Sound()
{
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::IDLE || m_pMesh->Get_Animationset() == (_uint)PlayerAniID::IDLE_WEAPON)
		Reset_Sound();

	


	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::ROLL)
	{
		if (m_bRollSound)
		{
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player);
			GET_MANAGEMENT->PlaySounds(L"Player_Roll.ogg", CSound_Manager::ESoundID::Player);
			m_bRollSound = false;
		}
	}
	else if (m_pMesh->Get_Animationset() != (_uint)PlayerAniID::ROLL && !m_bRollSound)
		m_bRollSound = true;

	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_AIR ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_FRONT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_BACK) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_LEFT) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_RIGHT))
	{
		if (m_bDashSound)
		{
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player);
			GET_MANAGEMENT->PlaySounds(L"Player_Dash.wav", CSound_Manager::ESoundID::Player);
			m_bDashSound = false;
		}
	}
	else
		m_bDashSound = true;

	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::JUMP_START)
	{
		if (m_bJumpSound)
		{
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player);
			GET_MANAGEMENT->PlaySounds(L"Player_Jump.ogg", CSound_Manager::ESoundID::Player);
			m_bJumpSound = false;
		}
	}
	else
		m_bJumpSound = true;

	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::JUMP_LAND)
	{
		if (m_bJumpOnDown)
		{
			_uint iCount = Mersen_ToInt(0, 2);
			TCHAR szFileName[256] = L"";
			wsprintf(szFileName, L"Player_Sprint%d.ogg", iCount);
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
			GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
			m_bJumpOnDown = false;
		}
	}
	else
		m_bJumpOnDown = true;

	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::WALK)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(38, 9))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Run%d.ogg", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(38, 26))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Run%d.ogg", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;

			}
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::RUN)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(25, 18))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Run%d.ogg", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(25, 8))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Run%d.ogg", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;

			}
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::SPRINT)
	{
		_uint iCount = Mersen_ToInt(0, 2);
		if (m_pMesh->Get_AnimationFrame(25, 19))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Sprint%d.ogg", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(25, 8))
		{
			if (m_bRightLag)
			{

				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Sprint%d.ogg", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_FRONT)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(31, 30))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if(m_pMesh->Get_AnimationFrame(31, 15))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_BACK)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(32, 30))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(32, 14))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_LEFT)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(32, 30))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(32, 15))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
			
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_RIGHT)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(32, 30))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(32, 15))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
	}
		
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK || m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(73, 61))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(73, 44))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(73, 26))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(73, 9))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(60, 59))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 45))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 30))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 15))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(60, 59))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 45))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 30))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 14))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(60, 59))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 45))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 30))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 15))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(60, 59))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
			
		}
		else if (m_pMesh->Get_AnimationFrame(60, 45))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 30))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(60, 15))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
	}

	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(90, 89))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 75))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 60))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 45))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 30))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 15))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(90, 89))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 75))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 60))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 45))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 30))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 15))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(90, 89))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 75))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 60))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 45))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 30))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 15))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT)
	{
		_uint iCount = Mersen_ToInt(0, 3);
		if (m_pMesh->Get_AnimationFrame(90, 89))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 75))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 60))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 45))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 30))
		{
			if (m_bRightLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bRightLag = false;
				m_bLeftLag = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(90, 15))
		{
			if (m_bLeftLag)
			{
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Player_Walk%d.wav", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player_Walk);
				GET_MANAGEMENT->PlaySounds(szFileName, CSound_Manager::ESoundID::Player_Walk);
				m_bLeftLag = false;
				m_bRightLag = true;
			}
		}
	}
}

void CPlayer::Reset_Sound()
{
	m_bRollSound = true;
	m_bLeftLag = true;
	m_bRightLag = true;
	m_bJumpSound = true;
	m_bJumpOnDown = true;
	m_bDashSound = true;
	m_bAttackSound = true;
}
