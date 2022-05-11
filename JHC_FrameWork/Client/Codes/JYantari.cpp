#include "stdafx.h"
#include "JYantari.h"
#include "UI_NormalMonster.h"
#include "Player.h"
#include "Effect_Texture_Ani.h"
#include "Effect_Texture.h"

#define MONSTER_CHECK_ACTIVATION (m_ePreTotalPattern == eMonsterPattern::Awake ||		\
m_ePreTotalPattern == eMonsterPattern::Walk ||											\
m_ePreTotalPattern == eMonsterPattern::Idle ||											\
m_ePreTotalPattern == eMonsterPattern::Hit ||											\
m_ePreTotalPattern == eMonsterPattern::Attack||											\
m_ePreTotalPattern == eMonsterPattern::Dead )

CJYantari::CJYantari(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CJBaseMonster(_pDevice, _PrototypeName)
	, m_ePattern(PATTERN::End)
	, m_bWalkPattern(false)
	, m_bCastSkill(false)
	, m_iDownAttack(0)
	, m_iAttackCount(0)
	, m_fCastSkillCoolTime(0.f)
	, m_vSkillPos(0.f,0.f,0.f)
	, m_fSkillEndY(0.f)
	, m_bPower(false)
	, m_pSkillEffect(nullptr)
	, m_bNearPattern2(false)
	, m_bNearPattern3(false)
	, m_bNearPattern4(false)
	, m_bNearPattern5(false)
	, m_bCastPattern(false)
	, m_bFarPattern1(false)
	, m_bFarPattern2(false)
	, m_fNearPattern2Time(0.f)
	, m_fNearPattern3Time(0.f)
	, m_fNearPattern4Time(0.f)
	, m_fNearPattern5Time(0.f)
	, m_fCastPatternTime(0.f)
	, m_fFarPattern1Time(0.f)
	, m_fFarPattern2Time(0.f)
	, m_bCreateEffect(false)
	, m_pDistortTex(nullptr)
	, m_bAttackSound(false)
	, m_fCastPower(10.f)
	, m_bDashSound(false)
{
}

CJYantari::CJYantari(const CJYantari & _rOther)
	: CJBaseMonster(_rOther)
	, m_ePattern(PATTERN::End)
	, m_bWalkPattern(false)
	, m_bCastSkill(false)
	, m_iDownAttack(0)
	, m_iAttackCount(0)
	, m_fCastSkillCoolTime(0.f)
	, m_vSkillPos(0.f, 0.f, 0.f)
	, m_fSkillEndY(0.f)
	, m_bPower(false)
	, m_pSkillEffect(nullptr)
	, m_bNearPattern2(false)
	, m_bNearPattern3(false)
	, m_bNearPattern4(false)
	, m_bNearPattern5(false)
	, m_bCastPattern(false)
	, m_bFarPattern1(false)
	, m_bFarPattern2(false)
	, m_fNearPattern2Time(0.f)
	, m_fNearPattern3Time(0.f)
	, m_fNearPattern4Time(0.f)
	, m_fNearPattern5Time(0.f)
	, m_fCastPatternTime(0.f)
	, m_fFarPattern1Time(0.f)
	, m_fFarPattern2Time(0.f)
	, m_bCreateEffect(false)
	, m_pDistortTex(nullptr)
	, m_bAttackSound(false)
	, m_fCastPower(10.f)
	, m_bDashSound(false)
{
}

HRESULT CJYantari::Ready_GameObject_Prototype()
{
	if (FAILED(CJBaseMonster::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJYantari::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseMonster::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(CJYantari::Weapon_Clone()))
	{
		ERROR_BOX(L"Failed to Weapon_Clone");
		return E_FAIL;
	}
	if (FAILED(CJYantari::UI_Clone()))
	{
		ERROR_BOX(L"Failed to UI_Clone");
		return E_FAIL;
	}

	if (FAILED(CJYantari::Cast_SkillEffectCreate()))
	{
		ERROR_BOX(L"Failed to UI_Clone");
		return E_FAIL;
	}

	m_iDropGoldMax = 500;
	m_iDropGoldMin = 250;
	m_iGold = Mersen_ToInt(m_iDropGoldMin, m_iDropGoldMax);

	m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
	m_iShaderPass = 2;

	m_tState.fAtt = 40.f;
	m_tState.fDef = 20.f;
	m_tState.fMaxHP = 300.f;
	m_tState.fHP = m_tState.fMaxHP;

	m_iDropGoldMin = 100;
	m_iDropGoldMax = 500;

	m_fAddAniPlayTime = 1.3f;
	m_fFindDistance = 35.f;

	m_fActivateTimer = 4.f;
	*const_cast<_float*>(&m_fSleepTime_Const) = -1.f;
	PatrolMaker();
	m_iItemCreateYAxisPosFix = 1.f;

	m_wstrCastSkill = m_tTransformDesc.szFileTitle;
	m_wstrCastSkill += L"_DownSkill";

	return S_OK;
}

_uint CJYantari::Update_GameObject(const _float & _fDeltaTime)
{
	//if (m_pWeapon)
	//	m_pWeapon->Trail_Switch(true);

	if (!m_vecPatrolPos.empty() && m_vecFriendMonster.empty())
	{
		Add_Friend(7, L"Legionnaire");
	}

	if (MONSTER_CHECK_ACTIVATION)
	{
		CJYantari::AttackCoolTimer(_fDeltaTime);
		Cast_SkillUpdate(_fDeltaTime);
		Cast_SkillCoolTime(_fDeltaTime);
		CJYantari::TrailFixWidth();
	}

	CJBaseMonster::Update_GameObject(_fDeltaTime);


	return m_iEvent;
}

HRESULT CJYantari::Render_GameObject()
{
	CJBaseMonster::Render_GameObject();

	const MY_FRAME* pMonsterFrame = m_pMesh->Get_FrameByName("Root_Axe_R");

	m_matChildWeapon = pMonsterFrame->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();

	return S_OK;
}

void CJYantari::Do_Idle(const _float & _fDeltaTime)
{
	_float fYDist = fabs(m_pTargetTransform->Get_TransformDesc().vPos.y - m_pTransform->Get_TransformDesc().vPos.y);

	m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));


	if (m_fTargetDist < m_fFindDistance && fYDist < 6.f)
	{
		m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
		m_bTargeting = true;
		m_bTargetLook = true;
		m_eCurTotalPattern = eMonsterPattern::Walk;
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

void CJYantari::Do_Attack(const _float & _fDeltaTime)
{
	if (m_ePattern != PATTERN::Pattern_Ready)
	{
		if (m_pPlayer->Get_WeaponParry())
		{
			m_ePattern = PATTERN::Parry;
			m_pPlayer->Set_WeaponParry(false);
			m_pWeapon->Set_Acttack(false);
		}

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
		case PATTERN::Near_5:
			Near_Pattern_5(_fDeltaTime);
			break;
		case PATTERN::Far_1:
			Far_Pattern_1(_fDeltaTime);
			break;
		case PATTERN::Far_2:
			Far_Pattern_2(_fDeltaTime);
			break;
		case PATTERN::Far_3:
			Cast_Pattern(_fDeltaTime);
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
			if (m_fTargetDist <= 4.5f)
			{
				switch (Mersen_ToInt(0, 4))
				{
				case 0:
					Near_Pattern_1(_fDeltaTime);
					break;
				case 1:
					if (!m_bNearPattern2)
					{
						Near_Pattern_2(_fDeltaTime);
						break;
					}
				case 2:
					if (!m_bNearPattern3)
					{
						Near_Pattern_3(_fDeltaTime);
						break;
					}
				case 3:
					if (!m_bNearPattern4)
					{
						Near_Pattern_4(_fDeltaTime);
						break;
					}
				case 4:
					if (!m_bNearPattern5)
						Near_Pattern_5(_fDeltaTime);
					else
						Near_Pattern_1(_fDeltaTime);
					break;
				}
				m_iAttackCount++;

			}
			else if (m_fTargetDist > 4.5f)
			{
				_uint iFarPattern = 0;

				if (!m_bWalkPattern)
					iFarPattern = Mersen_ToInt(0, 1);
				else
					iFarPattern = Mersen_ToInt(0, 2);

				switch (iFarPattern)
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
					if (!m_bCastPattern)
					{
						Cast_Pattern(_fDeltaTime);
						break;
					}
				default:
					m_eCurTotalPattern = eMonsterPattern::Walk;
					m_ePattern = PATTERN::Walk_F;
					break;
				}
			}
		}
	}
}

void CJYantari::Do_Hit(const _float & _fDeltaTime)
{
}

void CJYantari::Do_Walk(const _float & _fDeltaTime)
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
		if (m_fTargetDist <= 4.5f)
		{
			if (m_tState.fHP < m_tState.fMaxHP * 0.5f || m_iAttackCount>2)
			{
				iMovePattern = 1;
				m_iAttackCount = 0;
			}
			else
			{
				iMovePattern = Mersen_ToInt(2,4);
			}
		}
		else if (m_fTargetDist <= 8.f && m_fTargetDist > 4.5f)
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

void CJYantari::Do_Other(const _float & _fDeltaTime)
{
	if (m_ePreTotalPattern == eMonsterPattern::Sleep)
	{
		if (::CoolTime(_fDeltaTime, m_fSleepCountTime, m_fSleepTime_Const))
		{
			m_fDissolveTime = 0.f;
			m_eCurTotalPattern = eMonsterPattern::Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
			m_fSleepCountTime = 0.f;
		}
	}
	if (m_ePreTotalPattern == eMonsterPattern::Dead)
	{
		m_fDissolveTime += _fDeltaTime * powf(1.2f, m_fDissolveTime);
		Death_Pattern(_fDeltaTime);
	}
}

void CJYantari::Get_Damaged()
{
	_uint iCount = Mersen_ToInt(0, 2);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"YantariDamage%d.wav", iCount);
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
	GET_MANAGEMENT->Play3DSounds(szFileName, eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f,false,1.f);
}

HRESULT CJYantari::Weapon_Clone()
{
	auto pManagement = GET_MANAGEMENT;
	wstring wstrMonsterName = m_tTransformDesc.szFileTitle;

	_uint count = (_uint)wstrMonsterName.find(L"_", 0);
	wstring wstrinteger = wstrMonsterName;
	wstrinteger.erase(0, count + 1);
	wstrinteger += L"_Weapon";
	TRANSFORM_DESC tTransformDesc;
	tTransformDesc.vRotate = _float3(D3DXToRadian(90.f), D3DXToRadian(90.f), D3DXToRadian(90.f));
	lstrcpy(tTransformDesc.szFileTitle, m_tTransformDesc.szFileTitle);
	lstrcat(tTransformDesc.szFileTitle, L"_YantariAxe_");
	lstrcat(tTransformDesc.szFileTitle, wstrinteger.c_str());

	if (FAILED(pManagement->Clone_GameObject((_uint)ESceneType::Static,
		L"GameObject_YantariAxe",
		(CGameObject**)&m_pWeapon,
		&tTransformDesc,
		m_iSceneIdx)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pWeapon, m_iSceneIdx, L"Layer_Weapon")))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}
	m_pWeapon->Set_ParentClass(this, CJBaseWeapon::eMonsterParent::Normal);
	Safe_AddRef(m_pWeapon);

	return S_OK;

}

HRESULT CJYantari::UI_Clone()
{
	CManagement* pManagement = GET_MANAGEMENT;

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

HRESULT CJYantari::SetUp_ConstantTable(_EFFECT & _pEffect)
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

void CJYantari::Near_Pattern_1(const _float & _fDeltaTime)
{
	// 베기
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Near_1;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Attack2);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bAttackSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack2)
	{
		m_bTargetLook = false;
		if (m_pMesh->Get_AnimationFrame(62, 17) && !m_pMesh->Get_AnimationFrame(62, 26))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Idle;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
			m_bAttackSound = true;
		}
	}
}

void CJYantari::Near_Pattern_2(const _float & _fDeltaTime)
{
	// 내려찍었다가 올리기. 올릴때도 피격판정
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Near_2;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Attack3);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_fAddAniPlayTime = 1.3f;
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack3)
	{
		m_bTargetLook = false;
		if (!m_pMesh->Get_AnimationFrame(82, 22))
		{
			m_fAddAniPlayTime = 0.5f;
			m_tState.fAtt = m_tState.fAtt * 2.f;
			m_pWeapon->Set_AtkDamage(m_tState.fAtt, m_tState.fDef);
			//Make_ChargeParticle();
		}
		else
			m_fAddAniPlayTime = 1.3f;
		if (m_pMesh->Get_AnimationFrame(82, 22) && !m_pMesh->Get_AnimationFrame(82, 30))
		{
			if (m_bAttackSound)
			{
				CSound_Manager::ESoundID eSound;
				switch (m_iSoundIndex)
				{
				case 0:
					eSound = CSound_Manager::ESoundID::Monster_Weapon3D_0;
					break;
				case 1:
					eSound = CSound_Manager::ESoundID::Monster_Weapon3D_1;
					break;
				case 2:
					eSound = CSound_Manager::ESoundID::Monster_Weapon3D_2;
					break;
				}

				GET_MANAGEMENT->StopSound(eSound);

				GET_MANAGEMENT->Play3DSounds(L"YantariAtk3.ogg", eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f,false,1.f);
				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else if (m_pMesh->Get_AnimationFrame(82, 39) && !m_pMesh->Get_AnimationFrame(82, 47))
		{
			m_pWeapon->Set_Acttack(true);
		}
		else
		{
			m_pWeapon->Set_Acttack(false);
			m_bAttackSound = true;
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bNearPattern2 = true;
			m_tState.fAtt = m_tState.fAtt * 0.25f;
			m_pWeapon->Set_AtkDamage(m_tState.fAtt, m_tState.fDef);
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Idle;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
			m_bAttackSound = true;
		}
	}
}

void CJYantari::Near_Pattern_3(const _float & _fDeltaTime)
{
	//2단 내려찍기 후 돌진
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Near_3;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Attack3);
		m_fSkillMoveSpeed = 30.f;
		m_fAddAniPlayTime = 1.5f;
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack3)
	{
		if (!m_pMesh->Get_AnimationFrame(82, 22))
			m_bTargetLook = true;
		else
			m_bTargetLook = false;
		
		if (m_pMesh->Get_AnimationFrame(82, 22) && !m_pMesh->Get_AnimationFrame(82, 30))
		{
			if (m_bAttackSound)
			{
				CSound_Manager::ESoundID eSound;
				switch (m_iSoundIndex)
				{
				case 0:
					eSound = CSound_Manager::ESoundID::Monster_Weapon3D_0;
					break;
				case 1:
					eSound = CSound_Manager::ESoundID::Monster_Weapon3D_1;
					break;
				case 2:
					eSound = CSound_Manager::ESoundID::Monster_Weapon3D_2;
					break;
				}

				GET_MANAGEMENT->StopSound(eSound);
				GET_MANAGEMENT->Play3DSounds(L"YantariAtk3.ogg", eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f,false, 1.f);
				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else if (m_pMesh->Get_AnimationFrame(82, 39) && !m_pMesh->Get_AnimationFrame(82, 47))
		{
			m_pWeapon->Set_Acttack(true);
		}
		else
		{
			m_bAttackSound = true;
			m_pWeapon->Set_Acttack(false);

		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_pWeapon->Set_Acttack(true);
			if (m_fTargetDist <= 4.5f)
			{
				if (m_iDownAttack == 1)
				{
					m_fAddAniPlayTime = 1.f;
					m_eCurTotalPattern = eMonsterPattern::Attack;
					m_bTargetLook = true;
					m_pMesh->Set_Animationset((_uint)YantariAniID::Attack4);
					m_iDownAttack = 0;
				}
				else
				{
					m_fAddAniPlayTime = 1.5f;
					m_eCurTotalPattern = eMonsterPattern::Attack;
					m_bTargetLook = true;
					m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
					m_iDownAttack++;
				}
			}
			else
			{
				m_bAttackSound = true;
				m_bNearPattern3 = true;
				m_bAttackCoolTime = true;
				m_eCurTotalPattern = eMonsterPattern::Idle;
				m_bTargetLook = true;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
			}
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack4)
	{
		m_bTargetLook = false;

		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(62, 25) && !m_pMesh->Get_AnimationFrame(62, 35))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(62, 20) && !m_pMesh->Get_AnimationFrame(62, 28))
		{
			m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_bNearPattern3 = true;
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Idle;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
		}
	}
}

void CJYantari::Near_Pattern_4(const _float & _fDeltaTime)
{
	//찌르기 후 베기
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Near_4;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Attack4);
		m_fSkillMoveSpeed = 30.f;
		m_fAddAniPlayTime = 1.7f;
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack4)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(62, 25) && !m_pMesh->Get_AnimationFrame(62, 35))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_fAddAniPlayTime = 1.3f;
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Attack;
			m_bTargetLook = true;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Attack2);
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack2)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(62, 17) && !m_pMesh->Get_AnimationFrame(62, 26))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);

				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_bNearPattern4 = true;
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Idle;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
		}
	}
}

void CJYantari::Near_Pattern_5(const _float & _fDeltaTime)
{
	// 오른쪽 왼쪽 베기
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Near_5;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Attack1);
		m_fSkillMoveSpeed = 30.f;
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack1)
	{
		m_bTargetLook = false;

		if (!m_pMesh->Get_AnimationFrame(62, 31))
			m_fAddAniPlayTime = 2.5f;
		else
			m_fAddAniPlayTime = 1.3f;
		
		if (m_pMesh->Get_AnimationFrame(62, 31) && !m_pMesh->Get_AnimationFrame(62, 45))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_fAddAniPlayTime = 1.3f;
			m_eCurTotalPattern = eMonsterPattern::Attack;
			m_bTargetLook = true;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Attack2);
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack2)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(62, 17) && !m_pMesh->Get_AnimationFrame(62, 26))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);

				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_bNearPattern5 = true;
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Idle;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
		}
	}
}

void CJYantari::Far_Pattern_1(const _float & _fDeltaTime)
{
	// 돌진 베기
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Far_1;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Attack1);
		m_fSkillMoveSpeed = 30.f;
		m_fAddAniPlayTime = 1.f;
		m_bDashSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack1)
	{
		m_bTargetLook = false;
		if (m_bDashSound && m_pMesh->Get_AnimationFrame(62, 21))
		{
			DashSound();
			m_bDashSound = false;
		}
		if (m_pMesh->Get_AnimationFrame(62, 31) && !m_pMesh->Get_AnimationFrame(62, 45))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(62,20) && !m_pMesh->Get_AnimationFrame(62, 31))
		{
			m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		}
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_bFarPattern1 = true;
			m_fAddAniPlayTime = 1.3f;
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Idle;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
		}
	}
}

void CJYantari::Far_Pattern_2(const _float & _fDeltaTime)
{
	//짧게 뛰고 찌르기
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Far_2;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Attack4);
		m_fSkillMoveSpeed = 30.f;
		m_fAddAniPlayTime = 1.f;
		m_bDashSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack4)
	{
		m_bTargetLook = false;

		if (m_bDashSound && m_pMesh->Get_AnimationFrame(62, 21))
		{
			DashSound();
			m_bDashSound = false;
		}
		
		if (m_pMesh->Get_AnimationFrame(62, 25) && !m_pMesh->Get_AnimationFrame(62, 35))
		{
			if (m_bAttackSound)
			{
				m_pWeapon->WeaponSound(m_iSoundIndex);
				m_bAttackSound = false;
			}
			m_pWeapon->Set_Acttack(true);
		}
		else
			m_pWeapon->Set_Acttack(false);

		if (m_pMesh->Get_AnimationFrame(62, 20) && !m_pMesh->Get_AnimationFrame(62, 28))
		{
			m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		}
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_bFarPattern2 = true;
			m_fAddAniPlayTime = 1.3f;
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Idle;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
		}
	}
}

void CJYantari::Cast_Pattern(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Far_3;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Cast);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bCastSkill = false;
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Cast)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(88, 48))
		{
			Cast_SkillCreate();
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackSound = true;
			m_bCastPattern = true;
			m_bAttackCoolTime = true;
			m_eCurTotalPattern = eMonsterPattern::Idle;
			m_bTargetLook = true;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
		}
	}
}

void CJYantari::Walk_Pattern_F(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_F;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Walk_Front);
		m_fSkillMoveSpeed = 6.5f;
		m_bWalkPattern = !m_bWalkPattern;
		m_bRightLeg = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Walk_Front)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		m_fWalkMoveTime += _fDeltaTime;

		if (m_pMesh->Get_AnimationFrame(41, 39))
		{
			if (m_bLeftLeg)
			{
				WalkSound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(41, 19))
		{
			if (m_bRightLeg)
			{
				WalkSound();
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_bFarPattern1 && m_bFarPattern2 && m_bCastPattern)
		{
			if (m_fTargetDist < 4.5f)
			{
				m_fWalkMoveTime = 0.f;
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
				m_eCurTotalPattern = eMonsterPattern::Attack;
			}
		}
		else
		{
			if (m_bWalkPattern)
			{
				if (m_fTargetDist < 10.f)
				{
					m_fWalkMoveTime = 0.f;
					m_ePattern = PATTERN::Pattern_Ready;
					m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
					m_eCurTotalPattern = eMonsterPattern::Attack;
				}
			}
			else
			{
				if (m_fWalkMoveTime > 10.f || m_fTargetDist < 4.5f)
				{
					m_fWalkMoveTime = 0.f;
					m_ePattern = PATTERN::Pattern_Ready;
					m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
					m_eCurTotalPattern = eMonsterPattern::Attack;
				}
			}
		}
	}
}

void CJYantari::Walk_Pattern_B(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_B;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Walk_Back);
		m_fSkillMoveSpeed = 5.f;
		m_bRightLeg = true;
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Walk_Back)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		m_fWalkMoveTime += _fDeltaTime;
		
		if (m_pMesh->Get_AnimationFrame(41, 39))
		{
			if (m_bLeftLeg)
			{
				WalkSound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(41, 15))
		{
			if (m_bRightLeg)
			{
				WalkSound();
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_fWalkMoveTime >= 1.5f)
		{
			m_fWalkMoveTime = 0.f;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
			m_eCurTotalPattern = eMonsterPattern::Attack;
			m_bWalkPattern = true;
		}
	}
}
void CJYantari::Walk_Pattern_L(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_L;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Walk_Left);
		m_fSkillMoveSpeed = 3.f;
		m_bRightLeg = true;
	}

	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Walk_Left)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vRight, m_fSkillMoveSpeed, _fDeltaTime));
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vLook, 1.f, _fDeltaTime));
		m_fWalkMoveTime += _fDeltaTime;

		if (m_pMesh->Get_AnimationFrame(41, 39))
		{
			if (m_bLeftLeg)
			{
				WalkSound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(41, 21))
		{
			if (m_bRightLeg)
			{
				WalkSound();
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_fWalkMoveTime >= 1.f)
		{
			m_fWalkMoveTime = 0.f;
			m_fSkillMoveSpeed = 6.5f;
			m_ePattern = PATTERN::Walk_F;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Walk_Front);
			m_eCurTotalPattern = eMonsterPattern::Walk;
		}
	}
}

void CJYantari::Walk_Pattern_R(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_R;
		m_pMesh->Set_Animationset((_uint)YantariAniID::Walk_Right);
		m_fSkillMoveSpeed = 3.f;
		m_bRightLeg = true;
	}

	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Walk_Right)
	{
		m_bTargetLook = true;
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vRight, m_fSkillMoveSpeed, _fDeltaTime));
		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, -m_vLook, 1.f, _fDeltaTime));
		m_fWalkMoveTime += _fDeltaTime;

		if (m_pMesh->Get_AnimationFrame(41, 35))
		{
			if (m_bLeftLeg)
			{
				WalkSound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(41, 15))
		{
			if (m_bRightLeg)
			{
				WalkSound();
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_fWalkMoveTime >= 1.f)
		{
			m_fSkillMoveSpeed = 6.5f;
			m_fWalkMoveTime = 0.f;
			m_ePattern = PATTERN::Walk_F;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Walk_Front);
			m_eCurTotalPattern = eMonsterPattern::Walk;
		}
	}
}

void CJYantari::Hit_Pattern(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Hit)
	{
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
		}
	}
}

void CJYantari::Death_Pattern(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() != (_uint)YantariAniID::Death)
	{
		m_pMesh->Set_Animationset((_uint)YantariAniID::Death);
		CSound_Manager::ESoundID eSound;
		switch (m_iSoundIndex)
		{
		case 0:
			eSound = CSound_Manager::ESoundID::Monster_Death3D_0;
			break;
		case 1:
			eSound = CSound_Manager::ESoundID::Monster_Death3D_1;
			break;
		case 2:
			eSound = CSound_Manager::ESoundID::Monster_Death3D_2;
			break;
		}

		GET_MANAGEMENT->StopSound(eSound);
		GET_MANAGEMENT->Play3DSounds(L"YantariDead.wav", eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 0.7f);
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Death)
	{
		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_fAddAniPlayTime = 0.f;
		}
		if (m_fDissolveTime > 7.f)
		{
			m_eCurTotalPattern = eMonsterPattern::Sleep;
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Death3D_0);
		}
	}
}

void CJYantari::Patrol_Pattern(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
	{
		if (m_bFriendPatrolReady)
		{
			if (m_bPatrolReady)
			{
				m_ePattern = PATTERN::Patrol;
				m_pMesh->Set_Animationset((_uint)YantariAniID::Walk_Front);
				m_fSkillMoveSpeed = 3.2f;
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
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Walk_Front)
	{
		if (Patrol_Look(_fDeltaTime))
		{
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
			m_iNowPatrol++;
			if (m_iNowPatrol == m_iCapacitySize)
			{
				m_iNowPatrol = 0;
			}
			for (auto& iter : m_vecFriendMonster)
			{
				iter->Set_PatrolReady();
			}
		}
		
		if (m_pMesh->Get_AnimationFrame(41, 39))
		{
			if (m_bLeftLeg&& m_fTargetDist<55.f)
			{
				WalkSound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(41, 19))
		{
			if (m_bRightLeg&& m_fTargetDist<55.f)
			{
				WalkSound();
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		m_pTransform->Set_Pos(pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
	}
}

void CJYantari::Parry_Pattern(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() != (_uint)YantariAniID::Parry)
	{
		m_pMesh->Set_Animationset((_uint)YantariAniID::Parry);
		m_fAddAniPlayTime = 0.3f;
		m_fDamageMultiple = 1.5f;
	}

	if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Parry)
	{

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bAttackCoolTime = true;
			m_fAddAniPlayTime = 1.3f;
			m_fDamageMultiple = 1.f;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Idle);
		}
	}
}

void CJYantari::Set_Hit()
{
	if (m_bCheckHit)
	{
		if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Idle)
		{
			m_ePattern = PATTERN::Hit;
			m_pMesh->Set_Animationset((_uint)YantariAniID::Hit);
		}
	}
}

void CJYantari::PatrolMaker()
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
		m_iSoundIndex = 2;
		m_fCastPower = 5.f;
		break;
	case 1:
		m_tState.fAtt = 70.f;
		m_tState.fDef = 50.f;
		m_tState.fMaxHP = 300.f;
		m_tState.fHP = m_tState.fMaxHP;
		m_iDropGoldMax = 1000;
		m_iDropGoldMin = 500;
		m_iGold = Mersen_ToInt(m_iDropGoldMin, m_iDropGoldMax);
		m_iDiffuseIndex = 1;
		m_fCastPower = 10.f;
		m_iSoundIndex = 1;
		break;
	case 2:
		m_iSoundIndex = 0;
		m_iDiffuseIndex = 1;
		m_tState.fAtt = 70.f;
		m_tState.fDef = 50.f;
		m_tState.fMaxHP = 450.f;
		m_tState.fHP = m_tState.fMaxHP;
		m_iDropGoldMax = 1000;
		m_iDropGoldMin = 500;
		m_fCastPower = 10.f;
		m_iGold = Mersen_ToInt(m_iDropGoldMin, m_iDropGoldMax);
		m_pWeapon->Set_AtkDamage(m_tState.fAtt, m_tState.fDef);
		m_vecPatrolPos.reserve(4);
		m_vecFriendMonster.reserve(1);
		pPos = new _float3{ m_tTransformDesc.vPos.x,m_tTransformDesc.vPos.y,m_tTransformDesc.vPos.z };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 146.293610f, 37.0649910f,306.275848f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 154.856430f,37.064991f,343.194763f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 139.972778f,37.064991f,360.953003f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 153.436157f,37.064991f,325.257904f };
		m_vecPatrolPos.emplace_back(pPos);
		pPos = new _float3{ 140.665649f,37.064991f,296.990723f };
		m_vecPatrolPos.emplace_back(pPos);
		break;
	default:
		break;

	}
	m_iCapacitySize = (_uint)m_vecPatrolPos.size();

}

void CJYantari::Make_ChargeParticle()
{
	Get_Manager()->Generate_ItemParticle(
		m_iSceneIdx, m_pTransform->Get_TransformDesc().vPos, 6.f, 10, _float4(1.f, 1.f, 1.f, 0.2f), ePartilcePatternType::OnStartBlink, Mersen_ToInt(3, 5)
	);
	_float3 vCreatePos = m_pTransform->Get_TransformDesc().vPos;
	_float3 vTargetPos = m_pTargetTransform->Get_TransformDesc().vPos;

	vCreatePos.y -= 1.f;

	_float4x4* pMeshBoneMatrix = nullptr;
	_ulong	ulongCount = 0;
	_float4x4 matWorld;

	m_pMesh->Get_AllBoneMetrix(&pMeshBoneMatrix, &ulongCount);
	_ulong i = Mersen_ToInt(0, ulongCount);

	matWorld = *(m_pTransform->Get_WorldMatrix());
	memcpy(vCreatePos, &(pMeshBoneMatrix[i] * matWorld).m[3][0], sizeof(_float3));

	_uint iDrawIdx = Mersen_ToInt(3, 5);
	CParticle_Manager::Get_Instance()->Generate_ExplosionParticle(
		m_iSceneIdx, vCreatePos, 0.f, &vTargetPos,
		1, m_vDissolveColor,
		ePartilcePatternType::OnStartBlink, iDrawIdx, 2.2f, D3DX_PI, 1.f);

}

void CJYantari::Cast_SkillCreate()
{
	if (!m_bCastSkill)
	{
		_float3 vPos = m_pTargetTransform->Get_TransformDesc().vPos;
		vPos.y += 10.f;
		PxTransform transform = PxTransform(vPos.x, vPos.y, vPos.z);
		m_fSkillEndY = vPos.y - 10.f;

		_float fDamage = m_tState.fAtt* 0.5f + m_tState.fDef * 0.25f + m_fCastPower;

		Get_Manager()->Add_DynamicActor(m_wstrCastSkill, transform, fDamage, EInteractionType::Sphere, PxSphereGeometry(1.f));
		m_pSkillEffect->Set_RenderSwitch();
		m_pDistortTex->Set_RenderSwitch(true);
		
		CSound_Manager::ESoundID eSound;
		switch (m_iSoundIndex)
		{
		case 0:
			eSound = CSound_Manager::ESoundID::Monster_Weapon3D_0;
			break;
		case 1:
			eSound = CSound_Manager::ESoundID::Monster_Weapon3D_1;
			break;
		case 2:
			eSound = CSound_Manager::ESoundID::Monster_Weapon3D_2;
			break;
		}

		GET_MANAGEMENT->StopSound(eSound);
		GET_MANAGEMENT->Play3DSounds(L"Yantari_HitGround.ogg", eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f,false,1.f);
		
		_uint iCount = Mersen_ToInt(0, 3);
		TCHAR szFileName[256] = L"";
		wsprintf(szFileName, L"Yantari_Lightning%d.ogg", iCount);
		
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
		GET_MANAGEMENT->Play3DSounds(szFileName, eSound, vPos, 0.f, 50.f,false,1.f);
		
		m_bCastSkill = true;
		m_bCreateEffect = true;
	}
}

void CJYantari::Cast_SkillDelete()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	pPlayer->Set_HitSkill();

	Get_Manager()->Delete_Trigger(m_wstrCastSkill);
	m_pSkillEffect->Set_RenderSwitch(false);
	m_pDistortTex->Set_RenderSwitch(false);
	//m_pSkillEffect->Set_Color()
}

void CJYantari::Cast_SkillUpdate(const _float & _fDeltaTime)
{
	if (m_bCastSkill)
	{
		//_float3 vPower = _float3(0.f, 0.f, 0.f);
		//if (!m_bCreateEffect)
		//{
		//	vPower = _float3(0.f, -5.f, 0.f);
		//	Get_Manager()->SetDynamic_Actor(m_wstrCastSkill);
		//	m_bCreateEffect = true;
		//}
		Get_Manager()->SetDynamic_Actor(m_wstrCastSkill);
		_float3 vPos = _float3(0.f, 0.f, 0.f);

		Get_Manager()->ActorAddForce(m_wstrCastSkill, &vPos, _float3(0.f, -50.f, 0.f), 500.f, _fDeltaTime);
		if (vPos != _float3(0.f, 0.f, 0.f))
		{
			m_pSkillEffect->Set_Pos(vPos);
			m_pDistortTex->Set_Pos(vPos);
			m_vSkillPos = vPos;
			m_vSkillPos.y = vPos.y - 0.5f;
		}
	}
}

void CJYantari::Cast_SkillCoolTime(const _float & _fDeltaTime)
{
	if (m_bCastSkill)
	{
		if (m_fSkillEndY >= m_vSkillPos.y)
		{
			//스킬 삭제될 때
			//m_fSkillY 이펙트 바닥 쪽 위치

			Cast_SkillDelete();
			
			if(m_bCreateEffect)
			{
			//_float3 vPos;
			//Get_Manager()->ActorAddForce(m_wstrCastSkill, &vPos, _float3(0.f, -3000.f, 0.f), 30.f, _fDeltaTime);
			//m_pSkillEffect->Set_Pos(vPos);
				_float3 vPos = m_vSkillPos;
				vPos.y = m_fSkillEndY;
	
				Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, 1.5f
					, &vPos
					, 200,
					_float4(1.f, 1.f, 1.f, 1.f),
					(ePartilcePatternType)(
						(_ulong)ePartilcePatternType::OnStartBlink
					/*	| (_ulong)ePartilcePatternType::RevRotY*/
						| (_ulong)ePartilcePatternType::x2),
					0, 1.f, D3DX_PI*2.f, 5.f);
				Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, 0.5f
					, &vPos
					, 100,
					_float4(1.f, 1.f, 1.f, 1.f),
					(ePartilcePatternType)(
					(_ulong)ePartilcePatternType::OnStartBlink
						/*	| (_ulong)ePartilcePatternType::RevRotY*/
						| (_ulong)ePartilcePatternType::x2),
					0, 1.f, D3DX_PI*2.f, 5.f);
				Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, 1.f
					, &vPos
					,100,
					_float4(1.f, 1.f, 1.f, 1.f),
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
					_float4(1.f, 1.f, 1.f, 1.f),
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
					_float4(1.f, 1.f, 1.f, 1.f),
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
					_float4(1.f, 1.f, 1.f, 1.f),
					(ePartilcePatternType)(
					(_ulong)ePartilcePatternType::OnStartBlink
						| (_ulong)ePartilcePatternType::GoStraight
						| (_ulong)ePartilcePatternType::Gravity
						/*| (_ulong)ePartilcePatternType::x2*/),
					0, 2.f, D3DX_PI*2.f, 15.f);
				Get_Manager()->Generate_SphereParticle(m_iSceneIdx, vPos, 1.5f
				, &vPos
				, 200,
				_float4(1.f, 1.f, 1.f, 1.f),
				(ePartilcePatternType)(
				(_ulong)ePartilcePatternType::GoStraight
					| (_ulong)ePartilcePatternType::OnStartBlink
					| (_ulong)ePartilcePatternType::Gravity),
				0, 1.f,D3DX_PI*2.f,10.f);
				//TRANSFORM_DESC tTempTRANSFORM_DESC;
				//tTempTRANSFORM_DESC.vScale = { 7.5f,7.5f,7.5f };
				//tTempTRANSFORM_DESC.vPos = vPos;
				//Get_Manager()->Generate_Decal(m_iSceneIdx, tTempTRANSFORM_DESC
				//	, _float4(1.f, 1.f, 1.f, 0.f), 25
				//	, (ePartilcePatternType)(
				//		(_ulong)ePartilcePatternType::OnStartBlinkPow
				//		//(_ulong)ePartilcePatternType::GoStraight
				//		//| (_ulong)ePartilcePatternType::ScaleUp
				//		| (_ulong)ePartilcePatternType::YBillBoard
				//		| (_ulong)ePartilcePatternType::x5),1.6f);

				//tTempTRANSFORM_DESC.vScale = { 5.f,10.f,1.f };
				//tTempTRANSFORM_DESC.vPos.y+=5.f;
				//Get_Manager()->Generate_Decal(m_iSceneIdx, tTempTRANSFORM_DESC
				//	, _float4(1.f, 1.f, 1.f, 0.f), 24
				//	, (ePartilcePatternType)(
				//	(_ulong)ePartilcePatternType::OnStartBlinkPow
				//		//(_ulong)ePartilcePatternType::GoStraight
				//		//| (_ulong)ePartilcePatternType::ScaleUp
				//		| (_ulong)ePartilcePatternType::YBillBoard
				//		), 1.f);

				m_bCreateEffect = false;
			}
		}
	}
}

HRESULT CJYantari::Cast_SkillEffectCreate()
{
	CEffect_Texture_Ani::TEXOBJANI_DESC tTexDescAni;
	tTexDescAni.vArgb = { 1.f,1.f,1.f,1.f };
	tTexDescAni.tTransformDesc.vPos = { 0.f, 0.f, 0.f };
	tTexDescAni.tTransformDesc.vScale = { 5.f,10.f,1.f };
	//tTexDescAni.eTexMode = CEffect_Texture::eTexMode::Distortion;
	//tTexDescAni.eMovePattern = ePartilcePatternType::BillBoard;
	tTexDescAni.eMovePattern = ePartilcePatternType::YBillBoard;
	//tTexDescAni.eMovePattern = (ePartilcePatternType)0;
	//tTexDescAni.iRenderPass = 2;
	tTexDescAni.bPlayRepeat = true;
	tTexDescAni.iRenderId = 0;
	tTexDescAni.iMaxRenderId = 15;
	tTexDescAni.fPlaySpeed = 1.f;
	lstrcpy(tTexDescAni.szTexName, L"Component_Texture_Lightning2");
	//if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Effect_Tex_Ani_Obj",
	//	(_uint)ESceneType::Static,
	//	L"Layer_Effect", &tTexDescAni)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"GameObject_Effect_Tex_Ani_Obj", (CGameObject**)&m_pSkillEffect, &tTexDescAni), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pSkillEffect, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pSkillEffect);

	m_pSkillEffect->Set_RenderSwitch(false);


	CEffect_Texture::TEXOBJ_DESC tTexDistort;

	tTexDistort.eTexMode = CEffect_Texture::eTexMode::Distortion;
	tTexDistort.iRenderPass = 8;
	tTexDistort.iRenderId = 15;
	tTexDistort.vArgb = { 1.f,1.f,1.f,1.f };
	tTexDistort.eMovePattern = ePartilcePatternType::YBillBoard;
	tTexDistort.tTransformDesc.vPos = { 0.f, 0.f, 0.f };
	tTexDistort.tTransformDesc.vScale = { 5.f,10.f,1.f };
	//if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(
	//	(_uint)ESceneType::Static,
	//	L"GameObject_Tex_Obj",
	//	(_uint)ESceneType::Static,
	//	L"Layer_Effect", &tTexDescAni)))
	//{
	//	ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
	//	return E_FAIL;
	//}

	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static, L"GameObject_Tex_Obj", (CGameObject**)&m_pDistortTex, &tTexDistort), E_FAIL);
	FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pDistortTex, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	Safe_AddRef(m_pDistortTex);
	m_pDistortTex->Set_RenderSwitch(false);

	return S_OK;
}

void CJYantari::AttackCoolTimer(const _float & _fDeltaTime)
{
	if (m_bNearPattern2)
	{
		if (::CoolTime(_fDeltaTime, m_fNearPattern2Time, 10.f))
		{
			m_bNearPattern2 = false;
			m_fNearPattern2Time = 0.f;
		}
	}
	if (m_bNearPattern3)
	{
		if (::CoolTime(_fDeltaTime, m_fNearPattern3Time, 8.f))
		{
			m_bNearPattern3 = false;
			m_fNearPattern3Time = 0.f;
		}
	}
	if (m_bNearPattern4)
	{
		if (::CoolTime(_fDeltaTime, m_fNearPattern4Time, 3.f))
		{
			m_bNearPattern4 = false;
			m_fNearPattern4Time = 0.f;
		}
	}
	if (m_bNearPattern5)
	{
		if (::CoolTime(_fDeltaTime, m_fNearPattern5Time, 3.f))
		{
			m_bNearPattern5 = false;
			m_fNearPattern5Time = 0.f;
		}
	}
	if (m_bFarPattern1)
	{
		if (::CoolTime(_fDeltaTime, m_fFarPattern1Time, 7.f))
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
	if (m_bCastPattern)
	{
		if (::CoolTime(_fDeltaTime, m_fCastPatternTime, 10.f))
		{
			m_bCastPattern = false;
			m_fCastPatternTime = 0.f;
		}
	}
}

void CJYantari::WalkSound()
{
	_uint iCount = Mersen_ToInt(0, 2);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Yantari_Walk%d.ogg", iCount);

	CSound_Manager::ESoundID eSound;
	switch (m_iSoundIndex)
	{
	case 0:
		eSound = CSound_Manager::ESoundID::Monster_Move3D_0;
		break;
	case 1:
		eSound = CSound_Manager::ESoundID::Monster_Move3D_1;
		break;
	case 2:
		eSound = CSound_Manager::ESoundID::Monster_Move3D_2;
		break;
	}

	GET_MANAGEMENT->StopSound(eSound);
	GET_MANAGEMENT->Play3DSounds(szFileName, eSound, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f,false,0.5f);
}

void CJYantari::DashSound()
{
	_uint iCount = Mersen_ToInt(0, 4);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Yantari_Dash%d.ogg", iCount);

	CSound_Manager::ESoundID eSound;
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

	_float3 vPos;
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	GET_MANAGEMENT->StopSound(eSound);
	GET_MANAGEMENT->Play3DSounds(szFileName, eSound, vPos, 0.f, 50.f,false,1.f);

}

void CJYantari::TrailFixWidth()
{
	if (m_pWeapon)
	{
		if (m_pMesh->Get_Animationset() == (_uint)YantariAniID::Attack4)
		{
			m_pWeapon->Set_TrailWidth(50.f);
		}
		else
			m_pWeapon->Set_TrailWidth(75.f);
	}
}

CJYantari * CJYantari::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJYantari* pInstance = new CJYantari(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJYantari::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CJYantari* pClone = new CJYantari(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJYantari::Free()
{
	CJBaseMonster::Free();

	if (m_bCheckClone)
	{
		Safe_Release(m_pSkillEffect);
		Safe_Release(m_pDistortTex);
	}
}
