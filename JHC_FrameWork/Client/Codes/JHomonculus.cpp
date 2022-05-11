#include "stdafx.h"
#include "JHomonculus.h"
#include "UI_NormalMonster.h"

CJHomonculus::CJHomonculus(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CJBaseMonster(_pDevice, _PrototypeName)
	, m_ePattern(PATTERN::End)
	, m_bWalkPattern(false)
	, m_bChangeDefuse(true)
	, m_bEffect1(true)
	, m_bEffect2(true)
	, m_fLeftHands(0.f)
	,m_fRightHands(0.f)
{
}

CJHomonculus::CJHomonculus(const CJHomonculus & _rOther)
	: CJBaseMonster(_rOther)
	, m_ePattern(PATTERN::End)
	, m_bWalkPattern(false)
	, m_bChangeDefuse(true)
	, m_bEffect1(true)
	, m_bEffect2(true)
	, m_fLeftHands(0.f)
	, m_fRightHands(0.f)
{
}

HRESULT CJHomonculus::Ready_GameObject_Prototype()
{
	if (FAILED(CJBaseMonster::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJHomonculus::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseMonster::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(CJHomonculus::UI_Clone()))
	{
		ERROR_BOX(L"Failed to UI_Clone");
		return E_FAIL;
	}

	m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);
	m_iShaderPass = 2;
	m_fLeftHands = 6.f;
	m_fRightHands = 8.f;
	m_tState.fAtt = 30.f;
	m_tState.fDef = 30.f;
	m_tState.fMaxHP = 250.f;
	m_tState.fHP = m_tState.fMaxHP;
	m_fAddAniPlayTime = 1.3f;
	m_fFindDistance = 27.f;

	wstring PrototypeName = m_tTransformDesc.szFileTitle;
	_uint count = (_uint)PrototypeName.find(L"_", 0);
	wstring wstrinteger = PrototypeName;
	wstrinteger.erase(0, count + 1);
	_int iIndex = stoi(wstrinteger);
	m_iSoundIndex = iIndex % 3;

	*const_cast<_float*>(&m_fSleepTime_Const) = -1.f;

	auto pManagement = GET_MANAGEMENT;

	return S_OK;
}

_uint CJHomonculus::Update_GameObject(const _float & _fDeltaTime)
{
	CJBaseMonster::Update_GameObject(_fDeltaTime);
	if (m_iSceneIdx == (_uint)ESceneType::Stage2&&m_bChangeDefuse)
	{
		DiffuseChange();
	}

	if (eMonsterPattern::Sleep == m_ePreTotalPattern)
		CJHomonculus::WeaponDelete_RigidBody();

	if( eMonsterPattern::Walk == m_ePreTotalPattern ||
		eMonsterPattern::Idle == m_ePreTotalPattern ||
		eMonsterPattern::Hit == m_ePreTotalPattern ||
		eMonsterPattern::Attack == m_ePreTotalPattern)
		Set_Homonculus_Weapon_Matrix();

	return m_iEvent;
}

HRESULT CJHomonculus::Render_GameObject()
{
	CJBaseMonster::Render_GameObject();

	return S_OK;
}

void CJHomonculus::Do_Activation(const _float & _fDeltaTime)
{
	CJBaseMonster::Do_Activation(_fDeltaTime);
	
	CJHomonculus::WeaponCreate_RigidBody();
}

void CJHomonculus::Do_Idle(const _float & _fDeltaTime)
{
	m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);

	m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

	if (m_fTargetDist < m_fFindDistance)
	{
		m_bTargeting = true;
		m_bTargetLook = true;
		m_eCurTotalPattern = eMonsterPattern::Walk;
		m_ePattern = PATTERN::Pattern_Ready;
	}
}

void CJHomonculus::Do_Attack(const _float & _fDeltaTime)
{
	if (m_ePattern != PATTERN::Pattern_Ready)
	{
		switch (m_ePattern)
		{
		case PATTERN::Near_1:
			Attack_Pattern_1(_fDeltaTime);
			break;
		case PATTERN::Near_2:
			Attack_Pattern_2(_fDeltaTime);
			break;
		case PATTERN::Near_3:
			Attack_Pattern_3(_fDeltaTime);
			break;
		default:
			break;
		}
	}
	else
	{
		if (!m_bAttackCoolTime)
		{
			if (m_fTargetDist <= 3.5f)
			{
				switch (Mersen_ToInt(0, 2))
				{
				case 0:
					Attack_Pattern_1(_fDeltaTime);
					break;
				case 1:
					Attack_Pattern_2(_fDeltaTime);
					break;
				case 2:
					Attack_Pattern_3(_fDeltaTime);
					break;
				}
			}
			else
			{
				m_eCurTotalPattern = eMonsterPattern::Walk;
			}
		}
	}
}

void CJHomonculus::Do_Hit(const _float & _fDeltaTime)
{
}

void CJHomonculus::Do_Walk(const _float & _fDeltaTime)
{
	if (m_ePattern != PATTERN::Pattern_Ready)
	{
		switch (m_ePattern)
		{
		case PATTERN::Walk_F:
			Walk_Pattern_F(_fDeltaTime);
			break;
		case PATTERN::Walk_L:
			Walk_Pattern_L(_fDeltaTime);
			break;
		case PATTERN::Walk_R:
			Walk_Pattern_R(_fDeltaTime);
			break;
		}
	}
	else
	{
		_uint iMovePattern = 0;
		if (m_fTargetDist <= 3.5f)
			iMovePattern = 3;
		else if (m_fTargetDist <= 6.f && m_fTargetDist > 3.5f)
			iMovePattern = Mersen_ToInt(0, 2);
		else
			iMovePattern = 0;

		switch (iMovePattern)
		{
		case 0:
			Walk_Pattern_F(_fDeltaTime);
			break;
		case 1:
			Walk_Pattern_L(_fDeltaTime);
			break;
		case 2:
			Walk_Pattern_R(_fDeltaTime);
			break;
		default:
			m_eCurTotalPattern = eMonsterPattern::Attack;
			break;
		}
	}
}

void CJHomonculus::Do_Other(const _float & _fDeltaTime)
{
	if (m_ePreTotalPattern == eMonsterPattern::Sleep)
	{
		if (::CoolTime(_fDeltaTime, m_fSleepCountTime, m_fSleepTime_Const))
		{
			m_fDissolveTime = 0.f;
			m_eCurTotalPattern = eMonsterPattern::Ready;
			m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);
			m_fSleepCountTime = 0.f;
			m_fAddAniPlayTime = 1.3f;
		}
	}
	else if (m_ePreTotalPattern == eMonsterPattern::Dead)
	{
		m_fDissolveTime += m_fDeltaTime * powf(1.5f, m_fDissolveTime);
		m_fDissolveSpeed = 0.1f;
		Death_Pattern(_fDeltaTime);
	}
}

void CJHomonculus::Get_Damaged()
{
	_uint iCount = Mersen_ToInt(0, 2);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Homonculus_Damage%d.wav", iCount);

	CSound_Manager::ESoundID eSound;
	switch (m_iSoundIndex)
	{
	case 0:
		eSound = CSound_Manager::ESoundID::Monster_GetHit3D_0;
		break;
	case 1:
		eSound = CSound_Manager::ESoundID::Monster_GetHit3D_1;
		break;
	case 2:
		eSound = CSound_Manager::ESoundID::Monster_GetHit3D_2;
		break;
	}
	GET_MANAGEMENT->StopSound(eSound);
	if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
	{
		if (m_bCamMode)
			GET_MANAGEMENT->PlaySounds(szFileName, m_eSound, 1.f);
		else
			GET_MANAGEMENT->Play3DSounds(szFileName, m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);
	}
}

void CJHomonculus::Attack_Pattern_1(const _float & _fDeltaTime)
{
	// 빠른 공격
	auto pManagement = GET_MANAGEMENT;
	CSound_Manager::ESoundID eSound;
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Idle)
	{
		m_ePattern = PATTERN::Near_1;
		m_pMesh->Set_Animationset((_uint)HomonculusAniID::Attack1);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bAttackSound = true;
		m_bEffect1 = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Attack1)
	{
		m_bTargetLook = false;
		
		if (m_pMesh->Get_AnimationFrame(35, 4) && !m_pMesh->Get_AnimationFrame(35, 12))
		{
			Set_Attack(true, false);
			if (m_bAttackSound)
			{
				AttackSound();
				m_bAttackSound = false;
			}
			if (m_pMesh->Get_AnimationFrame(44, 8))
			{
				if (m_bEffect1)
				{
					switch (m_iSoundIndex)
					{
					case 0:
						eSound = CSound_Manager::ESoundID::Monster_Effect3D_0;
						break;
					case 1:
						eSound = CSound_Manager::ESoundID::Monster_Effect3D_1;
						break;
					case 2:
						eSound = CSound_Manager::ESoundID::Monster_Effect3D_2;
						break;
					}
					GET_MANAGEMENT->StopSound(eSound);
					if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
					{
						if (m_bCamMode)
							GET_MANAGEMENT->PlaySounds(L"Homon_AttackEffect_Normal.ogg", m_eSound, 1.f);
						else
							GET_MANAGEMENT->Play3DSounds(L"Homon_AttackEffect_Normal.ogg", eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);
					}

					m_bEffect1 = true;
				}
			}
		}
		else
		{
			Set_Attack(false, false);
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Effect3D_0);
			m_bAttackCoolTime = true;
			m_bTargetLook = true;
			m_eCurTotalPattern = eMonsterPattern::Walk;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);
			switch (m_iSoundIndex)
			{
			case 0:
				eSound = CSound_Manager::ESoundID::Monster_Effect3D_0;
				break;
			case 1:
				eSound = CSound_Manager::ESoundID::Monster_Effect3D_1;
				break;
			case 2:
				eSound = CSound_Manager::ESoundID::Monster_Effect3D_2;
				break;
			}
			GET_MANAGEMENT->StopSound(eSound);
		}
	}
}

void CJHomonculus::Attack_Pattern_2(const _float & _fDeltaTime)
{
	// 강공격
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Idle)
	{
		m_ePattern = PATTERN::Near_2;
		m_pMesh->Set_Animationset((_uint)HomonculusAniID::Attack2);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bAttackSound = true;
		m_bEffect1 = true;
		m_bEffect2 = true;
	}
	else if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Attack2)
	{
		m_bTargetLook = false;
		
		if (m_pMesh->Get_AnimationFrame(44, 10) && !m_pMesh->Get_AnimationFrame(44, 18))
		{
			Set_Attack(true, true);
			if (m_bAttackSound)
			{
				AttackSound();
				m_bAttackSound = false;
			}
		}
		else
		{
			Set_Attack(false, false);
		}

		if (m_pMesh->Get_AnimationFrame(44, 7))
		{
			if (m_bEffect1)
			{
				EffectSound();
				m_bEffect1 = false;
			}
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Walk;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);
		}
	}
}

void CJHomonculus::Attack_Pattern_3(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	// 연속 공격
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Idle)
	{
		m_ePattern = PATTERN::Near_3;
		m_pMesh->Set_Animationset((_uint)HomonculusAniID::Attack3);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bAttackSound = true;
		m_bEffect1 = true;
		m_bEffect2 = true;

	}
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Attack3)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(106, 35) && !m_pMesh->Get_AnimationFrame(106, 41))
		{
			Set_Attack(true, true);

			if (m_bAttackSound)
			{
				AttackSound();
				m_bAttackSound = false;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(106, 56) && !m_pMesh->Get_AnimationFrame(106, 66))
		{
			Set_Attack(false, true);

			if (m_bAttackSound)
			{
				AttackSound();
				m_bAttackSound = false;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(106, 78) && !m_pMesh->Get_AnimationFrame(106, 83))
		{
			Set_Attack(true, false);



			if (m_bAttackSound)
			{
				AttackSound();
				m_bAttackSound = false;
			}
		}
		else
		{
			Set_Attack(false, false);
			m_bAttackSound = true;

		}


		if (m_pMesh->Get_AnimationFrame(106, 78))
		{
			if (m_bEffect1)
			{
				EffectSound();
				m_bEffect1 = false;
				m_bEffect2 = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(106, 56))
		{

			if (m_bEffect2)
			{
				m_bEffect1 = true;
				m_bEffect2 = false;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(106, 36))
		{
			if (m_bEffect1)
			{
				EffectSound();
				m_bEffect1 = false;
				m_bEffect2 = true;
			}
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Walk;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);
		}
	}
}

void CJHomonculus::Walk_Pattern_F(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_F;
		m_pMesh->Set_Animationset((_uint)HomonculusAniID::WalkFront);
		m_fSkillMoveSpeed = 3.f;
		m_bRightLeg = true;

	}
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::WalkFront)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));

		if (m_pMesh->Get_AnimationFrame(51, 49))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(51, 19))
		{
			if (m_bRightLeg)
			{
				Move_Sound();
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_fTargetDist < 3.f)
		{
			m_fWalkMoveTime = 0.f;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);
			m_eCurTotalPattern = eMonsterPattern::Attack;
		}
	}
}

void CJHomonculus::Walk_Pattern_L(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_L;
		m_pMesh->Set_Animationset((_uint)HomonculusAniID::WalkLeft);
		m_fSkillMoveSpeed = 3.f;
		m_bRightLeg = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::WalkLeft)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vRight, m_fSkillMoveSpeed, _fDeltaTime));
		
		m_fWalkMoveTime += _fDeltaTime;

		if (m_pMesh->Get_AnimationFrame(51, 49))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(51, 19))
		{
			if (m_bRightLeg)
			{
				Move_Sound();
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_fTargetDist>3.5f)
		{
			if (m_fWalkMoveTime>3.f)
			{
				m_fWalkMoveTime = 0.f;
				m_ePattern = PATTERN::Walk_F;
				m_pMesh->Set_Animationset((_uint)HomonculusAniID::WalkFront);
				m_eCurTotalPattern = eMonsterPattern::Walk;
			}
		}
		else
		{
			m_fWalkMoveTime = 0.f;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);
			m_eCurTotalPattern = eMonsterPattern::Attack;
		}

	}

}

void CJHomonculus::Walk_Pattern_R(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_R;
		m_pMesh->Set_Animationset((_uint)HomonculusAniID::WalkRight);
		m_fSkillMoveSpeed =3.f;
		//m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bRightLeg = true;
	}
	else if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::WalkRight)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vRight, m_fSkillMoveSpeed, _fDeltaTime));
		m_fWalkMoveTime += _fDeltaTime;

		if (m_pMesh->Get_AnimationFrame(51, 49))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(51, 19))
		{
			if (m_bRightLeg)
			{
				Move_Sound();
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}


		if (m_fTargetDist < 3.f)
		{
			m_fWalkMoveTime = 0.f;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);
			m_eCurTotalPattern = eMonsterPattern::Attack;
		}
		if (m_fWalkMoveTime>3.f)
		{
			m_fWalkMoveTime = 0.f;
			m_ePattern = PATTERN::Walk_F;
			m_pMesh->Set_Animationset((_uint)HomonculusAniID::WalkFront);
			m_eCurTotalPattern = eMonsterPattern::Walk;
		}
	}

}

HRESULT CJHomonculus::UI_Clone()
{
	auto pManagement = GET_MANAGEMENT;
	CUI_NormalMonster::tagMobUIInfo tDesc;
	tDesc.tUIDesc.vPos = _float3(0.f, 0.f, 0.f);
	tDesc.tUIDesc.vSize = _float2(4.f, 0.1f);
	tDesc.tStatDesc = m_tState;
	tDesc.pFromObj = this;
	if (FAILED(pManagement->Clone_GameObject_ToLayer(
		(_uint)ESceneType::Static,
		L"GameObject_UI_MonsterBar",
		m_iSceneIdx,
		L"Layer_MonsterUI", &tDesc)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

void CJHomonculus::Hit_Pattern(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Hit)
	{
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)HomonculusAniID::Idle);
		}
	}
}

void CJHomonculus::Death_Pattern(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() != (_uint)HomonculusAniID::Death)
	{
		m_pMesh->Set_Animationset((_uint)HomonculusAniID::Death);
		m_fAddAniPlayTime = 0.4f;
		
		
		switch (m_iSoundIndex)
		{
		case 0:
			m_eSound = CSound_Manager::ESoundID::Monster_Death3D_0;
			break;
		case 1:
			m_eSound = CSound_Manager::ESoundID::Monster_Death3D_1;
			break;
		case 2:
			m_eSound = CSound_Manager::ESoundID::Monster_Death3D_2;
			break;
		}
		GET_MANAGEMENT->StopSound(m_eSound);
		if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
		{
			if (m_bCamMode)
				GET_MANAGEMENT->PlaySounds(L"HomonCulus_Dead.wav", m_eSound, 1.f);
			else
				GET_MANAGEMENT->Play3DSounds(L"HomonCulus_Dead.wav", m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)HomonculusAniID::Death)
	{
		if (m_pMesh->Is_AnimationSetEnd())
			m_fAddAniPlayTime = 0.f;

		if (m_fDissolveTime > 8.f)
		{
			m_eCurTotalPattern = eMonsterPattern::Sleep;
			GET_MANAGEMENT->StopSound(m_eSound);
		}

	}
}

void CJHomonculus::WeaponCreate_RigidBody()
{
	m_WeaponTitle[0] = (wstring)m_tTransformDesc.szFileTitle + L"_Right_Weapon";
	m_WeaponTitle[1] = (wstring)m_tTransformDesc.szFileTitle + L"_Left_Weapon";

	_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
	_float fDamage = m_tState.fAtt * 0.5f - m_tState.fDef * 0.25f + m_fRightHands;
	Get_Manager()->Add_Weapon(m_WeaponTitle[0], vPos, fDamage, PxSphereGeometry(1.f), 1.5f);
	
	fDamage = m_tState.fAtt * 0.5f - m_tState.fDef * 0.25f + m_fLeftHands;
	Get_Manager()->Add_Weapon(m_WeaponTitle[1], vPos, fDamage, PxSphereGeometry(1.f), 1.5f);
}

void CJHomonculus::WeaponDelete_RigidBody()
{
	for (_uint i = 0; i < 2; ++i)
	{
		Get_Manager()->Delete_Trigger(m_WeaponTitle[i]);
	}
}

void CJHomonculus::Set_Attack(_bool _bLeft, _bool _bRight)
{
	void* Data;
	Get_Manager()->GetData_Trigger(m_WeaponTitle[1], &Data);
	MyPublicData* WeaponData = (MyPublicData*)Data;
	WeaponData->Collision = _bLeft;
	Get_Manager()->SetData_Trigger(m_WeaponTitle[1], WeaponData);

	Data;
	Get_Manager()->GetData_Trigger(m_WeaponTitle[0], &Data);
	WeaponData = (MyPublicData*)Data;
	WeaponData->Collision = _bRight;
	Get_Manager()->SetData_Trigger(m_WeaponTitle[0], WeaponData);
}

void CJHomonculus::Set_Homonculus_Weapon_Matrix()
{
	_float4x4 matLeftHand, matRightHand;

	const MY_FRAME* pMonsterFrame = m_pMesh->Get_FrameByName("B_Index_finger_L_1");
	const MY_FRAME* pMonsterFrame2 = m_pMesh->Get_FrameByName("B_Index_finger_R_1");

	_float4x4 matLeftBone = pMonsterFrame->CombinedTranformationMatrix;
	_float4x4 matRightBone = pMonsterFrame2->CombinedTranformationMatrix;
	_float4x4 matHomonWorld = *m_pTransform->Get_WorldMatrix();

	m_matWeapon[0] = matRightBone * matHomonWorld;
	m_matWeapon[1] = matLeftBone  * matHomonWorld;	

	for (_uint i = 0; i < 2; ++i)
	{
		_float3 vScale;
		_float4Q vQuat;
		D3DXMatrixDecompose(&vScale, &vQuat, &m_vWeaponPos[i], &m_matWeapon[i]);

		Get_Manager()->Weapon_Update(m_WeaponTitle[i], m_vWeaponPos[i], _float4Q(0.f, 0.f, 0.f, 1.f));
	}
}

void CJHomonculus::DiffuseChange()
{
	wstring PrototypeName = m_tTransformDesc.szFileTitle;
	_uint count = (_uint)PrototypeName.find(L"_", 0);
	wstring wstrinteger = PrototypeName;
	wstrinteger.erase(0, count + 1);
	_int iIndex = stoi(wstrinteger);

	m_fFarRange = 150.f;

	if (iIndex == 2)
	{
		m_fLeftHands = 8.f;
		m_fRightHands = 12.f;
		m_tState.fAtt = 60.f;
		m_tState.fDef = 45.f;
		m_tState.fMaxHP = 300.f;
		m_tState.fHP = m_tState.fMaxHP;
		m_iDiffuseIndex = 1;
		m_iDropGoldMax = 600;
		m_iDropGoldMin = 300;
		m_iGold = Mersen_ToInt(m_iDropGoldMin, m_iDropGoldMax);
	}
	m_bChangeDefuse = false;
}

void CJHomonculus::AttackSound()
{
	switch (m_iSoundIndex)
	{
	case 0:
		m_eSound = CSound_Manager::ESoundID::Monster_Parry3D_0;
		break;
	case 1:
		m_eSound = CSound_Manager::ESoundID::Monster_Parry3D_1;
		break;
	case 2:
		m_eSound = CSound_Manager::ESoundID::Monster_Parry3D_2;
		break;
	}
	_uint iCount = Mersen_ToInt(0, 2);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Homonculus_Attack%d.ogg", iCount);
	GET_MANAGEMENT->StopSound(m_eSound);
	if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
	{
		if (m_bCamMode)
			GET_MANAGEMENT->PlaySounds(szFileName, m_eSound, 1.f);
		else
			GET_MANAGEMENT->Play3DSounds(szFileName, m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);
	}
}

void CJHomonculus::Move_Sound()
{
	switch (m_iSoundIndex)
	{
	case 0:
		m_eSound = CSound_Manager::ESoundID::Monster_Move3D_0;
		break;
	case 1:
		m_eSound = CSound_Manager::ESoundID::Monster_Move3D_1;
		break;
	case 2:
		m_eSound = CSound_Manager::ESoundID::Monster_Move3D_2;
		break;
	}

	_uint iCount = Mersen_ToInt(0, 6);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Homonculus_Walk%d.ogg", iCount);
	GET_MANAGEMENT->StopSound(m_eSound);
	if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
	{
		if (m_bCamMode)
			GET_MANAGEMENT->PlaySounds(szFileName, m_eSound, 1.f);
		else
			GET_MANAGEMENT->Play3DSounds(szFileName, m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);
	}
}

void CJHomonculus::EffectSound()
{
	switch (m_iSoundIndex)
	{
	case 0:
		m_eSound = CSound_Manager::ESoundID::Monster_Effect3D_0;
		break;
	case 1:
		m_eSound = CSound_Manager::ESoundID::Monster_Effect3D_1;
		break;
	case 2:
		m_eSound = CSound_Manager::ESoundID::Monster_Effect3D_2;
		break;
	}
	_uint iCount = Mersen_ToInt(0, 5);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Homon_AttackEffect%d.ogg", iCount);
	GET_MANAGEMENT->StopSound(m_eSound);
	if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
	{
		if (m_bCamMode)
			GET_MANAGEMENT->PlaySounds(szFileName, m_eSound, 1.f);
		else
			GET_MANAGEMENT->Play3DSounds(szFileName, m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);
	}
}

CJHomonculus * CJHomonculus::Create(_DEVICE _pDevice, const wstring & _PrototypeName)
{
	CJHomonculus* pInstance = new CJHomonculus(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJHomonculus::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CJHomonculus* pClone = new CJHomonculus(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}
