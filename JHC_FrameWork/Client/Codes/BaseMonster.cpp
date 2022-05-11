#include "stdafx.h"
#include "BaseMonster.h"
#include "Player_UI.h"
#include "Player.h"
#include "MainCamera.h"

CBaseMonster::CBaseMonster(_DEVICE _pDevice, const wstring& _PrototypeName /*= L""*/)
	:CGameObject(_pDevice)
	, m_pMesh(nullptr)
	, m_pTransform(nullptr)
	, m_pShader(nullptr)
	, m_pWeapon(nullptr)
	, m_vLook(0.f, 0.f, 0.f)
	, m_vRight(0.f, 0.f, 0.f)
	, m_vOriginPos()
	, m_vPatrolPos()
	, m_fChangeSpeed(0.f)
	, m_fDist(0.f)
	, m_fMoveTime(0.f)
	, m_fAttackCoolTime(0.f)
	, m_fHitCoolTime(0.f)
	, m_fDeathTime(0.f)
	, m_fPlayerAtt(0.f)
	, m_fDissolveTime(0.f)
	, m_fDissolveSpeed(0.f)
	, m_fMaxPatrolLength(0.f)
	, m_fNowPatrolLength(0.f)
	, m_fJumpTime(0.f)
	, m_bTarget(false)
	, m_bLook(false)
	, m_bHit(false)
	, m_bHitAble(false)
	, m_bAttackCoolTime(false)
	, m_bCinematicStart(false)
	, m_bCinematicEnd(true)
	, m_bPlayerDead(false)
	, m_bUIRender(false)
	, m_bNormalUIRender(false)
	, m_bAttackPattern(false)
	, m_bGravity(false)
	, m_bGraviStart(true)
	, m_bBossLook(false)
	, m_bBossCinematicAnimation(false)
	, m_bChangeGravity(true)
	, m_vGravityPos({ 0.f, 0.f, 0.f })
	, m_iNearPattern(0)
	, m_iFarPattern(0)
	, m_iMovePattern(0), m_iShaderPass(0)
	, m_iGold(0)
	, m_fFarRange(65.f)
	, m_fDeltaTime(0.f)
	, m_bFarRange(false)
	, m_bPatrolStart(false)
	, m_bPatrolEnd(false)
	, m_bDropField(false)
	, m_bReverseDissolve(true)
	, m_bPlayerWeapon(false)
	, m_bDelayStart(false)
	, m_bDelayEnd(false)
	, m_fDelayTime(0.f)
	, m_bReset(false)
	, m_bResetReady(false)
	, m_vResetPos(0.f, 0.f, 0.f)
	, m_fTimeMultiple(1.f)
{
	ZeroMemory(&m_tState, sizeof(STAT_DESC));
	m_PrototypeName = _PrototypeName;
}

CBaseMonster::CBaseMonster(const CBaseMonster & _rOther)
	:CGameObject(_rOther)
	, m_pMesh(_rOther.m_pMesh)
	, m_pTransform(_rOther.m_pTransform)
	, m_pShader(_rOther.m_pShader)
	, m_pWeapon(_rOther.m_pWeapon)
	, m_vLook(_rOther.m_vLook)
	, m_vRight(_rOther.m_vRight)
	, m_vOriginPos(_rOther.m_vOriginPos)
	, m_vPatrolPos(_rOther.m_vPatrolPos)
	, m_vGravityPos(_rOther.m_vGravityPos)
	, m_fChangeSpeed(_rOther.m_fChangeSpeed)
	, m_fDist(_rOther.m_fDist)
	, m_fMoveTime(_rOther.m_fMoveTime)
	, m_fAttackCoolTime(_rOther.m_fAttackCoolTime)
	, m_fHitCoolTime(_rOther.m_fHitCoolTime)
	, m_fDeathTime(_rOther.m_fDeathTime)
	, m_fPlayerAtt(_rOther.m_fPlayerAtt)
	, m_fDissolveTime(0.f)
	, m_fDissolveSpeed(0.f)
	, m_fMaxPatrolLength(_rOther.m_fMaxPatrolLength)
	, m_fNowPatrolLength(_rOther.m_fNowPatrolLength)
	, m_fJumpTime(_rOther.m_fJumpTime)
	, m_fFarRange(_rOther.m_fFarRange)
	, m_bTarget(_rOther.m_bTarget)
	, m_bLook(_rOther.m_bLook)
	, m_bHit(_rOther.m_bHit)
	, m_bHitAble(_rOther.m_bHit)
	, m_bAttackCoolTime(_rOther.m_bAttackCoolTime)
	, m_bCinematicStart(_rOther.m_bCinematicStart)
	, m_bCinematicEnd(_rOther.m_bCinematicEnd)
	, m_bPlayerDead(_rOther.m_bPlayerDead)
	, m_bUIRender(_rOther.m_bUIRender)
	, m_bNormalUIRender(_rOther.m_bNormalUIRender)
	, m_bAttackPattern(_rOther.m_bAttackPattern)
	, m_bGravity(_rOther.m_bGravity)
	, m_bGraviStart(_rOther.m_bGraviStart)
	, m_bBossLook(_rOther.m_bBossLook)
	, m_bBossCinematicAnimation(_rOther.m_bBossCinematicAnimation)
	, m_bChangeGravity(_rOther.m_bChangeGravity)
	, m_iNearPattern(_rOther.m_iNearPattern)
	, m_iFarPattern(_rOther.m_iFarPattern)
	, m_iMovePattern(_rOther.m_iMovePattern), m_iShaderPass(0)
	, m_iGold(_rOther.m_iGold)
	, m_fDeltaTime(_rOther.m_fDeltaTime)
	, m_bFarRange(_rOther.m_bFarRange)
	, m_bPatrolStart(_rOther.m_bPatrolStart)
	, m_bPatrolEnd(_rOther.m_bPatrolEnd)
	, m_bDropField(_rOther.m_bDropField)
	, m_bReverseDissolve(_rOther.m_bReverseDissolve)
	, m_bPlayerWeapon(_rOther.m_bPlayerWeapon)
	, m_bDelayEnd(_rOther.m_bDelayEnd)
	, m_fDelayTime(_rOther.m_fDelayTime)
	, m_bReset(_rOther.m_bReset)
	, m_bResetReady(_rOther.m_bResetReady)
	, m_vResetPos(_rOther.m_vResetPos)
	, m_fTimeMultiple(1.f)
{
	ZeroMemory(&m_tState, sizeof(STAT_DESC));
}

HRESULT CBaseMonster::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBaseMonster::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
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

	m_fDissolveSpeed = 3.f;

	auto pManagement = GET_MANAGEMENT;

	m_iGold = Mersen_ToInt(50, 100);
	
	//하단 보스 UI
	//m_tBackGroundDesc.vPos = { 20.f ,-300.f, 1.f };
	//m_tBackGroundDesc.vSize = { 900.f,120.f };
	//상단 보스 UI
	m_tBackGroundDesc.vPos = { 20.f ,300.f, 1.f };
	m_tBackGroundDesc.vSize = { 900.f,120.f };
	lstrcpy(m_tBackGroundDesc.szTexName, L"Component_Texture_BossGage");


	//하단 보스 피 게이지
	//m_tHpGageDesc.vPos = { 14.5f ,-314.f, 1.f };
	//m_tHpGageDesc.vSize = { 790.f ,10.f };
	//상단 보스 피 게이지
	m_tHpGageDesc.vPos = { 14.5f ,288.f, 1.f };
	m_tHpGageDesc.vSize = { 790.f ,10.f };
	lstrcpy(m_tHpGageDesc.szTexName, L"Component_Texture_Player_HP");

	if (FAILED(CBaseMonster::Add_Component()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;

	wstring wstrTrigger = m_tTransformDesc.szFileTitle;
	if (wstrTrigger.find(L"Preto") != wstring::npos)
	{
		vPos.y += 7.f;
		pManagement->Add_Controller(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(2.f, 9.f));
	}
	else
	{
		vPos.y += 1.5f;
		pManagement->Add_Controller(m_tTransformDesc.szFileTitle, vPos, EInteractionType::Capsule, PxCapsuleGeometry(0.7f, 2.f));
	}

	m_fChangeSpeed = m_tTransformDesc.fSpeedPerSecond;
	m_vOriginPos = m_tTransformDesc.vPos;

	return S_OK;
}

_uint CBaseMonster::Update_GameObject(const _float & _fDeltaTime)
{
	TargetCheck(_fDeltaTime);
	//CManagement* pManagement = GET_MANAGEMENT;
	m_fDeltaTime = _fDeltaTime;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	if (pPlayer->Get_FallDeath() || pPlayer->Get_Dead())
	{
		m_bPlayerDead = true;
		m_tState.fHP = m_tState.fMaxHP;
	}
	if (m_bReset)
	{
		if (m_bCinematicEnd)
			m_pTransform->Set_Pos(Get_Manager()->Set_GlobalPos(m_tTransformDesc.szFileTitle, m_vResetPos));
		m_bReset = false;
	}
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
	if (!m_bDelayEnd)
	{
		if (!m_bPlayerWeapon)
		{
			m_bPlayerWeapon = static_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"))->Get_Check_Weapon_Normal();
		}
		else
		{
			if (m_fDelayTime < 3.f)
			{
				m_fDelayTime += _fDeltaTime;
			}
			else
			{
				m_bNormalUIRender = true;
				m_bDelayEnd = true;
			}
		}
	}

	if (m_bFarRange&&m_bPlayerWeapon)
	{
		CGameObject::Update_GameObject(_fDeltaTime);

		if (m_fDissolveTime>0.f)
		{
		
				m_fDissolveSpeed = 1.f;
				if (m_iGold > 0)
				{
					static_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"))->Add_Souls(m_iGold);
					m_iGold = 0;
				}
				if (m_pWeapon)
					m_pWeapon->Set_Dissolve(true,0);

				if (m_iShaderPass == 4)
					m_iShaderPass += 4;
				else if (m_iShaderPass < 4)
					m_iShaderPass += 5;

		}

		m_pMonsterData = (MyMonsterData*)Get_Manager()->PhysXData_Update(m_tTransformDesc.szFileTitle);
		
		if (nullptr != m_pMonsterData)
		{
			if (m_pMonsterData->Collision)
			{
				Set_Hit(m_pMonsterData->Damage);
				m_pMonsterData->Collision = false;
				Get_Manager()->SetData_Controller(m_tTransformDesc.szFileTitle, m_pMonsterData);
			}

			if (m_pMonsterData->DropField)
			{
				// 들어올때
				m_bDropField = false;
				//m_pMonsterData->DropField = false;
				//pManagement->SetData_Controller(m_tTransformDesc.szFileTitle, m_pMonsterData);
			}
			else
			{
				// 나갈때
				m_bDropField = true;
			}

			if (m_bChangeGravity)
			{
				_float3 vPos = Get_Manager()->Public_Gravity(m_tTransformDesc.szFileTitle, &m_bGravity, &m_bGraviStart, &m_vGravityPos, &m_fJumpTime, 10.f, _fDeltaTime);
				m_pTransform->Set_Pos(vPos);
			}
		}		

		if (m_bBossLook)
		{
			if (m_bLook)
				BossLook(_fDeltaTime);
		}
		else
		{
			if (m_bTarget && m_bLook)
				Look(_fDeltaTime);
		}

		CBaseMonster::CoolTimer(_fDeltaTime);


		CBaseMonster::Drop_Dead();
		if (m_pWeapon != nullptr&&m_bDelayEnd)
			m_pWeapon->Set_Render(true);

		m_pTransform->Update_Transform();
		if (!m_bResetReady)
			m_bResetReady = true;
	}
	else
	{
		if (m_pWeapon != nullptr && m_bDelayEnd)
			m_pWeapon->Set_Render(false);
		if (m_bResetReady)
		{
			m_bReset = true;
			m_bResetReady = false;
			m_bUIRender = false;
		}
	}

	return m_iEvent;
}

HRESULT CBaseMonster::Render_GameObject()
{
	if (m_bBossCinematicAnimation)
		m_fDeltaTime *= 0.175f;
	else
		m_fDeltaTime *= m_fTimeMultiple;

	if (m_bFarRange&&m_bPlayerWeapon)
	{
		if (m_bReverseDissolve)
		{
			if (m_iShaderPass < 4)
				m_iShaderPass += 9;
			m_fDissolveTime += m_fDeltaTime*0.5f;
			if (m_fDissolveTime > 8.f)
			{
				m_bReverseDissolve = false;
				m_fDissolveTime = 0.f;
				if (m_iShaderPass > 8)
					m_iShaderPass -= 9;
			}
			m_fDissolveSpeed = .1f;
		}
	}
	if (!m_bDead)
		m_pMesh->Play_Animationset(m_fDeltaTime);
	else
		m_pMesh->Play_Animationset(0);
	
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

	//if (m_bFarRange)		////////////////이부분 때문에 렉걸림.
		m_pMesh->Render_Meshes(pEffect, 0);	//여기 뒤에 숫자 넣는걸로 디퓨즈등을 변경 가능

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

void CBaseMonster::Set_Hit(_float _fDamage)
{
	m_bHit = true;
	auto* pManagement = GET_MANAGEMENT;
	m_fPlayerAtt = _fDamage;


}

void CBaseMonster::Drop_Dead()
{
	_float3 vPos;
	auto* pManagement = GET_MANAGEMENT;
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	if (vPos.y < -50.f)
	{
		m_bDead = true;
		m_iEvent = (_int)EEvent::Dead;
		m_fDissolveSpeed = 1.f;
		if (m_iGold > 0)
		{
			CPlayer_UI* pPlayerUI = dynamic_cast<CPlayer_UI*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
			pPlayerUI->Add_Souls(m_iGold);
			m_iGold = 0;
		}
		if (m_pWeapon != nullptr)
			m_pWeapon->Set_Dead(true);
	}
}

void CBaseMonster::Set_Cinematic_Start(_bool _bCineStart)
{
	m_bCinematicStart = _bCineStart;
}

void CBaseMonster::Monster_Patrol()
{
	if (m_bPatrolStart)
	{
		//계산 이후 위치 지정
		_float4x4 matRot;
		
		D3DXMatrixIdentity(&matRot);
		
		_float3 vDir = { 0.f, 0.f, 1.f };
		
		_float fAngle = Mersen_ToFloat(0.f, 360.f);

		D3DXMatrixRotationY(&matRot, D3DXToRadian(fAngle));

		D3DXVec3TransformCoord(&vDir, &vDir, &matRot);

		m_vPatrolPos = m_vOriginPos + vDir * m_fNowPatrolLength;

		m_bPatrolStart = false;
		m_bPatrolEnd = false;
	}
	else
	{
		_float3 vPos;
		m_pTransform->Get_Info(EInfo::Pos, &vPos);
		if (D3DXVec3Length(&(vPos- m_vPatrolPos))<1.f)
		{
			m_bPatrolEnd = true;
		}
	}
	if (m_bPatrolEnd&&m_bPatrolStart)
	{

	}
}

HRESULT CBaseMonster::Add_Component()
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
	//m_tTransformDesc.vPos = {};
	/* For.Com_Transfrom */

	//임시 프레토 위치//
	/*if(!lstrcmp(m_tTransformDesc.szFileTitle,L"Preto_0"))
	{
		m_tTransformDesc.vPos = { -576.010559f, 209.935486f, 20.3497581f };
		m_tTransformDesc.vScale = { 0.03f,0.03f,0.03f };
	}*/
	//임시 프레토 위치//

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
		L"Shader_General_Dynamic",
		L"Shader_General_Dynamic",
		EComponentType::Static,
		(CComponent**)&m_pShader
	)))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
	auto pManagement = GET_MANAGEMENT;

	m_pTargetTransform = dynamic_cast<CTransform*>(pManagement->Get_Component(m_iSceneIdx, L"Layer_Player", L"Com_Transform"));


	return S_OK;
}

HRESULT CBaseMonster::SetUp_ConstantTable(_EFFECT & _pEffect)
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
	_pEffect->SetVector("g_vColor", &_float4(powf(1.f, 2.2f), powf(93/255.f, 2.2f), powf(48/255.f, 2.2f), 1.f));
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
	if (m_fDissolveTime>0.f)
	{
		pManagement->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture");
	}

	return S_OK;
}

void CBaseMonster::Look(const _float & _fDeltaTime)
{
	_float3 vPlayerPos, vUp;
	memcpy(&vPlayerPos, &m_pTargetTransform->Get_TransformDesc().matWorld.m[3][0], sizeof(_float3));
	vUp = { 0.f,1.f,0.f };
	m_pTransform->Get_Info(EInfo::Right, &m_vRight);
	D3DXVec3Cross(&m_vRight, &m_vLook, &vUp);
	D3DXVec3Normalize(&m_vRight, &m_vRight);

	m_vLook = vPlayerPos - m_pTransform->Get_TransformDesc().vPos;
	m_vLook *= -1.f;
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

	if (Radian != Radian || Radian <= 0.2f)
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


void CBaseMonster::BossLook(const _float & _fDeltaTime)
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

	if (Radian != Radian /*|| Radian <= 0.3f*/)
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

void CBaseMonster::TargetCheck(const _float & _fDeltaTime)
{
	_float3 vDir = m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos;
	
	m_fDist = D3DXVec3Length(&vDir);
	
	if (m_fDist<m_fFarRange)
		m_bFarRange = true;
	else
		m_bFarRange = false;

}


void CBaseMonster::CoolTimer(const _float & _fDeltaTime)
{
	if (m_bAttackCoolTime)
	{
		m_fAttackCoolTime += _fDeltaTime;

		if (m_fAttackCoolTime >= 1.f)
		{
			m_bAttackCoolTime = false;
			m_fAttackCoolTime = 0.f;
		}
	}	

	if (m_bDead)
	{
		if (m_fDeathTime == 0.f)
			DeadParticle();
		m_fDeathTime += _fDeltaTime;
		m_fDissolveTime += _fDeltaTime;
		if (m_pWeapon != nullptr)
			m_pWeapon->WeaponTrigger_Dead();

		if (m_fDeathTime >= 8.f)
		{
			m_iEvent = (_int)EEvent::Dead;
			m_fHitCoolTime = 0.f;
			if (m_pWeapon != nullptr)
				m_pWeapon->Set_Dead(true);
		}
	}
}

void CBaseMonster::DeadParticle()
{
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
			3, 
			//_ARGB(255, 255, 93, 48),
			_float4(1.f,0.364758f,0.188235f,1.f),
			//ePartilcePatternType::OnStratBlinkAndGoUp
			(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
			, iDrawIdx, 2.2f, D3DX_PI, 1.f);
	}


}


void CBaseMonster::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pWeapon);
	}
	CGameObject::Free();
}