#pragma once

#ifndef __JMalkhel_H__
#define __JMalkhel_H__

#include "JBaseBoss.h"
#include "UI_BossMonster.h"

USING(Engine)

class CJMalkhel final : public CJBaseBoss
{
private:
	enum class PATTERN
	{
		Appear,
		Far_1,		// â���� �ٴ����. �� ���Ͽ����� �÷��̾� �������� �������� Į�� ���� ���� Į���� �߻�. ������� �ö����� ���� ����
		Far_2,		// ���ƿö� �������
		Far_3,		// ������ ��. ��ü 3�� ���� �� ������ ����
		Near_1,		// 120�� �۾���
		Near_2,		// ������� 4��Ÿ
		Near_3,		// 360�� �۾���
		Near_4,		// �¿�� 2��Ÿ
		Walk_F,		// ������ �ȱ�
		Walk_B,		// �ڷ� �ȱ�
		Walk_L,		// �������� �ȱ�
		Walk_R,		// ���������� �ȱ�
		BackTeleport,	// �ڷ���Ʈ
		FrontTeleport,	// �ڷ���Ʈ
		Hit,		// ����
		Death,		// ����
		Pattern_Ready,
		End
	};

protected:
	explicit CJMalkhel(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CJMalkhel(const CJMalkhel& _rOther);
	virtual ~CJMalkhel() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;


private:
	HRESULT			Weapon_Clone();	
	HRESULT			Skill_Clone();
	HRESULT			Effect_Clone();
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
	virtual void Delete_RigidBody() override;
private:
	void Appear_Pattern(const _float & _fDeltaTime);

	void Far_Pattern_1(const _float & _fDeltaTime);
	void Far_Pattern_2(const _float & _fDeltaTime);
	void Far_Pattern_3(const _float & _fDeltaTime);

	void Near_Pattern_1(const _float & _fDeltaTime);
	void Near_Pattern_2(const _float & _fDeltaTime);
	void Near_Pattern_3(const _float & _fDeltaTime);
	void Near_Pattern_4(const _float & _fDeltaTime);

	void Move_Pattern_B(const _float & _fDeltaTime);
	void Move_Pattern_F(const _float & _fDeltaTime);
	void Move_Pattern_L(const _float & _fDeltaTime);
	void Move_Pattern_R(const _float & _fDeltaTime);

	void BackMove_Pattern_Teleport(const _float & _fDeltaTime);

	void FrontMove_Pattern_Teleport(const _float & _fDeltaTime);

	void Death_Pattern(const _float & _fDeltaTime);

	void AttackEndToMove_ReadyIdle();

	//void CinematicTimeer(const _float & _fDeltaTime);

	void BackTeleportPos();
	void FrontTeleportPos();
	void Hit_Pattern(const _float & _fDeltaTime);
	//------------------------------------------------Trail---------------------------------//
	HRESULT Clone_Trail();
	void Fix_DistortionTrailPos(const _float & _fDeltaTime);
	//------------------------------------------------Trail---------------------------------//
	HRESULT Clone_Distortion();
public:
	virtual void Get_Damaged()	override;
private:
	void Move_Sound();
public:
	static CJMalkhel* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;	


private:
	PATTERN				m_ePattern;
	CUI_BossMonster*	m_pBossUI;
	CTrailEffect*		m_pTrailEffect;

	_uint				m_iAttackCount;
	_uint				m_iAttack;
	_uint				m_iCinemaEndCount;

	_bool				m_bPhase2On;
	_bool				m_bAttackSound2;
	_bool				m_bNear2Teleport;
	_bool				m_bFar3Teleport;

	_bool				m_bFarPattern1;
	_bool				m_bFarPattern2;
	_bool				m_bFarPattern3;
	_bool				m_bNearPattern2;
	_bool				m_bNearPattern3;

	_float				m_fTargetDistHalf;
	_float				m_fWalkFrontAccelTime;
	_float				m_fFarPattern1Time;
	_float				m_fFarPattern2Time;
	_float				m_fFarPattern3Time;
	_float				m_fNearPattern2Time;
	_float				m_fNearPattern3Time;
	_float				m_fNearPattern2TeleportTime;

	_float				m_fAppearDelayTime;

	_float3				m_vTeleportPos;

	_float3				m_vOriginRotate;

	CTrailEffect*		m_pArrWingDistort_TrailEffect[6];

	class CGameObject*	m_pFireBallObject[3];
	class CGameObject*	m_pFireBallBackObject;
	class CGameObject*	m_pSpearObject[12];
	class CGameObject*	m_pEffect360;

	

	_bool				m_bSkillStart;
	_bool				m_bFireBallSkillStart[3];
	_bool				m_bFireBallBackStart;
	_bool				m_bFireBallBackEnd;
	_bool				m_bEffect360Start;
	_bool				m_bEffect360End;

private:

	class CEffect_Texture*	m_pTeleportDistortionTex;
	_float					m_fDistortionTime;

	void DisortionScaleFix(const _float& _fDeltaTime);


private:
	void Near_Pattern_2_Effect();
	class CEffect_Texture_Ani* m_pLightEffect;

	HRESULT Clone_LightEffect();
	void LightCoolTime(const _float& _fDeltaTime);
	_float m_fLightTime;
};

#endif // !__JMalkhel_H__