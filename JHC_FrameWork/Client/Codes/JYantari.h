#pragma once

#ifndef __JYANTARI_H__
#define __JYANTARI_H__

#include "JBaseMonster.h"

USING(Engine)

class CJYantari final : public CJBaseMonster
{
private:
	enum class PATTERN
	{
		Near_1,		// ����
		Near_2,		// ��������ٰ� �ø���. �ø����� �ǰ�����
		Near_3,		// ��� �� ����
		Near_4,		// ��� �� ����
		Near_5,		// �ι� ����
		Far_1,		// ��������
		Far_2,		// ���� ���
		Far_3,		// ���� ����
		Walk_F,		// ������ �ȱ�
		Walk_B,		// �ڷ� �ȱ�
		Walk_L,		// �������� �ȱ�
		Walk_R,		// ���������� �ȱ�
		Patrol,		// �����ϱ�
		Hit,		// ����
		Parry,		// �и�
		Death,		// ����
		Pattern_Ready,
		End
	};

protected:
	explicit CJYantari(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CJYantari(const CJYantari& _rOther);
	virtual ~CJYantari() = default;

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
	HRESULT			Weapon_Clone();
	HRESULT			UI_Clone();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect) override;

private:
	void			Near_Pattern_1(const _float & _fDeltaTime);
	void			Near_Pattern_2(const _float & _fDeltaTime);
	void			Near_Pattern_3(const _float & _fDeltaTime);
	void			Near_Pattern_4(const _float & _fDeltaTime);
	void			Near_Pattern_5(const _float & _fDeltaTime);
	void			Far_Pattern_1(const _float & _fDeltaTime);
	void			Far_Pattern_2(const _float & _fDeltaTime);
	void			Cast_Pattern(const _float & _fDeltaTime);

	void			Walk_Pattern_F(const _float & _fDeltaTime);
	void			Walk_Pattern_B(const _float & _fDeltaTime);
	void			Walk_Pattern_L(const _float & _fDeltaTime);
	void			Walk_Pattern_R(const _float & _fDeltaTime);
	void			Hit_Pattern(const _float & _fDeltaTime);
	void			Death_Pattern(const _float & _fDeltaTime);
	void			Patrol_Pattern(const _float & _fDeltaTime);
	void			Parry_Pattern(const _float & _fDeltaTime);
	void			Set_Hit();
	void			PatrolMaker();
	void			Make_ChargeParticle();

	void			Cast_SkillCreate();
	void			Cast_SkillDelete();
	void			Cast_SkillUpdate(const _float & _fDeltaTime);
	void			Cast_SkillCoolTime(const _float & _fDeltaTime);
	HRESULT			Cast_SkillEffectCreate();
	void			AttackCoolTimer(const _float & _fDeltaTime);

	void			WalkSound();
	void			DashSound();

	void			TrailFixWidth();

public:
	static CJYantari* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	PATTERN							m_ePattern;
	_bool							m_bWalkPattern;
	_bool							m_bAttackSound;
	_bool							m_bDashSound;
	
	_uint							m_iDownAttack;
	_uint							m_iAttackCount;

	_bool							m_bCastSkill;
	_float							m_fCastSkillCoolTime;
	_float							m_fSkillRadius;
	wstring							m_wstrCastSkill;

	_bool							m_bNearPattern2;
	_bool							m_bNearPattern3;
	_bool							m_bNearPattern4;
	_bool							m_bNearPattern5;
	_bool							m_bFarPattern1;
	_bool							m_bFarPattern2;
	_bool							m_bCastPattern;

	_float							m_fNearPattern2Time;
	_float							m_fNearPattern3Time;
	_float							m_fNearPattern4Time;
	_float							m_fNearPattern5Time;
	_float							m_fFarPattern1Time;
	_float							m_fFarPattern2Time;
	_float							m_fCastPatternTime;

	_float							m_fCastPower;

	_bool							m_bPower;
	_float							m_fSkillY;
	_bool							m_bCreateEffect;
	_float3							m_vSkillPos;
	_float							m_fSkillEndY;
	class CEffect_Texture_Ani*		m_pSkillEffect;
	class CEffect_Texture*			m_pDistortTex;

#pragma endregion
};

#endif // !__YANTARI_H__