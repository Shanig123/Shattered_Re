#include "stdafx.h"
#include "JBaseBoss.h"
#include "Player_UI.h"
#include "Player.h"
#include "MainCamera.h"

#define BOSS_CHECK_ACTIVATION (m_ePreTotalPattern == eBossMonsterPattern::Awake ||		\
m_ePreTotalPattern == eBossMonsterPattern::Move ||											\
m_ePreTotalPattern == eBossMonsterPattern::Idle ||											\
m_ePreTotalPattern == eBossMonsterPattern::Hit ||											\
m_ePreTotalPattern == eBossMonsterPattern::Attack||											\
m_ePreTotalPattern == eBossMonsterPattern::Dead )


CJBaseBoss::CJBaseBoss(_DEVICE _pDevice, const wstring& _PrototypeName /*= L""*/)
	:CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr), m_pTargetTransform(nullptr)
	, m_pShader(nullptr)
	, m_pWeapon(nullptr)
	, m_pBossUI(nullptr)
	, m_bAttackCoolTime(false)
	, m_bTargetLook(false), m_bApplyGravity(true), m_bHit(false)
	, m_bAppearEnd(false)
	, m_bDropField(false)
	, m_fDeltaTime(0.f), m_fSkillMoveSpeed(0.f), m_fAttackCoolTime(0.f), m_fTargetDist(0.f), m_fWalkMoveTime(0.f)
	, m_fHitDamage(0.f), m_fAddAniPlayTime(0.f)
	, m_fDissolveTime(0.f)
	, m_iShaderPass(0), m_iDiffuseIndex(0)
	, m_iDelayMovePattern(0)
	, m_iGold(0)
	, m_vRight(0.f, 0.f, 0.f), m_vLook(0.f, 0.f, 0.f)
	, m_bRenderOnce(false)
	, m_fFarRange(0.f)
	, m_bJump(true), m_bJumpStart(true), m_bDoubleJump(false), m_bJumping(true), m_bGravity(false), m_bGravityStart(true)
	, m_fJumpTime(0.f)
	, m_iJump(1), m_iDash(1)
	, m_vJumpPos(_float3(0.f, 0.f, 0.f)), m_vGravityPos(_float3(0.f, 0.f, 0.f))
	, m_vInitilizePos{ 0.f, 0.f, 0.f }
	, m_bCheck_FirstDeadFlag(false)
	, m_eCurTotalPattern(eBossMonsterPattern::End), m_ePreTotalPattern(eBossMonsterPattern::End)
	, m_matChildWeapon(_float4x4())
	, m_matSecondChildWeapon(_float4x4())
	, m_bHitEffectOnce(false)
	, m_bUpdatePhysx(true)
	, m_bAttackSound(false)
	, m_bLeftLeg(true)
	, m_bRightLeg(true)
	, m_iDissolveTexture(0)
{
	ZeroMemory(&m_tState, sizeof(STAT_DESC));
	m_PrototypeName = _PrototypeName;
}

CJBaseBoss::CJBaseBoss(const CJBaseBoss & _rOther)
	:CGameObject(_rOther)
	, m_pMesh(_rOther.m_pMesh)
	, m_pTransform(_rOther.m_pTransform), m_pTargetTransform(nullptr)
	, m_pShader(_rOther.m_pShader)
	, m_pWeapon(_rOther.m_pWeapon)
	, m_pBossUI(_rOther.m_pBossUI)
	, m_bAttackCoolTime(false)
	, m_bTargetLook(false), m_bApplyGravity(true), m_bHit(false)
	, m_bAppearEnd(false)
	, m_bDropField(false)
	, m_fDeltaTime(0.f), m_fSkillMoveSpeed(0.f), m_fAttackCoolTime(0.f), m_fTargetDist(0.f), m_fWalkMoveTime(0.f)
	, m_fHitDamage(0.f), m_fAddAniPlayTime(0.f)
	, m_fDissolveTime(0.f), m_fDissolveSpeed(0.f)
	, m_iShaderPass(0), m_iDiffuseIndex(0)
	, m_iDelayMovePattern(0)
	, m_iGold(0)
	, m_vRight(0.f, 0.f, 0.f), m_vLook(0.f, 0.f, 0.f)
	, m_bRenderOnce(false)
	, m_fFarRange(0.f)
	, m_bJump(true), m_bJumpStart(true), m_bDoubleJump(false), m_bJumping(true), m_bGravity(false), m_bGravityStart(true)
	, m_fJumpTime(0.f)
	, m_iJump(1), m_iDash(1)
	, m_vJumpPos(_float3(0.f, 0.f, 0.f)), m_vGravityPos(_float3(0.f, 0.f, 0.f))
	, m_vInitilizePos{ 0.f, 0.f, 0.f }
	, m_bCheck_FirstDeadFlag(false)
	, m_eCurTotalPattern(eBossMonsterPattern::End), m_ePreTotalPattern(eBossMonsterPattern::End)
	, m_iItemCreateYAxisPosFix(0.f)
	, m_bUpdatePhysx(true)
	, m_matSecondChildWeapon(_float4x4())
	, m_bAttackSound(false)
	, m_bLeftLeg(true)
	, m_bRightLeg(true)
	, m_iDissolveTexture(0)
{
	ZeroMemory(&m_tState, sizeof(STAT_DESC));
}

HRESULT CJBaseBoss::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJBaseBoss::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (FAILED(CGameObject::Ready_GameObject_Clone(_iSceneIdx,_pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (_pArg)
	{
		memcpy(&m_tTransformDesc, _pArg, sizeof(TRANSFORM_DESC));
	}

	if (FAILED(CJBaseBoss::Add_Component()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;

	m_iGold = Mersen_ToInt(1000, 2000);

	m_eCurTotalPattern = eBossMonsterPattern::Ready;
	m_ePreTotalPattern = eBossMonsterPattern::Ready;

	m_fFarRange = 300.f;
	
	return S_OK;
}

_uint CJBaseBoss::Update_GameObject(const _float & _fDeltaTime)
{
	m_fDeltaTime = _fDeltaTime;

	if (m_bUpdateInit)
	{
		CJBaseBoss::Do_TotalPattern(m_fDeltaTime);
		CJBaseBoss::PatternTotalControl(m_fDeltaTime);
	}
	else
		CJBaseBoss::Update_Init(m_fDeltaTime);

	CGameObject::Update_GameObject(_fDeltaTime);
	m_pTransform->Update_Transform();

	return m_iEvent;
	;
}

_uint CJBaseBoss::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	CJBaseBoss::CoolTimer(_fDeltaTime);

	CGameObject::LateUpdate_GameObject(_fDeltaTime);

	return m_iEvent;
}

HRESULT CJBaseBoss::Render_GameObject()
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

	m_pMesh->Render_Meshes(pEffect, m_iDiffuseIndex, nullptr, true);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CJBaseBoss::Add_Component()
{
	/* For.Com_DynamicMesh */
	wstring wstrPrototypeName = L"DynamicMesh_" + m_PrototypeName;
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

	m_pTargetTransform = dynamic_cast<CTransform*>(Get_Manager()->Get_Component(m_iSceneIdx, L"Layer_Player", L"Com_Transform"));

	if (FAILED(CGameObject::Add_Component
	(
		(_uint)ESceneType::Static,
		L"Shader_General_Dynamic",
		L"Shader_General_Dynamic",
		EComponentType::Static,
		(CComponent**)&m_pShader
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJBaseBoss::SetUp_ConstantTable(_EFFECT & _pEffect)
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
	
	_pEffect->SetVector("g_vColor", &m_vDissolveColor);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	pManagement->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveTexture);

	return S_OK;
}

#pragma region CONTROLLER

HRESULT CJBaseBoss::PatternTotalControl(const _float & _fDeltaTime)
{
	if (m_eCurTotalPattern != m_ePreTotalPattern)
	{
		switch (m_eCurTotalPattern)
		{
		case eBossMonsterPattern::Ready:
			{
			m_tState.fHP = m_tState.fMaxHP;
			m_tState.fStamina = m_tState.fMaxStamina;
			m_tState.fPoise = m_tState.fPoise;
			m_fDissolveTime = 0.f;

			m_bTargetLook = false;
			m_bGravity = false;

			m_fJumpTime = 0.f;
			m_vGravityPos = { 0.f, 0.f, 0.f };

			m_vRight = { 0.f, 0.f, 0.f };
			m_vLook = { 0.f, 0.f, 0.f };

			m_fHitDamage = 0.f;
			m_fDissolveSpeed = 0.f;
			m_fAddAniPlayTime = 1.3f;

			m_pTransform->Set_Pos(m_vInitilizePos);
			if (m_iShaderPass > 4 && m_iShaderPass < 9)
				m_iShaderPass -= 5;
			}
			break;
		case eBossMonsterPattern::Awake:
			{
				Do_Activation(_fDeltaTime);

			}
			break;
		case eBossMonsterPattern::Idle:
			{
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_STAGE_MEETBOSS);
			}
			break;
		case eBossMonsterPattern::Attack:
			{

			}
			break;
		case eBossMonsterPattern::Move:
			{

			}
			break;
		case eBossMonsterPattern::Hit:
			{

			}
			break;
		case eBossMonsterPattern::Dead:
			{
			m_bTargetLook = false;
			m_fDissolveTime = 0.f;
			m_fDissolveSpeed = 1.f;
			m_iGold = Mersen_ToInt(1000, 2000);
			if (m_pWeapon)
				m_pWeapon->Trail_Switch(false);

			switch (m_iShaderPass)
			{
			case 0:
			case 1:
			case 2:
				m_iShaderPass += 5;
				break;
			case 4:
				m_iShaderPass += 4;
				break;
			}

			CJBaseBoss::Check_KillCount();

			m_bCheck_FirstDeadFlag = true;
			}
			break;
		case eBossMonsterPattern::Sleep:
			{
			m_fAddAniPlayTime = 0.f;
			m_bTargetLook = false;
			Delete_RigidBody();
			if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_MEETBOSS)
					Get_EventMgr()->Min_Event(EVENT_VOLATILE_STAGE_MEETBOSS);
	/*		if(m_bCheck_FirstDeadFlag)
			{
			if (m_iGold > 0 )
			{
				dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"))->Add_Souls(m_iGold);

				FAILED_CHECK_RETURN(Item_Drop(), E_FAIL);

				m_iGold = 0;
			}
			}*/
			}
			break;
		case eBossMonsterPattern::Other:
			{

			}
			break;
		case eBossMonsterPattern::End:
			m_eCurTotalPattern = m_ePreTotalPattern;
			return S_OK;
		default:
			m_eCurTotalPattern = m_ePreTotalPattern;
			return E_FAIL;
		}
		m_ePreTotalPattern = m_eCurTotalPattern;
	}
	return S_OK;
}

HRESULT CJBaseBoss::Do_TotalPattern(const _float & _fDeltaTime)
{
	CJBaseBoss::Do_Always(_fDeltaTime);
	switch (m_ePreTotalPattern)
	{
		//Ready Check
	case eBossMonsterPattern::Ready:
		CJBaseBoss::Do_Ready(_fDeltaTime);
		break;

	default:
	{
		//Activate Check

		switch (m_ePreTotalPattern)
		{
		case eBossMonsterPattern::Idle:
			if (m_bTargetLook)
				Target_Look(_fDeltaTime);
			Do_Idle(_fDeltaTime);
			break;
		case eBossMonsterPattern::Attack:
			if (m_bTargetLook)
				Target_Look(_fDeltaTime);
			Do_Attack(_fDeltaTime);
			break;
		case eBossMonsterPattern::Move:
			if (m_bTargetLook)
				Target_Look(_fDeltaTime);
			Do_Walk(_fDeltaTime);
			break;
		case eBossMonsterPattern::Hit:
			if (m_bTargetLook)
				Target_Look(_fDeltaTime);
			Do_Hit(_fDeltaTime);
			break;

		default:
		{
			//NonActivate Check
			switch (m_ePreTotalPattern)
			{
			case eBossMonsterPattern::Awake:
				Do_Activating(_fDeltaTime);
				break;
			case eBossMonsterPattern::Dead:
			case eBossMonsterPattern::Sleep:
			case eBossMonsterPattern::Other:
				Do_Other(_fDeltaTime);
				IsSleepDead();
				break;
			case eBossMonsterPattern::End:
				break;
			default:
				return E_FAIL;
			}
			break;
		}
		}
	}
	break;
	}

	return S_OK;
}

#pragma endregion

#pragma region TO_DO_FUCTION

HRESULT CJBaseBoss::OutClassCheck(const _float & _fDeltaTime)
{
#pragma region Check_PlayerDead

	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAYER_DEAD)
	{
		GET_MANAGEMENT->StopAll(CSound_Manager::ESoundID::End);
	}
	//else if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAYER_REVIVE)
	//{
	//	m_eCurTotalPattern = eBossMonsterPattern::Ready;
	//}
	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAYER_REVIVE)
	{
		m_eCurTotalPattern = eBossMonsterPattern::Sleep;
	}
#pragma endregion
#pragma region Check_Target

	CJBaseBoss::TargetCheck();

#pragma endregion

	return S_OK;
}

//항상 구동 되는 함수
void CJBaseBoss::Do_Always(const _float & _fDeltaTime)
{
	CJBaseBoss::OutClassCheck(_fDeltaTime);

	if (BOSS_CHECK_ACTIVATION)
	{
		Do_AfterActivation_Always(_fDeltaTime);

	}
}

//몬스터 활성화 후 항상 구동되는 함수, 휴면시 구동되지 않음.
void CJBaseBoss::Do_AfterActivation_Always(const _float & _fDeltaTime)
{
	m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

	if (m_fTargetDist > m_fFarRange &&
		BOSS_CHECK_ACTIVATION)
	{
		Get_EventMgr()->Min_Event(EVENT_VOLATILE_END_BOSS_CINE);
		m_eCurTotalPattern = eBossMonsterPattern::Sleep;
	}
	//거리가 멀어지면 슬립모드로 전환 해당부분 지워야 할 수도 있음.


	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::NoAlpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}

	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}

	Do_PhysxFunction(_fDeltaTime);
//	m_pBossUI->Set_Hp(m_tState.fHP, m_tState.fMaxHP);
	//m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

	//if (m_fTargetDist > m_fFindDistance)
	//{
	//	m_bTargeting = false;

		// Walk or Attack// To Idle State
		//if (m_ePreTotalPattern == eBossMonsterPattern::Attack ||
		//	m_ePreTotalPattern == eBossMonsterPattern::Move)
		//	m_eCurTotalPattern = eBossMonsterPattern::Idle;
	//}

	CJBaseBoss::CheckDead(_fDeltaTime);
}

//피직스 구동함수	활성화 시에만 구동됩니다.
void CJBaseBoss::Do_PhysxFunction(const _float & _fDeltaTime)
{
	if(m_bUpdatePhysx)
	{
		CJBaseBoss::MonsterData_Update();

		if (m_bApplyGravity)
		{
			_float3 vPos = Get_Manager()->Public_Gravity(m_tTransformDesc.szFileTitle, &m_bGravity, &m_bGravityStart, &m_vGravityPos, &m_fJumpTime, 10.f, _fDeltaTime);
			if (NULL != vPos)
				m_pTransform->Set_Pos(vPos);
		}
	}
}

//활성화 시 한번 실행되는 함수
void CJBaseBoss::Do_Activation(const _float & _fDeltaTime)
{
	CJBaseBoss::Create_RigidBody();

}

//활성화 진행 중일 때 호출됨 
void CJBaseBoss::Do_Activating(const _float & _fDeltaTime)
{
	/*
	 아마 시네마틱 관련 함수 추가 필요할 듯함.
	*/
	if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_END_BOSS_CINE) )
	{
		m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

		if (m_fTargetDist < m_fFarRange &&
			BOSS_CHECK_ACTIVATION)
		{
			m_eCurTotalPattern = eBossMonsterPattern::Idle;
		}
	}
	return;
}

//활성 대기 상태일 때 호출됩니다. 렌더 되기 직전의 상태입니다.
void CJBaseBoss::Do_Ready(const _float & _fDeltaTime)
{
	//m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

	if (m_fTargetDist < m_fFarRange &&
		m_ePreTotalPattern == eBossMonsterPattern::Ready)
	{
		m_eCurTotalPattern = eBossMonsterPattern::Awake;
	}
	//if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAY_BOSS_CINE)
	//	&& m_ePreTotalPattern == eBossMonsterPattern::Ready)
	//{
	//	m_eCurTotalPattern = eBossMonsterPattern::Awake;
	//}
}

#pragma endregion

#pragma region UTILITY
//체력을 체크하여 데드상태로 변경합니다.
void CJBaseBoss::CheckDead(const _float & _fDeltaTime)
{
	if (m_tState.fHP <= NULL)
	{
	
		m_eCurTotalPattern = eBossMonsterPattern::Dead;
	}
}

void CJBaseBoss::TargetCheck()
{
	_float3 vDir = m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos;

	m_fTargetDist = D3DXVec3Length(&vDir);
}

void CJBaseBoss::Target_Look(const _float & _fDeltaTime)
{
	_float3 vPlayerPos, vUp, vPos;
	memcpy(&vPlayerPos, &m_pTargetTransform->Get_TransformDesc().matWorld.m[3][0], sizeof(_float3));
	vPlayerPos.y = 0.f;
	vUp = { 0.f,1.f,0.f };
	m_pTransform->Get_Info(EInfo::Right, &m_vRight);
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	m_vLook.y = 0.f;
	vPos.y = 0.f;
	D3DXVec3Cross(&m_vRight, &m_vLook, &vUp);
	m_vRight.y = 0.f;
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	m_vLook = vPlayerPos - vPos;
	m_vLook *= -1.f;
	m_vLook.y = 0.f;
	D3DXVec3Normalize(&m_vLook, &m_vLook);

	_float3 vWomanBossLook;
	memcpy(&vWomanBossLook, &m_pTransform->Get_TransformDesc().matWorld.m[2][0], sizeof(_float3));
	vWomanBossLook.y = 0.f;
	D3DXVec3Normalize(&vWomanBossLook, &vWomanBossLook);

	_float3 vWomanBossRight;
	memcpy(&vWomanBossRight, &m_pTransform->Get_TransformDesc().matWorld.m[0][0], sizeof(_float3));
	vWomanBossRight.y = 0.f;
	D3DXVec3Normalize(&vWomanBossRight, &vWomanBossRight);

	_float fDot = D3DXVec3Dot(&vWomanBossLook, &m_vLook);
	_float fRight = D3DXVec3Dot(&vWomanBossRight, &m_vLook);

	_float Radian = acosf(fDot);

	if (Radian != Radian)
		return;

	if (fDot < 0)
	{
		if (fRight > 0) // 왼쪽 뒤
		{
			m_pTransform->Rotate(EAxis::Y, Radian, _fDeltaTime * m_tTransformDesc.fRotatePerSecond);
		}
		else			// 오른쪽 뒤
		{
			m_pTransform->Rotate(EAxis::Y, Radian, -_fDeltaTime * m_tTransformDesc.fRotatePerSecond);
		}
	}
	else
	{
		if (fRight > 0) // 오른쪽 앞
		{
			m_pTransform->Rotate(EAxis::Y, Radian, _fDeltaTime * m_tTransformDesc.fRotatePerSecond);
		}
		else			// 왼쪽 앞
		{
			m_pTransform->Rotate(EAxis::Y, Radian, -_fDeltaTime * m_tTransformDesc.fRotatePerSecond);
		}
	}
}

void CJBaseBoss::Back_To_Look(const _float & _fDeltaTime, _float3 _vPos)
{
	_float3 vUp, vPos;
	vPos.y = 0.f;
	vUp = { 0.f,1.f,0.f };
	m_pTransform->Get_Info(EInfo::Right, &m_vRight);
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	m_vLook.y = 0.f;
	vPos.y = 0.f;
	D3DXVec3Cross(&m_vRight, &m_vLook, &vUp);
	m_vRight.y = 0.f;
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	m_vLook = _vPos - vPos;
	m_vLook *= -1.f;
	m_vLook.y = 0.f;
	D3DXVec3Normalize(&m_vLook, &m_vLook);

	_float3 vWomanBossLook;
	memcpy(&vWomanBossLook, &m_pTransform->Get_TransformDesc().matWorld.m[2][0], sizeof(_float3));
	vWomanBossLook.y = 0.f;
	D3DXVec3Normalize(&vWomanBossLook, &vWomanBossLook);

	_float3 vWomanBossRight;
	memcpy(&vWomanBossRight, &m_pTransform->Get_TransformDesc().matWorld.m[0][0], sizeof(_float3));
	vWomanBossRight.y = 0.f;
	D3DXVec3Normalize(&vWomanBossRight, &vWomanBossRight);

	_float fDot = D3DXVec3Dot(&vWomanBossLook, &m_vLook);
	_float fRight = D3DXVec3Dot(&vWomanBossRight, &m_vLook);

	_float Radian = acosf(fDot);

	if (Radian != Radian)
		return;

	if (fDot < 0)
	{
		if (fRight > 0) // 왼쪽 뒤
		{
			m_pTransform->Rotate(EAxis::Y, Radian, _fDeltaTime * m_tTransformDesc.fRotatePerSecond);
		}
		else			// 오른쪽 뒤
		{
			m_pTransform->Rotate(EAxis::Y, Radian, -_fDeltaTime * m_tTransformDesc.fRotatePerSecond);
		}
	}
	else
	{
		if (fRight > 0) // 오른쪽 앞
		{
			m_pTransform->Rotate(EAxis::Y, Radian, _fDeltaTime * m_tTransformDesc.fRotatePerSecond);
		}
		else			// 왼쪽 앞
		{
			m_pTransform->Rotate(EAxis::Y, Radian, -_fDeltaTime * m_tTransformDesc.fRotatePerSecond);
		}
	}
}

void CJBaseBoss::CoolTimer(const _float & _fDeltaTime)
{
	if (m_bAttackCoolTime)
	{
		m_fAttackCoolTime += _fDeltaTime;

		if (m_fAttackCoolTime >= .5f)
		{
			m_bAttackCoolTime = false;
			m_fAttackCoolTime = 0.f;
		}
	}
}

HRESULT CJBaseBoss::Item_Drop()
{
	ITEMTRANSFORM_DESC tItemTransformDesc;
	m_pTransform->Get_Info(EInfo::Pos, &tItemTransformDesc.vPos);
	tItemTransformDesc.vPos.y += m_iItemCreateYAxisPosFix;
	tItemTransformDesc.vScale = OBJECT_SIZE;
	tItemTransformDesc.vRotate = { 0.f, 0.f, 0.f };

	wstring PrototypeName = m_tTransformDesc.szFileTitle;
	_uint count = (_uint)PrototypeName.find(L"_", 0);
	wstring wstrinteger = PrototypeName;
	wstrinteger.erase(0, count + 1);

	wstring wstrName = PrototypeName;
	wstrName.erase(count, PrototypeName.size());
	
	if(PrototypeName == L"Preto_0")
		tItemTransformDesc.ItemID = eItemCode::Soul_Preto;
	else if(PrototypeName == L"Malkihel_0")
		tItemTransformDesc.ItemID = eItemCode::Soul_Mal;
	else
	{
		ERROR_BOX(L"Obj is Not Boss");
	}
	wstring wstrPrototypeName = wstrName + L"_Item_" + wstrinteger;

	lstrcpy(tItemTransformDesc.szFileTitle, wstrPrototypeName.c_str());

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_Item",
		m_iSceneIdx,
		L"Layer_Map",
		&tItemTransformDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}
	return S_OK;
}

void CJBaseBoss::DeadParticle()
{
	_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
	_float3 vTargetPos = m_pTargetTransform->Get_TransformDesc().vPos;

	vCreatePos.y -= 1.f;

	_float4x4* pMeshBoneMatrix = nullptr;
	_ulong	ulongCount = 0;
	_float4x4 matWorld;

	m_pMesh->Get_AllBoneMetrix(&pMeshBoneMatrix, &ulongCount);
	_ulong i = Mersen_ToInt(0, ulongCount);
	//for (_ulong i = 0; i < ulongCount; ++i)
	//{
	
		matWorld = *(m_pTransform->Get_WorldMatrix());
		memcpy(vCreatePos, &(pMeshBoneMatrix[i] * matWorld).m[3][0], sizeof(_float3));

		_uint iDrawIdx = Mersen_ToInt(3, 5);
		CParticle_Manager::Get_Instance()->Generate_ExplosionParticle(
			m_iSceneIdx, vCreatePos, 0.f, &vTargetPos,
			1,m_vDissolveColor,
			//ePartilcePatternType::OnStratBlinkAndGoUp,
			(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp),

			iDrawIdx, 2.2f, D3DX_PI, 1.f);
//	}


}

void CJBaseBoss::MonsterData_Update()
{

	MyMonsterData* pMonsterData = (MyMonsterData*)Get_Manager()->GetData_Controller(m_tTransformDesc.szFileTitle);

	if (nullptr != pMonsterData)
	{
		if (pMonsterData->Collision)
		{
			Set_Hit(pMonsterData->Damage);
			pMonsterData->Collision = false;
			Get_Manager()->SetData_Controller(m_tTransformDesc.szFileTitle, pMonsterData);
		}

		if (pMonsterData->DropField)
		{
			// 들어올때
			m_bDropField = false;
		}
		else
		{
			// 나갈때
			m_bDropField = true;
		}
	}
}

void CJBaseBoss::Set_Hit(_float _fDamage)
{
	m_bHit = true;
	m_fHitDamage = _fDamage;
	if (m_tState.fHP > 0)
	{
		Get_Damaged();
		m_tState.fHP -= abs((m_tState.fDef * 0.25f) - m_fHitDamage);
		m_bHit = false;
	}

	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
	pCamera->CamShakerWeak();
}

void CJBaseBoss::Update_Init(const _float & _fDeltaTime)
{
	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::NoAlpha, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}
	if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::Shadow, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return;
	}
	m_bUpdateInit = true;
}

void CJBaseBoss::Create_RigidBody()
{
	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	wstring wstrTrigger = m_tTransformDesc.szFileTitle;
	m_bUpdatePhysx = true;
	if (wstrTrigger.find(L"Preto") != wstring::npos)
	{
		vPos.y += 4.5f;
		Get_Manager()->Add_Controller(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(2.f, 4.5f));

		wstring m_WepaonTitle_Weapon = (wstring)m_tTransformDesc.szFileTitle + L"_Sword_Weapon";
		wstring m_WepaonTitle_Shield = (wstring)m_tTransformDesc.szFileTitle + L"_Shield_Weapon";

		_float fDamage = m_tState.fAtt * 0.5f - m_tState.fDef * 0.25f;
		Get_Manager()->Add_Weapon(m_WepaonTitle_Weapon, vPos, fDamage, PxCapsuleGeometry(2.f, 4.f), 4.f);
		Get_Manager()->Add_Weapon(m_WepaonTitle_Shield, vPos, fDamage, PxCapsuleGeometry(2.f, 2.f), -1.f);
	}
	if (wstrTrigger.find(L"Malkhel") != wstring::npos)
	{
		//말키엘 몸체와 말키엘 무기 바디
		vPos.y += 2.f;
		Get_Manager()->Add_Controller(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(1.f, 2.f));

		CTransform* pWeaponTransform = dynamic_cast<CTransform*>(m_pWeapon->Get_Component(L"Com_Transform"));
		if (nullptr != m_pWeapon)
		{
			_float fDamage = m_pWeapon->Set_AtkDamage(m_tState.fAtt, m_tState.fDef);
			Get_Manager()->Add_Weapon(pWeaponTransform->Get_TransformDesc().szFileTitle, pWeaponTransform->Get_TransformDesc().vPos, fDamage, PxCapsuleGeometry(0.3f, 2.f), 0.f);
		}
	}
}

void CJBaseBoss::Check_KillCount()
{
	Get_EventMgr()->Add_KillCount_Boss();
}

void CJBaseBoss::IsSleepDead()
{
	if (m_eCurTotalPattern == eBossMonsterPattern::Ready &&
		m_ePreTotalPattern == eBossMonsterPattern::Sleep &&
		m_bCheck_FirstDeadFlag )
	{
		m_eCurTotalPattern = eBossMonsterPattern::Sleep;
	}
}

#pragma endregion

void CJBaseBoss::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pBossUI);
		Safe_Release(m_pWeapon);
	}

	CGameObject::Free();
}
