#include "stdafx.h"
#include "JLegionnaire.h"
#include "JLegionnaireGun.h"
#include "UI_NormalMonster.h"
#include "Player.h"

#define MONSTER_CHECK_ACTIVATION (m_ePreTotalPattern == eMonsterPattern::Awake ||		\
m_ePreTotalPattern == eMonsterPattern::Walk ||											\
m_ePreTotalPattern == eMonsterPattern::Idle ||											\
m_ePreTotalPattern == eMonsterPattern::Hit ||											\
m_ePreTotalPattern == eMonsterPattern::Attack||											\
m_ePreTotalPattern == eMonsterPattern::Dead )

CJLegionnaire::CJLegionnaire(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CJBaseMonster(_pDevice, _PrototypeName)
	, m_ePattern(PATTERN::End)
	, m_bWalkPattern(false)
	, m_eBattleMode(BATTLEMODE::End)
	, m_bAttackPattern2(false)
	, m_bAttackPattern3(false)
	, m_bAttackPattern4(false)
	, m_bAttackPattern5(false)
	, m_fAttackPattern2(0.f)
	, m_fAttackPattern3(0.f)
	, m_fAttackPattern4(0.f)
	, m_fAttackPattern5(0.f)
	, m_fGunNearAttacking(0.f)
	, m_bGunNearStart(false)
	, m_bGunNearAttacking(false)
	, m_iAttackCount(0)
	, m_iAttackCountEnd(0)
	, m_bBulletReady(0)
	, m_bBulletShoot(0)
{
}

CJLegionnaire::CJLegionnaire(const CJLegionnaire & _rOther)
	: CJBaseMonster(_rOther)
	, m_ePattern(PATTERN::End)
	, m_eBattleMode(BATTLEMODE::End)
	, m_bAttackPattern2(false)
	, m_bAttackPattern3(false)
	, m_bAttackPattern4(false)
	, m_bAttackPattern5(false)
	, m_fAttackPattern2(0.f)
	, m_fAttackPattern3(0.f)
	, m_fAttackPattern4(0.f)
	, m_fAttackPattern5(0.f)
	, m_fGunNearAttacking(0.f)
	, m_bGunNearStart(false)
	, m_bGunNearAttacking(false)
	, m_iAttackCount(0)
	, m_iAttackCountEnd(0)
	, m_bBulletReady(0)
	, m_bBulletShoot(0)
{
}

HRESULT CJLegionnaire::Ready_GameObject_Prototype()
{
	if (FAILED(CJBaseMonster::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJLegionnaire::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseMonster::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	m_iShaderPass = 2;

	m_tState.fAtt = 40.f;
	m_tState.fDef = 20.f;
	m_tState.fMaxHP = 80.f;
	m_tState.fHP = m_tState.fMaxHP;

	m_iAttackCountEnd = Mersen_ToInt(0, 3);

	m_fAddAniPlayTime = 1.3f;

	m_fActivateTimer = 4.f;
	*const_cast<_float*>(&m_fSleepTime_Const) = -1.f;
	
	m_iItemCreateYAxisPosFix = 1.f;
	auto pManagement = GET_MANAGEMENT;
	

	wstring PrototypeName = m_tTransformDesc.szFileTitle;
	_uint count = (_uint)PrototypeName.find(L"_", 0);
	wstring wstrinteger = PrototypeName;
	wstrinteger.erase(0, count + 1);
	_int iIndex = stoi(wstrinteger);
	m_iSoundIndex = iIndex % 3;
	switch (iIndex)
	{
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		m_tState.fAtt = 60.f;
		m_tState.fDef = 10.f;
		m_tState.fMaxHP = 60.f;
		m_tState.fHP = m_tState.fMaxHP;
		m_fFindDistance = 50.f;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunIdle);
		m_eBattleMode = BATTLEMODE::GUN;
		m_bTargetYLook = true;
		break;
	default:
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle);
		m_eBattleMode = BATTLEMODE::SWORD;
		m_fFindDistance = 27.f;
		break;
	}

	if (FAILED(CJLegionnaire::Weapon_Clone()))
	{
		ERROR_BOX(L"Failed to Weapon_Clone");
		return E_FAIL;
	}

	if (FAILED(CJLegionnaire::UI_Clone()))
	{
		ERROR_BOX(L"Failed to UI_Clone");
		return E_FAIL;
	}

	return S_OK;
}

_uint CJLegionnaire::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_iSceneIdx == (_uint)ESceneType::Stage2 && m_vecPatrolPos.empty() && eMonsterParent::Normal == m_eParentCategory)
		PatrolMaker();

	if (MONSTER_CHECK_ACTIVATION)
		CJLegionnaire::AttackCoolTimer(_fDeltaTime);
	CJBaseMonster::Update_GameObject(_fDeltaTime);
	return m_iEvent;
}


HRESULT CJLegionnaire::Render_GameObject()
{
	CJBaseMonster::Render_GameObject();

	const MY_FRAME* pMonsterFrame = m_pMesh->Get_FrameByName("Legio-R-Finger32");

	m_matChildWeapon = pMonsterFrame->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();

	return S_OK;
}

void CJLegionnaire::Do_Idle(const _float & _fDeltaTime)
{
	m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));
	_float fYDist = fabs(m_pTargetTransform->Get_TransformDesc().vPos.y - m_pTransform->Get_TransformDesc().vPos.y);

	if (m_eBattleMode == BATTLEMODE::SWORD)
	{
		if (m_fTargetDist < m_fFindDistance && fYDist < 6.f)
		{
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
			m_bTargeting = true;
			m_bTargetLook = true;
			m_eCurTotalPattern = eMonsterPattern::Walk;
			//m_eCurTotalPattern = Mersen_ToInt(0, 1) ? eMonsterPattern::Walk : eMonsterPattern::Attack;
			m_ePattern = PATTERN::Pattern_Ready;
		}
		else
		{
			if (m_iCapacitySize)
			{
				Patrol_Pattern(_fDeltaTime);
			}
		}
	}

	if (m_eBattleMode == BATTLEMODE::GUN)
	{
		if (m_fTargetDist < m_fFindDistance && fYDist < 8.f)
		{
			if (m_fTargetDist <= 6.f && !m_bGunNearStart)
			{
				m_bGunNearStart = true;
			}
			if (m_fTargetDist > 15.f && m_bGunNearStart)
			{
				m_bGunNearStart = false;
				m_bGunNearAttacking = false;
			}

			if (m_bGunNearStart)
			{
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
				m_fAttackCoolTimeEnd = 2.f;
			}
			else
			{
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunIdle);
				m_fAttackCoolTimeEnd = 5.f;
			}
			m_bTargeting = true;
			m_bTargetLook = true;
			if (!m_bAttackCoolTime)
			{
				m_eCurTotalPattern = eMonsterPattern::Attack;
			}
			m_ePattern = PATTERN::Pattern_Ready;
		}
		else
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunIdle);
	}
	
}

void CJLegionnaire::Do_Attack(const _float & _fDeltaTime)
{

	if (m_ePattern != PATTERN::Pattern_Ready)
	{
		//패링 여부 받아오고 그걸로 m_bParried 트루
		if (m_pPlayer->Get_WeaponParry())
		{
			m_ePattern = PATTERN::Parry;
			m_pPlayer->Set_WeaponParry(false);
			m_pWeapon->Set_Acttack(false);
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
			GET_MANAGEMENT->StopSound(m_eSound);
			if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
			{
				if (m_bCamMode)
					GET_MANAGEMENT->PlaySounds(L"Parry_God.mp3", m_eSound, 1.f);
				else
					GET_MANAGEMENT->Play3DSounds(L"Parry_God.mp3", m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);

			}
				
		}
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
		case PATTERN::Far_1:
			Attack_Pattern_4(_fDeltaTime);
			break;
		case PATTERN::Far_2:
			Attack_Pattern_5(_fDeltaTime);
			break;
		case PATTERN::GunShoot:
			Shoot_Pattern(_fDeltaTime);
			break;
		case PATTERN::Parry:
			Parry_Pattern(_fDeltaTime);
			break;
		default:
			break;
		}
	}
	else
	{

		if (!m_bAttackCoolTime)
		{
			if (m_eBattleMode == BATTLEMODE::SWORD)
			{
				if (m_fTargetDist <= 3.5f)
				{
					_uint iAttackNum = 0;
					if (m_bAttackPattern2 && m_bAttackPattern3)
					{
						iAttackNum = 0;
					}
					else if (m_bAttackPattern2 && !m_bAttackPattern3)
					{
						iAttackNum = Mersen_ToInt(0, 1);
						if (iAttackNum)
						{
							iAttackNum++;
						}
					}
					else if (!m_bAttackPattern2 && m_bAttackPattern3)
					{
						iAttackNum = Mersen_ToInt(0, 1);
					}
					else
					{
						iAttackNum = Mersen_ToInt(0, 2);
					}
					switch (iAttackNum)
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
					++m_iAttackCount;
				}
				else if (m_fTargetDist > 3.5f && m_fTargetDist <= 27.f)
				{
					switch (Mersen_ToInt(0, 1))
					{
					case 0:
						if (!m_bAttackPattern4)
						{
							Attack_Pattern_4(_fDeltaTime);
							break;
						}
					case 1:
						if (!m_bAttackPattern5)
						{
							Attack_Pattern_5(_fDeltaTime);
							break;
						}
					default:
						m_eCurTotalPattern = eMonsterPattern::Walk;
						m_ePattern = PATTERN::Pattern_Ready;
						break;
					}
				}
			}
			if (m_eBattleMode == BATTLEMODE::GUN)
			{
				if (m_fTargetDist <= 50.f && m_fTargetDist > 6.f && !m_bGunNearStart)
				{
					Shoot_Pattern(_fDeltaTime);
				}
				if (m_fTargetDist <= 6.f && !m_bGunNearStart)
				{
					m_bGunNearStart = true;
				}
				if (m_fTargetDist > 15.f && m_bGunNearStart)
				{
					m_bGunNearStart = false;
					m_bGunNearAttacking = false;
				}
				if (m_bGunNearStart&&!m_bGunNearAttacking)
				{
					if (::CoolTime(_fDeltaTime,m_fGunNearAttacking,2.f))
					{
						m_bGunNearAttacking = true;
						m_fGunNearAttacking = 0.f;
					}
				}
				if (m_bGunNearStart)
				{
					if (m_fTargetDist <= 2.5f)
					{
						switch (Mersen_ToInt(0, 2))
						{
						case 0:
							Attack_Pattern_1(_fDeltaTime);
							break;
						case 1:
							if (!m_bAttackPattern2)
								Attack_Pattern_2(_fDeltaTime);
							else
								Attack_Pattern_1(_fDeltaTime);
							break;
							++m_iAttackCount;
						}
					}
					else if (m_fTargetDist > 2.5f && m_fTargetDist <= 15.f && m_bGunNearAttacking)
					{
						Attack_Pattern_5(_fDeltaTime);
					}
				}
			}
		}
	}

}

void CJLegionnaire::Do_Hit(const _float & _fDeltaTime)
{
}

void CJLegionnaire::Do_Walk(const _float & _fDeltaTime)
{
	if (m_ePattern != PATTERN::Pattern_Ready)
	{
		switch (m_ePattern)
		{
		case PATTERN::Walk_F:
			Walk_Pattern_F(_fDeltaTime);
			break;
		case PATTERN::Walk_B:
			Walk_Pattern_B(_fDeltaTime);
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
		{
			if (m_iAttackCount > m_iAttackCountEnd)
			{
				if (m_tState.fHP < m_tState.fMaxHP*0.5f)
					iMovePattern = Mersen_ToInt(1, 3);
				else
					iMovePattern = Mersen_ToInt(2, 3);
				m_iAttackCountEnd = Mersen_ToInt(0, 3);
				m_iAttackCount = 0;
			}
			else
				iMovePattern = 4;
		}
		else if (m_fTargetDist <= 8.f && m_fTargetDist > 3.5f)
			iMovePattern = Mersen_ToInt(2, 3);
		else
			iMovePattern = 0;

		switch (iMovePattern)
		{
		case 0:
			Walk_Pattern_F(_fDeltaTime);
			break;
		case 1:
			Walk_Pattern_B(_fDeltaTime);
			break;
		case 2:
			Walk_Pattern_L(_fDeltaTime);
			break;
		case 3:
			Walk_Pattern_R(_fDeltaTime);
			break;
		default:
			m_eCurTotalPattern = eMonsterPattern::Attack;
			break;
		}
	}
}

void CJLegionnaire::Do_Other(const _float & _fDeltaTime)
{
	if (m_ePreTotalPattern == eMonsterPattern::Sleep)
	{
		m_iNowPatrol = 1;
		m_bFriendPatrolReady = true;
		m_bPatrolReady = true;
		//강체 삭제
		//m_fSleepTime += _fDeltaTime;
		if (::CoolTime(_fDeltaTime, m_fSleepCountTime, m_fSleepTime_Const))
		{
			m_fDissolveTime = 0.f;
			m_eCurTotalPattern = eMonsterPattern::Ready;
			if (m_eBattleMode == BATTLEMODE::SWORD)
			{
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle);
			}
			if (m_eBattleMode == BATTLEMODE::GUN)
			{
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunIdle);
			}
			m_fSleepCountTime = 0.f;
		}
	}
	else if (m_ePreTotalPattern == eMonsterPattern::Dead)
	{
		m_fDissolveTime += _fDeltaTime * powf(1.2f, m_fDissolveTime);
		Death_Pattern(_fDeltaTime);
	}
}

void CJLegionnaire::Get_Damaged()
{
	_uint iCount = Mersen_ToInt(0, 2);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Legio_Damage%d.wav", iCount);
	
	switch (m_iSoundIndex)
	{
	case 0:
		m_eSound = CSound_Manager::ESoundID::Monster_GetHit3D_0;
		break;
	case 1:
		m_eSound = CSound_Manager::ESoundID::Monster_GetHit3D_1;
		break;
	case 2:
		m_eSound = CSound_Manager::ESoundID::Monster_GetHit3D_2;
		break;
	}
	GET_MANAGEMENT->StopSound(m_eSound);
	if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
	{
		if (m_bCamMode)
			GET_MANAGEMENT->PlaySounds(szFileName, m_eSound, 1.f);
		else
			GET_MANAGEMENT->Play3DSounds(szFileName, m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);
	}
}

HRESULT CJLegionnaire::Weapon_Clone()
{
	auto pManagement = GET_MANAGEMENT;

	wstring wstrMonsterName = m_tTransformDesc.szFileTitle;

	_uint count = (_uint)wstrMonsterName.find(L"_", 0);
	wstring wstrinteger = wstrMonsterName;
	wstrinteger.erase(0, count + 1);	
	wstrinteger += L"_Weapon";
	
	TRANSFORM_DESC tTransformDesc;


	if (m_eBattleMode == BATTLEMODE::SWORD)
	{
		tTransformDesc.vRotate = _float3(0.f, D3DXToRadian(90.f), 0.f);
		lstrcpy(tTransformDesc.szFileTitle, m_tTransformDesc.szFileTitle);
		lstrcat(tTransformDesc.szFileTitle, L"_LegionnaireSword_");
		lstrcat(tTransformDesc.szFileTitle, wstrinteger.c_str());
		if (FAILED(pManagement->Clone_GameObject(
			(_uint)ESceneType::Static,
			L"GameObject_LegionnaireSword",
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
	}
	if (m_eBattleMode == BATTLEMODE::GUN)
	{
		tTransformDesc.vRotate = _float3(D3DXToRadian(10.f), D3DXToRadian(0.f), D3DXToRadian(-90.f));
		lstrcpy(tTransformDesc.szFileTitle, m_tTransformDesc.szFileTitle);
		lstrcat(tTransformDesc.szFileTitle, L"_LegionnaireGun_");
		lstrcat(tTransformDesc.szFileTitle, wstrinteger.c_str());
		if (FAILED(pManagement->Clone_GameObject(
			(_uint)ESceneType::Static,
			L"GameObject_LegionnaireGun",
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
	}
	m_pWeapon->Set_ParentClass(this,CJBaseWeapon::eMonsterParent::Normal);
	Safe_AddRef(m_pWeapon);

	return S_OK;
}

HRESULT CJLegionnaire::UI_Clone()
{
	auto pManagement = GET_MANAGEMENT;
	CUI_NormalMonster::tagMobUIInfo tDesc;
	tDesc.tUIDesc.vPos = _float3(0.f, 0.f, 0.f);
	tDesc.tUIDesc.vSize = _float2(2.f, 0.1f);
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

HRESULT CJLegionnaire::SetUp_ConstantTable(_EFFECT & _pEffect)
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

	pManagement->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture");

	return S_OK;
}

void CJLegionnaire::Attack_Pattern_1(const _float & _fDeltaTime)
{
	// 1회 베기
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
	{
		m_ePattern = PATTERN::Near_1;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack1);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bGunNearAttacking = true;
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
		m_bAttackSound = true;
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Attack1)
	{
		m_bTargetLook = false;
		if (m_pMesh->Get_AnimationFrame(53, 19) && !m_pMesh->Get_AnimationFrame(53, 25))
			m_pWeapon->Set_Acttack(true);
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(53, 19))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackCoolTime = true;
			m_bAttackSound = true;
			if (m_eBattleMode == BATTLEMODE::SWORD)
			{
				m_eCurTotalPattern = eMonsterPattern::Walk;
				m_bTargetLook = true;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
			}
			else
			{
				if (m_iAttackCount>2)
				{
					m_eCurTotalPattern = eMonsterPattern::Walk;
					m_bTargetLook = true;
					m_ePattern = PATTERN::Walk_B;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Walk_B);
				}
				else
				{
					m_eCurTotalPattern = eMonsterPattern::Idle;
					m_bTargetLook = true;
					m_ePattern = PATTERN::Pattern_Ready;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
				}
			}
		}
	}
}

void CJLegionnaire::Attack_Pattern_2(const _float & _fDeltaTime)
{
	// 1회 베기 + 찍기
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
	{
		m_ePattern = PATTERN::Near_2;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack1);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bGunNearAttacking = true;
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
		m_bAttackSound = true;
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Attack1)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(53, 19) && !m_pMesh->Get_AnimationFrame(53, 25))
			m_pWeapon->Set_Acttack(true);
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(53, 19))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_bTargetLook = true;
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack3);
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Attack3)
	{

		if (m_pMesh->Get_AnimationFrame(56, 15) && !m_pMesh->Get_AnimationFrame(56, 21))
		{
			m_pWeapon->Set_Acttack(true);
			m_bTargetLook = false;
		}
		else
		{
			m_bTargetLook = true;
			m_pWeapon->Set_Acttack(false);
		}

		if (m_pMesh->Get_AnimationFrame(53, 19))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_bAttackCoolTime = true;
			m_bAttackPattern2 = true;
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
			if (m_eBattleMode == BATTLEMODE::SWORD)
			{
				m_eCurTotalPattern = eMonsterPattern::Walk;
				m_bTargetLook = true;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
			}
			else
			{
				if (m_iAttackCount>2)
				{
					m_eCurTotalPattern = eMonsterPattern::Walk;
					m_bTargetLook = true;
					m_ePattern = PATTERN::Walk_B;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Walk_B);
				}
				else
				{
					m_eCurTotalPattern = eMonsterPattern::Idle;
					m_bTargetLook = true;
					m_ePattern = PATTERN::Pattern_Ready;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
				}
			}
		}
	}
}

void CJLegionnaire::Attack_Pattern_3(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	// 2회 베기 + 회피(뒤)
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
	{
		m_ePattern = PATTERN::Near_3;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack2_Start);
		m_fSkillMoveSpeed = 15.f;
		m_bAttackSound = true;
		m_bRightLeg = true;
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Attack2_Start)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(30, 18) && !m_pMesh->Get_AnimationFrame(30, 25))
			m_pWeapon->Set_Acttack(true);
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(30, 19))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack2_Stop);
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
		}

	}
	
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Attack2_Stop)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(56, 17) && !m_pMesh->Get_AnimationFrame(56, 24))
			m_pWeapon->Set_Acttack(true);
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(56, 19))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
		}
		
		if (m_pMesh->Get_AnimationFrame(56, 30) && !m_pMesh->Get_AnimationFrame(56, 47))
		{
			m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
			if (m_bRightLeg)
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
				GET_MANAGEMENT->StopSound(m_eSound);
				if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
				{
					if (m_bCamMode)
						GET_MANAGEMENT->PlaySounds(L"Legio_BackDubling.ogg", m_eSound, 1.f);
					else
						GET_MANAGEMENT->Play3DSounds(L"Legio_BackDubling.ogg", m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
				}
				m_bRightLeg = false;
			}


		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack2_Stop);
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
			m_bAttackCoolTime = true;
			m_bAttackPattern3 = true;

			m_eCurTotalPattern = eMonsterPattern::Walk;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
		}
	}
}

void CJLegionnaire::Attack_Pattern_4(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	// 돌진 베기
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
	{
		m_ePattern = PATTERN::Far_1;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack4);
		m_fSkillMoveSpeed = 12.f;
		m_bRightLeg = true;
		m_bLeftLeg = false;
		m_bAttackSound = true;
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Attack4)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(91, 43) && !m_pMesh->Get_AnimationFrame(91, 54))
			m_pWeapon->Set_Acttack(true);
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(91, 43))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
		}

		_uint iCount = Mersen_ToInt(9, 13);

		if (m_pMesh->Get_AnimationFrame(91, 43))
		{
			if (m_bRightLeg)
			{
				Move_Sound();
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(91, 33))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(91, 25))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;

			}
		}

		if (m_pMesh->Get_AnimationFrame(91, 16) && !m_pMesh->Get_AnimationFrame(91, 45))
		{
			m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack2_Stop);
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
			m_bAttackCoolTime = true;
			m_bAttackPattern4 = true;

			m_eCurTotalPattern = eMonsterPattern::Walk;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);

		}
	}
}

void CJLegionnaire::Attack_Pattern_5(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	// 돌진 베기 + 찍기

	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
	{
		m_ePattern = PATTERN::Far_2;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack4);
		m_fSkillMoveSpeed = 12.f;
		m_bRightLeg = true;
		m_bLeftLeg = false;
	}

	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Attack4)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(91, 43) && !m_pMesh->Get_AnimationFrame(91, 54))
			m_pWeapon->Set_Acttack(true);
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(91, 43))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
		}

		if (m_pMesh->Get_AnimationFrame(91, 16) && !m_pMesh->Get_AnimationFrame(91, 45))
			m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));

		_uint iCount = Mersen_ToInt(9, 13);

		if (m_pMesh->Get_AnimationFrame(91, 43))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(91, 33))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(91, 25))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;

			}
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack2_Stop);
			m_bTargetLook = true;

			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack3);
		}
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Attack3)
	{

		if (m_pMesh->Get_AnimationFrame(56, 15) && !m_pMesh->Get_AnimationFrame(56, 23))
		{
			m_pWeapon->Set_Acttack(true);
			m_bTargetLook = false;
		}
		else
		{
			m_pWeapon->Set_Acttack(false);
			m_bTargetLook = true;
		}

		if (m_pMesh->Get_AnimationFrame(53, 19))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
		}
		
		if (m_pMesh->Is_AnimationSetEnd())
		{
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Attack2_Stop);
			m_bAttackSound = true;
			m_bAttackCoolTime = true;
			m_bAttackPattern5 = true;
			if (m_eBattleMode == BATTLEMODE::SWORD)
			{
				m_eCurTotalPattern = eMonsterPattern::Walk;
				m_bTargetLook = true;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
			}
			else
			{
				if (m_iAttackCount>2)
				{
					m_eCurTotalPattern = eMonsterPattern::Walk;
					m_bTargetLook = true;
					m_ePattern = PATTERN::Walk_B;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Walk_B);
				}
				else
				{
					m_eCurTotalPattern = eMonsterPattern::Idle;
					m_bTargetLook = true;
					m_ePattern = PATTERN::Pattern_Ready;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
				}
			}
		}
	}
}

void CJLegionnaire::Walk_Pattern_B(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
	{
		m_ePattern = PATTERN::Walk_B;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Walk_B);
		m_fSkillMoveSpeed = 5.f;
	}
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Walk_B)
	{
		m_bTargetLook = true;
		
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		
		m_fWalkMoveTime += _fDeltaTime;

		_uint iCount = Mersen_ToInt(9, 13);

		if (m_pMesh->Get_AnimationFrame(41, 35))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(41, 14))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;

			}
		}

		if (m_fWalkMoveTime >= 2.f)
		{
			if (m_eBattleMode == BATTLEMODE::SWORD)
			{
				m_fWalkMoveTime = 0.f;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
				m_eCurTotalPattern = eMonsterPattern::Attack;
			}
			else
			{
				m_eCurTotalPattern = eMonsterPattern::Idle;
				m_bTargetLook = true;
				m_ePattern = PATTERN::Pattern_Ready;
				if (m_fTargetDist>6.f)
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunIdle);
				else
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
			}

		}
	}
}

void CJLegionnaire::Walk_Pattern_F(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
	{
		m_ePattern = PATTERN::Walk_F;

		if (Mersen_ToInt(0, 1))
		{
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Run_F);
			m_fSkillMoveSpeed = 12.f;
		}

		m_fAddAniPlayTime = 1.f;

		if (m_eBattleMode == BATTLEMODE::SWORD)
			m_bWalkPattern = Mersen_ToInt(0, 1) ? true : false;
		else
			m_bWalkPattern = false;
		m_bRightLeg = true;
	}

	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Run_F)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));

		m_fWalkMoveTime += _fDeltaTime;

		_uint iCount = Mersen_ToInt(9, 13);

		if (m_pMesh->Get_AnimationFrame(25, 20))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(25, 9))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;

			}
		}

		if (m_bWalkPattern)
		{
			if (m_fWalkMoveTime >= 1.f || m_fTargetDist <= 3.5f)
			{
				m_fWalkMoveTime = 0.f;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
				m_eCurTotalPattern = eMonsterPattern::Attack;
				m_fAddAniPlayTime = 1.3f;
			}
		}
		else
		{
			if (m_fTargetDist <= 3.5f)
			{
				if (m_eBattleMode == BATTLEMODE::SWORD)
				{
					m_fWalkMoveTime = 0.f;
					m_ePattern = PATTERN::Pattern_Ready;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
					m_eCurTotalPattern = eMonsterPattern::Attack;
				}
				else
				{
					m_eCurTotalPattern = eMonsterPattern::Idle;
					m_bTargetLook = true;
					m_ePattern = PATTERN::Pattern_Ready;
					if (m_fTargetDist > 6.f)
					{
						m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunIdle);
					}
					else
					{
						m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
					}
				}
				m_fAddAniPlayTime = 1.3f;
			}
		}
	}
}
void CJLegionnaire::Walk_Pattern_L(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
	{
		m_ePattern = PATTERN::Walk_L;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Walk_L);
		m_fSkillMoveSpeed = 1.6f;
		m_fAddAniPlayTime = 1.f;
		m_bRightLeg = true;
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Walk_L)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vRight, m_fSkillMoveSpeed, _fDeltaTime));
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vLook, 1.f, _fDeltaTime));

		m_fWalkMoveTime += _fDeltaTime;

		_uint iCount = Mersen_ToInt(9, 13);
		if (m_pMesh->Get_AnimationFrame(40, 37))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(40, 21))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;

			}
		}

		if (m_fWalkMoveTime >= 2.f)
		{
			m_fAddAniPlayTime = 1.3f;

			if (m_eBattleMode == BATTLEMODE::SWORD)
			{
				m_fWalkMoveTime = 0.f;
				m_fSkillMoveSpeed = 15.f;
				m_ePattern = PATTERN::Walk_F;
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Run_F);
				m_eCurTotalPattern = eMonsterPattern::Walk;
			}
			else
			{
				m_bTargetLook = true;
				m_ePattern = PATTERN::Pattern_Ready;
				if (m_fTargetDist>6.f)
				{
					m_eCurTotalPattern = eMonsterPattern::Idle;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunIdle);
				}
				else
				{
					m_eCurTotalPattern = eMonsterPattern::Walk;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
				}
			}
		}
	}
}

void CJLegionnaire::Walk_Pattern_R(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
	{
		m_ePattern = PATTERN::Walk_R;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Walk_R);
		m_fSkillMoveSpeed = 1.6f;
		m_fAddAniPlayTime = 1.f;
		m_bRightLeg = true;
	}

	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Walk_R)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vRight, m_fSkillMoveSpeed, _fDeltaTime));
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vLook, 1.f, _fDeltaTime));
		
		m_fWalkMoveTime += _fDeltaTime;

		_uint iCount = Mersen_ToInt(9, 13);
		if (m_pMesh->Get_AnimationFrame(40, 40))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(40, 20))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_fWalkMoveTime >= 2.f)
		{
			m_fAddAniPlayTime = 1.3f;
			if (m_eBattleMode == BATTLEMODE::SWORD)
			{
				m_fWalkMoveTime = 0.f;
				m_fSkillMoveSpeed = 15.f;
				m_ePattern = PATTERN::Walk_F;
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Run_F);
				m_eCurTotalPattern = eMonsterPattern::Walk;
			}
			else
			{
				m_bTargetLook = true;
				m_ePattern = PATTERN::Pattern_Ready;
				if (m_fTargetDist>6.f)
				{
					m_eCurTotalPattern = eMonsterPattern::Idle;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunIdle);
				}
				else
				{
					m_eCurTotalPattern = eMonsterPattern::Walk;
					m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
				}
			}
		}
	}
}

void CJLegionnaire::Shoot_Pattern(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::GunIdle)
	{
		m_ePattern = PATTERN::GunShoot;
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunShoot);
		m_bBulletReady = true;
		m_bBulletShoot = true;
	}

	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::GunShoot)
	{
		m_bTargetLook = true;

		if (m_pMesh->Get_AnimationFrame(70, 42) && !m_pMesh->Get_AnimationFrame(70, 45))
		{
			_float3 vPlayerpos = m_pTargetTransform->Get_TransformDesc().vPos;
			vPlayerpos.y += 2.f;
			dynamic_cast<CJLegionnaireGun*>(m_pWeapon)->Set_BulletFire(vPlayerpos);
			if (m_bBulletShoot)
			{
				switch (m_iSoundIndex)
				{
				case 0:
					m_eSound = CSound_Manager::ESoundID::Monster_Weapon3D_0;
					break;
				case 1:
					m_eSound = CSound_Manager::ESoundID::Monster_Weapon3D_1;
					break;
				case 2:
					m_eSound = CSound_Manager::ESoundID::Monster_Weapon3D_2;
					break;
				}

				GET_MANAGEMENT->StopSound(m_eSound);
				if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
				{
					if (m_bCamMode)
						GET_MANAGEMENT->PlaySounds(L"GunshotLegio.ogg", m_eSound, 1.f);
					else
						GET_MANAGEMENT->Play3DSounds(L"GunshotLegio.ogg", m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
				}
				m_bBulletShoot = false;
			}
		}
		
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Idle;
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Weapon3D_0);
			if (m_fTargetDist>6.f)
			{
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::GunIdle);
			}
		}
	}
}

void CJLegionnaire::Parry_Pattern(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() != (_uint)LegionnaireAniID::Parry)
	{
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Parry);
		m_fAddAniPlayTime = 0.3f;
		m_fDamageMultiple = 1.5f;
	}

	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Parry)
	{

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackCoolTime = true;
			m_fAddAniPlayTime = 1.3f;
			m_fDamageMultiple = 1.f;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
		}
	}
}

void CJLegionnaire::Hit_Pattern(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Hit)
	{
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle_Fight);
		}
	}
}

void CJLegionnaire::Death_Pattern(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() != (_uint)LegionnaireAniID::Dead)
	{
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Dead);

		_uint iCount = Mersen_ToInt(0, 5);

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

		TCHAR szFileName[256] = L"";
		wsprintf(szFileName, L"Legio_Death%d.ogg", iCount);
		GET_MANAGEMENT->StopSound(m_eSound);
		if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
		{
			if (m_bCamMode)
				GET_MANAGEMENT->PlaySounds(szFileName, m_eSound, 1.f);
			else
				GET_MANAGEMENT->Play3DSounds(szFileName, m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);
		}

	}
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Dead)
	{
		if (m_pMesh->Is_AnimationSetEnd())
			m_fAddAniPlayTime = 0.f;
		
		if (m_fDissolveTime > 7.f)
		{
			m_eCurTotalPattern = eMonsterPattern::Sleep;
			GET_MANAGEMENT->StopSound(m_eSound);
		}
	}
}

void CJLegionnaire::Friends_Patrol_Ready()
{
	for (auto& iter : m_vecFriendMonster)
	{
		iter->Set_PatrolReady();
	}
}

void CJLegionnaire::Patrol_Pattern(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() != (_uint)LegionnaireAniID::Idle && m_pMesh->Get_Animationset() != (_uint)LegionnaireAniID::Walk_F)
	{
		m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle);
		DistCheck();
		m_bRightLeg = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle)
	{
		if (m_bFriendPatrolReady)
		{
			if (m_bPatrolReady)
			{
				m_ePattern = PATTERN::Patrol;
				m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Walk_F);
				m_fSkillMoveSpeed = 3.f;
				m_bPatrolReady = false;
				m_bFriendPatrolReady = false;
			}
			else
			{
				if (::CoolTime(_fDeltaTime, m_fPatrolCoolTime, 1.f))
				{
					m_bPatrolReady = true;
					m_fPatrolCoolTime = 0.f;
				}
			}
			m_bRightLeg = true;
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Walk_F)
	{
		if (Patrol_Look(_fDeltaTime))
		{
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle);
			m_iNowPatrol++;
			if (m_iNowPatrol == m_iCapacitySize)
			{
				m_iNowPatrol = 0;
			}
			Friends_Patrol_Ready();
		}
		_uint iCount = Mersen_ToInt(9, 13);

		if (m_pMesh->Get_AnimationFrame(39, 39))
		{
			if (m_bLeftLeg && m_fTargetDist<55.f)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(39, 19))
		{
			if (m_bRightLeg && m_fTargetDist<55.f)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, 3.f, _fDeltaTime));
	}
}

void CJLegionnaire::Set_Hit()
{
	if (m_bCheckHit)
	{
		if (m_pMesh->Get_Animationset() == (_uint)LegionnaireAniID::Idle_Fight)
		{
			m_ePattern = PATTERN::Hit;
			m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Hit);
		}
	}		
}

void CJLegionnaire::PatrolMaker()
{
	auto pManagement = GET_MANAGEMENT;
	wstring PrototypeName = m_tTransformDesc.szFileTitle;
	_uint count = (_uint)PrototypeName.find(L"_", 0);
	wstring wstrinteger = PrototypeName;
	wstrinteger.erase(0, count + 1);
	_int iIndex = stoi(wstrinteger);

	_float3* pPos = nullptr;
	
	m_fFarRange = 150.f;

	switch (iIndex)
	{
	case 0:
		m_fFindDistance = 20.f;
		m_vecPatrolPos.reserve(4);
		m_vecFriendMonster.reserve(1);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 1.f, 14.891429f, 61.f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -26.f, 14.792518f, 44.f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 1.f, 14.891429f, 61.f };
		m_vecPatrolPos.emplace_back(pPos);
		Add_Friend(1, m_PrototypeName);
		break;
	case 1:
		m_fFindDistance = 20.f;
		m_vecPatrolPos.reserve(4);
		m_vecFriendMonster.reserve(1);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -1.f, 14.891429f, 55.f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -22.f, 14.792518f, 40.f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -1.f, 14.891429f, 55.f };
		m_vecPatrolPos.emplace_back(pPos);
		Add_Friend(0, m_PrototypeName);
		break;
	case 2:
		m_iDropGoldMin = 500;
		m_iDropGoldMax = 1000;
		m_fFindDistance = 27.f;
		m_iDiffuseIndex = 2;
		m_vecPatrolPos.reserve(5);
		m_vecFriendMonster.reserve(2);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 35.3321114f, 25.9700546f,186.632050f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 22.4976711f, 25.9700546f,138.661438f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -5.20220375f, 26.1503429f,145.316956f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -10.3496170f, 25.9700546f,208.719330f };
		m_vecPatrolPos.emplace_back(pPos);
		Add_Friend(3, m_PrototypeName);
		m_tState.fAtt = 60.f;
		m_tState.fDef = 60.f;
		m_tState.fMaxHP = 125.f;
		m_tState.fHP = m_tState.fMaxHP;
		break;
	case 3:
		m_fFindDistance = 20.f;
		m_vecPatrolPos.reserve(5);
		m_vecFriendMonster.reserve(2);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 37.9919510f, 25.9700565f, 194.572250f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 26.2693214f,25.9700546f, 131.812378f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -7.54731131f, 26.1503429f, 141.f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -12.9358616f,25.9700546f,201.524445f };
		m_vecPatrolPos.emplace_back(pPos);
		Add_Friend(4, m_PrototypeName);
		break;
	case 4:
		m_fFindDistance = 20.f;
		m_vecPatrolPos.reserve(5);
		m_vecFriendMonster.reserve(2);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 33.0615845f, 25.9700546f, 196.826584f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 28.7085857f,25.9700546f, 144.119522f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -3.07837081f, 26.1503429f, 141.f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ -6.68400145f,25.9700546f, 202.064514f };
		m_vecPatrolPos.emplace_back(pPos);
		Add_Friend(2, m_PrototypeName);
		break;
	case 5:
		m_fFindDistance = 20.f;
		m_vecPatrolPos.reserve(2);
		m_vecFriendMonster.reserve(1);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 41.1267929f, 30.681604f, 251.165817f };
		m_vecPatrolPos.emplace_back(pPos);
		Add_Friend(6, m_PrototypeName);
		break;
	case 6:
		m_fFindDistance = 20.f;
		m_vecPatrolPos.reserve(2);
		m_vecFriendMonster.reserve(1);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 37.3008537f, 30.681604f, 247.180847f };
		m_vecPatrolPos.emplace_back(pPos);
		Add_Friend(5, m_PrototypeName);
		break;
	case 7:
		m_fFindDistance = 20.f;
		m_vecPatrolPos.reserve(6);
		m_vecFriendMonster.reserve(1);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 144.947510f,37.064991f,298.515259f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 158.223770f, 37.064991f, 344.649292f };
		//pPos = new _float3{ 155.608063f	, 37.064991f, 337.578339f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 152.025497f, 37.0649948f, 365.193298f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 152.766174f, 37.0649910f, 332.f };
		//pPos = new _float3{ 152.846710f, 37.064991f, 329.844147f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 144.083237f, 37.064991f, 295.171448f };
		m_vecPatrolPos.emplace_back(pPos);
		Add_Friend(8, m_PrototypeName);
		break;
	case 8:
		m_fFindDistance = 20.f;
		m_vecPatrolPos.reserve(6);
		m_vecFriendMonster.reserve(1);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 141.773575f, 37.064991f,301.508728f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 157.857361f, 37.064991f, 338.876007f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 140.561798f, 37.0649948f, 348.684875f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 157.613251f, 37.064991f, 332.f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 143.452072f, 37.064991f,298.366241f };
		m_vecPatrolPos.emplace_back(pPos);
		Add_Friend(2, L"Yantari");
		break;
	default:
		break;
	}

	m_iCapacitySize = (_uint)m_vecPatrolPos.size();

}

void CJLegionnaire::DistCheck()
{
	m_bPatrolReady = true;
	m_bFriendPatrolReady = true;
	_uint iClosePatrolIndex = 0;
	m_bTargeting = false;
	m_bTargetLook = false;
	_float fDist = D3DXVec3Length(&(*m_vecPatrolPos[0] - m_pTransform->Get_TransformDesc().vPos));
	for (auto& rPos : m_vecPatrolPos)
	{
		_float fNextDist = D3DXVec3Length(&(*rPos - m_pTransform->Get_TransformDesc().vPos));
		if (fDist > fNextDist)
		{
			fDist = fNextDist;
			m_iNowPatrol = iClosePatrolIndex;
		}
		iClosePatrolIndex++;
	}
}

void CJLegionnaire::AttackCoolTimer(const _float & _fDeltaTime)
{
	if (m_bAttackPattern2)
	{
		if (::CoolTime(_fDeltaTime, m_fAttackPattern2, 3.f))
		{
			m_bAttackPattern2 = false;
			m_fAttackPattern2 = 0.f;
		}
	}
	if (m_bAttackPattern3)
	{
		if (::CoolTime(_fDeltaTime, m_fAttackPattern3, 5.f))
		{
			m_bAttackPattern3 = false;
			m_fAttackPattern3 = 0.f;
		}
	}
	if (m_bAttackPattern4)
	{
		if (::CoolTime(_fDeltaTime, m_fAttackPattern4, 5.f))
		{
			m_bAttackPattern4 = false;
			m_fAttackPattern4 = 0.f;
		}
	}

	if (m_bAttackPattern5)
	{
		if (::CoolTime(_fDeltaTime, m_fAttackPattern5, 7.f))
		{
			m_bAttackPattern5 = false;
			m_fAttackPattern5 = 0.f;
		}
	}
}

void CJLegionnaire::Move_Sound()
{
	_uint iCount = Mersen_ToInt(9, 13);

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

	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Legio_Walk%d.ogg", iCount);

	GET_MANAGEMENT->StopSound(m_eSound);

	if (!(Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICSTART) || (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_CINEMATICEND))
	{
		if (m_bCamMode)
			GET_MANAGEMENT->PlaySounds(szFileName, m_eSound, 1.f);
		else
			GET_MANAGEMENT->Play3DSounds(szFileName, m_eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);
	}

}



CJLegionnaire * CJLegionnaire::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJLegionnaire* pInstance = new CJLegionnaire(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJLegionnaire::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CJLegionnaire* pClone = new CJLegionnaire(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJLegionnaire::Free()
{
	CJBaseMonster::Free();
}
