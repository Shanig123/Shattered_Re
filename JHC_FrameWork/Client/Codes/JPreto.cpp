#include "stdafx.h"
#include "JPreto.h"
#include "CinematicCamera_0.h"
#include "MainCamera.h"
#include"Player_UI.h"

#define BOSS_CHECK_ACTIVATION (m_ePreTotalPattern == eBossMonsterPattern::Awake ||		\
m_ePreTotalPattern == eBossMonsterPattern::Move ||											\
m_ePreTotalPattern == eBossMonsterPattern::Idle ||											\
m_ePreTotalPattern == eBossMonsterPattern::Hit ||											\
m_ePreTotalPattern == eBossMonsterPattern::Attack||											\
m_ePreTotalPattern == eBossMonsterPattern::Dead )


CJPreto::CJPreto(_DEVICE _pDevice, const wstring& _PrototypeName)
	: CJBaseBoss(_pDevice, _PrototypeName)
	, m_ePattern(PATTERN::End)
	, m_bCienmaticShakeStart(false)
	, m_fAppearDelayTime(0.f)
	, m_iAttackCount(0)
	, m_bNearPattern3WeaponRatate(false)
	, m_fFallDamage(0.f)
	, m_fAirTime(0.f)
	, m_pTrailEffect(nullptr)
	, m_pShield(nullptr)
	, m_fAccelTime(0.f)
	, m_bNearPattern2(false)
	, m_bNearPattern3(false)
	, m_bNearPattern4(false)
	, m_fNearPattern2Time(0.f)
	, m_fNearPattern3Time(0.f)
	, m_fNearPattern4Time(0.f)
	, m_vDropFieldPos(0.f, 0.f, 0.f)
	, m_bBackDash(false)
	, m_fBackDash(0.f)
	, m_bAttackSound2(false)
	, m_bResetPreto(false)
{
}

CJPreto::CJPreto(const CJPreto & _rOther)
	: CJBaseBoss(_rOther)
	, m_ePattern(PATTERN::End)
	, m_bCienmaticShakeStart(false)
	, m_fAppearDelayTime(0.f), m_fDeadPatilceEffectTime(0.f)
	, m_iAttackCount(0)
	, m_bNearPattern3WeaponRatate(false)
	, m_fFallDamage(0.f)
	, m_fAirTime(0.f)
	, m_pTrailEffect(nullptr)
	, m_pShield(nullptr)
	, m_fAccelTime(0.f)
	, m_bNearPattern2(false)
	, m_bNearPattern3(false)
	, m_bNearPattern4(false)
	, m_fNearPattern2Time(0.f)
	, m_fNearPattern3Time(0.f)
	, m_fNearPattern4Time(0.f)
	, m_vDropFieldPos(0.f, 0.f, 0.f)
	, m_bBackDash(false)
	, m_fBackDash(0.f)
	, m_bAttackSound2(false)
	, m_bResetPreto(false)
{
}

HRESULT CJPreto::Ready_GameObject_Prototype()
{
	if (FAILED(CJBaseBoss::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJPreto::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (FAILED(CJBaseBoss::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(CJPreto::Weapon_Clone()))
	{
		ERROR_BOX(L"Failed to Weapon_Clone");
		return E_FAIL;
	}

	if (FAILED(CJPreto::UI_Clone()))
	{
		ERROR_BOX(L"Failed to UI_Clone");
		return E_FAIL;
	}

	m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);

	m_iShaderPass = 2;

	m_tState.fAtt = 70.f;
	m_tState.fDef = 80.f;
	m_tState.fMaxHP = 1000.f;
	m_tState.fHP = m_tState.fMaxHP;
	m_fAddAniPlayTime = .7f;

	//릴리즈에서는 이 포스 안맞음;
	//m_vResetPos = _float3(-510.f, 193.f, 20.f);
	//*const_cast<_float3*>(&m_vInitilizePos) = _float3(-510.f, 185.055649f, 20.f);
	*const_cast<_float3*>(&m_vInitilizePos) = m_pTransform->Get_TransformDesc().vPos;
	//_float3 vCreatePos = m_tTransformDesc.vPos;
	//vCreatePos.y = 30.f;

	//*const_cast<_float3*>(&m_vInitilizePos) = vCreatePos;
	m_vDissolveColor = _float4(2.2f, 2.2f, 2.2f, 0.5f);

	m_WepaonTitle[0] = (wstring)m_tTransformDesc.szFileTitle + L"_Sword_Weapon";
	m_WepaonTitle[1] = (wstring)m_tTransformDesc.szFileTitle + L"_Shield_Weapon";
	m_iItemCreateYAxisPosFix = 5.f;
	auto pManagement = GET_MANAGEMENT;
	m_vDropFieldPos = _float3(-510.f, 200.f, 20.f);
	pManagement->Add_Trigger(L"BossUnDropField", PxTransform(PxVec3(m_vDropFieldPos.x, m_vDropFieldPos.y, m_vDropFieldPos.z)), EInteractionType::Box, PxBoxGeometry(35.f, 30.f, 38.f));

	if (FAILED(CJPreto::Clone_Trail()))
	{
		ERROR_BOX(L"Failed to Clone_Trail");
		return E_FAIL;
	}

	return S_OK;
}

_uint CJPreto::Update_GameObject(const _float & _fDeltaTime)
{
	CJBaseBoss::Update_GameObject(_fDeltaTime);

	if (BOSS_CHECK_ACTIVATION)
	{
		Set_WeaponWorld();
	}

	if (m_ePreTotalPattern == eBossMonsterPattern::Move ||
		m_ePreTotalPattern == eBossMonsterPattern::Attack ||
		m_ePreTotalPattern == eBossMonsterPattern::Idle)
	{
		if (m_pTargetTransform->Get_TransformDesc().vPos.x < -540.f ||
			m_pTargetTransform->Get_TransformDesc().vPos.x > -470.f ||
			m_pTargetTransform->Get_TransformDesc().vPos.z > 62.f ||
			m_pTargetTransform->Get_TransformDesc().vPos.z < -26.f)
		{
			if (!m_bResetPreto)
			{
				m_bResetPreto = true;
				Reset_Pattern(_fDeltaTime);
			}

		}
		else
		{
			m_bResetPreto = false;
		}
	}

	CJPreto::AttackCoolTimer(_fDeltaTime);

	//if (m_pWeapon)
	//	m_pWeapon->Trail_Switch(true);
	if (PATTERN::Near_3 == m_ePattern)
	{
		m_pWeapon->Set_TrailRateTime(0.01f);
	}
	else
	{
		m_pWeapon->Set_TrailRateTime(0.025f);
	}
	return m_iEvent;
}


HRESULT CJPreto::Render_GameObject()
{
	m_pMesh->Play_Animationset(m_fDeltaTime * m_fAddAniPlayTime);

	CJBaseBoss::Render_GameObject();

	_float4x4 matScale, matRotate, matTrans;
	D3DXMatrixScaling(&matScale, 0.5f, 0.5f, 0.5f);
	D3DXMatrixRotationYawPitchRoll(&matRotate, D3DXToRadian(0.f), D3DXToRadian(-30.f), D3DXToRadian(50.f));
	D3DXMatrixTranslation(&matTrans, 0.f, 0.f, 0.f);
	m_matChildWeapon = matScale * matRotate * matTrans;
	m_matChildWeapon *= m_matWeapon[0];

	_float4x4 matShieldScale, matShieldRotate, matShieldTrans;
	D3DXMatrixScaling(&matShieldScale, .5f, .5f, .5f);
	D3DXMatrixRotationYawPitchRoll(&matShieldRotate, D3DXToRadian(-40.f), D3DXToRadian(0.f), D3DXToRadian(180.f));
	D3DXMatrixTranslation(&matShieldTrans, 0.f, 0.f, 0.f);
	m_matSecondChildWeapon = matShieldScale * matShieldRotate * matShieldTrans;
	m_matSecondChildWeapon *= m_matWeapon[1];

	return S_OK;
}

HRESULT CJPreto::Weapon_Clone()
{
	auto pManagement = GET_MANAGEMENT;

	wstring WepaonTitle_Shield = (wstring)m_tTransformDesc.szFileTitle + L"_Shield_Weapon";

	TRANSFORM_DESC tTransformDesc;

	lstrcpy(tTransformDesc.szFileTitle, WepaonTitle_Shield.c_str());

	if (FAILED(pManagement->Clone_GameObject(
		(_uint)ESceneType::Static,
		L"GameObject_PretoShield",
		(CGameObject**)&m_pShield,
		&tTransformDesc,
		m_iSceneIdx)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	if (FAILED(pManagement->Clone_GameObject_ToLayer(m_pShield, m_iSceneIdx, L"Layer_Weapon")))
	{
		ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	m_pShield->Set_ParentClass(this, CJBaseWeapon::eMonsterParent::Boss);
	Safe_AddRef(m_pShield);
	return S_OK;
}

HRESULT CJPreto::UI_Clone()
{
	auto pManagement = GET_MANAGEMENT;
	//보스몬스터 배경 체력바 UI
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
	m_pBossUI->Set_BossName(CUI_BossMonster::eBossName::Preto);

	return S_OK;
}

void CJPreto::AttackCoolTimer(const _float & _fDeltaTime)
{
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
	if (m_bNearPattern4)
	{
		if (::CoolTime(_fDeltaTime, m_fNearPattern4Time, 5.f))
		{
			m_bNearPattern4 = false;
			m_fNearPattern4Time = 0.f;
		}
	}

	if (m_bBackDash)
	{
		if (::CoolTime(_fDeltaTime, m_fBackDash, 6.f))
		{
			m_bBackDash = false;
			m_fBackDash = 0.f;
		}
	}


}

void CJPreto::Do_Activating(const _float & _fDeltaTime)
{
	if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAY_BOSS_CINE) && 
		!(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_SKIP_BOSS_CINE))
		Appear_Pattern(_fDeltaTime);
	else
	{
		if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
		{
			m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);
			m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
			m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));
			
			void* Data = Get_Manager()->GetData_Controller(m_tTransformDesc.szFileTitle);
			MyMonsterData* MonsterData = (MyMonsterData*)Data;
			MonsterData->DropField = true;
			m_bDropField = false;
			Get_Manager()->SetData_Controller(m_tTransformDesc.szFileTitle, MonsterData);

			if (m_fTargetDist < 43.f &&
				BOSS_CHECK_ACTIVATION)
			{
				if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_SKIP_BOSS_CINE))
				{
					GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::BGM);
					GET_MANAGEMENT->PlayBGM(L"Preto_Battle.mp3", BGM_VOLUME);
				}

				m_eCurTotalPattern = eBossMonsterPattern::Idle;
			}
		}
		else
			m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);

	}

	CJBaseBoss::Do_Activating(_fDeltaTime);
}

void CJPreto::Do_Idle(const _float & _fDeltaTime)
{
	
	m_bTargetLook = true;
	if (!m_bResetPreto)
	{
		if (!m_bAttackCoolTime)
		{
			m_eCurTotalPattern = eBossMonsterPattern::Move;
			m_ePattern = PATTERN::Pattern_Ready;
		}
	}
}

void CJPreto::Do_Attack(const _float & _fDeltaTime)
{
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
		case PATTERN::Walk_F:
			if (m_bDropField)
				Walk_Pattern_BacktoField(_fDeltaTime);
			else
				Walk_Pattern_F(_fDeltaTime);
			break;
		case PATTERN::Walk_L:
			Walk_Pattern_L(_fDeltaTime);
			break;
		case PATTERN::Walk_R:
			Walk_Pattern_R(_fDeltaTime);
			break;
		case PATTERN::Dash_B:
			Dash_Pattern(_fDeltaTime);
			break;
		case PATTERN::Death:
			Death_Pattern(_fDeltaTime);
			break;
		case PATTERN::Reset:
			Reset_Pattern(_fDeltaTime);
			break;
		}
	}
	else
	{
		if (m_bResetPreto)
		{
			Reset_Pattern(_fDeltaTime);
		}
		else
		{
			if (m_fTargetDist <= 5.f)
			{
				switch (Mersen_ToInt(0, 3))
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
						Near_Pattern_4(_fDeltaTime);
					else
						Near_Pattern_1(_fDeltaTime);
					break;
				}
				++m_iAttackCount;
			}
			else
			{
				Far_Pattern_1(_fDeltaTime);
			}
		}
	}
}

void CJPreto::Do_Hit(const _float & _fDeltaTime)
{
}

void CJPreto::Do_Walk(const _float & _fDeltaTime)
{
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
		case PATTERN::Walk_F:
			if (m_bDropField)
				Walk_Pattern_BacktoField(_fDeltaTime);
			else
				Walk_Pattern_F(_fDeltaTime);
			break;
		case PATTERN::Walk_L:
			Walk_Pattern_L(_fDeltaTime);
			break;
		case PATTERN::Walk_R:
			Walk_Pattern_R(_fDeltaTime);
			break;
		case PATTERN::Dash_B:
			Dash_Pattern(_fDeltaTime);
			break;
		case PATTERN::Death:
			Death_Pattern(_fDeltaTime);
			break;
		case PATTERN::Reset:
			Reset_Pattern(_fDeltaTime);
			break;
		}
	}
	else
	{
		if (m_bResetPreto)
		{
			Reset_Pattern(_fDeltaTime);
		}
		else
		{
			if (m_fTargetDist <= 8.f)
			{
				if (m_iAttackCount > 2)
				{
					if (!m_bBackDash)
					{
						_float3 vPos = _float3{ m_vDropFieldPos.x,0.f,m_vDropFieldPos.z } -_float3{ m_pTransform->Get_TransformDesc().vPos.x,0.f, m_pTransform->Get_TransformDesc().vPos.y };

						_float fLenghth = D3DXVec3Length(&(vPos));

						if (fLenghth < 165.f)
							m_iDelayMovePattern = 3;
						else
							m_iDelayMovePattern = Mersen_ToInt(1, 2);
					}
					else
						m_iDelayMovePattern = Mersen_ToInt(1, 2);

					m_iAttackCount = 0;
				}
				else
				{
					m_eCurTotalPattern = eBossMonsterPattern::Attack;
					m_ePattern = PATTERN::Pattern_Ready;
					m_iDelayMovePattern = 5;
				}
			}
			else if (m_fTargetDist <= 18.f && m_fTargetDist > 8.f)
			{
				if (m_bDropField)
					m_iDelayMovePattern = 4;
				else
					m_iDelayMovePattern = Mersen_ToInt(1, 2);
			}
			else
			{
				if (m_bDropField)
					m_iDelayMovePattern = 4;
				else
					m_iDelayMovePattern = 0;
			}

			switch (m_iDelayMovePattern)
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
			case 3:
				Dash_Pattern(_fDeltaTime);
				break;
			case 4:
				Walk_Pattern_BacktoField(_fDeltaTime);
				break;
			default:
				break;
			}
		}
	}
}

void CJPreto::Do_Other(const _float & _fDeltaTime)
{
	if (m_ePreTotalPattern == eBossMonsterPattern::Sleep)
	{
		//강체 삭제
	//m_fSleepTime += _fDeltaTime;
		//if (::CoolTime(_fDeltaTime, m_fSleepCountTime, m_fSleepTime_Const))
		//{

		//	m_fDissolveTime = 0.f;
		//	m_eCurTotalPattern = eBossMonsterPattern::Ready;
		//	m_pMesh->Set_Animationset((_uint)LegionnaireAniID::Idle);
		//	m_fSleepCountTime = 0.f;
		//}
		/*
			Sleep->Ready 정의 필요함.
			아마 플레이어가 트리거 밟았을 때?
		*/


		m_fTargetDist = D3DXVec3Length(&(m_pTargetTransform->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos));

		if (m_fTargetDist < m_fFarRange &&
			m_ePreTotalPattern == eBossMonsterPattern::Sleep)
		{
			m_eCurTotalPattern = eBossMonsterPattern::Ready;
		}

		//if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAY_BOSS_CINE)
		//	&& m_ePreTotalPattern == eBossMonsterPattern::Sleep)
		//{
		//	m_eCurTotalPattern = eBossMonsterPattern::Ready;
		//}
		//Appear_Pattern(_fDeltaTime);
	}
	else if (m_ePreTotalPattern == eBossMonsterPattern::Dead)
	{
		//m_fDissolveTime += m_fDeltaTime * 0.5f;
		Death_Pattern(_fDeltaTime);
	}
}

void CJPreto::Delete_RigidBody()
{
	Get_Manager()->Delete_Controller(m_tTransformDesc.szFileTitle);

	for (_uint i = 0; i < 2; ++i)
	{
		Get_Manager()->Delete_Trigger(m_WepaonTitle[i]);
	}
}

void CJPreto::Set_Attack(_bool _bLeft, _bool _bRight)
{
	void* Data;
	Get_Manager()->GetData_Trigger(m_WepaonTitle[1], &Data);
	MyPublicData* WeaponData = (MyPublicData*)Data;
	WeaponData->Collision = _bLeft;
	Get_Manager()->SetData_Trigger(m_WepaonTitle[1], (void*)WeaponData);
	if (m_pTrailEffect)
		m_pTrailEffect->Set_RenderOnOff(_bRight);
	if (m_pWeapon)
		m_pWeapon->Trail_Switch(_bRight);

	void* SickleData;
	Get_Manager()->GetData_Trigger(m_WepaonTitle[0], &SickleData);
	MyPublicData* SickleWeaponData = (MyPublicData*)SickleData;
	SickleWeaponData->Collision = _bRight;
	Get_Manager()->SetData_Trigger(m_WepaonTitle[0], (void*)SickleWeaponData);
}

void CJPreto::Set_WeaponWorld()
{
	// 0 - 무기, 1 - 방패
	_float4x4 matWorld, matScale, matRotate, matTrans;
	const MY_FRAME* pMonsterFrame = m_pMesh->Get_FrameByName("B_Atache_R");
	m_matWeapon[0] = pMonsterFrame->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();	
	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixRotationYawPitchRoll(&matRotate, D3DXToRadian(0.f), D3DXToRadian(0.f), D3DXToRadian(45.f));
	D3DXMatrixTranslation(&matTrans, 0.f, 0.f, 0.f);
	m_matNearPattern3 = matScale * matRotate * matTrans;
	m_matNearPattern3 *= m_matWeapon[0];
		

	const MY_FRAME* pMonsterFrame1 = m_pMesh->Get_FrameByName("B_Atache_L");

	D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
	D3DXMatrixRotationYawPitchRoll(&matRotate, D3DXToRadian(0.f), D3DXToRadian(0.f), D3DXToRadian(30.f));
	D3DXMatrixTranslation(&matTrans, 0.f, 0.f, 0.f);
	matWorld = matScale * matRotate * matTrans;
	m_matWeapon[1] = matWorld * pMonsterFrame1->CombinedTranformationMatrix * *m_pTransform->Get_WorldMatrix();

	for (_uint i = 0; i < 2; ++i)
	{
		_float3 vScale;
		_float4Q vQuat;
		
		if(m_bNearPattern3WeaponRatate && i == 0)
			D3DXMatrixDecompose(&vScale, &vQuat, &m_vWeaponPos[i], &m_matNearPattern3);
		else
			D3DXMatrixDecompose(&vScale, &vQuat, &m_vWeaponPos[i], &m_matWeapon[i]);
		
		Get_Manager()->Weapon_Update(m_WepaonTitle[i], m_vWeaponPos[i], vQuat);
	}	
}

void CJPreto::Appear_Pattern(const _float & _fDeltaTime)
{
	// 등장 씬
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Appear;
		m_pMesh->Set_Animationset((_uint)PretoAniID::Appear);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_bAttackSound = true;
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Appear)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(101, 23) && !m_pMesh->Get_AnimationFrame(101, 42))
		{
			m_fAddAniPlayTime = 0.175f;
			m_bApplyGravity = false;

			_float3 vPos = Get_Manager()->Player_Gravity(m_tTransformDesc.szFileTitle, &m_bJump, &m_bJumpStart, &m_bDoubleJump, &m_bJumping, &m_bGravity, &m_bGravityStart, &m_iJump, &m_iDash, &m_vJumpPos, 30.f, &m_fJumpTime, &m_fFallDamage, &m_fAirTime, _fDeltaTime);
			
			m_pTransform->Set_Pos(vPos);
			m_pTransform->Set_Pos(Get_Manager()->Move(m_tTransformDesc.szFileTitle, AppearLook(), 30.f, _fDeltaTime));
			m_bCienmaticShakeStart = true;
		}
		else
		{
			if (m_bCienmaticShakeStart)
			{
				CCinematicCamera_0* pCamera = dynamic_cast<CCinematicCamera_0*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
				pCamera->CamShakeQuake();
				m_bCienmaticShakeStart = false;
			}

			m_fAddAniPlayTime = .7f;
			m_bApplyGravity = true;
		}
		if (m_pMesh->Get_AnimationFrame(101, 40))
		{
			if (m_bAttackSound)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::BGM);
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Preto_Appear.wav", CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 100.f);
				
				m_bAttackSound = false;
			}
		}
		if (m_pMesh->Is_AnimationSetEnd())
		{
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::BGM);
			GET_MANAGEMENT->PlayBGM(L"Preto_Battle.mp3", BGM_VOLUME);
			m_bAppearEnd = true;
			m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);
			m_eCurTotalPattern = eBossMonsterPattern::Idle;
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_END_BOSS_CINE | EVENT_VOLATILE_SKIP_BOSS_CINE);
			*const_cast<_float3*>(&m_vInitilizePos) = _float3(-510.f, 185.055649f, 20.f);
		}
	}
}

void CJPreto::Near_Pattern_1(const _float & _fDeltaTime)
{
	// 2회 연타 공격
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Near_1;
		m_pMesh->Set_Animationset((_uint)PretoAniID::Attack1);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_fAddAniPlayTime = 1.f;
		m_bAttackSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Attack1)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(68, 6) && !m_pMesh->Get_AnimationFrame(68, 41))
		{
			Set_Attack(false, true);
			if (m_bAttackSound)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Preto_Near1.ogg", CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
				m_bAttackSound = false;
			}
		}
		else
			Set_Attack(false, false);

		if (m_pMesh->Is_AnimationSetEnd())
		{
			CJPreto::AttackEndToMove_ReadyIdle();
		}
	}
}

void CJPreto::Near_Pattern_2(const _float & _fDeltaTime)
{
	// 내리 찍기
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Near_2;
		m_pMesh->Set_Animationset((_uint)PretoAniID::Attack3);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_fAddAniPlayTime = .85f;
		m_bAttackSound = true;
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Attack3)
	{
		m_bTargetLook = false;
		if (m_pMesh->Get_AnimationFrame(68, 17) && !m_pMesh->Get_AnimationFrame(68, 27))
		{
			Set_Attack(false, true);

		}
		else
			Set_Attack(false, false);

		if (m_bAttackSound && m_pMesh->Get_AnimationFrame(68, 26))
		{
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
			GET_MANAGEMENT->Play3DSounds(L"Preto_Near2.ogg", CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
			m_bAttackSound = false;
		}

		m_bNearPattern2 = true;

		if (m_pMesh->Is_AnimationSetEnd())
		{
			CJPreto::AttackEndToMove_ReadyIdle();
			m_bNearPattern2 = true;
		}
	}
}

void CJPreto::Near_Pattern_3(const _float & _fDeltaTime)
{
	// 부메랑
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Near_3;
		m_pMesh->Set_Animationset((_uint)PretoAniID::Attack2);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_fAddAniPlayTime = 1.f;
		m_bAttackSound = true;
		m_bAttackSound2 = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Attack2)
	{
		m_bTargetLook = false;

		if (m_pMesh->Get_AnimationFrame(122, 25))
		{
			if (m_bAttackSound2)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Preto_Near3_2.ogg", CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
				m_bAttackSound2 = false;
			}
		}

		if (m_pMesh->Get_AnimationFrame(122, 52) && !m_pMesh->Get_AnimationFrame(122, 81))
		{
			if (m_bAttackSound)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Preto_Near3_1.ogg", CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
				m_bAttackSound = false;
			}
			m_bNearPattern3WeaponRatate = true;
			Set_Attack(false, true);
		}
		else
		{
			m_bNearPattern3WeaponRatate = false;
			Set_Attack(false, false);
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			CJPreto::AttackEndToMove_ReadyIdle();
			m_bNearPattern3 = true;
		}
	}
}

void CJPreto::Near_Pattern_4(const _float & _fDeltaTime)
{
	// 방패로 내리찍기
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Near_4;
		m_pMesh->Set_Animationset((_uint)PretoAniID::ShieldAttack2);
		m_fSkillMoveSpeed = m_tTransformDesc.fSpeedPerSecond;
		m_fAddAniPlayTime = 0.7f;
		m_bAttackSound = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::ShieldAttack2)
	{
		m_bTargetLook = false;
		
		if (m_pMesh->Get_AnimationFrame(34, 8))
		{
			if (m_bAttackSound)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Preto_Near2.ogg", CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
				m_bAttackSound = false;
			}
		}

		if (!m_pMesh->Get_AnimationFrame(34, 15))
		{
			Set_Attack(true, false);
		}
		else
			Set_Attack(false, false);

		if (m_pMesh->Get_AnimationFrame(34, 9))
		{
			if (m_bApplyGravity)
			{
				m_vNear4SitPos = m_pTransform->Get_TransformDesc().vPos;
				m_vNear4SitPos.y += 2.8f;
				m_pTransform->Set_Pos(GET_MANAGEMENT->Set_GlobalPos(m_tTransformDesc.szFileTitle, m_vNear4SitPos));
				m_bApplyGravity = false;
			}
		}

		if (m_pMesh->Get_AnimationFrame(34, 12)&& !m_pMesh->Get_AnimationFrame(34, 26))
		{
			m_pTransform->Set_Pos(GET_MANAGEMENT->Move(m_tTransformDesc.szFileTitle, { 0.f, 1.f, 0.f }, 2.2f, _fDeltaTime));
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			CJPreto::AttackEndToMove_ReadyIdle();
			m_bNearPattern4 = true;
			m_bApplyGravity = true;
		}
	}
}

void CJPreto::Far_Pattern_1(const _float & _fDeltaTime)
{
	// 방패 돌진
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Far_1;
		m_pMesh->Set_Animationset((_uint)PretoAniID::ShieldAttack1);
		m_fAddAniPlayTime = 1.3f;
		m_fSkillMoveSpeed = 3.f;
		m_fAccelTime = 0.f;
		m_bAttackSound = true;
		m_bRightLeg = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::ShieldAttack1)
	{
		m_bTargetLook = false;
		m_fAccelTime += _fDeltaTime;

		if (m_pMesh->Get_AnimationFrame(101, 72))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(101, 60))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(101, 43))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(101, 28))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_pMesh->Get_AnimationFrame(101, 52) && !m_pMesh->Get_AnimationFrame(101, 72))
		{
			if (m_fTargetDist<5.5f)
			{
				m_fAddAniPlayTime = 1.6f;
			}
			if (m_bAttackSound)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
				GET_MANAGEMENT->Play3DSounds(L"Preto_ShiledDash.ogg", CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
				m_bAttackSound = false;
			}
			Set_Attack(true, false);
		}
		else
		{
			m_fAddAniPlayTime = 1.3f;
			Set_Attack(false, false);
		}


		if (!m_pMesh->Get_AnimationFrame(101, 30))
		{
			if (m_fSkillMoveSpeed < 50.f)
			{
				m_fSkillMoveSpeed += powf(1.2f, m_fAccelTime)*0.35f;
			}
			else
			{
				m_fSkillMoveSpeed = 50.f;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(101, 72))
		{
			if (m_fSkillMoveSpeed > 0.f)
			{
				m_fSkillMoveSpeed -= powf(1.2f, m_fAccelTime)*0.3f;
			}
			else
			{
				m_fSkillMoveSpeed = 0.f;
			}
		}

		if (!m_bDropField && m_fTargetDist > 4.5f)
		{
			_float3 vPos = pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime);

			vPos.y -= 1.f;

			if (m_pMesh->Get_AnimationFrame(101, 73) && !m_pMesh->Get_AnimationFrame(101, 83))
			{
				vPos.y -= 1.f*_fDeltaTime;
			}
			else if (m_pMesh->Get_AnimationFrame(101, 33) && !m_pMesh->Get_AnimationFrame(101, 96))
			{
				vPos.y += 1.f*_fDeltaTime;
			}

			m_pTransform->Set_Pos(vPos);
		}
		
		if (m_pMesh->Is_AnimationSetEnd())
		{
			CJPreto::AttackEndToMove_ReadyIdle();
			m_fAddAniPlayTime = 0.7f;
		}

	}

}

void CJPreto::Walk_Pattern_F(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_F;
		m_pMesh->Set_Animationset((_uint)PretoAniID::Walk_Forward);
		m_fSkillMoveSpeed = 7.f;
		m_fAddAniPlayTime = 1.5f;
		m_bRightLeg = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Walk_Forward)
	{
		m_bTargetLook = true;
		m_fWalkMoveTime += _fDeltaTime;

		if (!m_bDropField)
		{
			_float3 vPos = pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime);
			vPos.y -= 1.f;
			m_pTransform->Set_Pos(vPos);
		}

		if (m_pMesh->Get_AnimationFrame(77, 72))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(77, 36))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}
		
		if (m_fTargetDist < 5.f || m_bDropField || m_fWalkMoveTime > 5.f)
		{
			if (m_fTargetDist < 23.f)
			{
				m_eCurTotalPattern = eBossMonsterPattern::Attack;
				m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);
				m_ePattern = PATTERN::Pattern_Ready;
				m_fAddAniPlayTime = 1.f;
				m_fWalkMoveTime = 0.f;
			}
		}
	}
}

void CJPreto::Walk_Pattern_BacktoField(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_F;
		m_pMesh->Set_Animationset((_uint)PretoAniID::Walk_Forward);
		m_fSkillMoveSpeed = 7.f;
		m_fAddAniPlayTime = 1.5f;
		m_bRightLeg = true;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Walk_Forward)
	{
		m_bTargetLook = true;

		_float3 vPos = pManagement->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime);
		vPos.y -= 1.f;
		m_pTransform->Set_Pos(vPos);
		_uint iCount = Mersen_ToInt(0, 5);

		if (m_pMesh->Get_AnimationFrame(77, 72))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(77, 36))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		m_fWalkMoveTime += _fDeltaTime;

		if (!m_bDropField && m_fWalkMoveTime > 5.f)
		{
			m_fWalkMoveTime = 0.f;
			m_eCurTotalPattern = eBossMonsterPattern::Attack;
			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);
			m_fAddAniPlayTime = 1.f;
		}
	}
}

void CJPreto::Walk_Pattern_L(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_L;
		m_pMesh->Set_Animationset((_uint)PretoAniID::Walk_Left);
		m_fSkillMoveSpeed = 5.f;
		m_fAddAniPlayTime = 1.2f;
		m_bRightLeg = true;
	}

	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Walk_Left)
	{
		m_bTargetLook = true;
		
		if (!m_bDropField)
		{
			_float3 vPos = pManagement->Move(m_tTransformDesc.szFileTitle, m_vRight, m_fSkillMoveSpeed, _fDeltaTime);
			vPos.y -= 1.f;
			m_pTransform->Set_Pos(vPos);
		}

		m_fWalkMoveTime += _fDeltaTime;
		_uint iCount = Mersen_ToInt(0, 5);

		if (m_pMesh->Get_AnimationFrame(77, 72))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(77, 36))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_fWalkMoveTime > 4.f || m_bDropField)
		{
			//m_bDelayMove = false;
			m_eCurTotalPattern = eBossMonsterPattern::Attack;
			m_fWalkMoveTime = 0.f;
			m_fAddAniPlayTime = 1.f;

			if (m_fTargetDist < 5.f)
			{
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);
			}
			else
			{
				m_fSkillMoveSpeed = 7.f;
				m_fAddAniPlayTime = 1.5f;
				m_ePattern = PATTERN::Walk_F;
				m_pMesh->Set_Animationset((_uint)PretoAniID::Walk_Forward);
			}
		}
	}

}

void CJPreto::Walk_Pattern_R(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Walk_R;
		m_pMesh->Set_Animationset((_uint)PretoAniID::Walk_Right);
		m_fSkillMoveSpeed = 5.f;
		m_fAddAniPlayTime = 1.2f;
		m_bRightLeg = true;
	}
	
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Walk_Right)
	{
		m_bTargetLook = true;
		if (!m_bDropField)
		{
			_float3 vPos = pManagement->Move(m_tTransformDesc.szFileTitle, -m_vRight, m_fSkillMoveSpeed, _fDeltaTime);
			vPos.y -= 1.f;
			m_pTransform->Set_Pos(vPos);
		}

		m_fWalkMoveTime += _fDeltaTime;
		_uint iCount = Mersen_ToInt(0, 5);

		if (m_pMesh->Get_AnimationFrame(77, 72))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();

				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(77, 36))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_fWalkMoveTime > 4.f || m_bDropField)
		{
			m_eCurTotalPattern = eBossMonsterPattern::Attack;
			m_fWalkMoveTime = 0.f;
			m_fAddAniPlayTime = 1.f;

			if (m_fTargetDist < 5.f)
			{
				m_ePattern = PATTERN::Pattern_Ready;
				m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);
			}
			else
			{
				m_fSkillMoveSpeed = 7.f;
				m_fAddAniPlayTime = 1.5f;
				m_ePattern = PATTERN::Walk_F;
				m_pMesh->Set_Animationset((_uint)PretoAniID::Walk_Forward);
			}
		}
	}
}

void CJPreto::Dash_Pattern(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Idle)
	{
		m_ePattern = PATTERN::Dash_B;
		m_pMesh->Set_Animationset((_uint)PretoAniID::Back_Dash);
		m_fSkillMoveSpeed = 50.f;
		m_fAddAniPlayTime = 1.f;
		if (m_iAttackCount > 2)
			m_iAttackCount = 0;
		m_bAttackSound = true;
		m_bRightLeg = true;
	}

	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Back_Dash)
	{
		m_bTargetLook = true;
		
		if (m_pMesh->Get_AnimationFrame(26, 12))
		{
			m_fAddAniPlayTime = .5f;
		}

		m_fAccelTime += _fDeltaTime;
		m_fSkillMoveSpeed -= 2.f * powf(0.5f, m_fAccelTime);

		if (!m_pMesh->Get_AnimationFrame(26, 15))
		{
			if (!m_bDropField && m_fSkillMoveSpeed > 0.f)
				m_pTransform->Set_Pos(Get_Manager()->Move(m_tTransformDesc.szFileTitle, -m_vLook, m_fSkillMoveSpeed, _fDeltaTime));
		}
		
		if (m_pMesh->Get_AnimationFrame(26, 15))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(26, 0))
		{
			if (m_bRightLeg)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
				GET_MANAGEMENT->Play3DSounds(L"PretoBackDash.ogg", CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f);
				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}

		if (m_pMesh->Is_AnimationSetEnd())
		{
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
			m_fAddAniPlayTime = 1.f;

			m_ePattern = PATTERN::Pattern_Ready;
			m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);

			m_eCurTotalPattern = m_bDropField ? eBossMonsterPattern::Move : eBossMonsterPattern::Attack;
			m_bBackDash = true;
			m_fAccelTime = 0.f;
		}
	}
}

void CJPreto::Death_Pattern(const _float & _fDeltaTime)
{
	if (m_pMesh->Get_Animationset() != (_uint)PretoAniID::Death)
	{
		m_pMesh->Set_Animationset((_uint)PretoAniID::Death);
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Death3D_0);
		GET_MANAGEMENT->Play3DSounds(L"Preto_Death.ogg", CSound_Manager::ESoundID::Monster_Death3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f);
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::BGM);
		GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Death3D_1);
		GET_MANAGEMENT->PlaySounds(L"Boss_Dead.mp3", CSound_Manager::ESoundID::Monster_Death3D_1);
	}
	if (::CoolTime(_fDeltaTime, m_fDeadPatilceEffectTime, 0.1f))
	{
		Get_Manager()->Generate_ItemParticle(
			m_iSceneIdx, m_pTransform->Get_TransformDesc().vPos,6.f,10
			,_float4(1.f,1.f,1.f,0.2f)
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp)
			,Mersen_ToInt(3,5)
		);

		DeadParticle();

		m_fDeadPatilceEffectTime = 0.f;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Death)
	{
		m_bTargetLook = false;
		m_fAddAniPlayTime = .7f;
		if (m_pMesh->Get_AnimationFrame(140, 55) && !m_pMesh->Get_AnimationFrame(140, 63))
		{
			m_bUpdatePhysx = false;
			m_pTransform->Go_Up(-5.f, _fDeltaTime);
		}
		m_pTransform->Update_Transform();
		m_pTransform->Set_Transform();

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_fDissolveTime += _fDeltaTime *pow(1.1f, m_fDissolveTime);
			if(m_fDissolveTime > 4.f)
			{
				m_eCurTotalPattern = eBossMonsterPattern::Sleep;
				Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_DEAD_BOSS_PRETO);
			}
			m_fAddAniPlayTime = 0.f;
			if (m_iGold > 0)
			{
				dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"))->Add_Souls(m_iGold);

				FAILED_CHECK_RETURN(Item_Drop(), );

				m_iGold = 0;
			}
		}
	}
}

void CJPreto::Reset_Pattern(const _float & _fDeltaTime)
{
	if (m_ePattern != PATTERN::Reset)
	{
		m_ePattern = PATTERN::Reset;
	}
	if (m_pMesh->Get_Animationset() != (_uint)PretoAniID::Walk_Forward)
	{
		m_pMesh->Set_Animationset((_uint)PretoAniID::Walk_Forward);
	}
	if (m_pMesh->Get_Animationset() == (_uint)PretoAniID::Walk_Forward)
	{
		m_bTargetLook = false;
		Back_To_Look(_fDeltaTime, m_vDropFieldPos);

		_float3 vPos = GET_MANAGEMENT->Move(m_tTransformDesc.szFileTitle, m_vLook, m_fSkillMoveSpeed, _fDeltaTime);
		vPos.y -= 1.f;
		m_pTransform->Set_Pos(vPos);

		if (m_pMesh->Get_AnimationFrame(77, 72))
		{
			if (m_bLeftLeg)
			{
				Move_Sound();
				m_bLeftLeg = false;
				m_bRightLeg = true;
			}
		}
		else if (m_pMesh->Get_AnimationFrame(77, 36))
		{
			if (m_bRightLeg)
			{
				Move_Sound();

				m_bRightLeg = false;
				m_bLeftLeg = true;
			}
		}
		_float fLenght = D3DXVec3Length(&(m_vDropFieldPos - m_pTransform->Get_TransformDesc().vPos));
		if (fLenght < 16.f && m_bResetPreto)
		{
			m_eCurTotalPattern = eBossMonsterPattern::Idle;
			m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);
			m_ePattern = PATTERN::Pattern_Ready;
			m_fAddAniPlayTime = 1.f;
			m_fWalkMoveTime = 0.f;
		}
	}
}

void CJPreto::Set_Hit()
{
	if (m_bHit)
	{
		if (m_tState.fHP > 0)
		{
			m_bHit = false;
			m_tState.fHP -= m_fHitDamage;			
		}
	}

	if (m_tState.fHP <= 0)
	{
		if (PATTERN::Death != m_ePattern)
		{
			m_ePattern = PATTERN::Death;
			m_pMesh->Set_Animationset((_uint)PretoAniID::Death);
		}		
	}		
}

_float3 CJPreto::AppearLook()
{
	_float3 vPos, vStagePos, vLook;
	vStagePos = { -500.788330f, 0.f, 21.3792229f };
	m_pTransform->Get_Info(EInfo::Pos, &vPos);
	vPos.y = 0.f;
	vLook = vStagePos - vPos;
	vLook *= -1;
	D3DXVec3Normalize(&vLook, &vLook);

	return vLook;
}

void CJPreto::AttackEndToMove_ReadyIdle()
{
	//m_bDelayMove = true;
	
	m_eCurTotalPattern = eBossMonsterPattern::Idle;
	m_bAttackCoolTime = true;
	m_bTargetLook = true;
	m_ePattern = PATTERN::Pattern_Ready;
	m_pMesh->Set_Animationset((_uint)PretoAniID::Idle);
	m_fAccelTime = 0.f;
}

HRESULT CJPreto::Clone_Trail()
{

	// 어떤무기인지에 따라 트레일 크기 및 색상 변경하십시오.
	//TRAIL_DESC tTrailDesc;
	//tTrailDesc.pObj = this;
	//lstrcpy(tTrailDesc.szTransformComponentName, L"Com_Transform");
	//tTrailDesc.fTrailWidth = 300.f;
	//tTrailDesc.fTrailChekRate = 0.005f;

	////202 119 6
	////255 78 11
	//tTrailDesc.vColorRGB = _float3(1.f, 78.f/255.f, 11.f/255.f);
	//tTrailDesc.bRenderOnOff = false;

	//_float4x4 matWorld;
	//D3DXMatrixIdentity(&matWorld);
	//tTrailDesc.iRenderID = 5;
	//D3DXMatrixRotationX(&matWorld, D3DXToRadian(90.f));
	//memcpy(&matWorld.m[3][0], _float3(10, 100.f, -10.f), sizeof(_float3));

	//tTrailDesc.matLocal = matWorld;

	//FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject((_uint)ESceneType::Static,
	//	L"GameObject_TrailEffect", (CGameObject**)&m_pTrailEffect, &tTrailDesc), E_FAIL);
	//FAILED_CHECK_RETURN(Get_Manager()->Clone_GameObject_ToLayer(m_pTrailEffect, m_iSceneIdx, L"Layer_Effect"), E_FAIL);
	//Safe_AddRef(m_pTrailEffect);

	wstring wstrMonsterName = m_tTransformDesc.szFileTitle;

	_uint count = (_uint)wstrMonsterName.find(L"_", 0);
	wstring wstrinteger = wstrMonsterName;
	wstrinteger.erase(0, count + 1);
	wstrinteger += L"_Weapon";

	TRANSFORM_DESC tTransformDesc;

	tTransformDesc.vRotate = _float3(0.f, D3DXToRadian(90.f), 0.f);
	//lstrcpy(tTransformDesc.szFileTitle, m_tTransformDesc.szFileTitle);
	//lstrcat(tTransformDesc.szFileTitle, L"_LegionnaireSword_");
	//lstrcat(tTransformDesc.szFileTitle, wstrinteger.c_str());
	if (FAILED(Get_Manager()->Clone_GameObject(
		(_uint)ESceneType::Static,
		L"GameObject_PretoSickle",
		(CGameObject**)&m_pWeapon,
		&tTransformDesc,
		m_iSceneIdx)))
	{
		ERROR_BOX(L"pManagement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	if (FAILED(Get_Manager()->Clone_GameObject_ToLayer(m_pWeapon, m_iSceneIdx, L"Layer_Weapon")))
	{
		ERROR_BOX(L"pManag6ement is Clone_GameObject_ToLayer");
		return E_FAIL;
	}
	m_pWeapon->Set_ParentClass(this, CJBaseWeapon::eMonsterParent::Boss);
	Safe_AddRef(m_pWeapon);
	return S_OK;
}

void CJPreto::Move_Sound()
{
	_uint iCount = Mersen_ToInt(0, 5);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Preto_Walking%d.ogg", iCount);
	GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_Move3D_0);
	GET_MANAGEMENT->Play3DSounds(szFileName, CSound_Manager::ESoundID::Monster_Move3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 75.f,false,1.f);
}

void CJPreto::Get_Damaged()
{

	_uint iCount = Mersen_ToInt(0, 2);
	TCHAR szFileName[256] = L"";
	wsprintf(szFileName, L"Preto_Damage%d.wav", iCount);
	GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Monster_GetHit3D_0);
	GET_MANAGEMENT->Play3DSounds(szFileName, CSound_Manager::ESoundID::Monster_GetHit3D_0, m_pTransform->Get_TransformDesc().vPos, 0.f, 50.f, false, 1.f);

}

CJPreto * CJPreto::Create(_DEVICE _pDevice, const wstring& _PrototypeName)
{
	CJPreto* pInstance = new CJPreto(_pDevice, _PrototypeName);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CJPreto::Clone(const _uint & _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CJPreto* pClone = new CJPreto(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CJPreto::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pShield);
		Safe_Release(m_pTrailEffect);
	}
	CJBaseBoss::Free();
}
