#pragma once

#ifndef __JPRETO_H__
#define __JPRETO_H__

#include "JBaseBoss.h"
#include "UI_BossMonster.h"

USING(Engine)

class CJPreto final : public CJBaseBoss
{
public:
	enum class PATTERN
	{
		Appear,
		Near_1,		// 1번 베기
		Near_2,		// 1번 베기
		Near_3,		// 1번 베기
		Near_4,		// 1번 베기
		Far_1,		// 1번 베기
		Walk_F,		// 앞으로 걷기
		Dash_B,		// 뒤로 점프하기
		Walk_L,		// 왼쪽으로 걷기
		Walk_R,		// 오른쪽으로 걷기
		Death,		// 죽음
		Reset,
		Pattern_Ready,
		End
	};

protected:
	explicit CJPreto(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CJPreto(const CJPreto& _rOther);
	virtual ~CJPreto() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:/*Getter*/
	CJPreto::PATTERN Get_Pattern() const { return m_ePattern; }


private:
	HRESULT			Weapon_Clone();
	HRESULT			UI_Clone();
	void			AttackCoolTimer(const _float & _fDeltaTime);

protected:
	virtual void Do_Activating(const _float& _fDeltaTime) override;
	virtual void Do_Idle(const _float & _fDeltaTime) override;
	virtual void Do_Attack(const _float & _fDeltaTime) override;
	virtual void Do_Hit(const _float & _fDeltaTime) override;
	virtual void Do_Walk(const _float & _fDeltaTime) override;
	virtual void Do_Other(const _float & _fDeltaTime) override;

protected:
	//강체를 삭제합니다.
	virtual void Delete_RigidBody() override;
	// 타격 위치 파티클 생성
	//L방패 R무기
	void Set_Attack(_bool _bLeft, _bool _bRight);
	void Set_WeaponWorld();

private:
	void Appear_Pattern(const _float & _fDeltaTime);
	void Far_Pattern_1(const _float & _fDeltaTime);
	void Near_Pattern_1(const _float & _fDeltaTime);
	void Near_Pattern_2(const _float & _fDeltaTime);
	void Near_Pattern_3(const _float & _fDeltaTime);
	void Near_Pattern_4(const _float & _fDeltaTime);
	void Walk_Pattern_F(const _float & _fDeltaTime);
	void Walk_Pattern_BacktoField(const _float & _fDeltaTime);
	void Walk_Pattern_L(const _float & _fDeltaTime);
	void Walk_Pattern_R(const _float & _fDeltaTime);
	void Dash_Pattern(const _float & _fDeltaTime);
	void Death_Pattern(const _float & _fDeltaTime);
	void Reset_Pattern(const _float & _fDeltaTime);
	void Set_Hit();

	_float3 AppearLook();

	void CheckPlayerPos();

	void AttackEndToMove_ReadyIdle();
	//void CinematicTimeer(const _float & _fDeltaTime);

	HRESULT Clone_Trail();

	void Move_Sound();
public:
	virtual void Get_Damaged()	override;

public:
	static CJPreto* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free()override;
private:
	PATTERN				m_ePattern;
	CTrailEffect*		m_pTrailEffect;
	class CJBaseWeapon*	m_pShield;

	_bool				m_bAttackSound2;
	_bool				m_bNearPattern2;
	_bool				m_bNearPattern3;
	_bool				m_bNearPattern4;
	_bool				m_bBackDash;
	_bool				m_bResetPreto;
	
	_float				m_fNearPattern2Time;
	_float				m_fNearPattern3Time;
	_float				m_fNearPattern4Time;
	_float				m_fBackDash;

	_bool				m_bCienmaticShakeStart;
	_float				m_fAppearDelayTime,m_fFallDamage,m_fAirTime;
	_uint				m_iAttackCount;

	_float				m_fDeadPatilceEffectTime;
	_float				m_fAccelTime;

	// 진짜루 필요
	_bool				m_bNearPattern3WeaponRatate;
	_bool				m_bBackDashAble;
	_float3				m_vDropFieldPos;
	_float3				m_vNear4SitPos;
	_float3				m_vWeaponPos[2];
	_float4x4			m_matWeapon[2];
	_float4x4			m_matNearPattern3;
	wstring				m_WepaonTitle[2];
	
};

#endif // !__JPreto_H__