#pragma once

#ifndef __JLEGIONNAIRE_H__
#define __JLEGIONNAIRE_H__

#include "JBaseMonster.h"

USING(Engine)

class CJLegionnaire final : public CJBaseMonster
{
private:
	enum class PATTERN
	{
		Near_1,		// 1�� ����
		Near_2,		// 1�� ���� + ���
		Near_3,		// 2�� ���� + ȸ��(��)
		Far_1,		// ���� ����
		Far_2,		// ���� ���� + ���
		Walk_F,		// ������ �ȱ�
		Walk_B,		// �ڷ� �ȱ�
		Walk_L,		// �������� �ȱ�
		Walk_R,		// ���������� �ȱ�
		Patrol,		// �����ϱ�
		Hit,		// ����
		Death,		// ����
		Parry,		// �и�
		Pattern_Ready,
		GunShoot,
		End
		//End
	};
	enum class BATTLEMODE
	{
		SWORD,
		GUN,
		End
	};
protected:
	explicit CJLegionnaire(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CJLegionnaire(const CJLegionnaire& _rOther);
	virtual ~CJLegionnaire() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

protected:
	virtual void Do_Idle(const _float & _fDeltaTime) override;
	virtual void Do_Attack(const _float & _fDeltaTime) override;
	virtual void Do_Hit(const _float & _fDeltaTime) override;
	virtual void Do_Walk(const _float & _fDeltaTime) override;
	virtual void Do_Other(const _float & _fDeltaTime) override;
	virtual void Get_Damaged()	override;

private:
	HRESULT	Weapon_Clone();
	HRESULT	UI_Clone();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect) override;
public:
	//_uint	Get_FrindCount() { return m_iFriendCount; }
private:
	//void Pattern_Update(const _float & _fDeltaTime);
	void			Attack_Pattern_1(const _float & _fDeltaTime);
	void			Attack_Pattern_2(const _float & _fDeltaTime);
	void			Attack_Pattern_3(const _float & _fDeltaTime);
	void			Attack_Pattern_4(const _float & _fDeltaTime);
	void			Attack_Pattern_5(const _float & _fDeltaTime);

	void			Walk_Pattern_B(const _float & _fDeltaTime);
	void			Walk_Pattern_F(const _float & _fDeltaTime);
	void			Walk_Pattern_L(const _float & _fDeltaTime);
	void			Walk_Pattern_R(const _float & _fDeltaTime);
	//�Ǹ��
	void			Shoot_Pattern(const _float & _fDeltaTime);
	//
	void			Parry_Pattern(const _float & _fDeltaTime);

	void			Hit_Pattern(const _float & _fDeltaTime);
	void			Death_Pattern(const _float & _fDeltaTime);
	void			Friends_Patrol_Ready();
	void			Patrol_Pattern(const _float & _fDeltaTime);
	void			Set_Hit();
	void			PatrolMaker();
	void			DistCheck();
	void			AttackCoolTimer(const _float & _fDeltaTime);

	void			Move_Sound();
public:
	static CJLegionnaire* Create(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	PATTERN							m_ePattern;
	BATTLEMODE						m_eBattleMode;
	_bool							m_bWalkPattern;

	_bool							m_bBulletReady;
	_bool							m_bBulletShoot;

	_bool				m_bGunNearStart;
	_bool				m_bGunNearAttacking;

	_bool				m_bAttackPattern2;
	_bool				m_bAttackPattern3;
	_bool				m_bAttackPattern4;
	_bool				m_bAttackPattern5;
	
	_uint				m_iAttackCount;
	_uint				m_iAttackCountEnd;

	_float				m_fGunNearAttacking;

	_float				m_fAttackPattern2;
	_float				m_fAttackPattern3;
	_float				m_fAttackPattern4;
	_float				m_fAttackPattern5;

};

#endif // !__JLEGIONNAIRE_H__