#pragma once

#ifndef __JHomonculus_H__
#define __JHomonculus_H__

#include "JBaseMonster.h"
USING(Engine)
class CJHomonculus final : public CJBaseMonster
{
private:
	enum class PATTERN
	{
		Near_1,		// 빠른 공격
		Near_2,		// 강한 공격
		Near_3,		// 공격 3연타
		Walk_F,		// 앞으로 걷기
		Walk_B,		// 뒤로 걷기
		Walk_L,		// 왼쪽으로 걷기
		Walk_R,		// 오른쪽으로 걷기
		Hit,		// 맞음
		Death,		// 죽음
		Pattern_Ready,
		End
	};

protected:
	explicit CJHomonculus(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CJHomonculus(const CJHomonculus& _rOther);
	virtual ~CJHomonculus() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

protected:
	virtual void Do_Activation(const _float& _fDeltaTime);

protected:
	virtual void Do_Idle(const _float & _fDeltaTime) override;
	virtual void Do_Attack(const _float & _fDeltaTime) override;
	virtual void Do_Hit(const _float & _fDeltaTime) override;
	virtual void Do_Walk(const _float & _fDeltaTime) override;
	virtual void Do_Other(const _float & _fDeltaTime) override;
	virtual void Get_Damaged()	override;
private:
	//HRESULT	Weapon_Clone();
	HRESULT	UI_Clone();

private:
		void			Attack_Pattern_1(const _float & _fDeltaTime);
		void			Attack_Pattern_2(const _float & _fDeltaTime);
		void			Attack_Pattern_3(const _float & _fDeltaTime);
		void			Walk_Pattern_F(const _float & _fDeltaTime);
		void			Walk_Pattern_L(const _float & _fDeltaTime);
		void			Walk_Pattern_R(const _float & _fDeltaTime);
		void			Hit_Pattern(const _float & _fDeltaTime);
		void			Death_Pattern(const _float & _fDeltaTime);
		void			WeaponCreate_RigidBody();
		void			WeaponDelete_RigidBody();
		void			Set_Attack(_bool _bLeft, _bool _bRight);
		void			Set_Homonculus_Weapon_Matrix();
		void			DiffuseChange();
		void			AttackSound();
		void			Move_Sound();
		void			EffectSound();
public:
	static CJHomonculus* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;

private:
	PATTERN				m_ePattern;
	_bool				m_bWalkPattern;
	_bool				m_bChangeDefuse;
	_bool				m_bEffect1;
	_bool				m_bEffect2;
	_float				m_fLeftHands;
	_float				m_fRightHands;

	// 피직스
	wstring				m_WeaponTitle[2];
	_float3				m_vWeaponPos[2];
	_float4x4			m_matWeapon[2];
};

#endif // !__CJHomonculus_H__