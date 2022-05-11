#include "stdafx.h"
#include "Player.h"
#include "MainCamera.h"
#include "Player_Weapon.h"
#include "UI_Inventory.h"
#include "UI_Equipment.h"
#include "Player_UI.h"
#include "AllMapObject.h"
#include "CinematicCamera_0.h"
#include "JPreto.h"
#include "MapDynamicObject_Lever.h"
#include "MapStaticObject_Elevator.h"
#include "MapStaticObject_PortalGate.h"
#include "JBaseMonster.h"

void CPlayer::Light_Attack(const _float & _fDeltaTime)
{
	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSSTART)
		return;

	auto pManagement = GET_MANAGEMENT;
	_float3 vLook;
	m_pTransform->Get_Info(EInfo::Look, &vLook);
	D3DXVec3Normalize(&vLook, &vLook);
	CPlayer_Weapon* pWeapon = dynamic_cast<CPlayer_Weapon*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player_Weapon", EFindGameObject::Proto, L"Player_Weapon"));
	// 현재 들고있는 무기를 확인
	if (m_pInterfaceUI->Get_QucikEquipInfo() != nullptr)
	{
		if (m_pInterfaceUI->Get_QucikEquipInfo()->ubyItem_ID == (_ubyte)eWeaponID::Weapon_1)
			m_eWeapon = NORMAL;
		else if (m_pInterfaceUI->Get_QucikEquipInfo()->ubyItem_ID == (_ubyte)eWeaponID::Weapon_2)
			m_eWeapon = MOON;
		else
			return;
	}
	////
	if (m_bAttackMove)
		m_pTransform->Set_Pos(pManagement->Move(L"Player", vLook, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));

	//차징공격(기본 무기 한정)
	if (pWeapon->Get_WeaponStat().iNormalLevel >= 5.f)
	{
		if (pManagement->KeyPressing(EMouseKey::LB) && pWeapon->Get_Take_Normal() && !m_bStaminaCool)
		{
			if (m_eAttack != ATTACK1 && m_eAttack != ATTACK2 && m_eAttack != ATTACK3)
			{
				m_fAttackTime += _fDeltaTime;
				m_bCharging = true;
				m_eAttack = CHARGE;
				m_bAttackMove = false;
				if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_CHARGING_ATTACK && !m_pMesh->Get_AnimationFrame(92, 33))
					m_bChargingDir = true;
			}
		}
		if (!pManagement->KeyPressing(EMouseKey::LB) && !m_bStaminaCool)
		{
			m_fAttackTime = 0.f;
			m_bCharging = false;
			if (m_eAttack == CHARGE)
				m_eAttack = NO_ATTACK;
			m_bAttackMove = false;
		}
	}

	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_CHARGING_ATTACK && m_pMesh->Get_AnimationFrame(92, 33))
	{
		ChargingEffect();
		
		m_bAttackMove = false;
		m_bChargeAttack = true;
		m_bCharging = false;
		/*if(!m_pMesh->Get_AnimationFrame(92, 34))
			m_tState.fStamina -= 5.f;*/
		DashTrailSwitch(true);
		DashTrailColor_Set(_float3(0.75f, 0.75f, 0.75f));
		DashTrailWidth_Set(10.f);
		DashTrailRenderId_Set(2);

		
		if (!m_pMesh->Get_AnimationFrame(92, 58))
		{
			if (m_bChargingDir)
			{
				m_bChargingDir = false;
			}
			m_pTransform->Set_Pos(pManagement->Move(L"Player", vLook, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));
			m_tState.fStamina -= _fDeltaTime * 10.f;

			//m_pTransform->Set_Pos()
		}

	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_CHARGING_ATTACK && m_pMesh->Is_AnimationSetEnd())
	{
		DashTrailSwitch(false);
		m_bChargeAttack = false;
		m_bCharging = false;
		m_bAttackMove = false;
	}
	////


	if (pManagement->KeyUp(EMouseKey::LB) && m_fAttackTime == 0.f && !m_bChargeAttack && !m_bStaminaCool)
	{
		if (m_eAttack != HEAVY1 && m_eAttack != HEAVY2 && m_eAttack != HEAVY3 && m_eAttack != PARRY)
		{
			if (m_eAttack != NO_ATTACK)
				m_iLightAttack += 1;
			if (m_eAttack == NO_ATTACK && (pWeapon->Get_Take_Normal() || pWeapon->Get_Take_Moon()))
			{
				m_tState.fStamina -= 10.f;
				m_iLightAttack = 0;
				m_bAttack = true;
				m_eAttack = ATTACK1;
			}
		}
	}
	// 공격 애니메이션
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK1 && m_pMesh->Get_AnimationFrame(44,30))
	{
		if (m_iLightAttack > 0)
		{
			m_tState.fStamina -= 10.f;
			m_iLightAttack = 0;
			m_eAttack = ATTACK2;
		}
		else if (m_iLightAttack <= 0)
		{
			m_iLightAttack = 0;
			m_bAttackMove = false;
			//m_eAttack = NO_ATTACK;
		}
		if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK1 && m_pMesh->Is_AnimationSetEnd())
		{
			m_iLightAttack = 0;
			m_eAttack = NO_ATTACK;
			m_bAttackMove = false;
			m_bAttack = false;
		}
	}

	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK2 && m_pMesh->Is_AnimationSetEnd())
	{
		if (m_iLightAttack > 0)
		{
			m_tState.fStamina -= 10.f;
			m_iLightAttack = 0;
			m_bAttackMove = false;
			m_eAttack = ATTACK3;
		}
		else if (m_iLightAttack <= 0)
		{
			m_iLightAttack = 0;
			m_eAttack = NO_ATTACK;
			m_bAttack = false;
			m_bAttackMove = false;
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_WEAPON_ATTACK3 && m_pMesh->Is_AnimationSetEnd())
	{
		if (m_iLightAttack > 0)
		{
			m_tState.fStamina -= 10.f;
			m_iLightAttack = 0;
			m_bAttackMove = false;
			m_eAttack = ATTACK1;
		}
		else if (m_iLightAttack <= 0)
		{
			m_iLightAttack = 0;
			m_eAttack = NO_ATTACK;
			m_bAttack = false;
			m_bAttackMove = false;
		}
	
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::MOON_ATTACK1 && m_pMesh->Get_AnimationFrame(26, 16))
	{
		if (m_iLightAttack > 0)
		{
			m_tState.fStamina -= 10.f;
			m_iLightAttack = 0;
			m_eAttack = ATTACK2;
			m_bAttackMove = false;
		}
		else if (m_iLightAttack <= 0)
		{
			m_iLightAttack = 0;
			m_bAttackMove = false;
			//m_eAttack = NO_ATTACK;
		}
		if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::MOON_ATTACK1 && m_pMesh->Is_AnimationSetEnd())
		{
			m_iLightAttack = 0;
			m_eAttack = NO_ATTACK;
			m_bAttack = false;
			m_bAttackMove = false;
		}


		/*if (m_iLightAttack > 0)
		{
			m_tState.fStamina -= 5.f;
			m_iLightAttack = 0;
			m_eAttack = ATTACK2;
		}
		else if (m_iLightAttack <= 0)
		{
			m_iLightAttack = 0;
			m_eAttack = NO_ATTACK;
			m_bAttack = false;
		}*/
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::MOON_ATTACK2 && m_pMesh->Is_AnimationSetEnd())
	{
		if (m_iLightAttack > 0)
		{
			m_tState.fStamina -= 10.f;
			m_iLightAttack = 0;
			m_eAttack = ATTACK1;
			m_bAttackMove = false;
		}
		else if (m_iLightAttack <= 0)
		{
			m_iLightAttack = 0;
			m_eAttack = NO_ATTACK;
			m_bAttack = false;
			m_bAttackMove = false;
		}
	}
	/*if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::KATANA_ATTACK3 && m_pMesh->Is_AnimationSetEnd())
	{
		m_iLightAttack = 0;
		m_eAttack = NO_ATTACK;
		m_bAttack = false;
	}*/

	Check_Attack(_fDeltaTime);
}

void CPlayer::Heavy_Attack(const _float & _fDeltaTime)
{
	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSSTART)
		return;

	auto pManagement = GET_MANAGEMENT;
	_float3 vLook;
	m_pTransform->Get_Info(EInfo::Look, &vLook);
	D3DXVec3Normalize(&vLook, &vLook);

	if (m_pInterfaceUI->Get_QucikEquipInfo() != nullptr)
	{
		if (m_pInterfaceUI->Get_QucikEquipInfo()->ubyItem_ID == (_ubyte)eWeaponID::Weapon_1)
			m_eWeapon = NORMAL;
		else if (m_pInterfaceUI->Get_QucikEquipInfo()->ubyItem_ID == (_ubyte)eWeaponID::Weapon_2)
			m_eWeapon = MOON;
	}
	if (pManagement->KeyUp(EMouseKey::RB) && m_tState.fStamina > 0.f && !m_bStaminaCool)
	{
		if (m_eAttack != ATTACK1 && m_eAttack != ATTACK2 && m_eAttack != ATTACK3 && m_eAttack != PARRY)
		{
			CPlayer_Weapon* pWeapon = dynamic_cast<CPlayer_Weapon*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player_Weapon", EFindGameObject::Proto, L"Player_Weapon"));
			if (m_eAttack != NO_ATTACK && m_eAttack != PARRY)
				m_iHeavyAttack += 1;
			if (m_eAttack == NO_ATTACK && (pWeapon->Get_Take_Normal()))
			{
				m_tState.fStamina -= 15.f;
				m_iHeavyAttack = 0;
				m_bAttack = true;
				m_eAttack = HEAVY1;
			}
			if (m_eAttack == NO_ATTACK &&m_eWeapon == MOON)
			{
				m_tState.fStamina -= 7.f;
				m_iHeavyAttack = 0;
				m_bParry = true;
				m_eAttack = PARRY;
			}
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK1 && m_pMesh->Is_AnimationSetEnd())
	{
		dynamic_cast<CMainCamera*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Camera"))->CamShakerHeavy();
		m_pTransform->Move_Pos(&-vLook, 15.f, _fDeltaTime);
		if (m_iHeavyAttack > 0)
		{
			m_tState.fStamina -= 15.f;
			m_iHeavyAttack = 0;
			m_eAttack = HEAVY2;
		}
		else if (m_iHeavyAttack <= 0)
		{
			m_iHeavyAttack = 0;
			m_eAttack = NO_ATTACK;
			m_bAttack = false;
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK2 && m_pMesh->Get_AnimationFrame(44, 27))
	{
		if (m_bHeavyShake)
		{
			m_bHeavyShake = false;
			dynamic_cast<CMainCamera*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Camera"))->CamShakerHeavy();
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::NORMAL_HEAVY_ATTACK2 && m_pMesh->Is_AnimationSetEnd())
	{
		m_bHeavyShake = true;
		if (m_iHeavyAttack > 0)
		{
			m_tState.fStamina -= 10.f;
			m_iHeavyAttack = 0;
			m_eAttack = HEAVY1;
		}
		else if (m_iHeavyAttack <= 0)
		{
			m_iHeavyAttack = 0;
			m_eAttack = NO_ATTACK;
			m_bAttack = false;
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::PARRY && m_pMesh->Is_AnimationSetEnd())
	{
		m_bParry = false;
		m_eAttack = NO_ATTACK;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::PARRY)
	{
		if(m_pMesh->Get_AnimationFrame(40, 15))
			Delete_Parry();
		else if(m_pMesh->Get_AnimationFrame(40, 5))
			Create_Parry();
	}
	Check_Attack(_fDeltaTime);
}

void CPlayer::Check_Attack(const _float & _fDeltaTime)
{
	switch (m_eAttack)
	{
	case CPlayer::ATTACK1:
		if (m_eWeapon == NORMAL)
			m_eCurState = NORMAL_ATTACK1;
		else if (m_eWeapon == MOON)
			m_eCurState = KATANA_ATTACK1;
		break;
	case CPlayer::ATTACK2:
		if (m_eWeapon == NORMAL)
			m_eCurState = NORMAL_ATTACK2;
		else if (m_eWeapon == MOON)
			m_eCurState = KATANA_ATTACK2;
		break;
	case CPlayer::ATTACK3:
		if (m_eWeapon == NORMAL)
			m_eCurState = NORMAL_ATTACK3;
	/*	else if (m_eWeapon == KATANA)
			m_eCurState = KATANA_ATTACK3;*/
		break;
	case CPlayer::HEAVY1:
		if (m_eWeapon == NORMAL)
			m_eCurState = NORMAL_HEAVY_ATTACK1;
		break;
	case CPlayer::HEAVY2:
		if (m_eWeapon == NORMAL)
			m_eCurState = NORMAL_HEAVY_ATTACK2;
		break;
	case CPlayer::HEAVY3:
		if (m_eWeapon == NORMAL)
			m_eCurState = NORMAL_HEAVY_ATTACK2;
		break;
	case CPlayer::CHARGE:
		if (m_eWeapon == NORMAL)
			m_eCurState = NORMAL_CHARGING;
		else if (m_eWeapon == MOON)
			m_eCurState = KATANA_ATTACK1;
		break;
	case CPlayer::PARRY:
			m_eCurState = MOON_PARRY;
		break;
	case CPlayer::NO_ATTACK:
		m_bAttack = false;
		break;
	}
}

void CPlayer::Targeting()
{
	auto pManagement = GET_MANAGEMENT;

	if (!m_bTargeting)
	{
		
		list<CGameObject*>* listObj = pManagement->Get_LayerContainer(m_iSceneIdx, L"Layer_Monster");
		_float	fLowLength = 999.f;
		if (!listObj)
			return;
		for (auto& iter : *listObj)
		{
			CTransform* pTransformComponent = dynamic_cast<CTransform*>(iter->Get_Component(L"Com_Transform"));
			_float3 vPos = (pTransformComponent->Get_TransformDesc().vPos - m_pTransform->Get_TransformDesc().vPos);
			_float fLength = D3DXVec3Length(&vPos);

			if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_MEETBOSS)
			{
				CJBaseBoss* pMonster = dynamic_cast<CJBaseBoss*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, pTransformComponent->Get_TransformDesc().szFileTitle));
				if(pMonster == nullptr)
					continue;
				if (pMonster->Get_MonsterPattern() == eBossMonsterPattern::Awake
					|| pMonster->Get_MonsterPattern() == eBossMonsterPattern::Dead
					|| pMonster->Get_MonsterPattern() == eBossMonsterPattern::Other
					|| pMonster->Get_MonsterPattern() == eBossMonsterPattern::Sleep)
					continue;
			}
			else
			{
				CJBaseMonster* pMonster = dynamic_cast<CJBaseMonster*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, pTransformComponent->Get_TransformDesc().szFileTitle));
				if (pMonster == nullptr)
					continue;
				if (pMonster->Get_MonsterPattern() == eMonsterPattern::Awake
					|| pMonster->Get_MonsterPattern() == eMonsterPattern::Dead
					|| pMonster->Get_MonsterPattern() == eMonsterPattern::Other
					|| pMonster->Get_MonsterPattern() == eMonsterPattern::Sleep)
					continue;
			}


			if (fLength < 25.f)
			{
				_float3 vMonsterPos, vUp, vLook, vRight;
				vMonsterPos = pTransformComponent->Get_TransformDesc().vPos;
				vUp = { 0.f,1.f,0.f };
				m_pTransform->Get_Info(EInfo::Right, &vRight);
				D3DXVec3Cross(&vRight, &vLook, &vUp);
				D3DXVec3Normalize(&vRight, &vRight);

				vLook = vMonsterPos - m_pTransform->Get_TransformDesc().vPos;
				vLook *= -1.f;
				D3DXVec3Normalize(&vLook, &vLook);

				_float3 vPlayerLook;
				memcpy(&vPlayerLook, &m_pTransform->Get_TransformDesc().matWorld.m[2][0], sizeof(_float3));
				D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

				_float3 vPlayerRight;
				memcpy(&vPlayerRight, &m_pTransform->Get_TransformDesc().matWorld.m[0][0], sizeof(_float3));
				D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

				_float fDot = D3DXVec3Dot(&vPlayerLook, &vLook);
				_float fRight = D3DXVec3Dot(&vPlayerRight, &vLook);

				_float Radian = acosf(fDot);

				if (Radian != Radian)
					continue;

				if (fDot > 0)
				{
					if (fLowLength > fLength)
					{
						fLowLength = fLength;
						m_szTargetName = iter->Get_TransformDesc().szFileTitle;
					}
					//if (fRight > 0) // 오른쪽 앞
					//{

					//}
					//else			// 왼쪽 앞
					//{

					//}
				}
				
				//_float3 vLook;
				//m_pTransform->Get_Info(EInfo::Look, &vLook);
				//
				//_float fDir = acos(D3DXVec3Dot(&vLook, &pTransformComponent->Get_TransformDesc().vPos)) * (180.f / D3DX_PI);

				//if (fDir != fDir)
				//	return;

				//if (fDir > 180.f)
				//{
				//	_float fRemainder = fDir - 180.f;
				//	fDir *= -1.f;
				//	fDir += fRemainder * 2.f;
				//}
				//if (fDir < 90.f && fDir > -90.f)
				//{
				//	liststring.push_back(pTransformComponent->Get_TransformDesc().szFileTitle);
				//}
			}
		}
		if (fLowLength == 999.f)
		{
			m_szTargetName = L"";
			fLowLength = 999.f;
			m_vTargetPos = { 0.f,0.f,0.f };
			m_bTargeting = false;
		}
		else
		{
			m_bTargeting = true;
		}
	}
	else if (m_bTargeting)
	{
		m_szTargetName = L"";
		m_bTargeting = false;
	}
}

void CPlayer::OnTarget(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	list<CGameObject*>* listObj = pManagement->Get_LayerContainer(m_iSceneIdx, L"Layer_Monster");
	if (!listObj)
		return;
	for (auto& iter : *listObj)
	{
		CTransform* pTransformComponent = dynamic_cast<CTransform*>(iter->Get_Component(L"Com_Transform"));
		if (pTransformComponent->Get_TransformDesc().szFileTitle == m_szTargetName)
		{
			if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_MEETBOSS)
			{
				CJBaseBoss* pMonster = dynamic_cast<CJBaseBoss*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, pTransformComponent->Get_TransformDesc().szFileTitle));
				if (pMonster == nullptr)
					break;
				if (pMonster->Get_MonsterPattern() == eBossMonsterPattern::Awake
					|| pMonster->Get_MonsterPattern() == eBossMonsterPattern::Dead
					|| pMonster->Get_MonsterPattern() == eBossMonsterPattern::Other
					|| pMonster->Get_MonsterPattern() == eBossMonsterPattern::Sleep)
				{
					m_szTargetName = L"";
					m_vTargetPos = { 0.f,0.f,0.f };
					
					m_bTargeting = false;
					break;
				}
			}
			else
			{
				CJBaseMonster* pMonster = dynamic_cast<CJBaseMonster*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, pTransformComponent->Get_TransformDesc().szFileTitle));
				if (pMonster == nullptr)
					break;
				if (pMonster->Get_MonsterPattern() == eMonsterPattern::Awake
					|| pMonster->Get_MonsterPattern() == eMonsterPattern::Dead
					|| pMonster->Get_MonsterPattern() == eMonsterPattern::Other
					|| pMonster->Get_MonsterPattern() == eMonsterPattern::Sleep)
				{
					m_szTargetName = L"";
					m_vTargetPos = { 0.f,0.f,0.f };
					m_bTargeting = false;
					break;
				}
			}
			m_szTargetName = pTransformComponent->Get_TransformDesc().szFileTitle;

			_float3 vMonsterPos, vUp, vLook, vRight;
			vMonsterPos = pTransformComponent->Get_TransformDesc().vPos;
			m_vTargetPos = pTransformComponent->Get_TransformDesc().vPos;


			vUp = { 0.f,1.f,0.f };
			vLook = { 0.f,0.f,0.f };
			m_pTransform->Get_Info(EInfo::Right, &vRight);
			D3DXVec3Cross(&vRight, &vLook, &vUp);
			D3DXVec3Normalize(&vRight, &vRight);

			vLook = vMonsterPos - m_pTransform->Get_TransformDesc().vPos;
			vLook.y = 0.f;
			D3DXVec3Normalize(&vLook, &vLook);
			vLook *= -1.f;
			_float3 vPlayerLook;
			memcpy(&vPlayerLook, &m_pTransform->Get_TransformDesc().matWorld.m[2][0], sizeof(_float3));
			D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

			_float3 vPlayerRight;
			memcpy(&vPlayerRight, &m_pTransform->Get_TransformDesc().matWorld.m[0][0], sizeof(_float3));
			D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

			_float fDot = D3DXVec3Dot(&vPlayerLook, &vLook);
			_float fRight = D3DXVec3Dot(&vPlayerRight, &vLook);

			_float Radian = acosf(fDot);

			if (Radian != Radian)
				return;


			CMainCamera* pCamera = dynamic_cast<CMainCamera*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));


			if (fDot < 0)
			{
				if (fRight > 0) // 왼쪽 뒤
				{
					m_pTransform->Rotate(EAxis::Y, Radian, m_pTransform->Get_TransformDesc().fRotatePerSecond, _fDeltaTime);
					pCamera->Set_CameraAngleY(Radian* m_pTransform->Get_TransformDesc().fRotatePerSecond*_fDeltaTime);
				}
				else			// 오른쪽 뒤
				{
					m_pTransform->Rotate(EAxis::Y, Radian, m_pTransform->Get_TransformDesc().fRotatePerSecond, -_fDeltaTime);
					pCamera->Set_CameraAngleY(Radian* m_pTransform->Get_TransformDesc().fRotatePerSecond*-_fDeltaTime);
				}
			}
			else
			{
				if (fRight > 0) // 오른쪽 앞
				{
					m_pTransform->Rotate(EAxis::Y, Radian, m_pTransform->Get_TransformDesc().fRotatePerSecond, _fDeltaTime);
					pCamera->Set_CameraAngleY(Radian* m_pTransform->Get_TransformDesc().fRotatePerSecond*_fDeltaTime);
				}
				else			// 왼쪽 앞
				{
					m_pTransform->Rotate(EAxis::Y, Radian, m_pTransform->Get_TransformDesc().fRotatePerSecond, -_fDeltaTime);
					pCamera->Set_CameraAngleY(Radian* m_pTransform->Get_TransformDesc().fRotatePerSecond*-_fDeltaTime);
				}
			}
			return;
		}
	}
}

void CPlayer::Use_QuickItem()
{
	auto pManagement = GET_MANAGEMENT;
	if (m_pInterfaceUI->Get_QucikItemInfo() != nullptr)
	{
		if (m_eItem != ITEM::ITEM_NONE)
		{
			if (m_bHeal && m_eItem == ITEM::HEALTH)
			{
				if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Heal_Small)
				{
					m_fHealing += 1.f;
					m_fHealTime += 5.f;
				}
				else if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Heal_Medium)
				{
					m_fHealing += 3.f;
					m_fHealTime += 5.f;
				}
				else if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Heal_Large)
				{
					m_fHealing += 5.f;
					m_fHealTime += 5.f;
				}
				else if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Heal_Full)
				{
					m_fHealing += 10.f;
					m_fHealTime += 7.f;
				}
				if (m_pInterfaceUI->Use_Item())
				{
					_float3 vTargetPos = m_pTransform->Get_TransformDesc().vPos;
					const MY_FRAME*  pFrame = m_pMesh->Get_FrameByName("Bip001-L-Hand");
					_float4x4 matCreatePos = pFrame->CombinedTranformationMatrix;
					matCreatePos = matCreatePos*(*m_pTransform->Get_WorldMatrix());

					_float3 vCreatePos;

					memcpy(vCreatePos, &matCreatePos.m[3][0], sizeof(_float3));
					CManagement::Get_Instance()->Generate_ExplosionParticle(
						m_iSceneIdx,
						vCreatePos,
						1.f,
						&vTargetPos,
						50,
						_float4(1.f,0.648305f,0.92549f,1.f),
						//_ARGB(255, 153, 236, 255),
						//ePartilcePatternType::OnStartBlinkAndGoStraight
						(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraight)
					);
					m_bHeal = false;
					GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player);
					GET_MANAGEMENT->PlaySounds(L"Player_Use_Heal.ogg", CSound_Manager::ESoundID::Player);
				}
			}
			else if (m_bUse && m_eItem == ITEM::SOUL)
			{
				if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Small_1)
				{
					m_pInterfaceUI->Add_Souls(50);
				}
				else if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Small_2)
				{
					m_pInterfaceUI->Add_Souls(200);
				}
				else if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Medium)
				{
					m_pInterfaceUI->Add_Souls(1000);
				}
				else if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Preto)
				{
					m_pInterfaceUI->Add_Souls(3000);
				}
				else if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Mal)
				{
					m_pInterfaceUI->Add_Souls(5000);
				}
				if (m_pInterfaceUI->Use_Item())
				{
					_float3 vTargetPos = m_pTransform->Get_TransformDesc().vPos;
					const MY_FRAME*  pFrame = m_pMesh->Get_FrameByName("Bip001-L-Hand");
					_float4x4 matCreatePos = pFrame->CombinedTranformationMatrix;
					matCreatePos = matCreatePos*(*m_pTransform->Get_WorldMatrix());

					_float3 vCreatePos;

					memcpy(vCreatePos, &matCreatePos.m[3][0], sizeof(_float3));
					CManagement::Get_Instance()->Generate_ExplosionParticle(
						m_iSceneIdx,
						vCreatePos,
						1.f,
						&vTargetPos,
						50,
						_float4(0.6f, 0.925490f, 1.f, 1.f),
						//ePartilcePatternType::OnStartBlinkAndGoStraight
						(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraight)
					);
					m_bUse = false;
					GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player);
					GET_MANAGEMENT->PlaySounds(L"Player_Use_Item.ogg", CSound_Manager::ESoundID::Player);
				}
			}
		}
	}
}

void CPlayer::Swap_Item()
{
	auto pManagement = GET_MANAGEMENT;

	if (m_pInterfaceUI->Get_QucikItemInfo() != nullptr)
	{
		if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Heal_Small ||
			m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Heal_Medium ||
			m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Heal_Large ||
			m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Heal_Full)
			m_eItem = ITEM::HEALTH;
		else if (m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Mal ||
			m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Medium ||
			m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Small_1 ||
			m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Small_2 ||
			m_pInterfaceUI->Get_QucikItemInfo()->ubyItem_ID == (_ubyte)eItemID::Soul_Preto)
			m_eItem = ITEM::SOUL;
		else
			m_eItem = ITEM::ITEM_NONE;
	}
	else
		m_eItem = ITEM::ITEM_NONE;
}

void CPlayer::Update_Item()
{
	if ((m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH && m_pMesh->Get_AnimationFrame(53, 27)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK && m_pMesh->Get_AnimationFrame(73, 27)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_TIDLE && m_pMesh->Get_AnimationFrame(49, 27)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT && m_pMesh->Get_AnimationFrame(60, 27)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK && m_pMesh->Get_AnimationFrame(60, 27)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT && m_pMesh->Get_AnimationFrame(60, 27)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT && m_pMesh->Get_AnimationFrame(60, 27)))
	{
		if (m_bUse_Quick_Item)
		{
			m_bUse_Quick_Item = false;
			Use_QuickItem();
		}
	}

	if ((m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM && m_pMesh->Get_AnimationFrame(68, 50)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK && m_pMesh->Get_AnimationFrame(72, 50)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USEITEM_TIDLE && m_pMesh->Get_AnimationFrame(98, 50)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT && m_pMesh->Get_AnimationFrame(90, 50)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK && m_pMesh->Get_AnimationFrame(90, 50)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT && m_pMesh->Get_AnimationFrame(90, 50)) ||
		(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT && m_pMesh->Get_AnimationFrame(90, 50)))
	{
		if (m_bUse_Quick_Item)
		{
			m_bUse_Quick_Item = false;
			Use_QuickItem();
		}
	}

	if ((m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_TIDLE ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT) &&
		m_pMesh->Is_AnimationSetEnd())
	{
		m_bHeal = true;
		m_bUse_Heal = false;
	}
	else if ((m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USEITEM_TIDLE ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT) &&
		m_pMesh->Is_AnimationSetEnd())
	{
		m_bUse = true;
		m_bUse_Item = false;
	}
}

void CPlayer::Wake_Up(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	/*if ((_uint)ESceneType::Shrine == m_iSceneIdx || (_uint)ESceneType::Stage2 == m_iSceneIdx)
	{
		m_bStart = false;
		m_fStartTime = 0.f;
		m_eCurState = IDLE_PEACE;
		m_pInterfaceUI->Set_ViewInterface(true);
	}
	else
	{*/

		/*CCinematicCamera_0* pCamera = dynamic_cast<CCinematicCamera_0*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
>>>>>>> origin/develop
		m_fStartTime += _fDeltaTime;
		m_pTransform->Set_Pos((pManagement->Set_GlobalPos(L"Player", m_vSpawnPos)));*/

		/*if (pCamera->Get_CineEnd() && m_fStartTime >= 5.f)
		{
			m_eCurState = WAKE_UP;
			m_pInterfaceUI->Set_ViewInterface(true);
		}*/


		if ((Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_AWAKEPLAYER))
		{
			m_fStartTime += _fDeltaTime;
			if (m_fStartTime >= 5.f)
			{
				if ((_uint)ESceneType::Tutorial == m_iSceneIdx && Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_GET_WEAPON)
				{
					pManagement->StopSound(CSound_Manager::ESoundID::BGM);
					pManagement->PlayBGM(L"Tutorial.ogg", BGM_VOLUME);
				}
				else if ((_uint)ESceneType::Shrine == m_iSceneIdx)
				{
					pManagement->StopSound(CSound_Manager::ESoundID::BGM);
					pManagement->PlayBGM(L"Shrine.ogg", BGM_VOLUME);
				}
				else if ((_uint)ESceneType::Stage2 == m_iSceneIdx)
				{
					pManagement->StopSound(CSound_Manager::ESoundID::BGM);
					pManagement->PlayBGM(L"Themes_Stage2.ogg", BGM_VOLUME);
				}
				m_bResetCamera = true;
				m_eCurState = WAKE_UP;
				m_pInterfaceUI->Set_ViewInterface(true);
			}
		}
		if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::WAKE_UP && m_pMesh->Is_AnimationSetEnd())
		{
			m_bResetCamera = false;
			m_bStart = false;
			m_fStartTime = 0.f;
			m_eCurState = IDLE_PEACE;
		}
	//}

	
	m_pMesh->Play_Animationset(_fDeltaTime * 0.75f);
}

void CPlayer::Weapon_Swap()
{
	auto pManagement = GET_MANAGEMENT;
	CPlayer_Weapon* pWeapon = dynamic_cast<CPlayer_Weapon*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player_Weapon", EFindGameObject::Proto, L"Player_Weapon"));
	if (pManagement->Key_Down(DIK_1))
	{
		Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
		Get_Manager()->PlaySounds(L"Environment_ItemSlotWeaponSelect.ogg", CSound_Manager::ESoundID::UI, 1.f);
		m_pInterfaceUI->Get_Equip()->SwapQuickSlot();
	}
	if (m_pInterfaceUI->Get_QucikEquipInfo() == nullptr)
	{
		pWeapon->Set_Take_Normal(false);
		pWeapon->Set_Take_Moon(false);
	}
	else if (m_pInterfaceUI->Get_QucikEquipInfo() != nullptr)
	{

		if (m_pInterfaceUI->Get_QucikEquipInfo()->ubyItem_ID == (_ubyte)eWeaponID::Weapon_1)
		{
			pWeapon->Set_Take_Normal(true);
			pWeapon->Set_Take_Moon(false);
		}
		else if (m_pInterfaceUI->Get_QucikEquipInfo()->ubyItem_ID == (_ubyte)eWeaponID::Weapon_2)
		{
			pWeapon->Set_Take_Normal(false);
			pWeapon->Set_Take_Moon(true);
		}
		
	}
}

void CPlayer::Update_Move(const _float & _fDeltaTime)
{
	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSSTART)
		return;

	auto pManagement = GET_MANAGEMENT;

	_bool bCineEnd=true;
	if((_uint)ESceneType::Tutorial == m_iSceneIdx)
	{
		CCinematicCamera_0* pCineCamera = dynamic_cast<CCinematicCamera_0*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
		bCineEnd = pCineCamera->Get_CineEnd();
	}

	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));

	Update_Camera_Dir(pCamera);
	if (!m_pInterfaceUI->Get_CheckViewInterface())
		return;
	// 이동 관련
	
	if (pManagement->Key_Pressing(DIK_W) && !m_bDead && bCineEnd)
	{
		
		if (!m_bRoll && !m_bDash)
		{
			if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bStaminaCool && !m_bJump && !m_bGravity && !m_bUse_Item && !m_bUse_Heal && !m_bTargeting && pManagement->Key_Pressing(DIK_LSHIFT) && (m_tState.fStamina > 0.f))
			{
				m_tState.fStamina -= 10.f * _fDeltaTime;
				m_eCurState = SPRINT;
				DashTrailRandomSwitch();
			}
			else
			{
				DashTrailSwitch(false);
				if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bTargeting)
				{
					if (m_bUse_Heal)
						m_eCurState = HEAL_WALK;
					else if (m_bUse_Item)
						m_eCurState = USE_WALK;
					else
					{
						if (!m_bJump && !m_bGravity)
							m_eCurState = RUN;
					}
				}
				else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && m_bTargeting)
				{
					if (m_bUse_Heal)
							m_eCurState = TWALK_HEAL_FRONT;
					else if (m_bUse_Item)
							m_eCurState = TWALK_USE_FRONT;
					else if (!m_bUse_Heal && !m_bUse_Item)
					{
						if (!m_bJump && !m_bGravity)
							m_eCurState = TWALK_FRONT;
					}
				}
			}
			if (pManagement->Key_Pressing(DIK_A))
			{
				if (!m_bCharging && !m_bChargeAttack && !m_bAttack)
				m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vLU, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));
				if (m_bRotate)
				Player_Rotate(DIR_LF, _fDeltaTime, pCamera);
			}
			else if (pManagement->Key_Pressing(DIK_D))
			{
				if (!m_bCharging && !m_bChargeAttack && !m_bAttack)
				m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vRU, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));
				if (m_bRotate)
				Player_Rotate(DIR_RF, _fDeltaTime, pCamera);
			}
			else
			{
				if (!m_bCharging && !m_bChargeAttack && !m_bAttack)
				m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vLook, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));
				if (m_bRotate)
				Player_Rotate(DIR_FRONT, _fDeltaTime, pCamera);
			}
		}
	}
	else if (pManagement->Key_Pressing(DIK_S) && !m_bDead && bCineEnd)
	{
		if (!m_bRoll && !m_bDash)
		{
			if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bStaminaCool && !m_bJump && !m_bGravity && !m_bUse_Item && !m_bUse_Heal && !m_bTargeting && pManagement->Key_Pressing(DIK_LSHIFT) && (m_tState.fStamina > 0.f))
			{
				m_tState.fStamina -= 10.f * _fDeltaTime;
				m_eCurState = SPRINT;
				DashTrailRandomSwitch();
			}
			else
			{
				DashTrailSwitch(false);
				if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bTargeting)
				{
					if (m_bUse_Heal)
						m_eCurState = HEAL_WALK;
					else if (m_bUse_Item)
						m_eCurState = USE_WALK;
					else
					{
						if (!m_bJump && !m_bGravity)
							m_eCurState = RUN;
					}
				}
				else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && m_bTargeting)
				{
					if (!m_bUse_Heal && !m_bUse_Item)
					{
						if (!m_bJump && !m_bGravity)
							m_eCurState = TWALK_BACK;
					}
					else if (m_bUse_Heal)
						m_eCurState = TWALK_HEAL_BACK;
					else if (m_bUse_Item)
						m_eCurState = TWALK_USE_BACK;
				}
			}
			if (pManagement->Key_Pressing(DIK_A))
			{
				if(!m_bCharging && !m_bChargeAttack && !m_bAttack)
					m_pTransform->Set_Pos(pManagement->Move(L"Player", -m_vRU, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));
				if(m_bRotate)
				Player_Rotate(DIR_LB, _fDeltaTime, pCamera);
			}
			else if (pManagement->Key_Pressing(DIK_D))
			{
				if (!m_bCharging && !m_bChargeAttack && !m_bAttack)
				m_pTransform->Set_Pos(pManagement->Move(L"Player", -m_vLU, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));
				if (m_bRotate)
				Player_Rotate(DIR_RB, _fDeltaTime, pCamera);
			}
			else
			{
				if (!m_bCharging && !m_bChargeAttack && !m_bAttack)
				m_pTransform->Set_Pos(pManagement->Move(L"Player", -m_vLook, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));
				if (m_bRotate)
				Player_Rotate(DIR_BACK, _fDeltaTime, pCamera);
			}
		}
	}
	else if (pManagement->Key_Pressing(DIK_A) && !m_bDead && bCineEnd)
	{
		if (!m_bRoll && !m_bDash)
		{
			if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bStaminaCool && !m_bJump && !m_bGravity && !m_bUse_Item && !m_bUse_Heal && !m_bTargeting && pManagement->Key_Pressing(DIK_LSHIFT) && (m_tState.fStamina > 0.f))
			{
				m_tState.fStamina -= 10.f * _fDeltaTime;
				m_eCurState = SPRINT;
				DashTrailRandomSwitch();
			}
			else
			{
				DashTrailSwitch(false);
				if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bTargeting)
				{
					if (m_bUse_Heal)
						m_eCurState = HEAL_WALK;
					else if (m_bUse_Item)
						m_eCurState = USE_WALK;
					else
					{
						if (!m_bJump && !m_bGravity)
							m_eCurState = RUN;
					}
				}
				else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && m_bTargeting)
				{
					if (!m_bUse_Heal && !m_bUse_Item)
					{
						if (!m_bJump && !m_bGravity)
							m_eCurState = TWALK_LEFT;
					}
					else if (m_bUse_Heal)
						m_eCurState = TWALK_HEAL_LEFT;
					else if (m_bUse_Item)
						m_eCurState = TWALK_USE_LEFT;
				}
			}
			if (!m_bCharging && !m_bChargeAttack && !m_bAttack)
			m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vRight, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));
			if (m_bRotate)
			Player_Rotate(DIR_LEFT, _fDeltaTime, pCamera);
		}
	}
	else if (pManagement->Key_Pressing(DIK_D) && !m_bDead && bCineEnd)
	{
		if (!m_bRoll && !m_bDash)
		{
			if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bStaminaCool && !m_bJump && !m_bGravity && !m_bUse_Item && !m_bUse_Heal && !m_bTargeting && pManagement->Key_Pressing(DIK_LSHIFT) && (m_tState.fStamina > 0.f))
			{
				m_tState.fStamina -= 10.f * _fDeltaTime;
				m_eCurState = SPRINT;
				DashTrailRandomSwitch();
			}
			else
			{
				DashTrailSwitch(false);
				if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bTargeting)
				{
					if (m_bUse_Heal)
						m_eCurState = HEAL_WALK;
					else if (m_bUse_Item)
						m_eCurState = USE_WALK;
					else
					{
						if (!m_bJump && !m_bGravity)
							m_eCurState = RUN;
					}
				}
				else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && m_bTargeting)
				{
					
					if (m_bUse_Heal)
						m_eCurState = TWALK_HEAL_RIGHT;
					else if (m_bUse_Item)
						m_eCurState = TWALK_USE_RIGHT;
					else if (!m_bUse_Heal && !m_bUse_Item)
					{
						if (!m_bJump && !m_bGravity)
							m_eCurState = TWALK_RIGHT;
					}
				}
			}
			if (!m_bCharging && !m_bChargeAttack && !m_bAttack)
			m_pTransform->Set_Pos(pManagement->Move(L"Player", -m_vRight, m_tTransformDesc.fSpeedPerSecond, _fDeltaTime));
			if (m_bRotate)
			Player_Rotate(DIR_RIGHT, _fDeltaTime, pCamera);
		}
	}
	else
	{
		if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bGravity && !m_bJump && !m_bRoll && !m_bDash && !m_bTargeting && !m_bUse_Heal && !m_bUse_Item && !m_bDead)
			m_eCurState = IDLE_PEACE;
		else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bGravity && !m_bJump && !m_bRoll && !m_bDash && !m_bTargeting && m_bUse_Heal && !m_bUse_Item && !m_bDead)
			m_eCurState = HEAL;
		else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bGravity && !m_bJump && !m_bRoll && !m_bDash && !m_bTargeting && !m_bUse_Heal && m_bUse_Item && !m_bDead)
			m_eCurState = USE;
		else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bGravity && !m_bJump && !m_bRoll && !m_bDash && m_bTargeting && !m_bUse_Heal && !m_bUse_Item && !m_bDead)
			m_eCurState = IDLE_ATTACK;
		else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bGravity && !m_bJump && !m_bRoll && !m_bDash && m_bTargeting && m_bUse_Heal && !m_bUse_Item && !m_bDead)
			m_eCurState = TWALK_HEAL_IDLE;
		else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bGravity && !m_bJump && !m_bRoll && !m_bDash && m_bTargeting && !m_bUse_Heal && m_bUse_Item && !m_bDead)
			m_eCurState = TWALK_USE_IDLE;
	}
	// 스킬 구르기 등등의 상호작용 관련
	if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bStaminaCool && pManagement->Key_Down(DIK_SPACE) && m_iJump > 0 && !m_bRoll && !m_bDash && !m_bUse_Heal && !m_bUse_Item && !m_bDead && bCineEnd)
	{
		m_tState.fStamina -= 5.f;
		m_eCurState = JUMP_START;
		m_iJump -= 1;
		m_bJump = true;
		m_bJumping = true;
		m_bGravity = false;
		m_pTransform->Get_Info(EInfo::Pos, &m_vUp);
		//D3DXVec3Normalize(&m_vUp, &m_vUp);
		//m_pTransform->Set_Pos(pManagement->Move(m_vUp, 0.2f, _fDeltaTime));
	}
	if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bStaminaCool && pManagement->Key_Down(DIK_LCONTROL)&& !m_bDash &&!m_bRoll && !m_bAttack && !m_bUse_Heal && !m_bUse_Item && !m_bDead && bCineEnd)
	{
		if (!m_bTargeting && m_iDash > 0 && (m_bJump || m_bGravity))
		{
			m_tState.fStamina -= 5.f;

			m_pTransform->Get_Info(EInfo::Look, &m_vRollDir);
			m_eCurState = DASH_AIR;
			m_fRollTime = 0.f;
			m_bDash = true;
		/*	m_bJump = false;
			m_bGravity = false;*/
			m_iDash -= 1;
		}
		else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && m_bTargeting && m_iDash > 0)
		{
			if (pManagement->Key_Pressing(DIK_W))
			{
				if (pManagement->Key_Pressing(DIK_A))
				{
					m_vRollDir = m_vLU;
					m_eCurState = DASH_LEFT;
				}
				else if (pManagement->Key_Pressing(DIK_D))
				{
					m_vRollDir = m_vRU;
					m_eCurState = DASH_RIGHT;
				}
				else
				{
					m_vRollDir = m_vLook;
					m_eCurState = DASH_FRONT;
				}
			}
			else if (pManagement->Key_Pressing(DIK_S))
			{
				if (pManagement->Key_Pressing(DIK_A))
				{
					m_vRollDir = -m_vRU;
					m_eCurState = DASH_LEFT;
				}
				else if (pManagement->Key_Pressing(DIK_D))
				{
					m_vRollDir = -m_vLU;
					m_eCurState = DASH_RIGHT;
				}
				else
				{
					m_vRollDir = -m_vLook;
					m_eCurState = DASH_BACK;
				}
			}
			else if (pManagement->Key_Pressing(DIK_A))
			{
				m_vRollDir = m_vRight;
				m_eCurState = DASH_LEFT;
			}
			else if (pManagement->Key_Pressing(DIK_D))
			{
				m_vRollDir = -m_vRight;
				m_eCurState = DASH_RIGHT;
			}
			m_tState.fStamina -= 5.f;

			m_fRollTime = 0.f;
			m_bDash = true;
			m_iDash -= 1;
			//CreateBoneParticle();
		}
		else if (!m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bJump && !m_bGravity && !m_bTargeting)
		{
			m_pTransform->Get_Info(EInfo::Look, &m_vRollDir);
			m_tState.fStamina -= 5.f;
			m_fRollTime = 0.f;
			m_eCurState = ROLL;
			m_bRoll = true;
		}
		
	}
	if (pManagement->Key_Down(DIK_E) && !m_bCharging && !m_bChargeAttack && !m_bAttack && !m_bRoll && !m_bDash && !m_bJump && !m_bDead && bCineEnd)
	{
		Swap_Item();
		if (m_eItem != ITEM::ITEM_NONE)
		{
			m_bUse_Quick_Item = true;
			if (m_eItem == ITEM::HEALTH)
				m_bUse_Heal = true;
			else if (m_eItem == ITEM::SOUL)
				m_bUse_Item = true;
		}
	}
	if (!m_bCharging && !m_bChargeAttack && !m_bAttack && m_bRoll && m_fRollTime <= 0.8f)
	{
		D3DXVec3Normalize(&m_vRollDir, &m_vRollDir);
		_float fSpeed;
			m_bGod_Mode = true;
			if(m_pMesh->Get_Animationset() == (_uint)PlayerAniID::ROLL && !m_pMesh->Get_AnimationFrame(25,13))
				fSpeed = 15.f;
			else
				fSpeed = 10.f;
		m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vRollDir, fSpeed, _fDeltaTime));
		m_fRollTime += _fDeltaTime;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::ROLL)
	{
		if (m_pMesh->Get_AnimationFrame(25, 1))
			CreateBoneParticle();

		if (m_pMesh->Is_AnimationSetEnd())
		{
			m_bGod_Mode = false;
			m_bRoll = false;
		}
	}
		
	if (m_bDash)
	{
		Dash(_fDeltaTime);
		m_bGod_Mode = true;
	}
	if ((m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_AIR ||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_FRONT||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_BACK||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_LEFT||
		m_pMesh->Get_Animationset() == (_uint)PlayerAniID::DASH_RIGHT))
	{

		if (m_pMesh->Get_AnimationFrame(19, 1))
			CreateBoneParticle();

		if (m_pMesh->Is_AnimationSetEnd())
		{
			DashTrailSwitch(false);
			m_bJump = false;
			m_bGravity = true;
			m_bGravStart = true;
			m_bDash = false;
			m_bDashCool = true;
			m_bGod_Mode = false;
		}
	
	}
	if (m_bDashCool)
	{
		m_fDashTime += _fDeltaTime;
		if (m_fDashTime >= 1.f)
		{
			m_fDashTime = 0.f;
			m_bDashCool = false;
			m_iDash = 1;
		}
	}

	if (pManagement->KeyDown(EMouseKey::MB) && bCineEnd)
		Targeting();

	if (m_bTargeting)
		OnTarget(_fDeltaTime);

	Weapon_Swap();
}

void CPlayer::Update_AnimationControl()
{
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HIT && m_pMesh->Is_AnimationSetEnd())
		m_bHit = false;

	TRANSFORM_DESC tTempTRANSFORM_DESC;
	_float3 vPlayerUp;
	_float3 vLook;
	_float3 vThisPos = m_pTransform->Get_TransformDesc().vPos;
	if (m_ePreState != m_eCurState)
	{
		m_bBoneParticle = false;
		switch (m_eCurState)
		{
		case CPlayer::IDLE_PEACE:
			m_bOnWeapon = false;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::IDLE);
			m_fAniSpeed = 1.2f;
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_PLAYER_LIFE);
			Get_EventMgr()->Min_Event(EVENT_VOLATILE_PLAYER_DEAD | EVENT_VOLATILE_PLAYER_REVIVE);

			break;

		case CPlayer::IDLE_ATTACK:
			m_bOnWeapon = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::IDLE_WEAPON);
			m_fAniSpeed = 1.2f;

			break;

		case CPlayer::WALK:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_bOnWeapon = false;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::WALK);
			m_fAniSpeed = 1.f;
			m_bResetCamera = false;
			break;

		case CPlayer::RUN:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_bOnWeapon = false;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_fAniSpeed = 1.2f;
			m_bResetCamera = false;
			if (m_ePreState == SPRINT)
				m_pMesh->Change_Animationset((_uint)PlayerAniID::RUN);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::RUN);

			break;

		case CPlayer::SPRINT:
			m_tTransformDesc.fSpeedPerSecond = 15.f;
			m_bOnWeapon = false;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_fAniSpeed = 1.2f;
			if (m_ePreState == RUN)
				m_pMesh->Change_Animationset((_uint)PlayerAniID::SPRINT);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::SPRINT);

			break;

		case CPlayer::ROLL:
			m_bOnWeapon = false;
			m_bRollSound = true;
			m_fAniSpeed = 1.1f;
			//CreateBoneParticle();
			m_pMesh->Set_Animationset((_uint)PlayerAniID::ROLL);
	
			break;

		case CPlayer::JUMP_START:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			if(!m_bOnWeapon)
				m_bOnWeapon = false;
			else if (m_bOnWeapon)
				m_bOnWeapon = true;

			m_bJumpSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::JUMP_START);
			
			tTempTRANSFORM_DESC.vScale = { 1.1f,2.f,0.6f };
			tTempTRANSFORM_DESC.vPos = m_pTransform->Get_TransformDesc().vPos;
			tTempTRANSFORM_DESC.vPos.y += 0.2f;
		/*	m_pTransform->Get_Info(EInfo::Upk, &vPlayerUp);*/
			//tTempTRANSFORM_DESC.vPos -= vPlayerLook;
			tTempTRANSFORM_DESC.vRotate = m_pTransform->Get_TransformDesc().vRotate;

			tTempTRANSFORM_DESC.vRotate.x = D3DXToRadian(90.f);
			Get_Manager()->Generate_Decal(m_iSceneIdx, 
				tTempTRANSFORM_DESC, _float4(1.f, 1.f, 1.f,1.f), 13);
			Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, tTempTRANSFORM_DESC.vPos, 5.f,
				&vThisPos, 25,_float4(0.607843f,0.529411f,1.f,1.f) /*_ARGB(255, 155, 135, 255)*/
				//, ePartilcePatternType::OnStartBlinkAndGoStraightToTarget
				, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraightToTarget/* | (_ulong)ePartilcePatternType::x2*/)
				, 0, 2.2f);
			break;
		case CPlayer::JUMP_AIR:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			if (!m_bOnWeapon)
				m_bOnWeapon = false;
			else if (m_bOnWeapon)
				m_bOnWeapon = true;

			m_pMesh->Set_Animationset((_uint)PlayerAniID::JUMP_AIR);

			break;

		case CPlayer::JUMP_END:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			if (!m_bOnWeapon)
				m_bOnWeapon = false;
			else if (m_bOnWeapon)
				m_bOnWeapon = true;

			m_pMesh->Set_Animationset((_uint)PlayerAniID::JUMP_LAND);

			break;

		case CPlayer::SLEEPING:
			m_tTransformDesc.fSpeedPerSecond = 2.f;
			m_fAniSpeed = 0.7f;
			m_bOnWeapon = false;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::SLEEPING);
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_PLAYER_REVIVE);
			Get_EventMgr()->Min_Event(EVENT_VOLATILE_PLAYER_DEAD | EVENT_VOLATILE_PLAYER_LIFE);
			break;

		case CPlayer::WAKE_UP:
			m_tTransformDesc.fSpeedPerSecond = 2.f;
			m_fAniSpeed = 0.7f;
			m_bOnWeapon = false;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::WAKE_UP);
			break;

		case CPlayer::HEAL:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = false;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT)
			{
					m_pMesh->Change_Animationset((_uint)PlayerAniID::HEALTH);
			}
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::HEALTH);

			break;

		case CPlayer::HEAL_WALK:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = false;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT)
			{
				m_pMesh->Change_Animationset((_uint)PlayerAniID::HEALTH_WALK);
			}
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::HEALTH_WALK);

			break;

		case CPlayer::USE:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = false;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USEITEM_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT)
			{
					m_pMesh->Change_Animationset((_uint)PlayerAniID::USE_ITEM);
			}
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::USE_ITEM);

			break;

		case CPlayer::USE_WALK:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = false;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USEITEM_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT)
			{
					m_pMesh->Change_Animationset((_uint)PlayerAniID::USE_ITEM_WALK);
			}
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::USE_ITEM_WALK);

			break;

		case CPlayer::TWALK_FRONT:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_ePreState == TWALK_BACK || m_ePreState == TWALK_LEFT || m_ePreState == TWALK_RIGHT)
				m_pMesh->Change_Animationset((_uint)PlayerAniID::TARGETING_WALK_FRONT);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::TARGETING_WALK_FRONT);

			break;

		case CPlayer::TWALK_BACK:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_ePreState == TWALK_FRONT || m_ePreState == TWALK_LEFT || m_ePreState == TWALK_RIGHT)
				m_pMesh->Change_Animationset((_uint)PlayerAniID::TARGETING_WALK_BACK);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::TARGETING_WALK_BACK);

			break;

		case CPlayer::TWALK_LEFT:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_ePreState == TWALK_FRONT || m_ePreState == TWALK_BACK || m_ePreState == TWALK_RIGHT)
				m_pMesh->Change_Animationset((_uint)PlayerAniID::TARGETING_WALK_LEFT);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::TARGETING_WALK_LEFT);

			break;

		case CPlayer::TWALK_RIGHT:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_ePreState == TWALK_FRONT || m_ePreState == TWALK_BACK || m_ePreState == TWALK_LEFT)
				m_pMesh->Change_Animationset((_uint)PlayerAniID::TARGETING_WALK_RIGHT);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::TARGETING_WALK_RIGHT);

			break;

		case CPlayer::TWALK_HEAL_IDLE:
			m_bOnWeapon = true;
			m_fAniSpeed = 1.2f;
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT)
					m_pMesh->Change_Animationset((_uint)PlayerAniID::HEALTH_TIDLE);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::HEALTH_TIDLE);

			break;

		case CPlayer::TWALK_HEAL_FRONT:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT)
					m_pMesh->Change_Animationset((_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT);

			break;

		case CPlayer::TWALK_HEAL_BACK:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT)
					m_pMesh->Change_Animationset((_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK);

			break;

		case CPlayer::TWALK_HEAL_LEFT:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT)
					m_pMesh->Change_Animationset((_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT);

			break;

		case CPlayer::TWALK_HEAL_RIGHT:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::HEALTH_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::TARGETING_WALK_HEAL_LEFT)
					m_pMesh->Change_Animationset((_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::TARGETING_WALK_HEAL_RIGHT);

			break;

		case CPlayer::TWALK_USE_IDLE:
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT)
					m_pMesh->Change_Animationset((_uint)PlayerAniID::USEITEM_TIDLE);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::USEITEM_TIDLE);

			break;

		case CPlayer::TWALK_USE_FRONT:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USEITEM_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT)
				m_pMesh->Change_Animationset((_uint)PlayerAniID::USE_ITEM_TWALK_FRONT);
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::USE_ITEM_TWALK_FRONT);

			break;

		case CPlayer::TWALK_USE_BACK:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USEITEM_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT)
			{
				m_pMesh->Change_Animationset((_uint)PlayerAniID::USE_ITEM_TWALK_BACK);
			}
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::USE_ITEM_TWALK_BACK);

			break;

		case CPlayer::TWALK_USE_LEFT:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USEITEM_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT)
			{
				m_pMesh->Change_Animationset((_uint)PlayerAniID::USE_ITEM_TWALK_LEFT);
			}
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::USE_ITEM_TWALK_LEFT);

			break;

		case CPlayer::TWALK_USE_RIGHT:
			m_tTransformDesc.fSpeedPerSecond = 7.f;
			m_fAniSpeed = 1.2f;
			m_bOnWeapon = true;
			m_bLeftLag = true;
			m_bRightLag = true;
			if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_WALK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USEITEM_TIDLE ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_FRONT ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_BACK ||
				m_pMesh->Get_Animationset() == (_uint)PlayerAniID::USE_ITEM_TWALK_LEFT)
			{
				m_pMesh->Change_Animationset((_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT);
			}
			else
				m_pMesh->Set_Animationset((_uint)PlayerAniID::USE_ITEM_TWALK_RIGHT);

			break;

		case CPlayer::DASH_AIR:
			m_fAniSpeed = 2.8f;
			//CreateBoneParticle();
			m_bOnWeapon = false;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::DASH_AIR);

			

			tTempTRANSFORM_DESC.vScale = { 0.7f,1.5f,0.6f };
			tTempTRANSFORM_DESC.vPos = m_pTransform->Get_TransformDesc().vPos;
			tTempTRANSFORM_DESC.vPos.y += 1.5f;
			/*	m_pTransform->Get_Info(EInfo::Upk, &vPlayerUp);*/
			//tTempTRANSFORM_DESC.vPos -= vPlayerLook;
			tTempTRANSFORM_DESC.vRotate = m_pTransform->Get_TransformDesc().vRotate;
			
			memcpy(&vLook,&m_pTransform->Get_TransformDesc().matWorld.m[2][0],sizeof(_float3));
			D3DXVec3Normalize(&vLook, &vLook);
			tTempTRANSFORM_DESC.vPos -= vLook;
			/*tTempTRANSFORM_DESC.vRotate.x = D3DXToRadian(90.f);

			tTempTRANSFORM_DESC.vRotate.y = D3DXToRadian(90.f);*/
			//tTempTRANSFORM_DESC.vRotate.x = D3DXToRadian(90.f);

			Get_Manager()->Generate_Decal(m_iSceneIdx,
				tTempTRANSFORM_DESC, _float4(1.f, 1.f, 1.f, 1.f), 13);
			
			tTempTRANSFORM_DESC.vScale *= 1.5f;
			
			Get_Manager()->Generate_Decal(m_iSceneIdx,
				tTempTRANSFORM_DESC, _float4(1.f, 1.f, 1.f, 1.f), 13);
			
			tTempTRANSFORM_DESC.vScale *= 1.5f;
			tTempTRANSFORM_DESC.vScale.x = tTempTRANSFORM_DESC.vScale.y;
			tTempTRANSFORM_DESC.vScale.z = tTempTRANSFORM_DESC.vScale.y;
			tTempTRANSFORM_DESC.vPos += (vLook * 2.f);

			Get_Manager()->Generate_Decal(m_iSceneIdx,
				tTempTRANSFORM_DESC, _float4(1.f, 1.f, 1.f, 1.f), 19);

			tTempTRANSFORM_DESC.vScale *= 0.6f;
			tTempTRANSFORM_DESC.vPos -= vLook;

			Get_Manager()->Generate_Decal(m_iSceneIdx,
				tTempTRANSFORM_DESC, _float4(1.f, 1.f, 1.f, 1.f), 16,
				(ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlinkPow
					| (_ulong)ePartilcePatternType::GoStraight 
					| (_ulong)ePartilcePatternType::ScaleUp
					| (_ulong)ePartilcePatternType::RotateZ 
					| (_ulong)ePartilcePatternType::x3));
			
			Get_Manager()->Generate_ExplosionParticle(m_iSceneIdx, tTempTRANSFORM_DESC.vPos, 5.f,
				&vThisPos, 25, _float4(0.607843f, 0.529411f, 1.f, 1.f)
				//, ePartilcePatternType::OnStartBlinkAndGoStraightToTarget
				, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraightToTarget)
				, 0, 2.2f);

		
			DashTrailSwitch(true);
			DashTrailColor_Set(_float3(0.25f, 0.25f, 0.25f));
			DashTrailWidth_Set(20.f);
			DashTrailRenderId_Set(9);
			break;

		case CPlayer::DASH_FRONT:
			m_fAniSpeed = 2.8f;
			//CreateBoneParticle();
			m_bOnWeapon = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::DASH_FRONT);


			DashTrailSwitch(true);
			DashTrailColor_Set(_float3(0.25f, 0.25f, 0.25f));
			DashTrailWidth_Set(20.f);
			DashTrailRenderId_Set(9);
			break;

		case CPlayer::DASH_BACK:
			m_fAniSpeed = 2.8f;
			//CreateBoneParticle();
			m_bOnWeapon = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::DASH_BACK);

			DashTrailSwitch(true);
			DashTrailColor_Set(_float3(0.25f, 0.25f, 0.25f));
			DashTrailWidth_Set(20.f);
			DashTrailRenderId_Set(9);
			break;

		case CPlayer::DASH_LEFT:
			m_fAniSpeed = 2.8f;
			//CreateBoneParticle();
			m_bOnWeapon = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::DASH_LEFT);

			DashTrailSwitch(true);
			DashTrailColor_Set(_float3(0.25f, 0.25f, 0.25f));
			DashTrailWidth_Set(20.f);
			DashTrailRenderId_Set(9);
			break;

		case CPlayer::DASH_RIGHT:
			m_fAniSpeed = 2.8f;
			//CreateBoneParticle();
			m_bOnWeapon = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::DASH_RIGHT);

			DashTrailSwitch(true);
			DashTrailColor_Set(_float3(0.25f, 0.25f, 0.25f));
			DashTrailWidth_Set(20.f);
			DashTrailRenderId_Set(9);
			break;

		case CPlayer::INTERACTION:
			m_fAniSpeed = 1.2f;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::INTERACTION);
			m_bOnWeapon = false;
			break;

		case CPlayer::NORMAL_ATTACK1:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.3f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_bOnWeapon = true;
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::NORMAL_WEAPON_ATTACK1);

			break;

		case CPlayer::NORMAL_ATTACK2:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.3f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_bOnWeapon = true;
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::NORMAL_WEAPON_ATTACK2);

			break;

		case CPlayer::NORMAL_ATTACK3:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.3f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_bOnWeapon = true;
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::NORMAL_WEAPON_ATTACK3);

			break;

		case CPlayer::NORMAL_HEAVY_ATTACK1:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.1f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_bOnWeapon = true;
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::NORMAL_HEAVY_ATTACK1);

			break;

		case CPlayer::NORMAL_HEAVY_ATTACK2:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.1f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_bOnWeapon = true;
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::NORMAL_HEAVY_ATTACK2);

			break;
			
		case CPlayer::NORMAL_CHARGING:
			m_tTransformDesc.fSpeedPerSecond = 15.f;
			m_fAniSpeed = 1.2f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_bOnWeapon = true;
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::NORMAL_CHARGING_ATTACK);

			break;

		case CPlayer::KATANA_ATTACK1:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_bOnWeapon = true;
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::MOON_ATTACK1);

			break;

		case CPlayer::KATANA_ATTACK2:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_bOnWeapon = true;
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::MOON_ATTACK2);

			break;
		case CPlayer::MOON_PARRY:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.2f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_bOnWeapon = true;
			m_bAttackSound = true;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::PARRY);
			break;
		case CPlayer::HIT:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.f;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::HIT);

			break;

		case CPlayer::HARD_HIT:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_fAniSpeed = 1.f;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::HARD_DEAL);

			break;

		case CPlayer::DEAD:
			m_tTransformDesc.fSpeedPerSecond = 10.f;
			m_bLeftLag = true;
			m_bRightLag = true;
			m_fAniSpeed = 0.6f;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::DEAD);

			break;
		}
		m_ePreState = m_eCurState;
	}
}

void CPlayer::Player_Interact(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;
	CMapStaticObject_WeaponStand* pStand = dynamic_cast<CMapStaticObject_WeaponStand*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Proto, L"WeaponStand"));

	if ((PxU32)EObstacle::BossStartCinematic & m_pPlayerData->ObstacleType)
	{
		m_pPlayerData->ObstacleType ^= (PxU32)EObstacle::BossStartCinematic;
		pManagement->SetData_Controller(L"Player", m_pPlayerData);


		if (!(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_PLAY_BOSS_CINE) &&
			!(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_SKIP_BOSS_CINE))
		{
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_PLAY_BOSS_CINE);
			m_pTransform->Set_Pos(pManagement->Set_GlobalPos(L"Player", { -474.195770f, 185.652969f ,20.3311825f }));
		}
		//if (!m_bBossCinematic)
		//{
		//	CCinematicCamera_0* pCamera = dynamic_cast<CCinematicCamera_0*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
		//	m_pTransform->Set_Pos(pManagement->Set_GlobalPos(L"Player", { -474.195770f, 185.652969f ,20.3311825f }));
		//	pCamera->Play_NextCinematic(2);
		//	m_bBossCinematic = true;
		//}
		//CJPreto* pPreto = dynamic_cast<CJPreto*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Monster", EFindGameObject::Clone, L"Preto_0"));
		//pPreto->Set_Cinematic();

		//Get_EventMgr()->Add_Event(EVENT_VOLATILE_STAGE_MEETBOSS);
	}

	////if (true)
	////{
	////	CCinematicCamera_0* pCamera = dynamic_cast<CCinematicCamera_0*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
	////	pCamera->Play_NextEndingCinematic(4);
	////}

	//if ((Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_ENDINGSCENESTART) &&
	//	!(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_ENDINGSCENEEND))
	//{
	//	Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_ENDINGSCENEEND);
	//	m_pTransform->Set_Pos(pManagement->Set_GlobalPos(L"Player", { -38.8532715f, 101.555244f, 722.066528f }));
	//}

	//else
	//{
	//	if(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_MEETBOSS)
	//		Get_EventMgr()->Min_Event(EVENT_VOLATILE_STAGE_MEETBOSS);
	//}
	Update_Interact();
	



	if (pManagement->Key_Down(DIK_F))
	{
		if (m_pInterfaceUI->Get_OnRenderGetItem())
		{
			m_pInterfaceUI->OK();
		}
		else if ((PxU32)EObstacle::WeaponStand & m_pPlayerData->ObstacleType)
		{
			if (pStand->Get_OnNormalSword() || pStand->Get_OnMoonSword())
			{
				m_bInteract = true;
				m_eCurState = INTERACTION;
				m_bInteract_Stand = true;
				m_bInteract_StandTrigger = true;
				
				if(pStand->Get_OnMoonSword())
					Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_GET_MOON_WEAPON);
			}
		}
		else if ((PxU32)EObstacle::SavePoint & m_pPlayerData->ObstacleType)
		{
			CMainCamera* pCamera = dynamic_cast<CMainCamera*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Camera"));
			m_vCameraAt = pCamera->Get_CameraDesc().vAt;
			m_vCameraEye = pCamera->Get_CameraDesc().vEye;

			m_bInteract = true;
			m_eCurState = INTERACTION;
			m_bInteract_Save = true;
			Min_ObstacleType(EObstacle::SavePoint);

			
		}
		else if ((PxU32)EObstacle::Item & m_pPlayerData->ObstacleType)
		{
			//m_bInteract = true;
			//m_eCurState = INTERACTION;
			eItemID test = m_pPlayerData->ItemID;
			CItemObj* pItem = dynamic_cast<CItemObj*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, m_pPlayerData->szActivteName));
			if (nullptr != pItem)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player);
				GET_MANAGEMENT->PlaySounds(L"Get_Item.mp3", CSound_Manager::ESoundID::Player);
				m_pInterfaceUI->On_Item(pItem->Get_ItemCode());
				m_pInterfaceUI->Add_Used_Item(pItem->Get_ItemCode());
				pItem->Set_Dead(true);
				Min_ObstacleType(EObstacle::Item);
				
			}
		}
		else if ((PxU32)EObstacle::Lever & m_pPlayerData->ObstacleType)
		{
			m_bInteract = true;
			m_eCurState = INTERACTION;
			
			CMapDynamicObject_Lever* pLeaver = dynamic_cast<CMapDynamicObject_Lever*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, m_pPlayerData->szActivteName));
			
			if (nullptr != pLeaver && !pLeaver->Get_LeverOn())
			{
				m_bLeverOn = true;
				m_wstrLeverName = m_pPlayerData->szActivteName;
				pLeaver->Update_GameObject(_fDeltaTime);
				pLeaver->Render_GameObject();
				Min_ObstacleType(EObstacle::Lever);

			}	
		}
		else if ((PxU32)EObstacle::PortalGate & m_pPlayerData->ObstacleType)
		{
			m_bInteract = true;
			m_eCurState = INTERACTION;
			CMapStaticObject_PortalGate* pPortal = dynamic_cast<CMapStaticObject_PortalGate*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, m_pPlayerData->szActivteName));

			if (nullptr != pPortal && !pPortal->Get_PortalOn())
			{
				m_bPortalGateOn = true;
			}

			/*if ((_uint)ESceneType::Tutorial == m_iSceneIdx && (Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_DEAD_BOSS_PRETO))
			{
				Get_EventMgr()->Add_Event(EVENT_VOLATILE_CHANGE_SCENE);
			}*/
		}		
		else if ((PxU32)EObstacle::Choron & m_pPlayerData->ObstacleType&&!m_bNpcChoronInteract)
		{
			//m_bInteract = true;
			m_bNpcChoronInteract = true;
			//m_eCurState = INTERACTION;
		}
		else if ((PxU32)EObstacle::Valnir & m_pPlayerData->ObstacleType&&!m_bNpcValnirInteract)
		{
			//m_bInteract = true;
			m_bNpcValnirInteract = true;
			//m_eCurState = INTERACTION;
		}
		else if (((PxU32)EObstacle::Yaak & m_pPlayerData->ObstacleType))
		{
			if(!(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_STAGE2_CHESS_END))
				m_bNpcYaakInteract = true;
		}
		else if ((PxU32)EObstacle::LevelStone & m_pPlayerData->ObstacleType)
		{
			//m_bInteract = true;
			m_bLevelStoneInteract = true;
			//m_eCurState = INTERACTION;
			Min_ObstacleType(EObstacle::LevelStone);
		}
		else if (((PxU32)EObstacle::IronDoor & m_pPlayerData->ObstacleType)&&(Get_EventMgr()->Get_QuestFlag()&QUEST_END_1))
		{
			// 위 else if문 처리에 특정 이벤트 매니처에서 값 받아오기(NPC에게 물건 넘기기 or 퀘스트 성공)
			

			Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_DOOR_OPEN_SHRINE);
			m_bInteract = true;
			//m_bNpcValnirInteract = true;
			m_eCurState = INTERACTION;
			m_pMesh->Set_Animationset((_uint)PlayerAniID::IDLE);
		}

	} // 임시
	if (m_bInteract_Stand)
	{
		if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::INTERACTION && m_pMesh->Get_AnimationFrame(61, 30))
		{
			if (pStand->Get_OnNormalSword())
			{
				pStand->Set_OnNormalSword(false);
				CPlayer_Weapon* pStandWeapon = dynamic_cast<CPlayer_Weapon*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Proto, L"Player_Weapon"));
				CPlayer_Weapon* pWeapon = dynamic_cast<CPlayer_Weapon*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player_Weapon", EFindGameObject::Proto, L"Player_Weapon"));
				m_pInterfaceUI->Add_Equip_Item(eWeaponID::Weapon_1);
				pStandWeapon->Set_StandDsv();
				m_bGet_Weapon_Normal = true;
				m_bInteract_Stand = false;
			}
			else if (pStand->Get_OnMoonSword())
			{
				pStand->Set_OnMoonSword(false);
				CPlayer_Weapon* pStandWeapon = dynamic_cast<CPlayer_Weapon*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Proto, L"Player_Weapon"));
				CPlayer_Weapon* pWeapon = dynamic_cast<CPlayer_Weapon*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player_Weapon", EFindGameObject::Proto, L"Player_Weapon"));
				m_pInterfaceUI->Add_Equip_Item(eWeaponID::Weapon_2);
				pStandWeapon->Set_StandDsv();
				m_bGet_Weapon_Moon = true;
				m_bInteract_Stand = false;
			}

			Get_Manager()->PlaySounds(L"Environment_WeaponStand.ogg", CSound_Manager::ESoundID::Object, 0.3f);
			Get_Manager()->SoundVolume(CSound_Manager::ESoundID::Object, 0.5f);
		}
	}

	else if (m_bInteract_Save)
	{
		if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::INTERACTION && m_pMesh->Get_AnimationFrame(61, 30))
		{
			_float3 vPos = m_pTransform->Get_TransformDesc().vPos;
			vPos.y += 1.5f;
			m_vSpawnPos = vPos;
			m_bInteract_Save = false;
			m_bBonfireHeal = true;
			CMapDynamicObject_SavePoint* pSavePoint = dynamic_cast<CMapDynamicObject_SavePoint*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, m_pPlayerData->szActivteName));
			if (nullptr != pSavePoint)
			{
				GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Player);
				GET_MANAGEMENT->PlaySounds(L"Fire.mp3", CSound_Manager::ESoundID::Player, 0.3f);
				pSavePoint->Set_Active();
				pSavePoint->On_Sound();
			}
		}
	}

	/*else if (m_bNpcValnirInteract)
	{
			m_bNpcValnirInteract = false;
	}*/
	/*else if (m_bNpcValnirInteract || m_bNpcChoronInteract || m_bLevelStoneInteract)
	{
		if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::INTERACTION)
		{
			m_bNpcValnirInteract = false;
			m_bNpcChoronInteract = false;
			m_bLevelStoneInteract = false;
			m_bInteract = false;
		}
	}

	else if (m_bInteract_Save)
	{
		if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::INTERACTION)
		{
			m_bPortalGateOn = false;
		}
	}
	
	}*/
	if (m_bNpcValnirInteract || m_bNpcChoronInteract || m_bLevelStoneInteract || m_bNpcYaakInteract)
	{
		m_eCurState = IDLE_PEACE;
		m_bNPCInteracting = true;
	}
	else
		m_bNPCInteracting = false;

	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::INTERACTION && m_pMesh->Is_AnimationSetEnd())
		m_bInteract = false;
}

void CPlayer::Update_Interact()
{
	/*else if (((PxU32)EObstacle::SavePoint & m_pPlayerData->ObstacleType) && !m_bInteract)
	{
	m_eCollAct = INTERACT::INTER_ACT;
	}
	else if (((PxU32)EObstacle::Item & m_pPlayerData->ObstacleType) && !m_bInteract)
	{
	m_eCollAct = INTERACT::INTER_ACT;
	}
	else if (((PxU32)EObstacle::Lever & m_pPlayerData->ObstacleType) && !m_bInteract)
	{
	m_eCollAct = INTERACT::INTER_ACT;
	}
	else if (((PxU32)EObstacle::PortalGate & m_pPlayerData->ObstacleType) && !m_bInteract)
	{
	m_eCollAct = INTERACT::INTER_ACT;
	}
	else if (((PxU32)EObstacle::LevelStone & m_pPlayerData->ObstacleType) && !m_bInteract)
	{
	m_eCollAct = INTERACT::INTER_ACT;
	}*/
	if (((PxU32)EObstacle::WeaponStand & m_pPlayerData->ObstacleType) && !m_bInteract)
	{
		if (m_iSceneIdx == (_uint)ESceneType::Tutorial && !m_bGet_Weapon_Normal)
			m_eCollAct = INTERACT::INTER_ACT;
		else if(((PxU32)EObstacle::IronDoor & m_pPlayerData->ObstacleType) && (Get_EventMgr()->Get_QuestFlag()&QUEST_END_1) && !(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_DOOR_OPEN_SHRINE))
			m_eCollAct = INTERACT::INTER_ACT;
		else if (m_iSceneIdx == (_uint)ESceneType::Shrine && !m_bGet_Weapon_Moon)
			m_eCollAct = INTERACT::INTER_ACT;
		else
			m_eCollAct = INTERACT::INTER_END;
	}
		else if (((PxU32)EObstacle::SavePoint & m_pPlayerData->ObstacleType)
			|| (((PxU32)EObstacle::Item & m_pPlayerData->ObstacleType) && !m_bElevator)
			|| ((PxU32)EObstacle::Lever & m_pPlayerData->ObstacleType)
			|| ((PxU32)EObstacle::PortalGate & m_pPlayerData->ObstacleType)
			|| ((PxU32)EObstacle::LevelStone & m_pPlayerData->ObstacleType)
			&& !m_bInteract)
		{
			m_eCollAct = INTERACT::INTER_ACT;
		}
		else if (((PxU32)EObstacle::Choron & m_pPlayerData->ObstacleType && !m_bNpcChoronInteract)
			|| ((PxU32)EObstacle::Valnir & m_pPlayerData->ObstacleType && !m_bNpcValnirInteract)
			|| (((PxU32)EObstacle::Yaak & m_pPlayerData->ObstacleType)) && !m_bInteract)
		{
			m_eCollAct = INTERACT::INTER_TALK;
		}
		else
			m_eCollAct = INTERACT::INTER_END;
}

void CPlayer::Player_Rotate(DIR _eDir, const _float & _fDeltaTime, CMainCamera* _pCamera)
{
	// 절대 건들지마세요 
	auto pManagement = GET_MANAGEMENT;
	_float3 vCamLook, vCamRight, vCamLU, vCamRU, vPlayerLook, vPlayerRight;
	_float fDot, fLeft, fRight;
	vCamLook = _pCamera->Get_PlayerMovingCam().vAt - _pCamera->Get_PlayerMovingCam().vEye;
	D3DXVec3Cross(&vCamRight, &vCamLook, &_float3(0.f,1.f,0.f));
	m_pTransform->Get_Info(EInfo::Look, &vPlayerLook);
	m_pTransform->Get_Info(EInfo::Right, &vPlayerRight);
	vCamLook.y = 0.f;
	vCamRight.y = 0.f;
	vCamRU = vCamLook + -vCamRight;
	vCamLU = vCamLook + vCamRight;
	D3DXVec3Normalize(&vCamLU, &vCamLU);
	D3DXVec3Normalize(&vCamRU, &vCamRU);
	D3DXVec3Normalize(&vCamLook, &vCamLook);
	D3DXVec3Normalize(&vCamRight, &vCamRight);
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);
	switch (_eDir)
	{
	case CPlayer::DIR_LEFT:
		fDot = acos(D3DXVec3Dot(&vPlayerLook, &m_vRight)) * (180.f / D3DX_PI);
		fLeft = D3DXVec3Dot(&-m_vRight, &-vPlayerRight);
		fRight = D3DXVec3Dot(&-m_vRight, &vPlayerRight);
		break;
	case CPlayer::DIR_RIGHT:
		fDot = acos(D3DXVec3Dot(&vPlayerLook, &-m_vRight)) * (180.f / D3DX_PI);
		fLeft = D3DXVec3Dot(&m_vRight, &-vPlayerRight);
		fRight = D3DXVec3Dot(&m_vRight, &vPlayerRight);
		break;
	case CPlayer::DIR_FRONT:
		fDot = acos(D3DXVec3Dot(&vPlayerLook, &m_vLook)) * (180.f / D3DX_PI);
		fLeft = D3DXVec3Dot(&-m_vLook, &-vPlayerRight);
		fRight = D3DXVec3Dot(&-m_vLook, &vPlayerRight);
		break;
	case CPlayer::DIR_BACK:
		fDot = acos(D3DXVec3Dot(&vPlayerLook, &-m_vLook)) * (180.f / D3DX_PI);
		fLeft = D3DXVec3Dot(&m_vLook, &-vPlayerRight);
		fRight = D3DXVec3Dot(&m_vLook, &vPlayerRight);
		break;
	case CPlayer::DIR_LF:
		fDot = acos(D3DXVec3Dot(&vPlayerLook, &m_vLU)) * (180.f / D3DX_PI);
		fLeft = D3DXVec3Dot(&-m_vLU, &-vPlayerRight);
		fRight = D3DXVec3Dot(&-m_vLU, &vPlayerRight);
		break;
	case CPlayer::DIR_RF:
		fDot = acos(D3DXVec3Dot(&vPlayerLook, &m_vRU)) * (180.f / D3DX_PI);
		fLeft = D3DXVec3Dot(&-m_vRU, &-vPlayerRight);
		fRight = D3DXVec3Dot(&-m_vRU, &vPlayerRight);
		break;
	case CPlayer::DIR_LB:
		fDot = acos(D3DXVec3Dot(&vPlayerLook, &-m_vRU)) * (180.f / D3DX_PI);
		fLeft = D3DXVec3Dot(&m_vRU, &-vPlayerRight);
		fRight = D3DXVec3Dot(&m_vRU, &vPlayerRight);
		break;
	case CPlayer::DIR_RB:
		fDot = acos(D3DXVec3Dot(&vPlayerLook, &-m_vLU)) * (180.f / D3DX_PI);
		fLeft = D3DXVec3Dot(&m_vLU, &-vPlayerRight);
		fRight = D3DXVec3Dot(&m_vLU, &vPlayerRight);
		break;
	}
	if (fDot != fDot)
		return;
	if (fRight > 0 && fLeft < 0.f)
		fDot *= -1.f;
	else if (fRight < 0 && fLeft > 0.f)
		fDot *= 1.f;

	if (!m_bTargeting)
	{
		if (fDot != 0.f)
		{
			m_pTransform->Rotate(EAxis::Y, D3DXToRadian(fDot), m_pTransform->Get_TransformDesc().fRotatePerSecond, _fDeltaTime);
			_pCamera->Set_CameraAngleY(-fDot* m_pTransform->Get_TransformDesc().fRotatePerSecond*_fDeltaTime);
		}
	}
}

void CPlayer::Player_Fall(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	if (!m_bDash)
	{
		_float3 vPos = pManagement->Player_Gravity(L"Player", &m_bJump, &m_bJumpStart, &m_bDoubleJump, &m_bJumping, &m_bGravity, &m_bGravStart, &m_iJump, &m_iDash, &m_vJumpPos, 8.f, &m_fJumpTime, &m_fFallDamage ,&m_fAirTime, _fDeltaTime);
		if (m_fFallDamage != 0.f)
		{
			Add_PlayerAttack(m_fFallDamage,DAMAGE::FALL_DAMAGE);
			m_fFallDamage = 0.f;
		}

		if ((m_pPlayerData->ObstacleType & (PxU32)EObstacle::Elevator) && !m_bJump && !m_bGravity)
		{
			CMapStaticObject_Elevator* pElevator = dynamic_cast<CMapStaticObject_Elevator*>(pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Clone, m_pPlayerData->szActivtedName));
			if (!pElevator->Get_MovingDir())
			{
				m_bOnElevater = true;
				//m_wstrlevator = m_pPlayerData->szActivtedName;
				vPos = pManagement->Move(L"Player", { 0.f, 1.f, 0.f }, 10.f, _fDeltaTime);
				vPos.y -= 0.4f;
			}
			else
			{
				m_bOnElevater = false;
			}
				//m_wstrlevator = L"";
		}
		else
		{
			m_bOnElevater = false;
		}
		m_pTransform->Set_Pos(vPos);
	}		
	
	if (!m_bStart && !m_bRoll && !m_bDead && !m_bHit && !m_bJump && !m_bDash && m_bGravity)
	{
		if (m_bAttack)
		{
			
			m_bAttack = false;
			m_eAttack = NO_ATTACK;
			m_iHeavyAttack = 0;
			m_iLightAttack = 0;
		}
		if (m_bChargeAttack || m_bCharging)
		{
			m_bAttack = false;
			m_eAttack = NO_ATTACK;
			m_fAttackTime = 0.f;
			m_bChargeAttack = false;
			m_bCharging = false;
		}
		if (m_bUse_Heal || m_bUse_Item)
		{
			m_bUse_Heal = false;
			m_bUse_Item = false;
		}
		m_bJumping = true;
		m_eCurState = JUMP_AIR;
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::JUMP_START && m_pMesh->Is_AnimationSetEnd())
		m_eCurState = JUMP_AIR;
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::JUMP_AIR && m_pMesh->Is_AnimationSetEnd() && (!m_bJumping))
	{
		m_eCurState = JUMP_END;
		m_bJump = false;
		m_bGravity = false;
		m_iJump = 2;
	
	}
	else if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::JUMP_LAND && m_pMesh->Is_AnimationSetEnd())
	{
		m_eCurState = IDLE_PEACE;
		m_iJump = 2;
		m_iDash = 1;
	}
}

void CPlayer::Update_Camera_Dir(class CMainCamera* _pCamera)
{
	_float3 vUp;
	auto pManagement = GET_MANAGEMENT;
	m_vLook = _pCamera->Get_PlayerMovingCam().vEye - _pCamera->Get_PlayerMovingCam().vAt;
	m_vLook.y = 0.f;
	D3DXVec3Normalize(&m_vLook, &m_vLook);
	m_pTransform->Get_Info(EInfo::Up, &vUp);
	D3DXVec3Normalize(&vUp, &vUp);

	D3DXVec3Cross(&m_vRight, &m_vLook, &vUp);
	m_vRight.y = 0.f;
	m_vRU = m_vLook + -m_vRight;
	m_vLU = m_vLook + m_vRight;
	D3DXVec3Normalize(&m_vRight, &m_vRight);
	D3DXVec3Normalize(&m_vLU, &m_vLU);
	D3DXVec3Normalize(&m_vRU, &m_vRU);
}

void CPlayer::Ending(const _float & _fDeltaTime)
{
	m_bEndingStart = true;

	if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_ENDING) && !(Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_DEAD_BOSS_MALKHEL))
	{
		m_eCurState = STATE::IDLE_PEACE;
		m_pTransform->Set_Pos(GET_MANAGEMENT->Set_GlobalPos(L"Player", { 79.3681946f, 28.3146477f ,512.534668f }));
	}

	if ((Get_EventMgr()->Get_CurStaticEvent() & EVENT_STATIC_DEAD_BOSS_MALKHEL) && !m_bCinematicTeleport1)
	{
		m_pTransform->Set_Pos(GET_MANAGEMENT->Set_GlobalPos(L"Player", { 39.5513992f, 28.3146477f ,588.565796f }));
		m_bCinematicTeleport1 = true;
	}

	if (m_bCinematicTeleport1 && !(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_CINEMATICSTART))
	{
		CManagement::Get_Instance()->StopSound(CSound_Manager::ESoundID::BGM);
		CManagement::Get_Instance()->PlayBGM(L"Themes_Ending.ogg", 0.5f);

		_float3 vLook = _float3{ 39.5513992f, 0.f ,588.565796f } - _float3{-30.283842f,0.f,706.484558f};
		vLook;
		D3DXVec3Normalize(&vLook, &vLook);
		_float3 vPlayerLook, vPlayerRight;
		m_pTransform->Get_Info(EInfo::Look, &vPlayerLook);
		m_pTransform->Get_Info(EInfo::Right, &vPlayerRight);
		D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

		m_pTransform->Set_Info(EInfo::Look, -vLook);

		vLook.y = 0.f;
		
		_float fDot = D3DXVec3Dot(&vPlayerLook, &vLook);
		_float fRight = D3DXVec3Dot(&vPlayerRight, &vLook);
		_float fLeft = D3DXVec3Dot(&-vPlayerRight, &vLook);
		_float Radian = acosf(fDot);
		
		if (Radian != Radian)
			Radian = 0.f;

		if (fRight > 0 && fLeft < 0.f)
			fDot *= -1.f;
		else if (fRight < 0 && fLeft > 0.f)
			fDot *= 1.f;

		m_pTransform->Rotate(EAxis::Y, Radian, _fDeltaTime * m_tTransformDesc.fRotatePerSecond);

		m_pTransform->Set_Pos(GET_MANAGEMENT->Move(L"Player", vLook, 1.5f, _fDeltaTime));

		m_eCurState = STATE::WALK;
		if (::CoolTime(_fDeltaTime, m_fCinematicTimer, 8.f))
		{
			m_fCinematicTimer = 0.f;
			CCinematicCamera_0* pCamera = dynamic_cast<CCinematicCamera_0*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_CinemaCamera"));
			pCamera->Play_NextEndingCinematic(4);
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_CINEMATICSTART);
		}
	}

	if ((Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_CINEMATICSTART) && !m_bCinematicTeleport2)
	{
		m_pTransform->Set_Pos(GET_MANAGEMENT->Set_GlobalPos(L"Player", { -32.0465584f,99.5323944f,709.766174f }));
		m_bCinematicTeleport2 = true;
	}

	if (m_bCinematicTeleport1 && !(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_CINEMATICEND))
	{
		_float3 vLook = _float3{ 39.5513992f, 0.f ,588.565796f } -_float3{ -30.283842f,0.f,706.484558f };
		D3DXVec3Normalize(&vLook, &vLook);
		m_pTransform->Set_Pos(GET_MANAGEMENT->Move(L"Player", vLook, 1.5f, _fDeltaTime));
		m_pTransform->Set_Info(EInfo::Look, -vLook);

		m_eCurState = STATE::WALK;
		if (::CoolTime(_fDeltaTime, m_fCinematicTimer, 10.f))
		{
			m_fCinematicTimer = 0.f;
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_CINEMATICEND);
			m_eCurState = STATE::INTERACTION;
		}
	}
	if (m_pMesh->Get_Animationset() == (_uint)PlayerAniID::INTERACTION)
	{
		if (m_pMesh->Is_AnimationSetEnd())
		{
			/////////여기에서 씬전환 하시면 됩니다!/////////////
			m_eCurState = STATE::IDLE_PEACE;
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_CHANGE_SCENE);
			/////////여기에서 씬전환 하시면 됩니다!/////////////
		}
	}
}

void CPlayer::Update_Hit(const _float & _fDeltaTime)
{
}

void CPlayer::Dash(const _float & _fDeltaTime)
{
	auto pManagement = GET_MANAGEMENT;

	D3DXVec3Normalize(&m_vRollDir, &m_vRollDir);
	m_vRollDir;
	_float	fSpeed = 15.f;
	m_pTransform->Set_Pos(pManagement->Move(L"Player", m_vRollDir, fSpeed, _fDeltaTime * 2.f));
}


