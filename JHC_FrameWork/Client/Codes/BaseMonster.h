#pragma once
#ifndef __BASEMONSTER_H__
#define __BASEMONSTER_H__

#include "GameObject.h"
#include "BaseWeapon.h"
#include "UIBase.h"
USING(Engine)

class CBaseMonster abstract : public CGameObject
{
protected:
	explicit CBaseMonster(_DEVICE _pDevice, const wstring& _PrototypeName = L"");
	explicit CBaseMonster(const CBaseMonster& _rOther);
	virtual ~CBaseMonster() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg);
	virtual _uint Update_GameObject(const _float & _fDeltaTime);
	//virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime);
	virtual HRESULT Render_GameObject();


public:
	void			Drop_Dead();
	void			Set_Cinematic_Start(_bool _bCineStart);
	void			Monster_Patrol();
	_bool			Get_UIRender() { return m_bNormalUIRender; };

protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);

private:
	void Set_Hit(_float _fDamage);
	void Look(const _float& _fDeltaTime);
	void BossLook(const _float & _fDeltaTime);
	void TargetCheck(const _float& _fDeltaTime);
	void CoolTimer(const _float& _fDeltaTime);
	void DeadParticle();
public:
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) PURE;
	virtual void Free() PURE;

protected:
	class CDynamicMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;
	class CTransform*			m_pTargetTransform;

	_float3						m_vResetPos;
	_float3						m_vLook;
	_float3						m_vRight;
	_float3						m_vOriginPos;
	_float3						m_vPatrolPos;
	_float3						m_vGravityPos;

	_float						m_fChangeSpeed;
	_float						m_fDist;
	_float						m_fMoveTime;
	_float						m_fDeathTime;
	_float						m_fAttackCoolTime;
	_float						m_fHitCoolTime;
	_float						m_fPlayerAtt;
	_float						m_fDissolveTime;
	_float						m_fDissolveSpeed;
	_float						m_fDeltaTime;
	_float						m_fMaxPatrolLength;
	_float						m_fNowPatrolLength;
	_float						m_fJumpTime;
	_float						m_fFarRange;
	_float						m_fDelayTime;
	_float						m_fTimeMultiple;

	_bool						m_bResetReady;
	_bool						m_bReset;
	_bool						m_bTarget;
	_bool						m_bLook;
	_bool						m_bHit;
	_bool						m_bHitAble;
	_bool						m_bAttackCoolTime;
	_bool						m_bCinematicStart;
	_bool						m_bCinematicEnd;
	_bool						m_bPlayerDead;
	_bool						m_bNormalUIRender;
	_bool						m_bUIRender;
	_bool						m_bAttackPattern;
	_bool						m_bFarRange;
	_bool						m_bPatrolStart;
	_bool						m_bPatrolEnd;
	_bool						m_bGravity;
	_bool						m_bGraviStart;
	_bool						m_bDropField;
	_bool						m_bBossLook;
	_bool						m_bBossCinematicAnimation;
	_bool						m_bChangeGravity;
	_bool						m_bReverseDissolve;
	_bool						m_bPlayerWeapon;
	_bool						m_bDelayStart;
	_bool						m_bDelayEnd;

	_uint						m_iShaderPass;
	_uint						m_iNearPattern;
	_uint						m_iFarPattern;
	_uint						m_iMovePattern;
	_uint						m_iGold;

	MyMonsterData*				m_pMonsterData;
	CBaseWeapon*				m_pWeapon;
	CUIBase::UI_DESC			m_tBackGroundDesc;
	CUIBase::UI_DESC			m_tHpGageDesc;
};

#endif // !__BASEMONSTER_H__
