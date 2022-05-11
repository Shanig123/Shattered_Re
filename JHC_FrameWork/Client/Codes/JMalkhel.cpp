#include "stdafx.h"
#include "JMalkhel.h"
#include "Player_UI.h"
#include "JMalkhel_FireBall.h"
#include "JMalkhel_FireBallBack.h"
#include "JMalkhel_Spear.h"
#include "JMalkhel_Effect_360.h"
#include "CinematicCamera_0.h"
#include "Effect_Texture.h"
#include "Effect_Texture_Ani.h"

#define BOSS_CHECK_ACTIVATION (m_ePreTotalPattern == eBossMonsterPattern::Awake ||		\
m_ePreTotalPattern == eBossMonsterPattern::Move ||											\
m_ePreTotalPattern == eBossMonsterPattern::Idle ||											\
m_ePreTotalPattern == eBossMonsterPattern::Hit ||											\
m_ePreTotalPattern == eBossMonsterPattern::Attack||											\
m_ePreTotalPattern == eBossMonsterPattern::Dead )

CJMalkhel::CJMalkhel(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CJBaseBoss(_pDevice, _PrototypeName)
	, m_ePattern(PATTERN::Pattern_Ready)
	, m_pBossUI(nullptr)
	, m_vTeleportPos(0.f, 0.f, 0.f)
	, m_fAppearDelayTime(0.f)
	, m_iAttackCount(0)
	, m_bFarPattern1(false)
	, m_bFarPattern2(false)
	, m_bFarPattern3(false)
	, m_bNearPattern2(false)
	, m_bNearPattern3(false)
	, m_fFarPattern1Time(0.f)
	, m_fFarPattern2Time(0.f)
	, m_fFarPattern3Time(0.f)
	, m_fNearPattern2Time(0.f)
	, m_fNearPattern3Time(0.f)
	, m_bNear2Teleport(false)
	, m_bPhase2On(false)
	, m_fNearPattern2TeleportTime(0.f)
	, m_fWalkFrontAccelTime(0.f)
	, m_fTargetDistHalf(0.f)
	, m_bAttackSound2(false)
	, m_iCinemaEndCount(false)
	, m_vOriginRotate(0.f, 0.f, 0.f)
	, m_fDistortionTime(0.f), m_fLightTime(0.f)
	, m_pTeleportDistortionTex(nullptr), m_pLightEffect(nullptr)
{	
	for (_uint i = 0; i < 3; ++i)
	{
		m_pFireBallObject[i] = nullptr;
		m_bFireBallSkillStart[i] = false;
	}

	m_pFireBallBackObject = nullptr;

	for (_uint i = 0; i < 12; ++i)
		m_pSpearObject[i] = nullptr;

	m_pEffect360 = nullptr;

	m_bSkillStart = false;
	m_bFireBallBackStart = false;
	m_bFireBallBackEnd = false;
	m_bEffect360Start = false;
	m_bEffect360End = false;
}

CJMalkhel::CJMalkhel(const CJMalkhel & _rOther)
	: CJBaseBoss(_rOther)
	, m_ePattern(PATTERN::Pattern_Ready)
	, m_pBossUI(_rOther.m_pBossUI)
	, m_vTeleportPos(_rOther.m_vTeleportPos)
	, m_fAppearDelayTime(0.f)
	, m_iAttackCount(0)
	, m_bFarPattern1(false)
	, m_bFarPattern2(false)
	, m_bFarPattern3(false)
	, m_bNearPattern2(false)
	, m_bNearPattern3(false)
	, m_fFarPattern1Time(0.f)
	, m_fFarPattern2Time(0.f)
	, m_fFarPattern3Time(0.f)
	, m_fNearPattern2Time(0.f)
	, m_fNearPattern3Time(0.f)
	, m_bNear2Teleport(false)
	, m_bPhase2On(false)
	, m_fNearPattern2TeleportTime(0.f)
	, m_fWalkFrontAccelTime(0.f)
	, m_fTargetDistHalf(0.f)
	, m_bAttackSound2(false)
	, m_iCinemaEndCount(false)
	, m_vOriginRotate(0.f, 0.f, 0.f)
	, m_fDistortionTime(0.f), m_fLightTime(0.f)
	, m_pTeleportDistortionTex(nullptr), m_pLightEffect(nullptr)
{
	for (_uint i = 0; i < 3; ++i)
	{
		m_pFireBallObject[i] = nullptr;
		m_bFireBallSkillStart[i] = false;
	}

	m_pFireBallBackObject = nullptr;

	for (_uint i = 0; i < 12; ++i)
		m_pSpearObject[i] = nullptr;

	m_pEffect360 = nullptr;

	m_bSkillStart = false;
	m_bFireBallBackStart = false;
	m_bFireBallBackEnd = false;
	m_bEffect360Start = false;
	m_bEffect360End = false;
}

HRESULT CJMalkhel::Ready_GameObject_Prototype()
{
	if (FAILED(CJBaseBoss::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CJMalkhel::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseBoss::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(CJMalkhel::Weapon_Clone()))
	{
		ERROR_BOX(L"Failed to Weapon_Clone");
		return E_FAIL;
	}

	if (FAILED(CJMalkhel::Skill_Clone()))
	{
		ERROR_BOX(L"Failed to Skill_Clone");
		return E_FAIL;
	}	

	if (FAILED(CJMalkhel::Effect_Clone()))
	{
		ERROR_BOX(L"Failed to Effect_Clone");
		return E_FAIL;
	}

	if (FAILED(CJMalkhel::UI_Clone()))
	{
		ERROR_BOX(L"Failed to UI_Clone");
		return E_FAIL;
	}

	m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);

	m_iShaderPass = 4;

	m_tState.fAtt = 60.f;
	m_tState.fDef = 35.f;
	m_tState.fMaxHP = 2000.f;
	m_tState.fHP = m_tState.fMaxHP;
	m_fAddAniPlayTime = 1.5f;

	m_fSkillMoveSpeed = 13.f;

	m_iDissolveTexture = 0;

	*const_cast<_float3*>(&m_vInitilizePos) = m_pTransform->Get_TransformDesc().vPos;
	m_vOriginRotate = m_pTransform->Get_TransformDesc().vRotate;


	m_bApplyGravity = false;
	

	if (FAILED(CJMalkhel::Clone_Trail()))
	{
		ERROR_BOX(L"Failed to Clone_Trail");
		return E_FAIL;
	}
	if (FAILED(CJMalkhel::Clone_Distortion()))
	{
		ERROR_BOX(L"Failed to Clone_Distortion");
		return E_FAIL;
	}
	if (FAILED(CJMalkhel::Clone_LightEffect()))
	{
		ERROR_BOX(L"Failed to Clone_LightEffect");
		return E_FAIL;
	}
	m_vDissolveColor = _float4(*m_pWeapon->Get_TrailColor(),1.f);
	return S_OK;
}

_uint CJMalkhel::Update_GameObject(const _float & _fDeltaTime)
{

	//if (m_pWeapon)
	//	m_pWeapon->Trail_Switch(true);
	CJBaseBoss::Update_GameObject(_fDeltaTime);
	if (BOSS_CHECK_ACTIVATION)
	{
		CJMalkhel::AttackCoolTimer(_fDeltaTime);
		for (_uint i = 0; i < 6; ++i)
		{
			m_pArrWingDistort_TrailEffect[i]->Set_RenderOnOff(true);
		}
		Fix_DistortionTrailPos(_fDeltaTime);
		DisortionScaleFix(_fDeltaTime);
		LightCoolTime(_fDeltaTime);
	}
	else
	{
		m_pLightEffect->Set_RenderSwitch(false);
		for (_uint i = 0; i < 6; ++i)
		{
			m_pArrWingDistort_TrailEffect[i]->Set_RenderOnOff(false);
		}
	}

	if (GET_MANAGEMENT->Key_Down(DIK_NUMPAD0))
	{
		m_tState.fHP -= m_tState.fMaxHP*0.2f;
	}

	//if (m_pWeapon)
	//	m_pWeapon->Trail_Switch(true);
	return m_iEvent;
}

HRESULT CJMalkhel::Render_GameObject()
{
	m_pMesh->Play_Animationset(m_fDeltaTime * m_fAddAniPlayTime);

	CJBaseBoss::Render_GameObject();

	const MY_FRAME* pMonsterFrame = m_pMesh->Get_FrameByName("Malkhel-R-Finger2");
	m_matChildWeapon = pMonsterFrame->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();

	const MY_FRAME* pMonsterFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingSup_R02");
	_float4x4 Wing = pMonsterFrame->CombinedTranformationMatrix;
	Wing = pMonsterFrame->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();

	return S_OK;
}

void CJMalkhel::Do_Activating(const _float & _fDeltaTime)
{
	if ((Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_STAGE2_BOSS_START) &&
		!(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_SKIP_BOSS_CINE))
		Appear_Pattern(_fDeltaTime);
	else
	{
		if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Idle)
		{
			m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
			m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));
			if (m_bPhase2On)
			{
				m_bPhase2On = false;
			}
			if (m_fTargetDist < 40.f &&
				BOSS_CHECK_ACTIVATION)
			{
				if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_SKIP_BOSS_CINE))
				{
					GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::BGM);
					GET_MANAGEMENT->PlayBGM(L"MalKhel_Theme.wav", BGM_VOLUME);
				}

				m_eCurTotalPattern = eBossMonsterPattern::Idle;
			}
		}
		else
			m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
	}
	CJBaseBoss::Do_Activating(_fDeltaTime);
}

void CJMalkhel::Do_Idle(const _float & _fDeltaTime)
{
	m_bTargetLook = true;

	if (!m_bAttackCoolTime)
	{
		m_eCurTotalPattern = eBossMonsterPattern::Attack;
		m_ePattern = PATTERN::Pattern_Ready;
	}
}

void CJMalkhel::Do_Attack(const _float & _fDeltaTime)
{
	m_bApplyGravity = false;
	if (m_ePattern != PATTERN::Pattern_Ready)
	{
		switch (m_ePattern)
		{
		case PATTERN::Near_1:
			Near_Pattern_1(_fDeltaTime);
			break;
		case PATTERN::Near_2:
			Near_Pattern_2(_fDeltaTime);
			break;
		case PATTERN::Near_3:
			Near_Pattern_3(_fDeltaTime);
			break;
		case PATTERN::Near_4:
			Near_Pattern_4(_fDeltaTime);
			break;
		case PATTERN::Far_1:
			Far_Pattern_1(_fDeltaTime);
			break;
		case PATTERN::Far_2:
			Far_Pattern_2(_fDeltaTime);
			break;
		case PATTERN::Far_3:
			Far_Pattern_3(_fDeltaTime);
			break;
		case PATTERN::Walk_F:
			Move_Pattern_F(_fDeltaTime);
			break;
		case PATTERN::Walk_L:
			Move_Pattern_L(_fDeltaTime);
			break;
		case PATTERN::Walk_R:
			Move_Pattern_R(_fDeltaTime);
			break;
		case PATTERN::Walk_B:
			Move_Pattern_B(_fDeltaTime);
			break;
		case PATTERN::BackTeleport:
			BackMove_Pattern_Teleport(_fDeltaTime);
			break;
		case PATTERN::FrontTeleport:
			FrontMove_Pattern_Teleport(_fDeltaTime);
			break;
		case PATTERN::Death:
			Death_Pattern(_fDeltaTime);
			break;
		}
	}
	else
	{
		if (m_fTargetDist <= 5.f)
		{
			if (!m_bAttackCoolTime)
			{
				_uint iNearPattern = 0;
				if (m_iAttackCount > 2)
				{
					iNearPattern = 5;
				}
				else
				{
					iNearPattern = Mersen_ToInt(0, 4);
				}
				switch (iNearPattern)
				{
				case 0:
					if (m_bPhase2On)
					{
						Near_Pattern_4(_fDeltaTime);
						m_iAttackCount++;
					}
					break;
				case 1:
					if (!m_bNearPattern2)
					{
						Near_Pattern_2(_fDeltaTime);
						m_iAttackCount++;
						break;
					}
				case 2:
					if (!m_bNearPattern3)
					{
						Near_Pattern_3(_fDeltaTime);
						m_iAttackCount++;
						break;
					}
				case 3:
					if (m_bPhase2On&& !m_bFarPattern3 && m_iAttackCount != 0)
					{
						m_bFar3Teleport = true;
						Far_Pattern_3(_fDeltaTime);
						break;
					}
				case 4:
					Near_Pattern_1(_fDeltaTime);
					m_iAttackCount++;
					break;
				default:
					m_eCurTotalPattern = eBossMonsterPattern::Move;
					break;
				}

			}
		}
		else if (m_fTargetDist > 5.f && m_fTargetDist <= 27.f)
		{
			if (!m_bAttackCoolTime)
			{
				switch (Mersen_ToInt(0, 3))
				{
				case 0:
					if (!m_bFarPattern1)
					{
						Far_Pattern_1(_fDeltaTime);
						break;
					}
				case 1:
					if (!m_bFarPattern2)
					{
						Far_Pattern_2(_fDeltaTime);
						break;
					}
				case 2:
					if (m_bPhase2On&& !m_bFarPattern3)
					{
						m_bFar3Teleport = false;
						Far_Pattern_3(_fDeltaTime);
						break;
					}
				default:
					m_eCurTotalPattern = eBossMonsterPattern::Move;
					if (m_bPhase2On)
					{
						if (Mersen_ToInt(0, 1))
						{
							m_pMesh->Set_Animationset((_uint)MalkihelAniID::Move_Front);
							m_ePattern = PATTERN::Walk_F;
						}
						else
						{
							m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
							m_ePattern = PATTERN::FrontTeleport;
						}
					}
					else
					{
						m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
						m_ePattern = PATTERN::Walk_F;
					}
					break;
				}
			}
		}
		else
		{
			m_eCurTotalPattern = eBossMonsterPattern::Move;
		}
	}
}

void CJMalkhel::Do_Hit(const _float & _fDeltaTime)
{
}

void CJMalkhel::Do_Walk(const _float & _fDeltaTime)
{
	m_bApplyGravity = true;
	if (m_ePattern != PATTERN::Pattern_Ready)
	{
		switch (m_ePattern)
		{
		case CJMalkhel::PATTERN::Walk_F:
			Move_Pattern_F(_fDeltaTime);
			break;
		case CJMalkhel::PATTERN::Walk_B:
			Move_Pattern_B(_fDeltaTime);
			break;
		case CJMalkhel::PATTERN::Walk_L:
			Move_Pattern_L(_fDeltaTime);
			break;
		case CJMalkhel::PATTERN::Walk_R:
			Move_Pattern_R(_fDeltaTime);
			break;
		case CJMalkhel::PATTERN::BackTeleport:
			BackMove_Pattern_Teleport(_fDeltaTime);
			break;
		case CJMalkhel::PATTERN::FrontTeleport:
			FrontMove_Pattern_Teleport(_fDeltaTime);
			break;
		}
	}
	else
	{

		if (5.f >= m_fTargetDist)
		{
			if (m_iAttackCount > 2)
			{
				if (m_bPhase2On)
				{
					m_iDelayMovePattern = Mersen_ToInt(2, 3);
				}
				else
				{
					m_iDelayMovePattern = 2;
				}
				m_iAttackCount = 0;
			}
			else
			{
				m_iDelayMovePattern = 6;
				m_eCurTotalPattern = eBossMonsterPattern::Attack;
			}
		}
		else if (5.f < m_fTargetDist && 7.f >= m_fTargetDist)
		{
			if (m_bPhase2On)
			{
				m_iDelayMovePattern = Mersen_ToInt(0, 1);
			}
			else
			{
				m_iDelayMovePattern = 0;
			}
		}
		else if (7.f < m_fTargetDist && 12.f >= m_fTargetDist)
			m_iDelayMovePattern = Mersen_ToInt(4, 5);
		else if (m_fTargetDist > 12.f)
		{
			if (m_bPhase2On)
				m_iDelayMovePattern = Mersen_ToInt(0, 1);
			else
				m_iDelayMovePattern = 0;
		}

		if (!m_bAttackCoolTime)
		{
			switch (m_iDelayMovePattern)
			{
			case 0:
				Move_Pattern_F(_fDeltaTime);
				break;
			case 1:
				FrontMove_Pattern_Teleport(_fDeltaTime);
				break;
			case 2:
				Move_Pattern_B(_fDeltaTime);
				break;
			case 3:
				BackMove_Pattern_Teleport(_fDeltaTime);
				break;
			case 4:
				Move_Pattern_R(_fDeltaTime);
				break;
			case 5:
				Move_Pattern_L(_fDeltaTime);
				break;
			default:
				break;
			}
		}
	}
}

void CJMalkhel::Do_Other(const _float & _fDeltaTime)
{
	if (m_ePreTotalPattern == eBossMonsterPattern::Sleep)
	{
		m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));
		if (m_fTargetDist < m_fFarRange &&
			m_ePreTotalPattern == eBossMonsterPattern::Sleep)
		{
			m_eCurTotalPattern = eBossMonsterPattern::Ready;
		}
	}
	if (m_ePreTotalPattern == eBossMonsterPattern::Dead)
	{
		Death_Pattern(_fDeltaTime);
		for (_uint i = 0; i < 6; ++i)
			m_pArrWingDistort_TrailEffect[i]->Set_RenderOnOff(false);
	}
}

void CJMalkhel::Delete_RigidBody()
{
	Get_Manager()->Delete_Controller(m_tTransformDesc.szFileTitle);
	if (m_pWeapon)
	{
		wstring wstrTemp = static_cast<CTransform*>(m_pWeapon->Get_Component(L"Com_Transform"))->Get_TransformDesc().szFileTitle;
		Get_Manager()->Delete_Trigger(wstrTemp/* + L"_Weapon"*/);
	}
}

void CJMalkhel::Appear_Pattern(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Appear;
		m_pMesh->Set_Animationset((_uint)MalkihelAniID::Fly_Away);
	}
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Fly_Away)
	{
		m_bTargetLook = false;

		if (m_pMesh->Is_AnimationSetEnd())
		{
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::BGM);
			GET_MANAGEMENT->PlayBGM(L"MalKhel_Theme.wav", BGM_VOLUME);
			m_bAppearEnd = true;
			m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
			m_eCurTotalPattern = eBossMonsterPattern::Idle;
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_END_BOSS_CINE | EVENT_VOLATILE_SKIP_BOSS_CINE);
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
		}
	}
}

void CJMalkhel::Far_Pattern_1(const _float & _fDeltaTime)
{
	// 창으로 바닥찍기. 이 패턴에서는 플레이어 기준으로 원형으로 칼날 생성 이후 칼날이 발사. 언더테일 플라위가 쓰는 패턴
	if (m_pMesh->Get_Animationset() == (_uint) MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Far_1;
		m_pMesh->Set_Animationset((_uint) MalkihelAniID::Attack1);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bAttackSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint) MalkihelAniID::Attack1)
	{
		m_bTargetLook = false;
		if (m_pMesh->Get_AnimationFrame(68, 28))
		{
			if (m_bAttackSound)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Malkhel_Far_1.ogg", CSound_Manager::ESoundID::Monster_Weapon3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
				m_bAttackSound = false;
			}
			if (!m_bSkillStart)
			{
				_float fSpearDamage = m_tState.fAtt*0.5f + m_tState.fDef*0.25f + 5.f;

				for (_uint i = 0; i < 12; ++i)
				{
					static_cast<CJMalkhel_Spear*>(m_pSpearObject[i])->SkillCreate(30.f * (i + 1), fSpearDamage * 0.5f);

				}					

				m_bSkillStart = true;
			}
		}
		if (m_pMesh->Is_AnimationSetEnd())
		{
			AttackEndToMove_ReadyIdle();
			m_eCurTotalPattern = eBossMonsterPattern::Move;
			m_bFarPattern1 = true;
			m_iAttackCount = 0;
			m_bSkillStart = false;
		}
	}	
}

void CJMalkhel::Far_Pattern_2(const _float & _fDeltaTime)
{
	//날아올라 돌진찌르기
	_float fMaxSpeed = 40.f;
	_float fMinSpeed = 10.f;
	if (m_bPhase2On)
	{
		fMaxSpeed = 50.f;
	}

	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Far_2;
		m_pMesh->Set_Animationset((_uint)MalkihelAniID::Attack5);
		m_fSkillMoveSpeed = 10.f;
		m_bAttackSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Attack5)
	{
		if (!m_pMesh->Get_AnimationFrame(82, 33))
		{
			m_bTargetLook = true;
		}
		else if (m_pMesh->Get_AnimationFrame(82, 33) && !m_pMesh->Get_AnimationFrame(82, 54))
		{
			m_pWeapon->Set_Acttack(true);
			m_bTargetLook = false;
		}
		else
		{
			m_pWeapon->Set_Acttack(false);
		}

		if (m_pMesh->Get_AnimationFrame(82, 30) &&!m_pMesh->Get_AnimationFrame(82, 54))
		{
			m_fWalkMoveTime += _fDeltaTime;
			m_fWalkFrontAccelTime += _fDeltaTime;
			if (m_fSkillMoveSpeed < fMaxSpeed)
			{
				m_fSkillMoveSpeed += 6.f * powf(0.5f, m_fWalkFrontAccelTime);
			}
			else
			{
				m_fSkillMoveSpeed = fMaxSpeed;
				m_fWalkFrontAccelTime = 0.f;
			}
			if (m_bAttackSound)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Malkhel_Spear0.ogg", CSound_Manager::ESoundID::Monster_Weapon3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
				m_bAttackSound = false;
			}
			m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		}
		else if (m_pMesh->Get_AnimationFrame(82, 54) && !m_pMesh->Get_AnimationFrame(82, 72))
		{
			m_fWalkFrontAccelTime += _fDeltaTime;
			if (m_fSkillMoveSpeed > fMinSpeed)
			{
				m_fSkillMoveSpeed -= 6.f * powf(0.5f, m_fWalkFrontAccelTime);
				Print_Debug_Float(L"", m_fSkillMoveSpeed);
			}
			else
			{
				m_fSkillMoveSpeed = fMinSpeed;
				m_fWalkFrontAccelTime = 0.f;
				m_iAttackCount = 0;
			}
			m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		}
		if (m_pMesh->Is_AnimationSetEnd())
		{
			AttackEndToMove_ReadyIdle();
			m_eCurTotalPattern = eBossMonsterPattern::Move;
			m_bFarPattern2 = true;
		}

	}
}

void CJMalkhel::Far_Pattern_3(const _float & _fDeltaTime)
{
	//손짓만 함. 구체 3개 생성 후 던지는 패턴
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Idle)
	{
		if (m_bFar3Teleport)
			BackTeleportPos();
		m_ePattern = PATTERN::Far_3;
		m_pMesh->Set_Animationset((_uint)MalkihelAniID::Attack6);
		m_fAddAniPlayTime = 1.f;
	}
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Attack6)
	{
		m_bTargetLook = true;
		_float fTotalDamage = m_tState.fAtt*0.5f + m_tState.fDef*0.25f + 3.f;

		if (!m_bFireBallBackStart)
		{
			static_cast<CJMalkhel_FireBallBack*>(m_pFireBallBackObject)->FireBallBack_Render(true);
			static_cast<CJMalkhel_FireBallBack*>(m_pFireBallBackObject)->ReDissolveOn();
			m_bFireBallBackStart = true;
		}

		if (m_bFireBallBackStart)
		{
			const MY_FRAME* pMonsterFrame = m_pMesh->Get_FrameByName("Malkhel-Head");
			_float4x4 matHead = pMonsterFrame->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();

			static_cast<CJMalkhel_FireBallBack*>(m_pFireBallBackObject)->Set_FireBallBackPos(matHead);
			
		}

		if (m_pMesh->Get_AnimationFrame(86, 57) && !(m_pMesh->Get_AnimationFrame(86, 60)))
		{
			m_fAddAniPlayTime = 0.3f;
			if (!m_bFireBallSkillStart[0])
			{
				_float3 vTargetPos;
				m_pTargetTransform->Get_Info(EInfo::Pos, &vTargetPos);

				const MY_FRAME* pMonsterFrame = m_pMesh->Get_FrameByName("Malkhel-R-Finger2");
				_float4x4 matHead = pMonsterFrame->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();
				_float3 vCreatePos = _float3(matHead._41, matHead._42, matHead._43);

				static_cast<CJMalkhel_FireBall*>(m_pFireBallObject[0])->SkillCreate(vCreatePos, vTargetPos, fTotalDamage, m_bPhase2On);

				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Malkhel_AimBullet1.ogg", CSound_Manager::ESoundID::Monster_Weapon3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);

				m_bFireBallSkillStart[0] = true;
			}
		}
		if (m_pMesh->Get_AnimationFrame(86, 60) && !(m_pMesh->Get_AnimationFrame(86, 63)))
		{
			//2타째
			if (!m_bFireBallSkillStart[1])
			{
				_float3 vTargetPos;
				m_pTargetTransform->Get_Info(EInfo::Pos, &vTargetPos);

				const MY_FRAME* pMonsterFrame = m_pMesh->Get_FrameByName("Malkhel-R-Finger2");
				_float4x4 matHead = pMonsterFrame->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();
				_float3 vCreatePos = _float3(matHead._41, matHead._42, matHead._43);

				static_cast<CJMalkhel_FireBall*>(m_pFireBallObject[1])->SkillCreate(vCreatePos, vTargetPos, fTotalDamage , m_bPhase2On);

				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_1);
				GET_MANAGEMENT->Play3DSounds(L"Malkhel_AimBullet1.ogg", CSound_Manager::ESoundID::Monster_Weapon3D_1, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);

				m_bFireBallSkillStart[1] = true;
			}
		}
		if (m_pMesh->Get_AnimationFrame(86, 63) && !(m_pMesh->Get_AnimationFrame(86, 66)))
		{
			//3타째
			if (!m_bFireBallSkillStart[2])
			{
				_float3 vTargetPos;
				m_pTargetTransform->Get_Info(EInfo::Pos, &vTargetPos);

				const MY_FRAME* pMonsterFrame = m_pMesh->Get_FrameByName("Malkhel-R-Finger2");
				_float4x4 matHead = pMonsterFrame->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();
				_float3 vCreatePos = _float3(matHead._41, matHead._42, matHead._43);

				//static_cast<CJMalkhel_FireBall*>(m_pFireBallObject[2])->SkillCreate(vCreatePos, vTargetPos, 3000.f, m_bPhase2On);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_2);
				GET_MANAGEMENT->Play3DSounds(L"Malkhel_AimBullet1.ogg", CSound_Manager::ESoundID::Monster_Weapon3D_2, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);

				static_cast<CJMalkhel_FireBall*>(m_pFireBallObject[2])->SkillCreate(vCreatePos, vTargetPos, fTotalDamage, m_bPhase2On);



				m_fAddAniPlayTime = 1.7f;
				
				m_bFireBallSkillStart[2] = true;
			}
		}

		if(m_pMesh->Get_AnimationFrame(86, 40))
		{
			if (!m_bFireBallBackEnd)
			{
				static_cast<CJMalkhel_FireBallBack*>(m_pFireBallBackObject)->DissolveOn();
				m_bFireBallBackEnd = true;
			}
		}		

		if (m_bFar3Teleport)
		{
			m_pTransform->Set_Pos(GET_MANAGEMENT->Set_GlobalPos(m_tTransformDesc.szFileTitle, m_vTeleportPos));
			m_iAttackCount = 0;
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			if (m_bPhase2On)
				m_fAddAniPlayTime = 1.7f;
			else
				m_fAddAniPlayTime = 1.5f;
			
			AttackEndToMove_ReadyIdle();
			m_eCurTotalPattern = eBossMonsterPattern::Move;
			m_bFarPattern3 = true;
			m_iAttackCount = 0;
			m_bSkillStart = false;			
			m_bFireBallBackStart = false;
			static_cast<CJMalkhel_FireBallBack*>(m_pFireBallBackObject)->FireBallBack_Render(false);
			m_bFireBallBackEnd = false;

			for (_uint i = 0; i < 3; ++i)
				m_bFireBallSkillStart[i] = false;
		}

	}
}

void CJMalkhel::Near_Pattern_1(const _float & _fDeltaTime)
{
	//120도 휩쓸기
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Near_1;
		m_pMesh->Set_Animationset((_uint)MalkihelAniID::Attack2);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bAttackSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Attack2)
	{
		m_bTargetLook = false;
		
		if (m_pMesh->Get_AnimationFrame(91, 40) && !m_pMesh->Get_AnimationFrame(91, 56))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(Mersen_ToInt(0, 1));
				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_eCurTotalPattern = eBossMonsterPattern::Idle;
			AttackEndToMove_ReadyIdle();
		}

	}
}

void CJMalkhel::Near_Pattern_2(const _float & _fDeltaTime)
{
	//내리찍기 3연타
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Near_2;
		m_pMesh->Set_Animationset((_uint)MalkihelAniID::Attack3);
		m_bNear2Teleport = true;
		m_bAttackSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Attack3)
	{

		if ((m_pMesh->Get_AnimationFrame(175, 0) && !m_pMesh->Get_AnimationFrame(175, 17)) ||
			(m_pMesh->Get_AnimationFrame(175, 56) && !m_pMesh->Get_AnimationFrame(175, 72)) ||
			(m_pMesh->Get_AnimationFrame(175, 101) && !m_pMesh->Get_AnimationFrame(175, 111)))
		{
			if (m_bPhase2On)
			{
				if (m_bNear2Teleport)
				{
					_float3 vPos = *(_float3*)&m_pTransform->Get_TransformDesc().matWorld.m[3][0];
					vPos.y += 50.f;
					//m_pTeleportDistortionTex->Set_Pass(9);
					m_pTeleportDistortionTex->Set_Pos(m_pTransform->Get_TransformDesc().vPos);
					m_pTeleportDistortionTex->Set_RenderSwitch();

					m_fAddAniPlayTime = 0.3f;
					m_bNear2Teleport = false;
				}
			}
		}
		else
		{
			m_tTransformDesc.fRotatePerSecond = 5.f;
		}

		if (m_pMesh->Get_AnimationFrame(175, 143))
		{
			if (m_bAttackSound)
			{
				_uint iCount = Mersen_ToInt(0, 2);
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Malkhel_Ground_%d.ogg", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
				GET_MANAGEMENT->Play3DSounds(szFileName, CSound_Manager::ESoundID::Monster_Weapon3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);
				
				
				iCount = Mersen_ToInt(0, 3);
				TCHAR szFileName2[256] = L"";
				wsprintf(szFileName2, L"Yantari_Lightning%d.ogg", iCount);
				
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Effect3D_0);
				GET_MANAGEMENT->Play3DSounds(szFileName2, CSound_Manager::ESoundID::Monster_Effect3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);

				m_bAttackSound = false;
				m_bAttackSound2 = true;
				Near_Pattern_2_Effect();
			}
		}
		else if (m_pMesh->Get_AnimationFrame(175, 98))
		{

			if (m_bAttackSound2)
			{
				_uint iCount = Mersen_ToInt(0, 2);
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Malkhel_Ground_%d.ogg", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
				GET_MANAGEMENT->Play3DSounds(szFileName, CSound_Manager::ESoundID::Monster_Weapon3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);


				iCount = Mersen_ToInt(0, 3);
				TCHAR szFileName2[256] = L"";
				wsprintf(szFileName2, L"Yantari_Lightning%d.ogg", iCount);

				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Effect3D_0);
				GET_MANAGEMENT->Play3DSounds(szFileName2, CSound_Manager::ESoundID::Monster_Effect3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);

				m_bAttackSound = true;
				m_bAttackSound2 = false;
				Near_Pattern_2_Effect();
			}
		}
		else if (m_pMesh->Get_AnimationFrame(175, 53))
		{

			if (m_bAttackSound)
			{
				_uint iCount = Mersen_ToInt(0, 2);
				TCHAR szFileName[256] = L"";
				wsprintf(szFileName, L"Malkhel_Ground_%d.ogg", iCount);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
				GET_MANAGEMENT->Play3DSounds(szFileName, CSound_Manager::ESoundID::Monster_Weapon3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);


				iCount = Mersen_ToInt(0, 3);
				TCHAR szFileName2[256] = L"";
				wsprintf(szFileName2, L"Yantari_Lightning%d.ogg", iCount);

				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Effect3D_0);
				GET_MANAGEMENT->Play3DSounds(szFileName2, CSound_Manager::ESoundID::Monster_Effect3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);

				m_bAttackSound = false;
				m_bAttackSound2 = true;
				Near_Pattern_2_Effect();
			}
		}

		if ((m_pMesh->Get_AnimationFrame(175, 56) && !m_pMesh->Get_AnimationFrame(175, 65)) ||
			(m_pMesh->Get_AnimationFrame(175, 100) && !m_pMesh->Get_AnimationFrame(175, 110)))
		{
			if (!m_bPhase2On)
			{
				_float3 vDir = m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos;
				vDir.y = 0.f;
				if (D3DXVec3Length(&vDir) > 5.f)
				{
					m_fAddAniPlayTime = 1.5f;
					m_pMesh->Set_Animationset((_uint)MalkihelAniID::Attack3ToEnd);
				}
			}
		}

		if (m_fAddAniPlayTime == 0.3f)
		{
			m_iShaderPass = 8;
			m_fDissolveTime += _fDeltaTime * powf(5.f, m_fDissolveTime);
			if (1.f<m_fDissolveTime)
			{
				FrontTeleportPos();
				m_pTransform->Set_Pos(GET_MANAGEMENT->Set_GlobalPos(m_tTransformDesc.szFileTitle, m_vTeleportPos));
				m_fDissolveTime = 0.f;
				m_iShaderPass = 9;
				m_fAddAniPlayTime = 2.f;
			}
		}
		if (m_iShaderPass == 9)
		{
			m_fDissolveTime += _fDeltaTime * powf(20.f, m_fDissolveTime);
			m_tTransformDesc.fRotatePerSecond = 30.f;
			if (1.f<m_fDissolveTime)
			{
				m_fDissolveTime = 0.f;
				m_iShaderPass = 4;
				m_fAddAniPlayTime = 2.f;
			}
		}

		if ((m_pMesh->Get_AnimationFrame(175, 18) && !m_pMesh->Get_AnimationFrame(175, 25)) ||
			(m_pMesh->Get_AnimationFrame(175, 73) && !m_pMesh->Get_AnimationFrame(175, 80)) ||
			(m_pMesh->Get_AnimationFrame(175, 112) && !m_pMesh->Get_AnimationFrame(175, 120)))
		{
			if (!m_bPhase2On)
			{
				m_fAddAniPlayTime = 0.5f;
				m_pTransform->Set_Pos(GET_MANAGEMENT->Move(m_tTransformDesc.szFileTitle, _float3(0.f, 1.f, 0.f), 10.f, _fDeltaTime));
			}
			else
			{
				m_pTransform->Set_Pos(GET_MANAGEMENT->Move(m_tTransformDesc.szFileTitle, _float3(0.f, 1.f, 0.f), 30.f, _fDeltaTime));

			}
		}
		else
		{
			if (!m_bPhase2On)
				m_fAddAniPlayTime = 1.3f;
		}

		if ((m_pMesh->Get_AnimationFrame(175, 43) && !m_pMesh->Get_AnimationFrame(175, 56))|| 
			(m_pMesh->Get_AnimationFrame(175, 88) && !m_pMesh->Get_AnimationFrame(175, 100)))
		{
			m_pTransform->Set_Pos(GET_MANAGEMENT->Move(m_tTransformDesc.szFileTitle, _float3(0.f, -1.f, 0.f), 50.f, _fDeltaTime));
			m_pWeapon->Set_Acttack(true);
			if (!m_bPhase2On)
				m_bTargetLook = false;
			else
				m_bNear2Teleport = true;
		}
		else if (m_pMesh->Get_AnimationFrame(175, 133) && !m_pMesh->Get_AnimationFrame(175, 146))
		{
			m_pTransform->Set_Pos(GET_MANAGEMENT->Move(m_tTransformDesc.szFileTitle, _float3(0.f, -1.f, 0.f), 30.f, _fDeltaTime));
			m_pWeapon->Set_Acttack(true);
			if (!m_bPhase2On)
				m_bTargetLook = false;
		}
		else
		{
			if (!m_bPhase2On)
			{
				m_bTargetLook = true;
				_float3 vPlyPos = m_pTargetTransform->Get_TransformDesc().vPos;
				_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
				vPlyPos.y = 0.f;
				vPos.y = 0.f;

				if (D3DXVec3Length(&(vPlyPos - vPos))>3.5f)
				{
					m_pTransform->Set_Pos(GET_MANAGEMENT->Move(m_tTransformDesc.szFileTitle, m_vLook, 10.f, _fDeltaTime));
				}
			}
			m_pWeapon->Set_Acttack(false);
		}

		m_bNearPattern2 = true;
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_eCurTotalPattern = eBossMonsterPattern::Idle;
			m_bNear2Teleport = false;
			if (!m_bPhase2On)
				m_fAddAniPlayTime = 1.5f;
			else
				m_fAddAniPlayTime = 1.7f;
			AttackEndToMove_ReadyIdle();
		}
	}


	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Attack3ToEnd)
	{
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_eCurTotalPattern = eBossMonsterPattern::Idle;
			m_bNear2Teleport = false;
			if (!m_bPhase2On)
				m_fAddAniPlayTime = 1.5f;
			else
				m_fAddAniPlayTime = 1.7f;
			AttackEndToMove_ReadyIdle();
		}
	}
}

void CJMalkhel::Near_Pattern_3(const _float & _fDeltaTime)
{
	//360도 휩쓸기
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Near_3;
		m_pMesh->Set_Animationset((_uint)MalkihelAniID::Attack4);
		m_bAttackSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Attack4)
	{
		m_bTargetLook = false;

		if (!m_bEffect360Start&&m_bPhase2On)
		{
			static_cast<CJMalkhel_Effect_360*>(m_pEffect360)->Effect_360_Render(true);
			static_cast<CJMalkhel_Effect_360*>(m_pEffect360)->ReDissolveOn();
			_float3 vPos;
			m_pTransform->Get_Info(EInfo::Pos, &vPos);
			static_cast<CJMalkhel_Effect_360*>(m_pEffect360)->Set_Effect_360Pos(vPos);
			m_bEffect360Start = true;
		}
		
		if (m_pMesh->Get_AnimationFrame(79, 22) && !m_pMesh->Get_AnimationFrame(79, 48))
			m_pWeapon->Set_Acttack(true);
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(79, 30))
		{
			if (!m_bEffect360End)
			{
				//static_cast<CJMalkhel_Effect_360*>(m_pEffect360)->DissolveOn();
				m_bEffect360End = true;
			}
		}
		if (m_pMesh->Get_AnimationFrame(79, 24))
		{
			if (m_bAttackSound)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Malkhel_Near_3.ogg", CSound_Manager::ESoundID::Monster_Weapon3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);
				m_bAttackSound = false;
			}
		}
		m_bNearPattern3 = true;
		if (m_pMesh->Is_AnimationSetEnd())
		{
			//static_cast<CJMalkhel_Effect_360*>(m_pEffect360)->Effect_360_Render(false);
			m_bEffect360Start = false;
			m_bEffect360End = true;
			m_eCurTotalPattern = eBossMonsterPattern::Idle;
			AttackEndToMove_ReadyIdle();
		}

	}
}

void CJMalkhel::Near_Pattern_4(const _float & _fDeltaTime)
{
	// 좌우로 2연타
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Near_4;
		m_pMesh->Set_Animationset((_uint)MalkihelAniID::Attack7);
		m_bAttackSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Attack7)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(115, 49) && !m_pMesh->Get_AnimationFrame(115, 62))
			m_pWeapon->Set_Acttack(true);
		else if (m_pMesh->Get_AnimationFrame(115, 71) && !m_pMesh->Get_AnimationFrame(115, 82))
			m_pWeapon->Set_Acttack(true);
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(115, 71))
		{
			if (m_bAttackSound2)
			{
				m_pWeapon->WeaponSound(Mersen_ToInt(0, 1));

				m_bAttackSound = true;
				m_bAttackSound2 = false;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(115, 52))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(Mersen_ToInt(0, 1));

				m_bAttackSound2 = true;
				m_bAttackSound = false;
			}
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_eCurTotalPattern = eBossMonsterPattern::Idle;
			AttackEndToMove_ReadyIdle();
		}

	}
}

void CJMalkhel::Move_Pattern_B(const _float & _fDeltaTime)
{

	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_B;
		m_pMesh->Set_Animationset((_uint)MalkihelAniID::Move_Back);
		if (m_bPhase2On)
			m_fSkillMoveSpeed = 18.f;
		else
			m_fSkillMoveSpeed = 13.f;

	}
	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Move_Back)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		m_fWalkMoveTime += _fDeltaTime;

		if (m_pMesh->Get_AnimationFrame(68, 25))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
			}
		}
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bRightLeg = true;
		}
		if (m_fWalkMoveTime > 1.f)
		{
			m_fWalkMoveTime = 0.f;
			m_eCurTotalPattern = eBossMonsterPattern::Attack;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
		}
	}
}

void CJMalkhel::Move_Pattern_F(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	_float fMaxSpeed, fMinSpeed;
	if (m_bPhase2On)
	{
		fMaxSpeed = 20.f;
		fMinSpeed = 15.f;
	}
	else
	{
		fMaxSpeed = 18.f;
		fMinSpeed = 13.f;
	}
	if (m_pMesh->Get_Animationset() == (_uint) MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_F;
		m_pMesh->Set_Animationset((_uint) MalkihelAniID::Move_Front);
		if (m_bPhase2On)
			m_fSkillMoveSpeed = 15.f;
		else
			m_fSkillMoveSpeed = 13.f;
		m_fTargetDistHalf = m_fTargetDist*0.5f;
		m_bRightLeg = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint) MalkihelAniID::Move_Front)
	{
		m_bTargetLook = true;
		m_fWalkMoveTime += _fDeltaTime;

		if (m_pMesh->Get_AnimationFrame(68, 25))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
			}
		}

		if (m_fTargetDist > 7.5f)
		{ 
			if (m_fSkillMoveSpeed < fMaxSpeed)
			{
				m_fWalkFrontAccelTime += _fDeltaTime;
				m_fSkillMoveSpeed += powf(0.1f, m_fWalkFrontAccelTime)*0.5f;
				//Print_Debug_Float(L"", m_fSkillMoveSpeed);
			}

		}
		else
		{
			if (m_fSkillMoveSpeed > fMinSpeed)
			{
				m_fWalkFrontAccelTime -= _fDeltaTime;
				m_fSkillMoveSpeed -= powf(0.1f, m_fWalkFrontAccelTime)*0.5f;
				//Print_Debug_Float(L"", m_fSkillMoveSpeed);
			}
		}

		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bRightLeg = true;
		}
		if (m_fTargetDist < 5.f/*|| m_fWalkMoveTime>3.f*/)
		{
			if (m_iAttackCount>0)
			{
				--m_iAttackCount;
			}
			if (m_fTargetDist < 5.f)
			{
				m_eCurTotalPattern = eBossMonsterPattern::Attack;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
			}
			else
			{
				FrontTeleportPos();
				m_ePattern = PATTERN::FrontTeleport;
			}
			m_fWalkFrontAccelTime = 0.f;
			m_fWalkMoveTime = 0.f;
		}

	}
}

void CJMalkhel::Move_Pattern_L(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint) MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_L;
		m_pMesh->Set_Animationset((_uint) MalkihelAniID::Move_Left);
		if (m_bPhase2On)
			m_fSkillMoveSpeed = 15.f;
		else
			m_fSkillMoveSpeed = 10.f;
	}
	if (m_pMesh->Get_Animationset() == (_uint) MalkihelAniID::Move_Left)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vRight, m_fSkillMoveSpeed, _fDeltaTime));
		m_fWalkMoveTime += _fDeltaTime;
		
		if (m_pMesh->Get_AnimationFrame(68, 25))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
			}
		}
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bRightLeg = true;
		}
		if (m_fWalkMoveTime>2.f)
		{
			if (m_fTargetDist <= 5.f)
			{
				m_eCurTotalPattern = eBossMonsterPattern::Attack;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
			}
			else
			{
				if (Mersen_ToInt(0, 1))
				{
					m_eCurTotalPattern = eBossMonsterPattern::Attack;
					m_ePattern = PATTERN::Pattern_Ready;
					m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
				}
				else
				{
					m_eCurTotalPattern = eBossMonsterPattern::Move;
					m_ePattern = PATTERN::Walk_F;
					m_pMesh->Set_Animationset((_uint)MalkihelAniID::Move_Front);
				}
			}
			m_fWalkMoveTime = 0.f;
		}
	}

}

void CJMalkhel::Move_Pattern_R(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint) MalkihelAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_R;
		m_pMesh->Set_Animationset((_uint) MalkihelAniID::Move_Right);
		if (m_bPhase2On)
			m_fSkillMoveSpeed = 15.f;
		else
			m_fSkillMoveSpeed = 10.f;

	}
	if (m_pMesh->Get_Animationset() == (_uint) MalkihelAniID::Move_Right)
	{
		m_bTargetLook = true;
		_float3 vPos = pManagement->Move(m_tTransformDesc.szFileTitle, -m_vRight, m_fSkillMoveSpeed, _fDeltaTime);
		m_pTransform->Set_Pos(vPos);
		m_fWalkMoveTime += _fDeltaTime;

		if (m_pMesh->Get_AnimationFrame(68, 25))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
			}
		}
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bRightLeg = true;
		}
		if (m_fWalkMoveTime>2.f)
		{
			if (m_fTargetDist <= 5.f)
			{
				m_eCurTotalPattern = eBossMonsterPattern::Attack;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
			}
			else
			{
				if (Mersen_ToInt(0, 1))
				{
					m_eCurTotalPattern = eBossMonsterPattern::Attack;
					m_ePattern = PATTERN::Pattern_Ready;
					m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
				}
				else
				{
					m_eCurTotalPattern = eBossMonsterPattern::Move;
					m_ePattern = PATTERN::Walk_F;
					m_pMesh->Set_Animationset((_uint)MalkihelAniID::Move_Front);
				}
			}
			m_fWalkMoveTime = 0.f;
		}
	}
}

void CJMalkhel::BackMove_Pattern_Teleport(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_ePattern != PATTERN::BackTeleport)
	{
		BackTeleportPos();
		m_ePattern = PATTERN::BackTeleport;
		if (m_bPhase2On)
			m_fSkillMoveSpeed = 15.f;
		else
			m_fSkillMoveSpeed = 10.f;

		m_pTransform->Set_Pos(pManagement->Set_GlobalPos(m_tTransformDesc.szFileTitle, m_vTeleportPos));
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Effect3D_0);

		//GET_MANAGEMENT->Play3DSounds(L"Malkhel_Near_3.ogg", CSound_Manager::ESoundID::Monster_Effect3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);
		/*
			순간이동 시작
		
		*/
		_float3 vPos = *(_float3*)&m_pTransform->Get_TransformDesc().matWorld.m[3][0];
		vPos.y += 50.f;
		m_pTeleportDistortionTex->Set_Pos(m_pTransform->Get_TransformDesc().vPos);
		m_pTeleportDistortionTex->Set_RenderSwitch();

		GET_MANAGEMENT->Play3DSounds(L"Malkhel_Teleport.ogg", CSound_Manager::ESoundID::Monster_Effect3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);


	}
	if (::CoolTime(_fDeltaTime, m_fWalkMoveTime, .5f))
	{
		m_fWalkMoveTime = 0.f;
		if (Mersen_ToInt(0, 1))
		{
			m_eCurTotalPattern = eBossMonsterPattern::Attack;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
		}
		else
		{
			m_eCurTotalPattern = eBossMonsterPattern::Move;
			if (Mersen_ToInt(0, 1))
			{
				m_pMesh->Set_Animationset((_uint)MalkihelAniID::Move_Left);
				m_ePattern = PATTERN::Walk_L;
			}
			else
			{
				m_pMesh->Set_Animationset((_uint)MalkihelAniID::Move_Right);
				m_ePattern = PATTERN::Walk_R;
			}
		}
	}
}

void CJMalkhel::FrontMove_Pattern_Teleport(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_ePattern == PATTERN::Pattern_Ready)
	{
		FrontTeleportPos();
		m_ePattern = PATTERN::FrontTeleport;
		if (m_bPhase2On)
			m_fSkillMoveSpeed = 15.f;
		else
			m_fSkillMoveSpeed = 10.f;
		m_pTransform->Set_Pos(pManagement->Set_GlobalPos(m_tTransformDesc.szFileTitle, m_vTeleportPos));
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Effect3D_0);

		//GET_MANAGEMENT->Play3DSounds(L"Malkhel_Near_3.ogg", CSound_Manager::ESoundID::Monster_Effect3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);
		/*
			순간이동 시작

		*/
		_float3 vPos = *(_float3*)&m_pTransform->Get_TransformDesc().matWorld.m[3][0];
		vPos.y += 50.f;
		//m_pTeleportDistortionTex->Set_Pass(9);
		m_pTeleportDistortionTex->Set_Pos(m_pTransform->Get_TransformDesc().vPos);
		m_pTeleportDistortionTex->Set_RenderSwitch();

		GET_MANAGEMENT->Play3DSounds(L"Malkhel_Teleport.ogg", CSound_Manager::ESoundID::Monster_Effect3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);


	}

	if (m_ePattern == PATTERN::FrontTeleport)
	{

		if (::CoolTime(_fDeltaTime, m_fWalkMoveTime, .5f))
		{
			m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
			m_fWalkMoveTime = 0.f;
			m_eCurTotalPattern = eBossMonsterPattern::Attack;
			m_ePattern = PATTERN::Pattern_Ready;

		}
	}


}

void CJMalkhel::BackTeleportPos()
{
	_float3 vLook;
	m_pTransform->Get_Info(EInfo::Pos, &m_vTeleportPos);
	m_pTransform->Get_Info(EInfo::Look, &vLook);
	vLook.y = 0.f;
	D3DXMATRIX matRot;
	_float fRadian = Mersen_ToFloat(-1.f, 1.f);
	D3DXMatrixRotationY(&matRot, fRadian);

	D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
	vLook*= 500.f;
	vLook.y = 0.f;
	m_vTeleportPos += vLook;
	_float vTeleportLimit1 = D3DXVec3Length(&_float3{ m_vTeleportPos.x - 85.f,0.f,m_vTeleportPos.z - 554.f });
	_float vTeleportLimit2 = D3DXVec3Length(&_float3{ m_vTeleportPos.x - 47.f,0.f,m_vTeleportPos.z - 532.f });
	_float vTeleportLimit3 = D3DXVec3Length(&_float3{ m_vTeleportPos.x - 24.f,0.f,m_vTeleportPos.z - 566.f });
	_float vTeleportLimit4 = D3DXVec3Length(&_float3{ m_vTeleportPos.x - 62.f,0.f,m_vTeleportPos.z - 588.f });

	if (vTeleportLimit1<5.5f ||
		vTeleportLimit2<5.5f ||
		vTeleportLimit3<5.5f ||
		vTeleportLimit4<5.5f)
	{
		m_vTeleportPos += vLook*0.1f;
	}
	m_vTeleportPos.y += 2.f;


}

void CJMalkhel::FrontTeleportPos()
{
	_float3 vLook;
	m_pTargetTransform->Get_Info(EInfo::Pos, &m_vTeleportPos);
	m_pTargetTransform->Get_Info(EInfo::Look, &vLook);
	D3DXMATRIX matRot;
	_float Angle = Mersen_ToFloat(-180.f, 180.f);
	D3DXMatrixIdentity(&matRot);
	D3DXMatrixRotationY(&matRot, D3DXToRadian(Angle));
	D3DXVec3TransformNormal(&vLook, &vLook, &matRot);
	vLook *= 200.f;
	m_vTeleportPos += vLook;

	_float vTeleportLimit1 = D3DXVec3Length(&_float3{ m_vTeleportPos.x - 85.f,0.f,m_vTeleportPos.z - 554.f });
	_float vTeleportLimit2 = D3DXVec3Length(&_float3{ m_vTeleportPos.x - 47.f,0.f,m_vTeleportPos.z - 532.f });
	_float vTeleportLimit3 = D3DXVec3Length(&_float3{ m_vTeleportPos.x - 24.f,0.f,m_vTeleportPos.z - 566.f });
	_float vTeleportLimit4 = D3DXVec3Length(&_float3{ m_vTeleportPos.x - 62.f,0.f,m_vTeleportPos.z - 588.f });

	if (vTeleportLimit1<5.5f ||
		vTeleportLimit2<5.5f ||
		vTeleportLimit3<5.5f ||
		vTeleportLimit4<5.5f)
	{
		m_vTeleportPos += vLook;
	}
	m_vTeleportPos.y += 2.f;
}

void CJMalkhel::Hit_Pattern(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	if (m_pMesh->Get_Animationset() == (_uint)MalkihelAniID::Hit)
	{
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
		}
	}
}

HRESULT CJMalkhel::Clone_Trail()
{

	for (_uint i = 0; i < 6; ++i)
	{
		//m_pArrWing_TrailEffect[i] = nullptr;
		m_pArrWingDistort_TrailEffect[i] = nullptr;
		TRAIL_DESC tTrailDesc;
		_float4x4 matWorld;
		D3DXMatrixIdentity(&matWorld);

		tTrailDesc.pObj = this;
		lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");
		tTrailDesc.fTrailChekRate = 0.01f;
		tTrailDesc.fTrailWidth = 100.f;

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
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingSup_L03");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}
		else if (1 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingInf_L03");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}
		else if (2 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingSup_R03");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}
		else if (2 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingSup_R03");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}
		else if (3 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingInf_R03");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}
		else if (2 == i)
		{
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingSup_R03");
			matWorld = pFrame1->CombinedTranformationMatrix;
			matWorld.m[3][0] += 5.f;
			tTrailDesc.matLocal = matWorld;
		}


		//FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
		//	L"GameObject_TrailEffect", (CGameObject**)&m_pArrDash_TrailEffect[i], &tTrailDesc), E_FAIL);
		//FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer((CGameObject*)(m_pArrDash_TrailEffect[i]), m_iSceneIdx, L"Layer_Effect"), E_FAIL);

		//Safe_AddRef(m_pArrDash_TrailEffect[i]);

		tTrailDesc.fTrailWidth = 40.f;
		tTrailDesc.iRenderID = 4;
		tTrailDesc.eRenderState = ERenderPriority::Environment;
		tTrailDesc.iRenderPass = 4;

		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
			L"GameObject_TrailEffect", (CGameObject**)&m_pArrWingDistort_TrailEffect[i], &tTrailDesc), E_FAIL);
		FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer((CGameObject*)(m_pArrWingDistort_TrailEffect[i]), m_iSceneIdx, L"Layer_Effect"), E_FAIL);
		Safe_AddRef(m_pArrWingDistort_TrailEffect[i]);
	}






	return S_OK;
}

void CJMalkhel::Fix_DistortionTrailPos(const _float & _fDeltaTime)
{
	for (_uint i = 0; i < 6; ++i)
	{
		//m_pArrWing_TrailEffect[i] = nullptr;
		//m_pArrWingDistort_TrailEffect[i] = nullptr;
	
		_float4x4 matRotate,matFrame;
		D3DXMatrixIdentity(&matRotate);
		//D3DXMatrixRotationZ(&matRotate, D3DXToRadian(45.f));
		
		if (0 == i)
		{
			D3DXMatrixRotationYawPitchRoll(&matRotate, -D3DXToRadian(90.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingSup_L03");
			matFrame = pFrame1->CombinedTranformationMatrix;
			m_pArrWingDistort_TrailEffect[i]->Set_Width(150.f);
			//matFrame.m[3][0] -= 15.f;
			matFrame.m[3][1] -= 5.f;
		}
		else if (1 == i)
		{
			D3DXMatrixRotationYawPitchRoll(&matRotate, -D3DXToRadian(90.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingInf_L02");
			matFrame = pFrame1->CombinedTranformationMatrix;
			m_pArrWingDistort_TrailEffect[i]->Set_Width(80.f);
			//matFrame.m[3][0] -= 15.f;
			matFrame.m[3][1] -= 5.f;
		}
		else if (2 == i)
		{
			D3DXMatrixRotationYawPitchRoll(&matRotate, -D3DXToRadian(90.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingInf_L03");
			matFrame = pFrame1->CombinedTranformationMatrix;
			m_pArrWingDistort_TrailEffect[i]->Set_Width(150.f);
			//matFrame.m[3][0] -= 15.f;
			matFrame.m[3][1] -= 5.f;
		}
		else if (3 == i)
		{
			D3DXMatrixRotationYawPitchRoll(&matRotate, -D3DXToRadian(90.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingSup_R03");
			matFrame = pFrame1->CombinedTranformationMatrix;
			m_pArrWingDistort_TrailEffect[i]->Set_Width(150.f);
			//matFrame.m[3][0] -= 15.f;
			matFrame.m[3][1] -= 5.f;
		}
		else if (4 == i)
		{
			D3DXMatrixRotationYawPitchRoll(&matRotate, -D3DXToRadian(90.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingInf_R02");
			matFrame = pFrame1->CombinedTranformationMatrix;
			m_pArrWingDistort_TrailEffect[i]->Set_Width(80.f);
			//matFrame.m[3][0] -= 15.f;
			matFrame.m[3][1] -= 5.f;
		}
		else if (5 == i)
		{
			D3DXMatrixRotationYawPitchRoll(&matRotate, -D3DXToRadian(90.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
			const MY_FRAME*  pFrame1 = m_pMesh->Get_FrameByName("Malkhel-WingInf_R03");
			matFrame = pFrame1->CombinedTranformationMatrix;
			m_pArrWingDistort_TrailEffect[i]->Set_Width(150.f);
			//matFrame.m[3][0] -= 15.f;
			matFrame.m[3][1] -= 5.f;
		}
		m_pArrWingDistort_TrailEffect[i]->Set_LocalWorld(matRotate*matFrame/**matRotate*/);
	
		m_pArrWingDistort_TrailEffect[i]->Set_RenderID(2);
		m_pArrWingDistort_TrailEffect[i]->Set_TrailRateTime(0.05f);
		m_pArrWingDistort_TrailEffect[i]->Set_Pass(8);
	}
}

HRESULT CJMalkhel::Clone_Distortion()
{
	CEffect_Texture::TEXOBJ_DESC tTexDistort;

	tTexDistort.eTexMode = CEffect_Texture::eTexMode::DistortionFilter;
	tTexDistort.iRenderPass = 9;
	tTexDistort.iRenderId = 2;
	tTexDistort.vArgb = { 1.f,1.f,1.f,1.f };
	tTexDistort.eMovePattern = ePartilcePatternType::BillBoard;
	tTexDistort.tTransformDesc.vPos = { 0.f, 0.f, 0.f };
	tTexDistort.tTransformDesc.vScale = { 5.f,10.f,1.f };
	tTexDistort.iRenderId_Filter = 5;
	//if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Tex_Obj",
	//	(_uint)ESceneType::Static,
	//	L"Layer_Effect", &tTexDescAni)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"GameObject_Tex_Obj", (CGameObject**)&m_pTeleportDistortionTex, &tTexDistort), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTeleportDistortionTex, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pTeleportDistortionTex);
	m_pTeleportDistortionTex->Set_RenderSwitch(false);

	m_pTeleportDistortionTex->Set_RenderId_Fileter(5);
	m_pTeleportDistortionTex->Set_Pass(8);
	m_pTeleportDistortionTex->Set_RenderId(4);
	m_pTeleportDistortionTex->Set_Tileling(5.f);
	return S_OK;
}

void CJMalkhel::Get_Damaged()
{
	_uint iCount = Mersen_ToInt(0, 2);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Malkhel_Damage%d.wav", iCount);
	GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_GetHit3D_0);
	GET_MANAGEMENT->Play3DSounds(szFileName, CSound_Manager::ESoundID::Monster_GetHit3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);

}

void CJMalkhel::Move_Sound()
{
	_uint iCount = Mersen_ToInt(0, 1);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Malkhel_Wing%d.ogg", iCount);
	GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
	GET_MANAGEMENT->Play3DSounds(szFileName, CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);

}

void CJMalkhel::Death_Pattern(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() != (_uint) MalkihelAniID::Death)
	{
		m_pBossUI->Set_Dead(true);
		m_pTransform->Set_Pos(m_vInitilizePos);
		m_pTransform->Set_Rotate(m_vOriginRotate);
		m_pTransform->Update_Transform();
		m_ePattern = PATTERN::Death;
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Death3D_0);
		GET_MANAGEMENT->Play3DSounds(L"Malkhel_Death.ogg", CSound_Manager::ESoundID::Monster_Death3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f, false, 1.f);
		m_fAddAniPlayTime = 0.5f;
		m_pMesh->Set_Animationset((_uint) MalkihelAniID::Death);
		m_iCinemaEndCount = 0;
		CCinematicCamera_0* pCamera = static_cast<CCinematicCamera_0*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
		Get_EventMgr()->Set_Event(EVENT_VOLATILE_ENDING);
		pCamera->Play_NextEndingCinematic(m_iCinemaEndCount);
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::BGM);
		m_pBossUI->Set_Dead(true);
	}
	if (m_pMesh->Get_Animationset() == (_uint) MalkihelAniID::Death)
	{
		m_pTransform->Set_Pos(m_vInitilizePos);
		m_pTransform->Set_Rotate(m_vOriginRotate);
		m_bTargetLook = false;
		
		if (m_pMesh->Get_AnimationFrame(126, 76))
		{
			if (1 == m_iCinemaEndCount)
			{
				m_iCinemaEndCount++;
				CCinematicCamera_0* pCamera = static_cast<CCinematicCamera_0*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
				pCamera->Play_NextEndingCinematic(m_iCinemaEndCount);
			}
		}
		else if (m_pMesh->Get_AnimationFrame(126, 30))
		{
			if (0 == m_iCinemaEndCount)
			{
				m_iCinemaEndCount++;
				CCinematicCamera_0* pCamera = static_cast<CCinematicCamera_0*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
				pCamera->Play_NextEndingCinematic(m_iCinemaEndCount);
			}

		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_fDissolveTime += _fDeltaTime *pow(1.1f, m_fDissolveTime);

			m_fAddAniPlayTime = 0.f;

			if (m_fDissolveTime > 4.f)
			{
				if (2 == m_iCinemaEndCount)
				{
					m_iCinemaEndCount++;
					CCinematicCamera_0* pCamera = static_cast<CCinematicCamera_0*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
					pCamera->Play_NextEndingCinematic(m_iCinemaEndCount);
				}

				m_eCurTotalPattern = eBossMonsterPattern::Sleep;
				Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_DEAD_BOSS_MALKHEL);
			}

			//m_pBossUI->Set_Dead(true);
			//m_bDead = true;
		}
	}
}

void CJMalkhel::AttackEndToMove_ReadyIdle()
{
	//m_bDelayMove = true;
	m_bAttackCoolTime = true;
	m_bTargetLook = true;
	m_ePattern = PATTERN::Pattern_Ready;
	m_pMesh->Set_Animationset((_uint)MalkihelAniID::Idle);
}

HRESULT CJMalkhel::Weapon_Clone()
{
	auto pManagement = GET_MANAGEMENT;

	wstring wstrMonsterName = m_tTransformDesc.szFileTitle;

	_uint count = (_uint)wstrMonsterName.find(L"_", 0);
	wstring wstrinteger = wstrMonsterName;
	wstrinteger.erase(0, count + 1);
	wstrinteger += L"_Weapon";

	TRANSFORM_DESC tTransformDesc;
	tTransformDesc.vRotate = _float3(0.f, D3DXToRadian(90.f), 0.f);
	lstrcpy(tTransformDesc.szFileTitle, m_tTransformDesc.szFileTitle);
	lstrcat(tTransformDesc.szFileTitle, L"_Malkhel_Lughan_");
	lstrcat(tTransformDesc.szFileTitle, wstrinteger.c_str());
	if (FAILED(pManagement->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_Malkhel_Lughan",
		(CGameObject**)&m_pWeapon,
		&tTransformDesc,
		m_iSceneIdx)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pWeapon, m_iSceneIdx, L"Layer_Weapon")))
	{
		ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	m_pWeapon->Set_ParentClass(this, CJBaseWeapon::eMonsterParent::Boss);
	m_pWeapon->Set_AtkDamage(m_tState.fAtt, m_tState.fDef);
	Safe_AddRef(m_pWeapon);

	return S_OK;
}

HRESULT CJMalkhel::Skill_Clone()
{
	auto pManagement = GET_MANAGEMENT;

	for (_uint i = 0; i < 3; ++i)
	{
		wstring wstrCloneName = L"Malkhel_FireBall_" + to_wstring(i) + L"_Weapon";
		TRANSFORM_DESC tTransformDesc;
		tTransformDesc.vScale = _float3(0.08f, 0.08f, 0.08f);
		tTransformDesc.vRotate = _float3(0.f, 0.f, 0.f);
		lstrcpy(tTransformDesc.szFileTitle, wstrCloneName.c_str());
		if (FAILED(pManagement->Clone_GameObject((_uint)ESceneType::Static,
			L"GameObject_Malkhel_FireBall",
			(CGameObject**)&m_pFireBallObject[i],
			&tTransformDesc,
			m_iSceneIdx)))
		{
			ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
			return E_FAIL;
		}

		if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pFireBallObject[i], m_iSceneIdx, L"Layer_Skill")))
		{
			ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
			return E_FAIL;
		}

		Safe_AddRef(m_pFireBallObject[i]);
	}

	for (_uint i = 0; i < 12; ++i)
	{
		wstring wstrCloneName = L"Malkhel_Spear_" + to_wstring(i) + L"_Weapon";
		TRANSFORM_DESC tTransformDesc;
		tTransformDesc.vScale = _float3(0.03f, 0.03f, 0.03f);
		tTransformDesc.vRotate = _float3(0.f, D3DXToRadian(120.f + (i * 30.f)), 0.f);
		lstrcpy(tTransformDesc.szFileTitle, wstrCloneName.c_str());
		if (FAILED(pManagement->Clone_GameObject((_uint)ESceneType::Static,
			L"GameObject_Malkhel_Spear",
			(CGameObject**)&m_pSpearObject[i],
			&tTransformDesc,
			m_iSceneIdx)))
		{
			ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
			return E_FAIL;
		}

		if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pSpearObject[i], m_iSceneIdx, L"Layer_Skill")))
		{
			ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
			return E_FAIL;
		}

		Safe_AddRef(m_pSpearObject[i]);
	}	

	return S_OK;
}

HRESULT CJMalkhel::Effect_Clone()
{
	auto pManagement = GET_MANAGEMENT;

	wstring wstrCloneName = L"Malkhel_FireBall_BackEffect";
	TRANSFORM_DESC tTransformDesc;
	tTransformDesc.vScale = _float3(1.f, 1.f, 1.f);
	tTransformDesc.vRotate = _float3(D3DXToRadian(45.f), 0.f, 0.f);
	lstrcpy(tTransformDesc.szFileTitle, wstrCloneName.c_str());
	if (FAILED(pManagement->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_Malkhel_FireBall_BackEffect",
		(CGameObject**)&m_pFireBallBackObject,
		&tTransformDesc,
		m_iSceneIdx)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pFireBallBackObject, m_iSceneIdx, L"Layer_Skill")))
	{
		ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	Safe_AddRef(m_pFireBallBackObject);

	wstrCloneName = L"Malkhel_Effect_360";
	tTransformDesc;
	tTransformDesc.vScale = _float3(0.f, 0.f, 0.f);
	tTransformDesc.vRotate = _float3(0.f, 0.f, 0.f);
	lstrcpy(tTransformDesc.szFileTitle, wstrCloneName.c_str());
	if (FAILED(pManagement->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_Malkhel_Effect_360",
		(CGameObject**)&m_pEffect360,
		&tTransformDesc,
		m_iSceneIdx)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pEffect360, m_iSceneIdx, L"Layer_Skill")))
	{
		ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	Safe_AddRef(m_pEffect360);

	return S_OK;
}

HRESULT CJMalkhel::UI_Clone()
{
	auto pManagement = GET_MANAGEMENT;
	
	if (FAILED(pManagement->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_BossUI",
		(CGameObject**)&m_pBossUI,
		&m_tBackGroundDesc,
		m_iSceneIdx)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pBossUI, m_iSceneIdx, L"Layer_UI")))
	{
		ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}
	
	Safe_AddRef(m_pBossUI);
	m_pBossUI->Set_ParentClass(this);
	m_pBossUI->Set_BossName(CUI_BossMonster::eBossName::Malkhel);

	return S_OK;
}

void CJMalkhel::AttackCoolTimer(const _float & _fDeltaTime)
{
	if (m_bFarPattern1)
	{
		if (::CoolTime(_fDeltaTime,m_fFarPattern1Time,7.f))
		{
			m_bFarPattern1 = false;
			m_fFarPattern1Time = 0.f;
		}
	}
	if (m_bFarPattern2)
	{
		if (::CoolTime(_fDeltaTime, m_fFarPattern2Time, 5.f))
		{
			m_bFarPattern2 = false;
			m_fFarPattern2Time = 0.f;
		}
	}
	if (m_bFarPattern3)
	{
		if (::CoolTime(_fDeltaTime, m_fFarPattern3Time, 10.f))
		{
			m_bFarPattern3 = false;
			m_fFarPattern3Time = 0.f;
		}
	}
	if (m_bNearPattern2)
	{
		if (::CoolTime(_fDeltaTime, m_fNearPattern2Time, 7.f))
		{
			m_bNearPattern2 = false;
			m_fNearPattern2Time = 0.f;
		}
	}
	if (m_bNearPattern3)
	{
		if (::CoolTime(_fDeltaTime, m_fNearPattern3Time, 5.f))
		{
			m_bNearPattern3 = false;
			m_fNearPattern3Time = 0.f;
		}
	}
	if (!m_bPhase2On)
	{
		if (m_tState.fHP < m_tState.fMaxHP*0.5f)
		{
			m_bPhase2On = true;
			m_fAddAniPlayTime = 1.7f;
		}
	}
}

CJMalkhel * CJMalkhel::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJMalkhel* pInstance = new CJMalkhel(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJMalkhel::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CJMalkhel* pClone = new CJMalkhel(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJMalkhel::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pBossUI);
		for (_uint i = 0; i < 6; ++i)
		{
			Safe_Release(m_pArrWingDistort_TrailEffect[i]);
		}

		for (_uint i = 0; i < 3; ++i)
			Safe_Release(m_pFireBallObject[i]);

		for(_uint i = 0 ; i < 12; ++i)
			Safe_Release(m_pSpearObject[i]);

		Safe_Release(m_pFireBallBackObject);
		Safe_Release(m_pEffect360);
		Safe_Release(m_pLightEffect);
		Safe_Release(m_pTeleportDistortionTex);
	}
	CJBaseBoss::Free();
}

void CJMalkhel::DisortionScaleFix(const _float & _fDeltaTime)
{
	if(!m_pTeleportDistortionTex)
		return;


	if (m_pTeleportDistortionTex->Get_RenderSwitch())	//켜져 있을 때
	{
		m_fDistortionTime += _fDeltaTime*50.f;
		_float3 vScale = { 30.f,30.f,30.f };
		if (m_fDistortionTime >= vScale.x)
			m_pTeleportDistortionTex->Set_RenderSwitch(false);
		vScale.x -= m_fDistortionTime;
		vScale.y -= m_fDistortionTime;
		vScale.z -= m_fDistortionTime;
		if (vScale.x <= 0.f)
			vScale.x = 0.f;
		if (vScale.y <= 0.f)
			vScale.y = 0.f;
		if (vScale.z <= 0.f)
			vScale.z = 0.f;
		m_pTeleportDistortionTex->Set_Scale(vScale);
		m_pTeleportDistortionTex->Set_Tileling(10.f);
	}
	else //꺼져있을 때
	{
		m_pTeleportDistortionTex->Set_Scale(_float3(20.f, 20.f, 20.f));
		m_fDistortionTime = 0.f;
	}
}

void CJMalkhel::Near_Pattern_2_Effect()
{
	_float3 vThisPos = m_pTransform->Get_TransformDesc().vPos;
	_float3 vLook = *(_float3*)&m_pTransform->Get_TransformDesc().matWorld.m[2][0];
	D3DXVec3Normalize(&vLook, &m_vLook);
	vLook *= -5.f;
	_float3 vPos = vThisPos+ vLook;
	_float4 vLightningColor = _float4(*m_pWeapon->Get_TrailColor(), 1.f);
	Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, 1.5f
		, &vPos
		, 200,
		vLightningColor,
		(ePartilcePatternType)(
		(_ulong)ePartilcePatternType::OnStartBlink
			/*	| (_ulong)ePartilcePatternType::RevRotY*/
			| (_ulong)ePartilcePatternType::x2),
		0, 1.f, D3DX_PI*2.f, 5.f);
	Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, 0.5f
		, &vPos
		, 100,
		vLightningColor,
		(ePartilcePatternType)(
		(_ulong)ePartilcePatternType::OnStartBlink
			/*	| (_ulong)ePartilcePatternType::RevRotY*/
			| (_ulong)ePartilcePatternType::x2),
		0, 1.f, D3DX_PI*2.f, 5.f);
	Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, 1.f
		, &vPos
		, 100,
		vLightningColor,
		(ePartilcePatternType)(
		(_ulong)ePartilcePatternType::OnStartBlink
			/*	| (_ulong)ePartilcePatternType::RevRotY*/
			| (_ulong)ePartilcePatternType::x2),
		0, 1.f, D3DX_PI*2.f, 5.f);
	/*			Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, vPos, 1.5f
					, &vPos
					, 250,
					_float4(0.3f, 0.3f, 0.3f, 1.f),
					(ePartilcePatternType)(
					(_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::GoStraight
						| (_ulong)ePartilcePatternType::RevRotY
						| (_ulong)ePartilcePatternType::RevRotX
						| (_ulong)ePartilcePatternType::RevRotZ),
					0, 1.f, D3DX_PI*2.f, 5.f);*/
	Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, vPos, 1.5f
		, &vPos
		, 100,
		vLightningColor,
		(ePartilcePatternType)(
		(_ulong)ePartilcePatternType::OnStartBlink
			| (_ulong)ePartilcePatternType::GoStraight
			| (_ulong)ePartilcePatternType::Gravity
			/*| (_ulong)ePartilcePatternType::x2*/),
		0, 2.f, D3DX_PI*2.f, 15.f);
	vPos.y += 1.f;
	Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, vPos, 1.5f
		, &vPos
		, 100,
		vLightningColor,
		(ePartilcePatternType)(
		(_ulong)ePartilcePatternType::OnStartBlink
			| (_ulong)ePartilcePatternType::GoStraight
			| (_ulong)ePartilcePatternType::Gravity
			/*| (_ulong)ePartilcePatternType::x2*/),
		0, 2.f, D3DX_PI*2.f, 15.f);
	vPos.y += 1.f;
	Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, vPos, 1.5f
		, &vPos
		, 100,
		vLightningColor,
		(ePartilcePatternType)(
		(_ulong)ePartilcePatternType::OnStartBlink
			| (_ulong)ePartilcePatternType::GoStraight
			| (_ulong)ePartilcePatternType::Gravity
			/*| (_ulong)ePartilcePatternType::x2*/),
		0, 2.f, D3DX_PI*2.f, 15.f);
	Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, 1.5f
		, &vPos
		, 200,
		vLightningColor,
		(ePartilcePatternType)(
		(_ulong)ePartilcePatternType::GoStraight
			| (_ulong)ePartilcePatternType::OnStartBlink
			| (_ulong)ePartilcePatternType::Gravity),
		0, 1.f, D3DX_PI*2.f, 10.f);
	m_pLightEffect->Set_Pos(vPos);
	m_pLightEffect->Set_RenderSwitch();
}

HRESULT CJMalkhel::Clone_LightEffect()
{
	CEffect_Texture_Ani::TEXOBJANI_DESC tTexDescAni;
	tTexDescAni.vArgb = _float4(*m_pWeapon->Get_TrailColor(), 1.f);
	tTexDescAni.tTransformDesc.vPos = { 0.f, 0.f, 0.f };
	tTexDescAni.tTransformDesc.vScale = { 5.f,10.f,1.f };
	tTexDescAni.eMovePattern = ePartilcePatternType::YBillBoard;
	
	tTexDescAni.bPlayRepeat = true;
	tTexDescAni.iRenderId = 0;
	tTexDescAni.iMaxRenderId = 15;
	tTexDescAni.fPlaySpeed = 1.f;
	lstrcpy(tTexDescAni.szTexName, L"Component_Texture_Lightning1");

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"GameObject_Effect_Tex_Ani_Obj", (CGameObject**)&m_pLightEffect, &tTexDescAni), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pLightEffect, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pLightEffect);

	m_pLightEffect->Set_RenderSwitch(false);
	return S_OK;
}

void CJMalkhel::LightCoolTime(const _float & _fDeltaTime)
{
	if (m_pLightEffect->Get_RenderSwitch() && ::CoolTime(_fDeltaTime, m_fLightTime, 0.1f))
	{
		m_fLightTime = 0.f;
		m_pLightEffect->Set_RenderSwitch(false);
	}
}
