#include "stdafx.h"
#include "JBaseMonster.h"
#include "Player_UI.h"
#include "Player.h"
#include "MainCamera.h"

#define MONSTER_CHECK_ACTIVATION (m_ePreTotalPattern == eMonsterPattern::Awake ||		\
m_ePreTotalPattern == eMonsterPattern::Walk ||											\
m_ePreTotalPattern == eMonsterPattern::Idle ||											\
m_ePreTotalPattern == eMonsterPattern::Hit ||											\
m_ePreTotalPattern == eMonsterPattern::Attack||											\
m_ePreTotalPattern == eMonsterPattern::Dead )

CJBaseMonster::CJBaseMonster(_DEVICE _pDevice, const wstring& _PrototypeName /*= L""*/)
	:CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr), m_pTargetTransform(nullptr)
	, m_pShader(nullptr)
	, m_pWeapon(nullptr)
	, m_bAttackCoolTime(false)
	, m_bTargetLook(false)
	, m_bApplyGravity(true)
	, m_bCheckHit(false)
	, m_bDissolve(false), m_bReverseDissolve(true)
	, m_fDeltaTime(0.f), m_fSkillMoveSpeed(0.f), m_fAttackCoolTime(0.f), m_fTargetDist(0.f), m_fWalkMoveTime(0.f), m_fDeathTime(0.f), m_fDelayTime(0.f)
	, m_fHitDamage(0.f), m_fAddAniPlayTime(0.f)
	, m_fDissolveTime(0.f), m_fDissolveSpeed(0.f)
	, m_iShaderPass(0), m_iDiffuseIndex(0)
	, m_iGold(0)
	, m_vRight(0.f, 0.f, 0.f), m_vLook(0.f, 0.f, 0.f)
	, m_bGravity(false), m_bGraviStart(true), m_fJumpTime(0.f), m_vGravityPos({ 0.f, 0.f, 0.f })
	, m_pMonsterData(nullptr)
	, m_bRenderOnce(false)
	, m_fFarRange(0.f)
	, m_bTargeting(false)
	, m_fFindDistance(35.f)
	, m_eCurTotalPattern(eMonsterPattern::End)
	, m_ePreTotalPattern(eMonsterPattern::End)
	, m_eParentCategory(eMonsterParent::Normal)
	, m_vDissolveColor(_float4()), m_vInitilizePos(_float3())
	, m_fSleepCountTime(0.f), m_fSleepTime_Const(0.f), m_fAdd_ActivateTimer(0.f)
	, m_bCheck_FirstDeadFlag(false), m_bDropItem(true)
	, m_matChildWeapon(_float4x4())
	, m_iCapacitySize(0)
	, m_iNowPatrol(1)
	, m_bPatrolReady(true)
	, m_fPatrolCoolTime(0.f)
	, m_bFriendPatrolReady(true)
	, m_fAttackCoolTimeEnd(0.f)
	, m_bTargetYLook(false)
	, m_fDamageMultiple(1.f)
	, m_pPlayer(nullptr)
	, m_bAttackSound(false)
	, m_bCamMode(false)
	, m_iSoundIndex(0)
	, m_eSound(CSound_Manager::ESoundID::End)
{
	ZeroMemory(&m_tState, sizeof(STAT_DESC));
	m_PrototypeName = _PrototypeName;
	m_vecPatrolPos.clear();
}

CJBaseMonster::CJBaseMonster(const CJBaseMonster & _rOther)
	:CGameObject(_rOther)
	, m_pMesh(_rOther.m_pMesh)
	, m_pTransform(_rOther.m_pTransform), m_pTargetTransform(nullptr)
	, m_pShader(_rOther.m_pShader)
	, m_pWeapon(_rOther.m_pWeapon)
	, m_bAttackCoolTime(false)
	, m_bTargetLook(false)
	, m_bApplyGravity(true)
	, m_bCheckHit(false)
	, m_bDissolve(false), m_bReverseDissolve(true)
	, m_fDeltaTime(0.f), m_fSkillMoveSpeed(0.f), m_fAttackCoolTime(0.f), m_fTargetDist(0.f), m_fWalkMoveTime(0.f), m_fDeathTime(0.f), m_fDelayTime(0.f)
	, m_fHitDamage(0.f), m_fAddAniPlayTime(0.f)
	, m_fDissolveTime(0.f), m_fDissolveSpeed(0.f)
	, m_iShaderPass(0), m_iDiffuseIndex(0)
	, m_iGold(0)
	, m_vRight(0.f, 0.f, 0.f), m_vLook(0.f, 0.f, 0.f)
	, m_bGravity(false), m_bGraviStart(true), m_fJumpTime(0.f), m_vGravityPos({ 0.f, 0.f, 0.f })
	, m_pMonsterData(nullptr)
	, m_bRenderOnce(false), m_bDropItem(true)
	, m_fFarRange(0.f)
	, m_bTargeting(false)
	, m_fFindDistance(35.f)
	, m_eCurTotalPattern(eMonsterPattern::End)
	, m_ePreTotalPattern(eMonsterPattern::End)
	, m_eParentCategory(eMonsterParent::Normal)
	, m_vDissolveColor(_float4()), m_vInitilizePos(_float3())
	, m_fSleepCountTime(0.f), m_fSleepTime_Const(0.f), m_fAdd_ActivateTimer(0.f)
	, m_iDropGoldMin(0), m_iDropGoldMax(0)
	, m_matChildWeapon(_float4x4())
	, m_bCheck_FirstDeadFlag(false)
	, m_iItemCreateYAxisPosFix(0.f)
	, m_iCapacitySize(0)
	, m_iNowPatrol(1)
	, m_bPatrolReady(true)
	, m_fPatrolCoolTime(0.f)
	, m_bFriendPatrolReady(true)
	, m_fAttackCoolTimeEnd(0.f)
	, m_bTargetYLook(false)
	, m_fDamageMultiple(1.f)
	, m_pPlayer(nullptr)
	, m_bAttackSound(false)
	, m_bCamMode(false)
	, m_iSoundIndex(0)
	, m_eSound(CSound_Manager::ESoundID::End)
{
	ZeroMemory(&m_tState, sizeof(STAT_DESC));
	m_vecPatrolPos.clear();
}

HRESULT CJBaseMonster::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJBaseMonster::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
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
	if (FAILED(CJBaseMonster::Add_Component()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}	
	m_fDamageMultiple = 1.f;
	m_iDropGoldMin = 50;
	m_iDropGoldMax = 100;
	m_iGold = Mersen_ToInt(m_iDropGoldMin, m_iDropGoldMax);	

	_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
	vCreatePos.y += 1.5f;

	*const_cast<_float3*>(&m_vInitilizePos) = vCreatePos;

	m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;	
	m_eCurTotalPattern = eMonsterPattern::Ready;
	m_fFarRange = 80.f;

	m_vDissolveColor = _float4(powf(1.f, 2.2f), powf(93 / 255.f, 2.2f), powf(48 / 255.f, 2.2f), 1.f);
	m_fActivateTimer = 4.f;
	m_fAdd_ActivateTimer = 1.1f;
	m_fAttackCoolTimeEnd = 1.f;
	return S_OK;
}

_uint CJBaseMonster::Update_GameObject(const _float & _fDeltaTime)
{
	m_fDeltaTime = _fDeltaTime;
	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
	if ((_uint)pCamera->Get_CamMode() > 1)
		m_bCamMode = true;
	else
		m_bCamMode = false;

	if (eMonsterParent::Normal == m_eParentCategory)
	{
		if (m_bUpdateInit)
		{
			CJBaseMonster::Do_TotalPattern(m_fDeltaTime);
			CJBaseMonster::PatternTotalControl(m_fDeltaTime);
		}
		else
			CJBaseMonster::Update_Init(m_fDeltaTime);
	}

	CGameObject::Update_GameObject(_fDeltaTime);
	m_pTransform->Update_Transform();

	return m_iEvent;
}

_uint CJBaseMonster::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (eMonsterParent::Normal == m_eParentCategory)
	{
		if (MONSTER_CHECK_ACTIVATION)
		{
			CJBaseMonster::CoolTimer(_fDeltaTime);

			CGameObject::LateUpdate_GameObject(_fDeltaTime);
		}
	}
	

	return m_iEvent;
}

HRESULT CJBaseMonster::Render_GameObject()
{
	if (MONSTER_CHECK_ACTIVATION)
		m_pMesh->Play_Animationset(m_fDeltaTime * m_fAddAniPlayTime);
	else
		m_pMesh->Play_Animationset(0);

	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();

	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	_uint iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	pEffect->BeginPass(m_iShaderPass);

	m_pMesh->Render_Meshes(pEffect, m_iDiffuseIndex, nullptr, true);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CJBaseMonster::Add_Component()
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

	m_pPlayer = dynamic_cast<CPlayer*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Player"));

	m_pTargetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(L"Com_Transform"));
	m_pPlayer->Get_WeaponParry();
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

HRESULT CJBaseMonster::SetUp_ConstantTable(_EFFECT & _pEffect)
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
	//255, 93, 48
	//218 122 0
	//_pEffect->SetVector("g_vColor", &_float4(2.2f, 0.13f, 0.0f, 2.f));
	_pEffect->SetVector("g_vColor", &m_vDissolveColor);
	//
	//_pEffect->SetVector("g_vColor", &_float4(powf(0.5490f,2.2f), powf(0.2156f,2.2f), powf(0.0274f, 2.2f), 1.f));
	//_pEffect->SetVector("g_vColor", &_float4(powf(1.f, 2.2f), powf(0.f, 2.2f), powf(0.f, 2.2f), 1.f));
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	//_pEffect->SetFloat("g_ftime", m_fDissolveTime);
	//_pEffect->SetFloat("g_fDissolveSpeed", 10.f);
	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	pManagement->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture");

	return S_OK;
}

HRESULT CJBaseMonster::PatternTotalControl(const _float& _fDeltaTime)
{
	if (m_eCurTotalPattern != m_ePreTotalPattern)
	{
		switch (m_eCurTotalPattern)
		{
		case eMonsterPattern::Ready:
		{
		
			//내부 변수 초기화 필요함
			m_tState.fHP = m_tState.fMaxHP;
			m_tState.fStamina = m_tState.fMaxStamina;
			m_tState.fPoise = m_tState.fPoise;
			m_fDissolveTime = 0.f;

			m_bTargetLook = false;
			m_bTargeting = false;
			m_bCheckHit = false;
			m_bGravity = false;
			m_bGraviStart = true;
			
			m_fJumpTime = 0.f;
			m_vGravityPos = { 0.f, 0.f, 0.f };

			m_vRight = { 0.f, 0.f, 0.f };
			m_vLook = { 0.f, 0.f, 0.f };
			//m_fSkillMoveSpeed(0.f), m_fAttackCoolTime(0.f), m_fTargetDist(0.f), m_fWalkMoveTime(0.f), m_fDeathTime(0.f), m_fDelayTime(0.f)
			 
			m_fHitDamage = 0.f;
			m_fDissolveSpeed = 0.f;
			m_fAddAniPlayTime = 1.3f;
			//m_iShaderPass(0), m_iDiffuseIndex(0)
				//, m_iDelayMovePattern(0)
			m_iGold = 0;
			m_pTransform->Set_Pos(m_vInitilizePos);
			if (m_iShaderPass > 4 && m_iShaderPass < 9)
				m_iShaderPass -= 5;
				//m_fJumpTime(0.f), m_vGravityPos({ 0.f, 0.f, 0.f })
				//, m_pMonsterData(nullptr)
				//, m_bRenderOnce(false)
				//, m_fFarRange(0.f)
				//, m_vResetPos(_float3(0.f, 0.f, 0.f))
				//, m_bReset(false)
				//, m_bResetReady(false)


		}
		break;
		case eMonsterPattern::Awake:
		{
			Do_Activation(_fDeltaTime);
		}
		break;
		case eMonsterPattern::Idle:
		{
			m_fDissolveTime = 0.f;

		if (m_iShaderPass > 8)
			m_iShaderPass -= 9;

		}
		break;
		case eMonsterPattern::Attack:
		{

		}
		break;
		case eMonsterPattern::Walk:
		{

		}
		break;
		case eMonsterPattern::Hit:
		{

		}
		break;
		case eMonsterPattern::Dead:
		{
			m_bTargetLook = false;
			m_fDissolveTime = 0.f;
			m_fDissolveSpeed = 1.f;
			if(m_bDropItem)
				m_iGold = Mersen_ToInt(m_iDropGoldMin, m_iDropGoldMax);
			if (m_iGold > 0)
			{
				dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"))->Add_Souls(m_iGold);
				if (m_pTransform->Get_TransformDesc().vPos.y >= -40.f)
				{
					if (Mersen_ToInt(1, 100) > 40)
					{		
						FAILED_CHECK_RETURN(Item_Drop(), E_FAIL);
					}
				}
				m_iGold = 0;
			}

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

			CJBaseMonster::Check_KillCount();
			DeadParticle();
			m_bCheck_FirstDeadFlag = true;
		}
		break;
		case eMonsterPattern::Sleep:
		{
			m_fAddAniPlayTime = 0.f;
			m_bTargetLook = false;
			CJBaseMonster::Delete_RigidBody();
		}
		break;
		case eMonsterPattern::Other:
		{

		}
			break;
		case eMonsterPattern::End:
		{
			m_eCurTotalPattern = m_ePreTotalPattern;
		}
		return S_OK;
		default:
			m_eCurTotalPattern = m_ePreTotalPattern;
			return E_FAIL;
		}
		m_ePreTotalPattern = m_eCurTotalPattern;
	}
	return S_OK;
}

HRESULT CJBaseMonster::Do_TotalPattern(const _float & _fDeltaTime)
{
	CJBaseMonster::Do_Always(_fDeltaTime);
	switch (m_ePreTotalPattern)
	{
		//Ready Check
		case eMonsterPattern::Ready:
			CJBaseMonster::Do_Ready(_fDeltaTime);
			break;

		default:
		{
			//Activate Check

			switch (m_ePreTotalPattern)
			{
			case eMonsterPattern::Idle:
				if (m_bTargetLook)
					Target_Look(_fDeltaTime);
				Do_Idle(_fDeltaTime);
				break;
			case eMonsterPattern::Attack:
				if (m_bTargetLook)
					Target_Look(_fDeltaTime);
				Do_Attack(_fDeltaTime);
				break;
			case eMonsterPattern::Walk:
				if (m_bTargetLook)
					Target_Look(_fDeltaTime);
				Do_Walk(_fDeltaTime);
				break;
			case eMonsterPattern::Hit:
				if (m_bTargetLook)
					Target_Look(_fDeltaTime);
				Do_Hit(_fDeltaTime);
				break;

			default:
				{
				//NonActivate Check
					switch (m_ePreTotalPattern)
					{
					case eMonsterPattern::Awake:
						Do_Activating(_fDeltaTime);
						break;
					case eMonsterPattern::Dead:
					case eMonsterPattern::Sleep:
					case eMonsterPattern::Other:
						Do_Other(_fDeltaTime);
						Check_SleepTime_Const();
						break;
					case eMonsterPattern::End:
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

void CJBaseMonster::Do_Always(const _float & _fDeltaTime)
{
	CJBaseMonster::OutClassCheck(_fDeltaTime);

	if (MONSTER_CHECK_ACTIVATION)
	{
		Do_AfterActivation_Always(_fDeltaTime);
	}
}

void CJBaseMonster::Do_AfterActivation_Always(const _float & _fDeltaTime)
{
	m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

	if (m_fTargetDist > m_fFarRange &&
		MONSTER_CHECK_ACTIVATION)
	{
		m_eCurTotalPattern = eMonsterPattern::Sleep;
	}
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

	m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

	if (m_fTargetDist > m_fFindDistance)
	{
		m_bTargeting = false;
		
		// Walk or Attack// To Idle State
		if(m_ePreTotalPattern == eMonsterPattern::Attack ||
			m_ePreTotalPattern == eMonsterPattern::Walk)
			m_eCurTotalPattern = eMonsterPattern::Idle;
	}

	CJBaseMonster::Drop_Dead();
	CJBaseMonster::CheckDead(_fDeltaTime);
}

HRESULT CJBaseMonster::OutClassCheck(const _float & _fDeltaTime)
{

#pragma region Check_PlayerDead

	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAYER_REVIVE)
	{
		m_eCurTotalPattern = eMonsterPattern::Sleep;
	}
	//else if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAYER_REVIVE)
	//{
	//	m_eCurTotalPattern = eMonsterPattern::Ready;
	//}

#pragma endregion
#pragma region Check_Target

	CJBaseMonster::TargetCheck();

#pragma endregion


	return S_OK;
}

void CJBaseMonster::Do_PhysxFunction(const _float & _fDeltaTime)
{
	CJBaseMonster::MonsterData_Update();

	if (m_bApplyGravity)
	{
		_float3 vPos = Get_Manager()->Public_Gravity(m_tTransformDesc.szFileTitle, &m_bGravity, &m_bGraviStart, &m_vGravityPos, &m_fJumpTime, 10.f, _fDeltaTime);
		if (NULL != vPos)
			m_pTransform->Set_Pos(vPos);
	}
}

void CJBaseMonster::Do_Activation(const _float & _fDeltaTime)
{
	CJBaseMonster::Create_RigidBody();
	m_fDissolveSpeed = 0.1f;

	if (m_iShaderPass < 4)
		m_iShaderPass += 9;
}

void CJBaseMonster::Do_Activating(const _float & _fDeltaTime)
{
	//m_fDissolveTime += m_fDeltaTime * 0.5f;	///Log 그래프로 수정 필요
	m_fDissolveTime += m_fDeltaTime * powf(m_fAdd_ActivateTimer,m_fDissolveTime);
	if (m_fDissolveTime > m_fActivateTimer)
	{

		m_eCurTotalPattern = eMonsterPattern::Idle;

		//m_bReverseDissolve = false;
	}

	m_tState.fHP = m_tState.fMaxHP;
}

void CJBaseMonster::Do_Ready(const _float & _fDeltaTime)
{
	m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

	if (m_fTargetDist < m_fFarRange &&
		m_ePreTotalPattern == eMonsterPattern::Ready &&
		(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAYER_LIFE))
	{
		m_eCurTotalPattern = eMonsterPattern::Awake;
	}
	/*else
		m_eCurTotalPattern = eMonsterPattern::Ready;*/
}

void CJBaseMonster::CheckDead(const _float & _fDeltaTime)
{
	if (m_tState.fHP <= NULL)
	{
		
		m_eCurTotalPattern = eMonsterPattern::Dead;
	}
}

void CJBaseMonster::Update_Init(const _float & _fDeltaTime)
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

void CJBaseMonster::Create_RigidBody()
{
	wstring wstrTrigger = m_tTransformDesc.szFileTitle;
	if (wstrTrigger.find(L"Homonculus") != wstring::npos)
	{
		_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
		vPos.y += 2.f;
		Get_Manager()->Add_Controller(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(1.f, 4.f));
	}
	else if (wstrTrigger.find(L"Yantari") != wstring::npos)
	{
		_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
		vPos.y += 1.f;
		Get_Manager()->Add_Controller(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(1.f, 2.f));
	}
	else if (wstrTrigger.find(L"Item") != wstring::npos)
	{
		_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
		vPos.y += 1.5f;
		Get_Manager()->Add_Controller(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(0.6f, 1.f));
	}
	else
	{
		_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
		vPos.y += 1.f;
		Get_Manager()->Add_Controller(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(0.7f, 2.f));
	}

	if (nullptr != m_pWeapon)
	{
		CTransform* pWeaponTransform = dynamic_cast<CTransform*>(m_pWeapon->Get_Component(L"Com_Transform"));
		_float fDamage = m_pWeapon->Set_AtkDamage(m_tState.fAtt, m_tState.fDef);

		wstring wstrWeaponName = pWeaponTransform->Get_TransformDesc().szFileTitle;
		if (wstrWeaponName.find(L"Yantari") != wstring::npos)
		{
			Get_Manager()->Add_Weapon(pWeaponTransform->Get_TransformDesc().szFileTitle, pWeaponTransform->Get_TransformDesc().vPos, fDamage, PxCapsuleGeometry(0.3f, 2.f), 0.f);
		}
		else
		{
			if(wstrWeaponName.find(L"Gun") != wstring::npos)
				Get_Manager()->Add_Weapon(pWeaponTransform->Get_TransformDesc().szFileTitle, pWeaponTransform->Get_TransformDesc().vPos, fDamage, PxCapsuleGeometry(0.2f, 2.f), 2.f, false);
			else
				Get_Manager()->Add_Weapon(pWeaponTransform->Get_TransformDesc().szFileTitle, pWeaponTransform->Get_TransformDesc().vPos, fDamage, PxCapsuleGeometry(0.2f, 2.f), 2.f);
		}		
	}
}


void CJBaseMonster::Delete_RigidBody()
{
	Get_Manager()->Delete_Controller(m_tTransformDesc.szFileTitle);
	if (m_pWeapon)
	{
		wstring wstrTemp = dynamic_cast<CTransform*>(m_pWeapon->Get_Component(L"Com_Transform"))->Get_TransformDesc().szFileTitle;
		Get_Manager()->Delete_Trigger(wstrTemp/* + L"_Weapon"*/);
	}
}

void CJBaseMonster::Check_KillCount()
{
	if (m_PrototypeName == L"Legionnaire")
	{
		Get_EventMgr()->Add_KillCount_Legionnaire();
	}
}

void CJBaseMonster::Check_SleepTime_Const()
{
	if (m_eCurTotalPattern == eMonsterPattern::Ready &&
		m_ePreTotalPattern == eMonsterPattern::Sleep &&
		m_bCheck_FirstDeadFlag && 
		m_fSleepTime_Const < 0.f)
	{
		m_eCurTotalPattern = eMonsterPattern::Sleep;
	}
}

void CJBaseMonster::TargetCheck()
{
	m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

	if (m_fTargetDist < m_fFindDistance)
	{
		m_bTargeting = true;
	}
	else
	{
		m_bTargeting = false;
	}
}

void CJBaseMonster::Target_Look(const _float & _fDeltaTime)
{
	_float3 vPlayerPos, vUp;
	memcpy(&vPlayerPos, &m_pTargetTransform->Get_TransformDesc().matWorld.m[3][0], sizeof(_float3));
	vUp = { 0.f,1.f,0.f };
	m_pTransform->Get_Info(EInfo::Right, &m_vRight);
	D3DXVec3Cross(&m_vRight, &m_vLook, &vUp);
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	m_vLook = m_pTransform->Get_TransformDesc().vPos - vPlayerPos;
	if (m_bTargetYLook)
	{
		m_vLook.y = 0.f;
	}
	D3DXVec3Normalize(&m_vLook, &m_vLook);

	_float3 vWomanBossLook;
	memcpy(&vWomanBossLook, &m_pTransform->Get_TransformDesc().matWorld.m[2][0], sizeof(_float3));
	D3DXVec3Normalize(&vWomanBossLook, &vWomanBossLook);

	_float3 vWomanBossRight;
	memcpy(&vWomanBossRight, &m_pTransform->Get_TransformDesc().matWorld.m[0][0], sizeof(_float3));
	D3DXVec3Normalize(&vWomanBossRight, &vWomanBossRight);

	_float fDot = D3DXVec3Dot(&vWomanBossLook, &m_vLook);
	_float fRight = D3DXVec3Dot(&vWomanBossRight, &m_vLook);

	_float Radian = acosf(fDot);

	if (m_bTargetYLook)
	{
		if (Radian != Radian)
			return;
	}
	else
	{
		if (Radian != Radian || Radian <= 0.2f)
			return;
	}


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

void CJBaseMonster::CoolTimer(const _float & _fDeltaTime)
{
	if (m_bAttackCoolTime)
	{
		m_fAttackCoolTime += _fDeltaTime;
		
		if (::CoolTime(_fDeltaTime, m_fAttackCoolTime, m_fAttackCoolTimeEnd))
		{
			m_bAttackCoolTime = false;
			m_fAttackCoolTime = 0.f;
		}
	}

	//if (m_bDissolve)
	//{
		//m_fDissolveTime += _fDeltaTime;

	//if (m_pWeapon != nullptr)
	//{
	//	m_pWeapon->Set_TriggerDead();
	//}
			
	//}

	//if (m_bDead)
	//{
	//	m_fDeathTime += _fDeltaTime;		
	//	
	//	if (::CoolTime(_fDeltaTime, m_fDeathTime, 3.f))
	//	{
	//		m_iEvent = (_int)EEvent::Dead;

	//		//if (m_pWeapon != nullptr)
	//		//	m_pWeapon->Set_Dead(true);
	//	}
	//}
}

void CJBaseMonster::MonsterData_Update()
{
	void* Data = Get_Manager()->GetData_Controller(m_tTransformDesc.szFileTitle);
	if (nullptr == Data)
		return;

	MyMonsterData* pMonsterData = (MyMonsterData*)Data;

	if (pMonsterData->Collision)
	{
		Set_Hit(pMonsterData->Damage * m_fDamageMultiple);
		pMonsterData->Collision = false;
		Get_Manager()->SetData_Controller(m_tTransformDesc.szFileTitle, (void*)pMonsterData);
	}
}

void CJBaseMonster::Set_Hit(_float _fDamage)
{
	m_bCheckHit = true;
	m_fHitDamage = _fDamage;

	if (m_tState.fHP > 0)
	{
		Get_Damaged();
		m_tState.fHP -= m_fHitDamage;
		m_bCheckHit = false;
	}

	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
	pCamera->CamShakerWeak();
}

void CJBaseMonster::Drop_Dead()
{
	_float3 vPos;
//	auto* pManagement = GET_MANAGEMENT;
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	if (vPos.y < -40.f)
	{
		m_tState.fHP = -1.f;
		m_bDead = true;
		//m_iEvent = (_int)EEvent::Dead;
		m_fDissolveSpeed = 1.f;
		if (m_iGold > 0)
		{
			CPlayer_UI* pPlayerUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
			NULL_CHECK_RETURN(pPlayerUI, );
			pPlayerUI->Add_Souls(m_iGold);
			m_iGold = 0;
		}

		//if (m_pWeapon != nullptr)
		//	m_pWeapon->Set_Dead(true);
	}
}

HRESULT CJBaseMonster::Item_Drop()
{
	ITEMTRANSFORM_DESC tItemTransformDesc;
	m_pTransform->Get_Info(EInfo::Pos, &tItemTransformDesc.vPos);

	if (m_PrototypeName.find(L"Homon") != wstring::npos)
	{
		tItemTransformDesc.vPos.y += 2.f;
	}

	tItemTransformDesc.vPos.y += m_iItemCreateYAxisPosFix;
	tItemTransformDesc.vScale = OBJECT_SIZE;
	tItemTransformDesc.vRotate = { 0.f, 0.f, 0.f };
	tItemTransformDesc.bJump = true;
	wstring PrototypeName = m_tTransformDesc.szFileTitle;
	_uint count = (_uint)PrototypeName.find(L"_", 0);
	wstring wstrinteger = PrototypeName;
	wstrinteger.erase(0, count + 1);

	wstring wstrName = PrototypeName;
	wstrName.erase(count, PrototypeName.size());

	_uint iCategory = Mersen_ToInt(0, 3);
	_uint iHeal = Mersen_ToInt(1, 3);
	_uint iStamina = Mersen_ToInt(1, 3);
	_uint iPoise = Mersen_ToInt(2, 3);
	_uint iItem = Mersen_ToInt(0, 3);

	switch (iCategory)
	{
	case 0: //Heal
		switch (iHeal)
		{
		case 1:
			tItemTransformDesc.ItemID = eItemCode::Heal_Medium;
			break;

		case 2:
			tItemTransformDesc.ItemID = eItemCode::Heal_Large;
			break;

		case 3:
			tItemTransformDesc.ItemID = eItemCode::Heal_Full;
			break;
		}
		break;
	case 1: //StaminaRegen
		switch (iStamina)
		{
	
		case 1:
			tItemTransformDesc.ItemID = eItemCode::StaminaRegen_Medium;
			break;

		case 2:
			tItemTransformDesc.ItemID = eItemCode::StaminaRegen_Large;
			break;

		case 3:
			tItemTransformDesc.ItemID = eItemCode::StaminaRegen_Full;
			break;
		}
		break;
	case 2: //Poise
		switch (iPoise)
		{
		case 0:
			tItemTransformDesc.ItemID = eItemCode::Poise_Small;
			break;

		case 1:
			tItemTransformDesc.ItemID = eItemCode::Poise_Medium;
			break;

		case 2:
			tItemTransformDesc.ItemID = eItemCode::Poise_Large;
			break;

		case 3:
			tItemTransformDesc.ItemID = eItemCode::Poise_Full;
			break;
		}
		break;
	case 3: //Soul
		switch (iItem)
		{
		case 0:
			tItemTransformDesc.ItemID = eItemCode::Soul_Small_1;
			break;

		case 1:
			tItemTransformDesc.ItemID = eItemCode::Soul_Small_2;
			break;

		case 2:
			tItemTransformDesc.ItemID = eItemCode::Soul_Medium;
			break;

		}
		break;
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
		m_iGold = 0;
		return E_FAIL;
	}
	return S_OK;
}

void CJBaseMonster::DeadParticle()
{
	if (!m_pMesh)
		return;
	_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
	_float3 vTargetPos = m_pTargetTransform->Get_TransformDesc().vPos;

	vCreatePos.y -= 1.f;

	_float4x4* pMeshBoneMatrix = nullptr;
	_ulong	ulongCount = 0;
	_float4x4 matWorld;

	m_pMesh->Get_AllBoneMetrix(&pMeshBoneMatrix, &ulongCount);
	for (_ulong i = 0; i < ulongCount; ++i)
	{
		matWorld = *(m_pTransform->Get_WorldMatrix());
		memcpy(vCreatePos, &(pMeshBoneMatrix[i] * matWorld).m[3][0], sizeof(_float3));

		_uint iDrawIdx = Mersen_ToInt(3, 5);
		CParticle_Manager::Get_Instance()->Generate_ExplosionParticle(
			m_iSceneIdx, vCreatePos, 0.f, &vTargetPos,
			3, m_vDissolveColor
			//,ePartilcePatternType::OnStratBlinkAndGoUp
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
			, iDrawIdx, 2.2f, D3DX_PI, 1.f);
	}
}

_bool CJBaseMonster::Patrol_Look(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	m_vLook = *m_vecPatrolPos[m_iNowPatrol] - m_pTransform->Get_TransformDesc().vPos;

	if (D3DXVec3Length(&m_vLook) > 1.f)
	{
		m_vLook *= -1.f;
		D3DXVec3Normalize(&m_vLook, &m_vLook);

		_float3 vMonsterLook;
		memcpy(&vMonsterLook, &m_pTransform->Get_TransformDesc().matWorld.m[2][0], sizeof(_float3));
		D3DXVec3Normalize(&vMonsterLook, &vMonsterLook);

		_float3 vMonsterRight;
		memcpy(&vMonsterRight, &m_pTransform->Get_TransformDesc().matWorld.m[0][0], sizeof(_float3));
		D3DXVec3Normalize(&vMonsterRight, &vMonsterRight);

		_float fDot = D3DXVec3Dot(&vMonsterLook, &m_vLook);
		_float fRight = D3DXVec3Dot(&vMonsterRight, &m_vLook);

		_float Radian = acosf(fDot);

		if (Radian != Radian)
			return false;

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


		return false;
	}
	else

	return true;
}

HRESULT CJBaseMonster::Add_Friend(_uint iFriendIndex, const wstring & _PrototypeName)
{
	auto pManagement = GET_MANAGEMENT;
	TCHAR szMonsterName[MAX_PATH] = L"";
	if (!lstrcmpi(_PrototypeName.c_str(), L"Legionnaire"))
	{
		swprintf_s(szMonsterName, L"Legionnaire_%d", iFriendIndex);

	}
	if (!lstrcmpi(_PrototypeName.c_str(), L"Yantari"))
	{
		swprintf_s(szMonsterName, L"Yantari_%d", iFriendIndex);
	}

	CJBaseMonster* pMon = (CJBaseMonster*)pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, szMonsterName);
	m_vecFriendMonster.emplace_back(pMon);

	return S_OK;
}

void CJBaseMonster::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pWeapon);
		for (auto& rPos : m_vecPatrolPos)
		{
			Safe_Delete(rPos);
		}
		m_vecPatrolPos.clear();
		m_vecPatrolPos.shrink_to_fit();
	}

	CGameObject::Free();
}